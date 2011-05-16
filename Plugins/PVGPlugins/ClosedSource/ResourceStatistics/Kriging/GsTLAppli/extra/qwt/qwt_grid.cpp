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
#include "qwt_grid.h"

//! Enables major grid, disables minor grid
QwtGrid::QwtGrid()
{
    d_xMinEnabled = FALSE;
    d_yMinEnabled = FALSE;
    d_xEnabled = TRUE;
    d_yEnabled = TRUE;
}

//! dtor
QwtGrid::~QwtGrid()
{
}

/*!
  \brief Enable or disable vertical gridlines
  \param tf Enable (TRUE) or disable

  \sa Minor gridlines can be enabled or disabled with
      QwtGrid::enableXMin()
*/
void QwtGrid::enableX(bool tf)
{
    if ( d_xEnabled != tf )
    {
        d_xEnabled = tf;
        gridChanged();
    }
}

/*!
  \brief Enable or disable horizontal gridlines
  \param tf Enable (TRUE) or disable
  \sa Minor gridlines can be enabled or disabled with QwtGrid::enableYMin()
*/
void QwtGrid::enableY(bool tf)
{
    if ( d_yEnabled != tf )
    {
        d_yEnabled = tf;  
        gridChanged();
    }
}

/*!
  \brief Enable or disable  minor vertical gridlines.
  \param tf Enable (TRUE) or disable
  \sa QwtGrid::enableX()
*/
void QwtGrid::enableXMin(bool tf)
{
    if ( d_xMinEnabled != tf )
    {
        d_xMinEnabled = tf;
        gridChanged();
    }
}

/*!
  \brief Enable or disable minor horizontal gridlines
  \param tf Enable (TRUE) or disable
  \sa QwtGrid::enableY()
*/
void QwtGrid::enableYMin(bool tf)
{
    if ( d_yMinEnabled != tf )
    {
        d_yMinEnabled = tf;
        gridChanged();
    }
}

/*!
  \brief Assign an x axis scale division
  \param sx Scale division
  \warning QwtGrid uses implicit sharing (see Qt Manual) for
  the scale divisions.
  \sa QwtScaleDiv
*/
void QwtGrid::setXDiv(const QwtScaleDiv &sx)
{
    if ( d_sdx != sx )
    {
        d_sdx = sx;
        gridChanged();
    }
}

/*!
  \brief Assign a y axis division
  \param sy Scale division
  \warning QwtGrid uses implicit sharing (see Qt Manual) for
  the scale divisions.
  \sa QwtScaleDiv
*/
void QwtGrid::setYDiv(const QwtScaleDiv &sy)
{
    if ( d_sdy != sy )
    {
        d_sdy = sy;    
        gridChanged();
    }
}

/*!
  \brief Assign a pen for both major and minor gridlines
  \param p Pen
*/
void QwtGrid::setPen(const QPen &p)
{
    if ( d_majPen != p || d_minPen != p )
    {
        d_majPen = p;
        d_minPen = p;
        gridChanged();
    }
}

/*!
  \brief Assign a pen for the major gridlines
  \param p Pen
*/
void QwtGrid::setMajPen(const QPen &p)
{
    if ( d_majPen != p )
    {
        d_majPen = p;
        gridChanged();
    }
}

/*!
  \brief Assign a pen for the minor gridlines
  \param p Pen
*/
void QwtGrid::setMinPen(const QPen &p)
{
    if ( d_minPen != p )
    {
        d_minPen = p;  
        gridChanged();
    }
}

