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

#ifndef QWT_AUTOSCL_H
#define QWT_AUTOSCL_H

#include "qwt_global.h"
#include "qwt_scldiv.h"

/*!
  \brief The Qwt Auto-Scaler

  This class can be  used to generate a scale which may span 
  multiple ranges of values. A scale consists of a lower boundary,
  an upper boundary, a vector of major scale ticks and a vector of
  minor scale ticks which divide it into subintervals.
  A quick look at the example below will give you an idea
  of how the auto-scaler works.
  
  The auto-scaler produces "reasonable" major and minor step sizes. For
  linear scales, the major step size will fit into the pattern 
  \f$\left\{ 1,2,5\right\} \cdot 10^{n}\f$,
  where n is an integer. In logarithmic mode (setOptions())
  the step size is measured in *decades*
  and the major step size will be adjusted to fit the pattern
  \f$\left\{ 1,2,3,5\right\} \cdot 10^{n}\f$, where n is a natural number
  including zero.
  
  The step size can be manipulated indirectly using
  setMaxMajor().
  The layout of the scale can be varied with  setOptions().
  
  The auto-scaling algorithm can be partly or completely disabled
  (even temporarily) if a user-defined scale is desired. This can be done with
  the setScale() function. It can be switched off
  again with setAutoScale().
  
  The two adjust() members are used to extend
  the scale if necessary in order to include another range or array of
  values. The resulting scale division can be obtained with
  scaleDiv(). reset() resets the scale to zero.

  \par Example

\code
#include <qwt_autoscl.h>
#include <iostream.h>

double x1[100];
double x2[200];
double range_min, range_max;

QwtAutoScale as;

// ... determine x1 and x1, range_min and range_max here ...

as.reset();                           // clear it
as.adjust(range_min, range_max);      // include a range
as.adjust(x1,100);                    // include an array
as.adjust(x2,200);                    // include another array

for (i=0;i<as.scaleDiv().majCnt(); i++)
{
  cout << "Scale tick " << i
  << " at " << as.scaleDiv().majMark(i) << "\n";
}
  \endcode

  \warning For logarithmic scales, the step size as well as
  the margins are measured in *decades*.
*/
class QWT_EXPORT QwtAutoScale 
{
public:
    enum {None = 0, IncludeRef = 1, Symmetric = 2, Floating = 4,
        Logarithmic = 8, Inverted = 16 };

    QwtAutoScale();
    ~QwtAutoScale();

    void setAutoScale();

    // added by Nicolas Remy, may 23, 2003
    void setAutoScale( bool on );

    bool autoScale() const;

    void setAutoRebuild(bool); 
    bool autoRebuild() const;

    void changeOptions(int opt, bool tf);
    void setOptions(int opt);
    bool option(int opt) const;
    int options() const;

    void setMaxMajor( int n );
    int maxMajor() const;
    void setMaxMinor(int n);
    int maxMinor() const;

    void setReference(double r);
    double reference() const;

    void setMargins(double m1, double m2);
    double loMargin() const;
    double hiMargin() const;

    void setScale(double xmin, double xmax, double step = 0.0);
    const QwtScaleDiv &scaleDiv() const;

    void adjust(double *arr, int n, int reset = 0);
    void adjust(double x1, double x2, int reset = 0);

    void build();
    void reset();

protected:
    void buildLinScale();
    void buildLogScale();
    void setRange(double x1, double x2);

private:
    QwtScaleDiv d_scldiv;

    double d_minValue;          // smallest input value
    double d_maxValue;          // greatest input value

    double d_scaleMin;          // scale minimum
    double d_scaleMax;          // scale maximum

    double d_step;          // user-defined step size
    int d_maxMajor;         // max. no. of scale divisions
    int d_maxMinor;         // max. number of minor intervals

    int d_scaleOpt;         // scale options
    bool d_autoScale;           // autoscale mode

    double d_loMargin;          // margins
    double d_hiMargin;

    int d_reset;            // d_minValue and d_maxValue are invalid
    double d_ref;           // reference point
    double d_lref;          // special reference point for logarithmic scales

    bool d_autoRebuild;         // rebuild scale automatically with
            // call to 'adjust'

};

#endif
