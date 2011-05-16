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

#ifndef QWT_PLOT_PRINTFILTER_H
#define QWT_PLOT_PRINTFILTER_H

#include <qcolor.h>
#include <qfont.h>
#include "qwt_global.h"


/*!
  \brief A base class for plot print filters

  QwtPlotPrintFilter can be used to customize QwtPlot::print.
*/

class QWT_EXPORT QwtPlotPrintFilter
{
public:
    //! Print options
    enum Options 
    {
        PrintMargin = 1,
        PrintTitle = 2,
        PrintLegend = 4,
        PrintGrid = 8,
        PrintBackground = 16,

        PrintAll = ~0
    }; 

    //! Print items
    enum Item
    {
        Title,
        Legend,
        Curve,
        CurveSymbol,
        Marker,
        MarkerSymbol,
        MajorGrid,
        MinorGrid,
        Background,
        AxisScale,
        AxisTitle
    };

    QwtPlotPrintFilter();
    virtual ~QwtPlotPrintFilter(); 

    virtual QColor color(const QColor &, Item item, int id = -1) const;
    virtual QFont font(const QFont &, Item item, int id = -1) const;

/*!
    \brief Set plot print options
    \param options Or'd QwtPlotPrintFilter::Options values
    
    \sa QwtPlotPrintFilter::options()
*/

    void setOptions(int options) { d_options = options; }

/*!
    \brief Get plot print options
    \sa QwtPlotPrintFilter::setOptions()
*/

    int options() const { return d_options; }

private:
    int d_options;
};

#endif
