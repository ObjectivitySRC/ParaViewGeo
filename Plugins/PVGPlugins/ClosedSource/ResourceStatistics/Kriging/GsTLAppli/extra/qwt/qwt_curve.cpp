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

#include <qpainter.h>
#include "qwt_math.h"
#include "qwt_dimap.h"
#include "qwt_curve.h"

#include <algorithm>

#define X11_COORD_MIN -16384
#define X11_COORD_MAX 16384

/*!
  \brief Initialize data members
*/
void QwtCurve::init(const QString &title)
{
    d_pen = QPen(Qt::black, 1);
    d_ref = 0.0;
    d_splineSize = 250;
    d_options = Auto;
    d_raw = FALSE;
    d_title = title;
    d_style = Lines;
}

/*!
  \brief Copy the contents of a curve into another curve
*/
void QwtCurve::copy(const QwtCurve &c)
{
    d_ref = c.d_ref;
    d_sym = c.d_sym;
    d_pen = c.d_pen;
    d_title = c.d_title;
    d_raw = FALSE;
    d_style = c.d_style;

    d_splineSize = c.d_splineSize;
    d_options = c.d_options;

    if (c.d_raw)
    {
       d_x.duplicate(c.d_x);
       d_y.duplicate(c.d_y);
    }
    else
    {
       d_x = c.d_x;
       d_y = c.d_y;
    }
}

//! Dtor
QwtCurve::~QwtCurve()
{
    if (d_raw)
    {
        d_x.resetRawData(d_x.data(), d_x.size());
        d_y.resetRawData(d_y.data(), d_y.size());
    }
}

/*!
  \brief Copy Constructor
  \warning If curve c has attached its data in raw mode
  (see QwtCurve::setRawData), the copy constructor
  creates a deep copy of the data. The data are implicitly
  shared otherwise.
*/
QwtCurve::QwtCurve(const QwtCurve &c)
{
    init(c.d_title);
    copy(c);
}

/*!
  \brief Copy Assignment
  \warning If curve c has attached its data in raw mode, the assignment operator
  creates a deep copy of the data. The data are implicitly
  shared otherwise.
  \sa QwtCurve::setRawData()
*/
const QwtCurve& QwtCurve::operator=(const QwtCurve &c)
{
    copy(c);
    curveChanged();
    return *this;
}

/*!
  \brief Set the curve's drawing style

  Valid styles are:
  <dl>
  <dt>QwtCurve::NoCurve</dt>
  <dd>Don't draw a curve. Note: This doesn't affect the symbol. </dd>
  <dt>QwtCurve::Lines</dt>
  <dd>Connect the points with straight lines.</dd>
  <dt>QwtCurve::Sticks</dt>
  <dd>Draw vertical sticks from a baseline which is defined by setBaseline().</dd>
  <dt>QwtCurve::Steps</dt>
  <dd>Connect the points with a step function. The step function
      is drawn from the left to the right or vice versa,
      depending on the 'Inverted' option.</dd>
  <dt>QwtCurves::Dots</dt>
  <dd>Draw dots at the locations of the data points. Note:
      This is different from a dotted line (see setPen()).</dd>
  <dt>QwtCurve::Spline</dt>
  <dd>Interpolate the points with a spline. The spline
      type can be specified with setOptions(),
      the size of the spline (= number of interpolated points)
      can be specified with setSplineSize().</dd>
  <dt>QwtCurve::UserCurve ...</dt>
  <dd>Styles >= QwtCurve::UserCurve are reserved for derived
      classes of QwtCurve that overload QwtCurve::draw() with
      additional application specific curve types.</dd>
  </dl>
  \sa QwtCurve::style()
*/
void QwtCurve::setStyle(int style, int options)
{
    d_options = options;
    d_style = style;
    curveChanged();
}

/*!
    \fn CurveStyle QwtCurve::style() const
    \brief Return the current style
    \sa QwtCurve::setStyle
*/
int QwtCurve::style() const 
{ 
    return d_style; 
}

/*!
  \brief Assign a symbol
  \param s symbol
  \sa QwtSymbol
*/
void QwtCurve::setSymbol(const QwtSymbol &s )
{
    d_sym = s;
    curveChanged();
}

/*!
    \brief Return the current symbol
    \sa QwtCurve::setSymbol
*/
const QwtSymbol &QwtCurve::symbol() const 
{ 
    return d_sym; 
}


