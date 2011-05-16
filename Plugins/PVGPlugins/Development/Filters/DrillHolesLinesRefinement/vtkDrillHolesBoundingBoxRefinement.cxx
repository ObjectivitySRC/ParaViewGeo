/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkDrillHolesBoundingBoxRefinementTo.cxx $
  Author:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:      
  Version:   0.1



  =========================================================================*/
#include "vtkDrillHolesBoundingBoxRefinement.h"
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


#include "vtkPVConnectivityFilter.h"

#include <assert.h>

vtkCxxRevisionMacro(vtkDrillHolesBoundingBoxRefinement, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkDrillHolesBoundingBoxRefinement);

//----------------------------------------------------------------------------
vtkDrillHolesBoundingBoxRefinement::vtkDrillHolesBoundingBoxRefinement()
{
  this->SetNumberOfInputPorts(1);
  
  this->Dimensions = 10;
  this->MaximumLevel = 5;

  this->MinimumLevel = 1;
 
  this->Levels = vtkIntArray::New();
  this->Asymetric = 1;

  this->MakeBounds=0;

  this->Capping=1;
  this->Radius=1.0;
  this->NumberOfSides=4;

  
  this->TopLevelSpacing[0] = 1.0;
  this->TopLevelSpacing[1] = 1.0;
  this->TopLevelSpacing[2] = 1.0;

  this->TopLevelOrigin[0] = 0.0;
  this->TopLevelOrigin[1] = 0.0;
  this->TopLevelOrigin[2] = 0.0;  

 
  //this->TimeStep = 0;
  //this->TimeStepRange[0] = 0;
  //this->TimeStepRange[1] = 10;
}

//----------------------------------------------------------------------------
vtkDrillHolesBoundingBoxRefinement::~vtkDrillHolesBoundingBoxRefinement()
{
  this->Levels->Delete();
  this->Levels = NULL;
}

//----------------------------------------------------------------------------
int vtkDrillHolesBoundingBoxRefinement::RequestDataObject(vtkInformation *,
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

//----------------------------------------------------------------------------
void vtkDrillHolesBoundingBoxRefinement::SetRBlockInfo(vtkRectilinearGrid *grid,int level,int *ext,int onFace[6])
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
    
    // first point
    coords[coord]->InsertNextValue(origin[coord]);
    
    double uniformCoordinate;
    
    int i=1;
    int c;
      c=dim[coord]-1;
      
    uniformCoordinate=origin[coord];
      
    while(i<c)
      {
      uniformCoordinate+=spacing[coord];
      // get a random number about 1/5 of the uniform spacing.
      double epsilon=(vtkMath::Random()-0.5)*spacing[coord]*0.4;
      coords[coord]->InsertNextValue(uniformCoordinate+epsilon);
      ++i;
      }    
   
    // last point 
    uniformCoordinate+=spacing[coord];
    coords[coord]->InsertNextValue(uniformCoordinate);
    
    ++coord;
    }
  
//  grid->SetSpacing(spacing);
  grid->SetXCoordinates(coords[0]);
  grid->SetYCoordinates(coords[1]);
  grid->SetZCoordinates(coords[2]);
  coord=0;
  while(coord<3)
    {
    coords[coord]->Delete();
    ++coord;
    }
 
}

//----------------------------------------------------------------------------
// Description:
// This is called by the superclass.
// This is the method you should override.
int vtkDrillHolesBoundingBoxRefinement::RequestInformation(
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
int vtkDrillHolesBoundingBoxRefinement::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  //info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 1 );
  return 1;
  }
  
//----------------------------------------------------------------------------
int vtkDrillHolesBoundingBoxRefinement::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkCompositeDataSet");
  return 1;
}
  

