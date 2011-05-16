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

#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>
#include "qwt_plot.h"
#include "qwt_plot_dict.h"
#include "qwt_rect.h"
#include "qwt_scale.h"
#include "qwt_legend.h"
#include "qwt_plot_canvas.h"
#include "qwt_math.h"

static const int ScrBarWidth = 20; // better use QStyle::scrollBarMetrics

/*!
  \brief Constructor
  \param parent Parent widget
  \param name Widget name
 */

QwtPlot::QwtPlot(QWidget *p, const char *name) :
    QFrame(p, name, WRepaintNoErase|WResizeNoErase),
    d_grid(this)
{
    initPlot();
}


/*!
  \brief Constructor
  \param title Title text
  \param parent Parent widget
  \param name Widget name
 */
QwtPlot::QwtPlot(const QString &title, QWidget *p, const char *name) :
    QFrame(p, name, WRepaintNoErase|WResizeNoErase),
    d_grid(this)
{
    initPlot(title);
}

//! Destructor
QwtPlot::~QwtPlot()
{
    delete d_curves;
    delete d_markers;
}

/*!
  \brief Initializes a QwtPlot instance
  \param title Title text
 */
void QwtPlot::initPlot(const QString &title)
{
    d_margin = 0;
    d_spacing = 5; // space below the title

    d_curves = new QwtCurveDict;
    d_markers = new QwtMarkerDict;

    d_autoReplot = FALSE;

    d_lblTitle = new QLabel(title, this);
    d_lblTitle->setFont(QFont("Helvetica", 14,QFont::Bold));
    d_lblTitle->setAlignment(AlignHCenter|AlignBottom|WordBreak|ExpandTabs);


#ifndef QWT_NO_LEGEND
    d_legend = new QwtLegend(this);
    connect(d_legend, SIGNAL(clicked(int)), SLOT(lgdClicked(int)));
#endif
    d_legendPos = Qwt::Bottom;
    d_autoLegend = FALSE;


    d_scale[yLeft] = new QwtScale(QwtScale::Left, this, "yLeft");
    d_scale[yRight] = new QwtScale(QwtScale::Right, this, "yRight");
    d_scale[xTop] = new QwtScale(QwtScale::Top, this, "xTop");
    d_scale[xBottom] = new QwtScale(QwtScale::Bottom, this, "xBottom");

    initAxes();

    d_grid.setPen(QPen(black, 0, DotLine));
    d_grid.setXDiv(d_sdiv[xBottom]);
    d_grid.setYDiv(d_sdiv[yLeft]);
    d_grid.enableXMin(FALSE);
    d_grid.enableYMin(FALSE);
    d_grid.setAxis(xBottom, yLeft);

    d_canvas = new QwtPlotCanvas(this);
    d_canvas->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    d_canvas->setLineWidth(2);
    d_canvas->setMidLineWidth(0);

    connect(d_canvas, SIGNAL(mousePressed(const QMouseEvent &)),
        this, SIGNAL(plotMousePressed(const QMouseEvent &)));
    connect(d_canvas, SIGNAL(mouseMoved(const QMouseEvent &)),
        this, SIGNAL(plotMouseMoved(const QMouseEvent &)));
    connect(d_canvas, SIGNAL(mouseReleased(const QMouseEvent &)),
        this, SIGNAL(plotMouseReleased(const QMouseEvent &)));
}

//! Initialize axes
void QwtPlot::initAxes()
{
    int i;

    QFont fscl("Helvetica", 10);
    QFont fttl("Helvetica", 12, QFont::Bold);


    for(i=0;i<axisCnt;i++)
    {
        d_scale[i]->setFont(fscl);
        d_scale[i]->setTitleFont(fttl);
    }

    d_axisEnabled[yLeft] = TRUE;
    d_axisEnabled[yRight] = FALSE;
    d_axisEnabled[xBottom] = TRUE;
    d_axisEnabled[xTop] = FALSE;

    for (i=0;i<axisCnt;i++)
    {
        d_as[i].adjust(0.0,1000.0,TRUE);
        d_sdiv[i] = d_as[i].scaleDiv();
        d_scale[i]->setScaleDiv(d_sdiv[i]);
    }
}

/*!
  \brief Adds handling of QEvent::LayoutHint
*/
bool QwtPlot::event(QEvent *e)
{
    bool ok = QFrame::event(e);
    switch(e->type())
    {
#if 0
        case QEvent::ChildInserted:
        case QEvent::ChildRemoved:
#endif
        case QEvent::LayoutHint:
            updateLayout();
            break;
        default:;
    }
    return ok;
}

/*!
  \brief Replots the plot if QwtPlot::autoReplot() is \c TRUE.
*/

void QwtPlot::autoRefresh()
{
    if (d_autoReplot)
        replot();
}

