/*=========================================================================

   Program:   ParaView
   Module:    pqPipelineModel.cxx

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

#include "pqPipelineModel.h"

#include "pqApplicationCore.h"
#include "pqDisplayPolicy.h"
#include "pqOutputPort.h"
#include "pqPipelineFilter.h"
#include "pqPipelineSource.h"
#include "pqServer.h"
#include "pqServerManagerModel.h"
#include "pqServerManagerObserver.h"
#include "pqSpreadSheetView.h"
#include "pqUndoStack.h"
#include "pqXYBarChartView.h"
#include "pqXYChartView.h"

#include <QApplication>
#include <QString>
#include <QStyle>
#include <QtDebug>
#include "QVTKWidget.h"

#include "vtkPVXMLElement.h"

//-----------------------------------------------------------------------------
class pqPipelineModelDataItem : public QObject
{
  bool InConstructor;
public:
   enum IconType
    {
    SERVER,
    LINK,
    GEOMETRY,
    BARCHART,
    LINECHART,
    TABLE,
    INDETERMINATE,
    EYEBALL,
    EYEBALL_GRAY,
    LAST
    };
public:
  pqPipelineModel* Model;
  pqPipelineModelDataItem* Parent;
  QList<pqPipelineModelDataItem*> Children;
  pqServerManagerModelItem* Object;
  pqPipelineModel::ItemType Type;
  IconType VisibilityIcon;
  bool Selectable;

  // This is a terrible iVar, agreed. But it makes my life easier.
  // This is valid only for elements of Type==Proxy. These refer to the link
  // items present for this item, if any. This list is automatically kept
  // updated.
  QList<pqPipelineModelDataItem*> Links;

  pqPipelineModelDataItem(QObject* p,
    pqServerManagerModelItem* object,
    pqPipelineModel::ItemType itemType,
    pqPipelineModel* model) :QObject(p)
    {
    this->InConstructor = true;
    this->Selectable = true;
    this->Model = model;
    this->Parent = NULL;
    this->Object = object;
    this->Type = itemType;
    this->VisibilityIcon = LAST;
    if (itemType == pqPipelineModel::Link)
      {
      pqPipelineModelDataItem* proxyItem = model->getDataItem(
        object, NULL, pqPipelineModel::Proxy);
      Q_ASSERT(proxyItem != 0);
      proxyItem->Links.push_back(this);
      }
    if (this->Object)
      {
      this->updateVisibilityIcon(this->Model->view(), false);
      }
    this->InConstructor = false;
    }
  ~pqPipelineModelDataItem()
    {
    if (this->Type == pqPipelineModel::Link && this->Model->Internal)
      {
      pqPipelineModelDataItem* proxyItem = this->Model->getDataItem(
        this->Object, NULL, pqPipelineModel::Proxy);
      if (proxyItem)
        {
        proxyItem->Links.removeAll(this);
        }
      }
    }

  pqPipelineModelDataItem& operator=(const pqPipelineModelDataItem& other)
    {
    this->Object = other.Object;
    this->Type = other.Type;
    this->VisibilityIcon = other.VisibilityIcon;
    foreach (pqPipelineModelDataItem* otherChild, other.Children)
      {
      pqPipelineModelDataItem* child = new pqPipelineModelDataItem(this,
        NULL, pqPipelineModel::Invalid , this->Model);
      this->addChild(child);
      *child = *otherChild;
      }
    return *this;
    }

  // no need to call this generally. Only needed when operator = is used.
  void updateLinks()
    {
    if (this->Type == pqPipelineModel::Link)
      {
      pqPipelineModelDataItem* proxyItem = this->Model->getDataItem(
        this->Object, NULL, pqPipelineModel::Proxy);
      Q_ASSERT(proxyItem != 0);
      proxyItem->Links.push_back(this);
      }
    foreach (pqPipelineModelDataItem* child, this->Children)
      {
      child->updateLinks();
      }
    }

  pqPipelineModel::ItemType getType()
    {
    return this->Type;
    }
  int getIndexInParent()
    {
    if (!this->Parent)
      {
      return 0;
      }
    return this->Parent->Children.indexOf(this);
    }

  IconType getIconType() const
    {
    switch (this->Type)
      {
    case pqPipelineModel::Server:
      return SERVER;

    case pqPipelineModel::Proxy:
        {
        pqPipelineSource* source = qobject_cast<pqPipelineSource*>(
            this->Object);
        if (source->getNumberOfOutputPorts() > 1)
          {
          // icon is shown on the output ports.
          return INDETERMINATE;
          }
        return this->getIconType(source->getOutputPort(0));
        }

    case pqPipelineModel::Port:
        {
        pqOutputPort* port = qobject_cast<pqOutputPort*>(this->Object);
        return this->getIconType(port);
        }

    case pqPipelineModel::Link:
        return LINK;

    case pqPipelineModel::Invalid:
        return INDETERMINATE;
      }
    return INDETERMINATE;
    }

  void addChild(pqPipelineModelDataItem* child)
    {
    if (child->Parent)
      {
      qCritical() << "child has parent.";
      return;
      }
    child->setParent(this);
    child->Parent = this;
    this->Children.push_back(child);
    }

  void removeChild(pqPipelineModelDataItem* child)
    {
    if (child->Parent != this)
      {
      qCritical() << "Cannot remove a non-child.";
      return;
      }
    child->setParent(NULL);
    child->Parent = NULL;
    this->Children.removeAll(child);
    }

  // returns true when the icon has changed.
  bool updateVisibilityIcon(pqView* view, bool traverse_subtree)
    {
    IconType newIcon = LAST;
    switch (this->Type)
      {
    case pqPipelineModel::Proxy:
    case pqPipelineModel::Link:
        {
        pqPipelineSource* source =
          qobject_cast<pqPipelineSource*>(this->Object);
        if (source && source->getNumberOfOutputPorts() == 1)
          {
          newIcon = this->getVisibilityIcon(source->getOutputPort(0), view);
          }
        }
      break;

    case pqPipelineModel::Port:
        {
        pqOutputPort* port = qobject_cast<pqOutputPort*>(this->Object);
        newIcon = this->getVisibilityIcon(port, view);
        }
      break;

    default:
      break;
      }

    bool ret_val = false;
    if (this->VisibilityIcon != newIcon)
      {
      this->VisibilityIcon = newIcon;
      if (!this->InConstructor && this->Model)
        {
        this->Model->itemDataChanged(this);
        }
      ret_val = true;
      }
    if (traverse_subtree)
      {
      foreach (pqPipelineModelDataItem* child, this->Children)
        {
        child->updateVisibilityIcon(view, traverse_subtree);
        }
      }
    return ret_val;
    }

  bool isModified() const
    {
    pqProxy* proxy = qobject_cast<pqProxy*>(this->Object);
    return (proxy && (proxy->modifiedState() != pqProxy::UNMODIFIED));
    }

private:
  IconType getVisibilityIcon(pqOutputPort* port, pqView* view) const
    {
    // If no view is present, it implies that a suitable type of view
    // will be created.
    pqApplicationCore* core = pqApplicationCore::instance();
    pqDisplayPolicy* policy = core->getDisplayPolicy();
    if (policy)
      {
      switch (policy->getVisibility(view, port))
        {
      case pqDisplayPolicy::Visible:
        return EYEBALL;

      case pqDisplayPolicy::Hidden:
        return EYEBALL_GRAY;

      case pqDisplayPolicy::NotApplicable:
      default:
        break;
        }
      }
    return LAST;
    }
  IconType getIconType(pqOutputPort* port) const
    {
    pqApplicationCore* core = pqApplicationCore::instance();
    pqDisplayPolicy* policy = core->getDisplayPolicy();
    if (policy)
      {
      QString type = policy->getPreferredViewType(
        port, false);
      if (type == pqXYBarChartView::XYBarChartViewType())
        {
        return BARCHART;
        }
      if (type == pqXYChartView::XYChartViewType())
        {
        return LINECHART;
        }
      if (type == pqSpreadSheetView::spreadsheetViewType())
        {
        return TABLE;
        }
      }
    return GEOMETRY;
    }
};

//-----------------------------------------------------------------------------
class pqPipelineModelInternal
{
public:
  pqPipelineModelInternal(pqPipelineModel* parent):
    Root(parent, NULL, pqPipelineModel::Invalid, parent)
  {
  this->ModifiedFont.setBold(true);
  }

  QFont ModifiedFont;
  pqPipelineModelDataItem Root;
};


//-----------------------------------------------------------------------------
void pqPipelineModel::constructor()
{
  this->Internal = new pqPipelineModelInternal(this);
  this->Editable = true;
  this->View = NULL;

  // Initialize the pixmap list.
  this->PixmapList = new QPixmap[pqPipelineModelDataItem::LAST+1];

  this->PixmapList[pqPipelineModelDataItem::SERVER].load(
    ":/pqWidgets/Icons/pqServer16.png");
  this->PixmapList[pqPipelineModelDataItem::LINK].load(
    ":/pqWidgets/Icons/pqLinkBack16.png");
  this->PixmapList[pqPipelineModelDataItem::GEOMETRY].load(
    ":/pqWidgets/Icons/pq3DView16.png");
  this->PixmapList[pqPipelineModelDataItem::BARCHART].load(
    ":/pqWidgets/Icons/pqHistogram16.png");
  this->PixmapList[pqPipelineModelDataItem::LINECHART].load(
    ":/pqWidgets/Icons/pqLineChart16.png");
  this->PixmapList[pqPipelineModelDataItem::TABLE].load(
    ":/pqWidgets/Icons/pqSpreadsheet16.png");
  this->PixmapList[pqPipelineModelDataItem::INDETERMINATE].load(
    ":/pqWidgets/Icons/pq3DView16.png");
  this->PixmapList[pqPipelineModelDataItem::EYEBALL].load(
    ":/pqWidgets/Icons/pqEyeball16.png");
  this->PixmapList[pqPipelineModelDataItem::EYEBALL_GRAY].load(
    ":/pqWidgets/Icons/pqEyeballd16.png");
}

//-----------------------------------------------------------------------------
pqPipelineModel::pqPipelineModel(QObject *p)
  : QAbstractItemModel(p)
{
  this->constructor();
}

//-----------------------------------------------------------------------------
pqPipelineModel::pqPipelineModel(
  const pqPipelineModel &other,
  QObject *parentObject)
: QAbstractItemModel(parentObject)
{
  this->constructor();
  this->Internal->Root = other.Internal->Root;
  this->Internal->Root.updateLinks();
}

//-----------------------------------------------------------------------------
pqPipelineModel::pqPipelineModel(const pqServerManagerModel &other,
    QObject *parentObject)
  : QAbstractItemModel(parentObject)
{
  this->constructor();

  // Build a pipeline model from the current server manager model.
  QList<pqPipelineSource *> sources;
  QList<pqPipelineSource *>::Iterator source;
  QList<pqServer *> servers = other.findItems<pqServer*>();
  QList<pqServer *>::Iterator server = servers.begin();
  for( ; server != servers.end(); ++server)
    {
    // Add the server to the model.
    this->addServer(*server);

    // Add the sources for the server.
    sources = other.findItems<pqPipelineSource *>(*server);
    for(source = sources.begin(); source != sources.end(); ++source)
      {
      this->addSource(*source);
      }

    // Set up the pipeline connections.
    for(source = sources.begin(); source != sources.end(); ++source)
      {
      int numOutputPorts = (*source)->getNumberOfOutputPorts();
      for(int port = 0; port < numOutputPorts; port++)
        {
        int numConsumers = (*source)->getNumberOfConsumers(port);
        for(int i = 0; i < numConsumers; ++i)
          {
          this->addConnection(*source, (*source)->getConsumer(port, i), port);
          }
        }
      }
    }
}

//-----------------------------------------------------------------------------
pqPipelineModel::~pqPipelineModel()
{
  // setting this->Internal to NULL keeps the ~pqPipelineModelDataItem() from
  // trying to update the link connections.
  pqPipelineModelInternal* internal = this->Internal;
  this->Internal = NULL;
  delete internal;

  if (this->PixmapList)
    {
    delete [] this->PixmapList;
    }
}

//-----------------------------------------------------------------------------
pqPipelineModel::ItemType pqPipelineModel::getTypeFor(
    const QModelIndex &idx) const
{
  if (idx.isValid() && idx.model() == this)
    {
    pqPipelineModelDataItem *item = reinterpret_cast<pqPipelineModelDataItem*>(
      idx.internalPointer());
    return item->getType();
    }

  return pqPipelineModel::Invalid;
}

//-----------------------------------------------------------------------------
void pqPipelineModel::setSelectable(const QModelIndex &idx, bool selectable)
{
  if(idx.isValid() && idx.model() == this)
    {
    pqPipelineModelDataItem*item = reinterpret_cast<pqPipelineModelDataItem*>(
      idx.internalPointer());
    item->Selectable = selectable;
    }
}

//-----------------------------------------------------------------------------
void pqPipelineModel::setSubtreeSelectable(pqServerManagerModelItem *smitem,
  bool selectable)
{
  pqOutputPort* port = qobject_cast<pqOutputPort*>(smitem);
  if (port && port->getSource())
    {
    smitem = port->getSource();
    }

  pqPipelineModelDataItem* item;
  pqServer* server = qobject_cast<pqServer*>(smitem);
  if (server)
    {
    item = this->getDataItem(smitem, &this->Internal->Root, pqPipelineModel::Server);
    }
  else
    {
    item = this->getDataItem(smitem,
      &this->Internal->Root, pqPipelineModel::Proxy);
    }
  this->setSubtreeSelectable(item, selectable);
}

//-----------------------------------------------------------------------------
void pqPipelineModel::setSubtreeSelectable(pqPipelineModelDataItem* item,
  bool selectable)
{
  if (item)
    {
    item->Selectable = selectable;
    foreach (pqPipelineModelDataItem* link, item->Links)
      {
      link->Selectable = selectable;
      }
    foreach (pqPipelineModelDataItem* child, item->Children)
      {
      this->setSubtreeSelectable(child, selectable);
      }
    }
}

//-----------------------------------------------------------------------------
bool pqPipelineModel::isSelectable(const QModelIndex &idx) const
{
  if(idx.isValid() && idx.model() == this)
    {
    pqPipelineModelDataItem *item = reinterpret_cast<pqPipelineModelDataItem*>(
        idx.internalPointer());
    return item->Selectable;
    }

  return false;
}
//-----------------------------------------------------------------------------
QModelIndex pqPipelineModel::getNextIndex(const QModelIndex idx,
    const QModelIndex &root) const
{
  // If the index has children, return the first child.
  if(this->rowCount(idx) > 0)
    {
    return this->index(0, 0, idx);
    }

  // Search up the parent chain for an index with more children.
  QModelIndex current = idx;
  while(current.isValid() && current != root)
    {
    QModelIndex parentIndex = current.parent();
    if(current.row() < this->rowCount(parentIndex) - 1)
      {
      return this->index(current.row() + 1, 0, parentIndex);
      }

    current = parentIndex;
    }

  return QModelIndex();
}

//-----------------------------------------------------------------------------
int pqPipelineModel::rowCount(const QModelIndex &parentIndex) const
{
  if (parentIndex.isValid() && parentIndex.model() == this)
    {
    pqPipelineModelDataItem *item = reinterpret_cast<pqPipelineModelDataItem*>(
      parentIndex.internalPointer());
    return item->Children.size();
    }
  return this->Internal->Root.Children.size();
}

//-----------------------------------------------------------------------------
int pqPipelineModel::columnCount(const QModelIndex &) const
{
  return 2;
}

//-----------------------------------------------------------------------------
bool pqPipelineModel::hasChildren(const QModelIndex &parentIndex) const
{
  return this->rowCount(parentIndex) > 0;
}

//-----------------------------------------------------------------------------
QModelIndex pqPipelineModel::index(int row, int column,
    const QModelIndex &parentIndex) const
{
  // Make sure the row and column number is within range.
  int rows = this->rowCount(parentIndex);
  int columns = this->columnCount(parentIndex);
  if(row < 0 || row >= rows || column < 0 || column >= columns)
    {
    return QModelIndex();
    }

  pqPipelineModelDataItem* parentItem = 0;
  if(parentIndex.isValid())
    {
    parentItem = reinterpret_cast<pqPipelineModelDataItem*>(
      parentIndex.internalPointer());
    }
  else
    {
    // The parent refers to the model root.
    parentItem = &this->Internal->Root;
    }

  return this->createIndex(row, column, parentItem->Children[row]);
}

//-----------------------------------------------------------------------------
QModelIndex pqPipelineModel::parent(const QModelIndex &idx) const
{
  if(idx.isValid() && idx.model() == this)
    {
    pqPipelineModelDataItem *item = reinterpret_cast<pqPipelineModelDataItem*>(
        idx.internalPointer());

    pqPipelineModelDataItem* _parent = item->Parent;
    return this->getIndex(_parent);
    }

  return QModelIndex();
}

//-----------------------------------------------------------------------------
QVariant pqPipelineModel::data(const QModelIndex &idx, int role) const
{
  if (!idx.isValid() || idx.model() != this)
    {
    return QVariant();
    }

  pqPipelineModelDataItem *item = reinterpret_cast<pqPipelineModelDataItem*>(
    idx.internalPointer());

  pqServer *server = qobject_cast<pqServer*>(item->Object);
  pqPipelineSource *source = qobject_cast<pqPipelineSource *>(item->Object);
  pqOutputPort* port = qobject_cast<pqOutputPort*>(item->Object);
  switch (role)
    {
  case Qt::DisplayRole:
    if (idx.column() == 1)
      {
      return QIcon(this->PixmapList[item->VisibilityIcon]);
      }
    // *** don't break.

  case Qt::ToolTipRole:
  case Qt::EditRole:
    if (idx.column() == 0)
      {
      if(server)
        {
        return QVariant(server->getResource().toURI());
        }
      else if(source)
        {
        return QVariant(source->getSMName());
        }
      else if (port)
        {
        return port->getPortName();
        }
      else
        {
        qDebug() << "Cannot decide type.";
        }
      }
    break;

  case Qt::DecorationRole:
    if (idx.column() == 0 && this->PixmapList)
      {
      if (item && item->getType() != pqPipelineModel::Invalid)
        {
        return QVariant(this->PixmapList[item->getIconType()]);
        }
      }
    break;

  case Qt::FontRole:
      {
      if (idx.column() == 0 && item->isModified())
        {
        return qVariantFromValue<QFont>(this->Internal->ModifiedFont);
        }
      break;
      }

    }

  return QVariant();
}

//-----------------------------------------------------------------------------
bool pqPipelineModel::setData(const QModelIndex &idx, const QVariant &value,
    int)
{
  if(value.toString().isEmpty())
    {
    return false;
    }

  QString name = value.toString();
  pqPipelineSource *source = qobject_cast<pqPipelineSource *>(
      this->getItemFor(idx));
  if (source && source->getSMName() != name)
    {
    BEGIN_UNDO_SET(
      QString("Rename %1 to %2").arg(source->getSMName()).arg(name));
    source->rename(name);
    END_UNDO_SET();
    }

  return true;
}

//-----------------------------------------------------------------------------
Qt::ItemFlags pqPipelineModel::flags(const QModelIndex &idx) const
{
  Qt::ItemFlags _flags = Qt::ItemIsEnabled;

  if (idx.column() == 0)
    {
    pqPipelineModelDataItem *item = reinterpret_cast<pqPipelineModelDataItem*>(
      idx.internalPointer());
    if (item->Selectable)
      {
      _flags |= Qt::ItemIsSelectable;
      }

    if (this->Editable &&
      item->Type == pqPipelineModel::Proxy)
      {
      _flags |= Qt::ItemIsEditable;
      }
    }

  return _flags;
}

//-----------------------------------------------------------------------------
pqServerManagerModelItem* pqPipelineModel::getItemFor(const QModelIndex& idx) const
{
  if (idx.isValid() && idx.model() == this)
    {
    pqPipelineModelDataItem* item =reinterpret_cast<pqPipelineModelDataItem*>(
      idx.internalPointer());
    return item->Object;
    }
  return NULL;
}

//-----------------------------------------------------------------------------
QModelIndex pqPipelineModel::getIndexFor(pqServerManagerModelItem *item) const
{
  pqPipelineModelDataItem* dataItem = this->getDataItem(item,
    &this->Internal->Root);
  if (!dataItem)
    {
    pqOutputPort* port = qobject_cast<pqOutputPort*>(item);
    if (port && port->getSource()->getNumberOfOutputPorts() == 1)
      {
      return this->getIndexFor(port->getSource());
      }
    }
  return this->getIndex(dataItem);
}

//-----------------------------------------------------------------------------
pqPipelineModelDataItem* pqPipelineModel::getDataItem(pqServerManagerModelItem* item,
  pqPipelineModelDataItem* _parent = 0,
  pqPipelineModel::ItemType type /*= pqPipelineModel::Invalid*/
  ) const
{
  if (_parent == 0)
    {
    _parent = &this->Internal->Root;
    }

  if (!item)
    {
    return 0;
    }

  if (_parent->Object == item &&
    (type == pqPipelineModel::Invalid ||
     type == _parent->Type))
    {
    return _parent;
    }

  foreach(pqPipelineModelDataItem* child, _parent->Children)
    {
    pqPipelineModelDataItem* retVal = this->getDataItem(item, child, type);
    if (retVal &&
      (type == pqPipelineModel::Invalid ||
       type == retVal->Type))
      {
      return retVal;
      }
    }
  return 0;
}

