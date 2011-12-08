/*=========================================================================

  Program:   ParaView
  Module:    vtkSMOrderedPropertyIterator.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMOrderedPropertyIterator.h"

#include "vtkObjectFactory.h"
#include "vtkSMProxy.h"
#include "vtkSMProxyInternals.h"

vtkStandardNewMacro(vtkSMOrderedPropertyIterator);

//---------------------------------------------------------------------------
vtkSMOrderedPropertyIterator::vtkSMOrderedPropertyIterator()
{
  this->Proxy = 0;
  this->Index = 0;
}

//---------------------------------------------------------------------------
vtkSMOrderedPropertyIterator::~vtkSMOrderedPropertyIterator()
{
  this->SetProxy(0);
}

//---------------------------------------------------------------------------
void vtkSMOrderedPropertyIterator::SetProxy(vtkSMProxy* proxy)
{
  if (this->Proxy != proxy)
    {
    if (this->Proxy != NULL) { this->Proxy->UnRegister(this); }
    this->Proxy = proxy;
    if (this->Proxy != NULL) 
      { 
      this->Proxy->Register(this); 
      this->Begin();
      }
    this->Modified();
    }
}

//---------------------------------------------------------------------------
void vtkSMOrderedPropertyIterator::Begin()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: Begin()");
    return;
    }

  this->Index = 0;
}

//---------------------------------------------------------------------------
int vtkSMOrderedPropertyIterator::IsAtEnd()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: IsAtEnd()");
    return 1;
    }

  if (this->Index >=
      this->Proxy->Internals->PropertyNamesInOrder.size())
    {
    return 1;
    }
  return 0;
}

//---------------------------------------------------------------------------
void vtkSMOrderedPropertyIterator::Next()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: Next()");
    return;
    }

  this->Index++;
}

//---------------------------------------------------------------------------
const char* vtkSMOrderedPropertyIterator::GetKey()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: GetKey()");
    return 0;
    }

  if (!this->IsAtEnd())
    {
    return this->Proxy->Internals->PropertyNamesInOrder[this->Index];
    }

  return 0;
}

//---------------------------------------------------------------------------
vtkSMProperty* vtkSMOrderedPropertyIterator::GetProperty()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: GetProperty()");
    return 0;
    }

  if (!this->IsAtEnd())
    {
    return this->Proxy->GetProperty(
      this->Proxy->Internals->PropertyNamesInOrder[this->Index]);
    }
  return 0;
}

//---------------------------------------------------------------------------
void vtkSMOrderedPropertyIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Proxy: " << this->Proxy << endl;
}
