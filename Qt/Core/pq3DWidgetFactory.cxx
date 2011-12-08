/*=========================================================================

   Program: ParaView
   Module:    pq3DWidgetFactory.cxx

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
#include "pq3DWidgetFactory.h"

// ParaView Server Manager includes.
#include "vtkSMNewWidgetRepresentationProxy.h"
#include "vtkSmartPointer.h"
#include "vtkSMProxyManager.h"

// Qt Includes.
#include <QList>
#include <QtDebug>

// ParaView includes.
#include "pqApplicationCore.h"
#include "pqObjectBuilder.h"
#include "pqServerManagerObserver.h"
#include "pqServer.h"

class pq3DWidgetFactoryInternal
{
public:
  typedef QList<vtkSmartPointer<vtkSMNewWidgetRepresentationProxy> > ListOfWidgetProxies;
  ListOfWidgetProxies Widgets;
  ListOfWidgetProxies WidgetsInUse;
};

//-----------------------------------------------------------------------------
pq3DWidgetFactory::pq3DWidgetFactory(QObject* _parent/*=null*/)
: QObject(_parent)
{
  this->Internal = new pq3DWidgetFactoryInternal();
  QObject::connect(pqApplicationCore::instance()->getServerManagerObserver(),
    SIGNAL(proxyUnRegistered(QString, QString, vtkSMProxy*)), this, 
    SLOT(proxyUnRegistered(QString, QString, vtkSMProxy*)));
}

//-----------------------------------------------------------------------------
pq3DWidgetFactory::~pq3DWidgetFactory()
{
  delete this->Internal;
}

//-----------------------------------------------------------------------------
vtkSMNewWidgetRepresentationProxy* pq3DWidgetFactory::get3DWidget(const QString& name,
    pqServer* server)
{
  pq3DWidgetFactoryInternal::ListOfWidgetProxies::iterator iter =
    this->Internal->Widgets.begin();
  for (; iter != this->Internal->Widgets.end(); iter++)
    {
    vtkSMNewWidgetRepresentationProxy* proxy = iter->GetPointer();
    if (proxy && proxy->GetSession() == server->session() &&
      name == proxy->GetXMLName())
      {
      this->Internal->WidgetsInUse.push_back(proxy);
      this->Internal->Widgets.erase(iter);
      return proxy;
      }
    }

  pqObjectBuilder* builder = 
    pqApplicationCore::instance()->getObjectBuilder();

  // We register  the 3DWidget proxy under prototypes so that it
  // is never saved in state
  vtkSMNewWidgetRepresentationProxy* proxy = vtkSMNewWidgetRepresentationProxy::SafeDownCast(
    builder->createProxy("representations", 
      name.toAscii().data(), server,
      "3d_widgets_prototypes"));
  if (!proxy)
    {
    qDebug() << "Could not create the 3D widget with name: " << name;
    return NULL;
    }
  this->Internal->WidgetsInUse.push_back(proxy);
  return proxy;
}

//-----------------------------------------------------------------------------
void pq3DWidgetFactory::free3DWidget(vtkSMNewWidgetRepresentationProxy* widget)
{
  pq3DWidgetFactoryInternal::ListOfWidgetProxies::iterator iter =
    this->Internal->WidgetsInUse.begin();
  for (; iter != this->Internal->WidgetsInUse.end(); iter++)
    {
    vtkSMNewWidgetRepresentationProxy* proxy = iter->GetPointer();
    if (proxy == widget)
      {
      this->Internal->Widgets.push_back(proxy);
      this->Internal->WidgetsInUse.erase(iter);
      return;
      }
    }
  // qDebug() << "free3DWidget called on a free widget on a widget not managed"
  // " by this class.";
}

//-----------------------------------------------------------------------------
void pq3DWidgetFactory::proxyUnRegistered(QString group, 
  QString vtkNotUsed(name), vtkSMProxy* proxy)
{
  vtkSMNewWidgetRepresentationProxy* widget;
  if (group != "3d_widgets_prototypes" || 
    (widget = vtkSMNewWidgetRepresentationProxy::SafeDownCast(proxy)) == 0)
    {
    return;
    }
  // Check if the unregistered proxy is the one managed by this class.
  pq3DWidgetFactoryInternal::ListOfWidgetProxies::iterator iter;
  for (iter = this->Internal->WidgetsInUse.begin(); 
    iter != this->Internal->WidgetsInUse.end(); iter++)
    {
    if (iter->GetPointer() == widget)
      {
      this->Internal->WidgetsInUse.erase(iter);
      break;
      }
    }

  for (iter = this->Internal->Widgets.begin();
    iter != this->Internal->Widgets.end(); ++iter)
    {
    if (iter->GetPointer() == widget)
      {
      this->Internal->Widgets.erase(iter);
      break;
      }
    }
}
