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
** GNU General Public License veBrsion 2 as published by the Free Software
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

#include "qwt_plot.h"
#include "qwt_plot_dict.h"
#include "qwt_math.h"
#include "qwt_legend.h"

/*!
  Find the curve which is closest to a
  specified point in the plotting area.
  \param xpos
  \param ypos position in the plotting region
  \retval dist distance in points between (xpos, ypos) and the
               closest data point
  \return Key of the closest curve or 0 if no curve was found.
*/
long QwtPlot::closestCurve(int xpos, int ypos, int &dist) const
{
    double x,y;
    int index;
    return closestCurve(xpos, ypos, dist, x,y, index);
}

/*!
  Find the curve which is closest to a point in the plotting area.
  
  Determines the position and index of the closest data point.
  \param xpos
  \param ypos coordinates of a point in the plotting region
  \retval xval
  \retval yvalvalues of the closest point in the curve's data array
  \retval dist -- distance in points between (xpos, ypos) and the
                  closest data point
  \retval index -- index of the closest point in the curve's data array
  \return Key of the closest curve or 0 if no curve was found.
*/
long QwtPlot::closestCurve(int xpos, int ypos, int &dist, double &xval,
                           double &yval, int &index) const
{
    QwtDiMap map[axisCnt];
    for ( int axis = 0; axis < axisCnt; axis++ )
        map[axis] = canvasMap(axis);

    long rv = 0;
    double dmin = 1.0e10;

    QIntDictIterator<QwtPlotCurve> itc(*d_curves);
    for (QwtPlotCurve *c = itc.toFirst(); c != 0; c = ++itc )
    {
        for (int i=0; i<c->dataSize(); i++)
        {
            double cx = map[c->xAxis()].xTransform(c->x(i)) - double(xpos);
            double cy = map[c->yAxis()].xTransform(c->y(i)) - double(ypos);

            double f = qwtSqr(cx) + qwtSqr(cy);
            if (f < dmin)
            {
                dmin = f;
                rv = itc.currentKey();
                xval = c->x(i);
                yval = c->y(i);
                index = i;
            }
        }
    }

    dist = int(sqrt(dmin));
    return rv;
}



/*!
  \return the style of the curve indexed by key
  \param key Key of the curve
  \sa setCurveStyle()
*/
int QwtPlot::curveStyle(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
	return c ? c->style() : 0;
}

/*!
  \brief the symbol of the curve indexed by key
  \param key Key of the curve
  \return The symbol of the specified curve. If the key is invalid,
          a symbol of type 'NoSymbol'.
*/
QwtSymbol QwtPlot::curveSymbol(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
	return c ? c->symbol() : QwtSymbol();
}

/*!
  \return the pen of the curve indexed by key
  \param key Key of the curve
*/
QPen QwtPlot::curvePen(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
	return c ? c->pen() : QPen();
}

/*!
  \return the drawing options of the curve indexed by key
  \param key Key of the curve
*/
int QwtPlot::curveOptions(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
	return c ? c->options() : 0;
}

/*!
  \return the spline size of the curve indexed by key
  \param key Key of the curve
*/
int QwtPlot::curveSplineSize(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
	return c ? c->splineSize() : 0;
}

/*!
  \return the title of the curve indexed by key
  \param key Key of the curve
*/
QString QwtPlot::curveTitle(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
	return c ? c->title() : QString::null;
}

/*!
  \return an array containing the keys of all curves
*/
QArray<long> QwtPlot::curveKeys() const
{
    int i;
    QIntDictIterator<QwtPlotCurve> ic(*d_curves);
    QArray<long> rv(d_curves->count());

    ic.toFirst();
    i = 0;
    while (ic.current() && (uint(i) < rv.size()))
    {
        rv[i] = ic.currentKey();
        ++ic;
        ++i;
    }   
    return rv;
}

/*!
  \brief Return the index of the x axis to which a curve is mapped
  \param key Key of the curve
  \return x axis of the curve or -1 if the key is invalid.
*/
int QwtPlot::curveXAxis(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
	return c ? c->xAxis() : -1;
}


