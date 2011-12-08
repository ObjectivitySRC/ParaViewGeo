/*=========================================================================

  Program:   ParaView
  Module:    vtkSMFileListDomain.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMFileListDomain.h"

#include "vtkObjectFactory.h"
#include "vtkSMStringVectorProperty.h"

vtkStandardNewMacro(vtkSMFileListDomain);

//---------------------------------------------------------------------------
vtkSMFileListDomain::vtkSMFileListDomain()
{
}

//---------------------------------------------------------------------------
vtkSMFileListDomain::~vtkSMFileListDomain()
{
}

//---------------------------------------------------------------------------
int vtkSMFileListDomain::SetDefaultValues(vtkSMProperty* property)
{
  vtkSMStringVectorProperty* svp = 
    vtkSMStringVectorProperty::SafeDownCast(property);
  if (svp && this->GetNumberOfStrings() > 0)
    {
    svp->SetElement(0, this->GetString(0));
    return 1;
    }

  return this->Superclass::SetDefaultValues(property);
}

//---------------------------------------------------------------------------
void vtkSMFileListDomain::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
