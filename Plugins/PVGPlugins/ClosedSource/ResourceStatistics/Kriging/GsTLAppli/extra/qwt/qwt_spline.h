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

#ifndef QWT_SPLINE_H
#define QWT_SPLINE_H

#include "qwt_global.h"

/*!
  \brief A class for spline interpolation

  The QwtSpline class is used for cubical spline interpolation.
  Two types of splines, natural and periodic, are supported.
  
  \par Usage:
  <ol>
  <li>First call QwtSpline::recalc() to determine the spline coefficients 
      for a tabulated function y(x).
  <li>After the coefficients have been set up, the interpolated
      function value for an argument x can be determined by calling 
      QwtSpline::value().
  </ol>
  In order to save storage space, QwtSpline can be advised
  not to buffer the contents of x and y.
  This means that the arrays have to remain valid and unchanged
  for the interpolation to work properly. This can be achieved
  by calling QwtSpline::copyValues().

  \par Example:
  \code
#include<qwt_spline.h>
#include<iostream.h>

QwtSpline s;
double x[30], y[30], xInter[300], yInter[300];
int i;

for(i=0;i<30;i++)               // fill up x[] and y[]
cin >> x[i] >> y[i];

if (s.recalc(x,y,30,0) == 0)    // build natural spline
{
   for(i=0;i<300;i++)          // interpolate
   {
     xInter[i] = x[0] + double(i) * (x[29] - x[0]) / 299.0;
     yInter[i] = s.value( xInter[i] );
   }

   do_something(xInter, yInter, 300);
}
else
  cerr << "Uhhh...\n";
  \endcode
*/

class QWT_EXPORT QwtSpline
{
public:
    QwtSpline();
    ~QwtSpline();

    double value(double x) const;
    int recalc(double *x, double *y, int n, int periodic = 0);
    void copyValues(int tf = 1);

private:
    int buildPerSpline();
    int buildNatSpline();
    int lookup(double x) const;
    void cleanup();

    // coefficient vectors
    double *d_a;
    double *d_b;
    double *d_c;
    double *d_d;

    // values
    double *d_x;
    double *d_y;
    double *d_xbuffer;
    double *d_ybuffer;
    int d_size;

    //flags
    int d_buffered;
};





#endif