/*!
  \brief Assign a pen
  \param p New pen
*/
void QwtCurve::setPen(const QPen &p)
{
    d_pen = p;
    curveChanged();
}

/*!
    \brief Return the pen used to draw the lines
    \sa QwtCurve::setPen
*/
const QPen& QwtCurve::pen() const 
{ 
    return d_pen; 
}



/*!
  \brief Copy x-y data from specified arrays
  \param x pointer to x data
  \param y pointer to y data
  \param size size of x and y
*/
void QwtCurve::setData(double *x, double *y, int size)
{
    if (d_raw)
    {
        d_x.resetRawData(d_x.data(), d_x.size());
        d_y.resetRawData(d_y.data(), d_y.size());
    }
    d_x.duplicate(x, size);
    d_y.duplicate(y, size);
    d_raw = FALSE;
    curveChanged();
}

/*!
  \brief Attach raw data

  setRawData is provided for efficiency. In contrast to setData,
  it does not copy the data, so it is important to keep the pointers
  valid while they are attached.

  The QwtCurve destructor does not delete the attached data, so you
  can safely call setRawData and setData several times subsequently.

  \param x pointer to x data
  \param y pointer to y data
  \param size size of x and y
*/
void QwtCurve::setRawData(double *x, double *y, int size)
{
    if (d_raw)
    {
        d_x.resetRawData(d_x.data(), d_x.size());
        d_y.resetRawData(d_y.data(), d_y.size());
    }

    d_x.setRawData(x, size);
    d_y.setRawData(y, size);
    d_raw = TRUE;
    curveChanged();
}

/*!
  \brief Assign a title to a curve
  \param title new title
*/
void QwtCurve::setTitle(const QString &title)
{
    d_title = title;
    curveChanged();
}

/*!
    \brief Return the title.
    \sa QwtCurve::setTitle
*/
const QString &QwtCurve::title() const 
{ 
    return d_title; 
}

/*!
  \brief find the smallest x value
*/
double QwtCurve::minXValue() const
{
    return qwtGetMin(d_x.data(),d_x.size());
}

/*!
  \brief find the smallest y value
*/
double QwtCurve::minYValue() const
{
    return qwtGetMin(d_y.data(), d_y.size());
}

/*!
  \brief find the largest x value
*/
double QwtCurve::maxXValue() const
{
    return qwtGetMax(d_x.data(), d_x.size());
}

/*!
  \brief find the largest y value
*/
double QwtCurve::maxYValue() const
{
    return qwtGetMax(d_y.data(), d_y.size());
}


/*!
  \brief Ctor
  \param title title of the curve   
*/
QwtCurve::QwtCurve(const QString &title)
{
    init(title);
}

/*!
  \brief Checks if a range of indices is valid and corrects it if necessary
  \param i1 Index 1
  \param i2 Index 2
*/
int QwtCurve::verifyRange(int &i1, int &i2)
{
    int size = dataSize();

    if (size < 1) return 0;

    i1 = qwtLim(i1, 0, size-1);
    i2 = qwtLim(i2, 0, size-1);
    qwtSort(i1, i2, i1, i2);

    return (i2 - i1 + 1);
}

/*!
  \brief Draw an intervall of the curve
  \param p Painter
  \param xMap maps x-values into pixel coordinates.
  \param yMap maps y-values into pixel coordinates.
  \param from index of the first point to be painted
  \param to index of the last point to be painted. If to < 0 the 
         curve will be painted to its last point.
  \sa qwt_drawPoint, qwt_drawLine, qwt_drawPolyline
*/
void QwtCurve::draw(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    if ( !painter || dataSize() <= 0 )
        return;

    if (to < 0)
        to = dataSize() - 1;

    if ( verifyRange(from, to) > 0 )
    {
        painter->save();
        painter->setPen(d_pen);

        drawCurve(painter, d_style, xMap, yMap, from, to);
        if (d_sym.style() != QwtSymbol::None)
            drawSymbols(painter, d_sym, xMap, yMap, from, to);

        painter->restore();
    }
}

/*!
  \brief Draw the line part (without symbols) of a curve interval. 
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa qwt_drawPoint, qwt_drawLine, qwt_drawPolyline
*/

