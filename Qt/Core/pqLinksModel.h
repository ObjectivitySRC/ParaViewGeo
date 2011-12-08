/*=========================================================================

   Program: ParaView
   Module:    pqLinksModel.h

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

#ifndef _pqLinksModel_h
#define _pqLinksModel_h


#include "pqCoreExport.h"
#include <QAbstractItemModel>

class pqProxy;
class pqRenderView;
class pqServer;
class pqServerManagerModelItem;
class vtkSMLink;
class vtkSMProxy;
class vtkSMProxyListDomain;

/// A Qt based model to represent the vtkSMLinks in the
/// server manager.
/// All links are bi-directional between two proxies.
class PQCORE_EXPORT pqLinksModel : public QAbstractTableModel
{
  Q_OBJECT
  typedef QAbstractTableModel Superclass;

public:
    /// type of link (camera, proxy or property)
  enum ItemType
    {
    Unknown,
    Proxy,
    Camera,
    Property
    };

public:
  /// construct a links model
  pqLinksModel(QObject *parent=0);

  /// destruct a links model
  ~pqLinksModel();

  // implementation to satisfy api
  /// the number of rows (number of links)
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  /// the number of columns
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  /// data for an index
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /// header data
  QVariant headerData(int section, Qt::Orientation orient, 
                      int role=Qt::DisplayRole) const;

  // subclass specific implementation
  /// get the type of link from model index
  ItemType getLinkType(const QModelIndex& idx) const;
  /// get the link from model index
  vtkSMLink* getLink(const QModelIndex& idx) const;
  /// search for a link and return model index
  QModelIndex findLink(vtkSMLink* link) const;

  /// get the first proxy for a link
  vtkSMProxy* getProxy1(const QModelIndex& idx) const;
  /// get the second proxy for a link
  vtkSMProxy* getProxy2(const QModelIndex& idx) const;
  
  /// get the first property for a link
  QString getProperty1(const QModelIndex& idx) const;
  /// get the second property for a link
  QString getProperty2(const QModelIndex& idx) const;
  
  /// get the name of a link
  QString getLinkName(const QModelIndex& idx) const;
  /// get the link from a name
  vtkSMLink* getLink(const QString& name) const;

  /// add a proxy based link
  void addProxyLink(const QString& name, 
                    vtkSMProxy* proxy1, vtkSMProxy* proxy2);
  
  /// add a camera based link
  void addCameraLink(const QString& name, 
                    vtkSMProxy* proxy1,
                    vtkSMProxy* proxy2);

  /// add a property based link
  void addPropertyLink(const QString& name,
                       vtkSMProxy* proxy1, const QString& prop1,
                       vtkSMProxy* proxy2, const QString& prop2);

  /// remove a link by index
  void removeLink(const QModelIndex& idx);
  /// remove a link by name
  void removeLink(const QString& name);

  /// Return a representative proxy.
  /// It could be itself, or in the case of internal proxies, the owning
  /// pqProxy.
  static pqProxy* representativeProxy(vtkSMProxy* proxy);
  
  /// return the proxy list domain for a proxy
  /// this domain is used to get internal linkable proxies
  static vtkSMProxyListDomain* proxyListDomain(vtkSMProxy* proxy);

protected slots:
  void onSessionCreated(pqServer*);
  void onSessionRemoved(pqServer*);

private:
  ItemType getLinkType(vtkSMLink* link) const;
  vtkSMProxy* getProxyFromIndex(const QModelIndex& idx, int dir) const;
  QString getPropertyFromIndex(const QModelIndex& idx, int dir) const;

  class pqInternal;
  pqInternal* Internal;
};

// internal class here for moc'ing reasons
class pqLinksModelObject : public QObject
{
  Q_OBJECT
public:
  pqLinksModelObject(QString name, pqLinksModel* p, pqServer*);
  ~pqLinksModelObject();

  QString name() const;
  vtkSMLink* link() const;

private slots:
  void proxyModified(pqServerManagerModelItem*);
  void refresh();
  void remove();

private:
  class pqInternal;
  pqInternal* Internal;

  void linkUndoStacks();
  void unlinkUndoStacks(pqRenderView*);
};

#endif

