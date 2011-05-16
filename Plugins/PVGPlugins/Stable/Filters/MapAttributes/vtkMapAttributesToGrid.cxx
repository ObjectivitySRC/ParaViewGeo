/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkMapAttributesToGrid.cxx $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      June 1, 2008
  Version:   1.1



  =========================================================================*/
#include "vtkMapAttributesToGrid.h"
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
#include "vtkStdString.h"

vtkCxxRevisionMacro ( vtkMapAttributesToGrid, "$Revision: 0.5 $" );
vtkStandardNewMacro ( vtkMapAttributesToGrid );

//----------------------------------------------------------------------------
vtkMapAttributesToGrid::vtkMapAttributesToGrid()
  {
  const int ports = 2;

  this->SetNumberOfInputPorts ( ports );
  this->SelectedComponent      = 0;
  this->AttributeMode          = 0;
  this->ComponentMode          = VTK_COMPONENT_MODE_USE_SELECTED;

  }
  
vtkMapAttributesToGrid::~vtkMapAttributesToGrid()
{      

}
//----------------------------------------------------------------------------
void vtkMapAttributesToGrid::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
  {
  this->SetInputConnection ( 1, algOutput );
  }

//----------------------------------------------------------------------------
vtkDataObject *vtkMapAttributesToGrid::GetSource()
  {
  if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
    {
    return NULL;
    }

  return this->GetExecutive()->GetInputData ( 1, 0 );
  }

//----------------------------------------------------------------------------
int vtkMapAttributesToGrid::RequestInformation(vtkInformation *vtkNotUsed(request),vtkInformationVector **inputVector,vtkInformationVector *outputVector)   
{
  
  return 1;
   
}

//----------------------------------------------------------------------------
int vtkMapAttributesToGrid::RequestData ( vtkInformation *vtkNotUsed ( request ), vtkInformationVector **inputVector, vtkInformationVector *outputVector )
  {
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject ( 0 );
  vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast (
                          inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  vtkImageData *source = vtkImageData::SafeDownCast (
                        sourceInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  vtkImageData *output = vtkImageData::SafeDownCast (
                         outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  //make sure the objects are of the right type
  if ( !source || !input )
    {
    vtkErrorMacro(<<"The source or the Input is null");
    return 1;
    }
  
  //needed for grabbing density
  vtkDataArray *inScalars = this->GetInputArrayToProcess(0,inputVector);  
  if (!inScalars)
    {
    vtkErrorMacro(<<"No scalar data to Map to Grid");
    return 1;
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
  int numberOfCells = source->GetNumberOfCells();
  double *sum = new double[numberOfCells];
  int *counts = new int[numberOfCells];
  //set them all to 0
  for (int i=0; i < numberOfCells; i++)
    {
    sum[i]=0;
    counts[i]=0;    
    }
  
  for ( int i = 0; i < input->GetNumberOfPoints(); i++ )
    {
      input->GetPoint( i, point );
      
      vtkIdType cellId = source->FindCell ( point, NULL, 0, 0.0, subId, pcoords, weights );
      if (cellId != -1)
        {
        
        double value = inScalars->GetComponent(i,this->SelectedComponent);
        counts[cellId]++;
        sum[cellId] += value;
        }
    }
    
  //convert vars
  vtkIntArray *countProp = vtkIntArray::New();
  countProp->SetArray(counts,numberOfCells,0); //we use 0 so the arrary will be deleted by the vtkIntArray
  countProp->SetName(this->CreateName(inScalars,"count"));

  vtkDoubleArray *sumProp = vtkDoubleArray::New();
  sumProp->SetArray(sum,numberOfCells,0); //we use 0 so the arrary will be deleted by vtkDoubleArray
  sumProp->SetName(this->CreateName(inScalars,"sum"));

  vtkDoubleArray *mean = vtkDoubleArray::New();
	mean->SetName(inScalars->GetName());  
  mean->SetNumberOfComponents(1);
  
  mean->SetNumberOfTuples(numberOfCells);
  for (int i=0; i < numberOfCells; i++)
    {
    double value=0;
    if (counts[i] > 0)
      {
      value = double(sum[i])/double(counts[i]);
      }
    mean->SetValue(i,value);
    }
    
    
  //create the output data as the same shape as the source data
	
	output->SetDimensions(source->GetDimensions());
	output->SetOrigin(source->GetOrigin());
	output->SetSpacing(source->GetSpacing());
    
	int *ext = source->GetExtent();
	output->SetWholeExtent(ext);
  output->SetExtent(ext);
  output->SetUpdateExtent(ext);
  output->AllocateScalars();
	
	output->CopyAttributes( source );
	vtkCellData *cd = output->GetCellData();
	
  cd->AddArray ( mean );
  cd->SetActiveScalars(mean->GetName());
  cd->AddArray ( countProp );
  cd->AddArray ( sumProp );
	
	
	mean->Delete();
	countProp->Delete();
	sumProp->Delete();

  return 1;
  }

//----------------------------------------------------------------------------
char* vtkMapAttributesToGrid::CreateName(vtkDataArray* propertyArray, char* name)
  {
  //c style brings up a million warnings, but at least it works :/
  char *temp = new char[256];
  strcpy(temp, propertyArray->GetName());
  strcat(temp, "-");
  strcat(temp, name);
  return temp;
  }

//----------------------------------------------------------------------------
int vtkMapAttributesToGrid::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData", 1 );
  return 1;
  }
  
//----------------------------------------------------------------------------
int vtkMapAttributesToGrid::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
  return 1;
}
  
// Return the method for manipulating scalar data as a string.
const char *vtkMapAttributesToGrid::GetAttributeModeAsString(void)
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
const char *vtkMapAttributesToGrid::GetComponentModeAsString(void)
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
void vtkMapAttributesToGrid::PrintSelf ( ostream& os, vtkIndent indent )
  {
  vtkDataObject *source = this->GetSource();

  this->Superclass::PrintSelf ( os, indent );
  os << indent << "Source: " << source << "\n";
  }
