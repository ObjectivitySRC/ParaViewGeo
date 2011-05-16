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

#ifndef QWT_PLOT_CLASSES_H
#define QWT_PLOT_CLASSES_H

#include "qwt_global.h"
#include "qwt_plot_item.h"

/*!
  \brief Curve class for QwtPlot
*/

class QWT_EXPORT QwtPlotCurve: public QwtCurve, public QwtPlotMappedItem
{
public:
    //! Constructor
    QwtPlotCurve(QwtPlot* parent, const char *name = 0): 
        QwtCurve(name), QwtPlotMappedItem(parent) {}

protected:
    //! Calls itemChanged()
    virtual void curveChanged() { itemChanged(); }
};

/*!
  \brief Grid class for QwtPlot
*/
class QWT_EXPORT QwtPlotGrid: public QwtGrid, public QwtPlotMappedItem
{
public:
    //! Constructor
    QwtPlotGrid(QwtPlot *parent): 
        QwtPlotMappedItem(parent) {}

protected:
    //! Calls itemChanged()
    virtual void gridChanged() { itemChanged(); }
};


/*!
  \brief Marker class for QwtPlot
*/
class QWT_EXPORT QwtPlotMarker: public QwtMarker, public QwtPlotMappedItem 
{
public:
    //! Constructor
    QwtPlotMarker(QwtPlot *parent): 
        QwtPlotMappedItem(parent),
        d_xValue(0.0),
        d_yValue(0.0)
    {}

    //! \return X Value
    double xValue() const { return d_xValue; }

    //! \return Y Value
    double yValue() const { return d_yValue; }

    //! Set X Value
    void setXValue(double val) { d_xValue = val; markerChanged(); }

    //! Set Y Value
    void setYValue(double val) { d_yValue = val; markerChanged(); }

protected:
    //! Calls itemChanged()
    virtual void markerChanged() { itemChanged(); }

private:
    double d_xValue;
    double d_yValue;
};

#endif
