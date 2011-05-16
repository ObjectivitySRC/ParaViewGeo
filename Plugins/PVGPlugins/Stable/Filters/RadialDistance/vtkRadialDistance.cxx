/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkRadialDistance.cxx $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      August 29, 2008
  Version:   1



  =========================================================================*/
  
#include "vtkRadialDistance.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkMath.h"

vtkCxxRevisionMacro ( vtkRadialDistance, "$Revision: 1 $" );
vtkStandardNewMacro ( vtkRadialDistance );


//----------------------------------------------------------------------------
vtkRadialDistance::vtkRadialDistance()
  {
  this->SetNumberOfInputPorts( 1 );
  this->SetNumberOfOutputPorts( 1 );
  this->MeasurePoint[0]=0;  
  this->MeasurePoint[1]=0;  
  this->MeasurePoint[2]=0;  
  }
  
vtkRadialDistance::~vtkRadialDistance()
{      
}

//----------------------------------------------------------------------------
int vtkRadialDistance::RequestData ( vtkInformation *vtkNotUsed ( request ), vtkInformationVector **inputVector, vtkInformationVector *outputVector )
  {
  
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast (
                          inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  
  vtkDataSet *output = vtkDataSet::SafeDownCast (
                         outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  //make sure the objects are of the right type  
  if ( !input)
    {
    vtkErrorMacro(<<"The Input is null");
    return 1;
    }
    
  
  
  //point found in the source
  double point[3];
  
  //findCell vars
  int subId = 0;
  
  //size of the point properties
  int numberOfPoints = input->GetNumberOfPoints();
  
  //create the point property arrays
  vtkDoubleArray *radialDist = vtkDoubleArray::New();
  radialDist->SetNumberOfValues( numberOfPoints );
  radialDist->SetName("Double Radial Distance");
 
  vtkIntArray *iradDistance = vtkIntArray::New();
  iradDistance->SetNumberOfValues( numberOfPoints );
  iradDistance->SetName("Radial Distance");
  
  vtkIntArray *stepDistance = vtkIntArray::New();
  stepDistance->SetNumberOfValues( numberOfPoints );
  stepDistance->SetName("StepDistance");
  
  double dist = -1;
  int idis = -1;
  int stepDis= -1;
  for ( int i = 0; i < input->GetNumberOfPoints(); i++ )
    {
      input->GetPoint( i, point );
            
      dist = sqrt( vtkMath::Distance2BetweenPoints(this->MeasurePoint, point ) );
      idis = static_cast<int>(dist);
      
      //now actually calculate out the nearest step number      
      stepDis = (idis / this->StepNumber );      
      
      
      radialDist->SetValue(i,dist);        
      iradDistance->SetValue(i,idis);
      stepDistance->SetValue(i,stepDis);
      
    }
    
 
  
  //copy the input data
  output->DeepCopy( input );
  
  //assign properties
  
  output->GetPointData()->AddArray( radialDist );
  output->GetPointData()->AddArray( iradDistance );  
  output->GetPointData()->SetScalars( stepDistance );  
  
  radialDist->Delete();
  iradDistance->Delete();
  stepDistance->Delete();
  
  return 1;
  }


//----------------------------------------------------------------------------
void vtkRadialDistance::PrintSelf ( ostream& os, vtkIndent indent )
  {
  this->Superclass::PrintSelf ( os, indent );  
  }
