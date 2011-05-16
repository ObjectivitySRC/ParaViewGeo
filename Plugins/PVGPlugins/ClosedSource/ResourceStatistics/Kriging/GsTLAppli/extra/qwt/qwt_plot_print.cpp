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

#include <qpixmap.h>
#include "qwt_plot_canvas.h"

#include <qlabel.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include "qwt_legend.h"
#include "qwt_plot.h"
#include "qwt_plot_dict.h"
#include "qwt_rect.h"
#include "qwt_scale.h"
#include "qwt_math.h"

void QwtPlot::print( QPixmap* pix, const QwtPlotPrintFilter &pfilter ) {
  // resize the pixmap to the size of the qwtplot and fill it with white.
  pix->resize( size() );
  pix->fill();

  QPainter painter( pix );
  print( &painter, pix->rect(), pfilter );
}



/*!
  \brief Print the plot to a \c QPaintDevice (\c QPrinter)
  This function prints the contents of a QwtPlot instance to
  \c QPaintDevice object. The size is derived from its device
  metrics.

  \param paintDev device to paint on, often a printer
  \param flags flags must be a bitwise OR of \c Qwt::PrintFlags values
  \param pfilter print filter
  \sa QwtPlot::print
  \sa QwtPlotPrintFilter
*/

void QwtPlot::print(QPaintDevice &paintDev,
    const QwtPlotPrintFilter &pfilter) const
{
    int margin = 0;
    QPaintDeviceMetrics mpr(&paintDev);
#if 0
    // The following printer specific code sets margin, document title and
    // creator of the print. All attributes should be definable
    // by the application, so I disable it. If nobody is missing
    // it I will remove it later. U.R. 12.12.01

    if ( paintDev.devType() == QInternal::Printer() &&
        paintDev.inherits("QPrinter") )
    {
      QPrinter &printer = (QPrinter &)paintDev;

      // d_lblTitle may be multi-line, but PostScript cannot handle this.
      // Thus we replace each "\n" */

      QString docName = d_lblTitle->text();
      docName.replace (QRegExp (QString::fromLatin1 ("\n")), tr (" -- "));
      printer.setDocName (docName);

      printer.setCreator("QwtPlot");

      margin = qwtMin(mpr.width(), mpr.height()) / 10;
    }
#endif

    QRect rect( margin, margin,
        mpr.width() - 2 * margin, mpr.height() - 2 * margin );
    rect.setHeight(qwtMin(rect.height(), int(0.66 * double(rect.width()))));

    QPainter p(&paintDev);
    print(&p, rect, pfilter);
}
/*!
  \brief Paint the plot into a given rectangle.
  Paint the contents of a QwtPlot instance into a given rectangle.

  \param painter Painter
  \param plotRect Bounding rectangle
  \param flags flags must be a bitwise OR of \c Qwt::PrintFlags values
  \param pfilter Print filter
  \sa QwtPlotPrintFilter
*/
void QwtPlot::print(QPainter *painter, const QRect &plotRect,
        const QwtPlotPrintFilter &pfilter) const
{
    if ( painter == 0 || !plotRect.isValid() || size().isNull() )
       return;

    int hDist, vDist;
    vDist = hDist = 2;

    if (d_axisEnabled[xTop])
        hDist = qwtMax(hDist, d_scale[xTop]->minBorderDist());
    if (d_axisEnabled[xBottom])
        hDist = qwtMax(hDist, d_scale[xBottom]->minBorderDist());

    if (d_axisEnabled[yLeft])
       vDist = qwtMax(vDist, d_scale[yLeft]->minBorderDist());
    if (d_axisEnabled[yRight])
       vDist = qwtMax(vDist, d_scale[yRight]->minBorderDist());

    QRect rTitle, rLegend, rPixmap, rAxis[axisCnt];
    findLayout(TRUE, plotRect, hDist, vDist, pfilter, rTitle,
        rLegend, rAxis, rPixmap);

    painter->save();

    if ((pfilter.options() & QwtPlotPrintFilter::PrintTitle)
        && (!d_lblTitle->text().isEmpty()))
    {
        painter->setFont(pfilter.font(d_lblTitle->font(),
            QwtPlotPrintFilter::Title));
        painter->drawText(rTitle, d_lblTitle->alignment(),
            d_lblTitle->text());
    }

#ifndef QWT_NO_LEGEND
    if ( (pfilter.options() & QwtPlotPrintFilter::PrintLegend)
        && d_legend->itemCnt() > 0 )
    {
        printLegend(painter, rLegend, rPixmap, pfilter);
    }
#endif

    for ( int axis = 0; axis < axisCnt; axis++ )
    {
        if (d_scale[axis])
        {
            int borderDist =
                ( axis == yLeft || axis == yRight ) ? vDist : hDist;
            int baseDist = d_scale[axis]->baseLineDist();
            if ( !(pfilter.options() & QwtPlotPrintFilter::PrintBackground) )
                baseDist = 0;

            printScale(painter, axis,
                borderDist, baseDist, rAxis[axis], pfilter);
        }
    }

    if ( pfilter.options() & QwtPlotPrintFilter::PrintBackground )
    {
        QColor c = pfilter.color(canvasBackground(),
            QwtPlotPrintFilter::Background);
        painter->fillRect(rPixmap, c);
    }
    else
        painter->drawRect(rPixmap);

    //
    // build maps
    //

    QwtDiMap map[axisCnt];
    for (int i=0; i<axisCnt; i++)
    {
        map[i].setDblRange(d_sdiv[i].lBound(),
            d_sdiv[i].hBound(), d_sdiv[i].logScale());

        if ( i == xTop || i == xBottom )
        {
            map[i].setIntRange(rAxis[i].left() + hDist,
                rAxis[i].right() - hDist);
        }
        else
        {
            map[i].setIntRange(rAxis[i].bottom() - vDist,
                rAxis[i].top() + vDist);
        }
    }

    printPixrect(painter, map, rPixmap, pfilter);

    painter->restore();
}

