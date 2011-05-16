/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkDrillHolesLinesRefinementTo.cxx $
  Author:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:      August 
  Version:   1.1

  =========================================================================*/
#include "vtkDrillHolesLinesRefinement.h"
#include "vtkTubeFilter.h"
#include "vtkExtractEdges.h"
#include "vtkAMRBox.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkExtractCTHPart.h" // for the BOUNDS key
#include "vtkHierarchicalBoxDataSet.h"
#include "vtkImageMandelbrotSource.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include <assert.h>
#include "vtkDataArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkSmartPointer.h"
#include "vtkUniformGrid.h"
#include "vtkUnsignedCharArray.h"
#include "vtkCell.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkTimerLog.h"

#include "vtkPVConnectivityFilter.h"

#include <assert.h>

vtkCxxRevisionMacro(vtkDrillHolesLinesRefinement, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkDrillHolesLinesRefinement);

//----------------------------------------------------------------------------
vtkDrillHolesLinesRefinement::vtkDrillHolesLinesRefinement()
{
  this->SetNumberOfInputPorts(1);
  
  this->Dimensions = 10;
  this->MaximumLevel = 2;

  this->MinimumLevel = 1;
 
  this->Levels = vtkIntArray::New();
  this->Asymetric = 1;

  this->MakeBounds=0;

  this->Capping=1;
  this->Radius=1.0;
  this->NumberOfSides=4;
  this->TypeOfRefinement=0;
  
  this->TopLevelSpacing[0] = 1.0;
  this->TopLevelSpacing[1] = 1.0;
  this->TopLevelSpacing[2] = 1.0;

  this->TopLevelOrigin[0] = 0.0;
  this->TopLevelOrigin[1] = 0.0;
  this->TopLevelOrigin[2] = 0.0;  

}

//----------------------------------------------------------------------------
vtkDrillHolesLinesRefinement::~vtkDrillHolesLinesRefinement()
{
  this->Levels->Delete();
  this->Levels = NULL;  
}