//-----------------------------------------------------------------------------
QModelIndex pqPipelineModel::getIndex(pqPipelineModelDataItem* dataItem) const
{
  if (dataItem && dataItem->Parent)
    {
    int rowNo = dataItem->getIndexInParent();
    if (rowNo != -1)
      {
      return this->createIndex(rowNo, 0, dataItem);
      }
    }

  // QModelIndex() implies the ROOT.
  return QModelIndex();
}


//-----------------------------------------------------------------------------
void pqPipelineModel::addChild(pqPipelineModelDataItem* _parent,
  pqPipelineModelDataItem* child)
{
  if (!_parent || !child)
    {
    qDebug() << "addChild cannot have null arguments.";
    return;
    }

  QModelIndex parentIndex = this->getIndex(_parent);
  int row = _parent->Children.size();

  this->beginInsertRows(parentIndex, row, row);
  _parent->addChild(child);
  this->endInsertRows();

  if(row == 0)
    {
    emit this->firstChildAdded(parentIndex);
    }
}

//-----------------------------------------------------------------------------
void pqPipelineModel::removeChildFromParent(
  pqPipelineModelDataItem* child)
{
  if (!child)
    {
    qDebug() << "removeChild cannot have null arguments.";
    return;
    }

  pqPipelineModelDataItem* _parent = child->Parent;
  if (!_parent)
    {
    qDebug() << "cannot remove ROOT.";
    return;
    }

  QModelIndex parentIndex = this->getIndex(_parent);
  int row = child->getIndexInParent();

  this->beginRemoveRows(parentIndex, row, row);
  _parent->removeChild(child);
  this->endRemoveRows();
}