/*!
  \brief Enables or disables outline drawing.
  When the outline feature is enabled, a shape will be drawn
  in the plotting region  when the user presses
  or drags the mouse. It can be used to implement crosshairs,
  mark a selected region, etc.
  \param tf \c TRUE (enabled) or \c FALSE (disabled)
  \warning An outline style has to be specified.
  \sa setOutlineStyle()
*/
void QwtPlot::enableOutline(bool tf)
{
    d_canvas->enableOutline(tf);
}

/*!
  \brief Set or reset the autoReplot option
  If the autoReplot option is set, the plot will be
  updated implicitly by manipulating member functions.
  Since this may be time-consuming, it is recommended
  to leave this option switched off and call replot()
  explicitly if necessary.

  The autoReplot option is set to FALSE by default, which
  means that the user has to call replot() in order to make
  changes visible.
  \param tf \c TRUE or \c FALSE. Defaults to \c TRUE.
  \sa replot()
*/
void QwtPlot::setAutoReplot(bool tf)
{
    d_autoReplot = tf;
}

/*!
    \return TRUE if the autoReplot option is set.
*/
bool QwtPlot::autoReplot() const
{
    return d_autoReplot; 
}

/*!
  \brief Change the plot's title
  \param t new title
*/
void QwtPlot::setTitle(const QString &t)
{
    d_lblTitle->setText(t);
}

/*!
  \return the plot's title
*/

QString QwtPlot::title() const
{
    return d_lblTitle->text();
}


/*!
  \brief Change the title font
  \param f new title font
*/
void QwtPlot::setTitleFont(const QFont &f)
{
    d_lblTitle->setFont(f);
}

/*!
  \return the plot's title font
*/
QFont QwtPlot::titleFont() const
{
    return d_lblTitle->font();
}

/*!
  \return the plot's canvas
*/
QwtPlotCanvas *QwtPlot::canvas()
{ 
    return d_canvas;
}   

/*!
  \return the plot's canvas
*/
const QwtPlotCanvas *QwtPlot::canvas() const
{ 
    return d_canvas;
}

//! \return MinimumExpanding/MinimumExpanding
QSizePolicy QwtPlot::sizePolicy() const
{
    QSizePolicy sp;
    sp.setHorData( QSizePolicy::MinimumExpanding );
    sp.setVerData( QSizePolicy::MinimumExpanding );
    return sp;
}

