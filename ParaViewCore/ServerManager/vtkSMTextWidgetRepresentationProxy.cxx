/*=========================================================================

  Program:   ParaView
  Module:    vtkSMTextWidgetRepresentationProxy.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMTextWidgetRepresentationProxy.h"

#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkSMProxyProperty.h"
#include "vtkSMViewProxy.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkSMTextWidgetRepresentationProxy);

//----------------------------------------------------------------------------
vtkSMTextWidgetRepresentationProxy::vtkSMTextWidgetRepresentationProxy()
{
  this->TextActorProxy = 0;
  this->TextPropertyProxy = 0;
}

//----------------------------------------------------------------------------
vtkSMTextWidgetRepresentationProxy::~vtkSMTextWidgetRepresentationProxy()
{
  this->TextActorProxy = 0;
  this->TextPropertyProxy = 0;
}

//----------------------------------------------------------------------------
void vtkSMTextWidgetRepresentationProxy::CreateVTKObjects()
{
  if (this->ObjectsCreated)
    {
    return;
    }

  this->TextActorProxy = this->GetSubProxy("Prop2DActor");
  if (!this->TextActorProxy)
    {
    vtkErrorMacro("Failed to find subproxy Prop2DActor.");
    return;
    }
  this->TextPropertyProxy = this->GetSubProxy("Prop2DProperty");
  if (!this->TextPropertyProxy)
    {
    vtkErrorMacro("Failed to find subproxy Prop2DProperty.");
    return;
    }

  this->TextActorProxy->SetLocation( vtkProcessModule::RENDER_SERVER |
                                     vtkProcessModule::CLIENT);
  this->TextPropertyProxy->SetLocation( vtkProcessModule::RENDER_SERVER |
                                       vtkProcessModule::CLIENT);

  this->Superclass::CreateVTKObjects();

  if (!this->RepresentationProxy)
    {
    vtkErrorMacro("Failed to find subproxy Prop2D.");
    return;
    }

  vtkSMProxyProperty* tppp = vtkSMProxyProperty::SafeDownCast(
    this->TextActorProxy->GetProperty("TextProperty"));
  if (!tppp)
    {
    vtkErrorMacro("Failed to find property TextProperty on TextActorProxy.");
    return;
    }
  if(!tppp->AddProxy(this->TextPropertyProxy))
    {
    return;
    }

  vtkSMProxyProperty* tapp = vtkSMProxyProperty::SafeDownCast(
    this->RepresentationProxy->GetProperty("TextActor"));
  if (!tapp)
    {
    vtkErrorMacro("Failed to find property TextActor on TextRepresentationProxy.");
    return;
    }
  if(!tapp->AddProxy(this->TextActorProxy))
    {
    return;
    }

  // Mark TextActor properties modified so the default value will be pushed at
  // the UpdateVTKObject call. This prevent them from behing overriden by some
  // vtk initialization code when the TextActor get linked to the representation
  this->TextActorProxy->MarkAllPropertiesAsModified();
}

//----------------------------------------------------------------------------
void vtkSMTextWidgetRepresentationProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