//-----------------------------------------------------------------------------
void pqPipelineModel::serverDataChanged()
{
  // TODO: we should determine which server data actually chnaged
  // and invalidate only that one. FOr now, just invalidate all.

  int max = this->Internal->Root.Children.size()-1;
  if (max >= 0)
    {
    QModelIndex minIndex = this->getIndex(this->Internal->Root.Children[0]);
    QModelIndex maxIndex = this->getIndex(this->Internal->Root.Children[max]);
    emit this->dataChanged(minIndex, maxIndex);
    }
}

//-----------------------------------------------------------------------------
void pqPipelineModel::itemDataChanged(pqPipelineModelDataItem* item)
{
  QModelIndex idx = this->getIndex(item);
  emit this->dataChanged(idx, idx);
}

//-----------------------------------------------------------------------------
void pqPipelineModel::addServer(pqServer *server)
{
  if(!server)
    {
    return;
    }

  pqPipelineModelDataItem* item = new pqPipelineModelDataItem(
    this, server, pqPipelineModel::Server, this);
  this->addChild(&this->Internal->Root, item);
  QObject::connect(server,
    SIGNAL(nameChanged(pqServerManagerModelItem*)),
    this, SLOT(updateData(pqServerManagerModelItem*)));
}

//-----------------------------------------------------------------------------
void pqPipelineModel::removeServer(pqServer *server)
{
  pqPipelineModelDataItem* item = this->getDataItem(server,
    &this->Internal->Root, pqPipelineModel::Server);

  if (!item)
    {
    qDebug() << "Requesting to remove a non-added server.";
    return;
    }

  this->removeChildFromParent(item);

  delete item;
}

