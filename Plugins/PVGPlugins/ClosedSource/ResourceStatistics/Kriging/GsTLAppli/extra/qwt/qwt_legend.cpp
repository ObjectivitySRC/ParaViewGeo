/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

/*-*- c++ -*-******************************************************************
 * Qwt Widget Library 
 * Copyright (C) 1997   Josef Wilgen
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *****************************************************************************/

#include "qwt_legend.h"

#ifndef QWT_NO_LEGEND

#include <qpainter.h>
#include "qwt_math.h"

static const int MinSymWidth = 5;
static const int MinLblWidth = 50;
static const int MinHBdrDist = 6;
static const int MinVBdrDist = 4;

/*!
  \brief Constructor
  \param txt Item text
  \param sym Item symbol
  \param pen Pen
  \param key Unique key for this item
*/
QwtLegendItem::QwtLegendItem(const QString &txt, const QwtSymbol &sym, 
        const QPen &pn, long key): 
    d_pen(pn), 
    d_sym(sym), 
    d_text(txt),  
    d_key(key) 
{
}

//! Default Constructor
QwtLegendItem::QwtLegendItem(): 
    d_key(0)
{
}

//! Destructor
QwtLegendItem::~QwtLegendItem()
{
}

/*!
  \brief Draw a legend item.
*/
void QwtLegendItem::draw(QPainter *p, 
    const QRect& symRect, const QRect& lblRect, int align)
{
    int yl = symRect.top() + symRect.height() / 2;
    p->save();
    
    p->drawText(lblRect, align, d_text);
    p->setPen(d_pen);
    p->drawLine(symRect.x(), yl, symRect.right(), yl);
    d_sym.draw(p, symRect.x() + symRect.width() / 2 - d_sym.size().width() / 2,
               yl - d_sym.size().height() / 2);
    p->restore();
    
}

/*!
  \brief Constructor
  \param p Parent widget
  \param name Widget name
*/
QwtLegend::QwtLegend(QWidget *p, const char *name): 
    QTable(0, 0, p,name)
{
    // A QwtLegend is more or less a grid of curve-labeled
    // flat push buttons. In the original implementation (qt 1.2x)
    // this was done with QTableView that died with qt 3.x. 
    // This implementation is simply a port to the QTable class.


    // Colors that look like pushbuttons.

    QPalette myPalette = palette();
    for ( int i = 0; i < QPalette::NColorGroups; i++ )
    {
        QPalette::ColorGroup cg = (QPalette::ColorGroup)i;
        myPalette.setColor(cg, QColorGroup::Base, 
            myPalette.color(cg, QColorGroup::Background));
    }
    setPalette(myPalette);

    // No horizontal and vertical header, no grid, no frame

    setFrameStyle(NoFrame);
    horizontalHeader()->hide();
    verticalHeader()->hide();

    setTopMargin(0);
    setLeftMargin(0);
    setShowGrid(FALSE);

    // ---

    d_maxCols = 5;
    d_maxRows = 0;
    d_sel.active = FALSE;
    d_sel.r = 0;
    d_sel.c = 0;
    d_item.setAutoDelete(TRUE);
    d_align = int(AlignLeft|AlignVCenter);
}


//! Destructor
QwtLegend::~QwtLegend()
{
    d_item.clear();
}


//! Remove all items from a legend
void QwtLegend::clear()
{
    d_item.clear();
}

/*!
  \brief Insert a new item at a specified position
  \param txt Label 
  \param s Symbol
  \param line pen
  \param key key
  \param  index position where to insert the item
*/
bool QwtLegend::insertItem(const QString &txt, const QwtSymbol &s, 
    const QPen &p, long key,  uint index)
{
    QwtLegendItem *newItem = new QwtLegendItem(txt, s, p, key);
    if (newItem) 
    {
        d_item.insert(index, newItem);
        updateTable();
        return TRUE;
    }
    else
       return FALSE;
}