int vtkDrillHolesLinesRefinement::RequestDataObject(vtkInformation *,
                                              vtkInformationVector **inV,
                                              vtkInformationVector *outV)
{
  vtkInformation *inInfo = inV[0]->GetInformationObject ( 0 );

  vtkInformation *outInfo = outV->GetInformationObject(0);
  vtkCompositeDataSet* outData = NULL;

  outData = vtkMultiBlockDataSet::New();
  
  outData->SetPipelineInformation(outInfo);
  outInfo->Set(vtkDataObject::DATA_EXTENT_TYPE(), outData->GetExtentType());
  outInfo->Set(vtkDataObject::DATA_OBJECT(), outData);
  outData->Delete();
  return 1;
}
/**************************MIRARCO*****************************************
* Function: 	DistancePointToLine				          	              *
* Description: compute the distance from a point to the segment defined   *
*             by  the two given points									  *	
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
float DistancePointToLine(double P[3], double A[3], double B[3])
{
	float distance=-1000;
	//calculate the scalar product AP scalar AB
   double AB[3],BA[3],BP[3], AP[3],P1[3];
   float AB_distance= sqrt( vtkMath::Distance2BetweenPoints( A, B ) ); 
   for (int i=0; i<3; ++i ){
      AB[i] = B[i] - A[i];
	  BA[i] = A[i] - B[i];
      AP[i] = P[i] - A[i];
	  BP[i] = P[i] - B[i];
   }
   double pscalar = AB[0]*AP[0] + AB[1]*AP[1] + AB[2]*AP[2] ;
   float alpha= pscalar/(AB_distance*AB_distance);
   double pscalar1 = BA[0]*BP[0] + BA[1]*BP[1] + BA[2]*BP[2] ;


   if(pscalar < 0)
   {
	    distance =sqrt( vtkMath::Distance2BetweenPoints( P, A ) );
   }
   else if(pscalar1 < 0 )
   {
	    distance =sqrt( vtkMath::Distance2BetweenPoints( P, B ) );
   }
   else
   {
	    //find the coordonnate of the orthogonal projected point
	    P1[0]= (1-alpha)* A[0] +  alpha* B[0];
		P1[1]= (1-alpha)* A[1] +  alpha* B[1];
		P1[2]= (1-alpha)* A[2] +  alpha* B[2];

	    distance =sqrt( vtkMath::Distance2BetweenPoints( P, P1 ) );
   }

   return distance;
}
/**************************MIRARCO*****************************************
* Function: 		DistancePointToLine2   	                              *
* Description: compute the distance from a point to the polyline.so that  *
*                   distance will be the minimum value   				  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
float DistancePointToLine2(double point[3], int HoleId,vtkCellArray* CellsInput1,
						   vtkDataArray* LinesInsideId1,
						   vtkDataArray* PointCoordsInput1)
{
	bool HoleFound=false;
	int currentCell=0;
	double* current;
	double* currentTuple;	   
	float distance=-100;
	CellsInput1->InitTraversal();			
	double segment_PointA[3];
	double segment_PointB[3];
	int cpt=0;

	int numberOfCellsInput1 = CellsInput1->GetNumberOfCells();
	while( (HoleFound==false) && (currentCell < numberOfCellsInput1) )
	{	
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType(); 
		// Create array of points in current cell.
		vtkIdType *pts = new vtkIdType(); 
		CellsInput1->GetNextCell(*npts, *&pts); //a revoir

		current=LinesInsideId1->GetTuple(currentCell);
		if(current[0]==HoleId)
		{
			currentTuple = NULL;
			currentTuple = PointCoordsInput1->GetTuple(pts[0]);

			segment_PointA[0]= currentTuple[0];
			segment_PointA[1]= currentTuple[1];
			segment_PointA[2]= currentTuple[2];	

			for(int currentPoint = 1; currentPoint < *npts; currentPoint++) // find the distance from the point to each segment of the line
																			//and return the minimum
			{
				currentTuple = NULL;
				currentTuple = PointCoordsInput1->GetTuple(pts[currentPoint]);
				
				segment_PointB[0]= currentTuple[0];
				segment_PointB[1]= currentTuple[1];
				segment_PointB[2]= currentTuple[2];

				float dist;
				dist=DistancePointToLine(point, segment_PointA, segment_PointB);
				if(cpt==0)
				{
				 distance= dist;
				 cpt=1;
				}	
				else if( (cpt=1) && (dist< distance) )
				distance= dist;
				
				segment_PointA[0]= currentTuple[0];
				segment_PointA[1]= currentTuple[1];
				segment_PointA[2]= currentTuple[2];	
			}		

			HoleFound= true;		
		
		}
		else currentCell++;
	}

	return distance;

}

/**************************MIRARCO*****************************************
* Function: 		SetRBlockInfo         	                              *
* Description:	set all informations of the grid						  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
void vtkDrillHolesLinesRefinement::SetRBlockInfo(vtkRectilinearGrid *grid,
												 int level,
												 int *ext,
												 int onFace[6],
												 int HoleId)
{  
 
  double bds[6];
  double origin[3];
  double spacing[3];
  
  this->CellExtentToBounds(level, ext, bds);
  origin[0] = bds[0];
  origin[1] = bds[2];
  origin[2] = bds[4];
  spacing[0] = (bds[1]-bds[0])/(double)(ext[1]-ext[0]+1);//define the specifications of this block, orgine+spacing
  spacing[1] = (bds[3]-bds[2])/(double)(ext[3]-ext[2]+1);
  spacing[2] = (bds[5]-bds[4])/(double)(ext[5]-ext[4]+1);

  // Handle Lower dimensions.  Assume that cell dimension of 1 is a collapsed
  // dimension.  Point dim equal 1 also.
  int dim[3];
  
  dim[0] = dim[1] = dim[2] = 1;
  if (ext[1] > ext[0])
    {
    dim[0] = ext[1]-ext[0]+2;
    }
  if (ext[3] > ext[2])
    {
    dim[1] = ext[3]-ext[2]+2;
    }
  if (ext[5] > ext[4])
    {
    dim[2] = ext[5]-ext[4]+2;
    }
  
  grid->SetDimensions(dim);
  
  vtkDoubleArray *coords[3];
  
  vtkMath::RandomSeed(1234);
  int coord=0;
  while(coord<3)
    {
    coords[coord]=vtkDoubleArray::New();
    //grid->SetOrigin(origin);
    
    //first point
    coords[coord]->InsertNextValue(origin[coord]);
    
    double uniformCoordinate;
    
    int i=1;
    int c;
      c=dim[coord]-1;
      
    uniformCoordinate=origin[coord];
      
    while(i<c)
      {
      uniformCoordinate+=spacing[coord];
      //get a random number about 1/5 of the uniform spacing.
      double epsilon=(vtkMath::Random()-0.5)*spacing[coord]*0.4;
      coords[coord]->InsertNextValue(uniformCoordinate+epsilon);
      ++i;
      }    
   
    //last point 
    uniformCoordinate+=spacing[coord];
    coords[coord]->InsertNextValue(uniformCoordinate);
    
    ++coord;
    }
  
  //grid->SetSpacing(spacing);
  grid->SetXCoordinates(coords[0]);
  grid->SetYCoordinates(coords[1]);
  grid->SetZCoordinates(coords[2]);
  coord=0;
  while(coord<3)
    {
    coords[coord]->Delete();
    ++coord;
    }


 //revoir l'utilité de ces infos
  vtkIntArray* array = vtkIntArray::New();
  int numCells=grid->GetNumberOfCells();
  array->Allocate(numCells);
  int cell=0;
  while(cell<numCells)
  {
    array->InsertNextValue(level);
	  ++cell;
  }
  array->SetName("level");
  grid->GetCellData()->AddArray(array);
  array->Delete();

  /* vtkIntArray* array1 = vtkIntArray::New();
  vtkFloatArray* array2 = vtkFloatArray::New();
  int numPoints=grid->GetNumberOfPoints();
  array1->Allocate(numPoints);
  array2->Allocate(numPoints);
  int point=0;
  while(point<numPoints)
  {
    array1->InsertNextValue(HoleId);
	if(HoleId==0)
	array2->InsertNextValue(-1);
	else {
		float distance=0;
		double pt[3];
		grid->GetPoint( point, pt );
		distance= DistancePointToLine2( pt, HoleId,this->CellsInput, this->LinesInsideId, 
						   this->PointCoordsInput);
		array2->InsertNextValue(distance);
	 }
	++point;
  }
  array1->SetName("HoleId"); //we compute the line in the grid wich make the refinement decision
							//we use that to calculate 
  grid->GetPointData()->AddArray(array1);
  array1->Delete();
  array2->SetName("DistanceToHole");
  grid->GetPointData()->AddArray(array2);
  array2->Delete(); */

 
 
}

