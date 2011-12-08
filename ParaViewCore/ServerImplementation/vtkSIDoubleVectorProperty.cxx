/*=========================================================================

  Program:   ParaView
  Module:    vtkSIDoubleVectorProperty.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSIDoubleVectorProperty.h"
#include "vtkSIVectorPropertyTemplate.txx"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkSIDoubleVectorProperty);
//----------------------------------------------------------------------------
vtkSIDoubleVectorProperty::vtkSIDoubleVectorProperty()
{
}

//----------------------------------------------------------------------------
vtkSIDoubleVectorProperty::~vtkSIDoubleVectorProperty()
{
}

//----------------------------------------------------------------------------
void vtkSIDoubleVectorProperty::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
