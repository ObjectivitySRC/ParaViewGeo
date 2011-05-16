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

#ifndef QWT_PLOT_H
#define QWT_PLOT_H

#include <qframe.h>
#include "qwt_global.h"
#include "qwt_grid.h"
#include "qwt_autoscl.h"
#include "qwt_scldraw.h"
#include "qwt_symbol.h"
#include "qwt_curve.h"
#include "qwt_marker.h"
#include "qwt_plot_printfilter.h"
#include "qwt_plot_classes.h"

class QLabel;
class QwtScale;
class QwtCurveDict;
class QwtMarkerDict;
class QwtPlotCanvas;
#ifndef QWT_NO_LEGEND
class QwtLegend;
#endif

/*!
  \brief A 2-D plotting widget

  QwtPlot is a widget for plotting two-dimensional graphs.
  An unlimited number of data pairs can be displayed as
  curves in different styles and colors. A plot can have
  up to four axes, with each curve attached to an x- and
  a y axis. The scales at the axes are dimensioned automatically
  using an algorithm which can be configured separately for each
  axis. Linear and logarithmic scaling is supported. Markers of
  different styles can be added to the plot.

  Curves and markers are identified by unique keys which are generated
  automatically when a curve or a marker is inserted. These keys are used to
  access the properties of the corresponding curves and markers.

  A QwtPlot widget can have up to four axes which are indexed by
  the constants QwtPlot::yLeft, QwtPlot::yRight, QwtPlot::xTop,
  and QwtPlot::xBottom. Curves, markers, and the grid must be
  attached to an x axis and a y axis (Default: yLeft and xBottom).

  \par Example
  The following example shows (schematically) the most simple
  way to use QwtPlot. By default, only the left and bottom axes are
  visible and their scales are computed automatically.
  \verbatim
#include "../include/qwt_plot.h>

QwtPlot *myPlot;
long curve1, curve2;                    // keys
double x[100], y1[100], y2[100];        // x and y values

myPlot = new QwtPlot("Two Graphs", parent, name);

// add curves
curve1 = myPlot->insertCurve("Graph 1");
curve2 = myPlot->insertCurve("Graph 2");

getSomeValues(x, y1, y2);

// copy the data into the curves
myPlot->setCurveData(curve1, x, y1, 100);
myPlot->setCurveData(curve2, x, y2, 100);

// finally, refresh the plot
myPlot->replot();
\endverbatim
*/

class QWT_EXPORT QwtPlot: public QFrame
{
friend class QwtPlotItem;
friend class QwtPlotCanvas;

    Q_OBJECT

public:
    //! Axis index
    enum Axis { yLeft, yRight, xBottom, xTop, axisCnt };

    QwtPlot(QWidget *p = 0, const char *name = 0);
    QwtPlot(const QString &title, QWidget *p = 0, const char *name = 0);
    virtual ~QwtPlot();

    void setAutoReplot(bool tf = TRUE);
    bool autoReplot() const;

    void print(QPaintDevice &p,
        const QwtPlotPrintFilter & = QwtPlotPrintFilter()) const;
    virtual void print(QPainter *, const QRect &rect,
        const QwtPlotPrintFilter & = QwtPlotPrintFilter()) const;

    // added by Nicolas Remy, may 13, 2003.
    void print( QPixmap* pix, const QwtPlotPrintFilter & = QwtPlotPrintFilter() );


    void clear();

    // Outline

    void enableOutline(bool tf);
    bool outlineEnabled() const;
    void setOutlineStyle(Qwt::Shape os);
    Qwt::Shape outlineStyle() const;
    void setOutlinePen(const QPen &pn);
    const QPen& outlinePen() const;

    // Margin

    void setMargin(int margin);
    int margin() const;

    // Title

    void setTitle(const QString &t);
    QString title() const;
    void setTitleFont (const QFont &f);
    QFont titleFont() const;

    // Canvas

    QwtPlotCanvas *canvas();
    const QwtPlotCanvas *canvas() const;

    void setCanvasBackground (const QColor &c);
    const QColor& canvasBackground() const;

    void setCanvasLineWidth(int w);
    int canvasLineWidth() const;


    QwtDiMap canvasMap(int axis) const;

    double invTransform(int axis, int pos) const;
    int transform(int axis, double value) const;

    // Curves

    long insertCurve(QwtPlotCurve *);
    long insertCurve(const QString &title,
        int xAxis = xBottom, int yAxis = yLeft);

