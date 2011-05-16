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

#ifndef QWT_GRID_H
#define QWT_GRID_H

#include <qpen.h>
#include "qwt_global.h"
#include "qwt_dimap.h"
#include "qwt_scldiv.h"

class QPainter;

/*!
  \brief A class which draws a coordinate grid

  The QwtGrid class can be used to draw a coordinate grid.
  A coordinate grid consists of major and minor vertical
  and horizontal gridlines. The locations of the gridlines
  are determined by the X and Y scale divisions which can
  be assigned with QwtGrid::setXDiv and QwtGrid::setYDiv()
  The draw() member draws the grid within a bounding
  rectangle.
*/

class QWT_EXPORT QwtGrid
{
public:

    QwtGrid();
    virtual ~QwtGrid();

    virtual const QwtGrid &operator=(const QwtGrid &g);

    virtual void draw(QPainter *p, const QRect &r, 
    const QwtDiMap &mx, const QwtDiMap &my);

    void enableX(bool tf);
    bool xEnabled() const;

    void enableY(bool tf);
    bool yEnabled() const;

    void enableXMin(bool tf);
    bool xMinEnabled() const;

    void enableYMin(bool tf);
    bool yMinEnabled() const;

    void setXDiv(const QwtScaleDiv &sx);
    const QwtScaleDiv &xScaleDiv() const;

    void setYDiv(const QwtScaleDiv &sy);
    const QwtScaleDiv &yScaleDiv() const;

    void setPen(const QPen &p);

    void setMajPen(const QPen &p);
    const QPen& majPen() const;

    void setMinPen(const QPen &p);
    const QPen& minPen() const;


protected:
    virtual void gridChanged();

private:

    bool d_xEnabled;
    bool d_yEnabled;
    bool d_xMinEnabled;
    bool d_yMinEnabled;

    QwtScaleDiv d_sdx;
    QwtScaleDiv d_sdy;

    QPen d_majPen;
    QPen d_minPen;
};

#endif
