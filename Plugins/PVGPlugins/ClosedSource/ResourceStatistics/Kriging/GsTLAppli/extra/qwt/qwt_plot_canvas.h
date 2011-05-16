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

#ifndef QWT_PLOT_CANVAS_H
#define QWT_PLOT_CANVAS_H

#include <qframe.h>
#include <qpen.h>
#include "qwt_global.h"
#include "qwt.h"

class QwtPlot;

/*!
  \brief Canvas of a QwtPlot. 

  \sa  QwtPlot 
*/

class QWT_EXPORT QwtPlotCanvas : public QFrame
{
	Q_OBJECT
	friend class QwtPlot;
public:
    void enableOutline(bool tf);
    bool outlineEnabled() const;

    void setOutlinePen(const QPen &p);
    const QPen& outlinePen() const;

    void setOutlineStyle(Qwt::Shape os);
    Qwt::Shape outlineStyle() const;
    
signals:
    /*!
      A signal which is emitted when the mouse is pressed in the
      plotting area.
      \param e Mouse event object, event coordinates referring
               to the plotting area
     */
    void mousePressed(const QMouseEvent &e);

    /*!
      A signal which is emitted when a mouse button has been
      released in the plotting area.
      \param e Mouse event object, event coordinates referring
               to the plotting area
     */
    void mouseReleased(const QMouseEvent &e);
    
    /*!
      A signal which is emitted when the mouse is moved in the
      plotting area.
      \param e Mouse event object, event coordinates referring
               to the plotting area
     */
    void mouseMoved(const QMouseEvent &e);

protected:
    QwtPlotCanvas(QwtPlot *);

	virtual void frameChanged();
	virtual void drawContents(QPainter *);

    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:    
    void drawOutline(QPainter &p);

    bool d_outlineEnabled;
    bool d_outlineActive;
    bool d_mousePressed;
    Qwt::Shape d_outline;

    QPen d_pen;
    QPoint d_entryPoint;
    QPoint d_lastPoint;
};

#endif
