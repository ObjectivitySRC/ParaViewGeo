/*=========================================================================

  Program:   ParaView
  Module:    vtkSMUndoStackBuilder.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMUndoStackBuilder.h"

#include "vtkCommand.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkPVXMLElement.h"
#include "vtkSMGlobalPropertiesManager.h"
#include "vtkSMMessage.h"
#include "vtkSMProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMProxyManager.h"
#include "vtkSMRemoteObjectUpdateUndoElement.h"
#include "vtkSMSession.h"
#include "vtkSMUndoStack.h"
#include "vtkUndoElement.h"
#include "vtkUndoSet.h"
#include "vtkUndoStackInternal.h"

#include <vtksys/RegularExpression.hxx>
#include <vtkstd/map>

vtkStandardNewMacro(vtkSMUndoStackBuilder);
vtkCxxSetObjectMacro(vtkSMUndoStackBuilder, UndoStack, vtkSMUndoStack);
//-----------------------------------------------------------------------------
vtkSMUndoStackBuilder::vtkSMUndoStackBuilder()
{
  this->UndoStack = 0;
  this->UndoSet = vtkUndoSet::New();
  this->Label = NULL;
  this->EnableMonitoring = 0;
  this->IgnoreAllChanges = false;
}

//-----------------------------------------------------------------------------
vtkSMUndoStackBuilder::~vtkSMUndoStackBuilder()
{
  if (this->UndoSet)
    {
    this->UndoSet->Delete();
    this->UndoSet = NULL;
    }
  this->SetLabel(NULL);
  this->SetUndoStack(0);
}
//-----------------------------------------------------------------------------
void vtkSMUndoStackBuilder::Begin(const char* label)
{
  if (!this->Label)
    {
    this->SetLabel(label);
    }

  ++this->EnableMonitoring;
}

//-----------------------------------------------------------------------------
void vtkSMUndoStackBuilder::End()
{
  if (this->EnableMonitoring == 0)
    {
    vtkWarningMacro("Unmatched End().");
    return;
    }
  this->EnableMonitoring--;

}

//-----------------------------------------------------------------------------
void vtkSMUndoStackBuilder::PushToStack()
{
  if(this->EnableMonitoring > 0)
    {
    return; // Only push the whole set when the first begin/end has been reached
    }

  if (this->UndoSet->GetNumberOfElements() > 0 && this->UndoStack)
    {
    this->UndoStack->Push( (this->Label? this->Label : "Changes"),
                           this->UndoSet);
    }
  this->InitializeUndoSet();
}

//-----------------------------------------------------------------------------
void vtkSMUndoStackBuilder::Clear()
{
  this->InitializeUndoSet();
}

//-----------------------------------------------------------------------------
void vtkSMUndoStackBuilder::InitializeUndoSet()
{
  this->SetLabel(NULL);
  this->UndoSet->RemoveAllElements();
}

//-----------------------------------------------------------------------------
bool vtkSMUndoStackBuilder::Add(vtkUndoElement* element)
{
  if (!element)
    {
    return false;
    }

  if (this->IgnoreAllChanges || !this->HandleChangeEvents() || !this->UndoStack)
    {
    return false;
    }

  this->UndoSet->AddElement(element);
  return true;
}
//-----------------------------------------------------------------------------
void vtkSMUndoStackBuilder::OnStateChange( vtkSMSession *session,
                                           vtkTypeUInt32 vtkNotUsed(globalId),
                                           const vtkSMMessage *previousState,
                                           const vtkSMMessage *newState)
{
  if (this->IgnoreAllChanges || !this->HandleChangeEvents() || !this->UndoStack)
    {
    return;
    }

  vtkSMRemoteObjectUpdateUndoElement* undoElement;
  undoElement = vtkSMRemoteObjectUpdateUndoElement::New();
  undoElement->SetSession(session);
  undoElement->SetUndoRedoState( previousState, newState );
  this->Add(undoElement);
  undoElement->FastDelete();
}

//-----------------------------------------------------------------------------
void vtkSMUndoStackBuilder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "IgnoreAllChanges: " << this->IgnoreAllChanges << endl;
  os << indent << "UndoStack: " << this->UndoStack << endl;
}
