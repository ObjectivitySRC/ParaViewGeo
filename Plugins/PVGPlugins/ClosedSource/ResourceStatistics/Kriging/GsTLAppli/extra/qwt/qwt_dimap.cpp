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

#include "qwt_dimap.h"
#include "qwt_math.h"

QT_STATIC_CONST_IMPL double QwtDiMap::LogMin = 1.0e-150;
QT_STATIC_CONST_IMPL double QwtDiMap::LogMax = 1.0e150;


/*!
  \brief Constructor

  The double and integer intervals are both set to [0,1].
*/
QwtDiMap::QwtDiMap()
{
    d_x1 = 0.0;
    d_x2 = 1.0;
    d_y1 = 0;
    d_y2 = 1;
    d_cnv = 1.0;
}


/*!
  \brief Constructor

  Constructs a QwtDiMap instance with initial integer
  and double intervals

  \param i1 first border of integer interval
  \param i2 second border of integer interval
  \param d1 first border of double interval
  \param d2 second border of double interval
  \bool logarithmic logarithmic mapping, TRUE or FALSE.
*/ 
QwtDiMap::QwtDiMap(int i1, int i2, double d1, double d2, bool logarithmic)
{
    d_log = logarithmic;
    setIntRange(i1,i2);
    setDblRange(d1, d2);
}

/*!
  \brief Destructor
*/
QwtDiMap::~QwtDiMap()
{
}

/*!
  \return TRUE if a value x lies inside or at the border of the
  map's double range.
  \param x value
*/
bool QwtDiMap::contains(double x) const
{
    return ( (x >= qwtMin(d_x1, d_x1)) && (x <= qwtMax(d_x1, d_x2)));
}

/*!
  \eturn TRUE if a value x lies inside or at the border of the
    map's integer range
  \param x value
*/
bool QwtDiMap::contains(int x) const
{
    return ( (x >= qwtMin(d_y1, d_y1)) && (x <= qwtMax(d_y1, d_y2)));
}

/*!
  \brief Specify the borders of the double interval
  \param d1 first border
  \param d2 second border 
  \param lg logarithmic (TRUE) or linear (FALSE) scaling
*/
void QwtDiMap::setDblRange(double d1, double d2, bool lg)
{
    if (lg)
    {
        d_log = TRUE;
        if (d1 < LogMin) 
           d1 = LogMin;
        else if (d1 > LogMax) 
           d1 = LogMax;
        
        if (d2 < LogMin) 
           d2 = LogMin;
        else if (d2 > LogMax) 
           d2 = LogMax;
        
        d_x1 = log(d1);
        d_x2 = log(d2);
    }
    else
    {
        d_log = FALSE;
        d_x1 = d1;
        d_x2 = d2;
    }
    newFactor();
}

/*!
  \brief Specify the borders of the integer interval
  \param i1 first border
  \param i2 second border
*/
void QwtDiMap::setIntRange(int i1, int i2)
{
    d_y1 = i1;
    d_y2 = i2;
    newFactor();
}



/*!
  \brief Transform a point in double interval into an point in the
    integer interval
    
  \param x value
  \return
  <dl>
  <dt>linear mapping:<dd>rint(i1 + (i2 - i1) / (d2 - d1) * (x - d1))
  <dt>logarithmic mapping:<dd>rint(i1 + (i2 - i1) / log(d2 / d1) * log(x / d1))
  </dl>
  \warning The specified point is allowed to lie outside the intervals. If you
  want to limit the returned value, use QwtDiMap::limTransform.
*/
int QwtDiMap::transform(double x) const
{
    if (d_log)
       return (d_y1 + int(floor ( (log(x) - d_x1) * d_cnv + 0.5)));    
    else
       return (d_y1 + int(floor ( (x - d_x1) * d_cnv + 0.5)));
}

/*!
  \brief Transform an integer value into a double value
  \param integer value to be transformed
  \return
  <dl>
  <dt>linear mapping:<dd>d1 + (d2 - d1) / (i2 - i1) * (y - i1)
  <dt>logarithmic mapping:<dd>d1 + (d2 - d1) / log(i2 / i1) * log(y / i1)
  </dl>
*/
double QwtDiMap::invTransform(int y) const 
{
    if (d_cnv == 0.0)
       return 0.0;
    else
    {
        if(d_log) 
           return exp(d_x1 + double(y - d_y1) / d_cnv );
        else
           return ( d_x1 + double(y - d_y1) / d_cnv );  
    }
}


/*! 
  \brief  Transform and limit

  The function is similar to QwtDiMap::transform, but limits the input value
  to the nearest border of the map's double interval if it lies outside
  that interval.

  \param x value to be transformed
  \return transformed value
*/
int QwtDiMap::limTransform(double x) const
{
    if ( x > qwtMax(d_x1, d_x2) )
       x = qwtMax(d_x1, d_x2);
    else if ( x < qwtMin(d_x1, d_x2))
       x = qwtMin(d_x1, d_x2);
    return transform(x);
}

/*!
  \brief Exact transformation
 
  This function is similar to QwtDiMap::transform, but
  makes the integer interval appear to be double. 
  \param x value to be transformed
  \return 
  <dl>
  <dt>linear mapping:<dd>i1 + (i2 - i1) / (d2 - d1) * (x - d1)
  <dt>logarithmic mapping:<dd>i1 + (i2 - i1) / log(d2 / d1) * log(x / d1)
  </dl>
*/
double QwtDiMap::xTransform(double x) const
{
    double rv;
    
    if (d_log)
       rv = double(d_y1) + (log(x) - d_x1) * d_cnv;    
    else
       rv = double(d_y1) + (x - d_x1) * d_cnv;

    return rv;
}


/*!
  \brief Re-calculate the conversion factor.
*/
void QwtDiMap::newFactor()
{
    if (d_x2 != d_x1)
       d_cnv = double(d_y2 - d_y1) / (d_x2 - d_x1); 
    else 
       d_cnv = 0.0;
}