/*!
  \brief Draw the grid
  
  The grid is drawn into the bounding rectangle such that 
  gridlines begin and end at the rectangle's borders. The X and Y
  maps are used to map the scale divisions into the drawing region
  screen.
  \param p  Painter
  \param r Bounding rectangle
  \param mx X axis map
  \param my Y axis 
*/
void QwtGrid::draw(QPainter *p, const QRect &r, 
    const QwtDiMap &mx, const QwtDiMap &my)
{
    int x1 = r.left();
    int x2 = r.right();
    int y1 = r.top();
    int y2 = r.bottom();

    //
    //  draw minor gridlines
    //
    p->setPen(d_minPen);
    
    if (d_xEnabled && d_xMinEnabled)
    {
        for (int i = 0; i < d_sdx.minCnt(); i++)
        {
            const int x = mx.transform(d_sdx.minMark(i));
            if ((x >= x1) && (x <= x2))
                p->drawLine(x, y1, x, y2);
        }
    }

    if (d_yEnabled && d_yMinEnabled)
    {
        for (int i = 0; i < d_sdy.minCnt(); i++)
        {
            const int y = my.transform(d_sdy.minMark(i));
            if ((y >= y1) && (y <= y2))
                p->drawLine(x1, y, x2, y);
        }
    }

    //
    //  draw major gridlines
    //
    p->setPen(d_majPen);
    
    if (d_xEnabled)
    {
        for (int i = 0; i < d_sdx.majCnt(); i++)
        {
            const int x = mx.transform(d_sdx.majMark(i));
            if ((x >= x1) && (x <= x2))
                p->drawLine(x, y1, x, y2);
        }
    
    }
    
    if (d_yEnabled)
    {
        for (int i = 0; i < d_sdy.majCnt(); i++)
        {
            const int y = my.transform(d_sdy.majMark(i));
            if ((y >= y1) && (y <= y2))
                p->drawLine(x1, y, x2, y);
        }
    }
}

/*!
  \brief Assignment operator 
  \param g Grid to be assigned
*/
const QwtGrid & QwtGrid::operator=(const QwtGrid &g)
{
    d_xEnabled = g.d_xEnabled;
    d_yEnabled = g.d_yEnabled;
    d_xMinEnabled = g.d_xMinEnabled;
    d_yMinEnabled = g.d_yMinEnabled;
    d_sdx = g.d_sdx;
    d_sdy = g.d_sdy;

    d_majPen = g.d_majPen;
    d_minPen = g.d_minPen;
    
    return *this;
}

/*!
  \return the pen for the major gridlines
  \sa QwtGrid::setMinPen, QwtGrid::setPen
*/
const QPen &QwtGrid::majPen() const 
{ 
    return d_majPen; 
}

/*!
  \return the pen for the minor gridlines
  \sa QwtGrid::setMinPen, QwtGrid::setPen
*/
const QPen &QwtGrid::minPen() const 
{ 
    return d_minPen; 
}
  
/*!
  \return TRUE if vertical gridlines are enabled
  \sa QwtGrid::enableX
*/
bool QwtGrid::xEnabled() const
{ 
    return d_xEnabled; 
}

/*!
  \return TRUE if minor vertical gridlines are enabled
  \sa QwtGrid::enableXMin
*/
bool QwtGrid::xMinEnabled() const 
{ 
    return d_xMinEnabled; 
}

/*!
  \return TRUE if horizontal gridlines are enabled
  \sa QwtGrid::enableY
*/
bool QwtGrid::yEnabled() const 
{ 
    return d_yEnabled; 
}

/*!
  \return TRUE if minor horizontal gridlines are enabled
  \sa QwtGrid::enableYMin
*/
bool QwtGrid::yMinEnabled() const 
{
    return d_yMinEnabled; 
}

  
/*! \return the scale division of the x axis */
const QwtScaleDiv &QwtGrid::xScaleDiv() const 
{ 
    return d_sdx; 
}

/*! \return the scale division of the y axis */
const QwtScaleDiv &QwtGrid::yScaleDiv() const 
{ 
    return d_sdy; 
}
 
/*!
  \brief Notify a change of attributes

  This virtual function is called when an attribute of the grid
  has changed. It can be redefined by derived classes. The
  default implementation does nothing.
*/
void QwtGrid::gridChanged() 
{
}