//!  \return minimumSizeHint
QSize QwtPlot::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \brief Return a minimum size hint
*/
QSize QwtPlot::minimumSizeHint() const
{
    // Compute plot frame margin
    // The contribution may already be included in the scale borders
    int hScaleMargin = 0, vScaleMargin = 0;

    if (d_axisEnabled[xTop])
        hScaleMargin = qwtMax(hScaleMargin, d_scale[xTop]->minBorderDist());
    if (d_axisEnabled[xBottom])
        hScaleMargin = qwtMax(hScaleMargin, d_scale[xBottom]->minBorderDist());
    if (d_axisEnabled[yLeft])
        vScaleMargin = qwtMax(vScaleMargin, d_scale[yLeft]->minBorderDist());
    if (d_axisEnabled[yRight])
        vScaleMargin = qwtMax(vScaleMargin, d_scale[yRight]->minBorderDist());

    int offset = qwtMin( vScaleMargin, hScaleMargin );
    offset = qwtMax( 0, offset - d_canvas->frameWidth() );
    offset *= 2;

    int pfw = d_canvas->frameWidth() + 2;
    hScaleMargin = qwtMax( offset, pfw - hScaleMargin );
    vScaleMargin = qwtMax( offset, pfw - vScaleMargin );

    // Compute scale contributions
    int hScaleW = 0, hScaleH = 0, vScaleW = 0, vScaleH = 0;
    if ( d_axisEnabled[xTop] )
    {
        const QSize sz = d_scale[xTop]->minimumSizeHint();
        hScaleW = qwtMax( hScaleW, sz.width() );
        hScaleH += sz.height();
    }
    if ( d_axisEnabled[xBottom] )
    {
        const QSize sz = d_scale[xBottom]->minimumSizeHint();
        hScaleW = qwtMax( hScaleW, sz.width() );
        hScaleH += sz.height();
    }
    if ( d_axisEnabled[yLeft] )
    {
        const QSize sz = d_scale[yLeft]->minimumSizeHint();
        vScaleW += sz.width();
        vScaleH = qwtMax( vScaleH, sz.height() );
    }
    if ( d_axisEnabled[yRight] )
    {
        const QSize sz = d_scale[yRight]->minimumSizeHint();
        vScaleW += sz.width();
        vScaleH = qwtMax( vScaleH, sz.height() );
    }

    // Add scale contributions
    int w = hScaleMargin + hScaleW + vScaleW;
    int h = vScaleMargin + hScaleH + vScaleH;

    if (!d_lblTitle->text().isEmpty())
    {
        // If only yLeft or yRight is showing, we must center
        // on the plot canvas.

        int titleW = w;
        if ( d_axisEnabled[yLeft] != d_axisEnabled[yRight] )
            titleW -= vScaleW;

        int titleH = d_lblTitle->heightForWidth(titleW);
        if ( titleH > titleW )
        {
            // Compensate for a long title

            w = titleW = titleH;
            if ( d_axisEnabled[yLeft] != d_axisEnabled[yRight] )
                w += vScaleW;

            titleH = d_lblTitle->heightForWidth( titleW );
        }
        h += titleH + d_spacing;
    }

#ifndef QWT_NO_LEGEND

    // Compute the legend contribution

    if ( d_legend->itemCnt() > 0 )
    {
        switch ( d_legendPos )
        {
            case Qwt::Left:
            case Qwt::Right:
            {
                // vertical legends have always one column 

                int legendW = 2 * d_legend->frameWidth() +
                    d_legend->colWidth();

                if ( (int)d_legend->itemCnt() * d_legend->rowHeight() +
                    2 * d_legend->frameWidth() > h )
                {
                    // we need a vertical scrollbar

                    legendW += ScrBarWidth;
                }

                if ( legendW > w / 3 )
                {
                    // The legend is limited to 1/4 of the complete
                    // layout
                    legendW = w / 3;
                }
                w += legendW;
                break;
            }
            case Qwt::Top:
            case Qwt::Bottom:
            default:
            {
                int legendH = 2 * d_legend->frameWidth();

                int numCols = (w - 2 * d_legend->frameWidth()) 
                    / d_legend->colWidth();
                if ( numCols == 0 )
                {
                    // not enough space for one column.
                    // we need a horizontal scrollbar

                    legendH += ScrBarWidth;
                    numCols = 1;
                }

                int numRows = d_legend->itemCnt() / numCols;
                if ( d_legend->itemCnt() % numCols > 0 )
                    numRows++;

                legendH += numRows * d_legend->rowHeight();
                if ( legendH > h / 3 )
                {
                    // The legend is limited to 1/4 of the complete
                    // layout
                    legendH = h / 3;
                }
                h += legendH;
                break;
            }
        }
    }
#endif

    w += 2 * (d_margin + frameWidth());
    h += 2 * (d_margin + frameWidth());

    return QSize( w, h );
}

//! Resize and update internal layout
void QwtPlot::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
    updateLayout();
}

/*!
  \brief Redraw the plot
  If the autoReplot option is not set (which is the default)
  or if any curves are attached to raw data, the plot has to
  be refreshed explicitly in order to make changes visible.
  \sa setAutoReplot()
*/
void QwtPlot::replot()
{
    bool doAutoReplot = autoReplot();
    setAutoReplot(FALSE);

    updateAxes();
    drawCanvas(d_canvas->contentsRect());

    setAutoReplot(doAutoReplot);
}

/*!
  \brief Adjust plot content to its current size.
  \sa QwtPlot::resizeEvent
*/
void QwtPlot::updateLayout()
{
    int hDist, vDist;
    vDist = hDist = d_canvas->frameWidth() + 2;

    if (d_axisEnabled[xTop])
        hDist = qwtMax(hDist, d_scale[xTop]->minBorderDist());
    if (d_axisEnabled[xBottom])
        hDist = qwtMax(hDist, d_scale[xBottom]->minBorderDist());

    if (d_axisEnabled[yLeft])
       vDist = qwtMax(vDist, d_scale[yLeft]->minBorderDist());
    if (d_axisEnabled[yRight])
       vDist = qwtMax(vDist, d_scale[yRight]->minBorderDist());

    QwtRect rPlot = this->contentsRect();
    rPlot.cutMargin(d_margin, d_margin, d_margin, d_margin);

#ifndef QWT_NO_LEGEND
    if (d_legend->itemCnt() > 0)
    {
        switch(d_legendPos)
        {
            case Qwt::Top:
            case Qwt::Bottom:
                d_legend->setMaxCols(
                    (rPlot.width() - ScrBarWidth) / d_legend->colWidth());
                break;
            case Qwt::Right:
            case Qwt::Left:
            default:
                d_legend->setMaxCols(1);
                break;
        }
    }
#endif

    QRect rTitle, rLegend, rPixmap, rAxis[axisCnt];
    findLayout(FALSE, rPlot, hDist, vDist, QwtPlotPrintFilter(), rTitle,
        rLegend, rAxis, rPixmap);

    //
    // resize and show the visible widgets
    //
    if (!d_lblTitle->text().isEmpty())
    {
        d_lblTitle->setGeometry(rTitle);
        if (!d_lblTitle->isVisible())
            d_lblTitle->show();
    }
    else
        d_lblTitle->hide();

    for ( int axis = 0; axis < axisCnt; axis++ )
    {
        if (d_axisEnabled[axis])
        {
            if ( axis == yLeft || axis == yRight )
                d_scale[axis]->setBorderDist(vDist, vDist);
            else
                d_scale[axis]->setBorderDist(hDist, hDist);

            d_scale[axis]->setGeometry(rAxis[axis]);
            if (!d_scale[axis]->isVisible())
                d_scale[axis]->show();
        }
        else
            d_scale[axis]->hide();
    }

#ifndef QWT_NO_LEGEND
    if (d_legend->itemCnt() > 0)
    {
        d_legend->setGeometry(rLegend);
        d_legend->show();
    }
    else
        d_legend->hide();
#endif

    d_canvas->setGeometry(rPixmap);
}

