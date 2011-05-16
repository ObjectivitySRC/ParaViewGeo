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

#include "qwt_plot.h"
#include "qwt_math.h"

//------------------------------------------------------------
//.C      Accessing and Manipulating the Coordinate Grid
//------------------------------------------------------------

/*!
  \brief Enable or disable vertical gridlines.
  \param tf Enable (TRUE) or disable (FALSE).
            Defaults to TRUE. 
*/
void QwtPlot::enableGridX(bool tf)
{
    d_grid.enableX(tf);
}

/*!
  \brief Enable or disable horizontal gridlines
  \param tf Enable (TRUE) or disable (FALSE).
            Defaults to TRUE.
*/
void QwtPlot::enableGridY(bool tf)
{
    d_grid.enableY(tf);
}

/*!
  \brief Enable or disable vertical gridlines for the minor scale marks
  \param tf enable (TRUE) or disable (FALSE). Defaults to TRUE.
*/
void QwtPlot::enableGridXMin(bool tf)
{
    d_grid.enableXMin(tf);
}

/*!
  \brief Enable or disable horizontal gridlines for the minor scale marks.
  \param tf enable (TRUE) or disable (FALSE). Defaults to TRUE.
*/
void QwtPlot::enableGridYMin(bool tf)
{
    d_grid.enableYMin(tf);
}


/*!
  \brief Attach the grid to an x axis
  \param axis x axis to be attached
*/
void QwtPlot::setGridXAxis(int axis)
{
    if ((axis==xBottom)||(axis==xTop))
    {
    d_grid.setXAxis(axis);
    d_grid.setXDiv(d_sdiv[axis]);
    }
}

/*!
    \return x-axis where the grid is attached to.
    \sa QwtPlot::setGridXAxis
*/
int QwtPlot::gridXAxis() const
{
    return d_grid.xAxis();
}

/*!
  \brief Attach the grid to an y axis
  \param axis y axis to be attached
*/
void QwtPlot::setGridYAxis(int axis)
{
    if ((axis==yLeft) || (axis == yRight))
    {
    d_grid.setYAxis(axis);
    d_grid.setYDiv(d_sdiv[axis]);
    }
}

/*!
    \return y-axis where the grid is attached to.
    \sa QwtPlot::setGridYAxis
*/
int QwtPlot::gridYAxis() const
{
    return d_grid.yAxis();
}

/*!
  \brief Change the grid's pens for major and minor gridlines
  \param p new pen
*/
void QwtPlot::setGridPen(const QPen &p)
{
    d_grid.setPen(p);
}

/*!
  \brief Change the pen for the minor gridlines
  \param p new pen
*/
void QwtPlot::setGridMinPen(const QPen &p)
{
    d_grid.setMinPen(p);
}

/*!
    \return pen of the minor grid
    \sa QwtPlot::setGridMinPen, QwtPlot::gridMajPen
*/
const QPen& QwtPlot::gridMinPen() const
{
    return d_grid.minPen();
}

/*!
  \brief Change the pen for the major gridlines
  \param p new pen
*/
void QwtPlot::setGridMajPen(const QPen &p)
{
    d_grid.setMajPen(p);
}

/*!
    \return pen of the major grid
    \sa QwtPlot::setGridMajPen, QwtPlot::gridMinPen
*/
const QPen& QwtPlot::gridMajPen() const
{
    return d_grid.majPen();
}
