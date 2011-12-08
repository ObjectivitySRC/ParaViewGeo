/*=========================================================================

   Program: ParaView
   Module:    pqSpreadSheetViewModel.cxx

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

========================================================================*/
#include "pqSpreadSheetViewModel.h"

// Server Manager Includes.
#include "vtkEventQtSlotConnect.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkPVArrayInformation.h"
#include "vtkPVCompositeDataInformation.h"
#include "vtkPVCompositeDataInformationIterator.h"
#include "vtkPVDataInformation.h"
#include "vtkPVDataSetAttributesInformation.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSmartPointer.h"
#include "vtkSMInputProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSpreadSheetView.h"
#include "vtkStdString.h"
#include "vtkTable.h"
#include "vtkUnsignedIntArray.h"
#include "vtkVariant.h"
#include "vtkWeakPointer.h"
#include "vtkCellType.h"

// Qt Includes.
#include <QTimer>
#include <QItemSelectionModel>
#include <QtDebug>
#include <QPointer>

// ParaView Includes.
#include "pqSMAdaptor.h"
#include "pqDataRepresentation.h"
#include "pqOutputPort.h"
#include "pqPipelineSource.h"


static uint qHash(pqSpreadSheetViewModel::vtkIndex index)
{
  return qHash(index.Tuple[2]);
}

namespace
{
  const char* getCellTypeAsString(int cell_type)
    {
    switch (cell_type)
      {
    case VTK_EMPTY_CELL:
      return "Empty";
    case VTK_VERTEX:
      return "Vertex";
    case VTK_POLY_VERTEX:
      return "Poly-Vertex";
    case VTK_LINE:
      return "Line";
    case VTK_POLY_LINE:
      return "Poly-Line";
    case VTK_TRIANGLE:
      return "Triangle";
    case VTK_TRIANGLE_STRIP:
      return "Triangle-Strip";
    case VTK_POLYGON:
      return "Polygon";
    case VTK_PIXEL:
      return "Pixel";
    case VTK_QUAD:
      return "Quad";
    case VTK_TETRA:
      return "Tetrahedron";
    case VTK_VOXEL:
      return "Voxel";
    case VTK_HEXAHEDRON:
      return "Hexahedron";
    case VTK_WEDGE:
      return "Wedge";
    case VTK_PYRAMID:
      return "Pyramid";
    case VTK_PENTAGONAL_PRISM:
      return "Pentagonal-Prism";
    case VTK_HEXAGONAL_PRISM:
      return "Hexagonal-Prism";

    case VTK_QUADRATIC_EDGE:
      return "Quadratic-Edge";
    case VTK_QUADRATIC_TRIANGLE:
      return "Quadratic-Triangle";
    case VTK_QUADRATIC_QUAD:
      return "Quadratic-Quad";
    case VTK_QUADRATIC_TETRA:
      return "Quadratic-Tetrahedron";
    case VTK_QUADRATIC_HEXAHEDRON:
      return "Quadratic Hexahedron";
    case VTK_QUADRATIC_WEDGE:
      return "Quadratic Wedge";
    case VTK_QUADRATIC_PYRAMID:
      return "Quadratic Pyramid";
    case VTK_BIQUADRATIC_QUAD:
      return "Bi-Quadratic-Quad";
    case VTK_TRIQUADRATIC_HEXAHEDRON:
      return "Tri-Quadratic-Hexahedron";
    case VTK_QUADRATIC_LINEAR_QUAD:
      return "Quadratice-Linear-Quad";
    case VTK_QUADRATIC_LINEAR_WEDGE:
      return "Quadratic-Linear-Wedge";
    case VTK_BIQUADRATIC_QUADRATIC_WEDGE:
      return "Bi-Quadratic-Wedge";
    case VTK_BIQUADRATIC_QUADRATIC_HEXAHEDRON:
      return "Bi-Quadratic-Quadratic-Hexahedron";
    case VTK_BIQUADRATIC_TRIANGLE:
      return "Bi-Quadratic-Triangle";
    case VTK_CUBIC_LINE:
      return "Cubic-Line";

    case VTK_CONVEX_POINT_SET:
      return "Convex-Point-Set";

    case VTK_POLYHEDRON:
      return "Polyhedron";
    case VTK_PARAMETRIC_CURVE:
      return "Parametric-Curve";
    case VTK_PARAMETRIC_SURFACE:
      return "Parametric-Surface";
    case VTK_PARAMETRIC_TRI_SURFACE:
      return "Parametric-Tri-Surface";
    case VTK_PARAMETRIC_QUAD_SURFACE:
      return "Parametric-Quad-Surface";
    case VTK_PARAMETRIC_TETRA_REGION:
      return "Parametric-Tetra-Region";
    case VTK_PARAMETRIC_HEX_REGION:
      return "Parametric-Hex-Region";

    case VTK_HIGHER_ORDER_EDGE:
        return "Higher-Order-Edge";
    case VTK_HIGHER_ORDER_TRIANGLE:
        return "Higher-Order-Triangle";
    case VTK_HIGHER_ORDER_QUAD:
        return "Higher-Order-Quad";
    case VTK_HIGHER_ORDER_POLYGON:
        return "Higher-Order-Polygon";
    case VTK_HIGHER_ORDER_TETRAHEDRON:
        return "Higher-Order-Tetrahedron";
    case VTK_HIGHER_ORDER_WEDGE:
        return "Higher-Order-Wedge";
    case VTK_HIGHER_ORDER_PYRAMID:
        return "Higher-Order-Pyramid";
    case VTK_HIGHER_ORDER_HEXAHEDRON:
        return "Higher-Order-Hexahedron";
      }
    return "Unknown";
    }
}
//-----------------------------------------------------------------------------
class pqSpreadSheetViewModel::pqInternal
{
public:
  pqInternal(pqSpreadSheetViewModel* svmodel) :
  SelectionModel(svmodel)
  {
  this->Dirty = true;
  this->VTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  this->DecimalPrecision = 6;
  this->ActiveRegion[0] = this->ActiveRegion[1] = -1;
  this->VTKView = NULL;

  this->LastColumnCount = 0;
  this->LastRowCount = 0;
  }

