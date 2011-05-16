/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkAdvancedCompositing.cxx $
  Author:    Arolde VIDJINNAGNI
	     
  Date:      JULY 24, 2008
  Version:   0.6
 =========================================================================*/
#include "vtkThreshold.h"
#include "vtkConnectivityFilter.h"
#include "vtkAdvancedCompositing.h"
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

vtkCxxRevisionMacro ( vtkAdvancedCompositing, "$Revision: 0.1 $" );
vtkStandardNewMacro ( vtkAdvancedCompositing );

/***************************************************************************/
vtkAdvancedCompositing::vtkAdvancedCompositing()
{
   const int ports = 2;
   this->SetNumberOfInputPorts ( ports );

	 this->Property= NULL; 
	 this->AssaysID= NULL; 
	 this->StartDepth= NULL; 
	 this->EndDepth= NULL; 

   this->TypeOfCompositing=0;
   this->TypeOfSampling=0;
   this->AdvancedSampling=0;
   this->TypeOfOutput=1;
   this->StepSyze=3;
}
//----------------------------------------------------------------------------
vtkAdvancedCompositing::~vtkAdvancedCompositing()
{
}
//----------------------------------------------------------------------------
void vtkAdvancedCompositing::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
{
   this->SetInputConnection ( 1, algOutput );
}

