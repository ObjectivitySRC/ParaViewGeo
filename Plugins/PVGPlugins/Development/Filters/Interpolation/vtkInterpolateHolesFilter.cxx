/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkDistanceCellsToDrillhole.cxx $
  Author:    Arolde VIDJINNAGNI	    
  Date:      MIRARCO May 08, 2009 
  Version:   0.2
 =========================================================================*/
#include "vtkInterpolateHolesFilter.h"
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
#include "vtkDataArray.h"
#include "vtkPolyData.h"
#include <math.h>
#include <time.h>
#include "vtkstd/vector"
#include "vtkstd/list"
#include "vtkPoints.h"
#include "vtkStdString.h"
//time tracker define
//#define MYDEBUG
#define NOT_FOUND -1
#define XYZ  3
#define X  0
#define Y  1
#define Z  2

using namespace std;

vtkCxxRevisionMacro ( vtkInterpolateHolesFilter, "$Revision: 1.1 $" );
vtkStandardNewMacro ( vtkInterpolateHolesFilter );

//----------------------------------------------------------------------------
vtkInterpolateHolesFilter::vtkInterpolateHolesFilter()
{
   this->TypeOfInterpolation=0;
   this->Theta=0.996194698;
     
}
//----------------------------------------------------------------------------
vtkInterpolateHolesFilter::~vtkInterpolateHolesFilter()
{
}
//----------------------------------------------------------------------------
// Description:
// This is called by the superclass.
// This is the method you should override.
int vtkInterpolateHolesFilter::RequestInformation(
  vtkInformation *request, 
  vtkInformationVector **inputVector, 
  vtkInformationVector *outputVector)
{
  if(!this->Superclass::RequestInformation(request,inputVector,outputVector))
    {
    return 0;
    }
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  inInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

  vtkInformation *outInfo=outputVector->GetInformationObject(0);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
  return 1;
}

