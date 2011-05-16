/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkProximityThreshold.cxx $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      June 16, 2008
  Version:   1.1



  =========================================================================*/
#include "vtkProximityThreshold.h"
#include "vtkUnstructuredGrid.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkDoubleArray.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkThreshold.h"
#include "vtkMath.h"

vtkCxxRevisionMacro ( vtkProximityThreshold, "$Revision: 0.1 $" );
vtkStandardNewMacro ( vtkProximityThreshold );

#define MODE_LOWER 0

//----------------------------------------------------------------------------
vtkProximityThreshold::vtkProximityThreshold()
  {
  const int ports = 2;

  this->SetNumberOfInputPorts ( ports );
  this->Invert   = 0;
  this->Distance = 0.0;
  }
  
vtkProximityThreshold::~vtkProximityThreshold()
{      

}

//----------------------------------------------------------------------------
void vtkProximityThreshold::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
  {
  this->SetInputConnection ( 1, algOutput );
  }


//----------------------------------------------------------------------------
int vtkProximityThreshold::RequestData ( vtkInformation *vtkNotUsed ( request ), vtkInformationVector **inputVector, vtkInformationVector *outputVector )
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
  
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast (
                         outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  //make sure the objects are of the right type
  if ( !source )
    {
    vtkErrorMacro(<<"The Source is null");
    return 1;
    }
  else if ( !input)
    {
    vtkErrorMacro(<<"The Input is null");
    return 1;
    }
    
  //defines  
  const int XYZ = 3;
  const int CUBE_ARRAY_SIZE = 8;
  
  //point to store in the loop
  double point[XYZ];
  
  //point found in the source
  double foundPoint[XYZ];
  
  //findCell vars
  int subId = 0;
  
  //vars for the counting on the single property
  //for now will just be occurance counting
  int numberOfPoints = source->GetNumberOfPoints();
  double *distance = new double[numberOfPoints];
 
  //set them all to 0
  for (int i=0; i < numberOfPoints; i++)
    {
    distance[i]=-1;    
    }
  
  for ( int i = 0; i < source->GetNumberOfPoints(); i++ )
    {
      source->GetPoint( i, point );
      
      //find the nearest point in the source, to the point in the input
      vtkIdType pointId = input->FindPoint( point );
      if (pointId != -1)
        {
        //calculate out the distance between the two points
        input->GetPoint( pointId, foundPoint );
        
        //store the distance
        distance[i]=sqrt( vtkMath::Distance2BetweenPoints( point, foundPoint ) );
        }
    }
    
  //convert vars
  vtkDoubleArray *distanceProp = vtkDoubleArray::New();
  distanceProp->SetArray(distance,numberOfPoints,0); //we use 0 so the arrary will be deleted by paraview
  distanceProp->SetName("distance");
 
  
  //make a temp object to threshold with the distance value
  vtkDataSet *distanceObject = vtkDataSet::SafeDownCast ( source );  
  
  distanceObject->GetPointData()->AddArray( distanceProp );
  
  
  if ( this->Distance > 0 )
    {
    //now we create a threshold of the distance object
    vtkThreshold *thresh = vtkThreshold::New();
    thresh->SetInput(distanceObject);
    
    if (this->AllScalars == 1)
      {
      thresh->AllScalarsOn();
      }
    else
      {
      thresh->AllScalarsOff();
      }
      
    thresh->SetInputArrayToProcess(0, 0,0,vtkDataObject::FIELD_ASSOCIATION_POINTS, distanceProp->GetName());
    
    if (this->Invert == MODE_LOWER)
      {
      thresh->ThresholdByLower( this->Distance );
      }  
    else
      {
      thresh->ThresholdByUpper( this->Distance );
      }
      
    thresh->Update();
      
    
    output->ShallowCopy ( thresh->GetOutput() );
    thresh->Delete();    
    }
  else
    {
    output->DeepCopy( distanceObject );
    }  
  distanceProp->Delete();
  return 1;
  }
//----------------------------------------------------------------------------
int vtkProximityThreshold::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 1 );
  return 1;
  }
//----------------------------------------------------------------------------
char* vtkProximityThreshold::CreateName(vtkDataArray* propertyArray, char* name)
  {
  //c style brings up a million warnings, but at least it works :/
  char *temp = new char[256];
  strcpy(temp, propertyArray->GetName());
  strcat(temp, "-");
  strcat(temp, name);
  return temp;
  }

  

//----------------------------------------------------------------------------
void vtkProximityThreshold::PrintSelf ( ostream& os, vtkIndent indent )
  {
  this->Superclass::PrintSelf ( os, indent );  
  }


#undef MODE_LOWER