/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkSliceCompositing.cxx $
  Author:    Marie-Gabrielle Vallet
			 Arolde VIDJINNAGNI
	     
  Date:      JULY 24, 2008
  Version:   0.6

 =========================================================================*/
#include "vtkThreshold.h"
#include "vtkConnectivityFilter.h"
#include "vtkSliceCompositing.h"
#include "vtkCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkSmartPointer.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkDataArray.h"
#include "vtkPolyData.h"
#include <math.h>
#include <time.h>
#include "vtkstd/vector"
#include "vtkstd/list"
#include "vtkPoints.h"
//time tracker define
//#define MYDEBUG
#define NOT_FOUND -1
#define XYZ  3
#define X  0
#define Y  1
#define Z  2

using namespace std;

vtkCxxRevisionMacro ( vtkSliceCompositing, "$Revision: 0.1 $" );
vtkStandardNewMacro ( vtkSliceCompositing );

/***************************************************************************/
vtkSliceCompositing::vtkSliceCompositing()
{
   const int ports = 2;
	 this->AssaysID= NULL; 
	 this->StartDepth= NULL; 
	 this->EndDepth= NULL; 
   this->SetNumberOfInputPorts ( ports );
   this->TypeOfSampling=0;
   this->AdvancedSampling=0;
   this->TypeOfOutput=1;
   this->StepSyze=3;
}
//----------------------------------------------------------------------------
vtkSliceCompositing::~vtkSliceCompositing()
{
}
//----------------------------------------------------------------------------
void vtkSliceCompositing::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
{
   this->SetInputConnection ( 1, algOutput );
}

//----------------------------------------------------------------------------
vtkPolyData *vtkSliceCompositing::GetSource()
{
   if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
   return NULL;
   
   vtkDataObject  *src = this->GetExecutive()->GetInputData ( 0, 0 );
   vtkPolyData *source = vtkPolyData::SafeDownCast ( src );
   
   return source;
}
//----------------------------------------------------------------------------
// Description:
// This is called by the superclass.
// This is the method you should override.
int vtkSliceCompositing::RequestInformation(
  vtkInformation *request, 
  vtkInformationVector **inputVector, 
  vtkInformationVector *outputVector)
{
  if(!this->Superclass::RequestInformation(request,inputVector,outputVector))
    {
    return 0;
    }
  vtkInformation *inInfo = inputVector[1]->GetInformationObject ( 0 );
  inInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

  vtkInformation *sourceInfo = inputVector[0]->GetInformationObject ( 0 );
  sourceInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

  vtkInformation *outInfo=outputVector->GetInformationObject(0);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
  return 1;
}