/*!
  \brief the index of the y axis to which a curve is mapped
  \param key Key of the curve
  \return y axis of the curve or -1 if the key is invalid.
*/
int QwtPlot::curveYAxis(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
	return c ? c->yAxis() : -1;
}


/*!
  \brief Generate a unique key for a new curve
  \return new unique key or 0 if no key could be found.
*/
long QwtPlot::newCurveKey()
{
    long newkey = d_curves->count() + 1;

    if (newkey > 1)                     // size > 0: check if key exists
    {
        if (d_curves->find(newkey))     // key size+1 exists => there must be a
                                        // free key <= size
        {
            // find the first available key <= size
            newkey = 1;
            while (newkey <= long(d_curves->count()))
            {
                if (d_curves->find(newkey))
                   newkey++;
                else
                   break;
            }

            // This can't happen. Just paranoia.
            if (newkey > long(d_curves->count()))
            {
                while (!d_curves->find(newkey))
                {
                    newkey++;
                    if (newkey > 10000) // prevent infinite loop
                    {
                        newkey = 0;
                        break;
                    }
                }
            }
        }
    }
    return newkey;
}

/*!
  \brief Insert a curve
  \param key unique key
  \param curve Curve
  \return The key of the new curve or 0 if no new key could be found
          or if no new curve could be allocated.
*/

long QwtPlot::insertCurve(QwtPlotCurve *curve)
{
    if (curve == 0)
        return 0;

    long key = newCurveKey();
    if (key == 0)
        return 0;

    curve->reparent(this);

    d_curves->insert(key, curve);
#ifndef QWT_NO_LEGEND
    if (d_autoLegend)
    {
        d_legend->appendItem(curve->title(), curve->symbol(),
                            curve->pen(), key);
		updateLayout();
    }
#endif

    return key;
}

/*!
  \brief Insert a new curve and return a unique key
  \param title title of the new curve
  \param xAxis x axis to be attached. Defaults to xBottom.
  \param yAxis y axis to be attached. Defaults to yLeft.
  \return The key of the new curve or 0 if no new key could be found
          or if no new curve could be allocated.
*/
long QwtPlot::insertCurve(const QString &title, int xAxis, int yAxis)
{
    QwtPlotCurve *curve = new QwtPlotCurve(this);
    if (!curve)
        return 0;

    curve->setAxis(xAxis, yAxis);
    curve->setTitle(title);

    long key = insertCurve(curve);
    if ( key == 0 )
        delete curve;

    return key;
}

/*!
  \brief Find and return an existing curve.
  \param key Key of the curve
  \return The curve for the given key or 0 if key is not valid.
*/
QwtPlotCurve *QwtPlot::curve(long key)
{
    return d_curves->find(key);
}

/*!
  \brief Find and return an existing curve.
  \param key Key of the curve
  \return The curve for the given key or 0 if key is not valid.
*/
const QwtPlotCurve *QwtPlot::curve(long key) const
{
    return d_curves->find(key);
}


/*!
  \brief remove the curve indexed by key
  \param key Key of the curve
*/
bool QwtPlot::removeCurve(long key)
{
    bool ok = d_curves->remove(key);
	if ( !ok )
		return FALSE;

#ifndef QWT_NO_LEGEND
	uint index = d_legend->findFirstKey(key);
	if ( index < d_legend->itemCnt() )
	{
		d_legend->removeItem(index);
		updateLayout();
	}
#endif
	
	autoRefresh();
	return TRUE;
}