//-----------------------------------------------------------------------------
void pqPipelineModel::addSource(pqPipelineSource* source)
{
  pqServer* server = source->getServer();
  pqPipelineModelDataItem* _parent = this->getDataItem(server,
    &this->Internal->Root, pqPipelineModel::Server);

  if (!_parent)
    {
    qDebug() << "Could not locate server on which the source is being added.";
    return;
    }

  pqPipelineModelDataItem* item = new pqPipelineModelDataItem(
    this, source, pqPipelineModel::Proxy, this);
  item->Object = source;
  item->Type = pqPipelineModel::Proxy; // source->getType();

  // Add the 'source' to the server.
  this->addChild(_parent, item);

  int numOutputPorts = source->getNumberOfOutputPorts();
  if (numOutputPorts > 1)
    {
    // add output-ports for this source.
    for (int cc=0; cc < numOutputPorts; cc++)
      {
      pqPipelineModelDataItem* opport = new pqPipelineModelDataItem(
        this, source->getOutputPort(cc), pqPipelineModel::Port, this);
      this->addChild(item, opport);
      }
    }

  QObject::connect(source,
    SIGNAL(visibilityChanged(pqPipelineSource*, pqDataRepresentation*)),
    this, SLOT(updateVisibility(pqPipelineSource*)));

  QObject::connect(source,
    SIGNAL(nameChanged(pqServerManagerModelItem*)),
    this, SLOT(updateData(pqServerManagerModelItem*)));
  QObject::connect(source,
    SIGNAL(modifiedStateChanged(pqServerManagerModelItem*)),
    this, SLOT(updateData(pqServerManagerModelItem*)));
}