void QwtPlot::findLayout(
#ifndef QWT_NO_LEGEND
    bool printing,
#else
    bool,
#endif
    const QRect &rect, int hDist, int vDist,
    const QwtPlotPrintFilter &pfilter, QRect &rTitle,
    QRect &rLegend, QRect rAxis[axisCnt], QRect &rPixmap) const
{
    rLegend = rTitle = rPixmap = QRect();
    for ( int i = 0; i < axisCnt; i++ )
        rAxis[i] = QRect();

    QwtRect rPlot = rect;

    /////////////
    // rLegend
    /////////////

#ifndef QWT_NO_LEGEND
    if ((pfilter.options() & QwtPlotPrintFilter::PrintLegend)
        && d_legend->itemCnt() > 0)
    {
        int border = 2 * d_legend->frameWidth();
        QSize cell = d_legend->cellSizeHint(pfilter.font(d_legend->font(),
            QwtPlotPrintFilter::Legend));
        QSize hint;
        hint.setWidth(d_legend->numCols() * cell.width() + border);
        hint.setHeight(d_legend->numRows() * cell.height() + border);
        cell.setWidth(cell.width() + border);
        cell.setHeight(cell.height() + border);

        switch(d_legendPos)
        {
            case Qwt::Left:
            {
                int w = qwtMin(hint.width(), rPlot.width() / 2);
                if ( !printing && hint.height() > rPlot.height() )
                    w += ScrBarWidth;
                rLegend = rPlot.cutLeft(w);
                break;
            }
            case Qwt::Right:
            {
                int w = qwtMin(hint.width(), rPlot.width() / 2);
                if ( !printing && hint.height() > rPlot.height() )
                    w += ScrBarWidth;
                rLegend = rPlot.cutRight(w);
                break;
            }
            case Qwt::Top:
            {
                int h = qwtMin(hint.height(), rPlot.height() / 4);
                h = qwtMax(h, cell.height());  // at least one cell high
                rLegend = rPlot.cutTop(h);
                break;
            }
            case Qwt::Bottom:
            default:
            {
                int h = qwtMin(hint.height(), rPlot.height() / 4);
                h = qwtMax(h, cell.height());  // at least one cell high
                rLegend = rPlot.cutBottom(h);
                break;
            }
        }
    }
#endif

    ///////////////////
    // rTitle, rPixmap
    ///////////////////

    int dimTitle = 0;
    int dim[axisCnt];
    int hSpacing = qwtMax( 0, hDist - vDist );
    int vSpacing = qwtMax( 0, vDist - hDist );
    dim[yLeft] = dim[yRight]  = hSpacing;
    dim[xTop]  = dim[xBottom] = vSpacing;

    bool done = FALSE;
    while (!done)
    {
        done = TRUE;

        // the size for the 4 axis depend on each other. Expanding
        // the height of a horizontal axis will shrink the height
        // for the verical axis, shrinking the height of a vertical
        // axis will result in a line break what will expand the
        // width and results in shrinking the width of a horizontal
        // axis what might result in a line break of a horizonatal
        // axis ... . So we loop as long as no size has to be changed.


        for ( int axis = 0; axis < axisCnt; axis++ )
        {
            if ((pfilter.options() & QwtPlotPrintFilter::PrintTitle) &&
                !d_lblTitle->text().isEmpty())
            {
                QFont f = d_lblTitle->font();
                d_lblTitle->setFont(pfilter.font(f, QwtPlotPrintFilter::Title));
                int w = rPlot.width();

                if ( d_axisEnabled[yLeft] != d_axisEnabled[yRight] )
                    w -= dim[yLeft] + dim[yRight]; // centered to the pixmap

                dimTitle = d_lblTitle->heightForWidth(w);
                d_lblTitle->setFont(f);
            }

            if (d_axisEnabled[axis])
            {
                int length;
                if ( axis == xTop || axis == xBottom )
                {
                  length = rPlot.width() - dim[yLeft] - dim[yRight] +
                           2 * hSpacing;
                }
                else
                {
                  length = rPlot.height() - dim[xTop] - dim[xBottom] +
                            2 * vSpacing;
                  if ( dimTitle > 0 )
                      length -= dimTitle + d_spacing;
                }

                const QwtScale *scale = d_scale[axis];

                QFontMetrics titleFm(pfilter.font(scale->titleFont(),
                    QwtPlotPrintFilter::AxisTitle));
                QFontMetrics scaleFm(pfilter.font(scale->font(),
                    QwtPlotPrintFilter::AxisScale));

                int d = scale->dimForLength(length, titleFm, scaleFm);

                if ( !(pfilter.options() &
                    QwtPlotPrintFilter::PrintBackground) )
                {
                    int baseDist = scale->baseLineDist();
                    d -= baseDist;
                }
                if ( d > dim[axis] )
                {
                    dim[axis] = d;
                    done = FALSE;
                }
            }
        }
    }

    if ((pfilter.options() & QwtPlotPrintFilter::PrintTitle) &&
        !d_lblTitle->text().isEmpty())
    {
        rTitle = QRect(rPlot.x(), rPlot.y(),
            rPlot.width(), dimTitle);
        if ( d_axisEnabled[yLeft] != d_axisEnabled[yRight] )
        {
            // if one of the y axes is missing we align
            // the title centered to the pixmap

            rTitle.setX(rPlot.x() + dim[yLeft]);
            rTitle.setWidth(rPlot.width() - dim[yLeft] - dim[yRight]);
        }
    }

    int spacing = 0;
    if ( dimTitle > 0 )
        spacing = d_spacing; // space between titel and canvas

    rPixmap = QRect(
        rPlot.x() + dim[yLeft],
        rPlot.y() + dimTitle + spacing + dim[xTop],
        rPlot.width() - dim[yRight] - dim[yLeft],
        rPlot.height() - dimTitle - spacing - dim[xBottom] - dim[xTop]);

    if (d_scale[xTop])
    {
        rAxis[xTop] = QRect(rPixmap.x() - hSpacing,
                            rPixmap.y() - dim[xTop],
                            rPixmap.width() + 2 * hSpacing,
                            dim[xTop]);

        if ( !(pfilter.options() & QwtPlotPrintFilter::PrintBackground) )
            rAxis[xTop].setBottom(rAxis[xTop].bottom() + 1);
    }

    if (d_scale[xBottom])
    {
        rAxis[xBottom] = QRect(rPixmap.x() - hSpacing,
                               rPixmap.y() + rPixmap.height(),
                               rPixmap.width() + 2 * hSpacing,
                               dim[xBottom]);
    }

    if (d_scale[yLeft])
    {
        rAxis[yLeft] = QRect(rPixmap.x() - dim[yLeft],
                             rPixmap.y() - vSpacing,
                             dim[yLeft],
                             rPixmap.height() + 2 * vSpacing);

        if ( !(pfilter.options() & QwtPlotPrintFilter::PrintBackground) )
            rAxis[yLeft].setRight(rAxis[yLeft].right() + 1);
    }

    if (d_scale[yRight])
    {
        rAxis[yRight] = QRect(rPixmap.x() + rPixmap.width(),
                              rPixmap.y() - vSpacing,
                              dim[yRight],
                              rPixmap.height() + 2 * vSpacing);
    }

#ifndef QWT_NO_LEGEND
    if (!printing && (pfilter.options() & QwtPlotPrintFilter::PrintLegend)
        && d_legend->itemCnt() > 0)
    {
        QSize hint = d_legend->sizeHint();
        if ( d_legendPos == Qwt::Right || d_legendPos == Qwt::Left )
        {
            if (rLegend.height() > hint.height() + ScrBarWidth)
            {
                // adjust the height; prevent scrollbars where possible
                if (rLegend.width() >= hint.width()) // has no hor. scrollbar
                    rLegend.setHeight(hint.height());
                else
                    rLegend.setHeight(hint.height() + ScrBarWidth);

                // put it at same the y position as rPixmap if useful;
                // center otherwise

                if (rLegend.height() <= rPixmap.height())
                    rLegend.moveBy(0, rPixmap.y() - rLegend.y());
                else
                    rLegend.moveBy(0,
                        contentsRect().center().y() - rLegend.center().y());
            }
        }
        else
        {
            if (rLegend.width() > (hint.width() + ScrBarWidth))
            {
                // adjust the width
                if (rLegend.height() >= hint.height()) // has novert. scrollbar
                    rLegend.setWidth(hint.width());
                else
                    rLegend.setWidth(hint.width() + ScrBarWidth);

                // put it in the center
                if (rLegend.width() <= (rPixmap.width() +
                        2 * qwtMin(rAxis[yLeft].width() , rAxis[yRight].width())))
                {
                    rLegend.moveBy(rPixmap.center().x()
                        - rLegend.center().x(), 0);
                }
                else
                {
                    rLegend.moveBy(contentsRect().center().x() -
                        rLegend.center().x(), 0);
                }
            }
        }
    }
#endif
}

