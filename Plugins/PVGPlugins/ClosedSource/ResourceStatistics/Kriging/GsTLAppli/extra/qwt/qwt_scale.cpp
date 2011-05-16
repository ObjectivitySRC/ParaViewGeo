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
#include <qpixmap.h>
#include "qwt_scale.h"
#include "qwt_math.h"

/*!
  \brief Constructor
  \param ori Orientation. Must be one of
             <code>QwtScale::Left, QwtScale::Right, QwtScale::Bottom,
             </code>or \c QwtScale::Top
  \param parent
  \param name passed to QWidget's CTOR
*/
QwtScale::QwtScale(Orientation ori, QWidget *parent, const char *name):
    QWidget(parent,name, WRepaintNoErase|WResizeNoErase)
{
    QwtScaleDraw::Orientation sdo;
    d_borderDist[0] = 0;
    d_borderDist[1] = 0;
    d_baseDist = 4;
    d_titleOffset = 0;
    d_titleDist = 2;
    d_titleAlign = AlignHCenter | WordBreak | ExpandTabs;

    switch(ori)
    {
        case QwtScale::Left:
            sdo = QwtScaleDraw::Left;
            break;
        case QwtScale::Right:
            sdo = QwtScaleDraw::Right;
            break;
        case QwtScale::Bottom:
            sdo = QwtScaleDraw::Bottom;
            break;
        case QwtScale::Top:
            sdo = QwtScaleDraw::Top;
            break;
        default:
            sdo = QwtScaleDraw::Right;
    }

    d_scaleDraw = new QwtScaleDraw;
    d_scaleDraw->setGeometry(0,0,10,sdo);
}

//! Destructor
QwtScale::~QwtScale()
{
    delete d_scaleDraw;
}

/*!
  \brief Assign a title
  \param title new title
  \sa QwtScale::title
*/
void QwtScale::setTitle(const QString &title)
{
    if ( title != d_title )
    {
        d_title = title;
        layoutScale();
    }
}


/*!
  Specify distances of the scale's endpoints from the
  widget's borders. The actual borders will never be less
  than minimum border distance.
  \param dist1 Left or top Distance
  \param dist2 Right or bottom distance
  \sa QwtScale::borderDist
*/
void QwtScale::setBorderDist(int dist1, int dist2)
{
    if ( dist1 != d_borderDist[0] || dist2 != d_borderDist[1] )
    {
        d_borderDist[0] = dist1;
        d_borderDist[1] = dist2;
        layoutScale();
    }
}

/*!
  \brief Specify the border distance of the scale's base line.
  \param bd border distance of the baseline
  \sa QwtScale::baseLineDist
*/
void QwtScale::setBaselineDist(int bd)
{
    bd = qwtMax( 0, bd );
    if ( bd != d_baseDist )
    {
        d_baseDist = bd;
        layoutScale();
    }
}

/*!
  \brief Change the title font
  \param f new font
  \sa QwtScale::title
*/
void QwtScale::setTitleFont(const QFont &f)
{
    if ( f != d_titleFont )
    {
        d_titleFont = f;
        layoutScale();
    }
}

/*!
  \brief Change the title color
  \param c new title color
  \sa QwtScale::titleColor
*/
void QwtScale::setTitleColor(const QColor &c)
{
    d_titleColor = c;
    update();
}

/*!
  \brief Change the title alignment
  \param flags see Qt::AlignmentFlags. flags are interpreted in
               direction of the label, AlignTop, AlignBottom can't be set
               as the title will always be aligned to the scale.
  \sa QwtScale::titleAlignment, Qt::AlignmentFlags

*/
void QwtScale::setTitleAlignment(int flags)
{
    flags &= ~(AlignTop | AlignBottom);
    d_titleAlign = flags;
    layoutScale(); // SingleLine, ExpandTabs might affect layout
}