/*!
  \brief Paint the legend into a given rectangle.
  Paint the legend into a given rectangle.

  \param painter Painter
  \param rect Bounding rectangle
  \param pfilter Print filter
  \sa QwtPlotPrintFilter
*/

void QwtPlot::printLegend(
#ifndef QWT_NO_LEGEND
QPainter *painter, const QRect &rect,
    const QRect &pixRect, const QwtPlotPrintFilter &pfilter
#else
QPainter *, const QRect &, const QRect &, const QwtPlotPrintFilter &
#endif
) const
{
#ifndef QWT_NO_LEGEND
    if ( d_legend->itemCnt() == 0 )
        return;

    const QFont f = pfilter.font(d_legend->font(), QwtPlotPrintFilter::Legend);

    const QSize cellSize = d_legend->cellSizeHint(f);
    const QSize cellSymbolSize = d_legend->cellSymbolSizeHint();
    const QSize cellLabelSize = d_legend->cellLabelSizeHint(f);
    int bd = (cellSize.width() - cellSymbolSize.width()
                - cellLabelSize.width()) / 3;

    int centerAlignX = 0;
    int centerAlignY = 0;

    switch(d_legendPos)
    {
        case Qwt::Top:
        case Qwt::Bottom:
            centerAlignX = ( rect.width() -
                d_legend->numCols() * cellSize.width() ) / 2;
            break;
        case Qwt::Left:
        case Qwt::Right:
            if ( d_legend->numRows() * cellSize.height() < pixRect.height() )
                centerAlignY = pixRect.y();
            break;
    }

    painter->save();
    painter->setFont(f);

    for ( int row = 0; row < d_legend->numRows(); row++ )
    {
        int y = rect.top() + centerAlignY + row * cellSize.height();

        for ( int col = 0; col < d_legend->numCols(); col++ )
        {
            int x = rect.left() + centerAlignX + col * cellSize.width();

            QRect symRect(x + bd, y, cellSymbolSize.width(),
                cellSize.height());

            QRect lblRect(x + bd + cellSymbolSize.width() + bd, y,
                cellLabelSize.width(), cellSize.height());

            QwtLegendItem *item =
                d_legend->d_item.at(d_legend->findIndex(row, col));
            if ( item )
            {
                QPen pen = item->pen();
                QPen cpPen = pen;

                QColor c = pfilter.color(pen.color(),
                    QwtPlotPrintFilter::Legend, item->key());
                cpPen.setColor(c);

                item->setPen(cpPen);
                item->draw(painter, symRect,
                    lblRect, d_legend->d_align);
                item->setPen(pen);
            }
        }
    }
    painter->restore();
#endif
}

/*!
  \brief Paint a scale into a given rectangle.
  Paint the scale into a given rectangle.

  \param painter Painter
  \param axis Axis
  \param borderDist Border distance
  \param baseDist Base distance
  \param rect Bounding rectangle
*/

