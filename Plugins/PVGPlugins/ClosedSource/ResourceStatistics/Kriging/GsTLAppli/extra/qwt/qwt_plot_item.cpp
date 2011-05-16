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
#include "qwt_plot_item.h"

//! Constructor

QwtPlotItem::QwtPlotItem(QwtPlot *parent, bool nbl)
{
    d_parent = parent;
    d_enabled = nbl;
}

//! reparent the item
void QwtPlotItem::reparent(QwtPlot *parent)
{
    d_parent = parent;
}

/*! 
    enables the item
    \sa QwtPlotItem::setEnabled
*/
void QwtPlotItem::setEnabled(bool tf) 
{ 
    if ( tf != d_enabled )
    {
        d_enabled = tf; 
        itemChanged(); 
    }
}

/*! 
    \return TRUE if enabled
    \sa QwtPlotItem::setEnabled
*/
bool QwtPlotItem::enabled()  const
{ 
    return d_enabled; 
}

//! Call QwtPlot::autoRefresh for the parent plot
void QwtPlotItem::itemChanged()
{
    if ( d_parent )
        d_parent->autoRefresh();
}

//! Constructor

QwtPlotMappedItem::QwtPlotMappedItem(QwtPlot *parent, bool nbl): 
    QwtPlotItem(parent, nbl)
{
    d_xAxis = QwtPlot::xBottom;
    d_yAxis = QwtPlot::yLeft;
}

//!  Set x and y axis
void QwtPlotMappedItem::setAxis(int xAxis, int yAxis)
{
    if (xAxis == QwtPlot::xBottom || xAxis == QwtPlot::xTop )
       d_xAxis = xAxis;

    if (yAxis == QwtPlot::yLeft || yAxis == QwtPlot::yRight )
       d_yAxis = yAxis;

    itemChanged();    
}

//!  Set y axis
void QwtPlotMappedItem::setXAxis(int axis)
{
    if (axis == QwtPlot::xBottom || axis == QwtPlot::xTop )
    {
       d_xAxis = axis;
       itemChanged();    
    }
}

//!  Set y axis
void QwtPlotMappedItem::setYAxis(int axis)
{
    if (axis == QwtPlot::yLeft || axis == QwtPlot::yRight )
    {
       d_yAxis = axis;
       itemChanged();   
    }
}

//!  \return xAxis
int QwtPlotMappedItem::xAxis() const 
{ 
    return d_xAxis; 
}

//!  \return xAxis
int QwtPlotMappedItem::yAxis() const 
{ 
    return d_yAxis; 
}
