/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkMapObjectAttributes.cxx $
  Author:    Robert Maynard
	Modified:  Matthew Livingstone
  MIRARCO, Laurentian University
  Date:      June 1, 2008
	Date Modified: Feb 2009
  Version:   1.1

	- New version allows for mapping of properties from any type of dataset
	  to any other type of dataset

  =========================================================================*/
#include "vtkMapObjectAttributes.h"
#include "vtkCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkImageData.h"
#include "vtkDataSet.h"
#include "vtkMath.h"

#define VTK_CELL_DATA 0
#define VTK_POINT_DATA 1



vtkCxxRevisionMacro ( vtkMapObjectAttributes, "$Revision: 0.5 $" );
vtkStandardNewMacro ( vtkMapObjectAttributes );

//----------------------------------------------------------------------------
vtkMapObjectAttributes::vtkMapObjectAttributes()
  {
  const int ports = 2;

  this->SetNumberOfInputPorts ( ports );
  this->SelectedComponent = -1;
  this->AttributeMode = 1;
  this->ComponentMode = 2;

  }
  
vtkMapObjectAttributes::~vtkMapObjectAttributes()
{      

}
//----------------------------------------------------------------------------
void vtkMapObjectAttributes::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
  {
  this->SetInputConnection ( 1, algOutput );
  }

//----------------------------------------------------------------------------
vtkDataObject *vtkMapObjectAttributes::GetSource()
  {
  if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
    {
    return NULL;
    }

  return this->GetExecutive()->GetInputData ( 1, 0 );
  }

//----------------------------------------------------------------------------
int vtkMapObjectAttributes::RequestDataObject(
  vtkInformation*, 
  vtkInformationVector** inputVector , 
  vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[1]->GetInformationObject(0);
  if (!inInfo)
    {
    return 0;
    }
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  if (input)
    {
    this->GetOutputPortInformation(0)->Set(
      vtkDataObject::DATA_EXTENT_TYPE(), input->GetExtentType());

    // for each output
    for(int i=0; i < this->GetNumberOfOutputPorts(); ++i)
      {
      vtkInformation* info = outputVector->GetInformationObject(i);
      vtkDataSet *output = vtkDataSet::SafeDownCast(
        info->Get(vtkDataObject::DATA_OBJECT()));
    
      if (!output || !output->IsA(input->GetClassName())) 
        {
        vtkDataSet* newOutput = input->NewInstance();
        newOutput->SetPipelineInformation(info);
        newOutput->Delete();
        }
      }
    return 1;
    }
  return 0;
}


//----------------------------------------------------------------------------
int vtkMapObjectAttributes::RequestData ( vtkInformation *vtkNotUsed ( request ),
																					 vtkInformationVector **inputVector,
																					 vtkInformationVector *outputVector )
  {
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject ( 0 );
  vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast (
                          inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  vtkDataSet *source = vtkDataSet::SafeDownCast (
                        sourceInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  
  vtkDataSet *output = vtkDataSet::SafeDownCast (
                         outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  //make sure the objects are of the right type
  if ( !source || !input)
    {
    vtkErrorMacro(<<"The source or the Input is null");
    return 1;
    }
  
  //needed for grabbing property
  vtkDataArray *inScalars = this->GetInputArrayToProcess(0,inputVector);
  if (!inScalars)
    {
    vtkErrorMacro(<<"No scalar data to Map from Grid");
    return 1;
    }

	//we need to figure out if we are in point or cell mode
	int mode = VTK_POINT_DATA;
	if ( inScalars->GetNumberOfTuples() == input->GetNumberOfCells() )
		{
		//we have cell, change the mode
		mode = VTK_CELL_DATA;
		}

  //defines  
  const int XYZ = 3;
  const int CUBE_ARRAY_SIZE = 8;
  
  //point to store in the loop
  double point[XYZ];
  
  //findCell vars
  double pcoords[XYZ];
  double weights[CUBE_ARRAY_SIZE];
  int subId = 0;
  
  //vars for the counting on the single property
  //for now will just be occurance counting
  int numberOfPoints = source->GetNumberOfPoints();
  double *values = new double[numberOfPoints];

	double *inputBounds = new double[6];

	input->GetBounds(inputBounds);
	
	double value;
	for ( int i = 0; i < source->GetNumberOfPoints(); i++ )
    {
		value = -1;
		source->GetPoint( i,	point );
		if ( point[0] >= inputBounds[0] && point[0] <= inputBounds[1] &&
					point[1] >= inputBounds[2] && point[1] <= inputBounds[3] &&
					point[2] >= inputBounds[4] && point[2] <= inputBounds[5])
			{
			if ( mode == VTK_POINT_DATA )
				{
				vtkIdType pointId = input->FindPoint( point );
				if ( pointId != -1 )
					{
					// MLivingstone
					// Using location 0 in tuple as we only deal with scalar values
					value = inScalars->GetComponent(pointId,0);		
					}
				}
			else
				{
				//we need to find the value at a given cellId so we can map it
				vtkIdType cellId = input->FindCell ( point, NULL, 0, 0.0, subId, pcoords, weights );
				if ( cellId != -1 )
					{
					value = inScalars->GetComponent(cellId,0);	
					}
				}
			}
		values[i] = value;
		}

	
  //convert vars
  
  vtkDoubleArray *valuesProp = vtkDoubleArray::New();
  valuesProp->SetArray(values,numberOfPoints,0); //we use 0 so the arrary will be deleted by paraview
  valuesProp->SetName(inScalars->GetName());
  
  //create output
  //copy the input data to the output data, so we have the proper shape
  output->ShallowCopy( source );
  
  output->GetPointData()->AddArray ( valuesProp );
	output->GetPointData()->SetActiveScalars(valuesProp->GetName());

	valuesProp->Delete();
  
  return 1;
  }
  
// Return the method for manipulating scalar data as a string.
const char *vtkMapObjectAttributes::GetAttributeModeAsString(void)
{
  if ( this->AttributeMode == VTK_ATTRIBUTE_MODE_DEFAULT )
    {
    return "Default";
    }
  else if ( this->AttributeMode == VTK_ATTRIBUTE_MODE_USE_POINT_DATA )
    {
    return "UsePointData";
    }
  else 
    {
    return "UseCellData";
    }
}

// Return a string representation of the component mode
const char *vtkMapObjectAttributes::GetComponentModeAsString(void)
{
  if ( this->ComponentMode == VTK_COMPONENT_MODE_USE_SELECTED )
    {
    return "UseSelected";
    }
  else if ( this->ComponentMode == VTK_COMPONENT_MODE_USE_ANY )
    {
    return "UseAny";
    }
  else 
    {
    return "UseAll";
    }
}

//----------------------------------------------------------------------------
void vtkMapObjectAttributes::PrintSelf ( ostream& os, vtkIndent indent )
  {
  vtkDataObject *source = this->GetSource();

  this->Superclass::PrintSelf ( os, indent );
  os << indent << "Source: " << source << "\n";
  }

#undef VTK_CELL_DATA
#undef VTK_POINT_DATA