//! Rebuild the scales and maps
void QwtPlot::updateAxes()
{
    int i;
    bool resetDone[axisCnt];
    for (i = 0; i < axisCnt; i++)
        resetDone[i] = FALSE;

    //
    //  Adjust autoscalers
    //

    QIntDictIterator<QwtPlotCurve> itc(*d_curves);
    for (QwtPlotCurve *c = itc.toFirst(); c != 0; c = ++itc )
    {
        if (c->dataSize() > 0)  // don't scale curves with no data
        {                       //  (minXValue() et al. would return 0.0)
            int axis = c->xAxis();
            if ( !resetDone[axis] )
            {
                d_as[axis].reset();
                resetDone[axis] = TRUE;
            }
            d_as[axis].adjust(c->minXValue(), c->maxXValue());

            axis = c->yAxis();
            if ( !resetDone[axis] )
            {
                d_as[axis].reset();
                resetDone[axis] = TRUE;
            }
            d_as[axis].adjust(c->minYValue(), c->maxYValue());
        }
    }

    //
    // Adjust scales
    //
    for (i=0; i<axisCnt; i++)
    {
        d_sdiv[i] = d_as[i].scaleDiv();
        d_scale[i]->setScaleDiv(d_sdiv[i]);
    }

    d_grid.setXDiv(d_sdiv[d_grid.xAxis()]);
    d_grid.setYDiv(d_sdiv[d_grid.yAxis()]);
}