/*!
  \brief Assign a pen to a curve indexed by key
  \param key Key of the curve
  \param pen new pen
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurvePen(long key, const QPen &pen)
{
    QwtPlotCurve *c = d_curves->find(key);
	if ( !c )
		return FALSE;
    
	c->setPen(pen);

#ifndef QWT_NO_LEGEND
	uint index = d_legend->findFirstKey(key);
	if ( index < d_legend->itemCnt() )
		d_legend->setPen(index, pen);
#endif

    return TRUE;
}

/*!
  \brief Assign a symbol to a curve indexed by key
  \param key Key of the curve
  \param s
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveSymbol(long key, const QwtSymbol &s)
{
    QwtPlotCurve *c = d_curves->find(key);
	if ( !c )
		return FALSE;

	c->setSymbol(s);

#ifndef QWT_NO_LEGEND
	uint index = d_legend->findFirstKey(key);
	if ( index < d_legend->itemCnt() )
	{
		d_legend->setSymbol(index, s);
		updateLayout();
	}
#endif

    return TRUE;
}

/*!
  \brief Attach a curve to the specified arrays without copying

  This function does not copy the values of the specified arrays.
  Removing the curve will not delete the data.

  \param key Key of the curve
  \param xdat
  \param ydat Pointers to x and y data
  \param size size of ydat and ydat
  \return \c TRUE if the curve exists
  \warning Don't delete dynamically allocated data while a curve
           is attached to them.
  \sa setCurveData(), QwtCurve::setRawData
*/
bool QwtPlot::setCurveRawData(long key, double *xdat, double *ydat, int size)
{
    QwtPlotCurve *c = d_curves->find(key);
	if ( !c )
		return FALSE;

    c->setRawData(xdat, ydat, size);
    return TRUE;
}

/*!
  \param key
  \param s
  \todo Documentation
*/
bool QwtPlot::setCurveTitle(long key, const QString &s)
{
    QwtPlotCurve *c = d_curves->find(key);
	if ( !c )
		return FALSE;

	c->setTitle(s);

#ifndef QWT_NO_LEGEND
	uint index = d_legend->findFirstKey(key);
	if ( index < d_legend->itemCnt() )
	{
		d_legend->setText(index, s);
		updateLayout();
	}
#endif

    return TRUE;
}

/*!
  \brief Assign x and y values to a curve.
  In opposite to @QwtPlot::setCurveRawData@, this function makes
  a 'deep copy' of the data.

  \param key curve key
  \param xdat pointer to x values
  \param ydat pointer to y values
  \param size size of xdat and ydat
  \sa setCurveRawData(), QwtCurve::setData
*/
bool QwtPlot::setCurveData(long key, double *xdat, double *ydat, int size)
{
    QwtPlotCurve *c = d_curves->find(key);
	if ( !c )
		return FALSE;

    c->setData(xdat, ydat, size);
    return TRUE;
}
    
/*!
  \brief Change a curve's style
  \param key Key of the curve
  \param s display style of the curve
  \param options style options
  \return \c TRUE if the curve exists
  \sa QwtCurve::setStyle() for a detailed description of valid styles.
*/
bool QwtPlot::setCurveStyle(long key, int s, int options)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

	c->setStyle(s, options);
    return TRUE;
}

/*!
  \brief Set the style options of a curve indexed by key
  \param key The curve's key
  \param opt The type of the spline
  \return \c TRUE if the specified curve exists.
  \sa QwtCurve::setOptions for a detailed description of valid options.
*/
bool QwtPlot::setCurveOptions(long key, int opt)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setOptions(opt);
    return TRUE;
}

/*!
  \brief Set the number of interpolated points of a curve indexed by key
  \param key key of the curve
  \param s size of the spline
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveSplineSize(long key, int s)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setSplineSize(s);
    return TRUE;
}


/*!
  \brief Attach a curve to an x axis
  \param key key of the curve
  \param axis x axis to be attached
*/
bool QwtPlot::setCurveXAxis(long key, int axis)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setXAxis(axis);
    return TRUE;
}

/*!
  \brief Attach a curve to an y axis
  \param key key of the curve
  \param axis y axis to be attached
*/
bool QwtPlot::setCurveYAxis(long key, int axis)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setYAxis(axis);
    return TRUE;
}


/*!
  \brief Set the baseline for a specified curve

  The baseline is needed for the curve style QwtCurve::Sticks,
  \param key curve key
  \param ref baseline offset from zero
  \sa QwtCurve::setBaseline
*/
bool QwtPlot::setCurveBaseline(long key, double ref)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setBaseline(ref);
    return TRUE;
}

/*!
  \brief Return the baseline offset for a specified curve
  \param key curve key
  \return Baseline offset of the specified curve,
          or 0 if the curve doesn't exist
  \sa setCurveBaseline()
*/
double QwtPlot::curveBaseline(long key) const
{
    double rv = 0.0;
    QwtPlotCurve *c;
    if ((c = d_curves->find(key)))
        rv = c->baseline();
    return rv;
}
