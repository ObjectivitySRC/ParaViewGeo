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
#include "qwt_scale.h"
#include "qwt_math.h"

//------------------------------------------------------------
//.C Members for Accessing and Manipulating Axes
//------------------------------------------------------------

/*!
  \return \c TRUE if autoscaling is enabled
  \param axis axis index
*/
bool QwtPlot::axisAutoScale(int axis) const
{
    if (axisValid(axis))
        return d_as[axis].autoScale();
    else
        return FALSE;
    
}

/*!
  \return \c TRUE if a specified axis is enabled
  \param axis axis index
*/
bool QwtPlot::axisEnabled(int axis) const
{
    if (axisValid(axis))
        return d_axisEnabled[axis];
    else
        return FALSE;
}

/*!
  \return the font of the scale labels for a specified axis
  \param axis axis index
*/
QFont QwtPlot::axisFont(int axis) const
{
    if (axisValid(axis))
        return d_scale[axis]->font();
    else
        return QFont();
    
}

/*!
  \brief Determine the scale margins for a specified axis
  \param axis axis index
  \retval mlo Margin at the lower limit
  \retval mhi Margin at the upper limit
  \sa QwtAutoScale::margins()
*/
bool QwtPlot::axisMargins(int axis, double &mlo, double &mhi) const
{
    int rv = FALSE;
    
    if (axisValid(axis))
    {
        mlo = d_as[axis].loMargin();
        mhi = d_as[axis].hiMargin();
        rv = TRUE;
    }
    
    return rv;
}

/*!
  \return the maximum number of major ticks for a specified axis
  \param axis axis index
*/
int QwtPlot::axisMaxMajor(int axis) const
{
    if (axisValid(axis))
        return d_as[axis].maxMajor();
    else
        return 0;
}

/*!
  \return the maximum number of minor ticks for a specified axis
  \param axis axis index
*/
int QwtPlot::axisMaxMinor(int axis) const
{
    if (axisValid(axis))
        return d_as[axis].maxMinor();
    else
        return 0;
}

/*!
  \return the autoscaling options of a specified axis
  \param axis axis index
  \sa QwtAutoScale::options
*/
int QwtPlot::axisOptions(int axis) const
{
    if (axisValid(axis))
        return d_as[axis].options();
    else
        return 0;
}

/*!
  The reference value is needed if the autoscaling options
  QwtAutoScale::Symmetric or QwtAutoScale::IncludeRef are
  set.
  \return the reference value of an axis
  \param axis axis index
  \sa QwtAutoScale, QwtPlot::setAxisOptions
*/
double QwtPlot::axisReference(int axis) const
{
    if (axisValid(axis))
        return d_as[axis].reference();
    else
        return 0.0;
}

/*!
  \returns the scale division of a specified axis
  \param axis axis index
  \sa QwtScaleDiv
*/
const QwtScaleDiv *QwtPlot::axisScale(int axis) const
{
    if (!axisValid(axis))
        return NULL;

    return &d_as[axis].scaleDiv();
}

/*!
  \returns the scale draw of a specified axis
  \param axis axis index
  \return specified scaleDraw for axis, or NULL if axis is invalid.
  \sa QwtScaleDraw
*/
const QwtScaleDraw *QwtPlot::axisScaleDraw(int axis) const
{
    if (!axisValid(axis))
		return NULL;

	return d_scale[axis]->scaleDraw();
}

/*!
  \return the number format for the major scale labels of a specified axis
  \param axis axis index
  \retval f format character
  \retval prec precision
  \retval fieldwidth minimum fieldwidth
  \sa QString::sprintf in the Qt manual
*/
void QwtPlot::axisLabelFormat(int axis, char &f, 
    int &prec, int &fieldwidth) const
{
    if (axisValid(axis))
        d_scale[axis]->labelFormat(f, prec, fieldwidth);
    else
    {
                // to do something useful for a bad call we
                // return the default values of QwtScaleDraw
        f = 'g'; 
        prec = 4;
        fieldwidth = 0;
    }
}

