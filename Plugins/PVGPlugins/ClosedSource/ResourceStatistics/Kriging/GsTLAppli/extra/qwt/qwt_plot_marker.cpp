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

#include "qwt_plot.h"
#include "qwt_plot_dict.h"
#include "qwt_math.h"

//------------------------------------------------------------
//.C    Accessing and Manipulating Markers
//------------------------------------------------------------

/*!
  \brief Find the marker which is closest to a given point.
  \param xpos
  \param ypos coordinates of a point in the plotting region
  \retval dist Distance in points between the marker and the specified point.
  \return Key of the closest marker or 0 if no marker was found
*/
long QwtPlot::closestMarker(int xpos, int ypos, int &dist) const
{
    QwtDiMap map[axisCnt];
    for ( int axis = 0; axis < axisCnt; axis++ )
        map[axis] = canvasMap(axis);

    long rv = 0;
    double dmin = 1.0e10;
    
    QIntDictIterator<QwtPlotMarker> itm(*d_markers);
    for (QwtPlotMarker *m = itm.toFirst(); m != 0; m = ++itm )
    {
        double cx = map[m->xAxis()].xTransform(m->xValue());
        double cy = map[m->yAxis()].xTransform(m->yValue());

        if (m->lineStyle() == QwtMarker::HLine)
        {
            if (m->symbol().style() == QwtSymbol::None)
               cx = double(xpos);
        }
        else if (m->lineStyle() == QwtMarker::VLine)
        {
            if (m->symbol().style() == QwtSymbol::None)
               cy = double(ypos);
        }
        
        double f = qwtSqr(cx - double(xpos)) + qwtSqr(cy - double(ypos));
        if (f < dmin)
        {
            dmin = f;
            rv = itm.currentKey();
        }
    }

    dist = int(sqrt(dmin));
    return rv;
}

//! Generate a key for a new marker
long QwtPlot::newMarkerKey()
{
    long newkey = d_markers->count() + 1;

    if (newkey > 1)                     // count > 0
    {
        if (d_markers->find(newkey))    // key count+1 exists => there must be a
                                        // free key <= count
        {
            // find the first available key <= count
            newkey = 1;
            while (newkey <= long(d_markers->count()))
            {
                if (d_markers->find(newkey))
                   newkey++;
                else
                   break;
            }

            // This can't happen. Just paranoia.
            if (newkey > long(d_markers->count()))
            {
                while (!d_markers->find(newkey))
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
  This function is a shortcut to insert a horizontal or vertical
  line marker, dependent on the specified axis.
  \param label Label
  \param axis Axis to be attached
  \return New key if the marker could be inserted, 0 if not.
*/
long QwtPlot::insertLineMarker(const QString &label, int axis)
{
    QwtMarker::LineStyle lineStyle = QwtMarker::NoLine;
    int xAxis = QwtPlot::xBottom;
    int yAxis = QwtPlot::yLeft;

    switch(axis)
    {
        case yLeft:
        case yRight:
            yAxis = axis;
            lineStyle = QwtMarker::HLine;
            break;
        case xTop:
        case xBottom:
            xAxis = axis;
            lineStyle = QwtMarker::VLine;
            break;
    }

    QwtPlotMarker *marker = new QwtPlotMarker(this);
    if ( marker == 0 )
        return 0;

    marker->setAxis(xAxis, yAxis);
    marker->setLabel(label);
    marker->setLineStyle(lineStyle);
    marker->setLabelAlignment(AlignRight|AlignTop);

    long key = insertMarker(marker);
    if ( key == 0 )
        delete marker;

    return key;
}

/*!
  \brief Insert a new marker
  \param label Label
  \param xAxis X axis to be attached
  \param yAxis Y axis to be attached
  \return New key if the marker could be inserted, 0 if not.
*/
long QwtPlot::insertMarker(const QString &label, int xAxis, int yAxis)
{
    QwtPlotMarker *marker = new QwtPlotMarker(this);
    if ( marker == 0 )
        return 0;

    marker->setAxis(xAxis, yAxis);
    marker->setLabel(label);

    long key = insertMarker(marker);
    if ( key == 0 )
        delete marker;

    return key;
}

/*!
  \brief Insert a new marker
  \param marker Marker
  \return New key if the marker could be inserted, 0 if not.
*/
long QwtPlot::insertMarker(QwtPlotMarker *marker)
{
    if ( marker == 0 )
        return 0;

    long key = newMarkerKey();
    if ( key == 0 )
        return 0;

    marker->reparent(this);
    d_markers->insert(key, marker);

    autoRefresh();

    return key;
}

/*!
  \brief Find and return an existing marker.
  \param key Key of the marker
  \return The marker for the given key or 0 if key is not valid.
*/

QwtPlotMarker *QwtPlot::marker(long key)
{
    return d_markers->find(key);
}

/*!
  \brief Find and return an existing marker.
  \param key Key of the marker
  \return The marker for the given key or 0 if key is not valid.
*/

const QwtPlotMarker *QwtPlot::marker(long key) const
{
    return d_markers->find(key);
}

/*!
  \return an array containing the keys of all markers
*/
QArray<long> QwtPlot::markerKeys() const
{

    int i;
    QIntDictIterator<QwtPlotMarker> im(*d_markers);
    QArray<long> rv(d_markers->count());

    im.toFirst();
    i = 0;
    while (im.current() && (uint(i) < rv.size()))
    {
        rv[i] = im.currentKey();
        ++im;
        ++i;
    }   
    return rv;
}

/*!
  \return the font of a marker
*/
QFont QwtPlot::markerFont(long key) const
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
        return m->font();
    else
        return QFont();
}

/*!
  \return a marker's label
  \param key Marker key
*/
const QString& QwtPlot::markerLabel(long key) const
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
        return m->label();
    else
        return QString::null;
}

/*!
  \return a marker's label alignment
  \param key Marker key
*/
int QwtPlot::markerLabelAlign(long key) const
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
        return m->labelAlignment();
    else
        return 0;
}

/*!
  \return the pen of a marker's label
  \param key Marker key
*/
QPen QwtPlot::markerLabelPen(long key) const
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
        return m->labelPen();
    else
        return QPen();
    
}