//----------------------------------------------------------------------------
int vtkInterpolateHolesFilter::RequestData( vtkInformation *vtkNotUsed ( request ), 
                                         vtkInformationVector **inputVector, 
                                         vtkInformationVector *outputVector )
{
   // get the info objects
   vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

   // get the input and ouptut
   vtkPolyData *input = vtkPolyData::SafeDownCast (
                       inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   vtkPolyData *output = vtkPolyData::SafeDownCast (
                        outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  this->ConstructSliceOutput ( input , output);
   
   return 1;
}
//----------------------------------------------------------------------------
void vtkInterpolateHolesFilter::ConstructSliceOutput ( vtkPolyData *input , vtkPolyData *output)
{  	
   vtkCellArray* lines = input->GetLines();
   vtkCellArray* Cells;
   vtkDataArray* PointCoords;  

	int numberOfPoints= input->GetPoints()->GetNumberOfPoints();
	int numberOfCells= 0;
	double* currentTuple;
	double current;
	if(lines->GetNumberOfCells() > 0)
	{
		Cells = input->GetLines();
    }
		
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
		

    vtkDataArray* cellId;
	vtkCellData* i1= input->GetCellData();
	cellId= i1->GetArray("Id");

	vtkFloatArray* newCellId;
	newCellId = vtkFloatArray::New();
	newCellId->Allocate(numberOfCells);
	((vtkFloatArray*) newCellId)->SetName("Id");


	vtkCellArray* listCells;//list of drillholes Cells
    listCells = vtkCellArray::New();

	vtkPoints* listPoints;//no removed points
	listPoints=vtkPoints::New();

	numberOfPoints= input->GetPoints()->GetNumberOfPoints();
  	float* myDepths=NULL;
	myDepths= new float[numberOfPoints];
	
	
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
			
		float distance=0;
		float a=0;
		for(int currentPoint = 0; currentPoint < *npts; currentPoint++)
		{

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
				lastPointsInside[currentCell][3]=myDepths[nbPts+ *npts -1]; //the depth of the first point					
			}
		}	

		nbPts+=*npts;	
	}


	int nbrePts;
	if(this->TypeOfInterpolation==0)//linear interpolation 
	{
		int compteur=0;	
		nbrePts=0;
		Cells->InitTraversal();	
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
		{	
						
			 //insertion of the second point of that cell
			 listPoints->InsertPoint(compteur,firstPointsInside[currentCell][0],firstPointsInside[currentCell][1],
												firstPointsInside[currentCell][2]);
			 compteur++;

			 //insertion of the second point of that cell
			 listPoints->InsertPoint(compteur,lastPointsInside[currentCell][0],lastPointsInside[currentCell][1],
												lastPointsInside[currentCell][2]);
			 compteur++;
			
			 listCells->InsertNextCell(2);
			 //current=NULL;	

			 current=cellId->GetTuple1(currentCell);				
			 newCellId->InsertNextTuple1(current); 
			 listCells->InsertCellPoint(compteur-2);
			 listCells->InsertCellPoint(compteur-1);
		}

	}	
	if(this->TypeOfInterpolation==1)//polylinear interpolation 
	{
		int compteur=0;	
		Cells->InitTraversal();	
		
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
		{	
			// Stores number of points in current cell
			vtkIdType *npts = new vtkIdType(); 
			// Create array of points in current cell.
			vtkIdType *pts; 

			Cells->GetNextCell(*npts, *&pts);
			double current;
			nbrePts=0;
			//insertion of the first point of that cell
			 listPoints->InsertPoint(compteur,firstPointsInside[currentCell][0],firstPointsInside[currentCell][1],
												firstPointsInside[currentCell][2]);
		    compteur++;
		    nbrePts++;
		    float a=0,b=0,c=0, cosAlpha=0;
			double pt1[3];
			pt1[0]= firstPointsInside[currentCell][0];
			pt1[1]= firstPointsInside[currentCell][1];
			pt1[2]= firstPointsInside[currentCell][2];
			double pt2[3];
			currentTuple = NULL;
			currentTuple = PointCoords->GetTuple(pts[1]);
			pt2[0]= currentTuple[0];
			pt2[1]= currentTuple[1];
			pt2[2]= currentTuple[2];

			bool newSegement=false;

			for(int currentPoint = 2; currentPoint < *npts-1; currentPoint++)
			{ 
				
				currentTuple = NULL;
				currentTuple = PointCoords->GetTuple(pts[currentPoint]);				

				//test of the angle if it is smoll i won't insert that point in the PolyLine
				a= sqrt( pow((pt2[0]-pt1[0]),2) 
				  + pow((pt2[1]-pt1[1]),2) 
				  + pow((pt2[2]-pt1[2]),2) );

				b= sqrt( pow((currentTuple[0]-pt2[0]),2)+ 
					     pow((currentTuple[1]-pt2[1]),2)+
						 pow((currentTuple[2]-pt2[2]),2) );

				c= sqrt( pow((currentTuple[0]-pt1[0]),2)+ 
				         pow((currentTuple[1]-pt1[1]),2)+
				         pow((currentTuple[2]-pt1[2]),2) );

				//angle relation to find cosAlpha
				cosAlpha= (a*a + b*b - c*c)/(2.0*a*b);

				if(abs(cosAlpha)< (this->Theta) )//cos(175)=-0,996194698
				{

					//new segment need to be generated
					//insertion of the second point of that cell
					 listPoints->InsertPoint(compteur,currentTuple[0],currentTuple[1],currentTuple[2]);
					 compteur++;
					 nbrePts++;

					 pt1[0]= pt2[0];
					 pt1[1]= pt2[1];
					 pt1[2]= pt2[2];

					 pt2[0]= currentTuple[0];
					 pt2[1]= currentTuple[1];
					 pt2[2]= currentTuple[2];
				}
				else 
				{
					//the point will be the new pt2
					pt2[0]= currentTuple[0];
					pt2[1]= currentTuple[1];
					pt2[2]= currentTuple[2];					
				}
			}	 

			 //insertion of the second point of that cell
			 listPoints->InsertPoint(compteur,lastPointsInside[currentCell][0],lastPointsInside[currentCell][1],
												lastPointsInside[currentCell][2]);
			 compteur++;
			 nbrePts++;

			 listCells->InsertNextCell(nbrePts);
			 //current=NULL;										
			 current=cellId->GetTuple1(currentCell);				
			 newCellId->InsertNextTuple1(current); 
			 for(int i=(compteur- nbrePts);i< compteur;i++)
			 listCells->InsertCellPoint(i);			 
		}

	}

	/*if(this->TypeOfInterpolation==2)//curvilinear interpolation  
	{
		int compteur=0;	
		Cells->InitTraversal();	
		
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
		{	
			// Stores number of points in current cell
			vtkIdType* npts = new vtkIdType(); 
			// Create array of points in current cell.
			vtkIdType *pts; 

			Cells->GetNextCell(*npts, *&pts);
			double* current;
			nbrePts=0;
			double tempPoint[3];		   		    
			double* currentTuple1;
			
		

			for(int currentPoint = 0; currentPoint < *npts-1; currentPoint++)
			{ 
				
				currentTuple = NULL;
				currentTuple = PointCoords->GetTuple(pts[currentPoint]);
				//insertion of the second point of that cell
				//listPoints->InsertPoint(compteur,currentTuple[0],currentTuple[1],currentTuple[2]);
				//compteur++;
				//nbrePts++;

				currentTuple1 = NULL;
				currentTuple1 = PointCoords->GetTuple(pts[currentPoint+1]);
				
				double nbPoints= (sqrt( pow((currentTuple1[0]-currentTuple[0]),2) 
						        + pow((currentTuple1[1]-currentTuple[1]),2) 
								+ pow((currentTuple1[2]-currentTuple[2]),2) ) )/this->curvilinearStep  -1;

				for(int i=0;i<nbPoints;i++)
				{ 
					tempPoint[0]= currentTuple[0] + ( (1- i*this->curvilinearStep)*(i*this->curvilinearStep) - (i*this->curvilinearStep) )*
													(currentTuple1[0] - currentTuple[0]);
					tempPoint[1]= currentTuple[1] + ( (1- i*this->curvilinearStep)*(i*this->curvilinearStep) - (i*this->curvilinearStep) )*
													(currentTuple1[1] - currentTuple[1]);
					tempPoint[2]= currentTuple[2] + ( (1- i*this->curvilinearStep)*(i*this->curvilinearStep) - (i*this->curvilinearStep) )*
													(currentTuple1[2] - currentTuple[2]);
					//insertion of the second point of that cell
					listPoints->InsertPoint(compteur,tempPoint[0],tempPoint[1],tempPoint[2]);
					compteur++;
					nbrePts++;				
				}
			}	 

			 //insertion of the second point of that cell
			// listPoints->InsertPoint(compteur,lastPointsInside[currentCell][0],lastPointsInside[currentCell][1],
			//									lastPointsInside[currentCell][2]);
			 //compteur++;
			// nbrePts++;

			 listCells->InsertNextCell(nbrePts);
			 current=NULL;										
			 current=cellId->GetTuple(currentCell);				
			 newCellId->InsertNextValue(current[0]); 
			 for(int i=(compteur- nbrePts);i< compteur;i++)
			 listCells->InsertCellPoint (i);			 
		}

	}*/

  output->SetPoints(listPoints);
  listPoints->Delete();
  
  output->SetLines(listCells);
  output->GetCellData()->AddArray(newCellId);
  listCells->Delete();
  newCellId->Delete();
  delete myDepths;
 
  //free up memory
 delete firstPointsInside;
 delete lastPointsInside;
 
} 
//----------------------------------------------------------------------------
int vtkInterpolateHolesFilter::FillInputPortInformation ( int port, vtkInformation* info ){
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
   return 1;
}

//----------------------------------------------------------------------------
void vtkInterpolateHolesFilter::PrintSelf ( ostream& os, vtkIndent indent )
{     
    this->Superclass::PrintSelf ( os, indent );
	os << indent << "Type of Interpolation " << this->TypeOfInterpolation << "\n";

}