  QItemSelectionModel SelectionModel;
  QTimer Timer;
  QTimer SelectionTimer;
  int DecimalPrecision;
  vtkIdType LastRowCount;
  vtkIdType LastColumnCount;

  int ActiveRegion[2];
  vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;
  QPointer<pqDataRepresentation> ActiveRepresentation;
  vtkWeakPointer<vtkSMProxy> ActiveRepresentationProxy;
  vtkSpreadSheetView *VTKView;
  bool Dirty;
};

//-----------------------------------------------------------------------------
pqSpreadSheetViewModel::pqSpreadSheetViewModel(vtkSMProxy* view,
  QObject* parentObject) : Superclass(parentObject)
{
  Q_ASSERT(view != NULL);
  this->ViewProxy = view;
  this->Internal = new pqInternal(this);
  this->Internal->VTKView = vtkSpreadSheetView::SafeDownCast(
    view->GetClientSideObject());

  this->Internal->VTKConnect->Connect(this->Internal->VTKView,
    vtkCommand::UpdateDataEvent,
    this, SLOT(forceUpdate()));

  this->Internal->VTKConnect->Connect(this->Internal->VTKView,
    vtkCommand::UpdateEvent,
    this, SLOT(onDataFetched(vtkObject*, unsigned long, void*, void*)));

  this->Internal->Timer.setSingleShot(true);
  this->Internal->Timer.setInterval(500);//milliseconds.
  QObject::connect(&this->Internal->Timer, SIGNAL(timeout()),
    this, SLOT(delayedUpdate()));

  this->Internal->SelectionTimer.setSingleShot(true);
  this->Internal->SelectionTimer.setInterval(10);//milliseconds.
  QObject::connect(&this->Internal->SelectionTimer, SIGNAL(timeout()),
    this, SLOT(triggerSelectionChanged()));

  QObject::connect(&this->Internal->SelectionModel,
    SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
    &this->Internal->SelectionTimer, SLOT(start()));
}

