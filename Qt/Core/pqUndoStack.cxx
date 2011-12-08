/*=========================================================================

   Program: ParaView
   Module:    pqUndoStack.cxx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "pqUndoStack.h"

#include "vtkEventQtSlotConnect.h"
#include "vtkProcessModule.h"
#include "vtkSmartPointer.h"
#include "vtkSMProxyManager.h"
#include "vtkSMUndoStackBuilder.h"
#include "vtkSMUndoStack.h"

#include "vtkUndoSet.h"
#include "vtkSMUndoElement.h"
#include "vtkSMRemoteObjectUpdateUndoElement.h"


#include <QtDebug>
#include <QPointer>

#include "pqApplicationCore.h"
#include "pqHelperProxyRegisterUndoElement.h"
#include "pqProxyModifiedStateUndoElement.h"
#include "pqServer.h"

//-----------------------------------------------------------------------------
class pqUndoStack::pqImplementation
{
public:
  pqImplementation()
    {
    this->NestedCount = 0;
    }
  vtkSmartPointer<vtkSMUndoStack> UndoStack;
  vtkSmartPointer<vtkSMUndoStackBuilder> UndoStackBuilder;
  vtkSmartPointer<vtkEventQtSlotConnect> VTKConnector;

  QList<bool> IgnoreAllChangesStack;
  int NestedCount;
};

//-----------------------------------------------------------------------------
pqUndoStack::pqUndoStack( vtkSMUndoStackBuilder* builder,
  QObject* _parent/*=null*/) 
:QObject(_parent)
{
  this->Implementation = new pqImplementation();
  this->Implementation->UndoStack = vtkSmartPointer<vtkSMUndoStack>::New();

  if (builder)
    {
    this->Implementation->UndoStackBuilder = builder;
    }
  else
    {
    // create default builder.
    builder = vtkSMUndoStackBuilder::New();
    this->Implementation->UndoStackBuilder = builder;
    builder->Delete();
    }

  builder->SetUndoStack(this->Implementation->UndoStack);

  this->Implementation->VTKConnector = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  this->Implementation->VTKConnector->Connect(this->Implementation->UndoStack,
    vtkCommand::ModifiedEvent, this, SLOT(onStackChanged()), NULL, 1.0);
}

//-----------------------------------------------------------------------------
pqUndoStack::~pqUndoStack()
{
  delete this->Implementation;
}

//-----------------------------------------------------------------------------
vtkSMUndoStackBuilder* pqUndoStack::GetUndoStackBuilder()
{
  return this->Implementation->UndoStackBuilder;
}

//-----------------------------------------------------------------------------
bool pqUndoStack::canUndo()
{
  return this->Implementation->UndoStack->CanUndo();
}

//-----------------------------------------------------------------------------
bool pqUndoStack::canRedo()
{
  return this->Implementation->UndoStack->CanRedo();
}

//-----------------------------------------------------------------------------
const QString pqUndoStack::undoLabel()
{
  return this->Implementation->UndoStack->CanUndo() ?
    this->Implementation->UndoStack->GetUndoSetLabel(0) :
    QString();
}

//-----------------------------------------------------------------------------
const QString pqUndoStack::redoLabel()
{
  return this->Implementation->UndoStack->CanRedo() ?
    this->Implementation->UndoStack->GetRedoSetLabel(0) :
    QString();
}

//-----------------------------------------------------------------------------
void pqUndoStack::addToActiveUndoSet(vtkUndoElement* element)
{
  if (this->ignoreAllChanges())
    {
    return;
    }

  this->Implementation->UndoStackBuilder->Add(element);
}

//-----------------------------------------------------------------------------
void pqUndoStack::onStackChanged()
{
  bool can_undo = false;
  bool can_redo = false;
  QString undo_label;
  QString redo_label;
  if (this->Implementation->UndoStack->CanUndo())
    {
    can_undo = true;
    undo_label = this->Implementation->UndoStack->GetUndoSetLabel(0);
    }
  if (this->Implementation->UndoStack->CanRedo())
    {
    can_redo = true;
    redo_label = this->Implementation->UndoStack->GetRedoSetLabel(0);
    }
    
  emit this->stackChanged(can_undo, undo_label, can_redo, redo_label);
  emit this->canUndoChanged(can_undo);
  emit this->canRedoChanged(can_redo);
  emit this->undoLabelChanged(undo_label);
  emit this->redoLabelChanged(redo_label);
}

//-----------------------------------------------------------------------------
void pqUndoStack::beginUndoSet(QString label)
{
  if(this->Implementation->NestedCount == 0)
    {
    this->Implementation->UndoStackBuilder->Begin(label.toAscii().data());
    }

  this->Implementation->NestedCount++;
}

//-----------------------------------------------------------------------------
void pqUndoStack::endUndoSet()
{
  if(this->Implementation->NestedCount == 0)
    {
    qDebug() << "endUndoSet called without a beginUndoSet.";
    return;
    }

  this->Implementation->NestedCount--;
  if(this->Implementation->NestedCount == 0)
    {
    this->Implementation->UndoStackBuilder->EndAndPushToStack();
    }
}

//-----------------------------------------------------------------------------
void pqUndoStack::undo()
{
  this->beginNonUndoableChanges();
  this->Implementation->UndoStack->Undo();

  // Update of proxies have to happen in order.
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies("sources", 1);
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies("lookup_tables", 1);
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies("representations", 1);
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies("scalar_bars", 1);
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies(1);

  this->endNonUndoableChanges();

  pqApplicationCore::instance()->render();

  emit this->undone();
}

//-----------------------------------------------------------------------------
void pqUndoStack::redo()
{
  this->beginNonUndoableChanges();
  this->Implementation->UndoStack->Redo();

  // Update of proxies have to happen in order.
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies("sources", 1);
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies("lookup_tables", 1);
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies("representations", 1);
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies("scalar_bars", 1);
  vtkSMProxyManager::GetProxyManager()->UpdateRegisteredProxies(1);

  this->endNonUndoableChanges();

  pqApplicationCore::instance()->render();

  emit this->redone();
}

//-----------------------------------------------------------------------------
void pqUndoStack::clear()
{
  this->Implementation->UndoStack->Clear();
  this->Implementation->UndoStackBuilder->Clear();
}
  
//-----------------------------------------------------------------------------
void pqUndoStack::beginNonUndoableChanges()
{
  this->Implementation->IgnoreAllChangesStack.push_back(
    this->ignoreAllChanges());

  this->Implementation->UndoStackBuilder->SetIgnoreAllChanges(true);
}

//-----------------------------------------------------------------------------
void pqUndoStack::endNonUndoableChanges()
{
  bool ignore = false;
  if (this->Implementation->IgnoreAllChangesStack.size() > 0)
    {
    ignore = this->Implementation->IgnoreAllChangesStack.takeLast();
    }
  this->Implementation->UndoStackBuilder->SetIgnoreAllChanges(ignore);
}

//-----------------------------------------------------------------------------
bool pqUndoStack::ignoreAllChanges() const
{
  return this->Implementation->UndoStackBuilder->GetIgnoreAllChanges();
}

//-----------------------------------------------------------------------------
void pqUndoStack::setActiveServer(pqServer* server)
{
  if (server)
    {
    this->endNonUndoableChanges();
    }
  else
    {
    this->beginNonUndoableChanges();
    }
}

//-----------------------------------------------------------------------------
bool pqUndoStack::getInUndo() const
{
  return this->Implementation->UndoStack->GetInUndo();
}

//-----------------------------------------------------------------------------
bool pqUndoStack::getInRedo() const
{
  return this->Implementation->UndoStack->GetInRedo();
}