//-----------------------------------------------------------------------------
void pqPipelineModel::removeSource(pqPipelineSource* source)
{
  pqPipelineModelDataItem* item = this->getDataItem(source,
    &this->Internal->Root, pqPipelineModel::Proxy);

  if (!item)
    {
    // qDebug() << "Requesting to remove a non-added source.";
    return;
    }

  while (item->Links.size() > 0)
    {
    pqPipelineModelDataItem* link = item->Links.last();
    this->removeChildFromParent(link);
    delete link;
    }

  this->removeChildFromParent(item);
  if (item->Children.size())
    {
    // Move the children to the server.
    pqServer* server = source->getServer();
    pqPipelineModelDataItem* _parent = this->getDataItem(server,
      &this->Internal->Root, pqPipelineModel::Server);
    if (!_parent)
      {
      _parent = &this->Internal->Root;
      }

    QList<pqPipelineModelDataItem*> childrenToMove;
    foreach(pqPipelineModelDataItem* child, item->Children)
      {
      if (child->Type == pqPipelineModel::Port)
        {
        childrenToMove.append(child->Children);
        }
      else
        {
        childrenToMove.push_back(child);
        }
      }

    foreach (pqPipelineModelDataItem* child, childrenToMove)
      {
      child->Parent = NULL;
      this->addChild(_parent, child);
      }
    }

  delete item;
}

