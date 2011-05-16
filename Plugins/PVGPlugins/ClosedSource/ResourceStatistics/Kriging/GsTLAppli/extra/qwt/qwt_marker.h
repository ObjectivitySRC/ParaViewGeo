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

#ifndef QWT_MARKER_H
#define QWT_MARKER_H

#include <qpen.h>
#include <qfont.h>
#include <qstring.h>
#include "qwt_global.h"
#include "qwt_symbol.h"

class QRect;

/*!
  \brief A class for drawing markers

  A marker can be a horizontal line, a vertical line,
  a symbol, a label or any combination of them, which can
  be drawn around a center point inside a bounding rectangle.

  The QwtMarker::setSymbol() member assigns a symbol to the marker.
  The symbol is drawn at the specified point.

  With QwtMarker::setLabel(), a label can be assigned to the marker.
  The QwtMarker::setLabelAlignment() member specifies where the label is
  drawn. All the Align*-constants in qwindefs.h (see Qt documentation)
  are valid. The interpretation of the alignment depends on the marker's
  line style. The alignment refers to the center point of
  the marker, which means, for example, that the label would be printed
  left above the center point if the alignment was set to AlignLeft|AlignTop.
*/

class QWT_EXPORT QwtMarker
{
public:

    /*!
        Line styles.
        \sa QwtMarker::setLineStyle, QwtMarker::lineStyle
    */
    enum LineStyle {NoLine, HLine, VLine, Cross};
   
    QwtMarker();
    virtual ~QwtMarker();

    const QwtMarker & operator=(const QwtMarker&);
    
    void setLineStyle(LineStyle st);
    LineStyle lineStyle() const;

    void setSymbol(const QwtSymbol &s);
    const QwtSymbol &symbol() const;

    void setLabel(const QString &txt);
    const QString& label() const;

    void setLabelAlignment(int align);
    int labelAlignment() const;

    void setLabelPen(const QPen &p);
    const QPen &labelPen() const;
    void setLinePen(const QPen &p);
    const QPen &linePen() const;

    void setFont(const QFont &f);
    const QFont &font() const;
    
    virtual void draw(QPainter *p, int x, int y, const QRect &r);
    
protected:
    virtual void markerChanged();
 
private:
    QString d_label;
    QPen d_pen;
    QPen d_tPen;
    QFont d_font;
    QwtSymbol d_sym;
    int d_align;
    LineStyle d_style;
};

#endif
