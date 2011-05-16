

#include "vtkOBBtoGrid.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkMath.h"

#include "vtkCoordinateSystemMapper.h"


vtkCxxRevisionMacro(vtkOBBtoGrid, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkOBBtoGrid);

vtkOBBtoGrid::vtkOBBtoGrid()
{
	this->GridSize[0] = 5;
	this->GridSize[1] = 5;
	this->GridSize[2] = 5;
}

vtkOBBtoGrid::~vtkOBBtoGrid()
{
}


int vtkOBBtoGrid::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData* input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkStructuredGrid* output = vtkStructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	if(this->GridSize[0] <= 0 ||
		this->GridSize[1] <= 0 ||
		this->GridSize[2] <= 0)
	{
		vtkErrorMacro("the grid size must be greater than 0");
		return 0;
	}

	int numberOfPoints = input->GetNumberOfPoints();

	vtkCellArray *inCells = input->GetPolys();
	vtkPoints* inPoints = input->GetPoints();


	// To draw the grid we will first find a coordinate system mapper 
	// to map between the world coordinate system and the bounding box
	// local coordinate system. Then we will find the bounds of the bounding
	// box in the local coordinate system compute
	// axis aligned grid in the local coordinate system. 
	// Then we will use the coordinate system mapper
	// to map the grid to the global coordinate system

	double xw[3] = {1.0, 0.0, 0.0};
	double yw[3] = {0.0, 1.0, 0.0};
	double zw[3] = {0.0, 0.0, 1.0};
	double cw[3] = {0.0, 0.0, 0.0};

	double xv[3];
	double yv[3];
	double zv[3];
	double cv[3];

	vtkIdType npts;
	vtkIdType* pts;
	double p[3];

	// the following code is used to locate the relative axis to the bounding box
	inCells->InitTraversal();
	inCells->GetNextCell(npts, pts);
	if(npts!=4)
	{
		vtkErrorMacro("this filter work only on oriented bounding box with cell size = 4");
		return 0;
	}

	inPoints->GetPoint(pts[0], cv);
	inPoints->GetPoint(pts[1], p);

	xv[0] = p[0] - cv[0];
	xv[1] = p[1] - cv[1];
	xv[2] = p[2] - cv[2];

	vtkMath::Normalize(xv);

	inPoints->GetPoint(pts[3], p);
	yv[0] = p[0] - cv[0];
	yv[1] = p[1] - cv[1];
	yv[2] = p[2] - cv[2];

	vtkMath::Normalize(yv);

	vtkMath::Cross(xv, yv, zv);

	vtkCoordinateSystemMapper *cmapper = vtkCoordinateSystemMapper::New();
	cmapper->setCoordSystemA(cw, xw, yw, zw);
	cmapper->setCoordSystemB(cv, xv, yv, zv);
	cmapper->computeMappingMatrices();

	inPoints->GetPoint(0, p);
	cmapper->mapPointA_B(p,p);
	double xmin = p[0];
	double xmax = p[0];
	double ymin = p[1];
	double ymax = p[1];
	double zmin = p[2];
	double zmax = p[2];

	for(vtkIdType i=1; i<numberOfPoints; i++)
	{
		inPoints->GetPoint(i, p);
		cmapper->mapPointA_B(p,p);

		if(xmin > p[0])
		{
			xmin = p[0];
		}
		else if(xmax < p[0])
		{
			xmax = p[0];
		}
		if(ymin > p[1])
		{
			ymin = p[1];
		}
		else if(ymax < p[1])
		{
			ymax = p[1];
		}
		if(zmin > p[2])
		{
			zmin = p[2];
		}
		else if(zmax < p[2])
		{
			zmax = p[2];
		}
	}





	double xSpacing = (xmax - xmin) / this->GridSize[0];
	double ySpacing = (ymax - ymin) / this->GridSize[1];
	double zSpacing = (zmax - zmin) / this->GridSize[2];

	p[0] = xmin;
	p[1] = ymin;
	p[2] = zmin;

	double pt[3];

	vtkPoints* outPoints = vtkPoints::New();

	for(vtkIdType i=0; i<this->GridSize[2]+1; i++)
	{
		for(vtkIdType j=0; j<this->GridSize[1]+1; j++)
		{
			for(vtkIdType k=0; k<this->GridSize[0]+1; k++)
			{
				cmapper->mapPointB_A(p,pt);
				outPoints->InsertNextPoint(pt);
				p[0] += xSpacing;
			}
			p[0] = xmin;
			p[1] += ySpacing;
		}
		p[1] = ymin; 
		p[2] += zSpacing;
	}

	cmapper->Delete();

	int extents[6];
	extents[0] = 0;
	extents[1] = this->GridSize[0];
	extents[2] = 0;
	extents[3] = this->GridSize[1];
	extents[4] = 0;
	extents[5] = this->GridSize[2];

	//set all the information for the output
	// Set the extents of the space
	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extents, 6);
	//output->SetExtent(extents);
	output->SetDimensions(this->GridSize[0]+1, this->GridSize[1]+1, this->GridSize[2]+1);
	output->SetPoints( outPoints );
	

	outPoints->Delete();

  return 1;
}




//----------------------------------------------------------------------------
int vtkOBBtoGrid::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}