/*!
  \brief Set the number format for the major scale labels

  Format character and precision have the same meaning as for the
  QString class.
  \param f  format character
  \param prec precision
  \param fieldwidth minimum fieldwidth
  \sa QwtScale::labelFormat, QString::sprintf in the Qt manual
*/

void QwtScale::setLabelFormat(char f, int prec, int fieldwidth)
{
    d_scaleDraw->setLabelFormat(f, prec, fieldwidth);
    layoutScale();
}

/*!
  \brief Set a scale draw
  sd has to be created with new and will be deleted in
  QwtScale::~QwtScale or the next call of QwtScale::setScaleDraw.
*/
void QwtScale::setScaleDraw(QwtScaleDraw *sd)
{
    if ( sd == NULL )
        return;

    *sd = *d_scaleDraw;

    delete d_scaleDraw;
    d_scaleDraw = sd;

    layoutScale();
}

/*!
  Format character and precision have the same meaning as for the
  QString class.
  \retval f format character
  \retval prec precision
  \retval fieldwidth minimum fieldwidth
  \return the number format for the major scale labels
  \sa QwtScale::setLabelFormat, QString::sprintf in the Qt manual
*/

void QwtScale::labelFormat(char &f, int &prec, int &fieldwidth) const
{
    d_scaleDraw->labelFormat(f, prec, fieldwidth);
}

/*! 
    scaleDraw of this scale
    \sa QwtScaleDraw::setScaleDraw
*/
const QwtScaleDraw *QwtScale::scaleDraw() const 
{ 
    return d_scaleDraw; 
}

/*! 
    \return alignment
    \sa QwtScale::setTitleAlignment, Qt::AlignmentFlags
*/
int QwtScale::titleAlignment() const 
{ 
    return d_titleAlign; 
}

/*! 
    \return title font
    \sa QwtScale::setTitleFont
*/
const QFont &QwtScale::titleFont() const 
{ 
    return d_titleFont; 
}

/*! 
    \return title color
    \sa QwtScale::setTitleColor
*/
const QColor &QwtScale::titleColor() const 
{ 
    return d_titleColor; 
}

/*! 
    \return title 
    \sa QwtScale::setTitle
*/
const QString& QwtScale::title() const 
{ 
    return d_title; 
}

/*! 
    \return start border distance 
    \sa QwtScale::setBorderDist
*/
int QwtScale::startBorderDist() const 
{ 
    return d_borderDist[0]; 
}  

/*! 
    \return end border distance 
    \sa QwtScale::setBorderDist
*/
int QwtScale::endBorderDist() const 
{ 
    return d_borderDist[1]; 
}

/*! 
    \return base line distance 
    \sa QwtScale::setBaselineDist
*/
int QwtScale::baseLineDist() const 
{ 
    return d_baseDist; 
}

/*!
  \brief paintEvent
*/
void QwtScale::paintEvent(QPaintEvent *e)
{
    // Use double-buffering
    QRect ur = e->rect();
    if ( ur.isValid() )
    {
        QPixmap pix( ur.size() );
        pix.fill( this, ur.topLeft() );
        QPainter p( &pix, this );
        p.translate( -ur.x(), -ur.y() );
        draw(&p);
        p.end();
        bitBlt( this, ur.topLeft(), &pix );
    }
}

/*!
  \brief draw the scale
*/
void QwtScale::draw(QPainter *p) const
{
    d_scaleDraw->draw(p);

    p->setFont(d_titleFont);

    QRect r = rect();

    switch(d_scaleDraw->orientation())
    {
        case QwtScaleDraw::Bottom:
            r.setTop( r.top() + d_titleOffset );
            break;

        case QwtScaleDraw::Left:
            r.setRight( r.right() - d_titleOffset );
            break;

        case QwtScaleDraw::Right:
            r.setLeft( r.left() + d_titleOffset );
            break;

        case QwtScaleDraw::Top:
        default:
            r.setBottom( r.bottom() - d_titleOffset );
            break;
    }
    drawTitle(p, d_scaleDraw->orientation(), r,
        d_titleAlign, d_title);
}

