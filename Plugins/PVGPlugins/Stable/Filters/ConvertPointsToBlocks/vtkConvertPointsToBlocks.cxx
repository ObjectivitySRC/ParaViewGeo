/*=========================================================================

   Program:   Visualization Toolkit
  Module:    $ vtkTopOfCell.h $
  Author:     Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:      May 7 2009
  Version:   0.1
=========================================================================*/



#include "vtkConvertPointsToBlocks.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkCleanUnstructuredGrid.h"
#include "vtkPoints.h"
#include "vtkCell.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPointLocator.h"
#include "vtkCellLocator.h"
#include "vtkIdList.h"
#include "vtkDoubleArray.h"

vtkCxxRevisionMacro(vtkConvertPointsToBlocks, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkConvertPointsToBlocks);

vtkConvertPointsToBlocks::vtkConvertPointsToBlocks()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
	
	this->XINC= NULL;
	this->YINC= NULL;
	this->ZINC= NULL;

	this->UseSizes=1; //use default values sizes from datamine
  this->SizeCX= 5;
  this->SizeCY= 5;
  this->SizeCZ= 5;

}

vtkConvertPointsToBlocks::~vtkConvertPointsToBlocks()
{
}
int vtkConvertPointsToBlocks::FillInputPortInformation(int port, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

int vtkConvertPointsToBlocks::FillOutputPortInformation(int port, vtkInformation *info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid");
  return 1;
}

int vtkConvertPointsToBlocks::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	  // get the info objects
	 vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );

	 vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );
	 vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast (
							 outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	 this->SizeCX= 5;
	 this->SizeCY= 5;
	 this->SizeCZ= 5;

	 // get the input and ouptut
	 vtkPolyData *input = vtkPolyData::SafeDownCast( inInfo->Get( vtkDataObject::DATA_OBJECT() ) );
     output->ShallowCopy(ConvertsToDefaultGeometry(input));

  return 1;
}

int vtkConvertPointsToBlocks::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  return 1;
}

vtkUnstructuredGrid* vtkConvertPointsToBlocks::ConvertsToDefaultGeometry(vtkPolyData* polys)
{

	vtkUnstructuredGrid* grid= vtkUnstructuredGrid::New();
	vtkUnstructuredGrid* temp= vtkUnstructuredGrid::New();
	vtkPoints *points = vtkPoints::New();
		
	int index = 0;
	int nbPoints= polys->GetNumberOfPoints();
	vtkIdList *pt = vtkIdList::New();
	double center[3];
	double bounds[6];
	vtkIdType pts[8];	
	vtkIdType idCell;
	double tempValue=0;

  
  //set the size if default is needed
	vtkDoubleArray* XINCArray;//size array on x dimension
	vtkDoubleArray* YINCArray;//size array on y dimension
	vtkDoubleArray* ZINCArray;//size array on z dimension
	if(this->UseSizes==1)
	{
		//check if the size properties are on the datamine block model
		XINCArray= vtkDoubleArray::New();
		YINCArray= vtkDoubleArray::New();
		ZINCArray= vtkDoubleArray::New();

		XINCArray= vtkDoubleArray::SafeDownCast(polys->GetPointData()->GetArray(this->XINC));
		YINCArray= vtkDoubleArray::SafeDownCast(polys->GetPointData()->GetArray(this->YINC));
		ZINCArray= vtkDoubleArray::SafeDownCast(polys->GetPointData()->GetArray(this->ZINC));

		if((XINCArray==NULL)||(YINCArray==NULL)||(ZINCArray==NULL))
		{
			vtkErrorMacro("Size arrays are not set on the block model");
		}	
	}
	//use the geometry set by the user to compute the stops now
	//convert each point by region to a cell of grid
	for(int i=0; i< nbPoints; i++)
	{			
		pt = polys->GetCell(i)->GetPointIds();
		polys->GetPoint(pt->GetId(0),center);

		//set the size of this block
		if(this->UseSizes==1)
		{
			SizeCX= XINCArray->GetTuple1(pt->GetId(0));
			SizeCY= YINCArray->GetTuple1(pt->GetId(0));
			SizeCZ= ZINCArray->GetTuple1(pt->GetId(0));
		}

		bounds[0]= center[0] - SizeCX/2; bounds[1]= center[0] + SizeCX/2;	
		bounds[2]= center[1] - SizeCY/2; bounds[3]= center[1] + SizeCY/2; 		
		bounds[4]= center[2] - SizeCZ/2; bounds[5]= center[2] + SizeCZ/2; 
		
		//the order is important here to add the points
		pts[0] = points->InsertNextPoint(bounds[0],bounds[2],bounds[4]);
		pts[1] = points->InsertNextPoint(bounds[1],bounds[2],bounds[4]);
		pts[2] = points->InsertNextPoint(bounds[1],bounds[3],bounds[4]);	
		pts[3] = points->InsertNextPoint(bounds[0],bounds[3],bounds[4]);

		pts[4] = points->InsertNextPoint(bounds[0],bounds[2],bounds[5]);	
		pts[5] = points->InsertNextPoint(bounds[1],bounds[2],bounds[5]);
		pts[6] = points->InsertNextPoint(bounds[1],bounds[3],bounds[5]);
		pts[7] = points->InsertNextPoint(bounds[0],bounds[3],bounds[5]);	
		
		idCell= temp->InsertNextCell(VTK_HEXAHEDRON,8,pts);	
		
	}		
	temp->SetPoints(points);	
	points->Delete();

	//clean first 
	vtkCleanUnstructuredGrid *clean  = vtkCleanUnstructuredGrid::New();
	clean->SetInput( temp );
	clean->Update();
	temp->Delete();

	//copy over temp to output
	grid->ShallowCopy( clean->GetOutput() );
	clean->Delete();	
  grid->GetCellData()->PassData(polys->GetPointData());
  if(this->UseSizes==1)
	{
		XINCArray->Delete();
		YINCArray->Delete();
		ZINCArray->Delete();
	}
	return grid;
}


void vtkConvertPointsToBlocks::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