    QwtPlotCurve *curve(long key);
    const QwtPlotCurve *curve(long key) const;

    bool removeCurve(long key);
    void removeCurves();

    QArray<long> curveKeys() const;

    long closestCurve(int xpos, int ypos, int &dist) const;
    long closestCurve(int xpos, int ypos, int &dist,
        double &xval, double &yval, int &index) const;

    bool setCurveBaseline(long key, double ref);
    double curveBaseline(long key) const;

    bool setCurveData(long key, double *x, double *y, int size);
    bool setCurveRawData(long key, double *x, double *y, int size);

    bool setCurveOptions(long key, int t);
    int curveOptions(long key) const;

    bool setCurvePen(long key, const QPen &pen);
    QPen curvePen(long key) const;

    bool setCurveSplineSize(long key, int s);
    int curveSplineSize(long key) const;

    bool setCurveStyle(long key, int s, int options = 0);
    int curveStyle(long key) const;

    bool setCurveSymbol(long key, const QwtSymbol &s);
    QwtSymbol curveSymbol(long key) const;

    bool setCurveTitle(long key, const QString &s);
    QString curveTitle(long key) const;

    bool setCurveXAxis(long key, int axis);
    int curveXAxis(long key) const;
    bool setCurveYAxis(long key, int axis);
    int curveYAxis(long key) const;

    //  Grid

    void enableGridX(bool tf = TRUE);
    void enableGridXMin(bool tf = TRUE);
    void enableGridY(bool tf = TRUE);
    void enableGridYMin(bool tf = TRUE);

    void setGridXAxis(int axis);
    int gridXAxis() const;
    void setGridYAxis(int axis);
    int gridYAxis() const;

    void setGridPen(const QPen &p);
    void setGridMajPen(const QPen &p);
    const QPen& gridMajPen() const;
    void setGridMinPen(const QPen &p);
    const QPen& gridMinPen() const;

    // Axes

    void setAxisAutoScale(int axis);

    // Added by Nicolas Remy, may 23, 2003
    void setAxisAutoScale( int axis, bool on );
    
    bool axisAutoScale(int axis) const;

    void enableAxis(int axis, bool tf = TRUE);
    bool axisEnabled(int axis) const;

    void changeAxisOptions(int axis, int opt, bool value);
    void setAxisOptions(int axis, int opt);
    int axisOptions(int axis) const;

    void setAxisFont(int axis, const QFont &f);
    QFont axisFont(int axis) const;

    void setAxisMargins(int axis, double mlo, double mhi);
    bool axisMargins(int axis, double &mlo, double &mhi) const;

    void setAxisScale(int axis, double min, double max, double step = 0);
    void setAxisScaleDraw(int axis, QwtScaleDraw *);
    const QwtScaleDiv *axisScale(int axis) const;
    const QwtScaleDraw *axisScaleDraw(int axis) const;

    void setAxisLabelFormat(int axis, char f, int prec, int fieldwidth = 0);
    void axisLabelFormat(int axis, char &f, int &prec, int &fieldwidth) const;

    void setAxisTitle(int axis, const QString &t);
    const QString& axisTitle(int axis) const;
    void setAxisTitleFont(int axis, const QFont &f);
    QFont axisTitleFont(int axis) const;
    void setAxisTitleAlignment(int axis, int align);
    int axisTitleAlignment(int axis) const;

    void setAxisMaxMinor(int axis, int maxMinor);
    int axisMaxMajor(int axis) const;
    void setAxisMaxMajor(int axis, int maxMajor);
    int axisMaxMinor(int axis) const;

    void setAxisReference(int axis, double value);
    double axisReference(int axis) const;

    //  Markers

    long insertMarker(QwtPlotMarker *);
    long insertMarker(const QString &label = QString::null,
                int xAxis = xBottom, int yAxis = yLeft);
    long insertLineMarker(const QString &label, int axis);

    QwtPlotMarker *marker(long key);
    const QwtPlotMarker *marker(long key) const;

    bool removeMarker(long key);
    void removeMarkers();

    long closestMarker(int xpos, int ypos, int &dist) const;
    QArray<long> markerKeys() const;

    bool setMarkerXAxis(long key, int axis);
    int markerXAxis(long key) const;
    bool setMarkerYAxis(long key, int axis);
    int markerYAxis(long key) const;

