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

#include <qpainter.h>
#include "qwt_marker.h"
#include "qwt_math.h"

static const int LabelDist = 2;

//! Sets alignment to Qt::AlignCenter, and style to NoLine
QwtMarker::QwtMarker()
{
    d_align = Qt::AlignCenter;
    d_style = NoLine;
}

//! Destructor
QwtMarker::~QwtMarker()
{
}

/*!
  \brief Assignment operator
  \param m Marker
*/
const QwtMarker& QwtMarker::operator=(const QwtMarker &m)
{
    d_label = m.d_label;
    d_pen = m.d_pen;
    d_tPen = m.d_tPen;
    d_font = m.d_font;
    d_sym = m.d_sym;
    d_align = m.d_align;
    d_style = m.d_style;

    return *this;
}


/*!
  \brief Draw the marker
  \param p Painter
  \param x X position
  \param y Y position
  \param r Bounding rectangle
*/
void QwtMarker::draw(QPainter *p, int x, int y, const QRect &r)
{
    // draw lines
    if (d_style != NoLine)
    {
        p->setPen(d_pen);
        if ((d_style == HLine) || (d_style == Cross))
            p->drawLine(r.left(), y, r.right(), y);
        if ((d_style == VLine)||(d_style == Cross))
            p->drawLine(x, r.top(), x, r.bottom());
    }

    // draw symbol
    QSize sSym;
    if (d_sym.style() != QwtSymbol::None)
    {
        sSym = d_sym.size();
        d_sym.draw(p, x - (sSym.width() - 1) / 2 ,y - (sSym.width() - 1) / 2);
    }
    else
    {
        sSym.setWidth(0);
        sSym.setHeight(0);
    }
    

    // draw label
    if (!d_label.isEmpty())
    {
        p->setPen(d_tPen);
        p->setFont(d_font);

        const int th = p->fontMetrics().height();
        const int tw = p->fontMetrics().width(d_label);
        int lw = qwtMax(int(d_pen.width()), 1);
        int lw1;

        if ((d_style == VLine) || (d_style == HLine))
        {
            lw1 = (lw + 1) / 2 + LabelDist;
            lw = lw / 2 + LabelDist;
        }
        else 
        {
            lw1 = qwtMax((lw + 1) / 2, (sSym.width() + 1) / 2) + LabelDist;
            lw = qwtMax(lw / 2, (sSym.width() + 1) / 2) + LabelDist;
        }

        
        QRect tr;
        if (d_style == VLine)
        {
            if (d_align & (int) Qt::AlignTop)
               tr.setY(r.top() + LabelDist);
            else if (d_align & (int) Qt::AlignBottom)
               tr.setY(r.bottom() - LabelDist - th);
            else
               tr.setY(r.top() + (r.bottom() - r.top()) / 2);
        }
        else 
        {
            if (d_align & (int) Qt::AlignTop)
               tr.setY(y - lw - th);
            else if (d_align & (int) Qt::AlignBottom)
               tr.setY(y + lw1);
            else
               tr.setY(y - th/2);
        }


        if (d_style == HLine)
        {
            if (d_align & (int) Qt::AlignLeft)
               tr.setX(r.left() + LabelDist);
            else if (d_align & (int) Qt::AlignRight)
               tr.setX(r.right() - tw - LabelDist);
            else
               tr.setX(r.left() + (r.right() - r.left()) / 2);
        }
        else
        {
            if (d_align & (int) Qt::AlignLeft)
               tr.setX(x - tw - lw);
            else if (d_align & (int) Qt::AlignRight)
               tr.setX(x + lw1);
            else
               tr.setX(x - tw/ 2);
        }

        tr.setHeight(th);
        tr.setWidth(tw);    

        p->drawText(tr, Qt::AlignTop|Qt::AlignHCenter, d_label); 
    }
}

/*!
  \brief Specify the font for the label
  \param f New font
  \sa QwtMarker::font()
*/
void QwtMarker::setFont(const QFont &f)
{
    if ( f != d_font )
    {
        d_font = f;
        markerChanged();
    }
}

/*!
  \return the font used for the label
  \sa QwtMarker::setFont()
*/
const QFont &QwtMarker::font() const 
{ 
    return d_font; 
}


/*!
  \brief Set the line style
  \param st Line style. Can be one of QwtMarker::NoLine,
    QwtMarker::HLine, QwtMarker::VLine or QwtMarker::Cross
  \sa QwtMarker::lineStyle()
*/
void QwtMarker::setLineStyle(QwtMarker::LineStyle st)
{
    if ( st != d_style )
    {
        d_style = st;
        markerChanged();
    }
}

/*!
  \return the line style
  \sa For a description of line styles, see QwtMarker::setLineStyle()
*/
QwtMarker::LineStyle QwtMarker::lineStyle() const 
{ 
    return d_style; 
}

/*!
  \brief Assign a symbol
  \param s New symbol 
  \sa QwtSymbol, QwtMarker::symbol()
*/
void QwtMarker::setSymbol(const QwtSymbol &s)
{
    d_sym = s;
    markerChanged();
}

/*!
  \return the symbol
  \sa QwtMarker::setSymbol(), QwtSymbol
*/
const QwtSymbol &QwtMarker::symbol() const 
{ 
    return d_sym; 
}

/*!
  \brief Set the label
  \param txt label text 
  \sa QwtMarker::label()
*/
void QwtMarker::setLabel(const QString &txt)
{
    if ( txt != d_label )
    {
        d_label = txt;
        markerChanged();
    }
}

/*!
  \return the label
  \sa QwtMarker::setLabel()
*/
const QString& QwtMarker::label() const 
{ 
    return d_label; 
}

/*!
  \brief Set the alignment of the label

  The alignment determines where the label is drawn relative to
  the marker's position.

  \param align Alignment. A combination of AlignTop, AlignBottom,
    AlignLeft, AlignRight, AlignCenter, AlgnHCenter,
    AlignVCenter.  
  \sa QwtMarker::labelAlignment()
*/
void QwtMarker::setLabelAlignment(int align)
{
    if ( align != d_align )
    {
        d_align = align;
        markerChanged();
    }
}

/*!
  \return the label alignment
  \sa QwtMarker::setLabelAlignment()
*/
int QwtMarker::labelAlignment() const 
{ 
    return d_align; 
}

/*!
  \brief Specify a pen for the line.
  \param p New pen
  \sa QwtMarker::linePen()
*/
void QwtMarker::setLinePen(const QPen &p)
{
    if ( p != d_pen )
    {
        d_pen = p;
        markerChanged();
    }
}

/*!
  \return the line pen
  \sa QwtMarker::setLinePen()
*/
const QPen &QwtMarker::linePen() const 
{ 
    return d_pen; 
}

/*!
  \brief Specify a pen for the label
  \param p New pen
  \sa QwtMarker::labelPen()
*/
void QwtMarker::setLabelPen(const QPen &p)
{
    if ( p != d_tPen )
    {
        d_tPen = p;
        markerChanged();
    }
}

/*!
  \return the label's pen
  \sa QwtMarker::setLabelPen()
*/
const QPen &QwtMarker::labelPen() const 
{ 
    return d_tPen; 
}

/*!
  \brief Notify a change of parameters

  This virtual function is called when a marker property
  has been changed and redrawing is advisable. It can be
  reimplemented by derived classes. The default implementation
  does nothing.
*/
void QwtMarker::markerChanged() 
{
}

