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

/*!
  \file qwt_math.h
  \brief A set of mathematical routines
*/

#ifndef QWT_MATH_H
#define QWT_MATH_H

#include <math.h>
#include "qwt_global.h"


#define LOG10_2 	0.30102999566398119802  /* log10(2) */
#define LOG10_3 	0.47712125471966243540  /* log10(3) */
#define LOG10_5 	0.69897000433601885749  /* log10(5) */
#ifndef M_2PI
#define M_2PI 		6.28318530717958623200  /* 2 pi */
#endif

#define LOG_MIN 1.0e-100
#define LOG_MAX 1.0e100

#ifdef Q_WS_WIN
# define M_E		2.7182818284590452354	/* e */
# define M_LOG2E	1.4426950408889634074	/* log_2 e */
# define M_LOG10E	0.43429448190325182765	/* log_10 e */
# define M_LN2		0.69314718055994530942	/* log_e 2 */
# define M_LN10		2.30258509299404568402	/* log_e 10 */
# define M_PI		3.14159265358979323846	/* pi */
# define M_PI_2		1.57079632679489661923	/* pi/2 */
# define M_PI_4		0.78539816339744830962	/* pi/4 */
# define M_1_PI		0.31830988618379067154	/* 1/pi */
# define M_2_PI		0.63661977236758134308	/* 2/pi */
# define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
# define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
# define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */
#endif

QWT_EXPORT double qwtCeil125(double x);
QWT_EXPORT double qwtFloor125(double x);
QWT_EXPORT double qwtGetMin (double *array, int size);
QWT_EXPORT double qwtGetMax( double *array, int size);
QWT_EXPORT void qwtTwistArray(double *array, int size);
QWT_EXPORT int qwtChkMono(double *array, int size);
QWT_EXPORT void qwtLinSpace(double *array, int size, double xmin, double xmax);
QWT_EXPORT void qwtLogSpace(double *array, int size, double xmin, double xmax);


//! Return the largest of two values
template <class T>
inline const T& qwtMax (const T& x, const T& y) 
{
    return ( x > y ? x : y );
}

//! Return the smallest of two values 
template <class T>
inline const T& qwtMin ( const T& x, const T& y)
{
    return ( x < y ? x : y );
}

//! Return the absolute value
template <class T>
inline T qwtAbs (const T& x)
{
    return( x > T(0) ? x : -x );
}

//! Return the sign 
template <class T>
inline int qwtSign(const T& x)
{
    if (x > T(0))
       return 1;
    else if (x < T(0))
       return (-1);
    else
       return 0;
}			 

//! Return the square of a number
template <class T>
inline T qwtSqr(const T&x)
{
    return x*x;
}

/*!
  \brief Copy an array into another
  \param dest Destination
  \param src Source
  \param n Number of elements  
*/
template <class T>
void qwtCopyArray(T *dest, T *src, int n) 
{
    int i;
    for (i=0; i<n;i++ )
       dest[i] = src[i];
}

/*!
  \brief Shift an array by a specified number of positions 
  \param arr Pointer to an array
  \param size Number of elements
  \param di Number of positions. A positive number shifts right,  
		a negative number shifts left.
*/
template <class T>
void qwtShiftArray(T *arr, int size, int di)
{
    int i, delta;
    T* buffer = 0;
    
    delta = qwtAbs(di);

    if ((delta > 0) && (delta < size))
    {
	if ((buffer = new T[delta]))
	{
	    if (di < 0)			// shift left
	    {
		qwtCopyArray(buffer, arr, delta);
		qwtCopyArray(&arr[0], &arr[delta], size - delta);
		qwtCopyArray(&arr[size - delta], buffer, delta);
		
	    }
	    else			// shift right
	    {
		qwtCopyArray(buffer, &arr[size - delta], delta);
		for ( i = size-delta-1; i >= 0; i-- )
		   arr[i + delta] = arr[i];
		qwtCopyArray(arr, buffer, delta);
	    }
	}
    }
    
    if (buffer != 0) delete[] buffer;
}

//! Swap two values 
template <class T>
void qwtSwap( T &x1, T& x2)
{
    T tmp;
    tmp = x1;
    x1 = x2;
    x2 = tmp;
}


//! Round a number to the nearest integer 
inline int qwtInt(double x)
{
    return int(floor (x + 0.5));
}

/*!
  \brief Sort two values in ascending order
  \param x1 First input value
  \param x2 Second input value
  \param xmax Greater value
  \param xmin Smaller value
*/
template <class T>
void qwtSort(const T& x1, const T& x2, T& xmin, T& xmax)
{
    T buffer;
    
    if (x2 < x1)
    {
	buffer = x1;
	xmin = x2;
	xmax = buffer;
    }
    else
    {
	xmin = x1;
	xmax = x2;
    }
}

//! Sort two values in ascending order 
template <class T>
void qwtSort(T& x1, T& x2)
{
    T buffer;
    
    if (x2 < x1)
    {
	buffer = x1;
	x1 = x2;
	x2 = buffer;
    }
}

/*!
  \brief Limit a value to fit into a specified interval
  \param x Input value
  \param x1 First interval boundary
  \param x2 Second interval boundary  
*/
template <class T>
T qwtLim(const T& x, const T& x1, const T& x2)
{
    T rv;
    T xmin, xmax;
    
    xmin = qwtMin(x1, x2);
    xmax = qwtMax(x1, x2);

    if ( x < xmin )
       rv = xmin;
    else if ( x > xmax )
       rv = xmax;
    else
       rv = x;

    return rv;
}

#endif
