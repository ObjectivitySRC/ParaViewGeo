/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkElliptic.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkElliptic.h"

#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkDiskSource.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

vtkCxxRevisionMacro(vtkElliptic, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkElliptic);

vtkElliptic::vtkElliptic()
{
  
  this->Translate[0] = 0;
  this->Translate[1] = 0;
  this->Translate[2] = 0;
  
  this->Scale[0] = 1;
  this->Scale[1] = 2;  
  
  this->Rotate[0] = 0;
  this->Rotate[1] = 0;
  this->Rotate[2] = 0;

  this->MatrixFlag = false;
  for (int i=0; i < 16; i++)
    {
    this->Matrix[i]=0;
    }
  
  this->RadialResolution = 20;  
	this->CircumferentialResolution = 20;  
  
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

void vtkElliptic::SetMatrix(double Matrix[16])
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

int vtkElliptic::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);  
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
 
  vtkDiskSource *ds  = vtkDiskSource::New();
  vtkTransform *transform = vtkTransform::New();
  vtkTransformFilter *tf = vtkTransformFilter::New();
  
  //do the transform stuff
  ds->SetRadialResolution( this->RadialResolution );
  ds->SetCircumferentialResolution( this->CircumferentialResolution );
  
  //need to make it a circle, not a disk with a hole
  ds->SetOuterRadius(0.5);
  ds->SetInnerRadius(0);
  
  if (this->MatrixFlag)
    {
    transform->SetMatrix(this->Matrix);  
    }
  else
    {
    transform->Translate(this->Translate);
    transform->RotateZ(this->Rotate[2]);
    transform->RotateX(this->Rotate[0]);
    transform->RotateY(this->Rotate[1]); 
    transform->Scale(this->Scale[0],this->Scale[1], 1);
    }
  
  tf->SetTransform(transform);  
  tf->SetInput(ds->GetOutput());
  
  //set output 
  tf->Update();
  output->ShallowCopy(tf->GetOutput());
  
  //delete the objects
  tf->Delete();
  transform->Delete();
  ds->Delete();


  return 1;
}

//----------------------------------------------------------------------------
int vtkElliptic::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

void vtkElliptic::PrintSelf(ostream& os, vtkIndent indent)
{
  
  this->Superclass::PrintSelf(os,indent);
}