//-----------------------------------------------------------------------------
pqSpreadSheetViewModel::~pqSpreadSheetViewModel()
{
  delete this->Internal;
}

//-----------------------------------------------------------------------------
pqDataRepresentation* pqSpreadSheetViewModel::activeRepresentation() const
{
  return this->Internal->ActiveRepresentation;
}

//-----------------------------------------------------------------------------
vtkSMProxy* pqSpreadSheetViewModel::activeRepresentationProxy() const
{
  return (this->Internal->ActiveRepresentation?
    this->Internal->ActiveRepresentation->getProxy() :
    this->Internal->ActiveRepresentationProxy.GetPointer());
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::setActiveRepresentation(pqDataRepresentation* repr)
{
  this->Internal->ActiveRepresentation = repr;
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::setActiveRepresentationProxy(vtkSMProxy* repr)
{
  this->Internal->ActiveRepresentationProxy = repr;
}

//-----------------------------------------------------------------------------
vtkSpreadSheetView* pqSpreadSheetViewModel::GetView() const
{
  return this->Internal->VTKView;
}

//-----------------------------------------------------------------------------
int pqSpreadSheetViewModel::rowCount(const QModelIndex&) const
{
  return this->Internal->VTKView->GetNumberOfRows();
}

//-----------------------------------------------------------------------------
int pqSpreadSheetViewModel::columnCount(const QModelIndex&) const
{
  return this->Internal->VTKView->GetNumberOfColumns();
}

//-----------------------------------------------------------------------------
int pqSpreadSheetViewModel::getFieldType() const
{
  if (this->activeRepresentationProxy())
    {
    return vtkSMPropertyHelper(
      this->activeRepresentationProxy(), "FieldAssociation").GetAsInt();
    }
  return -1;
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::forceUpdate()
{
  this->Internal->ActiveRegion[0] = -1;
  this->Internal->ActiveRegion[1] = -1;
  this->Internal->SelectionModel.clear();
  this->Internal->Timer.stop();
  this->Internal->SelectionTimer.stop();

  vtkIdType &rows = this->Internal->LastRowCount;
  vtkIdType &columns = this->Internal->LastColumnCount;

  // We force a reset only when the rows/columns counts have changed, otherwise
  // we simply say the data changed. This avoid flickers on the screen when user
  // clicks on the view or apply a filter change etc.
  if (this->rowCount() != rows || this->columnCount() != columns)
    {
    rows = this->rowCount();
    columns = this->columnCount();
    this->reset();
    }
  else
    {
    if (rows && columns)
      {
      // we always invalidate header data, just to be on a safe side.
      emit this->headerDataChanged(Qt::Horizontal, 0, columns-1);
      emit this->dataChanged(this->index(0, 0), this->index(rows-1, columns-1));
      }
    }
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::delayedUpdate()
{
  if (this->Internal->ActiveRegion[0] >= 0)
    {
    this->Internal->VTKView->GetValue(this->Internal->ActiveRegion[0], 0);
    }
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::triggerSelectionChanged()
{
  emit this->selectionChanged(this->Internal->SelectionModel.selection());
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::setActiveRegion(int row_top, int row_bottom)
{
  this->Internal->ActiveRegion[0] = row_top;
  this->Internal->ActiveRegion[1] = row_bottom;
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::onDataFetched(
  vtkObject*, unsigned long, void*, void* call_data)
{
  vtkIdType block = *reinterpret_cast<vtkIdType*>(call_data);
  vtkIdType blockSize = vtkSMPropertyHelper(this->ViewProxy,
    "BlockSize").GetAsIdType();

  // We deliberately invalid 1 row extra on each sides to ensure that the
  // visible viewport is always updated.
  vtkIdType rowMin = blockSize* block-1;
  vtkIdType rowMax = blockSize*(block+1);
  if (rowMin < 0)
    {
    rowMin = 0;
    }

  if (rowMax >= this->rowCount())
    {
    rowMax = this->rowCount() - 1;
    }

  QModelIndex topLeft(this->index(rowMax, 0));
  QModelIndex bottomRight(this->index(rowMax, this->columnCount()-1));

  this->dataChanged(topLeft, bottomRight);
  // we always invalidate header data, just to be on a safe side.
  this->headerDataChanged(Qt::Horizontal, 0, this->columnCount()-1);
}

//-----------------------------------------------------------------------------
QVariant pqSpreadSheetViewModel::data(
  const QModelIndex& idx, int role/*=Qt::DisplayRole*/) const
{
  if (role != Qt::DisplayRole)
    {
    return QVariant();
    }

  int row = idx.row();
  int column = idx.column();
  vtkSpreadSheetView * view = this->GetView();
  if (!view->IsAvailable(row))
    {
    this->Internal->Timer.start();
    return QVariant("...");
    }

  if (!this->isDataValid(idx))
    {
    // If displaying field data, check to make sure that the data is valid
    // since its arrays can be of different lengths
    return QVariant("");
    }

  vtkVariant value = view->GetValue(row, column);
  bool is_selected = view->IsRowSelected(row);
  if (is_selected)
    {
    this->Internal->SelectionModel.select(this->index(row, 0),
      QItemSelectionModel::Select|QItemSelectionModel::Rows);
    }

  QString str = value.ToString().c_str();
  if (value.IsChar() || value.IsUnsignedChar() || value.IsSignedChar())
    {
    // Don't show ASCII characted for char arrays.
    const char* column_name = view->GetColumnName(column);
    if (strcmp(column_name, "Cell Type")==0)
      {
      return getCellTypeAsString(value.ToInt());
      }
    else
      {
      str = QString::number(value.ToInt());
      }
    }
  else if(value.IsFloat() || value.IsDouble())
    {
    str = QString::number(value.ToDouble(), 'g',
      this->Internal->DecimalPrecision);
    }
  else if (value.IsArray())
    {
    // it's possible that it's a char array, then too we need to do the
    // number magic.
    vtkDataArray* array = vtkDataArray::SafeDownCast(value.ToArray());
    if (array)
      {
      switch(array->GetDataType())
        {
      case VTK_CHAR :
      case VTK_UNSIGNED_CHAR :
      case VTK_SIGNED_CHAR:
          {
          str = QString();
          for (vtkIdType cc=0; cc < array->GetNumberOfTuples(); cc++)
            {
            double *tuple = array->GetTuple(cc);
            for (vtkIdType kk=0; kk < array->GetNumberOfComponents(); kk++)
              {
              str += QString::number(static_cast<int>(tuple[kk])) + " ";
              }
            str = str.trimmed();
            }
          break;
          }
      case VTK_DOUBLE :
      case VTK_FLOAT :
          {
          str = QString();
          for (vtkIdType cc=0; cc < array->GetNumberOfTuples(); cc++)
            {
            double *tuple = array->GetTuple(cc);
            for (vtkIdType kk=0; kk < array->GetNumberOfComponents(); kk++)
              {
              str += QString::number(static_cast<double>(tuple[kk]), 'g',
                this->Internal->DecimalPrecision) + " ";
              }
            str = str.trimmed();
            }
          break;
          break;
          }
      default :
        break;
        }
      }
    }
  str.replace(" ", "\t");
  return str;
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::sortSection (int section, Qt::SortOrder order)
{
  vtkSpreadSheetView* view = this->GetView();
  if (view->GetNumberOfColumns() > section)
    {
    vtkSMPropertyHelper(this->ViewProxy, "ColumnToSort").Set(
      view->GetColumnName(section));
    switch(order)
      {
    case Qt::AscendingOrder:
      vtkSMPropertyHelper(this->ViewProxy, "InvertOrder").Set(1);
      break;
    case Qt::DescendingOrder:
      vtkSMPropertyHelper(this->ViewProxy, "InvertOrder").Set(0);
      break;
      }
    this->ViewProxy->UpdateVTKObjects();
    this->reset();
    }
}

//-----------------------------------------------------------------------------
bool pqSpreadSheetViewModel::isSortable(int section)
{
  vtkSpreadSheetView* view = this->GetView();
  if (view->GetNumberOfColumns() > section)
    {
    return strcmp(view->GetColumnName(section), "Structured Coordinates") != 0;
    }

  return false;
}

//-----------------------------------------------------------------------------
QVariant pqSpreadSheetViewModel::headerData (
  int section, Qt::Orientation orientation, int role/*=Qt::DisplayRole*/) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
    // No need to get updated data, simply get the current data.
    vtkSpreadSheetView* view = this->Internal->VTKView;
    if (view->GetNumberOfColumns() > section)
      {
      QString title = view->GetColumnName(section);
      // Convert names of some standard arrays to user-friendly ones.
      if (title == "vtkOriginalProcessIds")
        {
        title = "Process ID";
        }
      else if (title == "vtkOriginalIndices")
        {
        switch (this->getFieldType())
          {
        case vtkDataObject::FIELD_ASSOCIATION_POINTS:
          title  = "Point ID";
          break;

        case vtkDataObject::FIELD_ASSOCIATION_CELLS:
          title = "Cell ID";
          break;

        case vtkDataObject::FIELD_ASSOCIATION_VERTICES:
          title = "Vertex ID";
          break;

        case vtkDataObject::FIELD_ASSOCIATION_EDGES:
          title = "Edge ID";
          break;

        case vtkDataObject::FIELD_ASSOCIATION_ROWS:
          title = "Row ID";
          break;
          }
        }
      else if (title == "vtkOriginalCellIds" &&
        view->GetShowExtractedSelection())
        {
        title = "Cell ID";
        }
      else if (title == "vtkOriginalPointIds" &&
        view->GetShowExtractedSelection())
        {
        title = "Point ID";
        }
      else if (title == "vtkCompositeIndexArray")
        {
        title = "Block Number";
        }

      return QVariant(title);
      }
    return "__BUG__";
    }
  else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    {
    // Row number to start from 0.
    QVariant rowNo = this->Superclass::headerData(section, orientation, role);
    return QVariant(rowNo.toUInt()-1);
    }

  return this->Superclass::headerData(section, orientation, role);
}

//-----------------------------------------------------------------------------
QSet<pqSpreadSheetViewModel::vtkIndex> pqSpreadSheetViewModel::getVTKIndices(
  const QModelIndexList& indexes)
{
  // each variant in the vtkindices is 3 tuple
  // - (-1, pid, id) or
  // - (cid, pid, id) or
  // - (hlevel, hindex, id)
  QSet<vtkIndex> vtkindices;

  vtkSpreadSheetView* view = this->GetView();
  Q_ASSERT(view->GetShowExtractedSelection() == 0);

  vtkIdType numrows = view->GetNumberOfRows();
  foreach (QModelIndex idx, indexes)
    {
    vtkIdType row = idx.row();
    if (row >= numrows)
      {
      continue;
      }
    vtkIndex value;
    vtkVariant processId = view->GetValueByName(row, "vtkOriginalProcessIds");

    int pid = processId.IsValid()? processId.ToInt() : -1;
    value.Tuple[1] = pid;

    vtkVariant cid = view->GetValueByName(row, "vtkCompositeIndexArray");
    if (cid.IsValid())
      {
      // cid is either a single value (as uint) or a pair of values.
      // In the latter case the vtkVariant returns it as a vtkAbstractArray.
      if (cid.IsArray())
        {
        vtkUnsignedIntArray* array = vtkUnsignedIntArray::SafeDownCast(
          cid.ToArray());
        Q_ASSERT(array->GetNumberOfTuples()*array->GetNumberOfComponents() == 2);
        value.Tuple[0] = static_cast<vtkIdType>(array->GetValue(0));
        value.Tuple[1] = static_cast<vtkIdType>(array->GetValue(1));
        }
      else
        {
        value.Tuple[0] = cid.ToUnsignedInt();
        }
      }

    vtkVariant vtkindex = view->GetValueByName(row, "vtkOriginalIndices");
    value.Tuple[2] = static_cast<vtkIdType>(vtkindex.ToLongLong());
    vtkindices.insert(value);
    }

  return vtkindices;
}


//-----------------------------------------------------------------------------
bool pqSpreadSheetViewModel::isDataValid( const QModelIndex &idx) const
{
  if (this->getFieldType() != vtkDataObject::FIELD_ASSOCIATION_NONE)
    {
    return true;
    }

  // First make sure the index itself is valid
  pqDataRepresentation* repr = this->activeRepresentation();
  if (!idx.isValid() || repr == NULL)
    {
    return false;
    }

  vtkPVDataInformation* info = repr->getInputDataInformation();
  int field_type = this->getFieldType();

  // Get the appropriate attribute information object
  vtkPVDataSetAttributesInformation *attrInfo = info?
    info->GetAttributeInformation(field_type) : 0;

  if (attrInfo)
    {
    // Ensure that the row of this index is less than the length of the
    // data array associated with its column
    vtkPVArrayInformation *arrayInfo = attrInfo->GetArrayInformation(
      this->GetView()->GetColumnName(idx.column()));
    if (arrayInfo && idx.row() < arrayInfo->GetNumberOfTuples())
      {
      return true;
      }
    }

  return false;
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::resetCompositeDataSetIndex()
{
  if (!this->activeRepresentation())
    {
    return;
    }

  vtkSMProxy* reprProxy = this->activeRepresentation()->getProxy();
  int cur_index = vtkSMPropertyHelper(reprProxy,
    "CompositeDataSetIndex").GetAsInt();

  pqOutputPort* input_port = this->activeRepresentation()->getOutputPortFromInput();
  vtkSMSourceProxy* inputProxy = vtkSMSourceProxy::SafeDownCast(
    input_port->getSource()->getProxy());
  vtkSMSourceProxy* extractSelection = inputProxy->GetSelectionOutput(
    input_port->getPortNumber());
  if (!extractSelection)
    {
    return;
    }

  vtkPVDataInformation* mbInfo = extractSelection->GetDataInformation();
  if (!mbInfo || !mbInfo->GetCompositeDataClassName())
    {
    return;
    }

  vtkPVDataInformation* blockInfo = mbInfo->GetDataInformationForCompositeIndex(cur_index);
  if (blockInfo && blockInfo->GetNumberOfPoints() > 0)
    {
    return;
    }

  // find first index with non-empty points.
  vtkPVCompositeDataInformationIterator* iter = vtkPVCompositeDataInformationIterator::New();
  iter->SetDataInformation(mbInfo);
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
    vtkPVDataInformation* curInfo = iter->GetCurrentDataInformation();
    if (!curInfo || curInfo->GetCompositeDataClassName() != 0)
      {
      continue;
      }
    if (curInfo->GetDataSetType() != -1 && curInfo->GetNumberOfPoints() > 0)
      {
      cur_index = static_cast<int>(iter->GetCurrentFlatIndex());
      break;
      }
    }
  iter->Delete();

  vtkSMPropertyHelper(reprProxy, "CompositeDataSetIndex").Set(cur_index);
  reprProxy->UpdateVTKObjects();
}

//-----------------------------------------------------------------------------
void pqSpreadSheetViewModel::setDecimalPrecision(int dPrecision)
{
  if(this->Internal->DecimalPrecision != dPrecision)
    {
    this->Internal->DecimalPrecision = dPrecision;
    this->forceUpdate();
    }
}

//-----------------------------------------------------------------------------
int pqSpreadSheetViewModel::getDecimalPrecision()
{
  return this->Internal->DecimalPrecision;
}