/*!
  \brief paintEvent
*/
void QwtScale::resizeEvent(QResizeEvent *)
{
    layoutScale(FALSE);
}

//! Recalculate the scale's geometry and layout based on
//  the current rect and fonts.
//  \param update_geometry   notify the layout system and call update
//         to redraw the scale
void QwtScale::layoutScale( bool update_geometry )
{
    QRect r = this->rect();

    // Borders cannot be less than the minBorderDist
    int mbd = d_scaleDraw->minBorderDist( fontMetrics() );
    int bd0 = qwtMax( d_borderDist[0], mbd );
    int bd1 = qwtMax( d_borderDist[1], mbd );

    switch(d_scaleDraw->orientation())
    {
    case QwtScaleDraw::Bottom:
        d_scaleDraw->setGeometry(r.x() + bd0, r.y() + d_baseDist,
            r.width() - bd0 - bd1, QwtScaleDraw::Bottom);

        d_titleOffset = d_titleDist + d_baseDist +
            d_scaleDraw->maxHeight(QPen(), fontMetrics());
        break;

    case QwtScaleDraw::Top:
            d_scaleDraw->setGeometry(r.x() + bd0, r.bottom() - d_baseDist,
               r.width() - bd0 - bd1, QwtScaleDraw::Top);
        d_titleOffset =  d_titleDist + d_baseDist +
                           d_scaleDraw->maxHeight(QPen(), fontMetrics());
        break;

    case QwtScaleDraw::Left:
        d_scaleDraw->setGeometry(r.right() - d_baseDist, r.y() + bd0,
                            r.height() - bd0 - bd1, QwtScaleDraw::Left);
        d_titleOffset = d_baseDist + d_titleDist +
                        d_scaleDraw->maxWidth(QPen(), fontMetrics());
        break;

    case QwtScaleDraw::Right:
        d_scaleDraw->setGeometry(r.x() + d_baseDist, r.y() + bd0,
                            r.height() - bd0 - bd1, QwtScaleDraw::Right);
        d_titleOffset = d_baseDist + d_titleDist +
                          d_scaleDraw->maxWidth(QPen(), fontMetrics());
        break;
    default:
        break;
    }
    if ( update_geometry )
    {
      updateGeometry();
      update();
    }
}

/*!
  Rotate and paint a title according to its orientation into a given rectangle.
  \param painter Painter
  \param rect Bounding rectangle
  \param o Orientation
  \param text Text
*/

void QwtScale::drawTitle(QPainter *painter, QwtScaleDraw::Orientation o,
    const QRect &rect, int align, const QString &text)
{
    QRect r;
    double angle;

    switch(o)
    {
        case QwtScaleDraw::Left:
            align |= AlignTop;
            angle = -90.0;
            r.setRect(rect.left(), rect.bottom(),
                rect.height(), rect.width());
            break;
        case QwtScaleDraw::Right:
            align |= AlignTop;
            angle = 90.0;
            r.setRect(rect.left() + rect.width(), rect.top(),
                rect.height(), rect.width());
            break;
        case QwtScaleDraw::Top:
            align |= AlignTop;
            angle = 0.0;
            r = rect;
            break;
        case QwtScaleDraw::Bottom:
        default:
            align |= AlignBottom;
            angle = 0.0;
            r = rect;
            break;
    }

    painter->translate(r.x(), r.y());
    if (angle != 0.0)
      painter->rotate(angle);
    painter->drawText(0, 0, r.width(), r.height(), align, text);
    painter->resetXForm();
}

/*!
  \brief Notify a change of the scale

  This virtual function can be overloaded by derived
  classes. The default implementation updates the geometry
  and repaints the widget.
*/
void QwtScale::scaleChange()
{
    layoutScale();
}

