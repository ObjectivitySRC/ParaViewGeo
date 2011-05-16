/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkEllipsoid.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkEllipsoid.h"

#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkSphereSource.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

vtkCxxRevisionMacro(vtkEllipsoid, "$Revision: 1 $");
vtkStandardNewMacro(vtkEllipsoid);

vtkEllipsoid::vtkEllipsoid()
{
  
  this->Translate[0] = 0;
  this->Translate[1] = 0;
  this->Translate[2] = 0;
  
  this->Scale[0] = 0;
  this->Scale[1] = 0;
  this->Scale[2] = 0;
  
  this->Rotate[0] = 0;
  this->Rotate[1] = 0;
  this->Rotate[2] = 0;
  
  this->WFlags[0] = false;
  this->WFlags[1] = false;
  this->WAngle = 0; //you are locked to the range -180 to 180
  
  this->MatrixFlag = false;
  for (int i=0; i < 16; i++)
    {
    this->Matrix[i]=0;
    }
  
  this->PhiResolution = 20;
	this->ThetaResolution = 20;
  
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

void vtkEllipsoid::SetWAxis(double WAxis[3])
  {
  if (this->Rotate != WAxis)
    {
    this->WFlags[1]=true;
    this->Rotate[0] = WAxis[0];
    this->Rotate[1] = WAxis[1];
    this->Rotate[2] = WAxis[2];
    this->Modified();
    }      
  }
  
void vtkEllipsoid::SetMatrix(double Matrix[16])
  {
  if (this->Matrix != Matrix)
    {
    this->MatrixFlag=true;
    for (int i=0; i < 16; i++)
      {
      this->Matrix[i]=Matrix[i];
      }
    this->Modified();
    }      
  }
void vtkEllipsoid::SetWAngle(double WAngle)
  {
  if (this->WAngle != WAngle)
    {
    this->WFlags[0]=true;
    //boundary checks
    if (WAngle < -180)
      WAngle = -180;
    else if(WAngle > 180)
      WAngle = 180;
    this->WAngle = WAngle;
    this->Modified();
    }      
  }
    

int vtkEllipsoid::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);  
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
 
  vtkSphereSource *sphere = vtkSphereSource::New();
  vtkTransform *transform = vtkTransform::New();
  vtkTransformFilter *tf = vtkTransformFilter::New();
  
  //do the transform stuff
  sphere->SetPhiResolution( this->PhiResolution );
  sphere->SetThetaResolution( this->ThetaResolution );  
  
  if (this->MatrixFlag)
    {
    transform->SetMatrix(this->Matrix);  
    }
  else
    {
    transform->Translate(this->Translate);
    if (this->WFlags[0]&& this->WFlags[1])
      {      
      transform->RotateWXYZ(this->WAngle, this->Rotate); 
      }
    else
      {
      
      transform->RotateZ(this->Rotate[2]);
      transform->RotateX(this->Rotate[0]);
      transform->RotateY(this->Rotate[1]);    
      }
    transform->Scale(this->Scale); 
        
    }
  tf->SetTransform(transform);    
  tf->SetInput(sphere->GetOutput());  
  //set output 
  tf->Update();
  output->ShallowCopy(tf->GetOutput());
    
  sphere->Delete();
  transform->Delete();
  tf->Delete();
  return 1;
}

//----------------------------------------------------------------------------
int vtkEllipsoid::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

void vtkEllipsoid::PrintSelf(ostream& os, vtkIndent indent)
{
  
  this->Superclass::PrintSelf(os,indent);
}
