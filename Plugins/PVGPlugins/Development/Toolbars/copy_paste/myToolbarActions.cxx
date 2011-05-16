/*=========================================================================

   Program: ParaView
   Module:    $RCSfile: myToolbarActions.cxx,v $

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
#include "myToolbarActions.h"

#include <QApplication>
#include <QStyle>
#include <QtDebug>
#include <QList>
#include <QString>

#include "pqApplicationCore.h"
#include "pqObjectBuilder.h"
#include "pqServer.h"
#include "pqServerManagerModel.h"
#include "pqServerManagerSelectionModel.h"
#include "pqServerManagerModelItem.h"
#include "pqPipelineModel.h"
#include "pqServer.h"
#include "pqUndoStack.h"
#include "pqPipelineModel.h"
#include "pqPipelineSource.h"
#include "pqProxy.h"
#include "vtkSMProxy.h"
#include "vtkSMProperty.h"
#include "vtkSMProxyProperty.h"
#include "vtkSMInputProperty.h"
#include "vtkSMPropertyIterator.h"
#include "pqLinksModel.h"

void CopyProperties(vtkSMProxy* target, vtkSMProxy* src)
{
  vtkSMPropertyIterator* piter;
  piter = target->NewPropertyIterator();
  for(piter->Begin(); !piter->IsAtEnd(); piter->Next())
  {
    //copy everything except Input properties & Proxy properties
    if (!piter->GetProperty()->IsA("vtkSMInputProperty") && !piter->GetProperty()->IsA("vtkSMProxyProperty"))
    {
     vtkSMProperty* source = src->GetProperty(piter->GetKey());
     if (source)
     {
       piter->GetProperty()->Copy(source);
       piter->GetProperty()->SetImmediateUpdate(1);
       piter->GetProperty()->Modified();
     }
    }
    else
    {
      piter->GetProperty()->SetImmediateUpdate(1);
      piter->GetProperty()->Modified();
    }
  }
}

void CopyProxyProperties(pqPipelineSource* target, pqPipelineSource* src)
{
  vtkSMPropertyIterator* piter;
  piter = src->getProxy()->NewPropertyIterator();
  for(piter->Begin(); !piter->IsAtEnd(); piter->Next())
  {
    if (piter->GetProperty()->IsA("vtkSMProxyProperty"))
    {
      vtkSMProxyProperty* psource = (vtkSMProxyProperty*)piter->GetProperty();

      // get helperProxy ID
      int ID = -1;
      for (int i=0;i<src->getHelperProxies().size();i++)
      {
        if (src->getHelperProxies()[i] == psource->GetProxy(0))
        {
          ID = i;
          break;
        }
      }
      if (ID != -1)
      {
        vtkSMProxyProperty* ptarget = (vtkSMProxyProperty*)target->getProxy()->GetProperty(piter->GetKey());
        ptarget->SetProxy(0, target->getHelperProxies()[ID]);
      }
    }
  }
}

void RebuildPipeline(pqPipelineSource* target, pqPipelineSource* src,QList<pqPipelineSource*> FilterStack, pqObjectBuilder* builder)
{
  pqPipelineSource* newfilter;

  // create filter
  newfilter = builder->createFilter("filters",src->getProxy()->GetXMLName() , target);

  if (newfilter)
  {
    // copy HelperProxies
    for (int i=0;i<src->getHelperProxies().size();i++)
    {
      newfilter->getHelperProxies()[i]->Copy(src->getHelperProxies()[i]);
    }

    // copy ProxyProperties
    CopyProxyProperties(newfilter, src);

    // copy normal Properties
    CopyProperties(newfilter->getProxy(), src->getProxy());
  }

  //update Pipeline (Apply Button)
  newfilter->getProxy()->UpdateSelfAndAllInputs();
  newfilter->setModifiedState(pqProxy::MODIFIED);
  newfilter->getProxy()->Modified();

  // find next filter in pipeline
  for (int i=0;i<FilterStack.size();i++)
  {
    vtkSMInputProperty* pinput = (vtkSMInputProperty*)FilterStack[i]->getProxy()->GetProperty("Input");
    if (pinput)
    {
      if (pinput->GetProxy(0) == src->getProxy())
        //rekursiver Aufruf
        RebuildPipeline(newfilter, FilterStack[i], FilterStack, builder);
    }
  }

}

//-----------------------------------------------------------------------------
myToolbarActions::myToolbarActions(QObject* p) : QActionGroup(p)
{
  QIcon icon(QString("Copy_24.png"));
  this->FilterStack.clear();
  QAction* a = new QAction(icon, "Copy", this);
  a->setData("Copy");
  this->addAction(a);
  icon = QIcon(QString("Paste_24.png"));
  a = new QAction(icon, "Paste", this);
  a->setData("Paste");
  this->addAction(a);
  QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onAction(QAction*)));
}

//-----------------------------------------------------------------------------
void myToolbarActions::onAction(QAction* a)
{
  pqApplicationCore* core = pqApplicationCore::instance();
  pqObjectBuilder* builder = core->getObjectBuilder();
  pqServerManagerModel* sm = core->getServerManagerModel();
  pqServerManagerSelectionModel* sms = core->getSelectionModel();
  pqLinksModel* lm = core->getLinksModel();
  pqUndoStack* stack = core->getUndoStack();

  pqServer* serv;
  QList<pqPipelineSource*> PipelineItems;
  QList<vtkSMProxy*> helperProxies;
  pqPipelineSource* actSrc;
  pqPipelineSource* target;
  pqPipelineSource* newfilter;
  vtkSMProxy* actPrxy;
  vtkSMPropertyIterator* piter;


  /// Check that we are connect to some server (either builtin or remote).
  if (sm->getNumberOfItems<pqServer*>())
    {
      // just create it on the first server connection
      pqServer* s = sm->getItemAtIndex<pqServer*>(0);
      QString button_type = a->data().toString();

      // COPY
      if (button_type == "Copy")
      {
        // copy selected items to clipboard (= this->FilterStack)
        this->FilterStack.clear();
        PipelineItems = sm->findItems<pqPipelineSource*>();
        for (int i=0;i<PipelineItems.size();i++)
        {
          actSrc = PipelineItems[i];
          if (sms->isSelected(actSrc))
          {
            this->FilterStack.push_back(actSrc);
          }
        }
      }

      // PASTE
      if (button_type == "Paste")
      {
        // find selected source
        PipelineItems = sm->findItems<pqPipelineSource*>();
        for (int i=0;i<PipelineItems.size();i++)
        {
          actSrc = PipelineItems[i];
          if (sms->isSelected(actSrc))
          {
            target = actSrc;
            break;
          }
        }

        for (int i=0;i<this->FilterStack.size();i++)
        {
          // find filter without input in selected set (= first point
          // of the selected pipeline)
          int hasSource = 0;
          for (int j=0;j<this->FilterStack.size();j++)
          {
            vtkSMInputProperty* pinput = (vtkSMInputProperty*)this->FilterStack[i]->getProxy()->GetProperty("Input");
            if (pinput)
            {
              if (pinput->GetProxy(0) == FilterStack[j]->getProxy())
              {
                hasSource = 1;
                break;
              }
            }
            else
              hasSource = -1;
          }

          // first point of the selected pipeline found
          if (hasSource==0)
          {
            RebuildPipeline(target, FilterStack[i], FilterStack, builder);
          }
        }
      }
    }
}


