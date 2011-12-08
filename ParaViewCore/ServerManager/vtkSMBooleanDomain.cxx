/*=========================================================================

  Program:   ParaView
  Module:    vtkSMBooleanDomain.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMBooleanDomain.h"

#include "vtkObjectFactory.h"
#include "vtkPVXMLElement.h"
#include "vtkSMIntVectorProperty.h"

vtkStandardNewMacro(vtkSMBooleanDomain);

//---------------------------------------------------------------------------
vtkSMBooleanDomain::vtkSMBooleanDomain()
{
}

//---------------------------------------------------------------------------
vtkSMBooleanDomain::~vtkSMBooleanDomain()
{
}

//---------------------------------------------------------------------------
int vtkSMBooleanDomain::IsInDomain(vtkSMProperty* property)
{
  if (this->IsOptional)
    {
    return 1;
    }

  if (!property)
    {
    return 0;
    }
  vtkSMIntVectorProperty* ip = vtkSMIntVectorProperty::SafeDownCast(property);
  if (ip)
    {
    return 1;
    }

  return 0;
}

//---------------------------------------------------------------------------
void vtkSMBooleanDomain::SetAnimationValue(vtkSMProperty* property,
  int idx, double value)
{
  if (!property)
    {
    return;
    }
  vtkSMIntVectorProperty* ivp = 
    vtkSMIntVectorProperty::SafeDownCast(property);
  if (ivp)
    {
    ivp->SetElement(idx, static_cast<int>(value));
    }
}

//---------------------------------------------------------------------------
void vtkSMBooleanDomain::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

}