//-----------------------------------------------------------------------------
void pqPipelineModel::addConnection(pqPipelineSource *source,
    pqPipelineSource *sink, int sourceOutputPort)
{
  if(!source || !sink)
    {
    qDebug() << "Cannot connect a null source or sink.";
    return;
    }

  // Note: this slot is invoked after the connection has been set.

  // If fanIn == 1, take the sink form the server list
  // and put it under the source.
  // If fanIn == 2, take the sink from  the under the source that it was put
  //  when fanIn == 1, replace that with a link. Add a new link for the new
  //  connection and put the source under the server list.
  // If fanIn > 2, just create a link for this new connection. We have already
  //  flagged it as a link when fanIn == 2.

  pqPipelineFilter* filter = qobject_cast<pqPipelineFilter*>(sink);
  if (!filter)
    {
    qDebug() << "Sink has to be a filter.";
    return;
    }

  pqPipelineModelDataItem* srcItem = this->getDataItem(source,
    &this->Internal->Root, pqPipelineModel::Proxy);
  pqPipelineModelDataItem* sinkItem = this->getDataItem(sink,
    &this->Internal->Root, pqPipelineModel::Proxy);
  if (!srcItem || !sinkItem)
    {
    qDebug() << "Connection involves a non-added source. Ignoring.";
    return;
    }

  if (source->getNumberOfOutputPorts() > 1)
    {
    srcItem = srcItem->Children[sourceOutputPort];
    }

  // NOTE-TO-SELF: Never use actual values of current connections from the
  // sources/filters -- think of change input dialog to know why.

  pqPipelineModelDataItem* currentParent = sinkItem->Parent;
  if (currentParent->Type == pqPipelineModel::Server &&
    sinkItem->Links.size() > 0)
    {
    // sink has previously been identified as a "fan-in". We simply, create a
    // new link object for it.
    pqPipelineModelDataItem* link =
      new pqPipelineModelDataItem(this, sink, pqPipelineModel::Link, this);
    this->addChild(srcItem, link);
    return;
    }

  if (currentParent->Type == pqPipelineModel::Proxy ||
    currentParent->Type == pqPipelineModel::Port)
    {

    // this filter had just 1 input previously, now it has 2. So we need to
    // upgrade it to a fan-in.
    pqPipelineModelDataItem* linkOld = new pqPipelineModelDataItem(
      this, sink, pqPipelineModel::Link, this);
    this->addChild(currentParent, linkOld);

    pqPipelineModelDataItem* link =
      new pqPipelineModelDataItem(this, sink, pqPipelineModel::Link, this);
    this->addChild(srcItem, link);

    pqServer* server = sink->getServer();
    pqPipelineModelDataItem* serverItem = this->getDataItem(server,
      &this->Internal->Root, pqPipelineModel::Server);

    this->removeChildFromParent(sinkItem);
    this->addChild(serverItem, sinkItem);
    return;
    }

  // Adding the first input connection for this sink.
  // Remove the sink from where ever.
  this->removeChildFromParent(sinkItem);

  // Add to the children of the source.
  this->addChild(srcItem, sinkItem);
}