/***************************************************************************/
int vtkSliceCompositing::RequestData( vtkInformation *vtkNotUsed ( request ), 
                                         vtkInformationVector **inputVector, 
                                         vtkInformationVector *outputVector )
{
   // get the info objects
   vtkInformation *inInfo = inputVector[1]->GetInformationObject ( 0 );
   vtkInformation *sourceInfo = inputVector[0]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

    // multi group source input
   vtkPolyData *source = vtkPolyData::SafeDownCast (
                         sourceInfo->Get ( vtkPolyData::DATA_OBJECT() ) );   
   // get the input and ouptut
   vtkPolyData *input = vtkPolyData::SafeDownCast (
                       inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   vtkPolyData *output = vtkPolyData::SafeDownCast (
                        outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

    if ( !source ){
      return 0;
   }
   vtkCellArray* cells = source->GetPolys();
   if( cells->GetMaxCellSize() != 3 )
      cout << "Other polys than triangles are found in the surface" << endl;
  
  this->ConstructSliceOutput ( input , output);
   
   return 1;
} 
/**************************MIRARCO*****************************************
* Function: 					          	                              *
* Description: this function construct the output by the sampling         *
*    of the drillholes	according to the options specify by the user      *	
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
void vtkSliceCompositing::ConstructSliceOutput ( vtkPolyData *input , vtkPolyData *output)
{  	
   vtkPolyData *source = this->GetSource();	
   vtkCellArray* lines = input->GetLines();
	 vtkCellArray* polys = input->GetPolys();
   vtkCellArray* Cells;
   vtkDataArray* PointCoords;  

	int numberOfPoints= input->GetPoints()->GetNumberOfPoints();
	int numberOfCells= 0;
	double* currentTuple;
	if(lines->GetNumberOfCells() > 0)
			Cells = input->GetLines();
  else if(polys->GetNumberOfCells() > 0)
			Cells = input->GetPolys();
	
		
	numberOfCells= Cells->GetNumberOfCells();	

	PointCoords = input->GetPoints()->GetData();

	//list of inside points for each drillHole 
	double**  firstPointsInside;//list of first point of each initial cell
	firstPointsInside= new double*[numberOfCells];
	double**  lastPointsInside;//list of last point of each initial cell
	lastPointsInside= new double*[numberOfCells];
	for(int i=0; i<numberOfCells;i++){
		firstPointsInside[i]= new double[4];//x,y,z,depth
		lastPointsInside[i]= new double[4];//x,y,z,depth
	}
	
	vtkDataArray* listAssayId;
	vtkCellData* p10= source->GetCellData();
	listAssayId= p10->GetArray(this->AssaysID);

	vtkDataArray* liststartDepht;
	vtkCellData* p2= source->GetCellData();
	liststartDepht= p2->GetArray(this->StartDepth);

	vtkDataArray* listendDepth;
	vtkCellData* p3= source->GetCellData();
	listendDepth= p3->GetArray(this->EndDepth);

	vtkDataArray* cellId;
	vtkCellData* i1= input->GetCellData();
	cellId= i1->GetArray("Id");

	vtkCellArray* linesSource = source->GetLines();
    vtkCellArray* CellsSource;

    int numberOfCellsSource=0;
    if(linesSource->GetNumberOfCells() > 0)
	CellsSource = source->GetLines();     		
	numberOfCellsSource= CellsSource->GetNumberOfCells();


	//save the id of drillHoles and add it for each cells of the drillHole
	vtkDataArray* oldPointDepth;
	//oldPointDepth->New();
	vtkPointData* p= input->GetPointData();
	oldPointDepth= p->GetArray("PointDepth");//we have already the deph of points 
	float* myDepths=NULL;
	if( oldPointDepth==NULL )
	{
		myDepths= new float[numberOfPoints];
	}
	int nbPts=0;
	if( oldPointDepth==NULL )//we don't have the depth on the input property in that case
	{	
		Cells->InitTraversal();	
		int nbPts=0;
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
		{					
			// Stores number of points in current cell
			vtkIdType* npts = new vtkIdType(); 
			// Create array of points in current cell.
			vtkIdType *pts; 

			Cells->GetNextCell(*npts, *&pts);
			//list of inside points for each drillHole 
			double**  PointsInside;
			PointsInside= new double*[*npts];
			for(int i=0; i<*npts;i++)
			PointsInside[i]= new double[3];		
			double* current;
			if(AdvancedSampling==0)//start the sampling from the first point
			{
				float distance=0;
				float a=0;
				for(int currentPoint = 0; currentPoint < *npts; currentPoint++){

					currentTuple = NULL;
					currentTuple = PointCoords->GetTuple(pts[currentPoint]);
					PointsInside[currentPoint][0]=currentTuple[0];
					PointsInside[currentPoint][1]=currentTuple[1];
					PointsInside[currentPoint][2]=currentTuple[2];

					//calcul the depth of the point and add it the table
					if(currentPoint == 0){
						myDepths[nbPts]=0;					
					}
					else{
						distance= sqrt( pow((PointsInside[currentPoint][0]-PointsInside[currentPoint-1][0]),2) 
						  + pow((PointsInside[currentPoint][1]-PointsInside[currentPoint-1][1]),2) 
						  + pow((PointsInside[currentPoint][2]-PointsInside[currentPoint-1][2]),2) );

						myDepths[nbPts+currentPoint]= myDepths[(nbPts+currentPoint)-1]+distance;										
					}
					
					if(currentPoint == 0){
						firstPointsInside[currentCell][0]=currentTuple[0];
						firstPointsInside[currentCell][1]=currentTuple[1];
						firstPointsInside[currentCell][2]=currentTuple[2];
						firstPointsInside[currentCell][3]=myDepths[nbPts]; //the depth of the first point					
					}
					else if(currentPoint == (*npts -1) ){
						lastPointsInside[currentCell][0]=currentTuple[0];
						lastPointsInside[currentCell][1]=currentTuple[1];
						lastPointsInside[currentCell][2]=currentTuple[2];
						lastPointsInside[currentCell][3]=myDepths[nbPts+ *npts -1]; //the depth of the last point					
					}
				}
			}
			else// make the start sampling from the first assay position 
			{
				float distance=0;
				float a=0;
				for(int currentPoint = 0; currentPoint < *npts; currentPoint++){

					currentTuple = NULL;
					currentTuple = PointCoords->GetTuple(pts[currentPoint]);
					PointsInside[currentPoint][0]=currentTuple[0];
					PointsInside[currentPoint][1]=currentTuple[1];
					PointsInside[currentPoint][2]=currentTuple[2];

					//calculate the depth of the point and add it the table
					if(currentPoint == 0){
						myDepths[nbPts]=0;					
					}
					else{
						distance= sqrt( pow((PointsInside[currentPoint][0]-PointsInside[currentPoint-1][0]),2) 
						  + pow((PointsInside[currentPoint][1]-PointsInside[currentPoint-1][1]),2) 
						  + pow((PointsInside[currentPoint][2]-PointsInside[currentPoint-1][2]),2) );

						myDepths[nbPts+currentPoint]= myDepths[(nbPts+currentPoint)-1]+distance;										
					}
				}
				current= NULL;									
				current=cellId->GetTuple(currentCell);
				int id_cell= current[0]; 

			    int position=0;
				bool trouve=false;
				float assayId=0;
				//I figure out the position of the first assay of each drillHole
				while( (trouve==false)&&(position< numberOfCellsSource) )
				{
					current= NULL;									
					current=listAssayId->GetTuple(position);
					assayId= current[0]; 	

					if(id_cell==assayId)
						trouve=true;			
					else position++;
				}
				if(trouve==false){
					firstPointsInside[currentCell][0]=0;
					firstPointsInside[currentCell][1]=0;
					firstPointsInside[currentCell][2]=0;
					firstPointsInside[currentCell][3]=0; 

					lastPointsInside[currentCell][0]=0;
					lastPointsInside[currentCell][1]=0;
					lastPointsInside[currentCell][2]=0;
					lastPointsInside[currentCell][3]=0; 
				}
				else 
				{		
					bool endId=false;
					float assayStartDepth=0;
					float firstDepthSampling=0;
					bool trouve1=false;
					int cpt=0;
														
					current= NULL;									
					current=liststartDepht->GetTuple(position);
					firstDepthSampling= current[0];
					position++;

					while((endId==false)&&(position<numberOfCellsSource) )
					{
						current= NULL;									
						current=liststartDepht->GetTuple(position);
						assayStartDepth= current[0]; 

						if(assayStartDepth<firstDepthSampling)
							firstDepthSampling=assayStartDepth;

						current= NULL;									
						current=listAssayId->GetTuple(position+1);
						assayId= current[0];

						if(id_cell!=assayId)
						endId=true;

						position++;						
					}
					
					while( (trouve1==false)&&(cpt < *npts) ) 
					{											
						if(myDepths[nbPts+cpt]>firstDepthSampling)
						{
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[cpt]);	

							//I need to find the point of the drillhole at the same depth with the first assay position
							if(cpt!=0)
							{								
								int nIter=0;
								double pt1[4];
								double pt2[4];
								double milieu[4];								
						
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[cpt-1]);	
								pt1[0]= currentTuple[0];
								pt1[1]= currentTuple[1];
								pt1[2]= currentTuple[2];
								pt1[3]= myDepths[nbPts+cpt-1];
								
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[cpt]);	
								pt2[0]= currentTuple[0];
								pt2[1]= currentTuple[1];
								pt2[2]= currentTuple[2];
								pt2[3]= myDepths[nbPts+cpt];
								
								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
								milieu[3]= pt1[3]+(pt2[3] - pt1[3])/2;
								
								
								while(nIter<20){								

									if(milieu[3]>firstDepthSampling)
									{
										pt2[0]= milieu[0];
										pt2[1]= milieu[1];
										pt2[2]= milieu[2];
										pt2[3]= milieu[3];										
										
										milieu[3]= pt1[3]+(pt2[3] - pt1[3])/2;
										milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
										milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
										milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

									}
									else if(milieu[3]< firstDepthSampling)
									{
										pt1[0]= milieu[0];
										pt1[1]= milieu[1];
										pt1[2]= milieu[2];
										pt1[3]= milieu[3];
										
										milieu[3]= pt1[3]+(pt2[3] - pt1[3])/2;
										milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
										milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
										milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
									}
									else nIter=20;
									nIter++;

								}
								firstPointsInside[currentCell][0]=milieu[0];
								firstPointsInside[currentCell][1]=milieu[1];
								firstPointsInside[currentCell][2]=milieu[2];
								firstPointsInside[currentCell][3]=milieu[3];
								trouve1= true;
							}
							else{
								firstPointsInside[currentCell][0]=currentTuple[0];
								firstPointsInside[currentCell][1]=currentTuple[1];
								firstPointsInside[currentCell][2]=currentTuple[2];
								firstPointsInside[currentCell][3]=myDepths[nbPts+cpt];
								trouve1= true;		
							}
						}
						else if (myDepths[nbPts+cpt]==firstDepthSampling){
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[cpt]);	

							firstPointsInside[currentCell][0]=currentTuple[0];
							firstPointsInside[currentCell][1]=currentTuple[1];
							firstPointsInside[currentCell][2]=currentTuple[2];
							firstPointsInside[currentCell][3]=myDepths[nbPts+cpt];
							trouve1= true;		
						}
						else cpt++;
					}

					currentTuple = NULL;
					currentTuple = PointCoords->GetTuple(pts[*npts-1]);	
												
					lastPointsInside[currentCell][0]=currentTuple[0];
					lastPointsInside[currentCell][1]=currentTuple[1];
					lastPointsInside[currentCell][2]=currentTuple[2];
					lastPointsInside[currentCell][3]=myDepths[nbPts+ *npts-1];
				
			      }
			}

			nbPts+=*npts;
		}
	}
	else //we have already the depth of the points on the input property in that case
	{
		int compteur1=0;
		Cells->InitTraversal();	
		int nbPts=0;
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
		{		 
			// Stores number of points in current cell
			vtkIdType* npts = new vtkIdType(); 
			// Create array of points in current cell.
			vtkIdType *pts; 

			Cells->GetNextCell(*npts, *&pts);	
			double* current;

			
			if(AdvancedSampling==0)
			{
				
				for(int currentPoint = 0; currentPoint < *npts; currentPoint++){
					currentTuple = NULL;
					currentTuple = PointCoords->GetTuple(pts[currentPoint]);			
					
					if(currentPoint == 0){
						firstPointsInside[currentCell][0]=currentTuple[0];
						firstPointsInside[currentCell][1]=currentTuple[1];
						firstPointsInside[currentCell][2]=currentTuple[2];
						
						current=NULL;										
						current=oldPointDepth->GetTuple(compteur1);
						firstPointsInside[currentCell][3]=current[0]; //the depth of the first point
						
					}
					else if(currentPoint == (*npts -1) ){
						lastPointsInside[currentCell][0]=currentTuple[0];
						lastPointsInside[currentCell][1]=currentTuple[1];
						lastPointsInside[currentCell][2]=currentTuple[2];
						
						current=NULL;										
						current=oldPointDepth->GetTuple(compteur1);
						lastPointsInside[currentCell][3]=current[0]; //the depth of the last point					
					}
					compteur1++;
				}

			}
			else// make the start sampling on the first assay position 
			{
				current= NULL;									
				current=cellId->GetTuple(currentCell);
				int id_cell= current[0]; 

			    int position=0;
				bool trouve=false;
				float assayId=0;
				while( (trouve==false)&&(position< numberOfCellsSource) )
				{
					current= NULL;									
					current=listAssayId->GetTuple(position);
					assayId= current[0]; 

					if(id_cell==assayId)
						trouve=true;			
					else position++;
				}
				if(trouve==false){
					firstPointsInside[currentCell][0]=0;
					firstPointsInside[currentCell][1]=0;
					firstPointsInside[currentCell][2]=0;
					firstPointsInside[currentCell][3]=0; 

					lastPointsInside[currentCell][0]=0;
					lastPointsInside[currentCell][1]=0;
					lastPointsInside[currentCell][2]=0;
					lastPointsInside[currentCell][3]=0; 
				}
				else 
				{		
					bool endId=false;
					float assayStartDepth=0.0;
					float firstDepthSampling=0.0;
					bool trouve1=false;
					int cpt=0;
					float temp=0;
														
					current= NULL;									
					current=liststartDepht->GetTuple(position);
					firstDepthSampling= current[0];
					position++;

					while((endId==false)&&(position<numberOfCellsSource))
					{
						current= NULL;									
						current=liststartDepht->GetTuple(position);
						assayStartDepth= current[0]; 

						if(assayStartDepth<firstDepthSampling)
							firstDepthSampling=assayStartDepth;

						current= NULL;									
						current=listAssayId->GetTuple(position+1);
						assayId= current[0]; 

						if(id_cell!=assayId)
						endId=true;

						position++;
					}
					

					while( (trouve1==false)&&(cpt< *npts) ) //fisrt point for the sampling
					{
						current=NULL;
						temp=nbPts+cpt;
						current=oldPointDepth->GetTuple(temp);
						temp=current[0];
						
						if(temp>firstDepthSampling)
						{
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[cpt]);

							if(cpt!=0)
							{
								int nIter=0;
								double pt1[4];
								double pt2[4];
								double milieu[4];								
						
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[cpt-1]);	
								pt1[0]= currentTuple[0];
								pt1[1]= currentTuple[1];
								pt1[2]= currentTuple[2];
								current=oldPointDepth->GetTuple(nbPts+cpt-1);
								pt1[3]= current[0];
								
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[cpt]);	
								pt2[0]= currentTuple[0];
								pt2[1]= currentTuple[1];
								pt2[2]= currentTuple[2];
								current=oldPointDepth->GetTuple(nbPts+cpt);
								pt2[3]=  current[0];
								
								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
								milieu[3]= pt1[3]+(pt2[3] - pt1[3])/2;								
								
								while(nIter<20){								

									if(milieu[3]>firstDepthSampling)
									{
										pt2[0]= milieu[0];
										pt2[1]= milieu[1];
										pt2[2]= milieu[2];
										pt2[3]= milieu[3];

										milieu[3]= pt1[3]+(pt2[3] - pt1[3])/2;	
										milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
										milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
										milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

									}
									else if(milieu[3]< firstDepthSampling)
									{
										pt1[0]= milieu[0];
										pt1[1]= milieu[1];
										pt1[2]= milieu[2];
										pt1[3]= milieu[3];

										milieu[3]= pt1[3]+(pt2[3] - pt1[3])/2;
										milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
										milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
										milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
									}
									else nIter=20;
									nIter++;

								}
								firstPointsInside[currentCell][0]=milieu[0];
								firstPointsInside[currentCell][1]=milieu[1];
								firstPointsInside[currentCell][2]=milieu[2];
								firstPointsInside[currentCell][3]=milieu[3];
								trouve1= true;		
							}
							else{
								firstPointsInside[currentCell][0]=currentTuple[0];
								firstPointsInside[currentCell][1]=currentTuple[1];
								firstPointsInside[currentCell][2]=currentTuple[2];
								firstPointsInside[currentCell][3]=temp;
								trouve1= true;		
							}
						}
						else if(temp==firstDepthSampling){
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[cpt]);
							firstPointsInside[currentCell][0]=currentTuple[0];
							firstPointsInside[currentCell][1]=currentTuple[1];
							firstPointsInside[currentCell][2]=currentTuple[2];
							firstPointsInside[currentCell][3]=temp;
							trouve1= true;		
						}
						else cpt++;
					}

					currentTuple = NULL;
					currentTuple = PointCoords->GetTuple(pts[*npts-1]);	
												
					lastPointsInside[currentCell][0]=currentTuple[0];
					lastPointsInside[currentCell][1]=currentTuple[1];
					lastPointsInside[currentCell][2]=currentTuple[2];

					current=NULL;	
					temp=nbPts+ *npts-1; 
					current=oldPointDepth->GetTuple(temp);
					lastPointsInside[currentCell][3]=current[0];
				
			      }
			}

			nbPts+= *npts;			
		}	
	}
	//find the number of new cells and points to make 
	int nbCellToMake=0;
	int nbPtsTotal=0;
	for(int currentCell = 0; currentCell < numberOfCells; currentCell++){
		int nb=0;

		if( (this->TypeOfSampling==0))//downHoleCompositing in this case
		{
			nb= int( (lastPointsInside[currentCell][3]- firstPointsInside[currentCell][3] )/this->StepSyze);
			if( (nb*this->StepSyze)==(lastPointsInside[currentCell][3]- firstPointsInside[currentCell][3]) ){
				nbCellToMake+= nb;
				nbPtsTotal+= nb+1;
			}
			else{
				nbCellToMake+= nb+1;
				nbPtsTotal+= nb+2;
			}
		}
		else if( (this->TypeOfSampling==1)&& (lastPointsInside[currentCell][2]!=firstPointsInside[currentCell][2]) )//benchHoleCompositing in this case
		{
			//that sampling will follow the elevation so in our data it will be z coordonnate
			if(lastPointsInside[currentCell][2]>firstPointsInside[currentCell][2])
			{
				nb= int( (lastPointsInside[currentCell][2]- firstPointsInside[currentCell][2] )/this->StepSyze);
				if( (nb*this->StepSyze)==(lastPointsInside[currentCell][3]- firstPointsInside[currentCell][3]) ){
					nbCellToMake+= nb;
					nbPtsTotal+= nb+1;
				}
				else{
					nbCellToMake+= nb+1;
					nbPtsTotal+= nb+2;
				}
			}
			else if(lastPointsInside[currentCell][2]<firstPointsInside[currentCell][2])
			{
				nb= int( (firstPointsInside[currentCell][2] - lastPointsInside[currentCell][2])/this->StepSyze);
				if( (nb*this->StepSyze)==(firstPointsInside[currentCell][2] - lastPointsInside[currentCell][2]) ){
					nbCellToMake+= nb;
					nbPtsTotal+= nb+1;
				}
				else{
					nbCellToMake+= nb+1;
					nbPtsTotal+= nb+2;
				}
			}
		}
	}	

	//save the id of drillHoles and add it for each cells of the drillHole
	vtkDataArray* oldLinesInsideId;
	vtkCellData* p1= input->GetCellData();
	oldLinesInsideId= p1->GetArray("Id");

	//each cell have two points points and have the id of the original drillHole
	vtkFloatArray* newCellId;
	newCellId = vtkFloatArray::New();
	if(this->TypeOfOutput)
	newCellId->Allocate(nbCellToMake);
	else newCellId->Allocate(nbCellToMake+1);
	((vtkFloatArray*) newCellId)->SetName("Id");
	
	vtkCellArray* listNewCells;
    listNewCells = vtkCellArray::New();
	if(this->TypeOfOutput)
    listNewCells->Allocate(nbCellToMake);
	else listNewCells->Allocate(nbCellToMake+1);
	
    vtkFloatArray* newPtsInsideDepth;
	newPtsInsideDepth = vtkFloatArray::New();
	((vtkFloatArray*)newPtsInsideDepth)->SetName("PointDepth");

	vtkPoints* listNewPoints;//inside points
	listNewPoints=vtkPoints::New();
	
	int compteur=0;	
	int newstart=0;
	int nbrePts=0;
	vtkCellArray* Cells1;
	Cells1=Cells;
	Cells1->InitTraversal();
	for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
	{	
		newstart=0;
		int nb=0;
		int nbCell=0;
	    int nbPts=0;
		bool isMultiple=false;
		if( (this->TypeOfSampling==0) )//downHoleCompositing in this case
		{
			nb= int( (lastPointsInside[currentCell][3]- firstPointsInside[currentCell][3] )/this->StepSyze);
			if( (nb*this->StepSyze)==(lastPointsInside[currentCell][3]- firstPointsInside[currentCell][3]) ){
				nbCell+= nb;
				nbPts+= nb+1;
				isMultiple=true;
			}
			else{
				nbCell+= nb+1;
				nbPts+= nb+2;
				isMultiple=false;
			}
		}
		else if( (this->TypeOfSampling==1)&& (lastPointsInside[currentCell][2]!=firstPointsInside[currentCell][2]) )//benchHoleCompositing in this case
		{
			if(lastPointsInside[currentCell][2]>firstPointsInside[currentCell][2])
			{
				nb= int( (lastPointsInside[currentCell][2]- firstPointsInside[currentCell][2] )/this->StepSyze);
				if( (nb*this->StepSyze)==(lastPointsInside[currentCell][3]- firstPointsInside[currentCell][3]) ){
					nbCellToMake+= nb;
					nbPtsTotal+= nb+1;
				}
				else{
					nbCellToMake+= nb+1;
					nbPtsTotal+= nb+2;
				}
			}
			else if(lastPointsInside[currentCell][2]<firstPointsInside[currentCell][2])
			{
				nb= int( (firstPointsInside[currentCell][2] - lastPointsInside[currentCell][2])/this->StepSyze);
				if( (nb*this->StepSyze)==(firstPointsInside[currentCell][2] - lastPointsInside[currentCell][2]) ){
					nbCell+= nb;
					nbPts+= nb+1;
					isMultiple=true;
				}
				else{
					nbCell+= nb+1;
					nbPts+= nb+2;
					isMultiple=false;
				}
			}
			
		}
		int j=0, projectStepSize=0;
		double firstPoint[3];
		double secondPoint[3];
		double tempPoint1[3];
		double tempPoint2[3];
		float depth1=0,depth2=0;
		float alpha=0;		
	
			
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType(); 
		// Create array of points in current cell.
		vtkIdType *pts; 
		
		Cells1->GetNextCell(*npts, *&pts);	
		double* current;
		int currentPoint=0;
		bool isFound=false;
		double tempPoint[4];

		while(j<nbCell)
		{
			/***********************downHoleCompositing  in this case *****************************/
			if( (this->TypeOfSampling==0) )
			{
				int n=0;
				if(j==0)
				{
					if(nbCell==1)
					{
						firstPoint[0]=firstPointsInside[currentCell][0];
						firstPoint[1]=firstPointsInside[currentCell][1];
						firstPoint[2]=firstPointsInside[currentCell][2];
						depth1= firstPointsInside[currentCell][3];

						secondPoint[0]=lastPointsInside[currentCell][0];
						secondPoint[1]=lastPointsInside[currentCell][1];
						secondPoint[2]=lastPointsInside[currentCell][2];
						depth2= lastPointsInside[currentCell][3];
					}
					else
					{
						firstPoint[0]=firstPointsInside[currentCell][0];
						firstPoint[1]=firstPointsInside[currentCell][1];
						firstPoint[2]=firstPointsInside[currentCell][2];
						depth1= firstPointsInside[currentCell][3];


						isFound=false;

						currentPoint = 0; 
						float currentDepth=0;
						while( (isFound==false) && (currentPoint < *npts) )
						{
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							if( oldPointDepth==NULL )
							currentDepth= myDepths[nbrePts+currentPoint];
							else
							{
								current=NULL;
								n=nbrePts+currentPoint;
								current=oldPointDepth->GetTuple(n);
								currentDepth=current[0];							
							}
							if( currentDepth >= (depth1 + this->StepSyze) )
							{
								tempPoint1[0]= currentTuple[0];
								tempPoint1[1]= currentTuple[1];
								tempPoint1[2]= currentTuple[2];

								isFound=true;
							}
							else currentPoint++;
						}	
						
						if( oldPointDepth==NULL )
						{ 
							if(myDepths[nbrePts+currentPoint] >= firstPointsInside[currentCell][3])
							{
								alpha= this->StepSyze /(myDepths[nbrePts+currentPoint] - firstPointsInside[currentCell][3]);							
								secondPoint[0]=(1-alpha)* firstPoint[0]+ alpha* tempPoint1[0];
								secondPoint[1]=(1-alpha)* firstPoint[1]+ alpha* tempPoint1[1];
								secondPoint[2]=(1-alpha)*firstPoint[2]+  alpha* tempPoint1[2];
								depth2= firstPointsInside[currentCell][3]+ this->StepSyze;
							}
							else if(myDepths[nbrePts+currentPoint] < firstPointsInside[currentCell][3])
							{ 
								alpha= this->StepSyze /(firstPointsInside[currentCell][3] - myDepths[nbrePts+currentPoint]);
								secondPoint[0]=(1-alpha)*tempPoint1[0] + alpha*firstPoint[0] ;
								secondPoint[1]=(1-alpha)*tempPoint1[1] + alpha*firstPoint[1] ;
								secondPoint[2]=(1-alpha)*tempPoint1[2]+  alpha*firstPoint[2] ;
								depth2= firstPointsInside[currentCell][3]+ this->StepSyze;
							}
						}					
						else
						{
							current=NULL;										
							n=nbrePts+currentPoint;
							current=oldPointDepth->GetTuple(n);

							if(current[0] >= firstPointsInside[currentCell][3])
							{
								alpha= this->StepSyze /(current[0]- firstPointsInside[currentCell][3]);
								secondPoint[0]=(1-alpha)* firstPoint[0]+ alpha* tempPoint1[0];
								secondPoint[1]=(1-alpha)* firstPoint[1]+ alpha* tempPoint1[1];
								secondPoint[2]=(1-alpha)*firstPoint[2]+  alpha* tempPoint1[2];
								depth2= firstPointsInside[currentCell][3]+ this->StepSyze;
							}
							else if(current[0] < firstPointsInside[currentCell][3])
							{
								alpha= this->StepSyze /(firstPointsInside[currentCell][3] - current[0]);
								secondPoint[0]=(1-alpha)*tempPoint1[0] + alpha*firstPoint[0] ;
								secondPoint[1]=(1-alpha)*tempPoint1[1] + alpha*firstPoint[1] ;
								secondPoint[2]=(1-alpha)*tempPoint1[2]+  alpha*firstPoint[2] ;
								depth2= firstPointsInside[currentCell][3]+ this->StepSyze;							
							}
						}
					}
				}
				else if(j==nbCell-1)
				{
					depth1= firstPointsInside[currentCell][3]+(j)*this->StepSyze;

					isFound=false;
					currentPoint = 0; 
					float currentDepth=0;
					while(isFound==false && (currentPoint < *npts) )
					{
						currentTuple = NULL;
						currentTuple = PointCoords->GetTuple(pts[currentPoint]);
						if( oldPointDepth==NULL )
						currentDepth= myDepths[nbrePts+currentPoint];
						else
						{
							current=NULL;										
							n=nbrePts+currentPoint;
							current=oldPointDepth->GetTuple(n);
							currentDepth=current[0];							
						}
						if( currentDepth >= depth1 )
						{
							currentPoint--;
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							tempPoint1[0]= currentTuple[0];
							tempPoint1[1]= currentTuple[1];
							tempPoint1[2]= currentTuple[2];

							isFound=true;
						}
						else currentPoint++;
					}
					
					if( oldPointDepth==NULL ){
						alpha= (lastPointsInside[currentCell][3] - myDepths[nbrePts+currentPoint]) 
							- (lastPointsInside[currentCell][3] - depth1);

						alpha= alpha /( lastPointsInside[currentCell][3] - myDepths[nbrePts+currentPoint] );
					 }
					else
					{
						current=NULL;										
						n=nbrePts+currentPoint;
						current=oldPointDepth->GetTuple(n);

						alpha= (lastPointsInside[currentCell][3] - current[0]) 
							- (lastPointsInside[currentCell][3] - depth1);
						alpha= alpha /( lastPointsInside[currentCell][3] - current[0] );
				    }
					firstPoint[0]=(1-alpha)* tempPoint1[0]+ alpha* lastPointsInside[currentCell][0];
					firstPoint[1]=(1-alpha)* tempPoint1[1]+ alpha* lastPointsInside[currentCell][1];
					firstPoint[2]=(1-alpha)* tempPoint1[2]+ alpha* lastPointsInside[currentCell][2];
					
				
					secondPoint[0]=lastPointsInside[currentCell][0];
					secondPoint[1]=lastPointsInside[currentCell][1];
					secondPoint[2]=lastPointsInside[currentCell][2];
					depth2= lastPointsInside[currentCell][3];				
				}
				else
				{   
					depth1= firstPointsInside[currentCell][3]+(j)*this->StepSyze;

					isFound=false;
					currentPoint = 0; 
					float currentDepth=0;
					while(isFound==false && (currentPoint < *npts) )
					{
						currentTuple = NULL;
						currentTuple = PointCoords->GetTuple(pts[currentPoint]);
						if( oldPointDepth==NULL )
						currentDepth= myDepths[nbrePts+currentPoint];
						else
						{
							current=NULL;										
							n=nbrePts+currentPoint;
							current=oldPointDepth->GetTuple(n);
							currentDepth=current[0];							
						}
						if( currentDepth >= depth1 )
						{
							tempPoint2[0]= currentTuple[0];
							tempPoint2[1]= currentTuple[1];
							tempPoint2[2]= currentTuple[2];

							currentPoint--;
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							tempPoint1[0]= currentTuple[0];
							tempPoint1[1]= currentTuple[1];
							tempPoint1[2]= currentTuple[2];

							isFound=true;
						}
						else currentPoint++;
					}

					if( oldPointDepth==NULL ){
						alpha= ( myDepths[nbrePts+currentPoint+1] - myDepths[nbrePts+currentPoint]) 
							- (myDepths[nbrePts+currentPoint+1] - depth1);

						alpha= alpha /( myDepths[nbrePts+currentPoint+1] - myDepths[nbrePts+currentPoint] );
					 }
					else
					{
						current=NULL;										
						n=nbrePts+currentPoint;
						current=oldPointDepth->GetTuple(n);
						float d1= current[0];

						current=NULL;										
						n=nbrePts+currentPoint+1;
						current=oldPointDepth->GetTuple(n);
						float d2= current[0];

						alpha= (d2 - d1) - (d2 - depth1);
						alpha= alpha /( d2 - d1 );
				    }					
					firstPoint[0]=(1-alpha)* tempPoint1[0]+ alpha* tempPoint2[0];
					firstPoint[1]=(1-alpha)* tempPoint1[1]+ alpha* tempPoint2[1];
					firstPoint[2]=(1-alpha)* tempPoint1[2]+ alpha* tempPoint2[2];
					
					depth2= firstPointsInside[currentCell][3]+(j+1)*this->StepSyze;
					isFound=false;
					currentPoint = 0; 
					currentDepth=0;
					while(isFound==false && (currentPoint < *npts) )
					{
						currentTuple = NULL;
						currentTuple = PointCoords->GetTuple(pts[currentPoint]);
						if( oldPointDepth==NULL )
						currentDepth= myDepths[nbrePts+currentPoint];
						else
						{
							current=NULL;										
							n=nbrePts+currentPoint;
							current=oldPointDepth->GetTuple(n);
							currentDepth=current[0];							
						}
						if( currentDepth >= depth2 )
						{
							tempPoint2[0]= currentTuple[0];
							tempPoint2[1]= currentTuple[1];
							tempPoint2[2]= currentTuple[2];

							currentPoint--;
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							tempPoint1[0]= currentTuple[0];
							tempPoint1[1]= currentTuple[1];
							tempPoint1[2]= currentTuple[2];

							isFound=true;
						}
						else currentPoint++;
					}

					if( oldPointDepth==NULL ){
						alpha= ( myDepths[nbrePts+currentPoint+1] - myDepths[nbrePts+currentPoint]) 
							- (myDepths[nbrePts+currentPoint+1] - depth2);

						alpha= alpha /( myDepths[nbrePts+currentPoint+1] - myDepths[nbrePts+currentPoint] );
					 }
					else
					{
						current=NULL;										
						n=nbrePts+currentPoint;
						current=oldPointDepth->GetTuple(n);
						float d1= current[0];

						current=NULL;										
						n=nbrePts+currentPoint+1;
						current=oldPointDepth->GetTuple(n);
						float d2= current[0];

						alpha= (d2 - d1) - (d2 - depth2);
						alpha= alpha /( d2 - d1 );
				    }					
					secondPoint[0]=(1-alpha)* tempPoint1[0]+ alpha* tempPoint2[0];
					secondPoint[1]=(1-alpha)* tempPoint1[1]+ alpha* tempPoint2[1];
					secondPoint[2]=(1-alpha)* tempPoint1[2]+ alpha* tempPoint2[2];
					
				}
				if(newstart==0)//this is used to insert just one time the first point
				{				//is a way to not duplicate the point
					 listNewPoints->InsertPoint(compteur,firstPoint[0],firstPoint[1],firstPoint[2]);
					 //add the depht of the last inside point
					 newPtsInsideDepth->InsertNextValue(depth1); 					 
					 double* current;										
					 current=oldLinesInsideId->GetTuple(currentCell);
					 if(!this->TypeOfOutput){
						 listNewCells->InsertNextCell(1);
						 newCellId->InsertNextValue(current[0]); 
						 listNewCells->InsertCellPoint (compteur);
					 }	
					 newstart=1;
					 compteur++;

				}

				 //insertion of the second point of that cell
				 listNewPoints->InsertPoint(compteur,secondPoint[0],secondPoint[1],secondPoint[2]);
				 //add the depht of the last inside point
				 newPtsInsideDepth->InsertNextValue(depth2);				
				 double* current;										
			     current=oldLinesInsideId->GetTuple(currentCell);
				 if(!this->TypeOfOutput){
					 listNewCells->InsertNextCell(1);
					 newCellId->InsertNextValue(current[0]); 	
					 listNewCells->InsertCellPoint (compteur);
				 }	
				 compteur++;

				 if(this->TypeOfOutput){
				 listNewCells->InsertNextCell(2);
				 double* current;										
				 current=oldLinesInsideId->GetTuple(currentCell);				
				 newCellId->InsertNextValue(current[0]); 
				 listNewCells->InsertCellPoint (compteur-2);
				 listNewCells->InsertCellPoint (compteur-1);
				 
				 }	

				 j++;
			}
		    
		    /***********************BenchHolesCompositing in this case *****************************/
			else if( (this->TypeOfSampling==1) && (lastPointsInside[currentCell][2]!=firstPointsInside[currentCell][2] ) )
			{
				int n=0;
				if(j==0)
				{
					if(nbCell==1)
					{
						firstPoint[0]=firstPointsInside[currentCell][0];
						firstPoint[1]=firstPointsInside[currentCell][1];
						firstPoint[2]=firstPointsInside[currentCell][2];
						depth1= firstPointsInside[currentCell][3];

						secondPoint[0]=lastPointsInside[currentCell][0];
						secondPoint[1]=lastPointsInside[currentCell][1];
						secondPoint[2]=lastPointsInside[currentCell][2];
						depth2= lastPointsInside[currentCell][3];
					}
					else
					{
						firstPoint[0]=firstPointsInside[currentCell][0];
						firstPoint[1]=firstPointsInside[currentCell][1];
						firstPoint[2]=firstPointsInside[currentCell][2];
						depth1= firstPointsInside[currentCell][2];

						
						isFound=false;

						currentPoint = 0; 
						float currentDepth=0;

						if(firstPointsInside[currentCell][2]> lastPointsInside[currentCell][2])
						{
							while( (isFound==false) && (currentPoint < *npts) )
							{
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[currentPoint]);

								if( currentTuple[2] < (depth1 - this->StepSyze) )
								{
									tempPoint1[0]= currentTuple[0];
									tempPoint1[1]= currentTuple[1];
									tempPoint1[2]= currentTuple[2];
								
									int nIter=0;
									double pt1[3];
									double pt2[3];
									double milieu[3];
							
									pt1[0]= firstPointsInside[currentCell][0];
									pt1[1]= firstPointsInside[currentCell][1];
									pt1[2]= firstPointsInside[currentCell][2];

									pt2[0]= tempPoint1[0];
									pt2[1]= tempPoint1[1];
									pt2[2]= tempPoint1[2];
									milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
									milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
									milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

									while(nIter<20){								

										if(milieu[2]<(depth1 - this->StepSyze))
										{
											pt2[0]= milieu[0];
											pt2[1]= milieu[1];
											pt2[2]= milieu[2];

											milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
											milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
											milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

										}
										else if(milieu[2]> (depth1 - this->StepSyze))
										{
											pt1[0]= milieu[0];
											pt1[1]= milieu[1];
											pt1[2]= milieu[2];

											milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
											milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
											milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
										}	
										else nIter=20;
										nIter++;
									}
									secondPoint[0]=milieu[0];
									secondPoint[1]=milieu[1];
									secondPoint[2]=milieu[2];
									isFound=true;
								}
								else if(currentTuple[2] == (depth1 - this->StepSyze)){
									secondPoint[0]= currentTuple[0];
									secondPoint[1]= currentTuple[1];
									secondPoint[2]= currentTuple[2];
									isFound=true;
								}
								else currentPoint++;
							}
													
							depth1= firstPointsInside[currentCell][3];
							depth2= depth1+ sqrt( pow((secondPoint[0]-firstPoint[0]),2)
								+ pow((secondPoint[1]-firstPoint[1]),2) + pow((secondPoint[2]-firstPoint[2]),2)) ;
							tempPoint[0]= secondPoint[0];
							tempPoint[1]= secondPoint[1];
							tempPoint[2]= secondPoint[2];
							tempPoint[3]= depth2;						
						}
						else if(firstPointsInside[currentCell][2] < lastPointsInside[currentCell][2])
						{
							while( (isFound==false) && (currentPoint < *npts) )
							{
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[currentPoint]);

								if( currentTuple[2] > (depth1 + this->StepSyze) )
								{
									tempPoint1[0]= currentTuple[0];
									tempPoint1[1]= currentTuple[1];
									tempPoint1[2]= currentTuple[2];

									int nIter=0;
									double* pt1;
									double* pt2;
									double milieu[3];
							
									pt1= firstPoint;
									pt2= tempPoint1;
									milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
									milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
									milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

									while(nIter<20){								

										if(milieu[2]>(depth1 + this->StepSyze))
										{
											pt2[0]= milieu[0];
											pt2[1]= milieu[1];
											pt2[2]= milieu[2];

											milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
											milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
											milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

										}
										else if(milieu[2]< (depth1 + this->StepSyze))
										{
											pt1[0]= milieu[0];
											pt1[1]= milieu[1];
											pt1[2]= milieu[2];

											milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
											milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
											milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
										}	
										else nIter=20;
										nIter++;

									}
									secondPoint[0]=milieu[0];
									secondPoint[1]=milieu[1];
									secondPoint[2]=milieu[2];
									isFound=true;
								}
								else if(currentTuple[2] == (depth1 + this->StepSyze)){
									secondPoint[0]= currentTuple[0];
									secondPoint[1]= currentTuple[1];
									secondPoint[2]= currentTuple[2];
									isFound=true;
								}
								else currentPoint++;
							}
							
							depth1= firstPointsInside[currentCell][3];
							depth2= depth1+ sqrt( pow((secondPoint[0]-firstPoint[0]),2)
								+ pow((secondPoint[1]-firstPoint[1]),2) + pow((secondPoint[2]-firstPoint[2]),2)) ;
							tempPoint[0]= secondPoint[0];
							tempPoint[1]= secondPoint[1];
							tempPoint[2]= secondPoint[2];
							tempPoint[3]= depth2;					
						}
					}

				}
				else if(j==nbCell-1)
				{
					isFound=false;
					currentPoint = 0; 
					float currentDepth=0;
					if(firstPointsInside[currentCell][2]> lastPointsInside[currentCell][2])
					{	
						depth1= firstPointsInside[currentCell][2]-(j)*this->StepSyze;
						while(isFound==false && (currentPoint < *npts) )
						{
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							
							if( currentTuple[2] < depth1 )
							{
								currentPoint--;
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[currentPoint]);
								tempPoint1[0]= currentTuple[0];
								tempPoint1[1]= currentTuple[1];
								tempPoint1[2]= currentTuple[2];

								isFound=true;
							}
							else currentPoint++;
						}

						int nIter=0;
						double* pt1;
						double* pt2;
						double milieu[3];
				
						pt1= tempPoint1;
						pt2= lastPointsInside[currentCell];
						milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
						milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
						milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
						
						while(nIter<20){								

							if(milieu[2]< depth1)
							{
								pt2[0]= milieu[0];
								pt2[1]= milieu[1];
								pt2[2]= milieu[2];
								
								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

							}
							else if(milieu[2]> depth1)
							{
								pt1[0]= milieu[0];
								pt1[1]= milieu[1];
								pt1[2]= milieu[2];

								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
							}								
							nIter++;
						}
						firstPoint[0]=milieu[0];
						firstPoint[1]=milieu[1];
						firstPoint[2]=milieu[2];
						depth1= tempPoint[3];						
								
						secondPoint[0]=lastPointsInside[currentCell][0];
						secondPoint[1]=lastPointsInside[currentCell][1];
						secondPoint[2]=lastPointsInside[currentCell][2];
						depth2= lastPointsInside[currentCell][3];
					}
					else if(firstPointsInside[currentCell][2] < lastPointsInside[currentCell][2])
					{	
						depth1= firstPointsInside[currentCell][2]+(j)*this->StepSyze;
						while(isFound==false && (currentPoint < *npts) )
						{
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							
							if( currentTuple[2] < depth1 )
							{
								currentPoint--;
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[currentPoint]);
								tempPoint1[0]= currentTuple[0];
								tempPoint1[1]= currentTuple[1];
								tempPoint1[2]= currentTuple[2];

								isFound=true;
							}
							else currentPoint++;
						}				
						
						
						int nIter=0;
						double* pt1;
						double* pt2;
						double milieu[3];
				
						pt1= tempPoint1;
						pt2= lastPointsInside[currentCell];
						milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
						milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
						milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
						
						while(nIter<20){								

							if(milieu[2]> depth1)
							{
								pt2[0]= milieu[0];
								pt2[1]= milieu[1];
								pt2[2]= milieu[2];
								
								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

							}
							else if(milieu[2]< depth1)
							{
								pt1[0]= milieu[0];
								pt1[1]= milieu[1];
								pt1[2]= milieu[2];

								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
							}								
							nIter++;

						}
						firstPoint[0]=milieu[0];
						firstPoint[1]=milieu[1];
						firstPoint[2]=milieu[2];						
						depth1= tempPoint[3];

						secondPoint[0]=lastPointsInside[currentCell][0];
						secondPoint[1]=lastPointsInside[currentCell][1];
						secondPoint[2]=lastPointsInside[currentCell][2];
						depth2= lastPointsInside[currentCell][3];
					}
				}
				else
				{   
					isFound=false;
					currentPoint = 0; 
					float currentDepth=0;
						
					if(firstPointsInside[currentCell][2]> lastPointsInside[currentCell][2])
					{	
						depth1= firstPointsInside[currentCell][2]-(j)*this->StepSyze;						
						while(isFound==false && (currentPoint < *npts) )
						{
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							
							if( currentTuple[2] < depth1 )
							{
								currentPoint--;
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[currentPoint]);
								tempPoint1[0]= currentTuple[0];
								tempPoint1[1]= currentTuple[1];
								tempPoint1[2]= currentTuple[2];

								isFound=true;
							}
							else currentPoint++;
						}
						
						currentTuple = NULL;
						currentTuple = PointCoords->GetTuple(pts[currentPoint+1]);
					
						int nIter=0;
						double* pt1;
						double* pt2;
						double milieu[3];
				
						pt1= tempPoint1;
						pt2= currentTuple;
						milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
						milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
						milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
						
						while(nIter<20){								

							if(milieu[2]< depth1)
							{
								pt2[0]= milieu[0];
								pt2[1]= milieu[1];
								pt2[2]= milieu[2];
								
								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

							}
							else if(milieu[2]> depth1)
							{
								pt1[0]= milieu[0];
								pt1[1]= milieu[1];
								pt1[2]= milieu[2];

								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
							}								
							nIter++;

						}
						firstPoint[0]=milieu[0];
						firstPoint[1]=milieu[1];
						firstPoint[2]=milieu[2];
						depth1= tempPoint[3];							
						
					}
					else if(firstPointsInside[currentCell][2] < lastPointsInside[currentCell][2])
					{	
						depth1= firstPointsInside[currentCell][2]+(j)*this->StepSyze;						
						while(isFound==false && (currentPoint < *npts) )
						{
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							
							if( currentTuple[2] > depth1 )
							{
								currentPoint--;
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[currentPoint]);
								tempPoint1[0]= currentTuple[0];
								tempPoint1[1]= currentTuple[1];
								tempPoint1[2]= currentTuple[2];

								isFound=true;
							}
							else currentPoint++;
						}
						
						currentTuple = NULL;
						currentTuple = PointCoords->GetTuple(pts[currentPoint+1]);
						
						
						int nIter=0;
						double* pt1;
						double* pt2;
						double milieu[3];
				
						pt1= tempPoint1;
						pt2= currentTuple;
						milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
						milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
						milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
						
						while(nIter<20){								

							if(milieu[2]> depth1)
							{
								pt2[0]= milieu[0];
								pt2[1]= milieu[1];
								pt2[2]= milieu[2];
								
								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

							}
							else if(milieu[2]< depth1)
							{
								pt1[0]= milieu[0];
								pt1[1]= milieu[1];
								pt1[2]= milieu[2];

								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
							}								
							nIter++;

						}
						firstPoint[0]=milieu[0];
						firstPoint[1]=milieu[1];
						firstPoint[2]=milieu[2];
						depth1= tempPoint[3];									
						
					}
					
					//find the second point of this new cell
					isFound=false;
					currentPoint = 0; 
					if(firstPointsInside[currentCell][2]> lastPointsInside[currentCell][2])
					{	
						depth2= firstPointsInside[currentCell][2]-(j+1)*this->StepSyze;						
						while(isFound==false && (currentPoint < *npts) )
						{
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							
							if( currentTuple[2] < depth2 )
							{
								currentPoint--;
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[currentPoint]);
								tempPoint2[0]= currentTuple[0];
								tempPoint2[1]= currentTuple[1];
								tempPoint2[2]= currentTuple[2];

								isFound=true;
							}
							else currentPoint++;
						}
						
						currentTuple = NULL;
						currentTuple = PointCoords->GetTuple(pts[currentPoint+1]);
						
						
						int nIter=0;
						double* pt1;
						double* pt2;
						double milieu[3];
				
						pt1= tempPoint2;
						pt2= currentTuple;
						milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
						milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
						milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
						
						while(nIter<20){								

							if(milieu[2]< depth2)
							{
								pt2[0]= milieu[0];
								pt2[1]= milieu[1];
								pt2[2]= milieu[2];
								
								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

							}
							else if(milieu[2]> depth2)
							{
								pt1[0]= milieu[0];
								pt1[1]= milieu[1];
								pt1[2]= milieu[2];

								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
							}								
							nIter++;

						}
						secondPoint[0]=milieu[0];
						secondPoint[1]=milieu[1];
						secondPoint[2]=milieu[2];

						depth2= tempPoint[3]+ sqrt( pow((secondPoint[0]-tempPoint[0]),2)
							+ pow((secondPoint[1]-tempPoint[1]),2) + pow((secondPoint[2]-tempPoint[2]),2)) ;
						tempPoint[0]= secondPoint[0];
					    tempPoint[1]= secondPoint[1];
					    tempPoint[2]= secondPoint[2];
					    tempPoint[3]= depth2;					
					}
					else if(firstPointsInside[currentCell][2] < lastPointsInside[currentCell][2])
					{	
						depth2= firstPointsInside[currentCell][2]+(j+1)*this->StepSyze;							
						while(isFound==false && (currentPoint < *npts) )
						{
							currentTuple = NULL;
							currentTuple = PointCoords->GetTuple(pts[currentPoint]);
							
							if( currentTuple[2] > depth2 )
							{
								currentPoint--;
								currentTuple = NULL;
								currentTuple = PointCoords->GetTuple(pts[currentPoint]);
								tempPoint2[0]= currentTuple[0];
								tempPoint2[1]= currentTuple[1];
								tempPoint2[2]= currentTuple[2];

								isFound=true;
							}
							else currentPoint++;
						}
						
						currentTuple = NULL;
						currentTuple = PointCoords->GetTuple(pts[currentPoint+1]);
						
						int nIter=0;
						double* pt1;
						double* pt2;
						double milieu[3];
				
						pt1= tempPoint2;
						pt2= currentTuple;
						milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
						milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
						milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
						
						while(nIter<20){								

							if(milieu[2]> depth2)
							{
								pt2[0]= milieu[0];
								pt2[1]= milieu[1];
								pt2[2]= milieu[2];
								
								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];

							}
							else if(milieu[2]< depth2)
							{
								pt1[0]= milieu[0];
								pt1[1]= milieu[1];
								pt1[2]= milieu[2];

								milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
								milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
								milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];
							}								
							nIter++;

						}
						secondPoint[0]=milieu[0];
						secondPoint[1]=milieu[1];
						secondPoint[2]=milieu[2];

						depth2= tempPoint[3]+ sqrt( pow((secondPoint[0]-tempPoint[0]),2)
							+ pow((secondPoint[1]-tempPoint[1]),2) + pow((secondPoint[2]-tempPoint[2]),2)) ;
						tempPoint[0]= secondPoint[0];
					    tempPoint[1]= secondPoint[1];
					    tempPoint[2]= secondPoint[2];
					    tempPoint[3]= depth2;					
					}	
				}		
				if(newstart==0) //this is used to insert just one time the first point
				{				//is a way to not duplicate the point
					 listNewPoints->InsertPoint(compteur,firstPoint[0],firstPoint[1],firstPoint[2]);
					 //add the depht of the last inside point
					 newPtsInsideDepth->InsertNextValue(depth1); 
					 compteur++;
					 newstart=1;
					 double* current;										
					 current=oldLinesInsideId->GetTuple(currentCell);
					 if(!this->TypeOfOutput){
						 listNewCells->InsertNextCell(1);
						 newCellId->InsertNextValue(current[0]); //add the HoleId	
						 listNewCells->InsertCellPoint (compteur-1);
					 }	

				}

				 /*i need to change the insertion of the second pooint here because i duplicate the point*/

				 //insertion of the second point of that cell
				 listNewPoints->InsertPoint(compteur,secondPoint[0],secondPoint[1],secondPoint[2]);
				 //add the depht of the last inside point
				 newPtsInsideDepth->InsertNextValue(depth2); 
				 compteur++;
				 double* current;										
				 current=oldLinesInsideId->GetTuple(currentCell);
				 if(!this->TypeOfOutput){
					 listNewCells->InsertNextCell(1);
					 newCellId->InsertNextValue(current[0]); //add the HoleId	
					 listNewCells->InsertCellPoint (compteur-1);
				 }	
				 //insert the cell
				 if(this->TypeOfOutput){
				 listNewCells->InsertNextCell(2);
				 double* current;										
				 current=oldLinesInsideId->GetTuple(currentCell);				
				 newCellId->InsertNextValue(current[0]); //add the HoleId	
				 listNewCells->InsertCellPoint (compteur-2);
				 listNewCells->InsertCellPoint (compteur-1);
				 }	
				 j++;
			}			
		}

		nbrePts+= *npts;					
  }
  if(this->TypeOfOutput) { //in this case the sampling result is shown by lines
	  output->SetPoints(listNewPoints);
	  output->GetPointData()->AddArray(newPtsInsideDepth);	
	  output->SetLines(listNewCells);
	  output->GetCellData()->AddArray(newCellId);
	 
	  }
  else { //in this case the sampling result is shown by points
	  output->SetPoints(listNewPoints);
	  output->GetPointData()->AddArray(newPtsInsideDepth);	
	  output->SetVerts(listNewCells);
	  output->GetCellData()->AddArray(newCellId);	 
  }

  //free up memory
 delete firstPointsInside;
 delete lastPointsInside;
 oldLinesInsideId->Delete();
 
 listNewPoints->Delete();
 listNewCells->Delete();
 //oldPointDepth->Delete();
} 
/***************************************************************************/
int vtkSliceCompositing::FillInputPortInformation ( int port, vtkInformation* info ){
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
   return 1;
}

/***************************************************************************/
void vtkSliceCompositing::PrintSelf ( ostream& os, vtkIndent indent )
{     
  vtkPolyData *source = this->GetSource();
  this->Superclass::PrintSelf ( os, indent );
  os << indent << "Source: " << source << "\n";
  os << indent << "TypeOfSampling: " << this->TypeOfSampling << "\n";
  os << indent << "AdvancedSampling: " << this->AdvancedSampling << "\n";
  os << indent << "TypeOfOutput: " << this->TypeOfOutput << "\n";
  os << indent << "StepSyze: " << this->StepSyze << "\n";

}
