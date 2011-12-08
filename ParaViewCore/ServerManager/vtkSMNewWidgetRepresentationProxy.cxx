/*=========================================================================

  Program:   ParaView
  Module:    vtkSMNewWidgetRepresentationProxy.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMNewWidgetRepresentationProxy.h"

#include "vtk3DWidgetRepresentation.h"
#include "vtkAbstractWidget.h"
#include "vtkClientServerStream.h"
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkPVGenericRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMPropertyIterator.h"
#include "vtkSMPropertyLink.h"
#include "vtkSMProxyProperty.h"
#include "vtkSMRenderViewProxy.h"
#include "vtkSMSession.h"
#include "vtkSMViewProxy.h"
#include "vtkSMWidgetRepresentationProxy.h"
#include "vtkWeakPointer.h"
#include "vtkWidgetRepresentation.h"

#include <vtkstd/list>

vtkStandardNewMacro(vtkSMNewWidgetRepresentationProxy);

class vtkSMNewWidgetRepresentationObserver : public vtkCommand
{
public:
  static vtkSMNewWidgetRepresentationObserver *New() 
    { return new vtkSMNewWidgetRepresentationObserver; }
  virtual void Execute(vtkObject*, unsigned long event, void*)
    {
      if (this->Proxy)
        {
        this->Proxy->ExecuteEvent(event);
        }
    }
  vtkSMNewWidgetRepresentationObserver():Proxy(0) {}
  vtkSMNewWidgetRepresentationProxy* Proxy;
};

struct vtkSMNewWidgetRepresentationInternals
{
  typedef vtkstd::list<vtkSmartPointer<vtkSMLink> > LinksType;
  LinksType Links;
  vtkWeakPointer<vtkSMRenderViewProxy> ViewProxy;
};

//----------------------------------------------------------------------------
vtkSMNewWidgetRepresentationProxy::vtkSMNewWidgetRepresentationProxy()
{
  this->SetLocation(vtkPVSession::CLIENT_AND_SERVERS);
  this->RepresentationProxy = 0;
  this->WidgetProxy = 0;
  this->Widget = 0;
  this->Observer = vtkSMNewWidgetRepresentationObserver::New();
  this->Observer->Proxy = this;
  this->Internal = new vtkSMNewWidgetRepresentationInternals;
}

//----------------------------------------------------------------------------
vtkSMNewWidgetRepresentationProxy::~vtkSMNewWidgetRepresentationProxy()
{
  this->RepresentationProxy = 0;
  this->WidgetProxy = 0;
  this->Widget = 0;
  this->Observer->Proxy = 0;
  this->Observer->Delete();

  if (this->Internal)
    {
    delete this->Internal;
    }
}

//-----------------------------------------------------------------------------
void vtkSMNewWidgetRepresentationProxy::CreateVTKObjects()
{
  if (this->ObjectsCreated)
    {
    return;
    }

  this->RepresentationProxy = this->GetSubProxy("Prop");
  if (!this->RepresentationProxy)
    {
    this->RepresentationProxy = this->GetSubProxy("Prop2D");
    }
  if (!this->RepresentationProxy)
    {
    vtkErrorMacro(
      "A representation proxy must be defined as a Prop (or Prop2D) sub-proxy");
    return;
    }
  this->RepresentationProxy->SetLocation(
    vtkPVSession::RENDER_SERVER | vtkPVSession::CLIENT);

  this->WidgetProxy = this->GetSubProxy("Widget");
  if (this->WidgetProxy)
    {
    this->WidgetProxy->SetLocation(vtkPVSession::CLIENT);
    }

  this->Superclass::CreateVTKObjects();

  // Bind the Widget and the representations on the server side
  vtkClientServerStream stream;
  stream << vtkClientServerStream::Invoke
         << VTKOBJECT(this)
         << "SetRepresentation"
         << VTKOBJECT(this->RepresentationProxy)
         << vtkClientServerStream::End;
  this->ExecuteStream(stream, false, vtkPVSession::RENDER_SERVER |
    vtkPVSession::CLIENT);

  // Location 0 is for prototype objects !!! No need to send to the server something.
  if (!this->WidgetProxy || this->Location == 0)
    {
    return;
    }

  vtkSMProxyProperty* pp = vtkSMProxyProperty::SafeDownCast(
    this->WidgetProxy->GetProperty("Representation"));
  if (pp)
    {
    pp->AddProxy(this->RepresentationProxy);
    }
  this->WidgetProxy->UpdateVTKObjects();

  // Get the local VTK object for that widget
  this->Widget = vtkAbstractWidget::SafeDownCast(
    this->WidgetProxy->GetClientSideObject());

  if (this->Widget)
    {
    this->Widget->AddObserver(
      vtkCommand::StartInteractionEvent, this->Observer);
    this->Widget->AddObserver(
      vtkCommand::EndInteractionEvent, this->Observer);
    this->Widget->AddObserver(
      vtkCommand::InteractionEvent, this->Observer);
    }

  vtk3DWidgetRepresentation* clientObject =
    vtk3DWidgetRepresentation::SafeDownCast(this->GetClientSideObject());
  clientObject->SetWidget(this->Widget);

  // Since links copy values from input to output,
  // we need to make sure that input properties i.e. the info
  // properties are not empty.
  this->UpdatePropertyInformation();

  vtkSMPropertyIterator* piter = this->NewPropertyIterator();
  for(piter->Begin(); !piter->IsAtEnd(); piter->Next())
    {
    vtkSMProperty* prop = piter->GetProperty();
    vtkSMProperty* info = prop->GetInformationProperty();
    if (info)
      {      
      // This ensures that the property value from the loaded state is
      // preserved, and not overwritten by the default value from 
      // the property information
      info->Copy(prop);
      
      vtkSMPropertyLink* link = vtkSMPropertyLink::New();

      link->AddLinkedProperty(this,
                              piter->GetKey(), 
                              vtkSMLink::OUTPUT);
      link->AddLinkedProperty(this,
                              this->GetPropertyName(info),
                              vtkSMLink::INPUT);

      this->Internal->Links.push_back(link);
      link->Delete();
      }
    }
  piter->Delete();

}

//-----------------------------------------------------------------------------
void vtkSMNewWidgetRepresentationProxy::ExecuteEvent(unsigned long event)
{
  this->InvokeEvent(event);

  vtkSMWidgetRepresentationProxy* widgetRepresentation =
      vtkSMWidgetRepresentationProxy::SafeDownCast(this->RepresentationProxy);

  if (event == vtkCommand::StartInteractionEvent)
    {
    vtkPVGenericRenderWindowInteractor* inter =
        vtkPVGenericRenderWindowInteractor::SafeDownCast(
            this->Widget->GetInteractor());
    if (inter)
      {
      inter->InteractiveRenderEnabledOn();
      }
    if(widgetRepresentation)
      {
      widgetRepresentation->OnStartInteraction();
      }
    }
  else if (event == vtkCommand::InteractionEvent)
    {
    this->RepresentationProxy->UpdatePropertyInformation();
    this->UpdateVTKObjects();

    if(widgetRepresentation)
      {
      widgetRepresentation->OnInteraction();
      }
    }
  else if (event == vtkCommand::EndInteractionEvent)
    {
    vtkPVGenericRenderWindowInteractor* inter =
        vtkPVGenericRenderWindowInteractor::SafeDownCast(
            this->Widget->GetInteractor());
    if (inter)
      {
      inter->InteractiveRenderEnabledOff();
      }
    vtkSMProperty* sizeHandles =
        this->RepresentationProxy->GetProperty("SizeHandles");
    if (sizeHandles)
      {
      sizeHandles->Modified();
      this->RepresentationProxy->UpdateProperty("SizeHandles");
      }
    if(widgetRepresentation)
      {
      widgetRepresentation->OnEndInteraction();
      }
    }
}

//----------------------------------------------------------------------------
void vtkSMNewWidgetRepresentationProxy::UnRegister(vtkObjectBase* obj)
{
  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  // If the object is not being deleted by the interpreter and it
  // has a reference count of 2 (SelfID and the reference that is
  // being released), delete the internals so that the links
  // release their references to the proxy
  if ( pm && this->Internal )
    {
    int size = this->Internal->Links.size();
    if (size > 0 && this->ReferenceCount == 2 + 2*size)
      {
      vtkSMNewWidgetRepresentationInternals* aInternal = this->Internal;
      this->Internal = 0;
      delete aInternal;
      aInternal = 0;
      }
    }

  this->Superclass::UnRegister(obj);
}

//----------------------------------------------------------------------------
void vtkSMNewWidgetRepresentationProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