//----------------------------------------------------------------------------
// Description:
// This is called by the superclass.
// This is the method you should override.
int vtkDrillHolesBoundingBoxRefinement::RequestData(
  vtkInformation *vtkNotUsed(request), 
  vtkInformationVector **inputVector, 
  vtkInformationVector *outputVector)
{
   // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  vtkPolyData *input = vtkPolyData::SafeDownCast (
				  inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

  vtkDataObject *doOutput=outInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkCompositeDataSet *output=vtkCompositeDataSet::SafeDownCast(doOutput);

  if(output==0)
    {
    vtkErrorMacro("The output is not a HierarchicalDataSet");//la sortie n'est pas une structure hiérachique
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
	  ox = OriginCX[0];
	  oy = OriginCX[1];
	  oz = OriginCX[2];
	//  size values wich can be changed
	  xSize = SizeCX[0];
	  ySize = SizeCX[1];
	  zSize = SizeCX[2];
   }
  else { //in this case we make a fisrt block will be the bonding box to all of the drillHoles
	  double bounds[6];
	  input->GetBounds(bounds);

	  ox = bounds[0];
	  oy = bounds[2];
	  oz = bounds[4];
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
  ext[1] = ext[3] = ext[5] = this->Dimensions - 1; //oui parfait
  if (this->Asymetric) 
    { // The changes to an extra 2 in the next level.
    ext[1] += 1;
    }

   vtkTubeFilter *connect = vtkTubeFilter::New();
   connect->SetInput(input);
   connect->SetNumberOfSides(this->NumberOfSides);
   connect->SetRadius(this->Radius);
   connect->SetCapping(this->Capping);
   connect->Update();
  
   vtkPolyData* input1= vtkPolyData::New();
  // input1->ShallowCopy(connect->GetOutput());
   input1->CopyStructure(connect->GetOutput());
   input1->GetPointData()->PassData ( connect->GetOutput()->GetPointData() );
   input1->GetCellData()->PassData ( connect->GetOutput()->GetCellData() );
  // connect->Delete();

   vtkExtractEdges *connect1 = vtkExtractEdges::New();
   connect1->SetInput(input1);
   connect1->Update();
   vtkPolyData* input2= vtkPolyData::New();
   input2->CopyStructure(connect1->GetOutput());
   input2->GetPointData()->PassData ( connect1->GetOutput()->GetPointData() );
   input2->GetCellData()->PassData ( connect1->GetOutput()->GetCellData() );
  // connect1->Delete();
   //input1->Delete();




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
  this->RefineBlock(blockId, 0, output, input2, ext[0], ext[1], ext[2], ext[3], ext[4],
                 ext[5],onFace);

  // Generate our share of the blocks.
  this->StartBlock = (int)((float)(piece*this->BlockCount)/(float)(numPieces));
  this->EndBlock = (int)((float)((piece+1)*this->BlockCount)/(float)(numPieces)) - 1;
  this->BlockCount = 0;

  this->Levels->Initialize();
  this->RefineBlock(blockId, 0, output, input2, ext[0], ext[1], ext[2], ext[3], ext[4],
                 ext[5],onFace);  
  
  double bounds[6];

  bounds[0]=ox;
  bounds[1]=ox+xSize;
  bounds[2]=oy;
  bounds[3]=oy+ySize;
  bounds[4]=oz;
  bounds[5]=oz+zSize;
    
  
  outInfo->Set(vtkExtractCTHPart::BOUNDS(),bounds,6);// permet de faire un clipping sur ce bloack apres
  
  return 1;
}  
//----------------------------------------------------------------------------
// Volume of a tetrahedron.
// Return a positive value if AB, AC, AD follows the right hand rule.
// Return a negative value otherwise
double OrientedVolume1( double* A, double* B, double* C, double* D )
{
   double AB[3], AC[3], AD[3];
   for (int i=0; i<3; ++i ){
      AB[i] = B[i] - A[i];
      AC[i] = C[i] - A[i];
      AD[i] = D[i] - A[i];
   }
   double pv0 = AC[1]*AD[2] - AD[1]*AC[2];
   double pv1 = AC[2]*AD[0] - AD[2]*AC[0];
   double pv2 = AC[0]*AD[1] - AD[0]*AC[1];
   return 0.33333333333333 * ( AB[0]*pv0 + AB[1]*pv1 + AB[2]*pv2 );
}
//----------------------------------------------------------------------------
double RectangleArea1( double* A, double* B, double* C )
{
   double AB[3], AC[3];
   for (int i=0; i<3; ++i ){
      AB[i] = B[i] - A[i];
      AC[i] = C[i] - A[i];
   }
   // Area = norm ( AB x AC ) / 2
   double pv0 = AB[1]*AC[2] - AC[1]*AB[2];
   double pv1 = AB[2]*AC[0] - AC[2]*AB[0];
   double pv2 = AB[0]*AC[1] - AC[0]*AB[1];
   return sqrt( pv0*pv0 + pv1*pv1 + pv2*pv2 );
}
//----------------------------------------------------------------------------
int LineIntersectBoxTest1(double A[3], double B[3],double face_ptA[3], double face_ptB[3], double face_ptC[3],double bds[6])
{
	double Xmax= bds[1];
	double Xmin= bds[0];
	double Ymax= bds[3];
	double Ymin= bds[2];
	double Zmax= bds[5];
	double Zmin= bds[4];

  	double eps= 0.000002;	
	int nIter=0;
	double pt1[3];
	double pt2[3];
	double milieu[3];

	double temp=0;

	temp= A[0];
	pt1[0]=temp; 
	temp= A[1];
	pt1[1]=temp; 
	temp= A[2];
	pt1[2]=temp; 
	

	temp= B[0];
	pt2[0]=temp; 
	temp= B[1];
	pt2[1]=temp; 
	temp= B[2];
	pt2[2]=temp; 


	double v_pt2=0;
	double v_pt1=0;
	double v_milieu=0;
	
	while(nIter<20)
	{	
		milieu[0]= 0.5*pt1[0] + 0.5*pt2[0];
		milieu[1]= 0.5*pt1[1] + 0.5*pt2[1];
		milieu[2]= 0.5*pt1[2] + 0.5*pt2[2];		

		if( ((milieu[0]>=Xmin)&&(milieu[0]<=Xmax))&&((milieu[1]>=Ymin)&&(milieu[1]<=Ymax))
								&&((milieu[2]>=Zmin)&&(milieu[2]<=Zmax)) )
		return 1;
		else 
		{
			double area = RectangleArea1( face_ptA, face_ptB, face_ptC );
			double zero = eps * area; // homogeneus to a volume
			v_pt1= OrientedVolume1( face_ptA, face_ptB, face_ptC, pt1 );
			v_pt2= OrientedVolume1( face_ptA, face_ptB, face_ptC, pt2 );
			v_milieu= OrientedVolume1( face_ptA, face_ptB, face_ptC, milieu );

			if(v_pt1*v_milieu< zero*zero)
			{
				pt2[0]= milieu[0];
			    pt2[1]= milieu[1];
			    pt2[2]= milieu[2];
			}
			else if(v_pt2*v_milieu< zero*zero)
			{
				pt1[0]= milieu[0];
			    pt1[1]= milieu[1];
			    pt1[2]= milieu[2];
			}
		}
		nIter++;
	}
	return 0;
}
//----------------------------------------------------------------------------
int vtkDrillHolesBoundingBoxRefinement::subdivideTest(vtkPolyData* input2,
                            double bds[6], int level) 
{
 	
  // input2->BuildCells();
	vtkCellArray* linesInput =  vtkCellArray::SafeDownCast(input2->GetLines());
   vtkCellArray* CellsInput;
   vtkDataArray* PointCoordsInput;
   vtkPoints* inPoints;
   int numberOfCellsInput=0;
   double* currentTuple;
   // CellsInput->InitTraversal();
    if(linesInput->GetNumberOfCells() > 0){
		CellsInput = vtkCellArray::SafeDownCast(input2->GetLines());
	}
	    		
	numberOfCellsInput= CellsInput->GetNumberOfCells();
	inPoints= input2->GetPoints();
	PointCoordsInput = input2->GetPoints()->GetData();
	int numberOfPointsInput= input2->GetPoints()->GetNumberOfPoints();

	double Xmax= bds[1];
	double Xmin= bds[0];
	double Ymax= bds[3];
	double Ymin= bds[2];
	double Zmax= bds[5];
	double Zmin= bds[4];
	CellsInput->InitTraversal();	
	for(int currentCell = 0; currentCell < numberOfCellsInput; currentCell++)
	{		 
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType(); 
		// Create array of points in current cell.
		vtkIdType *pts = new vtkIdType(); 
		
		CellsInput->GetNextCell(*npts, *&pts);	

		double segment_PointA[3];
		currentTuple = NULL;
		currentTuple = PointCoordsInput->GetTuple(pts[0]);
		//currentTuple = inPoints->GetPoint(pts[0]);

		segment_PointA[0]= currentTuple[0];
		segment_PointA[1]= currentTuple[1];
		segment_PointA[2]= currentTuple[2];
	
	    
		if( ((segment_PointA[0]>=Xmin)&&(segment_PointA[0]<=Xmax))&&((segment_PointA[1]>=Ymin)&&(segment_PointA[1]<=Ymax))
								&&((segment_PointA[2]>=Zmin)&&(segment_PointA[2]<=Zmax)) )
		return 1;


		double segment_PointB[3];

		double face_ptA[3];
		double face_ptB[3];
		double face_ptC[3];

		double vb = 0;
		double va = 0;
		double eps = 0.000002; // distance relative to triangle size
		double area =0;
		double zero =0; // homogeneus to a volume

		for(int currentPoint = 1; currentPoint < *npts; currentPoint++)
		{
			currentTuple = NULL;
			currentTuple = PointCoordsInput->GetTuple(pts[currentPoint]);
			//currentTuple = inPoints->GetPoint(pts[currentPoint]);

			segment_PointB[0]= currentTuple[0];
			segment_PointB[1]= currentTuple[1];
			segment_PointB[2]= currentTuple[2];
			
			if( ((segment_PointB[0]>=Xmin)&&(segment_PointB[0]<=Xmax))&&((segment_PointB[1]>=Ymin)&&(segment_PointB[1]<=Ymax))
								&&((segment_PointB[2]>=Zmin)&&(segment_PointB[2]<=Zmax)) )
			return 1;

			/*i have one segment and i need to find if i have an intersection 
			with one face of 6 faces of my block */

			//face1
			face_ptA[0]=Xmin;   face_ptA[1]=Ymin;   face_ptA[2]=Zmin;
			face_ptB[0]=Xmax;   face_ptB[1]=Ymin;   face_ptB[2]=Zmin;
			face_ptC[0]=Xmin;   face_ptC[1]=Ymax;   face_ptC[2]=Zmin;
			area = RectangleArea1( face_ptA, face_ptB, face_ptC );
			zero = eps * area; // homogeneus to a volume
			vb= OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointA );
			va= OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointB );
			if(va*vb< zero*zero)
			{
				if(LineIntersectBoxTest1(segment_PointA, segment_PointB, face_ptA, face_ptB, face_ptC, bds)==1)
				return 1;
			}
			//face2
			face_ptA[0]=Xmin;   face_ptA[1]=Ymin;   face_ptA[2]=Zmax;
			face_ptB[0]=Xmax;   face_ptB[1]=Ymin;   face_ptB[2]=Zmax;
			face_ptC[0]=Xmin;   face_ptC[1]=Ymax;   face_ptC[2]=Zmax;
			area = RectangleArea1( face_ptA, face_ptB, face_ptC );
			zero = eps * area; // homogeneus to a volume
			vb = OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointA );
			va = OrientedVolume1( face_ptA, face_ptB, face_ptC,segment_PointB );
			if(va*vb< zero*zero)
			{
				if(LineIntersectBoxTest1(segment_PointA, segment_PointB, face_ptA, face_ptB, face_ptC, bds)==1)
				return 1;
			}
			//face3
			face_ptA[0]=Xmin;   face_ptA[1]=Ymin;   face_ptA[2]=Zmin;
			face_ptB[0]=Xmin;   face_ptB[1]=Ymin;   face_ptB[2]=Zmax;
			face_ptC[0]=Xmin;   face_ptC[1]=Ymax;   face_ptC[2]=Zmin;
			area = RectangleArea1( face_ptA, face_ptB, face_ptC );
			zero = eps * area; // homogeneus to a volume
			vb= OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointA );
			va= OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointB );
			if(va*vb< zero*zero)
			{
				if(LineIntersectBoxTest1(segment_PointA, segment_PointB, face_ptA, face_ptB, face_ptC, bds)==1)
				return 1;
			}
			//face4
			face_ptA[0]=Xmax;   face_ptA[1]=Ymin;   face_ptA[2]=Zmin;
			face_ptB[0]=Xmax;   face_ptB[1]=Ymin;   face_ptB[2]=Zmax;
			face_ptC[0]=Xmax;   face_ptC[1]=Ymax;   face_ptC[2]=Zmin;
			area = RectangleArea1( face_ptA, face_ptB, face_ptC );
			zero = eps * area; // homogeneus to a volume
			vb= OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointA );
			va = OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointB );
			if(va*vb< zero*zero)
			{
				if(LineIntersectBoxTest1(segment_PointA, segment_PointB, face_ptA, face_ptB, face_ptC, bds)==1)
				return 1;
			}
			//face5
			face_ptA[0]=Xmin;   face_ptA[1]=Ymin;   face_ptA[2]=Zmin;
			face_ptB[0]=Xmax;   face_ptB[1]=Ymin;   face_ptB[2]=Zmin;
			face_ptC[0]=Xmin;   face_ptC[1]=Ymin;   face_ptC[2]=Zmax;
			area = RectangleArea1( face_ptA, face_ptB, face_ptC );
			zero = eps * area; // homogeneus to a volume
			vb= OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointA );
			va= OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointB );
			if(va*vb< zero*zero)
			{
				if(LineIntersectBoxTest1(segment_PointA, segment_PointB, face_ptA, face_ptB, face_ptC, bds)==1)
				return 1;
			}
			//face6
			face_ptA[0]=Xmin;   face_ptA[1]=Ymax;   face_ptA[2]=Zmin;
			face_ptB[0]=Xmax;   face_ptB[1]=Ymax;   face_ptB[2]=Zmin;
			face_ptC[0]=Xmin;   face_ptC[1]=Ymax;   face_ptC[2]=Zmax;
			area = RectangleArea1( face_ptA, face_ptB, face_ptC );
			zero = eps * area; // homogeneus to a volume
			vb= OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointA );
			va= OrientedVolume1( face_ptA, face_ptB, face_ptC, segment_PointB );
			if(va*vb< zero*zero)
			{
				if(LineIntersectBoxTest1(segment_PointA, segment_PointB, face_ptA, face_ptB, face_ptC, bds)==1)
				return 1;
			}

			segment_PointA[0]= currentTuple[0];
			segment_PointA[1]= currentTuple[1];
			segment_PointA[2]= currentTuple[2];	
		}	

		
	 }
	//set cellsInput to nULL than delete	
	//delete npts;
	//delete pts;
	//CellsInput->Delete();
	//linesInput->Delete();
	//PointCoordsInput->Delete();
	return 0;	
}
//----------------------------------------------------------------------------
void vtkDrillHolesBoundingBoxRefinement::RefineBlock(int &blockId,
									  int level,
                                      vtkCompositeDataSet* output, vtkPolyData* input2,
                                      int x0,
                                      int x3,
                                      int y0,
                                      int y3,
                                      int z0,
                                      int z3,
                                      int onFace[6])
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
   res= subdivideTest(input2, bds, level);
   if( res==1 )
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
      this->RefineBlock(blockId, level, output, input2, x0,x1,y0,y1,z0,z1,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnFace[2]=onFace[2];
//      subOnFace[3]=0;
      this->RefineBlock(blockId, level, output, input2, x2,x3,y0,y1,z0,z1,subOnFace);
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=0;
      subOnFace[3]=onFace[3];
      this->RefineBlock(blockId, level, output, input2, x0,x1,y2,y3,z0,z1,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnface[2]=0;
//      subOnFace[3]=onFace[3];
      this->RefineBlock(blockId, level, output, input2, x2,x3,y2,y3,z0,z1,subOnFace);
      
      
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=onFace[2];
      subOnFace[3]=0;
      subOnFace[4]=0;
      subOnFace[5]=onFace[5];
      this->RefineBlock(blockId, level, output, input2, x0,x1,y0,y1,z2,z3,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnFace[2]=onFace[2];
//      subOnFace[3]=0;
      this->RefineBlock(blockId, level, output, input2, x2,x3,y0,y1,z2,z3,subOnFace);
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=0;
      subOnFace[3]=onFace[3];
      this->RefineBlock(blockId, level, output, input2, x0,x1,y2,y3,z2,z3,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnFace[2]=0;
//      subOnFace[3]=onFace[3];
      this->RefineBlock(blockId, level, output, input2, x2,x3,y2,y3,z2,z3,subOnFace);
      }
    else 
      {
      if (this->BlockCount >= this->StartBlock
          && this->BlockCount <= this->EndBlock)
        {
       
       vtkRectilinearGrid *grid=vtkRectilinearGrid::New();
       this->AppedDataSetToLevel(output, level, ext, grid);
       grid->Delete();
       this->SetRBlockInfo(grid, level, ext,onFace);
         
        this->Levels->InsertValue(blockId, level);
        ++blockId;
        }
      ++this->BlockCount;
      }
    
}

//----------------------------------------------------------------------------
void vtkDrillHolesBoundingBoxRefinement::CellExtentToBounds(int level,
                                                int ext[6],
                                                double bds[6])
{
  int spacingFactor = 1;
  double spacing[3];
  spacingFactor = spacingFactor << level;
  
  spacing[0] = this->TopLevelSpacing[0] / (double)(spacingFactor);
  spacing[1] = this->TopLevelSpacing[1] / (double)(spacingFactor);
  spacing[2] = this->TopLevelSpacing[2] / (double)(spacingFactor); //we get at that moment le space correct en passant pas le level

  bds[0] = this->TopLevelOrigin[0] + (double)(ext[0]) * spacing[0];
  bds[1] = this->TopLevelOrigin[0] + (double)(ext[1]+1) * spacing[0];
  bds[2] = this->TopLevelOrigin[1] + (double)(ext[2]) * spacing[1];
  bds[3] = this->TopLevelOrigin[1] + (double)(ext[3]+1) * spacing[1];
  bds[4] = this->TopLevelOrigin[2] + (double)(ext[4]) * spacing[2];
  bds[5] = this->TopLevelOrigin[2] + (double)(ext[5]+1) * spacing[2];
}


//----------------------------------------------------------------------------
// il ajoute ici le niveau de raffinement le plus grand
unsigned int vtkDrillHolesBoundingBoxRefinement::AppedDataSetToLevel(
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
  
  return index;
}

//----------------------------------------------------------------------------
void vtkDrillHolesBoundingBoxRefinement::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Dimensions: " << this->Dimensions << endl;
  os << indent << "MaximumLevel: " << this->MaximumLevel << endl;
  os << indent << "MinimumLevel: " << this->MinimumLevel << endl;
  os << indent << "Asymetric: " << this->Asymetric << endl;
  os<< endl;
 // os << indent << "TimeStep: " << this->TimeStep << endl;
 // os << indent << "TimeStepRange: " 
//     << this->TimeStepRange[0] << " " << this->TimeStepRange[1]
//     << endl;
}