/*!
  \brief Append a new item to the legend
  \param txt Label
  \param s Symbol
  \param  Line pen
  \param key Key
  \return The new size of the list
*/
uint QwtLegend::appendItem(const QString &txt, const QwtSymbol &s,
    const QPen &p, long key)
{
    QwtLegendItem *newItem = new QwtLegendItem(txt, s, p, key);
    if (newItem)
    {
        d_item.append(newItem);
        updateTable();
    }
    return d_item.count();
}

/*!
  \brief Update the cell dimendions and the table dimensions

  This function combines QwtLegend::resizeCells()
  and QwtLegend::resizeTable()
*/
void QwtLegend::updateTable()
{
    resizeTable();
    resizeCells();
}

//! Paint the contents of a cell

void QwtLegend::paintCell(QPainter * p, int row, int col, 
        const QRect &, bool) 
{
    const uint index = findIndex(row, col);
    
    QRect rect(0, 0, colWidth(), rowHeight());
    p->fillRect(rect, colorGroup().base());

    if (index < d_item.count())
    {
        if (d_sel.active && (d_sel.r == row) && (d_sel.c == col) )
           p->fillRect(rect, colorGroup().light());

        QwtLegendItem *curItem = d_item.at(index);
        if (curItem)
           curItem->draw(p, d_symRect, d_lblRect, d_align);
    }
}

/*!
  \brief Empty implementation to disable QTable behaviour
*/
void QwtLegend::paintFocus(QPainter *, const QRect &)
{
    return; // no Focus
}

/*!
  \brief Return the index of an item with specified row and column
  \param row Row
  \param col Column
*/
uint QwtLegend::findIndex(int row, int col)
{
    uint rv;
    if (d_maxCols)
       rv = uint( row * numCols() + col);
    else
       rv = uint( col * numRows() + row);
    
    return qwtMin(d_item.count(), rv);
}

/*!
  \brief Set the maximum number of columns and the layout policy.

  The setMaxCols and setMaxRows members determine in which order 
  the cells are arranged in the table. If a maximum number of
  columns is set, the legend items will be arranged in rows from
  the left to the right,
  starting at the first row, and beginning a new row when the
  actual number of columns has reached that maximum. The number of
  rows is thereby unlimited.

  The default setting is a maximum of 5 columns with an unlimited
  number of rows.

  \param n max. number of columns
  \warning setMaxRows and setMaxCols are mutually exclusive. The last one
        wins.
*/
void QwtLegend::setMaxCols(int n)
{
    d_maxRows = 0;
    d_maxCols = qwtMax(n,1);
    resizeTable();
}

//! Max. number of columns
int QwtLegend::maxCols() const 
{ 
    return d_maxCols; 
}

/*!
  \brief Specify the maximum number of rows and the layout policy.

  The setMaxRows and setMaxCols members determine in which order 
  the cells are arranged in the table. If a maximum number of
  rows is set, the legend items will be arranged in columns from
  the top to the bottom,
  starting at the first column, and beginning a new column when the
  actual number or rows has reached that maximum. The number of
  columns is thereby unlimited.

  The default setting is a maximum of 5 columns with an unlimited
  number of rows.

  \param n max. number of rows
  \warning setMaxRows and setMaxCols are mutually exclusive. The last one
        wins.
*/
void QwtLegend::setMaxRows(int n)
{
    d_maxCols = 0;
    d_maxRows = qwtMax(n,1);
    resizeTable();
}

//! Max. number of rows
int QwtLegend::maxRows() const 
{ 
    return d_maxRows; 
}

//! the column width
int QwtLegend::colWidth() const 
{ 
    return QTable::columnWidth(0); 
}

//! the row height
int QwtLegend::rowHeight() const 
{ 
    return QTable::rowHeight(0); 
}

//! the number of items
uint QwtLegend::itemCnt() const 
{ 
    return d_item.count(); 
}