void QwtCurve::drawCurve(QPainter *painter, int style,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    switch (style)
    {
        case NoCurve:
            break;
        case Lines:
            drawLines(painter, xMap, yMap, from, to);
            break;
        case Sticks:
            drawSticks(painter, xMap, yMap, from, to);
            break;
        case Steps:
            drawSteps(painter, xMap, yMap, from, to);
            break;
        case Spline:
            if ( from > 0 || to < dataSize() - 1 )
                drawLines(painter, xMap, yMap, from, to);
            else
                drawSpline(painter, xMap, yMap);
            break;
        case Dots:
            drawDots(painter, xMap, yMap, from, to);
            break;
        //--------------
        // added by Nicolas Remy, may 4 2003
        case Histogram:
            drawHistogram( painter, xMap, yMap, from, to );
            break;
        //---------------
        default:
            break;
    }
}

/*!
  \brief Draw lines
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa qwt_drawPoint, qwt_drawLine, qwt_drawPolyline
*/
void QwtCurve::drawLines(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    QPointArray polyline(to - from + 1);
    for (register int i = from; i <= to; i++)
    {
        int x = xMap.transform(d_x[i]);
        int y = yMap.transform(d_y[i]);

        polyline.setPoint(i - from, x, y);
    }

    QwtCurve::drawPolyline(painter, polyline);
}

/*!
  \brief Draw sticks
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa qwt_drawPoint, qwt_drawLine, qwt_drawPolyline
*/
void QwtCurve::drawSticks(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    int x0 = xMap.transform(d_ref);
    int y0 = yMap.transform(d_ref);

    for (int i = from; i <= to; i++)
    {
        int x = xMap.transform(d_x[i]);
        int y = yMap.transform(d_y[i]);

        if (d_options & Xfy)
            QwtCurve::drawLine(painter, x0, y, x, y);
        else
            QwtCurve::drawLine(painter, x, y0, x, y);
    }
}

/*!
  \brief Draw dots
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa qwt_drawPoint, qwt_drawLine, qwt_drawPolyline
*/
void QwtCurve::drawDots(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    for (int i = from; i <= to; i++)
    {
        int x = xMap.transform(d_x[i]);
        int y = yMap.transform(d_y[i]);
        QwtCurve::drawPoint(painter, x,y);
    }
}

/*!
  \brief Draw step function
  \param p Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa qwt_drawPoint, qwt_drawLine, qwt_drawPolyline
*/
void QwtCurve::drawSteps(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    QPointArray polyline(2 * (to - from) + 1);

    bool inverted = d_options & Yfx;
    if ( d_options & Inverted )
        inverted = !inverted;

    int i,ip;
    for (i = from, ip = 0; i <= to; i++, ip += 2)
    {
        int x = xMap.transform(d_x[i]);
        int y = yMap.transform(d_y[i]);

        if ( ip > 0 )
        {
            if (inverted)
                polyline.setPoint(ip - 1, polyline[ip-2].x(), y);
            else
                polyline.setPoint(ip - 1, x, polyline[ip-2].y());
        }

        polyline.setPoint(ip, x, y);
    }

    QwtCurve::drawPolyline(painter, polyline);
}

//-------------------------
// added by Nicolas Remy, may 4 2003
void QwtCurve::drawHistogram( QPainter *painter,
                             const QwtDiMap &xMap, const QwtDiMap &yMap,
                             int from, int to) {
    QPointArray polyline(2 * (to - from) + 1);

    int y_ref = yMap.transform(d_ref);

    int i,ip;
    for (i = from, ip = 0; i <= to; i++, ip += 2)
    {
      int x0 = xMap.transform(d_x[i]);
      int y0 = yMap.transform(d_y[i]);

      if( i+1 <= to ) {
        int x1 = xMap.transform(d_x[i+1]);
        int y1 = yMap.transform(d_y[i+1]);
        painter->fillRect( QRect( QPoint(x0, y0), QPoint(x1, y_ref) ), QColor( 150,150,150 ) );
        painter->drawRect( QRect( QPoint(x0, y0), QPoint(x1, y_ref) ) );
      }      
    }
}

//-------------------------

