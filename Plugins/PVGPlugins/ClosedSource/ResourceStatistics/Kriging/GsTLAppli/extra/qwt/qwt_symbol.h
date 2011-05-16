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

#ifndef QWT_SYMBOL_H
#define QWT_SYMBOL_H

#include <qbrush.h>
#include <qsize.h>
#include <qpointarray.h>
#include "qwt_global.h"
#include "qwt.h"

class QPainter;

//! A class for drawing symbols
class QWT_EXPORT QwtSymbol
{
public:
    /*!
        Style
        \sa QwtSymbol::setStyle, QwtSymbol::style
     */
    enum Style { None, Ellipse, Rect, Diamond, Triangle, DTriangle,
        UTriangle, LTriangle, RTriangle, Cross, XCross, StyleCnt }; 
   
public:
    QwtSymbol();
    QwtSymbol(Style st, const QBrush &bd, const QPen &pn, const QSize &s);
    virtual ~QwtSymbol();
    
    void setSize(const QSize &s);
    void setSize(int a, int b = -1);
    void setBrush(const QBrush& b);
    void setPen(const QPen &p);
    void setStyle (Style s);

    //! \return Brush
    const QBrush& brush() const { return d_brush; }
    //! \return Pen
    const QPen& pen() const { return d_pen; }
    //! \return Size
    const QSize& size() const { return d_size; }
    //! \return Style
    Style style() const { return d_style; } 
    
    void draw(QPainter *p, const QPoint &pt); 
    void draw(QPainter *p, int x, int y);
    virtual void draw(QPainter *p, const QRect &r);

protected:
    QBrush d_brush;
    QPen d_pen;
    QSize d_size;
    QPointArray d_pa;
    Style d_style;
};

#endif