int vtkDrillHolesLinesRefinement::RequestInformation(
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
int vtkDrillHolesLinesRefinement::FillInputPortInformation ( int port, vtkInformation* info )
  {
	if(port==0)
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
	else 
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 1 );
  return 1;
  }
  
//----------------------------------------------------------------------------
int vtkDrillHolesLinesRefinement::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkCompositeDataSet");
  return 1;
}

int vtkDrillHolesLinesRefinement::RequestData(
  vtkInformation *vtkNotUsed(request), 
  vtkInformationVector **inputVector, 
  vtkInformationVector *outputVector)
{
	vtkTimerLog *timer= vtkTimerLog::New();
	timer->StartTimer();
   // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  vtkPolyData *input = vtkPolyData::SafeDownCast (
				  inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

  vtkDataObject *doOutput=outInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkCompositeDataSet *output=vtkCompositeDataSet::SafeDownCast(doOutput);

  if(output==0)
    {
    vtkErrorMacro("The output is not a HierarchicalDataSet");
    return 0;
    }
  if (!outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()) ||
    !outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES()))
    {
    vtkErrorMacro("Expected information not found. "
                  "Cannot provide update extent.");
    return 0;
    }

  output->Initialize(); // remove all previous blocks
  
  
  // By setting SetMaximumNumberOfPieces(-1) 
  // then GetUpdateNumberOfPieces() should always return the number
  // of processors in the parallel job and GetUpdatePiece() should
  // return the specific process number
  int piece=outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
  int numPieces =outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());
  
  float ox=0.0,oy=0.0,oz=0.0;
  float xSize=0.0,ySize=0.0,zSize=0.0;
  if(this->MakeBounds==0)
  {

	  ox = OriginCx;   
	  oy = OriginCy;   
	  oz = OriginCz;   

	  //size values wich can be changed
	  xSize = SizeCx;   
	  ySize = SizeCy;  
	  zSize = SizeCz;  
   }
  else { 
	  double bounds[6];
	  input->GetBounds(bounds);
      double temp;
	  temp= bounds[0];
	  ox  = temp;
	  temp= bounds[2];
	  oy  = temp;
	  temp= bounds[4];
	  oz  = temp;
	//  size values wich can be changed
	  xSize = bounds[1] - bounds[0];
	  ySize = bounds[3] - bounds[2];
	  zSize = bounds[5] - bounds[4];
   }
  
  int blockId = 0;

  // This is 10x10x10 in cells.
  this->SetTopLevelOrigin(ox, oy, oz);
  this->SetTopLevelSpacing(xSize/this->Dimensions,
                           ySize/this->Dimensions,
                           zSize/this->Dimensions);
  
  
  int ext[6];
  ext[0] = ext[2] = ext[4] = 0;
  ext[1] = ext[3] = ext[5] = this->Dimensions - 1; 
  if (this->Asymetric) 
    { // The changes to an extra 2 in the next level.
    ext[1] += 1;
    }
  if(this->TypeOfRefinement==1)
  { 
   //make a tube to each drillHole we have
   vtkTubeFilter *connect = vtkTubeFilter::New();
   connect->SetInput(input);
   connect->SetNumberOfSides(this->NumberOfSides);
   connect->SetRadius(this->Radius);
   connect->SetCapping(this->Capping);
   connect->Update();

   vtkPolyData* input1= vtkPolyData::New();
   input1->CopyStructure(connect->GetOutput());
   input1->GetPointData()->PassData ( connect->GetOutput()->GetPointData() );
   input1->GetCellData()->PassData ( connect->GetOutput()->GetCellData() );
   connect->Delete();

   //extract the edges of the tube
   vtkExtractEdges *connect1 = vtkExtractEdges::New();
   connect1->SetInput(input1);
   connect1->Update();
   vtkPolyData* input2= vtkPolyData::New();
   input2->CopyStructure(connect1->GetOutput());
   input2->GetPointData()->PassData ( connect1->GetOutput()->GetPointData() );
   input2->GetCellData()->PassData ( connect1->GetOutput()->GetCellData() );
   connect1->Delete();
   input1->Delete();

    this->CellsInput = vtkCellArray::SafeDownCast(input2->GetLines());
	this->PointCoordsInput = input2->GetPoints()->GetData();	 

   }
  else
  { 
	 this->CellsInput = vtkCellArray::SafeDownCast(input->GetLines());
	 this->PointCoordsInput = input->GetPoints()->GetData();	
	 vtkCellData* p= input->GetCellData();
	 this->LinesInsideId= p->GetArray("HoleId");//get the id of the drillHoles
  }
  // Get a global (across all processes) count of the blocks.
  // Do not create the blocks.
  this->StartBlock = 0;
  this->EndBlock = -1;
  this->BlockCount = 0;
  int onFace[6];
  onFace[0]=1;
  onFace[1]=1;
  onFace[2]=1;
  onFace[3]=1;
  onFace[4]=1;
  onFace[5]=1;
  int id=1;
  /*this->RefineBlock(blockId, 0, output, ext[0], ext[1], ext[2], ext[3], ext[4],
					 ext[5],onFace,id);*/

  this->InsertedGridsInfos= vtkIntArray::New();

  this->drillholesRefine(blockId, 0, output, ext[0], ext[1], ext[2], ext[3], ext[4],
                 ext[5],onFace,id );
  // Generate our share of the blocks.
  this->StartBlock = (int)((float)(piece*this->BlockCount)/(float)(numPieces));
  this->EndBlock = (int)((float)((piece+1)*this->BlockCount)/(float)(numPieces)) - 1;
  this->BlockCount = 0;

  this->Levels->Initialize();
  /* this->RefineBlock(blockId, 0, output, ext[0], ext[1], ext[2], ext[3], ext[4],
                 ext[5],onFace,id);  */
  
  this->drillholesRefine(blockId, 0, output, ext[0], ext[1], ext[2], ext[3], ext[4],
                 ext[5],onFace,id );
  double bounds[6];
  bounds[0]=ox;
  bounds[1]=ox+xSize;
  bounds[2]=oy;
  bounds[3]=oy+ySize;
  bounds[4]=oz;
  bounds[5]=oz+zSize;
    
  
  outInfo->Set(vtkExtractCTHPart::BOUNDS(),bounds,6);

 
 //this->CellsInput= NULL;
 this->CellsInput->Delete();