/*!
  \brief Draw a spline
  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa qwt_drawPoint, qwt_drawLine, qwt_drawPolyline
*/
void QwtCurve::drawSpline(QPainter *painter,
    const QwtDiMap &xMap, const QwtDiMap &yMap)
{
    register int i;

    int size = dataSize();
    double *txval = new double[size];
    double *tyval = new double[size];


    if ( !txval || !tyval )
    {
        if (txval) delete[] txval;
        if (tyval) delete[] tyval;
        return;
    }

    QPointArray polyline(d_splineSize);

    //
    // Transform x and y values to window coordinates
    // to avoid a distinction between linear and
    // logarithmic scales.
    //
    for (i=0;i<size;i++)
    {
        txval[i] = xMap.xTransform(d_x[i]);
        tyval[i] = yMap.xTransform(d_y[i]);
    }

    int stype;
    if (! (d_options & (Yfx|Xfy|Parametric)))
    {
        if (qwtChkMono(txval, size))
        {
            stype = Yfx;
        }
        else
        {
            if(qwtChkMono(tyval, size))
            {
                stype = Xfy;
            }
            else
            {
                stype = Parametric;
                if ( (d_options & Periodic) ||
                    ( (d_x[0] == d_x[size-1])
                    && (d_y[0] == d_y[size-1])))
                {
                    stype |= Periodic;
                }
            }
        }
    }
    else
    {
        stype = d_options;
    }

    if (stype & Parametric)
    {
        double *param = new double[size];
        if (param)
        {
            //
            // setup parameter vector
            //
            param[0] = 0.0;
            for (i=1; i<size; i++)
            {
                double delta = sqrt( qwtSqr(txval[i] - txval[i-1])
                              + qwtSqr( tyval[i] - tyval[i-1]));
                param[i] = param[i-1] + qwtMax(delta, 1.0);
            }

            //
            // setup splines
            int rc = d_spx.recalc(param, txval, size, stype & Periodic);
            if (!rc)
                rc = d_spy.recalc(param, tyval, size, stype & Periodic);

            if (rc)
            {
                drawLines(painter, xMap, yMap, 0, size - 1);
            }
            else
            {
                // fill point array
                double delta = param[size - 1] / double(d_splineSize-1);
                for (i=0;i<d_splineSize;i++)
                {
                    double dtmp = delta * double(i);
                    polyline.setPoint(i, int(floor (d_spx.value(dtmp) + 0.5)),
                                  int(floor (d_spy.value(dtmp) + 0.5)));
                }
            }

            delete[] param;
        }
    }
    else if (stype & Xfy)
    {
        if (tyval[size-1] < tyval[0])
        {
            qwtTwistArray(txval, size);
            qwtTwistArray(tyval, size);
        }

        // 1. Calculate spline coefficients
        int rc = d_spx.recalc(tyval, txval, size, stype & Periodic);
        if (rc)                         // an error occurred
        {
            drawLines(painter, xMap, yMap, 0, size - 1);
        }
        else                            // Spline OK
        {
            double ymin = qwtGetMin(tyval, size);
            double ymax = qwtGetMax(tyval, size);
            double delta = (ymax - ymin) / double(d_splineSize - 1);

            for (i=0;i<d_splineSize;i++)
            {
                double dtmp = ymin + delta * double(i);
                polyline.setPoint(i, int(floor(d_spx.value(dtmp) + 0.5)),
                              int(floor(dtmp + 0.5)));
            }
        }
    }
    else
    {
        if (txval[size-1] < txval[0])
        {
            qwtTwistArray(tyval, size);
            qwtTwistArray(txval, size);
        }


        // 1. Calculate spline coefficients
        int rc = d_spy.recalc(txval, tyval, size, stype & Periodic);
        if (rc)                         // error
        {
            drawLines(painter, xMap, yMap, 0, size - 1);
        }
        else                            // Spline OK
        {
            double xmin = qwtGetMin(txval, size);
            double xmax = qwtGetMax(txval, size);
            double delta = (xmax - xmin) / double(d_splineSize - 1);

            for (i=0;i<d_splineSize;i++)
            {
                double dtmp = xmin + delta * double(i);
                polyline.setPoint(i, int(floor (dtmp + 0.5)),
                              int(floor(d_spy.value(dtmp) + 0.5)));
            }
        }
    }

    delete[] txval;
    delete[] tyval;

    QwtCurve::drawPolyline(painter, polyline);
}

