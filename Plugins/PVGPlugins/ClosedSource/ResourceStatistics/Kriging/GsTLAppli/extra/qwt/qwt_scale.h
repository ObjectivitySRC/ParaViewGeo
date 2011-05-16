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

#ifndef QWT_SCALE_H
#define QWT_SCALE_H

#include <qwidget.h>
#include <qfont.h>
#include <qcolor.h>
#include <qstring.h>

#include "qwt_global.h"
#include "qwt_scldiv.h"
#include "qwt_scldraw.h"

class QPainter;

/*!
  \brief A Widget which contains a scale

  This Widget can be used to decorate composite widgets with
  a scale.
*/

class QWT_EXPORT QwtScale : public QWidget
{
public:
    /*! 
        Orientation/Position of the scale
        \sa QwtScale::QwtScale
     */
    
    enum Orientation { Left, Right, Top, Bottom };

    QwtScale(Orientation, QWidget *parent, const char *name = 0);
    virtual ~QwtScale();

    void setTitle(const QString &title);
    const QString& title() const;

    void setTitleColor(const QColor &c);
    const QColor &titleColor() const;

    void setTitleFont(const QFont &f);
    const QFont &titleFont() const;

    void setTitleAlignment(int flags);
    int titleAlignment() const;

    void setBorderDist(int start, int end);
    int startBorderDist() const;
    int endBorderDist() const;
    int minBorderDist() const;

    void setBaselineDist(int bd);
    int baseLineDist() const;

    void setScaleDiv(const QwtScaleDiv &sd);
    void setScaleDraw(QwtScaleDraw *);
    const QwtScaleDraw *scaleDraw() const;

    void setLabelFormat(char f, int prec, int fieldWidth);
    void labelFormat(char &f, int &prec, int &fieldWidth) const;

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual QSizePolicy sizePolicy() const;

    int dimForLength(int length,
        const QFontMetrics &titleFontMetrics,
        const QFontMetrics &scaleFontMetrics ) const;

    static void drawTitle(QPainter *, QwtScaleDraw::Orientation o,
        const QRect &, int align, const QString &);

protected:
    void draw(QPainter *p) const;
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void scaleChange();
    void fontChange(const QFont &oldfont);
    void layoutScale( bool update = TRUE );

private:
    QwtScaleDraw *d_scaleDraw;
    QwtScaleDiv d_sdiv;

    int d_borderDist[2];
    int d_scaleLength;
    int d_baseDist;

    int d_titleOffset;
    QString d_title;
    int d_titleDist;
    QFont d_titleFont;
    QColor d_titleColor;
    int d_titleAlign;
};

#endif
