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

#ifndef QWT_CURVE_H
#define QWT_CURVE_H

#include <qpen.h>
#include <qstring.h>
#include <qmemarray.h>
#include "qwt_global.h"
#include "qwt_spline.h"
#include "qwt_symbol.h"

class QPainter;
class QwtDiMap;

#if defined(QWT_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QWT_EXPORT QMemArray<double>;
// MOC_SKIP_END
#endif

/*!
  \brief A class which draws curves

  This class can be used to display data as a curve in the  x-y plane.
  It supports different display styles, spline interpolation and symbols.

  \par Usage
  <dl><dt>A. Assign Properties</dt>
  <dd>When a curve is created, it is configured to draw black solid lines
  with QwtCurve::Lines and no symbols. You can change this by calling 
  QwtCurve::setPen(), QwtCurve::setStyle() and QwtCurve::setSymbol().</dd>
  <dt>B. Assign or change data.</dt>
  <dd>Data can be assigned in two ways:<ul>
  <li>QwtCurve::setData() copies the x and y data from the specified
  arrays into its internal buffer.
  <li>QwtCurve::setRawData() does not make a copy of the data, but
  only stores the pointers and size information instead. This
  function is less safe (you must not delete the data while they are attached),
  but more efficient, and more
  convenient if the data change dynamically.</ul></dd>
  <dt>C. Draw</dt>
  <dd>QwtCurve::drawCurve() maps the curve points into pixel coordinates 
      and paints them.
  </dd></dl>

  \par Example:
  see examples/curvdemo

  \sa QwtSymbol, QwtDiMap
*/
class QWT_EXPORT QwtCurve
{
public:
    /*! 
        Curve styles. 
        \sa QwtCurve::setStyle
        Nicolas Remy - may 4, 2003 - added Histogram style
    */
    enum CurveStyle
    {
        NoCurve,
        Lines,
        Sticks,
        Steps,
        Dots,
        Spline,
        Histogram,
        UserCurve = 100
    };

    /*! 
        Curve options. 
        \sa QwtCurve::setOptions
    */
    enum CurveOption
    {
        Auto = 0,
        Yfx = 1,
        Xfy = 2,
        Parametric = 4,
        Periodic = 8,
        Inverted = 16
    };

    QwtCurve(const QString &title = QString::null);
    QwtCurve(const QwtCurve &c);
    virtual ~QwtCurve();

    virtual const QwtCurve& operator= (const QwtCurve &c);

    void setRawData(double *x, double *y, int size);
    void setData(double *x, double *y, int size);

    int dataSize() const;
    inline double x(int i) const;
    inline double y(int i) const;

    virtual double minXValue() const;
    virtual double maxXValue() const;
    virtual double minYValue() const;
    virtual double maxYValue() const;

    void setOptions(int t);
    int options() const;

    void setTitle(const QString &title);
    const QString &title() const;

    void setPen(const QPen &p);
    const QPen& pen() const;

    void setBaseline(double ref);
    double baseline() const;

    void setStyle(int style, int options = 0);
    int style() const;

    void setSymbol(const QwtSymbol &s);
    const QwtSymbol& symbol() const;

    void setSplineSize(int s);
    int splineSize() const;

    virtual void draw(QPainter *p, const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from = 0, int to = -1);

	static void drawPoint(QPainter *, int x, int y);
	static void drawLine(QPainter *, int x1, int y1, int x2, int y2);
	static void drawPolyline(QPainter *, const QPointArray &);

	//TL modified
	QMemArray<double> & dataX() { return d_x; }
	QMemArray<double> & dataY() { return d_y; }

protected:

    void init(const QString &title);
    void copy(const QwtCurve &c);

    virtual void drawCurve(QPainter *p, int style,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);

    virtual void drawSymbols(QPainter *p, QwtSymbol &,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);

    void drawLines(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);
    void drawSticks(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);
    void drawDots(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);
    void drawSteps(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);
    void drawSpline(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap);
//----------------
    // added by Nicolas Remy, may 4 2003
    void drawHistogram(QPainter *p,
        const QwtDiMap &xMap, const QwtDiMap &yMap,
        int from, int to);
//----------------
    virtual void curveChanged();

    int verifyRange(int &i1, int &i2);

protected:
    bool d_raw;
    QMemArray<double> d_x;
    QMemArray<double> d_y;

    QwtSpline d_spx;
    QwtSpline d_spy;

private:
    int d_style;
    double d_ref;

    QwtSymbol d_sym;

    QPen d_pen;
    QString d_title;

    int d_options;
    int d_splineSize;
};

/*!
    \param i index
    \return x-value at position i
*/
inline double QwtCurve::x(int i) const 
{ 
    return d_x[i]; 
}

/*!
    \param i index
    \return y-value at position i
*/
inline double QwtCurve::y(int i) const 
{ 
    return d_y[i]; 
}

#endif