/*!
  \return Fixed/MinimumExpanding for vertical,
          MinimumExpanding/Fixed for horizontal scales.
*/
QSizePolicy QwtScale::sizePolicy() const
{
    QSizePolicy sp;
    if ( d_scaleDraw->orientation() == QwtScaleDraw::Left ||
        d_scaleDraw->orientation() == QwtScaleDraw::Right )
    {
        sp.setHorData( QSizePolicy::Fixed );
        sp.setVerData( QSizePolicy::MinimumExpanding );
    }
    else
    {
        sp.setHorData( QSizePolicy::MinimumExpanding );
        sp.setVerData( QSizePolicy::Fixed );
    }
    return sp;
}

/*!
  \return a size hint
*/
QSize QwtScale::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \return a minimum size hint
*/
QSize QwtScale::minimumSizeHint() const
{
    Qt::Orientation o = Qt::Horizontal;
    if ( d_scaleDraw->orientation() == QwtScaleDraw::Left ||
        d_scaleDraw->orientation() == QwtScaleDraw::Right )
    {
        o = Qt::Vertical;
    }

    // Border Distance cannot be less than the scale minBorderDist
    // Note, the minBorderDist is already included in minHeight/minWidth
    int length = 0;
    int mbd = d_scaleDraw->minBorderDist( fontMetrics() );
    length += qwtMax( 0, d_borderDist[0] - mbd );
    length += qwtMax( 0, d_borderDist[1] - mbd );
    if ( o == Qt::Vertical )
        length += d_scaleDraw->minHeight(QPen(), fontMetrics());
    else
        length += d_scaleDraw->minWidth(QPen(), fontMetrics());

    int dim = dimForLength(length, QFontMetrics(d_titleFont),
        fontMetrics());
    if ( length < dim )
    {
      // compensate for long titles
      length = dim;
      dim = dimForLength(length, QFontMetrics(d_titleFont), fontMetrics());
    }

    QSize size(length, dim);
    if ( o == Qt::Vertical )
        size.transpose();

    return size;
}

/*!
  \brief Find the minimum dimension for a given length.
         dim is the height, length the width seen in
         direction of the title.
  \param length width for horizontal, height for vertical scales
  \return height for horizontal, width for vertical scales

*/

int QwtScale::dimForLength(int length,
    const QFontMetrics &titleFontMetrics,
    const QFontMetrics &scaleFontMetrics) const
{
    int dim;
    if ( d_scaleDraw->orientation() == QwtScaleDraw::Left ||
        d_scaleDraw->orientation() == QwtScaleDraw::Right )
    {
        dim = d_scaleDraw->minWidth(QPen(), scaleFontMetrics);
    }
    else
        dim = d_scaleDraw->minHeight(QPen(), scaleFontMetrics);

    QRect titleRect = titleFontMetrics.boundingRect(0, 0, length, 10000,
        d_titleAlign, d_title);

    dim += titleRect.height() + d_baseDist + d_titleDist;
    return dim;
}

/*!
  \brief Determine the minimum Border distance

  This member function returns the minimum distance
  of the scale's endpoints from the widget borders which
  is required for the mark labels to fit into the widget.
  \warning
  <ul> <li>The minimum border distance depends on the font.</ul>
  \sa QwtScale::setBorderDist()
*/
int QwtScale::minBorderDist() const
{
    return d_scaleDraw->minBorderDist( fontMetrics() );
}

/*!
  \brief Notify a change of the font

  This virtual function may be overloaded by derived widgets.
  The default implementation resizes the scale and repaints
  the widget.
  \param oldfont old font
*/
void QwtScale::fontChange(const QFont &f)
{
    QWidget::fontChange( f );
    layoutScale();
}

/*!
  \brief Assign a scale division

  The scale division determines where to set the tick
  marks.
  \param sd Scale Division
  \sa For more information about scale divisions, see QwtScaleDiv.
*/
void QwtScale::setScaleDiv(const QwtScaleDiv &sd)
{
    if (d_scaleDraw->scaleDiv() != sd)
    {
        d_scaleDraw->setScale(sd);
        layoutScale();
    }
}