//! drawContents
// \sa QFrame::drawContents
void QwtPlot::drawContents( QPainter * )
{
  // We must erase the region that is not
  // occupied by our children
  QRegion cr( contentsRect() );
  cr = cr.subtract( childrenRegion() );
  erase( cr );
}

//! Setup a QPixmap for double-buffering
// and call drawCanvas(QPainter*)
//  \param ur update rect
void QwtPlot::drawCanvas(const QRect &ur)
{
    if ( ur.isValid() )
    {
        // Use double-buffering
        QPixmap pix( ur.size() );
        pix.fill( d_canvas, ur.topLeft() );
        QPainter p( &pix, d_canvas );
        p.translate( -ur.x(), -ur.y() );
        drawCanvas(&p);
        p.end();

        bitBlt( d_canvas, ur.topLeft(), &pix );
    }
}

//! Redraw grid, curves, and markers. The draw code
//  does not clear clipRegion prior to painting.
//  \param p painter used for drawing
void QwtPlot::drawCanvas(QPainter *p)
{
    QwtDiMap map[axisCnt];
    for ( int axis = 0; axis < axisCnt; axis++ )
        map[axis] = canvasMap(axis);

    QRect rect = d_canvas->contentsRect();

    //
    // draw grid
    //
    if ( d_grid.enabled() &&
         axisEnabled( d_grid.xAxis() ) &&
         axisEnabled( d_grid.yAxis() ) )
    {
        d_grid.draw(p, rect, map[d_grid.xAxis()], map[d_grid.yAxis()]);
    }

    //
    //  draw curves
    //

    QIntDictIterator<QwtPlotCurve> itc(*d_curves);
    for (QwtPlotCurve *curve = itc.toFirst(); curve != 0; curve = ++itc )
    {
      if ( curve->enabled() &&
           axisEnabled( curve->xAxis() ) &&
           axisEnabled( curve->yAxis() ) )
      {
            curve->draw(p, map[curve->xAxis()], map[curve->yAxis()]);
      }
    }

    //
    // draw markers
    //

    QIntDictIterator<QwtPlotMarker> itm(*d_markers);
    for (QwtPlotMarker *marker = itm.toFirst(); marker != 0; marker = ++itm )
    {
        if ( marker->enabled() && axisEnabled( marker->xAxis() ) &&
               axisEnabled( marker->yAxis() ) )
        {
            marker->draw(p,
                map[marker->xAxis()].transform(marker->xValue()),
                map[marker->yAxis()].transform(marker->yValue()),
                rect);
        }
    }
}