/*!
  \return the title of a specified axis
  \param axis axis index
*/
const QString& QwtPlot::axisTitle(int axis) const
{
    if (axisValid(axis))
        return d_scale[axis]->title();
    else
        return QString::null;
}

/*!
  \return the title font of a specified axis
  \param axis axis index
*/
QFont QwtPlot::axisTitleFont(int axis) const
{
    if (axisValid(axis))
        return d_scale[axis]->titleFont();
    else
        return QFont();
}

/*!
  \return the title alignment of a specified axis
  \param axis axis index
  \sa QwtPlot::setAxisTitleAlignment
*/
int QwtPlot::axisTitleAlignment(int axis) const
{
    if (axisValid(axis))
        return d_scale[axis]->titleAlignment();
    else
        return 0;
}

/*!
  \brief Change specified autoscaling options of an axis
  \param axis axis index
  \param opt Set of or'ed options
  \param value Value to be assigned to all the specified options (TRUE or FALSE)
  \sa QwtAutoScale::changeOptions
*/
void QwtPlot::changeAxisOptions(int axis, int opt, bool value)
{
    if (axisValid(axis))
    {
        d_as[axis].changeOptions(opt, value);
        autoRefresh();
    }
}


/*!
  \brief Enable or disable a specified axis

  When an axis is disabled, this only means that it is not
  visible on the screen. Curves, markers and can be attached
  to disabled axes, and transformation of screen coordinates
  into values works as normal.

  Only xBottom and yLeft are enabled by default.
  \param axis axis index
  \param tf \c TRUE (enabled) or \c FALSE (disabled)
*/
void QwtPlot::enableAxis(int axis, bool tf)
{
    if (axisValid(axis))
    {
        d_axisEnabled[axis] = tf;
		updateLayout();
    }
}

/*!
  Transform the x or y coordinate of a position in the
  drawing region into a value.
  \param axis axis index
  \param pos position
  \warning The position can be an x or a y coordinate,
           depending on the specified axis.
*/
double QwtPlot::invTransform(int axis, int pos) const
{
    if (axisValid(axis))
       return(canvasMap(axis).invTransform(pos));
    else
       return 0.0;
}


/*!
  \brief Transform a value into a coordinate in the plotting region
  \param axis axis index
  \param value value
  \return X or y coordinate in the plotting region corresponding
          to the value.
*/
int QwtPlot::transform(int axis, double value) const
{
    if (axisValid(axis))
       return(canvasMap(axis).transform(value));
    else
       return 0;
    
}

/*!
  \brief Change the font of an axis
  \param axis axis index
  \param f font
  \warning This function changes the font of the tick labels,
           not of the axis title.
*/
void QwtPlot::setAxisFont(int axis, const QFont &f)
{
    if (axisValid(axis))
        d_scale[axis]->setFont(f);
}

/*!
  \brief Assign margins to a specified axis
  \param axis axis index
  \param mlo margin at the lower end of the scale
  \param mhi margin at the upper end of the scale
  \warning If the scale is logarithmic, the margins are measured in
           decades
  \sa QwtAutoScale::setMargins
*/
void QwtPlot::setAxisMargins(int axis, double mlo, double mhi)
{
    if (axisValid(axis))
    {
        d_as[axis].setMargins(mlo, mhi);
        autoRefresh();
    }
}

/*!
  \brief Reset scale options and set specified options for a specified axis
  \param axis axis index
  \param opt set of options
  \sa QwtAutoScale::setOptions
*/
void QwtPlot::setAxisOptions(int axis, int opt)
{
    if (axisValid(axis))
    {
        d_as[axis].setOptions(opt);
        autoRefresh();
    }
}


/*!
  \brief Enable autoscaling for a specified axis

  This member function is used to switch back to autoscaling mode
  after a fixed scale has been set. Autoscaling is enabled by default.

  \param axis axis index
  \sa QwtPlot::setAxisScale()
*/
void QwtPlot::setAxisAutoScale(int axis)
{
    if (axisValid(axis))
    {
        d_as[axis].setAutoScale();
        autoRefresh();
    }
}