/*!
  \return a marker's line pen
  \param key Marker key
*/
QPen QwtPlot::markerLinePen(long key) const 
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
        return m->linePen();
    else
        return QPen();
    
}

/*!
  \return a marker's line style
  \param key Marker key
*/
QwtMarker::LineStyle QwtPlot::markerLineStyle(long key) const
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
        return m->lineStyle();
    else
        return QwtMarker::NoLine;
}

/*!
  \brief Get the position of a marker
  \param key Marker key
  \retval mx
  \retval my Marker position 
*/

void QwtPlot::markerPos(long key, double &mx, double &my ) const
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
    {
        mx = m->xValue();
        my = m->yValue();
    }
    else
    {
        mx = 0;
        my = 0;
    }
}

/*!
  \return a marker's symbol
  \param key Marker key
*/
QwtSymbol QwtPlot::markerSymbol(long key) const
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
        return m->symbol();
    else
        return QwtSymbol();
}


/*!
  \return the x axis to which a marker is attached
  \param key Marker key
*/
int QwtPlot::markerXAxis(long key) const
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
        return m->xAxis();
    else
        return -1;
    
}


/*!
  \return the y axis to which a marker is attached
  \param key Marker key
*/
int QwtPlot::markerYAxis(long key) const
{
    QwtPlotMarker *m = d_markers->find(key);
    if (m)
        return m->yAxis();
    else
        return -1;
    
}

/*!
  \brief Remove the marker indexed by key
  \param key unique key
*/
bool QwtPlot::removeMarker(long key)
{
    if (d_markers->remove(key))
    {
        autoRefresh();
        return TRUE;
    }
    else
       return FALSE;
}


/*!
  \brief Attach the marker to an x axis
  \return \c TRUE if the specified marker exists.
*/
bool QwtPlot::setMarkerXAxis(long key, int axis)
{
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setXAxis(axis);
        return TRUE;
    }
    else
       return FALSE;
}

/*!
  \brief Attach the marker to a Y axis
  \param key Marker key
  \param axis Axis to be attached
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerYAxis(long key, int axis)
{
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setYAxis(axis);
        return TRUE;
    }
    else
       return FALSE;
}

/*!
  \brief Specify a font for a marker's label
  \param key Marker key
  \param f New font
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerFont(long key, const QFont &f)
{
    int rv = FALSE;
    
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setFont(f);
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Specify a pen for a marker's line
  \param key Marker key
  \param p New pen
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerLinePen(long key, const QPen &p)
{
    int rv = FALSE;
    
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setLinePen(p);
        rv = TRUE;
    }
    return rv;

}


/*!
  \brief Specify the line style for a marker
  \param key Marker key
  \param st Line style: <code>QwtMarker::HLine, QwtMarker::VLine,
                        QwtMarker::NoLine</code> or a combination of them.
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerLineStyle(long key, QwtMarker::LineStyle st)
{
    int rv = FALSE;
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setLineStyle(st);
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Specify a pen for a marker's label.
  \param key Marker key
  \param p New pen
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerPen(long key, const QPen &p)
{
    int rv = FALSE;
    
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setLinePen(p);
        m->setLabelPen(p);
        rv = TRUE;
    }
    return rv;
}


/*!
  \brief Change the position of a marker
  \param key Marker key
  \param xval
  \param yval Position of the marker in axis coordinates.
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerPos(long key, double xval, double yval)
{
    int rv = FALSE;
    
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setXValue(xval);
        m->setYValue(yval);
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Specify the X position of a marker
  \param key Marker key
  \param val X position of the marker
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerXPos(long key, double val)
{
    int rv = FALSE;
    
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setXValue(val);
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Specify the Y position of the marker
  \param key Marker key
  \param val Y position of the marker
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerYPos(long key, double val)
{
    int rv = FALSE;
    
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setYValue(val);
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Assign a symbol to a specified marker
  \param key Marker key
  \param s new symbol
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerSymbol(long key, const QwtSymbol &s)
{
    int rv = FALSE;
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setSymbol(s);
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Assign a label to a marker
  \param key Marker key
  \param txt Label text
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerLabel(long key, const QString &txt)
{
    int rv = FALSE;
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setLabel(txt);
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Specify the alignment of a marker's label

  The alignment determines the position of the label relative to the
  marker's position. The default setting is AlignCenter.
  \param key Marker key
  \param align Alignment: AlignLeft, AlignRight, AlignTop, AlignBottom,
                          AlignHCenter, AlignVCenter, AlignCenter
                          or a combination of them.
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerLabelAlign(long key, int align)
{
    int rv = FALSE;
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setLabelAlignment(align);
        rv = TRUE;
    }
    return rv;
}

/*!
  \brief Specify a pen for a marker's label
  \param key Marker key
  \param p Label pen
  \return \c TRUE if the specified marker exists
*/
bool QwtPlot::setMarkerLabelPen(long key, const QPen &p)
{
    int rv = FALSE;
    QwtPlotMarker *m;
    if ((m = d_markers->find(key)))
    {
        m->setLabelPen(p);
        rv = TRUE;
    }
    return rv;
}