/*!
  \param axis Axis
  \return Map for the axis on the canvas. With this map pixel coordinates can
          translated to plot coordinates and vice versa.
  \sa QwtDiMap, QwtPlot::transform, QwtPlot::invTransform
  
*/
QwtDiMap QwtPlot::canvasMap(int axis) const
{
    QwtDiMap map;
    if ( d_canvas && d_scale[axis] )
    {
        const QwtScale *s = d_scale[axis];
        map = *s->scaleDraw();
        if ( axis == yLeft || axis == yRight )
        {
            int y = s->y() + s->startBorderDist() - d_canvas->y();
            int h = s->height() - s->startBorderDist() - s->endBorderDist();
            map.setIntRange(y + h, y);
        }
        else
        {
            int x = s->x() + s->startBorderDist() - d_canvas->x();
            int w = s->width() - s->startBorderDist() - s->endBorderDist();
            map.setIntRange(x, x + w);
        }
    }
    return map;
}

/*!
  \brief Change the margin of the plot
  \param margin new margin
*/
void QwtPlot::setMargin(int margin)
{
    if ( margin < 0 )
        margin = 0;

    if ( margin != d_margin )
    {
        d_margin = margin;
        updateLayout();
    }
}

/*!
    \return margin
*/
int QwtPlot::margin() const
{
    return d_margin;
}

/*!
  \brief Change the background of the plotting area
  
  Sets c to QColorGroup::Background of all colorgroups of 
  the palette of the canvas. Using canvas()->setPalette()
  is a more powerful way to set these colors.
  \param c new background color
*/
void QwtPlot::setCanvasBackground(const QColor &c)
{
    QPalette p = d_canvas->palette();

    for ( int i = 0; i < QPalette::NColorGroups; i++ )
        p.setColor((QPalette::ColorGroup)i, QColorGroup::Background, c);

    canvas()->setPalette(p);
}

/*!
  Nothing else than: canvas()->palette().color(
        QPalette::Normal, QColorGroup::Background);
  
  \return the background color of the plotting area.
*/
const QColor & QwtPlot::canvasBackground() const
{
    return canvas()->palette().color(
        QPalette::Normal, QColorGroup::Background);
}

/*!
  \brief Change the border width of the plotting area
  Nothing else than canvas()->setLineWidth(w), 
  left for compatibility only.
  \param w new border width
*/
void QwtPlot::setCanvasLineWidth(int w)
{
    canvas()->setLineWidth(w);
}
 
