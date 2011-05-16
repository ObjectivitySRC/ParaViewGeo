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

#ifndef QWT_SCLDRAW_H
#define QWT_SCLDRAW_H

#include <qstring.h>
#include <qfont.h>
#include <qrect.h>
#include <qcolor.h>
#include "qwt_global.h"
#include "qwt_dimap.h"
#include "qwt_scldiv.h"

class QPainter;

/*!
  \brief A class for drawing scales

  QwtScaleDraw can be used to draw linear or logarithmic scales.
  A scale has an origin,
  an orientation and a length, which all can be specified with
  QwtScaleDraw::setGeometry().

  After a scale division has been specified as a QwtScaleDiv object
  using QwtScaleDraw::setScale(const QwtScaleDiv &s),
  or determined internally using QwtScaleDraw::setScale(double vmin, double vmax, int maxMajIntv, int maxMinIntv, double step = 0.0, int logarithmic = 0),
  the scale can be drawn with the QwtScaleDraw::draw() member.
*/
class QWT_EXPORT QwtScaleDraw: public QwtDiMap
{
public:
    /*! 
        Orientation/Position of the scale draw
        \sa QwtScaleDraw::setGeometry
     */
    enum Orientation { Bottom, Top, Left, Right, Round };
 
    QwtScaleDraw();
    virtual ~QwtScaleDraw();
    
    void setScale(const QwtScaleDiv &s);
    void setScale(double vmin, double vmax, int maxMajIntv, int maxMinIntv,
                  double step = 0.0, int logarithmic = 0);
    void setGeometry(int xorigin, int yorigin, int length, Orientation o);
    int x() const;
    int y() const;
    int length() const;
    Orientation orientation() const;

    void setAngleRange(double angle1, double angle2);
    void setLabelFormat(char f, int prec, int fieldwidth = 0);

    /*! \return scale division */
    const QwtScaleDiv& scaleDiv() const { return d_scldiv; }
    QRect maxBoundingRect(const QFontMetrics &fm) const;
    int maxWidth(const QPen &pen, const QFontMetrics &fm) const;
    int maxHeight(const QPen &pen, const QFontMetrics &fm) const;

    int minBorderDist(const QFontMetrics &fm) const;
    int minHeight(const QPen &pen, const QFontMetrics &fm) const; 
    int minWidth(const QPen &pen, const QFontMetrics &fm) const; 
    void labelFormat(char &f, int &prec, int &fieldwidth) const;
        
    virtual void draw(QPainter *p) const;
    virtual QString label(double) const;

protected:
    virtual int maxLabelWidth(const QFontMetrics &) const;

private:
    void drawTick(QPainter *p, double val, int len) const;
    void drawBackbone(QPainter *p) const;
    void drawLabel(QPainter *p, double val) const;
        
    static const int minLen;
        
    QwtScaleDiv d_scldiv;
    Orientation d_orient;
        
    int d_xorg;
    int d_yorg;
    int d_len;
        
    int d_hpad;
    int d_vpad;
        
    int d_medLen;
    int d_majLen;
    int d_minLen;

    int d_minAngle;
    int d_maxAngle;

    double d_xCenter;
    double d_yCenter;
    double d_radius;

    char d_fmt;
    int d_prec;
    int d_fieldwidth;
};

#endif