void QwtPlot::setAxisAutoScale( int axis, bool on )
{
    if (axisValid(axis))
    {
        d_as[axis].setAutoScale( on );
        autoRefresh();
    }
}

/*!
  \brief Disable autoscaling and specify a fixed scale for a selected axis.
  \param axis axis index
  \param min
  \param max minimum and maximum of the scale
  \param step Major step size. If <code>step == 0</code>, the step size is
            calculated automatically using the maxMajor setting.
  \sa QwtPlot::setAxisMaxMajor()
*/
void QwtPlot::setAxisScale(int axis, double min, double max, double step)
{
    if (axisValid(axis))
    {
        d_as[axis].setScale(min,max,step);
        autoRefresh();
    }
    
}

/*!
  \brief Set a scale draw
  \param axis axis index
  \param scaleDraw object responsible for drawing scales.

  By passing scaleDraw it is possible to extend QwtScaleDraw
  functionality and let it take place in QwtPlot. Please note
  that scaleDraw has to be created with new and will be deleted
  by the corresponding QwtScale member ( like a child object ).

  \sa QwtScaleDraw, QwtScale
*/

void QwtPlot::setAxisScaleDraw(int axis, QwtScaleDraw *scaleDraw)
{
    if (axisValid(axis))
		d_scale[axis]->setScaleDraw(scaleDraw);
}

/*!
  \brief Change the number format for the major scale of a selected axis
  \param axis axis index
  \param f format
  \param prec precision
  \param fieldwidth minimum fieldwidth
  \sa QString::sprintf in the Qt manual
*/
void QwtPlot::setAxisLabelFormat(int axis, char f, int prec, int fieldwidth)
{
    if (axisValid(axis))
        d_scale[axis]->setLabelFormat(f, prec, fieldwidth);
}

/*!
  \brief Change the title font of a selected axis
  \param axis axis index
  \param f font
*/
void QwtPlot::setAxisTitleFont(int axis, const QFont &f)
{
    if (axisValid(axis))
        d_scale[axis]->setTitleFont(f);
}

/*!
  \brief Change the title alignment of a selected axis
  \param axis axis index
  \param align or'd Qt::Alignment flags.
  \sa QwtScale::setTitleAlignment 
*/
void QwtPlot::setAxisTitleAlignment(int axis, int align)
{
    if (axisValid(axis))
        d_scale[axis]->setTitleAlignment(align);
}


/*!
  \brief Set the maximum number of minor scale intervals for a specified axis
  \param axis axis index
  \param maxMinor maximum number of minor steps
  \sa QwtAutoScale::setMaxMinor
*/
void QwtPlot::setAxisMaxMinor(int axis, int maxMinor)
{
    if (axisValid(axis))
    {
        d_as[axis].setMaxMinor(maxMinor);
        autoRefresh();
    }
}

/*!
  \brief Set the maximum number of major scale intervals for a specified axis
  \param axis axis index
  \param maxMajor maximum number of major steps
  \sa QwtAutoScale::setMaxMajor
*/
void QwtPlot::setAxisMaxMajor(int axis, int maxMajor)
{
    if (axisValid(axis))
    {
        d_as[axis].setMaxMajor(maxMajor);
        autoRefresh();
    }
}

/*!
  \brief Set a reference value for a specified axis

  The reference value is used by some autoscaling modes.
  \param axis axis index
  \param value reference value
  \sa QwtAutoScale::setReference, QwtPlot::setAxisOptions()
*/
void QwtPlot::setAxisReference(int axis, double value)
{
    if (axisValid(axis))
    {
        d_as[axis].setReference(value);
        autoRefresh();
    }
}

/*!
  \brief Change the title of a specified axis
  \param axis axis index
  \param t axis title
*/
void QwtPlot::setAxisTitle(int axis, const QString &t)
{
    if (axisValid(axis))
        d_scale[axis]->setTitle(t);
}
