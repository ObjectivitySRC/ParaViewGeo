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
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_math.h"


//! Sets a cross cursor, and an invisible red outline

QwtPlotCanvas::QwtPlotCanvas(QwtPlot *plot):
    QFrame(plot, "canvas", WRepaintNoErase|WResizeNoErase),
    d_outlineEnabled(FALSE),
    d_outlineActive(FALSE),
    d_mousePressed(FALSE),
    d_outline(Qwt::Rect),
    d_pen(red)
{
    setCursor(crossCursor);
}

//! Requires layout updates of the parent plot
void QwtPlotCanvas::frameChanged()
{
    QFrame::frameChanged();

    // frame changes change the size of the contents rect, what
    // is related to the axes. So we have to update the layout.

    ((QwtPlot *)parent())->updateLayout();
}

//! Redraw the canvas
void QwtPlotCanvas::drawContents(QPainter *painter)
{
    QRect ur = painter->clipRegion().boundingRect();
    ((QwtPlot *)parent())->drawCanvas(ur);

    if ( d_outlineActive )
    {
        painter->save();
        drawOutline(*painter); // redraw outline
        painter->restore();
    }
}

//! mousePressEvent
void QwtPlotCanvas::mousePressEvent(QMouseEvent *e)
{
    const QRect &r = contentsRect();
    QMouseEvent m(QEvent::MouseButtonDblClick, e->pos() - r.topLeft(),
                  e->button(), e->state());

    if (d_outlineActive)
    {
        QPainter p(this);
        drawOutline(p); // Delete active outlines
    }

    d_outlineActive = FALSE;

    //
    // store this point as entry point
    //
    d_lastPoint = e->pos();
    d_entryPoint = e->pos();

    if (d_outlineEnabled)
    {
        QPainter p(this);
        drawOutline(p); // draw new outline
        d_outlineActive = TRUE;
    }

    d_mousePressed = TRUE;

    emit mousePressed(m);
}

//! mouseReleaseEvent
void QwtPlotCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    const QRect &r = contentsRect();
    QMouseEvent m(QEvent::MouseButtonDblClick, e->pos() - r.topLeft(),
                  e->button(), e->state());


    if (d_outlineActive)
    {
        QPainter p(this);
        drawOutline(p);
    }

    d_outlineActive = FALSE;
    d_mousePressed = FALSE;

    emit mouseReleased(m);
}

//! mouseMoveEvent
void QwtPlotCanvas::mouseMoveEvent(QMouseEvent *e)
{
    const QRect &r = contentsRect();
    QMouseEvent m(QEvent::MouseButtonDblClick, e->pos() - r.topLeft(),
                  e->button(), e->state());

    if (d_outlineActive)
    {
        QPainter p(this);
        drawOutline(p);
        d_lastPoint = e->pos();
        drawOutline(p);
    }

    emit mouseMoved(m);
}

/*!
  \brief Enables or disables outline drawing.
  When the outline feature is enabled, a shape will be drawn
  in the plotting region  when the user presses
  or drags the mouse. It can be used to implement crosshairs,
  mark a selected region, etc.
  \param tf \c TRUE (enabled) or \c FALSE (disabled)
  \warning An outline style has to be specified.
  \sa QwtPlotCanvas::setOutlineStyle()
*/

void QwtPlotCanvas::enableOutline(bool tf)
{

    //
    //  If the mouse is pressed, erase existing outline
    //  or draw new outline if 'tf' changes the 'enabled' state.
    //
    if ((tf != d_outlineEnabled) && d_mousePressed)
    {
        QPainter p(this);
        drawOutline(p);
        d_outlineActive = tf;
    }
    d_outlineEnabled = tf;
}

/*!
  \return \c TRUE if the outline feature is enabled
  \sa QwtPlotCanvas::enableOutline
*/

bool QwtPlotCanvas::outlineEnabled() const 
{ 
    return d_outlineEnabled; 
}

