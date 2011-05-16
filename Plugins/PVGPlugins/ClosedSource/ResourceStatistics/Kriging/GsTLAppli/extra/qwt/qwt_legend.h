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

#ifndef QWT_LEGEND_H
#define QWT_LEGEND_H

#include "qwt_global.h"

#ifndef QWT_NO_LEGEND
#include <qptrlist.h>
#include <qtable.h>
#include <qpen.h>
#include "qwt_symbol.h"
#else
#warning QwtLegend disabled
#include <qobject.h>
#endif

class QPainter;

#ifndef QWT_NO_LEGEND
/*!
  \brief A legend item
  \sa QwtLegend
*/
class QWT_EXPORT QwtLegendItem
{
public:
    QwtLegendItem();
    QwtLegendItem(const QString &txt, const QwtSymbol &sym, 
         const QPen &pn, long key);

    virtual ~QwtLegendItem();

    //! the item pen
    const QPen& pen() const { return d_pen; }
    //! the item symbol
    const QwtSymbol& symbol() const { return d_sym; }
    //! the item text
    const QString& text() const { return d_text; }
    //! the item key
    long key() const { return d_key; }

    //! set item symbol
    void setSymbol(const QwtSymbol &s) {d_sym = s;}
    //! set item pen
    void setPen(const QPen &pn) {d_pen = pn;}
    //! set item key
    void setKey(long key) {d_key = key; }
    //! set item text
    void setText(const QString &s) {d_text = s;}
    
    virtual void draw(QPainter *p, const QRect &symRect, 
        const QRect& lblRect, int align);

private:
    QPen d_pen;
    QwtSymbol d_sym;
    QString d_text;
    long d_key;
};

#if defined(QWT_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QWT_EXPORT QPtrList<QwtLegendItem>;
// MOC_SKIP_END
#endif

#endif // !QWT_NO_LEGEND

// some workarounds against moc's painful precompiler ignorance ...
#ifndef QWT_NO_LEGEND
#define QwtTable QTable
#else 
#define QwtTable QObject
#endif

/*!
  \brief The legend widget

  The QwtLegend widget is a tabular arrangement of cells,
  called legend items, which consist of a label, a line
  a symbol, and a key of type long as an 
  identifier. The symbol is drawn on top of
  the line at the left side of the label. An item's
  property can be accessed using the item's index,
  which is independent of the key. The cells can
  be arranged in row or column order with QwtLegend::setMaxRows
  or QwtLegend::setMaxCols. Scrollbars are displayed
  automatically if  the cells don't fit into the widget area.
*/

class QWT_EXPORT QwtLegend : public QwtTable
{
    Q_OBJECT
    friend class QwtPlot;

#ifndef QWT_NO_LEGEND
public:

    QwtLegend(QWidget *w = 0, const char *name = 0);
    ~QwtLegend();
    
    bool insertItem(const QString &txt, const QwtSymbol &s, const QPen &p,
            long key, uint index);
    uint appendItem(const QString &txt, const QwtSymbol &s, 
        const QPen &p, long key);
    
    bool setText(uint index, const QString &s);
    bool setSymbol(uint index,  const QwtSymbol &s);
    bool setPen(uint index, const QPen &pn);
    bool setKey(uint index, long key);

    long key(uint index) const;
    bool removeItem(uint index);

    void setMaxCols(int columns);
    int maxCols() const;

    void setMaxRows(int rows);
    int maxRows() const;

    int colWidth() const;
    int rowHeight() const;
    
    void clear();
    QSize sizeHint() const;
    QSize cellSizeHint(const QFontMetrics &) const;

    uint itemCnt() const;
    uint findFirstKey(long key);
#endif

signals:
    
    /*!
      \brief Emitted when the user presses a mouse button down on an item
      \param index  The item's index
    */
    void pressed(int index);

    /*!
      \brief Emitted when the user clicks on an item.
      \param index The item's index
    */
    void clicked(int index);

#ifndef QWT_NO_LEGEND
protected:

    virtual void paintCell(QPainter *p, int row, int col, 
        const QRect &, bool selected);
    virtual void contentsMousePressEvent(QMouseEvent *e);
    virtual void contentsMouseReleaseEvent(QMouseEvent *e);
    virtual QWidget *createEditor(int row, int col, 
        bool initFromCell) const;

    virtual void resizeEvent(QResizeEvent *);

    void resizeCells();
    void resizeTable();
    void updateTable();

    virtual void fontChange(const QFont &oldFont);

    QPtrList<QwtLegendItem> d_item;

private:
    uint findIndex(int row, int col);
    void findRowCol(uint index, int &row, int &col);

    virtual void paintFocus(QPainter *, const QRect &);

    QSize cellSymbolSizeHint() const;
    QSize cellLabelSizeHint(const QFontMetrics &) const;

    int d_align;
    QRect d_symRect;
    QRect d_lblRect;
    int d_maxCols;
    int d_maxRows;

    struct selection 
    {
        bool active;
        int r;
        int c;
    } d_sel;
#endif // !QWT_NO_LEGEND
};

#endif // QWT_LEGEND_H
