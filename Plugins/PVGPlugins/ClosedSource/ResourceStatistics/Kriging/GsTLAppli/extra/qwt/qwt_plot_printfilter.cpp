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

#include "qwt_plot_printfilter.h"

/*!
  Sets filter options to QwtPlotPrintFilter::PrintAll
*/  

QwtPlotPrintFilter::QwtPlotPrintFilter():
    d_options(PrintAll)
{
}

//! Destructor
QwtPlotPrintFilter::~QwtPlotPrintFilter()
{
}

/*!
  \brief Modifies a color for printing
  \param c Color to be modified
  \param item Type of item where the color belongs
  \param id Optional id of the item (curveId/markerId)
  \return Modified color.

  In case of !(QwtPlotPrintFilter::options() & PrintBackground) 
  MajorGrid is modified to Qt::darkGray, MinorGrid to Qt::gray. 
  All other colors are returned unmodified.
*/

QColor QwtPlotPrintFilter::color(const QColor &c, Item item, int) const
{
    if ( !(options() & PrintBackground))
    {
        switch(item)
        {
            case MajorGrid:
                return Qt::darkGray;
            case MinorGrid:
                return Qt::gray;
            default:;
        }
    }
    return c;
}

/*!
  \brief Modifies a font for printing
  \param f Font to be modified
  \param item Type of item where the font belongs
  \param id Optional id of the item (curveId/markerId)

  All fonts are returned unmodified
*/

QFont QwtPlotPrintFilter::font(const QFont &f, Item, int) const
{
    return f;
}
