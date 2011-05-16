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
#include "qwt_symbol.h"

QwtSymbol NoSymbol();

/*!
  \brief Default Constructor

  The symbol is constructed with gray interior,
  black outline with zero width, no size and style 'None'.
*/
QwtSymbol::QwtSymbol(): 
    d_brush(Qt::gray), 
    d_pen(Qt::black), 
    d_size(0,0)
{
    d_style = QwtSymbol::None;
}

/*!
  \brief Constructor
  \param st Symbol Style
  \param bd brush to fill the interior
  \param pn outline pen 
  \param size size
*/
QwtSymbol::QwtSymbol(QwtSymbol::Style st, const QBrush &br, 
        const QPen &pn, const QSize &s): 
    d_brush(br), 
    d_pen(pn), 
    d_size(s)
{
    d_style = st;
}

//! Destructor
QwtSymbol::~QwtSymbol()
{
}


/*!
  \brief Specify the symbol's size

  If the 'h' parameter is left out or less than 0,
  and the 'w' parameter is greater than or equal to 0,
  the symbol size will be set to (w,w).
  \param w width
  \param h height (defaults to -1)
*/
void QwtSymbol::setSize(int w, int h)
{
    if ((w >= 0) && (h < 0)) 
        h = w;
    d_size = QSize(w,h);
}

//! Set the symbol's size
void QwtSymbol::setSize(const QSize &s)
{
    if (s.isValid()) 
        d_size = s;
}

/*!
  \brief Assign a brush

  The brush is used to draw the interior of the symbol.
  \param br brush
*/
void QwtSymbol::setBrush(const QBrush &br)
{
    d_brush = br;
}

/*!
  \brief Assign a pen

  The pen is used to draw the symbol's outline.

  \param pn pen
*/
void QwtSymbol::setPen(const QPen &pn)
{
    d_pen = pn;
}

/*!
  \brief Draw the symbol at a point (x,y).

  The point (x,y) is the upper left corner of a rectangle with
  the symbol's size.
*/
void QwtSymbol::draw(QPainter *p, int x, int y)
{
    QRect r (x,y,d_size.width(), d_size.height());
    draw(p, r);
}


/*!
  \brief Draw the symbol into a bounding rectangle.

  This function overrides the symbol's size settings,
  but it doesn't modify them.
  \param p Painter
  \param r Bounding rectangle
*/
void QwtSymbol::draw(QPainter *p, const QRect& r)
{
    const int w2 = r.width() / 2;
    const int h2 = r.height() / 2;

    p->setBrush(d_brush);
    p->setPen(d_pen);
    
    switch(d_style)
    {
        case QwtSymbol::Ellipse:
            p->drawEllipse(r);
            break;
        case QwtSymbol::Rect:
            p->drawRect(r);
            break;
        case QwtSymbol::Diamond:
            d_pa.resize(4);
            d_pa.setPoint(0, r.x() + w2, r.y());
            d_pa.setPoint(1, r.right(), r.y() + h2);
            d_pa.setPoint(2, r.x() + w2, r.bottom());
            d_pa.setPoint(3, r.x(), r.y() + h2);
            p->drawPolygon(d_pa);
            break;
        case QwtSymbol::Cross:
            p->drawLine(r.x() + w2, r.y(), r.x() + w2, r.bottom());
            p->drawLine(r.x(), r.y() + h2, r.right(), r.y() + h2);
            break;
        case QwtSymbol::XCross:
            p->drawLine(r.x(), r.y(), r.right(), r.bottom());
            p->drawLine(r.x(), r.bottom(), r.right(), r.top());
            break;
        case QwtSymbol::Triangle:
        case QwtSymbol::UTriangle:
            d_pa.resize(3);
            d_pa.setPoint(0, r.x() + w2, r.y());
            d_pa.setPoint(1, r.right(), r.bottom());
            d_pa.setPoint(2, r.x(), r.bottom());
            p->drawPolygon(d_pa);
            break;
        case QwtSymbol::DTriangle:
            d_pa.resize(3);
            d_pa.setPoint(0, r.x(), r.y());
            d_pa.setPoint(1, r.right(), r.y());
            d_pa.setPoint(2, r.x() +  w2, r.bottom());
            p->drawPolygon(d_pa);
            break;
        case QwtSymbol::LTriangle:
            d_pa.resize(3);
            d_pa.setPoint(0, r.x(), r.y());
            d_pa.setPoint(1, r.right(), r.y() + h2);
            d_pa.setPoint(2, r.x(), r.bottom());
            p->drawPolygon(d_pa);
            break;
        case QwtSymbol::RTriangle:
            d_pa.resize(3);
            d_pa.setPoint(0, r.right(), r.y());
            d_pa.setPoint(1, r.x(), r.y() + h2);
            d_pa.setPoint(2, r.right(), r.bottom());
            p->drawPolygon(d_pa);
            break;
        default:;
    }
}

/*!
  \brief Draw the symbol at a specified point

  The point specifies the upper left corner of a
  rectangle with the symbol's size.
  \param p painter
  \param pt point
*/
void QwtSymbol::draw(QPainter *p, const QPoint &pt)
{
    draw(p, pt.x(), pt.y());
}

/*!
  \brief Specify the symbol style

  The following styles are defined:<dl>
  <dt>QwtSymbol::None<dd>No Style. The symbol cannot be drawn.
  <dt>QwtSymbol::Ellipse<dd>Ellipse or circle
  <dt>QwtSymbol::Rect<dd>Rectangle
  <dt>QwtSymbol::Diamond<dd>Diamond
  <dt>QwtSymbol::Triangle<dd>Triangle pointing upwards
  <dt>QwtSymbol::DTriangle<dd>Triangle pointing downwards
  <dt>QwtSymbol::UTriangle<dd>Triangle pointing upwards
  <dt>QwtSymbol::LTriangle<dd>Triangle pointing left
  <dt>QwtSymbol::RTriangle<dd>Triangle pointing right
  <dt>QwtSymbol::Cross<dd>Cross
  <dt>QwtSymbol::XCross<dd>Diagonal cross</dl>
  \param s style
*/
void QwtSymbol::setStyle(QwtSymbol::Style s)
{
    d_style = s;
}