/*!
  \brief Specify options for the drawing style  

  The options can be used to modify the drawing style.
  Options can be or-combined.
  The following options are defined:<dl>
  <dt>QwtCurve::Auto</dt>
  <dd>The default setting. For QwtCurve::spline,
      this means that the type of the spline is
      determined automatically, depending on the data.
      For all other styles, this means that y is
      regarded as a function of x.</dd>
  <dt>QwtCurve::Yfx</dt>
  <dd>Draws y as a function of x (the default). The
      baseline is interpreted as a horizontal line
      with y = baseline().</dd>
  <dt>QwtCurve::Xfy</dt>
  <dd>Draws x as a function of y. The baseline is
      interpreted as a vertical line with x = baseline().</dd>
  <dt>QwtCurve::Parametric</dt>
  <dd>For QwtCurve::Spline only. Draws a parametric spline.</dd>
  <dt>QwtCurve::Periodic</dt>
  <dd>For QwtCurve::Spline only. Draws a periodic spline.</dd>
  <dt>QwtCurve::Inverted</dt>
  <dd>For QwtCurve::Steps only. Draws a step function
      from the right to the left.</dd></dl>

  \param opt new options
  /sa QwtCurve::options()
*/
void QwtCurve::setOptions(int opt)
{
    d_options = opt;
    curveChanged();
}

/*!
    \brief Return the current style options
    \sa QwtCurve::setOptions
*/
int QwtCurve::options() const 
{ 
    return d_options; 
}

/*!
  \brief Change the number of interpolated points
  \param s new size
  \warning The default is 250 points.
*/
void QwtCurve::setSplineSize(int s)
{
    d_splineSize = qwtMax(s, 10);
    curveChanged();
}

/*!
    \fn int QwtCurve::splineSize() const
    \brief Return the spline size
    \sa QwtCurve::setSplineSize
*/

int QwtCurve::splineSize() const 
{ 
    return d_splineSize; 
}

/*!
  \brief Draw symbols
  \param painter Painter
  \param symbol Curve symbol
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
*/
void QwtCurve::drawSymbols(QPainter *painter, QwtSymbol &symbol,
    const QwtDiMap &xMap, const QwtDiMap &yMap, int from, int to)
{
    int w2 = (symbol.size().width() - 1) / 2;
    int h2 = (symbol.size().height() - 1) / 2;

    for (int i = from; i <= to; i++)
    {
        int u = xMap.transform(d_x[i]);
        int v = yMap.transform(d_y[i]);
#if defined(Q_WS_X11)
        if ( u - w2 > X11_COORD_MIN && u + w2 <= X11_COORD_MAX
            && v - h2 > X11_COORD_MIN && v + h2 <= X11_COORD_MAX )
#endif
        {
            symbol.draw(painter, u - w2, v - h2);
        }
    }
}

/*!
  \brief Set the value of the baseline

  The baseline is needed for the QwtCurve::Sticks
  drawing style. The default value is 0.0. The interpretation
  of the baseline depends on the style options. With QwtCurve::Yfx,
  the baseline is interpreted as a horizontal line at y = baseline(),
  with QwtCurve::Yfy, it is interpreted as a vertical line at
  x = baseline().
  \param ref baseline
  \sa QwtCurve::setStyle(), QwtCurve::setOptions()
*/
void QwtCurve::setBaseline(double ref)
{
    d_ref = ref;
    curveChanged();
}

/*!
    \brief Return the value of the baseline
    \sa QwtCurve::setBaseline
*/
double QwtCurve::baseline() const 
{ 
    return d_ref; 
}

/*!
  \brief Return the size of the data arrays
*/
int QwtCurve::dataSize() const
{
    return qwtMin(d_x.size(), d_y.size());
}

/*!
    \brief Notify a change of attributes.
    This virtual function is called when an attribute of the curve
    has changed. It can be redefined by derived classes.
    The default implementation does nothing.
*/

void QwtCurve::curveChanged() 
{
}

#if defined(Q_WS_X11)
/*
 * Sutherland-Hodgman polygon clipping
 */

static bool qwt_inside_edge( const QPoint &p, const QRect &r, int edge )
{
    switch ( edge ) {
    case 0:
        return p.x() > r.left();
    case 1:
        return p.y() > r.top();
    case 2:
        return p.x() < r.right();
    case 3:
        return p.y() < r.bottom();
    }

    return FALSE;
}