// this->PointCoordsInput=NULL;
 this->PointCoordsInput->Delete();
 
 timer->StopTimer();

 //vtkErrorMacro("Timer of all iterations= "<< timer->GetElapsedTime());
  return 1;
}  

/**************************MIRARCO*****************************************
* Function: 	LineIntersectBoxTest       	                              *
* Description:	detect if the line intersect or not the given grid   	  *
* Input parameters : 													  *		                                                                 
* Output Parametres: int                                                      *
* Date:					                                                  *
***************************************************************************/
int LineIntersectBoxTest(double x0, double y0, double z0,
                             double x1, double y1, double z1, 
                             double bds[6]) 
{
  // intersect line with box
  double x, y, z;
  double k;

  // Special case ane point is inside box.
  if (x0>bds[0] && x0<bds[1] && y0>bds[2] && y0<bds[3] && z0>bds[4]
      && z0<bds[5])
    {
    return 1;
    }
  if (x1>bds[0] && x1<bds[1] && y1>bds[2] && y1<bds[3] && z1>bds[4]
      && z1<bds[5])
    {
    return 1;
    }

  // Do not worry about divide by zero.
  // min x
  x = bds[0];
  k = (x- x0) / (x1-x0);
  if (k >=0.0 && k <= 1.0)
    {
    y = y0 + k*(y1-y0);
    z = z0 + k*(z1-z0);
    if (y >= bds[2] && y <= bds[3] && z >= bds[4] && z <= bds[5])
      {
      return 1;
      }
    } 
  // max x
  x = bds[1];
  k = (x- x0) / (x1-x0);
  if (k >=0.0 && k <= 1.0)
    {
    y = y0 + k*(y1-y0);
    z = z0 + k*(z1-z0);
    if (y >= bds[2] && y <= bds[3] && z >= bds[4] && z <= bds[5])
      {
      return 1;
      }
    } 
  // min y
  y = bds[2];
  k = (y- y0) / (y1-y0);
  if (k >=0.0 && k <= 1.0)
    {
    x = x0 + k*(x1-x0);
    z = z0 + k*(z1-z0);
    if (x >= bds[0] && x <= bds[1] && z >= bds[4] && z <= bds[5])
      {
      return 1;
      }
    } 
  // max y
  y = bds[3];
  k = (y- y0) / (y1-y0);
  if (k >=0.0 && k <= 1.0)
    {
    x = x0 + k*(x1-x0);
    z = z0 + k*(z1-z0);
    if (x >= bds[0] && x <= bds[1] && z >= bds[4] && z <= bds[5])
      {
      return 1;
      }
    } 
  // min z
  z = bds[4];
  k = (z- z0) / (z1-z0);
  if (k >=0.0 && k <= 1.0)
    {
    x = x0 + k*(x1-x0);
    y = y0 + k*(y1-y0);
    if (y >= bds[2] && y <= bds[3] && x >= bds[0] && x <= bds[1])
      {
      return 1;
      }
    } 

  return 0;
}