//-----------------------------------------------------------------------------
void pqPipelineModel::removeConnection(pqPipelineSource *source,
    pqPipelineSource *sink, int sourceOutputPort)
{
  if(!source || !sink)
    {
    qDebug() << "Cannot disconnect a null source or sink.";
    return;
    }

  pqPipelineModelDataItem* sinkItem = this->getDataItem(sink,
    &this->Internal->Root, pqPipelineModel::Proxy);
  pqPipelineModelDataItem* srcItem = this->getDataItem(source,
    &this->Internal->Root, pqPipelineModel::Proxy);

  if (!sinkItem || !srcItem)
    {
    // qDebug() << "Connection involves a non-added source. Ignoring.";
    return;
    }

  // Note: this slot is invoked after the connection has been broken.

  if (sinkItem->Links.size() == 0)
    {
    // Simplest case, sink had just 1 input.
    pqServer* server = sink->getServer();
    pqPipelineModelDataItem* serverItem = this->getDataItem(server,
      &this->Internal->Root, pqPipelineModel::Server);
    if (!serverItem)
      {
      qDebug() << "Failed to locate data item for server.";
      return;
      }

    this->removeChildFromParent(sinkItem);
    this->addChild(serverItem, sinkItem);
    return;
    }


  if (source->getNumberOfOutputPorts() > 1)
    {
    srcItem = srcItem->Children[sourceOutputPort];
    }

  // Has a fan-in for sure.
  // Remove the link item under the source.
  pqPipelineModelDataItem* linkItem = this->getDataItem(sink,
    srcItem, pqPipelineModel::Link);
  Q_ASSERT(linkItem != 0);
  this->removeChildFromParent(linkItem);
  delete linkItem;

  // locate all links for sink. If number of links == 1, remove the link and
  // move the sink to where the link was.
  if (sinkItem->Links.size() == 1)
    {
    linkItem = sinkItem->Links[0];
    pqPipelineModelDataItem* parentItem = linkItem->Parent;
    this->removeChildFromParent(linkItem);
    delete linkItem;

    this->removeChildFromParent(sinkItem);

    this->addChild(parentItem, sinkItem);
    }
}