static QPoint qwt_intersect_edge(const QPoint &p1, const QPoint &p2,
    const QRect &r, int edge )
{
    int x=0, y=0;
    double m = 0;
    double dy = p2.y() - p1.y();
    double dx = p2.x() - p1.x();

    switch ( edge ) {
    case 0:
        x = r.left();
        m = double(QABS(p1.x() - x)) / QABS(dx);
        y = p1.y() + int(dy * m);
        break;
    case 1:
        y = r.top();
        m = double(QABS(p1.y() - y)) / QABS(dy);
        x = p1.x() + int(dx * m);
        break;
    case 2:
        x = r.right();
        m = double(QABS(p1.x() - x)) / QABS(dx);
        y = p1.y() + int(dy * m);
        break;
    case 3:
        y = r.bottom();
        m = double(QABS(p1.y() - y)) / QABS(dy);
        x = p1.x() + int(dx * m);
        break;
    }

    return QPoint(x,y);
}

static QPointArray qwt_clipPolyline(const QRect &r,
    const QPointArray &pa, int &index, int &npoints )
{
    if ( r.contains( pa.boundingRect() ) )
    return pa;

    QPointArray rpa = pa;
    QPointArray cpa( pa.size() );
    unsigned int count = 0;

#define ADD_POINT(p) if ( cpa.size() == count ) cpa.resize( count+5 ); \
             cpa.setPoint( count++, p );

    for ( int e = 0; e < 4; e++ ) {
    count = 0;
    QPoint s = rpa.point( index+npoints-1 );
    for ( int j = 0; j < npoints; j++ ) {
        QPoint p = rpa.point( index+j );
        if ( qwt_inside_edge( p, r, e ) ) {
        if ( qwt_inside_edge( s, r, e ) ) {
            ADD_POINT(p);
        } else {
            QPoint i = qwt_intersect_edge( s, p, r, e );
            ADD_POINT(i);
            ADD_POINT(p);
        }
        } else if ( qwt_inside_edge( s, r, e ) ) {
        QPoint i = qwt_intersect_edge( s, p, r, e );
        ADD_POINT(i);
        }
        s = p;
    }
    index = 0;
    npoints = count;
    rpa = cpa.copy();
    }

#undef ADD_POINT

    cpa.resize( npoints );

    return cpa;
}
#endif // Q_WS_X11

/*!
  \brief Wrapper for painter->drawLine(x1, y1, x2, y2). On X11
         the cutting points with [-16384 .. 16384] will be calculated.
         Everything outside will be skipped to avoid overruns.
*/

void QwtCurve::drawLine(QPainter *painter, int x1, int y1, int x2, int y2)
{
#if defined(Q_WS_X11)
    QPointArray pa(2);
    pa.setPoint(0, QPoint(x1, y1));
    pa.setPoint(1, QPoint(x2, y2));
    QwtCurve::drawPolyline(painter, pa);
#else
    painter->drawLine(x1, y1, x2, y2);
#endif
}

/*!
  \brief Wrapper for painter->drawPolyline(x1, y1, x2, y2). On X11
         the cutting points with [-16384 .. 16384] will be calculated.
         Everything outside will be skipped to avoid overruns.
*/

void QwtCurve::drawPolyline(QPainter *painter, const QPointArray &pa)
{
#if defined(Q_WS_X11)
    // clip to short coords
    QRect clip;
    clip.setCoords( X11_COORD_MIN, X11_COORD_MIN,
        X11_COORD_MAX, X11_COORD_MAX );
    int idx = 0;
    int size = pa.size();
    QPointArray cpa = qwt_clipPolyline(clip, pa, idx, size);
    painter->drawPolyline(cpa);
#else
    painter->drawPolyline(pa);
#endif
}

/*!
  \brief Wrapper for painter->drawPoint(x, y). On X11 points outside
         [-16384 .. 16384] will be skipped to avoid overruns.
*/

void QwtCurve::drawPoint(QPainter *painter, int x, int y)
{
#if defined(Q_WS_X11)
    if ( x > X11_COORD_MIN && x <= X11_COORD_MAX
        && y > X11_COORD_MIN && y <= X11_COORD_MAX )
#endif

    {
        painter->drawPoint(x,y);
    }
}