//**************************************************************************
void vtkDrillHolesLinesRefinement::drillholesRefine(int &blockId,
									  int level,
                                      vtkCompositeDataSet* output,
                                      int x0,
                                      int x3,
                                      int y0,
                                      int y3,
                                      int z0,
                                      int z3,
                                      int onFace[6],int id) 
{
	int numberOfCellsInput=0;
    double* currentTuple;	    		
	numberOfCellsInput= this->CellsInput->GetNumberOfCells();

	this->CellsInput->InitTraversal();	
	double segment_PointA[3];
	double segment_PointB[3];
	double* current;

	
	for(int currentCell = 0; currentCell < numberOfCellsInput; currentCell++)
	{		 
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType(); 
		// Create array of points in current cell.
		vtkIdType *pts = new vtkIdType(); 
		
		this->CellsInput->GetNextCell(*npts, *&pts);
		
		currentTuple = NULL;
		currentTuple = this->PointCoordsInput->GetTuple(pts[0]);
		
		segment_PointA[0]= currentTuple[0];
		segment_PointA[1]= currentTuple[1];
		segment_PointA[2]= currentTuple[2];	
	   
		for(int currentPoint = 1; currentPoint < *npts; currentPoint++)
		{
			currentTuple = NULL;
			currentTuple = this->PointCoordsInput->GetTuple(pts[currentPoint]);
			
			segment_PointB[0]= currentTuple[0];
			segment_PointB[1]= currentTuple[1];
			segment_PointB[2]= currentTuple[2];

													
			current=this->LinesInsideId->GetTuple(currentCell);			

			RefineBlock(blockId, 0, output, x0, x3, y0, y3, z0,
						z3,onFace,current[0],
						segment_PointA, segment_PointB );

			segment_PointA[0]= currentTuple[0];
			segment_PointA[1]= currentTuple[1];
			segment_PointA[2]= currentTuple[2];	

		 }	

		npts= NULL;pts=NULL; 
		delete npts;				
		delete pts;
		//delete current;

	 }	


 }	