/*!
  \brief Update the table dimensions (rows x cols)

  This function is called when an item has been
  appended or deleted or when the max. number of
  rows/columns has been changed.
*/
void QwtLegend::resizeTable()
{
    int rows, cols;
    
    //
    // adjust table dimensions 
    // 
    if (d_maxCols)
    {
        cols = qwtMin(d_maxCols, int(d_item.count()));
        cols = qwtMax(cols, 1);
        rows = qwtMax( (int(d_item.count()) - 1) / cols + 1, 1);
    }
    if (d_maxRows)
    {
        rows = qwtMin(d_maxRows, int(d_item.count()));
        rows = qwtMax(rows, 1);
        cols = qwtMax( (int(d_item.count()) - 1) / rows + 1, 1);
    }

    if (rows != numRows()) 
        setNumRows(rows);

    if (cols != numCols()) 
        setNumCols(cols);
}


/*!
  \brief Resize the cells

  This function is called when an item has been inserted, removed
  or changed. It adjusts the dimensions of the table according to
  the layout policy and sets the cell dimensions such that all
  items fit into the cells.
*/
void QwtLegend::resizeCells()
{
    const int ws = cellSymbolSizeHint().width();
    const int wl = cellLabelSizeHint(fontMetrics()).width();
    const QSize hint = cellSizeHint(fontMetrics());

    d_symRect = QRect(MinHBdrDist, 0, ws, hint.height());
    d_lblRect = QRect(d_symRect.right() + MinHBdrDist, 0, wl, hint.height()); 

    for ( int row = 0; row < numRows(); row++ )
        setRowHeight(row, hint.height());
    for ( int col = 0; col < numCols(); col++ )
        setColumnWidth(col, hint.width());
}

//! a size hint for the symbols in each cell 
QSize QwtLegend::cellSymbolSizeHint() const
{
    int w = MinSymWidth;
    int h = 0;

    QListIterator<QwtLegendItem> it(d_item);
    for (const QwtLegendItem *item = it.toFirst(); item != 0; item = ++it )
    {
        const QSize s = item->symbol().size();
        if ( s.height() > h )
            h = s.height();
    
        if ( s.width() > h )
            w = s.width();
    }

    return QSize(w, h);
}

//! a size hint for the labels in each cell for given font metrics.
QSize QwtLegend::cellLabelSizeHint(const QFontMetrics &fm) const
{
    int w = MinLblWidth;

    QListIterator<QwtLegendItem> it(d_item);
    for (const QwtLegendItem *item = it.toFirst(); item != 0; item = ++it )
    {
        int width = fm.width(item->text());
        if ( width > w )
            w = width;
    }

    return QSize(w, fm.height());
}

//! a size hint for each cell for given font metrics.

QSize QwtLegend::cellSizeHint(const QFontMetrics &fm) const
{
    const QSize symbolHint = cellSymbolSizeHint();
    const QSize labelHint = cellLabelSizeHint(fm);

    const int ws = symbolHint.width();
    const int wl = labelHint.width();
    const int h = qwtMax(symbolHint.height(), labelHint.height());
    const int wd = MinHBdrDist;

    return QSize(wd + ws + wd + wl + wd, h + 2 * MinVBdrDist);
}

//! a size hint
QSize QwtLegend::sizeHint() const
{
    const QSize s(numCols() * colWidth() + 2 * frameWidth(),
            numRows() * rowHeight() + 2 * frameWidth());
    return s;
}


/*!
  \brief Find the index of an item with a specified key
  \param key Key
  \return The index of the first item which has the specified key
          or the total number of items if the key was not found.

*/
uint QwtLegend::findFirstKey(long key)
{
    uint rv = 0;
    QwtLegendItem *curItem;
    QListIterator<QwtLegendItem> iti(d_item);

    iti.toFirst();
    while ((curItem = iti.current()))
    {
        if (curItem->key() == key) 
            break;
        ++rv;
        ++iti;
    }
           
    return rv;
}

