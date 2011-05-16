/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVectorTransform.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkVectorTransform.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"

vtkCxxRevisionMacro(vtkVectorTransform, "$Revision: 1.45 $");
vtkStandardNewMacro(vtkVectorTransform);

vtkVectorTransform::vtkVectorTransform()
{
  this->Transform = vtkTransform::New();
  for (int i=0; i<3; i++)
    {
    this->Point1[i] = this->Point2[i] = 0.0;
    }
}

vtkVectorTransform::~vtkVectorTransform()
{
}

int vtkVectorTransform::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkTransform::SafeDownCast(this->Transform)
    ->Translate(this->Point2[0] - this->Point1[0],
                this->Point2[1] - this->Point1[1],
                this->Point2[2] - this->Point1[2]);
  return this->Superclass::RequestData(request, inputVector, outputVector);
}

void vtkVectorTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