/*! 
  Nothing else than: canvas()->lineWidth(), 
  left for compatibility only.
  \return the border width of the plotting area
*/
int QwtPlot::canvasLineWidth() const
{ 
    return canvas()->lineWidth();
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
  \sa enableOutline()
*/
void QwtPlot::setOutlineStyle(Qwt::Shape os)
{
    d_canvas->setOutlineStyle(os);
}



/*!
  \brief Specify a pen for the outline
  \param pn new pen
*/
void QwtPlot::setOutlinePen(const QPen &pn)
{
    d_canvas->setOutlinePen(pn);
}

/*!
  \return \c TRUE if the specified axis exists, otherwise \c FALSE
  \param axis axis index
 */
bool QwtPlot::axisValid(int axis)
{
    return ((axis >= QwtPlot::yLeft) && (axis < QwtPlot::axisCnt));
}


/*!
  \return \c TRUE if the outline feature is enabled
*/
bool QwtPlot::outlineEnabled() const
{
     return d_canvas->outlineEnabled();
}

/*!
  \return the pen used to draw outlines
*/
const QPen & QwtPlot::outlinePen() const
{
    return d_canvas->outlinePen();
}

/*!
  \return the outline style
  \sa setOutlineStyle()
*/
Qwt::Shape QwtPlot::outlineStyle() const
{
    return d_canvas->outlineStyle();
}

/*!
  Called internally when the legend has been clicked on.
  Emits a legendClicked() signal.
*/

#ifndef QWT_NO_LEGEND
void QwtPlot::lgdClicked(int index)
{
    emit legendClicked(d_legend->key(index));
}
#else
void QwtPlot::lgdClicked(int)
{
}
#endif

//! Remove all curves and markers
void QwtPlot::clear()
{
#ifndef QWT_NO_LEGEND
    d_legend->clear();
#endif
    d_curves->clear();
    d_markers->clear();
}


//! Remove all curves
void QwtPlot::removeCurves()
{
    d_curves->clear();
#ifndef QWT_NO_LEGEND
    d_legend->clear();
#endif
    autoRefresh();
}

//! Remove all markers
void QwtPlot::removeMarkers()
{
    d_markers->clear();
    autoRefresh();
}

/*!
  \brief Set or reset the autoLegend option
  If the autoLegend option is set, a item will be added
  to the legend whenever a curve is inserted.

  The autoLegend option is set to FALSE by default, which
  means that the user has to call enableLegend.
  \param tf \c TRUE or \c FALSE. Defaults to \c FALSE.
  \sa QwtPlot::enableLegend()
*/
void QwtPlot::setAutoLegend(bool tf)
{
    d_autoLegend = tf;
}

/*!
    \return TRUE if the autoLegend option is set.
*/
bool QwtPlot::autoLegend() const
{
    return d_autoLegend;
}


/*!
  \brief Enable or disable the legend
  \param tf \c TRUE (enabled) or \c FALSE (disabled)
  \param curveKey Key of a existing curve.
                  If curveKey < 0 the legends for all
                  curves will be updated.
  \sa QwtPlot::setAutoLegend()
  \sa QwtPlot::setLegendPos()
*/
void QwtPlot::enableLegend(
#ifndef QWT_NO_LEGEND
    bool enable, long curveKey
#else
    bool, long
#endif
)
{
#ifndef QWT_NO_LEGEND
    QwtPlotCurve *curCurve;

    bool isUpdateEnabled = d_legend->isUpdatesEnabled();
    d_legend->setUpdatesEnabled(FALSE);

    if ( curveKey < 0 ) // legends for all curves
    {
        if ( enable )
        {
            if ( d_legend->itemCnt() < d_curves->count() )
            {
                // not all curves have a legend

                d_legend->clear();

                QIntDictIterator<QwtPlotCurve> itc(*d_curves);
                itc.toFirst();

                while ((curCurve = itc.current()))
                {
                    d_legend->appendItem(curCurve->title(),
                        curCurve->symbol(), curCurve->pen(), itc.currentKey());

                    ++itc;
                }
            }
        }
        else
        {
            if ( d_legend->itemCnt() > 0 )
                d_legend->clear();
        }
    }
    else
    {
        uint index = d_legend->findFirstKey(curveKey);
        if ( enable )
        {
            curCurve = d_curves->find(curveKey);
            if ( curCurve && ( index >= d_legend->itemCnt() ) )
            {
                // curve exists and has no legend

                d_legend->appendItem(curCurve->title(),
                        curCurve->symbol(), curCurve->pen(), curveKey);
            }
        }
        else
        {
            if ( index < d_legend->itemCnt() )
                d_legend->removeItem(index);
        }
    }

    d_legend->setUpdatesEnabled(isUpdateEnabled);
	updateLayout();
#endif
}

/*!
  \param curveKey Curve key.
  \return \c TRUE if legend is enabled, otherwise \c FALSE
*/

bool QwtPlot::legendEnabled(
#ifndef QWT_NO_LEGEND
long curveKey
#else
long
#endif
) const
{
#ifndef QWT_NO_LEGEND
    return d_legend->findFirstKey(curveKey) < d_legend->itemCnt();
#else
    return FALSE;
#endif
}

/*!
  \brief Specify the position of the legend within the widget
  \param pos The legend's position. Valid values are \c Qwt::Left,
           \c Qwt::Right, \c Qwt::Top, \c QwtBottom.
*/
void QwtPlot::setLegendPos(int pos)
{
    if (pos != d_legendPos)
    {
        switch(pos)
        {
        case Qwt::Top:
        case Qwt::Bottom:
            break;

        case Qwt::Left:
        case Qwt::Right:
#ifndef QWT_NO_LEGEND
            d_legend->setMaxCols(1);
#endif
            break;
        default:
            pos = Qwt::Bottom;
            break;
        }
        d_legendPos = pos;
#ifndef QWT_NO_LEGEND
        updateLayout();
#endif
    }
}

/*!
    \return position of the legend
    \sa QwtPlot::setLegendPos
*/
int QwtPlot::legendPos() const
{
    return d_legendPos;
}

/*!
  \brief Change the font of the legend items
  \param fnew font
*/
void QwtPlot::setLegendFont(
#ifndef QWT_NO_LEGEND
const QFont &f
#else
const QFont &
#endif
)
{
#ifndef QWT_NO_LEGEND
    d_legend->setFont(f);
    if (d_legend->isVisible())
        updateLayout();
#endif
}

/*!
  \brief Change the legend's frame style
  \param stFrame Style. See Qt manual on QFrame.
*/
void QwtPlot::setLegendFrameStyle(
#ifndef QWT_NO_LEGEND
int st
#else
int
#endif
)
{
#ifndef QWT_NO_LEGEND
    d_legend->setFrameStyle(st);
    updateLayout();
#endif
}



/*!
  \return the frame style of the legend
*/
int QwtPlot::legendFrameStyle() const
{
#ifndef QWT_NO_LEGEND
    return d_legend->frameStyle();
#else
    return QFrame::NoFrame;
#endif
}

/*!
  \return the font of the legend items
*/
const QFont QwtPlot::legendFont() const
{
#ifndef QWT_NO_LEGEND
    return d_legend->font();
#else
    return QFont();
#endif
}
