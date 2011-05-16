/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkMapAttributes.cxx $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      June 1, 2008
  Version:   1.1



  =========================================================================*/
#include "vtkMapAttributes.h"
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
#include "vtkPolyData.h"
#include "vtkDataSet.h"

#include "vtkSmartPointer.h"
#include "vtkPolyStruct.h"
#include "vtkMapAttributesToGrid.h"
#include "vtkMapAttributesFromGrid.h"


vtkCxxRevisionMacro ( vtkMapAttributes, "$Revision: 0.5 $" );
vtkStandardNewMacro ( vtkMapAttributes );

//----------------------------------------------------------------------------
vtkMapAttributes::vtkMapAttributes()
  {
  const int ports = 2;

  this->SetNumberOfInputPorts ( ports );
  this->XCubes = 10;
  this->YCubes = 10;
  this->ZCubes = 10;
  this->SelectedComponent     = 0;
  this->AttributeMode               = 0;
  this->ComponentMode          = VTK_COMPONENT_MODE_USE_SELECTED;

  }
  
vtkMapAttributes::~vtkMapAttributes()
{      

}
//----------------------------------------------------------------------------
void vtkMapAttributes::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
  {
  this->SetInputConnection ( 1, algOutput );
  }

//----------------------------------------------------------------------------
vtkDataObject *vtkMapAttributes::GetSource()
  {
  if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
    {
    return NULL;
    }

  return this->GetExecutive()->GetInputData ( 1, 0 );
  }

//----------------------------------------------------------------------------
int vtkMapAttributes::RequestInformation ( vtkInformation *vtkNotUsed ( request ), vtkInformationVector **inputVector, vtkInformationVector *outputVector )
  {
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  vtkDataArray *inScalars = this->GetInputArrayToProcess(0, inputVector);
  vtkDataSet *input = vtkDataSet::SafeDownCast (
                          inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  if (!inScalars)
    {
    //if their is no property currently selected we want to autoselect the first point property, and if  there 
    //is no point properties use the first cell proptery
    
    //if both of those fail the algorithm will throw a vtkErrorMacro in RequestData since it needs a propety to map
    const char *name;
    if (input->GetPointData()->GetNumberOfArrays() > 0)
      {
      name = input->GetPointData()->GetArrayName(0);
      this->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_POINTS,name);
      }
    else if (input->GetCellData()->GetNumberOfArrays() > 0)
      {
      name = input->GetCellData()->GetArrayName(0);
      this->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,name);
      }
    name = NULL;
  }
  return 1;
  }
//----------------------------------------------------------------------------
int vtkMapAttributes::RequestData ( vtkInformation *vtkNotUsed ( request ), vtkInformationVector **inputVector, vtkInformationVector *outputVector )
  {
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject ( 0 );
  vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast (
                          inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  vtkPolyData *source = vtkPolyData::SafeDownCast (
                        sourceInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  vtkPolyData *output = vtkPolyData::SafeDownCast (
                         outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  //make sure input and source exist before processing
  if (!input || !source)
    {
    vtkErrorMacro("Source or Input was NULL");
    return 1;
    }
  vtkDataArray *inScalars = this->GetInputArrayToProcess(0, inputVector);
  if (!inScalars)
    {
    vtkErrorMacro(<<"No property was selected");
    return 1;
    }

  //the first step is that we have to create the image data for this program
  //so used PolyToStruct to do that
  vtkSmartPointer<vtkPolyStruct> imageGrid = vtkSmartPointer<vtkPolyStruct>::New();
  
  imageGrid->SetInput(source);
  imageGrid->SetXCubes(this->XCubes);
  imageGrid->SetYCubes(this->YCubes);
  imageGrid->SetZCubes(this->ZCubes);
  imageGrid->Update();
  
  
  //now that we have the image grid, we need to transfer the property from the Input
  //onto the grid
  vtkSmartPointer<vtkMapAttributesToGrid> mapToGrid = vtkSmartPointer<vtkMapAttributesToGrid>::New();
  
  mapToGrid->SetInput(0,input);
  mapToGrid->SetInput(1,imageGrid->GetOutput());
  
  mapToGrid->SetInputArrayToProcess(0, 0,0,vtkDataObject::FIELD_ASSOCIATION_POINTS, inScalars->GetName());
  //need to pass along the selected Property
  mapToGrid->Update();

  if (!mapToGrid->GetOutput())
    {
    vtkErrorMacro(<<"Map to Grid failed");
    return 1;
    }
  

  //now map mapToGrid onto the polyData Output
  vtkSmartPointer<vtkMapAttributesFromGrid> mapFromGrid = vtkSmartPointer<vtkMapAttributesFromGrid>::New();
  mapFromGrid->SetInput(0,mapToGrid->GetOutput());
  mapFromGrid->SetInput(1, source);
  mapFromGrid->SetInputArrayToProcess(0, 0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS, inScalars->GetName());
  //cross your fingers
  mapFromGrid->Update();
  if (!mapFromGrid->GetOutput())
    {
    vtkErrorMacro(<<"Map from Grid failed");
    return 1;
    }

  //now we have to set output to mapFromGrid
  //output->ShallowCopy(mapFromGrid->GetOutput());
  output->CopyStructure(mapFromGrid->GetOutput());
  output->GetPointData()->PassData ( mapFromGrid->GetOutput()->GetPointData() );
  output->GetCellData()->PassData ( mapFromGrid->GetOutput()->GetCellData() );
  
  return 1;
  }


//----------------------------------------------------------------------------
int vtkMapAttributes::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 1 );
  return 1;
  }
  
//----------------------------------------------------------------------------
int vtkMapAttributes::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  return 1;
}
  
// Return the method for manipulating scalar data as a string.
const char *vtkMapAttributes::GetAttributeModeAsString(void)
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
const char *vtkMapAttributes::GetComponentModeAsString(void)
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
void vtkMapAttributes::PrintSelf ( ostream& os, vtkIndent indent )
  {
  vtkDataObject *source = this->GetSource();

  this->Superclass::PrintSelf ( os, indent );
  os << indent << "Source: " << source << "\n";
  }