//----------------------------------------------------------------------------
vtkPolyData *vtkAdvancedCompositing::GetSource()
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
int vtkAdvancedCompositing::RequestInformation(
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
int vtkAdvancedCompositing::RequestData( vtkInformation *vtkNotUsed ( request ), 
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
   
  vtkDataArray* listAugpt;
  vtkCellData* p4= source->GetCellData();
	listAugpt= p4->GetArray(this->Property);

  if (!listAugpt)
  {
	  vtkErrorMacro(<<"No property was selected");
      return 1;
  }

   vtkSliceCompositing *connect = vtkSliceCompositing::New();
   connect->SetInput(0,source);
   connect->SetInput(1,input);

   connect->SetInputArrayToProcess(0, 0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS, listAugpt->GetName());
   connect->SetTypeOfOutput(this->TypeOfOutput);
   connect->SetTypeOfSampling(this->TypeOfSampling);
   connect->SetStepSyze(this->StepSyze);
   connect->SetAdvancedSampling(this->AdvancedSampling);
   connect->Update();
   
   vtkPolyData* input1= vtkPolyData::New();
   input1->CopyStructure(connect->GetOutput());
   input1->GetPointData()->PassData ( connect->GetOutput()->GetPointData() );
   input1->GetCellData()->PassData ( connect->GetOutput()->GetCellData() );
   connect->Delete();

  // Second step is taging inside points
  this->ConstructOutput( listAugpt, input1, output );
   
   return 1;
}
//
/**************************MIRARCO*****************************************
* Function: 		ConstructOutput       	                              *
* Description:	we construt the output by this function after sampling    *
*			   first with sliceCompositing filter we make the compositing *														  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
void vtkAdvancedCompositing::ConstructOutput (vtkDataArray* listAugpt, vtkPolyData *input1,
                                               vtkPolyData *output )
{  	
   vtkPolyData *source = this->GetSource();	
   vtkCellArray* linesInput = input1->GetLines();
   vtkCellArray* CellsInput;
   vtkDataArray* PointCoordsInput;
   double* currentTuple;
   int numberOfCellsInput=0;
   vtkIdType *nodesa;  

   
    if(linesInput->GetNumberOfCells() > 0)
	CellsInput = input1->GetLines();
    		
	numberOfCellsInput= CellsInput->GetNumberOfCells();
	CellsInput->InitTraversal();
	PointCoordsInput = input1->GetPoints()->GetData();

	int numberOfPointsInput= input1->GetPoints()->GetNumberOfPoints();

    vtkDataArray* cellId;
	vtkCellData* i1= input1->GetCellData();
	cellId= i1->GetArray("Id");

	vtkDataArray* pointDepth;
	vtkPointData* i2= input1->GetPointData();
	pointDepth= i2->GetArray("PointDepth");

	vtkCellArray* linesSource = source->GetLines();
    vtkCellArray* CellsSource;
    int numberOfCellsSource=0;

    if(linesSource->GetNumberOfCells() > 0)
	CellsSource = source->GetLines();
     		
	numberOfCellsSource= CellsSource->GetNumberOfCells();

	vtkDataArray* listAssayId;
	vtkCellData* p1= source->GetCellData();
	listAssayId= p1->GetArray(this->AssaysID);

	vtkDataArray* liststartDepht;
	vtkCellData* p2= source->GetCellData();
	liststartDepht= p2->GetArray(this->StartDepth);

	vtkDataArray* listendDepth;
	vtkCellData* p3= source->GetCellData();
	listendDepth= p3->GetArray(this->EndDepth);

	vtkFloatArray* outcellId;
	outcellId = vtkFloatArray::New();
	outcellId->Allocate(numberOfCellsInput);
	((vtkFloatArray*)outcellId)->SetName("Id");

	vtkFloatArray* outPointsDepth;
	outPointsDepth = vtkFloatArray::New();
	outPointsDepth->Allocate(numberOfPointsInput);
	((vtkFloatArray*)outPointsDepth)->SetName("PointDepth");

	vtkFloatArray* outcellaugpt;
	outcellaugpt = vtkFloatArray::New();
	outcellaugpt->Allocate(numberOfCellsInput);
	((vtkFloatArray*)outcellaugpt)->SetName(listAugpt->GetName()/*"augpt"*/);

	vtkFloatArray* outcellUseful;
	outcellUseful = vtkFloatArray::New();
	outcellUseful->Allocate(numberOfCellsInput);
	((vtkFloatArray*)outcellUseful)->SetName("useful");

	vtkCellArray* outCells;// copy for output
    outCells = vtkCellArray::New();
    outCells->Allocate(numberOfCellsInput);

	vtkPoints* listNewPoints;//list of points
	listNewPoints=vtkPoints::New();
	listNewPoints->Allocate(numberOfPointsInput);

	vtkPoints* listMiddlePoints;//list of middle points
	listMiddlePoints=vtkPoints::New();
	listMiddlePoints->Allocate(numberOfCellsInput);

	vtkFloatArray* middleDepth;
	middleDepth = vtkFloatArray::New();
	middleDepth->Allocate(numberOfCellsInput);
	((vtkFloatArray*)middleDepth)->SetName("Depth");

	vtkCellArray* cellsMiddle;
	cellsMiddle = vtkCellArray::New();
    cellsMiddle->Allocate(numberOfCellsInput);

	vtkFloatArray* middleAugpt;
	middleAugpt = vtkFloatArray::New();
	middleAugpt->Allocate(numberOfCellsInput);
	((vtkFloatArray*)middleAugpt)->SetName(listAugpt->GetName() /*"augpt"*/);
			 
	int compteur=0,compteur1=0;	
	float useful=0;//useful segment of each cell
	float id_cell=0;
	float cellDepth1=0;
	float cellDepth2=0;
	float assayStartDepth=0;
	float assayEndDepth=0;
	float augpt=0;
	double point1[3];//first point of the cell
	double point2[3];//second point of the cell
	double middle[3];//middle point of the cell
	float midDepth;//the depth of the middle point of the cell

	double* current;
	for(int currentCell = 0; currentCell < numberOfCellsInput; currentCell++)
	{		 
		useful=0;//useful segment of each cell
		id_cell=0;
		cellDepth1=0;//start deph of the cell
		cellDepth2=0;//end deph of the cell
		assayStartDepth=0;//start deph of the assey
		assayEndDepth=0;//end deph of the assey
		augpt=0;
		point1[0]=0; point1[1]=0; point1[2]= 0;
		point2[0]= 0; point2[1]= 0 ;point2[2]= 0;
		middle[0]= 0; middle[1]= 0; middle[2]= 0;
		midDepth=0;

		current= NULL;									
		current=cellId->GetTuple(currentCell);
		id_cell= current[0]; //add the HoleId	of the cell

		current= NULL;									
		current=pointDepth->GetTuple(2*currentCell);
		cellDepth1= current[0];

		current= NULL;									
		current=pointDepth->GetTuple(2*currentCell+1);
		cellDepth2= current[0];
		
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType(); 
		// Create array of points in current cell.
		vtkIdType *pts; 
		CellsInput->GetNextCell(*npts, *&pts);

		double**  PointsInside;
		PointsInside= new double*[*npts];
		for(int i=0; i<*npts;i++)
		PointsInside[i]= new double[3];
		for(int currentPoint = 0; currentPoint < *npts; currentPoint++){
			currentTuple = NULL;
			currentTuple = PointCoordsInput->GetTuple(pts[currentPoint]);
			PointsInside[currentPoint][0]=currentTuple[0];
			PointsInside[currentPoint][1]=currentTuple[1];
			PointsInside[currentPoint][2]=currentTuple[2];
		}
		point1[0]= PointsInside[0][0];point1[1]= PointsInside[0][1];point1[2]= PointsInside[0][2];
		point2[0]= PointsInside[1][0];point2[1]= PointsInside[1][1];point2[2]= PointsInside[1][2];

		middle[0]= (point1[0]+point2[0])/2; middle[1]= (point1[1]+point2[1])/2; middle[2]= (point1[2]+point2[2])/2;
		midDepth= cellDepth1+ (cellDepth1+cellDepth2)/2;		


		int position=0;
		bool trouve=false;
		float assayId;
		while( (trouve==false)&&(position< numberOfCellsSource) ){
			current= NULL;									
			current=listAssayId->GetTuple(position);
			assayId= current[0]; //add the HoleId	

			if(id_cell==assayId)
				trouve=true;			
			else position++;
		}
		if(trouve==false){
			augpt=0;
			useful=0;	
		}
		else{		
			bool endId=false;
			while( (endId==false)&&(position< numberOfCellsSource) ){
				current= NULL;									
				current=liststartDepht->GetTuple(position);
				assayStartDepth= current[0]; //add the HoleId	

				current= NULL;									
				current=listendDepth->GetTuple(position);
				assayEndDepth= current[0]; //add the HoleId	

			if( (cellDepth1>assayStartDepth)&&(cellDepth2<assayEndDepth) ){
					current= NULL;									
					current=listAugpt->GetTuple(position);					
					useful+= cellDepth2 - cellDepth1;
					augpt += current[0]*(cellDepth2 - cellDepth1); 										
				}
				else if( (cellDepth1<assayStartDepth)&&(cellDepth2>assayEndDepth) ){
					current= NULL;									
					current=listAugpt->GetTuple(position);					
					useful+= assayEndDepth - assayStartDepth;
					augpt +=current[0]*(assayEndDepth - assayStartDepth); 					
				}
				else if( (cellDepth1<assayStartDepth)&&( (cellDepth2<assayEndDepth)&&(
							cellDepth2>assayStartDepth) ) ){
					current= NULL;									
					current=listAugpt->GetTuple(position);					
					useful+= cellDepth2 - assayStartDepth;
					augpt +=current[0]*(cellDepth2 - assayStartDepth);
				}
				else if( ((cellDepth1>assayStartDepth)&&(cellDepth1<assayEndDepth) )&&
							(cellDepth2>assayEndDepth) ){
					current= NULL;									
					current=listAugpt->GetTuple(position);					
					useful+= assayEndDepth - cellDepth1;
					augpt +=current[0]*(assayEndDepth - cellDepth1);					
				}
				
				position++;
				current= NULL;									
				current=listAssayId->GetTuple(position);
				assayId= current[0]; //add the HoleId	

				if(id_cell!=assayId)
				endId=true;//end of that id assays				
			}			
			useful= useful /(cellDepth2 - cellDepth1);
			augpt= augpt/(cellDepth2 - cellDepth1);
		}
		
		//insertion of the fist point
		 listNewPoints->InsertPoint(compteur,point1[0],point1[1],point1[2]);
		 //add the depht of the last inside point
		 outPointsDepth->InsertNextValue(cellDepth1); 
		 compteur++;
		
		 //insertion of the second point of that cell
		 listNewPoints->InsertPoint(compteur,point2[0],point2[1],point2[2]);
		 //add the depht of the last inside point
		 outPointsDepth->InsertNextValue(cellDepth2); 
		 compteur++;	

		 //add the new cell
		 //insert the cell
		 outCells->InsertNextCell(2);
		 outCells->InsertCellPoint (compteur-2);
		 outCells->InsertCellPoint (compteur-1);
		 outcellId->InsertNextValue(id_cell); //add the HoleId
		 outcellaugpt->InsertNextValue(augpt);//add the augpt
		 outcellUseful->InsertNextValue(useful);//add the useful of the segment

		 nodesa = new vtkIdType[1];
	     nodesa[0] = listMiddlePoints->InsertNextPoint(middle[0],middle[1],middle[2]);
	     cellsMiddle->InsertNextCell(1, nodesa);
		 middleDepth->InsertNextValue(midDepth); 
		 middleAugpt->InsertNextValue(augpt);		
	}

	if(this->TypeOfCompositing==0){
	  output->SetPoints(listNewPoints);
	  output->GetPointData()->AddArray(outPointsDepth);
	  listNewPoints->Delete();
	  outPointsDepth->Delete();

	  output->SetLines(outCells);
	  output->GetCellData()->AddArray(outcellId);
	  output->GetCellData()->AddArray(outcellaugpt);
	  output->GetCellData()->AddArray(outcellUseful);
	  outCells->Delete();
	  outcellId->Delete();
	  outcellaugpt->Delete();
	  outcellUseful->Delete();
	  
	}
	else if(this->TypeOfCompositing==1){
	   output->SetPoints(listMiddlePoints);
	   output->SetVerts(cellsMiddle);
	   output->GetPointData()->AddArray(middleAugpt);
	   output->GetPointData()->AddArray(middleDepth);	  

	   listMiddlePoints->Delete();
	   cellsMiddle->Delete();
	   middleAugpt->Delete();
	   middleDepth->Delete();		
	}   	
}
/***************************************************************************/
int vtkAdvancedCompositing::FillInputPortInformation ( int port, vtkInformation* info ){
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
   return 1;
}
/***************************************************************************/
void vtkAdvancedCompositing::PrintSelf ( ostream& os, vtkIndent indent )
{   
   vtkPolyData *source = this->GetSource();
  this->Superclass::PrintSelf ( os, indent );
  os << indent << "Source: " << source << "\n";
}