//-----------------------------------------------------------------------------
void pqPipelineModel::setView(pqView *newview)
{
  if (this->View == newview)
    {
    return;
    }
  this->View = newview;
  // update all VisibilityIcons.
  this->Internal->Root.updateVisibilityIcon(newview, true);
}

//-----------------------------------------------------------------------------
void pqPipelineModel::updateVisibility(pqPipelineSource* source)
{
  pqPipelineModelDataItem* item = this->getDataItem(source,
    &this->Internal->Root, pqPipelineModel::Proxy);
  if (item)
    {
    item->updateVisibilityIcon(this->View, false);
    foreach (pqPipelineModelDataItem* child, item->Children)
      {
      if (child->Type == Port)
        {
        child->updateVisibilityIcon(this->View, false);
        }
      }

    foreach (pqPipelineModelDataItem* link, item->Links)
      {
      link->updateVisibilityIcon(this->View, false);
      }
    }
}

//-----------------------------------------------------------------------------
void pqPipelineModel::updateData(pqServerManagerModelItem* source)
{
  pqPipelineModelDataItem* item = this->getDataItem(source,
    &this->Internal->Root, pqPipelineModel::Proxy);
  if (item)
    {
    item->updateVisibilityIcon(this->View, false);
    this->itemDataChanged(item);
    foreach (pqPipelineModelDataItem* link, item->Links)
      {
      item->updateVisibilityIcon(this->View, false);
      this->itemDataChanged(link);
      }
    }
}

//-----------------------------------------------------------------------------
void pqPipelineModel::setModifiedFont(const QFont& font)
{
  this->Internal->ModifiedFont = font;
}