void QwtPlot::printScale(QPainter *painter,
    int axis, int borderDist, int baseDist, const QRect &rect,
    const QwtPlotPrintFilter &pfilter) const
{
    if (!d_axisEnabled[axis])
        return;

    QwtScaleDraw::Orientation o;
    int x, y, w;

    switch(axis)
    {
        case yLeft:
        {
            x = rect.right() - baseDist;
            y = rect.y() + borderDist;
            w = rect.height() - 2 * borderDist;
            o = QwtScaleDraw::Left;
            break;
        }
        case yRight:
        {
            x = rect.left() + baseDist;
            y = rect.y() + borderDist;
            w = rect.height() - 2 * borderDist;
            o = QwtScaleDraw::Right;
            break;
        }
        case xTop:
        {
            x = rect.left() + borderDist;
            y = rect.bottom() - baseDist;
            w = rect.width() - 2 * borderDist;
            o = QwtScaleDraw::Top;
            break;
        }
        case xBottom:
        {
            x = rect.left() + borderDist;
            y = rect.top() + baseDist;
            w = rect.width() - 2 * borderDist;
            o = QwtScaleDraw::Bottom;
            break;
        }
        default:
            return;
    }

    const QwtScale *scale = d_scale[axis];

    painter->setPen(pfilter.color(Qt::black,
        QwtPlotPrintFilter::AxisTitle, axis));
    painter->setFont(pfilter.font(scale->titleFont(),
        QwtPlotPrintFilter::AxisTitle, axis));
    QwtScale::drawTitle(painter, o, rect,
        scale->titleAlignment(), scale->title());

    painter->setPen(pfilter.color(Qt::black,
        QwtPlotPrintFilter::AxisScale, axis));
    painter->setFont(pfilter.font(scale->font(),
        QwtPlotPrintFilter::AxisScale, axis));

    QwtScaleDraw *sd = (QwtScaleDraw *)scale->scaleDraw();
    int xSd = sd->x();
    int ySd = sd->y();
    int lengthSd = sd->length();

    sd->setGeometry(x, y, w, o);
    sd->draw(painter);
    sd->setGeometry(xSd, ySd, lengthSd, o); // reset previous values
}

/*!
  \brief Paint the pixrect into a given rectangle.
  Paint the pixrect into a given rectangle.

  \param painter Painter
  \param pixRect Bounding rectangle
  \param pfilter Print filter
  \sa QwtPlotPrintFilter
*/

void QwtPlot::printPixrect(QPainter *painter, const QwtDiMap map[],
        const QRect &pixRect, const QwtPlotPrintFilter &pfilter) const
{
    if ( pfilter.options() & QwtPlotPrintFilter::PrintGrid )
    {
        QwtGrid cpGrid = d_grid;

        QPen cpPen = cpGrid.majPen();
        cpPen.setColor(pfilter.color(cpGrid.majPen().color(),
            QwtPlotPrintFilter::MajorGrid));
        cpGrid.setMajPen(cpPen);

        cpPen = cpGrid.minPen();
        cpPen.setColor(pfilter.color(cpGrid.minPen().color(),
            QwtPlotPrintFilter::MinorGrid));
        cpGrid.setMinPen(cpPen);

        cpGrid.draw(painter, pixRect,
            map[d_grid.xAxis()], map[d_grid.yAxis()]);
    }

    //
    //  draw curves
    //

    painter->setClipping(TRUE);
    painter->setClipRect(pixRect.x(), pixRect.y(),
        pixRect.width() - 1, pixRect.height() - 1);

    QIntDictIterator<QwtPlotCurve> itc(*d_curves);
    for (QwtPlotCurve *curCurve = itc.toFirst();
        curCurve != 0; curCurve = ++itc )
    {
        QwtCurve cpCurve = *curCurve;
        QPen cpPen = cpCurve.pen();
        cpPen.setColor(pfilter.color(cpPen.color(),
            QwtPlotPrintFilter::Curve, itc.currentKey()));
        cpCurve.setPen(cpPen);

        QwtSymbol s = cpCurve.symbol();
        cpPen = s.pen();
        cpPen.setColor(pfilter.color(cpPen.color(),
            QwtPlotPrintFilter::CurveSymbol, itc.currentKey()));
        s.setPen(cpPen);
        cpCurve.setSymbol(s);

        painter->save();
        cpCurve.draw(painter, map[curCurve->xAxis()], map[curCurve->yAxis()]);
        painter->restore();
    }

    //
    // draw markers
    //

    QIntDictIterator<QwtPlotMarker> itm(*d_markers);
    for (QwtPlotMarker *curMarker = itm.toFirst();
        curMarker != 0; curMarker = ++itm )
    {
        QwtMarker cpMarker = *curMarker;

        QPen cpPen = cpMarker.linePen();
        cpPen.setColor(pfilter.color(cpPen.color(),
            QwtPlotPrintFilter::Marker, itm.currentKey()));

        cpMarker.setLinePen(cpPen);

        cpPen = cpMarker.labelPen();
        cpPen.setColor(pfilter.color(cpPen.color(),
            QwtPlotPrintFilter::Marker, itm.currentKey()));
        cpMarker.setLabelPen(cpPen);

        QwtSymbol cpSym = cpMarker.symbol();
        cpPen = cpSym.pen();
        cpPen.setColor(pfilter.color(cpPen.color(),
            QwtPlotPrintFilter::MarkerSymbol, itm.currentKey()));
        cpSym.setPen(cpPen);

        cpMarker.setSymbol(cpSym);

        cpMarker.setFont(pfilter.font(cpMarker.font(),
            QwtPlotPrintFilter::Marker, itm.currentKey()));

        painter->save();
        cpMarker.draw(painter,
                map[curMarker->xAxis()].transform(curMarker->xValue()),
                map[curMarker->yAxis()].transform(curMarker->yValue()),
                pixRect);
        painter->restore();
    }
}