/**************************MIRARCO*****************************************
* Function: 	subdivideTest	          	                              *
* Description:		test if the bound will be subdivide or not													  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
int vtkDrillHolesLinesRefinement::subdivideTest(double bds[6], int level,
												double segment_PointA[3],
												double segment_PointB[3]) 
{
    
    if( LineIntersectBoxTest(segment_PointA[0], segment_PointA[1], segment_PointA[2],
                     segment_PointB[0], segment_PointB[1], segment_PointB[2], bds)==1)
    {
		return 1; 			
	}
	else return 0;	
}

/**************************MIRARCO*****************************************
* Function: 			RefineBlock       	                              *
* Description:	this method is iterative method and refine the block to   *
*                    the maximum level                                    *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
void vtkDrillHolesLinesRefinement::RefineBlock(int &blockId,
									  int level,
                                      vtkCompositeDataSet* output,
                                      int x0,
                                      int x3,
                                      int y0,
                                      int y3,
                                      int z0,
                                      int z3,
                                      int onFace[6],int id, double segment_PointA[3], double segment_PointB[3])
{
   int subdivide = 0;    

  double bds[6];
  int x1, x2, y1, y2, z1, z2;
  
  // Get the bounds of the proposed block.
  int ext[6];
  ext[0]=x0; ext[1]=x3; ext[2]=y0; ext[3]=y3, ext[4]=z0; ext[5]=z3;
  this->CellExtentToBounds(level, ext, bds);

  x0 = x0*2;
  x3 = (x3+1)*2 - 1;
  y0 = y0*2;
  y3 = (y3+1)*2 - 1;
  z0 = z0*2;
  z3 = (z3+1)*2 - 1;

  x2 = x0+this->Dimensions;
  x1 = x2-1;
  y2 = y0+this->Dimensions;
  y1 = y2-1;
  z2 = z0+this->Dimensions;
  z1 = z2-1;

  if (x3-x2-x1+x0 > 2)
    { // balance asymetric blocks.
    x2 += 2;
    x1 += 2;
    }
    
   int subOnFace[6];  
   // 3D
   int res=0;
   res= subdivideTest( bds, level,segment_PointA,segment_PointB);
   if(res==1) 
   subdivide=1;

   // Check for hard coded minimum and maximum level restrictions.
 if (level < this->MinimumLevel)
    {
    subdivide = 1;
    }
  if (level >= this->MaximumLevel)
    {
    subdivide = 0;
    }
  
   if(subdivide )
      { // break block into eight.
      ++level;
      // RefineBlock the 8 new blocks.
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=onFace[2];
      subOnFace[3]=0;
      subOnFace[4]=onFace[4];
      subOnFace[5]=0;
      this->RefineBlock(blockId, level, output, x0,x1,y0,y1,z0,z1,subOnFace,id, segment_PointA, segment_PointB);
     
	  subOnFace[0]=0;
      subOnFace[1]=onFace[1];
      this->RefineBlock(blockId, level, output, x2,x3,y0,y1,z0,z1,subOnFace,id, segment_PointA, segment_PointB);
     
	  subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=0;
      subOnFace[3]=onFace[3];
      this->RefineBlock(blockId, level, output, x0,x1,y2,y3,z0,z1,subOnFace,id, segment_PointA, segment_PointB);
      
	  subOnFace[0]=0;
      subOnFace[1]=onFace[1];
      this->RefineBlock(blockId, level, output, x2,x3,y2,y3,z0,z1,subOnFace,id, segment_PointA, segment_PointB);
      
      
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=onFace[2];
      subOnFace[3]=0;
      subOnFace[4]=0;
      subOnFace[5]=onFace[5];
      this->RefineBlock(blockId, level, output, x0,x1,y0,y1,z2,z3,subOnFace,id, segment_PointA, segment_PointB);
    
	  subOnFace[0]=0;
      subOnFace[1]=onFace[1];
      this->RefineBlock(blockId, level, output, x2,x3,y0,y1,z2,z3,subOnFace,id, segment_PointA, segment_PointB);
     
	  subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=0;
      subOnFace[3]=onFace[3];
      this->RefineBlock(blockId, level, output, x0,x1,y2,y3,z2,z3,subOnFace,id, segment_PointA, segment_PointB);
    
	  subOnFace[0]=0;
      subOnFace[1]=onFace[1];
      this->RefineBlock(blockId, level, output, x2,x3,y2,y3,z2,z3,subOnFace,id, segment_PointA, segment_PointB);
      }
    else 
      {
		//if(level==this->MaximumLevel)// this change now
		  //{
		
		  unsigned int addblock=0;
		  if (this->BlockCount >= this->StartBlock
			  && this->BlockCount <= this->EndBlock)
		  {
			  //those next iterations check any duplicate of grid and if isFound 
			  //is true the grid is not added again
			  bool isFound= false;			  
			 
			  if(level>0)
			  {
				  
			    int numberOfTuple= this->InsertedGridsInfos->GetNumberOfTuples();
				if(numberOfTuple>0) 
				{
				  double currentTuple[7];	
				  int j=0; 
				  
				  while( (!isFound) && (j< (numberOfTuple/7)))
				  {
					  currentTuple[0]= this->InsertedGridsInfos->GetTuple1(7*j+0);
					  currentTuple[1]= this->InsertedGridsInfos->GetTuple1(7*j+1);
					  currentTuple[2]= this->InsertedGridsInfos->GetTuple1(7*j+2);
					  currentTuple[3]= this->InsertedGridsInfos->GetTuple1(7*j+3);
					  currentTuple[4]= this->InsertedGridsInfos->GetTuple1(7*j+4);
					  currentTuple[5]= this->InsertedGridsInfos->GetTuple1(7*j+5);
					  currentTuple[6]= this->InsertedGridsInfos->GetTuple1(7*j+6);
					 	 
					  if( (currentTuple[0]==ext[0])&&(currentTuple[1]==ext[1])
												   &&(currentTuple[2]==ext[2])
												   &&(currentTuple[3]==ext[3])
												   &&(currentTuple[4]==ext[4])
												   &&(currentTuple[5]==ext[5])
												   &&(currentTuple[6]==level)  )
				     isFound= true;
					 
					  j++;
					 
				  }
				  
				}
			  }	
			  
			  if(!isFound)
			  {
				  int* temp1;
				  temp1= new int;
				  *temp1= ext[0];
				  this->InsertedGridsInfos->InsertTupleValue(7*blockId+0,temp1);
				 
				  *temp1= ext[1];
				  this->InsertedGridsInfos->InsertTupleValue(7*blockId+1,temp1);
				
				  *temp1= ext[2];
				  this->InsertedGridsInfos->InsertTupleValue(7*blockId+2,temp1);
				
				  *temp1= ext[3];
				  this->InsertedGridsInfos->InsertTupleValue(7*blockId+3,temp1);
				
				  *temp1= ext[4];
				  this->InsertedGridsInfos->InsertTupleValue(7*blockId+4,temp1);
				
				  *temp1= ext[5];
				  this->InsertedGridsInfos->InsertTupleValue(7*blockId+5,temp1);
				 
				  *temp1= level;
				  this->InsertedGridsInfos->InsertTupleValue(7*blockId+6,temp1);
				  delete temp1;



				  vtkRectilinearGrid *grid=vtkRectilinearGrid::New();
			   
			      this->AppedDataSetToLevel(output, level, ext, grid);//adding the new grid to the output
			      grid->Delete();
			      
			      this->SetRBlockInfo(grid, level, ext,onFace,id);
			
			      this->Levels->InsertValue(blockId, level);	
			     ++blockId;	
			  }
			 	
			}
		
		  ++this->BlockCount;
		// }
      }    
}


/**************************MIRARCO*****************************************
* Function: 			CellExtentToBounds 	                              *
* Description:	defined the bound with the given bounds coordonnates	  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
void vtkDrillHolesLinesRefinement::CellExtentToBounds(int level,
                                                int ext[6],
                                                double bds[6])
{
  int spacingFactor = 1;
  double spacing[3];
  spacingFactor = spacingFactor << level;
  
  spacing[0] = this->TopLevelSpacing[0] / (double)(spacingFactor);
  spacing[1] = this->TopLevelSpacing[1] / (double)(spacingFactor);
  spacing[2] = this->TopLevelSpacing[2] / (double)(spacingFactor);

  bds[0] = this->TopLevelOrigin[0] + (double)(ext[0]) * spacing[0];
  bds[1] = this->TopLevelOrigin[0] + (double)(ext[1]+1) * spacing[0];
  bds[2] = this->TopLevelOrigin[1] + (double)(ext[2]) * spacing[1];
  bds[3] = this->TopLevelOrigin[1] + (double)(ext[3]+1) * spacing[1];
  bds[4] = this->TopLevelOrigin[2] + (double)(ext[4]) * spacing[2];
  bds[5] = this->TopLevelOrigin[2] + (double)(ext[5]+1) * spacing[2];
}

/**************************MIRARCO*****************************************
* Function: 			AppedDataSetToLevel	                              *
* Description:	add the grid to the MultiBlockDataSet					  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
void vtkDrillHolesLinesRefinement::AppedDataSetToLevel(
  vtkCompositeDataSet* composite,
  unsigned int level,
  int extents[6],
  vtkDataSet* dataset)
{
	
  unsigned int index = 0;

 
  vtkMultiBlockDataSet* mbDS = vtkMultiBlockDataSet::SafeDownCast(composite);
 
  vtkMultiBlockDataSet* block = vtkMultiBlockDataSet::SafeDownCast(
  mbDS->GetBlock(level));

  if (!block)
  {
    block = vtkMultiBlockDataSet::New();
    mbDS->SetBlock(level, block);
    block->Delete();	
  }
  index = block->GetNumberOfBlocks();
  block->SetBlock(index, dataset);     
	
}


void vtkDrillHolesLinesRefinement::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Dimensions: " << this->Dimensions << endl;
  os << indent << "MaximumLevel: " << this->MaximumLevel << endl;
  os << indent << "MinimumLevel: " << this->MinimumLevel << endl;
  os<< endl;

}

