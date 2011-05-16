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

#ifndef QWT_DIMAP_H
#define QWT_DIMAP_H

#include "qwt_global.h"

/*!
  \brief Map a double interval into an integer interval

  The QwtDiMap class maps an interval of type double into an interval of
  type int. It consists
  of two intervals D = [d1, d2] (double) and I = [i1, i2] (int), which are
  specified with the QwtDiMap::setDblRange and QwtDiMap::setIntRange
  members. The point d1 is mapped to the point i1, and d2 is mapped to i2. 
  Any point inside or outside D can be mapped to a point inside or outside
  I using QwtDiMap::transform or QwtDiMap::limTransform or vice versa
  using QwtPlot::invTransform. D can be scaled linearly or
  logarithmically, as specified with QwtDiMap::setDblRange.

  <b>Usage</b>
\verbatim
#include <qwt_dimap.h>

QwtDiMap map;
int ival;
double dval;

map.setDblRange(0.0, 3.1415);   // Assign an interval of type double with
				// linear mapping
map.setIntRange(0,100);         // Assign an integer interval

ival = map.transform(1.0);      // obtain integer value corresponding to 1.0
dval = map.invTransform(77);    // obtain double value corresponding to 77
\endverbatim
*/

class QWT_EXPORT QwtDiMap
{
public:
	QwtDiMap();
	QwtDiMap(int i1, int i2, double d1, double d2, bool lg = FALSE);
	~QwtDiMap();

	bool contains(double x) const;
	bool contains(int x) const;

	void setIntRange(int i1, int i2);
	void setDblRange(double d1, double d2, bool lg = FALSE);

	int transform(double x) const;
	double invTransform(int i) const;

	int limTransform(double x) const;
	double xTransform(double x) const;

	inline double d1() const;
	inline double d2() const;
	inline int i1() const;
	inline int i2() const;
	inline bool logarithmic() const;

	QT_STATIC_CONST double LogMin;
	QT_STATIC_CONST double LogMax;

private:
	void newFactor();	

	double d_x1, d_x2;  // double interval boundaries
	int d_y1, d_y2;     // integer interval boundaries
	double d_cnv;       // conversion factor
	bool d_log;		// logarithmic scale?
};

/*!
    \return the first border of the double interval
*/
inline double QwtDiMap::d1() const 
{
	return d_x1;
}

/*!
    \return the second border of the double interval
*/
inline double QwtDiMap::d2() const 
{
	return d_x2;
}

/*!
    \return the second border of the integer interval
*/
inline int QwtDiMap::i1() const 
{
	return d_y1;
}

/*!
    \return the second border of the integer interval
*/
inline int QwtDiMap::i2() const 
{
	return d_y2;
}

/*!
    \return TRUE if the double interval is scaled logarithmically
*/
inline bool QwtDiMap::logarithmic() const 
{
	return d_log;
}

#endif
