/*=========================================================================

Program:   Visualization Toolkit
Module:    $ vtkConvertPointsToBlocks $
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

	this->XEntry = NULL;
	this->YEntry = NULL;
	this->ZEntry = NULL;

	this->SizeCX= 5;
	this->SizeCY= 5;
	this->SizeCZ= 5;

}

vtkConvertPointsToBlocks::~vtkConvertPointsToBlocks()
{
}
int vtkConvertPointsToBlocks::FillInputPortInformation(int port, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
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

	// get the input and ouptut
	vtkDataSet *input = vtkDataSet::SafeDownCast( inInfo->Get( vtkDataObject::DATA_OBJECT() ) );
	output->ShallowCopy(ConvertsToDefaultGeometry(input));

	return 1;
}

int vtkConvertPointsToBlocks::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	return 1;
}

vtkUnstructuredGrid* vtkConvertPointsToBlocks::ConvertsToDefaultGeometry(vtkDataSet* input)
{
	vtkUnstructuredGrid* grid= vtkUnstructuredGrid::New();
	vtkUnstructuredGrid* temp= vtkUnstructuredGrid::New();
	vtkPoints *points = vtkPoints::New();

	int index = 0;
	int nbPoints= input->GetNumberOfPoints();
	vtkIdList *pt = vtkIdList::New();
	double center[3];
	double bounds[6];
	vtkIdType pts[8];	
	vtkIdType idCell;
	double tempValue=0;


	bool useXArray = strcmp(this->XEntry, "Property") == 0;
	bool useYArray = strcmp(this->YEntry, "Property") == 0;
	bool useZArray = strcmp(this->ZEntry, "Property") == 0;

	//set the size if default is needed
	vtkDoubleArray* XINCArray;//size array on x dimension
	vtkDoubleArray* YINCArray;//size array on y dimension
	vtkDoubleArray* ZINCArray;//size array on z dimension
	if(useXArray)
	{
		XINCArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->XINC));
		if((XINCArray==NULL))
		{
			vtkErrorMacro("Coudn't read X Property");
			return 0;
		}	
	}
	if(useYArray)
	{
		YINCArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->YINC));
		if((YINCArray==NULL))
		{
			vtkErrorMacro("Coudn't read Y Property");
			return 0;
		}	
	}
	if(useZArray)
	{
		ZINCArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->ZINC));
		if((ZINCArray==NULL))
		{
			vtkErrorMacro("Coudn't read Z Property");
			return 0;
		}	
	}

	double dx, dy, dz;
	for(int i=0; i< nbPoints; i++)
	{			
		input->GetPoint(i, center);

		//set the size of this block
		if(useXArray)
			dx = XINCArray->GetValue(i);
		else
			dx = SizeCX;
		if(useYArray)
			dy = YINCArray->GetValue(i);
		else
			dy = SizeCY;
		if(useZArray)
			dz = ZINCArray->GetValue(i);
		else
			dz = SizeCZ;

		bounds[0] = center[0] - dx/2; bounds[1]= center[0] + dx/2;	
		bounds[2] = center[1] - dy/2; bounds[3]= center[1] + dy/2; 		
		bounds[4] = center[2] - dz/2; bounds[5]= center[2] + dz/2; 

		//the order is important here to add the points
		pts[0] = points->InsertNextPoint(bounds[0],bounds[2],bounds[4]);
		pts[1] = points->InsertNextPoint(bounds[1],bounds[2],bounds[4]);
		pts[2] = points->InsertNextPoint(bounds[1],bounds[3],bounds[4]);	
		pts[3] = points->InsertNextPoint(bounds[0],bounds[3],bounds[4]);

		pts[4] = points->InsertNextPoint(bounds[0],bounds[2],bounds[5]);	
		pts[5] = points->InsertNextPoint(bounds[1],bounds[2],bounds[5]);
		pts[6] = points->InsertNextPoint(bounds[1],bounds[3],bounds[5]);
		pts[7] = points->InsertNextPoint(bounds[0],bounds[3],bounds[5]);	

		idCell = temp->InsertNextCell(VTK_HEXAHEDRON,8,pts);	

	}		
	temp->SetPoints(points);	

	//clean first 
	vtkCleanUnstructuredGrid *clean  = vtkCleanUnstructuredGrid::New();
	clean->SetInput( temp );
	clean->Update();
	temp->Delete();

	//copy over temp to output
	grid->ShallowCopy( clean->GetOutput() );
	clean->Delete();	
	grid->GetCellData()->PassData(input->GetPointData());
	
	return grid;
}


void vtkConvertPointsToBlocks::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