    bool setMarkerPos(long key, double xval, double yVal);
    bool setMarkerXPos(long key, double val);
    bool setMarkerYPos(long key, double val);
    void markerPos(long key, double &mx, double &my) const;

    bool setMarkerFont(long key, const QFont &f);
    QFont markerFont(long key) const;
    bool setMarkerPen(long key, const QPen &p);

    bool setMarkerLabel(long key, const QString &txt);
    const QString& markerLabel(long key) const;
    bool setMarkerLabelAlign(long key, int align);
    int markerLabelAlign(long key) const;
    bool setMarkerLabelPen(long key, const QPen &p);
    QPen markerLabelPen(long key) const;

    bool setMarkerLinePen(long key, const QPen &p);
    QPen markerLinePen(long key) const;
    bool setMarkerLineStyle(long key, QwtMarker::LineStyle st);
    QwtMarker::LineStyle markerLineStyle(long key) const;

    bool setMarkerSymbol(long key, const QwtSymbol &s);
    QwtSymbol markerSymbol(long key) const;

    // Legend 

    void setAutoLegend(bool enabled);
    bool autoLegend() const;

    void enableLegend(bool tf, long curveKey = -1);
    bool legendEnabled(long curveKey) const;

    void setLegendPos(int pos);
    int legendPos() const;

    void setLegendFont(const QFont &f);
    const QFont legendFont() const;

    void setLegendFrameStyle(int st);
    int legendFrameStyle() const;

#ifndef QWT_NO_LEGEND
    QwtLegend * legend() { return d_legend; }
#endif    

    // Misc
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual QSizePolicy sizePolicy() const;

    virtual bool event(QEvent *);

public slots:
    virtual void replot();

signals:
    /*!
      A signal which is emitted when the mouse is pressed in the
      plotting area.
      \param e Mouse event object, event coordinates referring
               to the plotting area
     */
    void plotMousePressed(const QMouseEvent &e);

    /*!
      A signal which is emitted when a mouse button has been
      released in the plotting area.
      \param e Mouse event object, event coordinates referring
               to the plotting area
     */
    void plotMouseReleased(const QMouseEvent &e);

    /*!
      A signal which is emitted when the mouse is moved in the
      plotting area.
      \param e Mouse event object, event coordinates referring
               to the plotting area
     */
    void plotMouseMoved(const QMouseEvent &e);

    /*!
      A signal which is emitted when the user has clicked
      on a legend item.
      \param key Key of the curve corresponding to the
                 selected legend item
     */
    void legendClicked(long key);


protected:
    void autoRefresh();
    static bool axisValid(int axis);

    void drawCanvas(const QRect &);
    virtual void drawCanvas(QPainter *);
    virtual void drawContents( QPainter* p );

    void updateAxes();
    void updateLayout();

    virtual void resizeEvent(QResizeEvent *e);

protected:
    QLabel *d_lblTitle;
    QwtPlotCanvas *d_canvas;
    QwtScale *d_scale[axisCnt];
#ifndef QWT_NO_LEGEND
    QwtLegend *d_legend;
#endif

    QwtAutoScale d_as[axisCnt];
    QwtScaleDiv d_sdiv[axisCnt];
    QwtPlotGrid d_grid;

    QwtCurveDict *d_curves;
    QwtMarkerDict *d_markers;

private slots:
    void lgdClicked(int index);

private:
    void initAxes();
    long newCurveKey();
    long newMarkerKey();
    void initPlot(const QString &title = QString::null);

    void printScale(QPainter *, int axis, int borderDist,
        int baseDist, const QRect &, const QwtPlotPrintFilter &) const;
    void printPixrect(QPainter *, const QwtDiMap map[], const QRect &,
        const QwtPlotPrintFilter &) const;
    void printLegend(QPainter *, const QRect &, const QRect &pixRect,
        const QwtPlotPrintFilter &) const;

    //!  Recalculate the layout.  
    virtual void findLayout(bool printing, const QRect &rect, 
        int hDist, int vDist,
        const QwtPlotPrintFilter &, QRect &rTitle, QRect &rLegend,
        QRect rAxis[axisCnt], QRect &rPixmap) const;

private:
    bool d_axisEnabled[axisCnt];
    int d_legendPos;

    int d_margin;
    int d_spacing;

    bool d_autoReplot;
    bool d_autoLegend;
};

#endif