/*!
  \todo Documentation
  \sa QWidget::mousePressEvent(QMouseEvent *e)
*/
void QwtLegend::contentsMousePressEvent(QMouseEvent *e)
{
    const int r = rowAt(e->pos().y());
    const int c = columnAt(e->pos().x());

    if ((c >= 0) && (r >= 0))
    {
        const uint index = findIndex(r, c);
        if (index < d_item.count())
        {
            d_sel.active = TRUE;
            d_sel.r = r;
            d_sel.c = c;
            updateCell(r,c);
            emit pressed(index);
        }
    }
}

/*!
  \todo Documentation
  \sa QWidget::mouseReleaseEvent(QMouseEvent *e)
*/
void QwtLegend::contentsMouseReleaseEvent(QMouseEvent *e)
{
    const int r = rowAt(e->pos().y());
    const int c = columnAt(e->pos().x());

    // undo selection
    if (d_sel.active)
    {
        d_sel.active = FALSE;
        updateCell(d_sel.r, d_sel.c);
    }

    // emit clicked() signal
    if ((c >= 0) && (r >= 0))
    {
        const uint index = findIndex(r, c);
        if (index < d_item.count())
            emit clicked(index);
    }
}

/*!
  \brief Empty implementation to disable QTable behaviour
*/
QWidget * QwtLegend::createEditor(int, int, bool) const
{
    return 0; // this table is not editable
}

/*!
  \brief Change the label of an item
  \param index The item's index
  \param s New label
  \return TRUE if the index is valid
*/
bool QwtLegend::setText(uint index, const QString &s)
{
    bool rv = FALSE;
    QwtLegendItem *curItem = d_item.at(index);
    if (curItem)
    {
        curItem->setText(s);
        resizeCells();
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Change the symbol of an item
  \param index   The item's index
  \param s  New symbol
  \return TRUE if the index is valid
*/
bool QwtLegend::setSymbol(uint index, const QwtSymbol &s)
{
    bool rv = FALSE;
    QwtLegendItem *curItem = d_item.at(index);
    if (curItem)
    {
        curItem->setSymbol(s);
        resizeCells();
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Change the line pen of an item
  \param index The item's index
  \param pn New pen
  \return TRUE if the index is valid
*/
bool QwtLegend::setPen(uint index, const QPen &pn)
{
    bool rv = FALSE;
    QwtLegendItem *curItem = d_item.at(index);
    if (curItem)
    {
        curItem->setPen(pn);

        int row, col;
        findRowCol(index, row, col);
        updateCell(row, col);   
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Change the key of an item
  \param index  The item's index
  \param  key New key
*/
bool QwtLegend::setKey(uint index, long key)
{
    bool rv = FALSE;
    QwtLegendItem *curItem = d_item.at(index);
    if (curItem)
    {
        curItem->setKey(key);
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Remove an item
  \param index The item's index
*/
bool QwtLegend::removeItem(uint index)
{
    bool rv = d_item.remove(index);
    if ( rv )
        updateTable();

    return rv;
}

/*!
  \brief Get row and column of an item with a specified index
  \param index The item's index
  \param row The item's row
  \param c The item's column
*/
void QwtLegend::findRowCol(uint index, int &row, int &col)
{
    if (d_maxCols)
    {
        row = int(index) / numCols();
        col = int(index) - row * numCols();
    }
    else
    {
        col = int(index) / numRows();
        row = int(index) - row * numRows();
    }
}

/*!
  \brief Reorganizes columns rows
*/

void QwtLegend::resizeEvent(QResizeEvent *re)
{
    updateTable();
    QTable::resizeEvent(re);
}

/*!
  \brief Notify a font change
 \param oldFont Old font
*/
void QwtLegend::fontChange(const QFont &)
{
    resizeCells();
}

/*!
  \brief Return an item's key
  \param index The item's index
  \return The item's key, or 0 if the specified index is invalid
*/
long QwtLegend::key(uint index) const
{
    const QwtLegendItem *curItem = ((QwtLegend *)this)->d_item.at(index);
    if (curItem)
        return curItem->key();
    else
       return 0;
}

#endif // QWT_NO_LEGEND