/*!
  \brief Specify the style of the outline

  The outline style determines which kind of shape is drawn
  in the plotting region when the user presses a mouse button
  or drags the mouse. Valid Styles are:
  \param os Outline Style. Valid values are: \c Qwt::HLine, \c Qwt::VLine,
            \c Qwt::Cross, \c Qwt::Rect, \c Qwt::Ellipse
  <dl>
  <dt>Qwt::Cros
  <dd>Cross hairs are drawn across the plotting area
      when the user presses a mouse button. The lines
      intersect at the point where the mouse was pressed
      and move with the mouse pointer.
  <dt>Qwt::HLine, Qwt::VLine
  <dd>A horizontal or vertical line appears when
      the user presses a mouse button. This is useful
      for moving line markers.
  <dt>Qwt::Rect
  <dd>A rectangle is displayed when the user drags
      the mouse. One corner is fixed at the point where
      the mouse was pressed, and the opposite corner moves
      with the mouse pointer. This can be used for selecting
      regions.
  <dt>Qwt::Ellipse
  <dd>Similar to Qwt::Rect, but with an ellipse inside
      a bounding rectangle.
  </dl>
  \sa QwtPlotCanvas::enableOutline(), QwtPlotCanvas::outlineStyle()
*/

void QwtPlotCanvas::setOutlineStyle(Qwt::Shape os)
{
    if (d_outlineActive)
    {
        QPainter p(this); // erase old outline
        drawOutline(p);
    }

    d_outline = os;

    if (d_outlineActive)
    {
        QPainter p(this);
        drawOutline(p); // draw new outline
    }
}

/*!
  \return the outline style
  \sa QwtPlotCanvas::setOutlineStyle()
*/
Qwt::Shape QwtPlotCanvas::outlineStyle() const 
{ 
    return d_outline; 
}

/*!
  \brief Specify a pen for the outline
  \param pn new pen
  \sa QwtPlotCanvas::outlinePen
*/

void QwtPlotCanvas::setOutlinePen(const QPen &c)
{
    d_pen = c;
}

/*!
  \return the pen used to draw outlines
  \sa QwtPlotCanvas::setOutlinePen
*/

const QPen& QwtPlotCanvas::outlinePen() const 
{ 
    return d_pen; 
}

/*!
    draw an outline
*/
void QwtPlotCanvas::drawOutline(QPainter &p)
{
    const QRect &r = contentsRect();

    QColor bg = ((QwtPlot *)parent())->canvasBackground();

    QPen pn = d_pen;
    pn.setColor(QColor(0, (bg.pixel() ^ d_pen.color().pixel())));

    p.setPen(pn);
    p.setRasterOp(XorROP);
    p.setClipRect(r);
    p.setClipping(TRUE);

    switch(d_outline)
    {
        case Qwt::VLine:
            p.drawLine(d_lastPoint.x(), r.top(), d_lastPoint.x(), r.bottom());
            break;
        
        case Qwt::HLine:
            p.drawLine(r.left(), d_lastPoint.y(), r.right(), d_lastPoint.y());
            break;
        
        case Qwt::Cross:
            p.drawLine(r.left(), d_lastPoint.y(), r.right(), d_lastPoint.y());
            p.drawLine(d_lastPoint.x(), r.top(), d_lastPoint.x(), r.bottom());
            break;

        case Qwt::Rect:
            p.drawRect(d_entryPoint.x(), d_entryPoint.y(),
               d_lastPoint.x() - d_entryPoint.x() + 1,
               d_lastPoint.y() - d_entryPoint.y() + 1);
            break;
        
        case Qwt::Ellipse:
            p.drawEllipse(d_entryPoint.x(), d_entryPoint.y(),
               d_lastPoint.x() - d_entryPoint.x() + 1,
               d_lastPoint.y() - d_entryPoint.y() + 1);
            break;

        default:
            break;
    }
}
