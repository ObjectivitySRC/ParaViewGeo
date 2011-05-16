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

#ifndef QWT_PLOT_ITEM
#define QWT_PLOT_ITEM

#include "qwt_global.h"

class QwtPlot;

/*!
  \brief Base class for items on the plot canvas
*/

class QWT_EXPORT QwtPlotItem
{
public:
    QwtPlotItem(QwtPlot *parent, bool nbl = TRUE);

    void reparent(QwtPlot *plot);

    //! \return parent plot
    QwtPlot *parentPlot() { return d_parent; }

    //! \return parent plot
    const QwtPlot *parentPlot() const { return d_parent; }
    
    virtual void setEnabled(bool);
    bool enabled() const;

    virtual void itemChanged();

private:
    bool d_enabled;
    QwtPlot *d_parent;
};
            

/*!
  \brief Base class for items on the plot canvas,
  that are attached to x and y axes.
*/
class QWT_EXPORT QwtPlotMappedItem : public QwtPlotItem 
{
public:
    QwtPlotMappedItem(QwtPlot *parent, bool nbl = TRUE);
    
    void setAxis(int xAxis, int yAxis);

    void setXAxis(int axis);
    int xAxis() const;

    void setYAxis(int axis);
    int yAxis() const;

private:
    int d_xAxis;
    int d_yAxis;
};

#endif
