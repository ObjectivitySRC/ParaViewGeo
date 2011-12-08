/*=========================================================================

  Program:   ParaView
  Module:    vtkSIIntVectorProperty.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSIIntVectorProperty.h"
#include "vtkSIVectorPropertyTemplate.txx"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkSIIntVectorProperty);
//----------------------------------------------------------------------------
vtkSIIntVectorProperty::vtkSIIntVectorProperty()
{
}

//----------------------------------------------------------------------------
vtkSIIntVectorProperty::~vtkSIIntVectorProperty()
{
}

//----------------------------------------------------------------------------
void vtkSIIntVectorProperty::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
