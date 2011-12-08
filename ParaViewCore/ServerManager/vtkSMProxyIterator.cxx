/*=========================================================================

  Program:   ParaView
  Module:    vtkSMProxyIterator.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMProxyIterator.h"

#include "vtkObjectFactory.h"
#include "vtkSMProxyManager.h"
#include "vtkSMProxyManagerInternals.h"

vtkStandardNewMacro(vtkSMProxyIterator);

struct vtkSMProxyIteratorInternals
{
  vtkSMProxyManagerProxyListType::iterator ProxyIterator;
  vtkSMProxyManagerProxyMapType::iterator ProxyListIterator;
  vtkSMProxyManagerInternals::ProxyGroupType::iterator GroupIterator;
};

//---------------------------------------------------------------------------
vtkSMProxyIterator::vtkSMProxyIterator()
{
  this->Internals = new vtkSMProxyIteratorInternals;
  this->Mode = vtkSMProxyIterator::ALL;
  this->SkipPrototypes = true;
  this->Begin();
}

//---------------------------------------------------------------------------
vtkSMProxyIterator::~vtkSMProxyIterator()
{
  delete this->Internals;
}

//---------------------------------------------------------------------------
void vtkSMProxyIterator::Begin(const char* groupName)
{
  vtkSMProxyManager* pm = vtkSMObject::GetProxyManager();
  if (!pm)
    {
    vtkWarningMacro("ProxyManager is not set. Can not perform operation: Begin()");
    return;
    }
  this->Internals->GroupIterator = 
    pm->Internals->RegisteredProxyMap.find(groupName);
  if (this->Internals->GroupIterator!=pm->Internals->RegisteredProxyMap.end())
    {
    this->Internals->ProxyListIterator = 
      this->Internals->GroupIterator->second.begin();
    if (this->Internals->ProxyListIterator !=
      this->Internals->GroupIterator->second.end())
      {
      this->Internals->ProxyIterator =
        this->Internals->ProxyListIterator->second.begin();
      }
    }
}

//---------------------------------------------------------------------------
void vtkSMProxyIterator::Begin()
{
  vtkSMProxyManager* pm = vtkSMObject::GetProxyManager();
  if (!pm)
    {
    vtkWarningMacro("ProxyManager is not set. Can not perform operation: Begin()");
    return;
    }
  this->Internals->GroupIterator = pm->Internals->RegisteredProxyMap.begin();
  while (this->Internals->GroupIterator!=pm->Internals->RegisteredProxyMap.end())
    {
    this->Internals->ProxyListIterator = 
      this->Internals->GroupIterator->second.begin();
    while (this->Internals->ProxyListIterator != 
      this->Internals->GroupIterator->second.end())
      {
      this->Internals->ProxyIterator = 
        this->Internals->ProxyListIterator->second.begin();
      if (this->Internals->ProxyIterator !=
        this->Internals->ProxyListIterator->second.end())
        {
        break;
        }
      this->Internals->ProxyListIterator++;
      }
    if (this->Internals->ProxyListIterator != 
      this->Internals->GroupIterator->second.end())
      {
      break;
      }
    this->Internals->GroupIterator++;
    }
  if (this->SkipPrototypes)
    {
    if (this->GetProxy() && this->GetProxy()->GetSession() == NULL)
      {
      this->Next();
      }
    }
}

//---------------------------------------------------------------------------
int vtkSMProxyIterator::IsAtEnd()
{
  vtkSMProxyManager* pm = vtkSMObject::GetProxyManager();
  if (this->Internals->GroupIterator == 
      pm->Internals->RegisteredProxyMap.end())
    {
    return 1;
    }
  if ( this->Mode == vtkSMProxyIterator::ONE_GROUP &&
       this->Internals->ProxyListIterator == 
       this->Internals->GroupIterator->second.end() )
    {
    return 1;
    }
  return 0;
}

//---------------------------------------------------------------------------
void vtkSMProxyIterator::Next()
{
  this->NextInternal();
  if (this->SkipPrototypes)
    {
    if (!this->IsAtEnd() &&
      this->GetProxy() && this->GetProxy()->GetSession() == NULL)
      {
      this->Next();
      }
    }
}

//---------------------------------------------------------------------------
void vtkSMProxyIterator::NextInternal()
{
  vtkSMProxyManager* pm = vtkSMObject::GetProxyManager();

  if (this->Internals->GroupIterator != 
      pm->Internals->RegisteredProxyMap.end())
    {
    if (this->Mode == vtkSMProxyIterator::GROUPS_ONLY)
      {
      this->Internals->GroupIterator++;
      if (this->Internals->GroupIterator != 
          pm->Internals->RegisteredProxyMap.end())
        {
        this->Internals->ProxyListIterator = 
          this->Internals->GroupIterator->second.begin();
        if (this->Internals->ProxyListIterator !=
          this->Internals->GroupIterator->second.end())
          {
          this->Internals->ProxyIterator = 
            this->Internals->ProxyListIterator->second.begin();
          }
        }
      }
    else
      {
      if (this->Internals->ProxyIterator != 
          this->Internals->ProxyListIterator->second.end())
        {
        this->Internals->ProxyIterator++;
        }

      if (this->Internals->ProxyIterator ==
        this->Internals->ProxyListIterator->second.end())
        {
        if (this->Internals->ProxyListIterator !=
          this->Internals->GroupIterator->second.end())
          {
          // Advance the proxy list iterator till
          // we reach a non-empty proxy list. The proxy iterator
          // must also be moved to the start of this new list.
          this->Internals->ProxyListIterator++;
          while (this->Internals->ProxyListIterator !=
            this->Internals->GroupIterator->second.end())
            {
            this->Internals->ProxyIterator =
              this->Internals->ProxyListIterator->second.begin();
            if (this->Internals->ProxyIterator !=
              this->Internals->ProxyListIterator->second.end())
              {
              break;
              }
            this->Internals->ProxyListIterator++;
            }
          }
        }
      
      if (this->Mode != vtkSMProxyIterator::ONE_GROUP)
        {
        if (this->Internals->ProxyListIterator == 
            this->Internals->GroupIterator->second.end())
          {
          // Advance the group iter till we reach a non-empty group.
          // The proxt list iter and the proxy iter also need to be
          // updated accordingly.
          this->Internals->GroupIterator++;
          while (this->Internals->GroupIterator !=
                 pm->Internals->RegisteredProxyMap.end())
            {
            this->Internals->ProxyListIterator = 
              this->Internals->GroupIterator->second.begin();

            while (this->Internals->ProxyListIterator !=
              this->Internals->GroupIterator->second.end())
              {
              this->Internals->ProxyIterator = 
                this->Internals->ProxyListIterator->second.begin();
              if (this->Internals->ProxyIterator !=
                this->Internals->ProxyListIterator->second.end())
                {
                break;
                }
              this->Internals->ProxyListIterator++;
              }

            if (this->Internals->ProxyListIterator !=
              this->Internals->GroupIterator->second.end())
              {
              break;
              }
            this->Internals->GroupIterator++;
            }
          }
        }
      }
    }
}

//---------------------------------------------------------------------------
const char* vtkSMProxyIterator::GetGroup()
{
  vtkSMProxyManager* pm = vtkSMObject::GetProxyManager();

  if (this->Internals->GroupIterator != 
      pm->Internals->RegisteredProxyMap.end())
    {
    return this->Internals->GroupIterator->first.c_str();
    }
  return 0;
}

//---------------------------------------------------------------------------
const char* vtkSMProxyIterator::GetKey()
{
  vtkSMProxyManager* pm = vtkSMObject::GetProxyManager();

  if (this->Internals->GroupIterator != 
      pm->Internals->RegisteredProxyMap.end())
    {
    if (this->Internals->ProxyListIterator != 
        this->Internals->GroupIterator->second.end())
      {
      return this->Internals->ProxyListIterator->first.c_str();
      }
    }
  return 0;
}

//---------------------------------------------------------------------------
vtkSMProxy* vtkSMProxyIterator::GetProxy()
{
  vtkSMProxyManager* pm = vtkSMObject::GetProxyManager();

  if (this->Internals->GroupIterator != 
      pm->Internals->RegisteredProxyMap.end())
    {
    if (this->Internals->ProxyListIterator != 
        this->Internals->GroupIterator->second.end())
      {
      if (this->Internals->ProxyIterator !=
        this->Internals->ProxyListIterator->second.end())
        {
        return this->Internals->ProxyIterator->GetPointer()->Proxy.GetPointer();
        }
      }
    }
  return 0;
}

//---------------------------------------------------------------------------
void vtkSMProxyIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "SkipPrototypes: " << this->SkipPrototypes << endl;
  os << indent << "Mode: " << this->Mode << endl;
}
