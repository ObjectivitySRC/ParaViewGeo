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
#include "qwt_scldraw.h"
#include "qwt_math.h"


int const QwtScaleDraw::minLen = 10;

const double step_eps = 1.0e-6;

/*!
  \brief Constructor

  The range of the scale is initialized to [0, 100],
  the angle range is set to [-135, 135], the geometry
  is initialized such that the origin is at (0,0), the
  length is 100, and the orientation is QwtScaleDraw::Bottom.
*/
QwtScaleDraw::QwtScaleDraw()
{
    d_hpad = 6;
    d_vpad = 3;
    d_majLen = 8;
    d_minLen = 4;
    d_medLen = 6;

    d_minAngle = -135 * 16;
    d_maxAngle = 135 * 16;
    d_fmt = 'g';
    d_prec = 4;
    d_fieldwidth = 0;
        
    // initialize scale and geometry
    setGeometry(0,0,100,Bottom);
    setScale(0,100,0,0,10);
}

//! Destructor
QwtScaleDraw::~QwtScaleDraw()
{
}

/*!
  \brief Adjust the range of the scale

  If step == 0.0, the step width is calculated automatically
  dependent on the maximal number of scale ticks.
  \param x1 value at the left/low endpoint of the scale
  \param x2 value at the right/high endpoint of the scale
  \param step step size (default : 0.0)
  \param logscale logarithmic scale (default : 0)
*/
void QwtScaleDraw::setScale(double x1, double x2, int maxMajIntv,
                            int maxMinIntv, double step, int logscale)
{
    d_scldiv.rebuild( x1, x2, maxMajIntv, maxMinIntv, logscale, step, FALSE );
    setDblRange( d_scldiv.lBound(), d_scldiv.hBound(), d_scldiv.logScale());
}


/*!
  \brief Change the scale division
  \param sd new scale division
*/
void QwtScaleDraw::setScale(const QwtScaleDiv &s)
{
    d_scldiv = s;
    setDblRange(d_scldiv.lBound(),d_scldiv.hBound(),d_scldiv.logScale());
}

/*!
  \brief Draw the scale
  \param p the painter
*/
void QwtScaleDraw::draw(QPainter *p) const
{
    double val,hval,majTick;
    
    int i;
    
    for (i=0; i< d_scldiv.majCnt(); i++)
    {
        val = d_scldiv.majMark(i);
        drawTick(p, val, d_majLen);
        drawLabel(p, val);
    }

    if (d_scldiv.logScale())
    {
        for (i=0; i< d_scldiv.minCnt(); i++)
            drawTick(p,d_scldiv.minMark(i),d_minLen);
    }
    else
    {
        int k = 0;
        int kmax = d_scldiv.majCnt() - 1;
        if (kmax > 0) 
        {
           majTick = d_scldiv.majMark(0);
           hval = majTick - 0.5 * d_scldiv.majStep();

           for (i=0; i< d_scldiv.minCnt(); i++)
           {
               val = d_scldiv.minMark(i);
               if  (val > majTick)
               {
                   if (k < kmax)
                   {
                       k++;
                       majTick = d_scldiv.majMark(k);
                   }
                   else
                   {
                       majTick += d_scldiv.majMark(kmax) + d_scldiv.majStep();
                   }
                   hval = majTick - 0.5 * d_scldiv.majStep();
                   
               }
               if (qwtAbs(val-hval) < step_eps * d_scldiv.majStep())
                  drawTick(p, val, d_medLen);
               else
                  drawTick(p, val, d_minLen);
           }
        }
    }

    drawBackbone(p);
}


//! Draws a singls scale tick
void QwtScaleDraw::drawTick(QPainter *p, double val, int len) const
{
    const int tval = transform(val);

    switch(d_orient)
    {
        case Right:
            p->drawLine(d_xorg, tval, d_xorg + len, tval);
            break;

        case Bottom: 
            p->drawLine(tval, d_yorg, tval, d_yorg + len);
            break;

        case Left:
            p->drawLine(d_xorg, tval, d_xorg - len, tval);
            break;

        case Round:
            if ((tval <= d_minAngle + 359 * 16) 
                || (tval >= d_minAngle - 359 * 16))
            {
                const double arc = double(tval) / 16.0 * M_PI / 180.0;
                const int x1 = qwtInt(d_xCenter + sin(arc) * d_radius);
                const int x2 = qwtInt(d_xCenter + sin(arc) 
                    * (d_radius + double(len)));
                const int y1 = qwtInt(d_yCenter - cos(arc) * d_radius);
                const int y2 = qwtInt(d_yCenter - cos(arc) 
                    * (d_radius + double(len)));

                p->drawLine(x1, y1, x2, y2);
            }
            break;

        case Top:
        default:
            p->drawLine(tval, d_yorg, tval, d_yorg - len);
            break;
    }
}

//! Draws the number label for a major scale tick
void QwtScaleDraw::drawLabel(QPainter *p, double val) const
{
    static const double pi_4 = M_PI * 0.25;
    static const double pi_75 = M_PI * 0.75;
    
    const QFontMetrics fm = p->fontMetrics();
    
    const int tval = transform(val);
    
    // correct rounding errors if val = 0
    if ((!d_scldiv.logScale()) 
        && (qwtAbs(val) < qwtAbs(step_eps * d_scldiv.majStep())))
    {
       val = 0.0;
    }
    
    QString lbl = label(val);
    
    switch(d_orient)
    {
        case Right:
            p->drawText(d_xorg + d_majLen + d_hpad,
                tval + (fm.ascent()-1) / 2, lbl);
            break;
        case Left:
            p->drawText(d_xorg - d_majLen - d_hpad - fm.width(lbl),
                tval + (fm.ascent() -1) / 2, lbl);
            break;
        case Bottom:
            p->drawText(tval - (fm.width(lbl)-1) / 2, 
                d_yorg + d_majLen + d_vpad + fm.ascent(), lbl);
            break;
        case Round:
        {
            if ((tval > d_minAngle + 359 * 16) 
                || (tval < d_minAngle - 359 * 16))
            {
               break;
            }
            
            double arc = double(tval) / 16.0 * M_PI / 180.0;
            
            // Map arc into the interval -pi <= arc <= pi
            if ((arc < -M_PI) || (arc > M_PI))
               arc -= floor((arc + M_PI) / M_PI * 0.5) * 2.0 * M_PI;
            
            const int xpos = 1 + qwtInt(d_xCenter + 
                (d_radius + double(d_majLen + d_vpad)) * sin(arc));
            const int ypos = qwtInt(d_yCenter - 
                (d_radius + double(d_majLen + d_vpad)) * cos(arc));
            
            if (arc < -pi_75)
            {
                p->drawText(xpos - qwtInt(double(fm.width(lbl))
                        * (1.0 + (arc + pi_75) * M_2_PI) ),
                    ypos + fm.ascent() - 1, lbl);
            }
            else if (arc < -M_PI_4)
            {
                p->drawText(xpos - fm.width(lbl),
                    ypos - qwtInt(double(fm.ascent() - 1)
                      * (arc + M_PI_4) * M_2_PI), lbl);
            }
            else if (arc < pi_4)
            {
                p->drawText(xpos + qwtInt(double(fm.width(lbl))
                      * ( arc - M_PI_4 ) * M_2_PI ),
                    ypos, lbl);
            }
            else if (arc < pi_75)
            {
                p->drawText(xpos, ypos + qwtInt(double(fm.ascent() - 1)
                  * (arc - M_PI_4) * M_2_PI), lbl);
            }
            else
            {
                p->drawText(xpos - qwtInt(double(fm.width(lbl))
                      * ( arc - pi_75) * M_2_PI ),
                    ypos + fm.ascent() - 1, lbl);
            }
            break;
        }
        case Top:
        default:
            p->drawText(tval - (fm.width(lbl)-1) / 2, 
                d_yorg - d_majLen - d_vpad, lbl);
            break;
    }
}

//! Draws the baseline of the scale
void QwtScaleDraw::drawBackbone(QPainter *p) const
{
    const int bw2 = p->pen().width() / 2;
    
    switch(d_orient)
    {
        case Left:
            p->drawLine(d_xorg - bw2, d_yorg, d_xorg - bw2, d_yorg + d_len - 1);
            break;
        case Right:
            p->drawLine(d_xorg + bw2, d_yorg, d_xorg + bw2, d_yorg + d_len - 1);
            break;
        case Round:
        {
            const int a1 = qwtMin(i1(), i2()) - 90 * 16; 
            const int a2 = qwtMax(i1(), i2()) - 90 * 16; 
            
            p->drawArc(d_xorg, d_yorg, d_len, d_len,
                -a2, a2 - a1 + 1);           // counterclockwise
            break;
        }
        case Top:
            p->drawLine(d_xorg, d_yorg - bw2, 
                d_xorg + d_len - 1, d_yorg - bw2);
            break;
        case Bottom:
            p->drawLine(d_xorg, d_yorg + bw2, 
                d_xorg + d_len - 1, d_yorg + bw2);
            break;
        default:
            p->drawLine(d_xorg, d_yorg, d_xorg + d_len - 1, d_yorg);
            break;
    }
}


/*!
  \brief Specify the geometry of the scale

  The parameters xorigin, yorigin and length have different meanings,
  dependent on the
  orientation:
  <dl>
  <dt>QwtScaleDraw::Left
  <dd>The origin is the topmost point of the
      baseline. The baseline is a vertical line with the
      specified length. Scale marks and labels are drawn
      at the left of the baseline.
  <dt>QwtScaleDraw::Right
  <dd>The origin is the topmost point of the
      baseline. The baseline is a vertical line with the
      specified length. Scale marks and labels are drawn
      at the right of the baseline.
  <dt>QwtScaleDraw::Top
  <dd>The origin is the leftmost point of the
      baseline. The baseline is a horizontal line with the
      specified length. Scale marks and labels are drawn
      above the baseline.
  <dt>QwtScaleDraw::Bottom
  <dd>The origin is the leftmost point of the
      baseline. The baseline is a horizontal line with the
      specified length. Scale marks and labels are drawn
      below the baseline.
  <dt>QwtScaleDraw::Round
  <dd>The origin is the top left corner of the
      bounding rectangle of the baseline circle. The baseline
      is the segment of a circle with a diameter of the specified length.
      Scale marks and labels are drawn outside the baseline
      circle.</dl>

  \param xorigin x coordinate of the origin
  \param yorigin y coordinate of the origin
  \param length length or diameter of the scale, excluding border distance
  \param o The orientation
*/
void QwtScaleDraw::setGeometry(int xorigin, int yorigin, 
    int length, Orientation o)
{
    d_xorg = xorigin;
    d_yorg = yorigin;
    d_radius = double(length) * 0.5;
    d_xCenter = double(xorigin) + double(length) * 0.5;
    d_yCenter = double(yorigin) + double(length) * 0.5;
    
    if (length > minLen)
       d_len = length;
    else
       d_len = minLen;
    
    d_orient = o;
    
    switch(d_orient)
    {
        case Left:
        case Right:
            setIntRange(d_yorg + d_len - 1, d_yorg);
            break;
        case Round:
            setIntRange(d_minAngle, d_maxAngle);
            break;
        case Top:
        case Bottom:
        default:
            setIntRange(d_xorg, d_xorg + d_len - 1);
            break;
    }
}

/*!
  \param pen pen
  \param fm font metrics
  \return the maximum width of the scale 
*/
int QwtScaleDraw::maxWidth(const QPen &pen, const QFontMetrics &fm) const
{
    int rv = maxLabelWidth(fm);
    
    switch (d_orient)
    {
        case Left:
        case Right:
            rv += (pen.width() + d_hpad + d_majLen);
            break;
        case Round:
            rv += (pen.width() + d_vpad + d_majLen);
            break;
        case Top:
        case Bottom:
        default:
            rv += d_len;
    }
    return rv;
}

/*!
  \param pen pen
  \param fm font metrics
  \return the maximum height of the scale
*/
int QwtScaleDraw::maxHeight(const QPen &pen, const QFontMetrics &fm) const 
{
    int rv = 0;
    
    switch (d_orient)
    {
        case Top:
        case Bottom:
        case Round:
            rv = pen.width() + d_vpad + d_majLen + fm.height();
            break;
        case Left:
        case Right:
        default:
          rv = d_len + fm.height();
    }
    
    return rv;
}

/*!
  The bounding rectangle is not very exact for round scales
  with strange angle ranges.
  \param p painter
  \return the maximum bounding rectangle of the scale
          for a specified painter
*/
QRect QwtScaleDraw::maxBoundingRect(const QFontMetrics &fm) const
{
    QRect r;
    
    const int wl = maxLabelWidth(fm);
    
    switch(d_orient)
    {
        case Left:
            r = QRect( d_xorg - d_hpad - d_majLen - wl,
                      d_yorg - fm.ascent(),
                      d_majLen + d_hpad + wl,
                      d_len + fm.height());
            break;
            
        case Right:
            r = QRect( d_xorg,
                      d_yorg - fm.ascent(),
                      d_majLen + d_hpad + wl,
                      d_len + fm.height());
            break;
            
        case Top:
            r = QRect ( d_xorg - wl / 2,
                       d_yorg - d_majLen - fm.ascent(),
                       d_len + wl,
                       d_majLen + d_vpad + fm.ascent());
            break;
            
        case Bottom:
            
            r = QRect ( d_xorg - wl / 2,
                       d_yorg,
                       d_len + wl,
                       d_majLen + d_vpad + fm.height());
            break;
            
        case Round:
        {
            int i;
            int amin = 2880;
            int amax = 0;

            for (i = 0; i < d_scldiv.majCnt(); i++)
            {
                int a = transform(d_scldiv.majMark(i));
                
                while (a > 2880) 
                    a -= 5760;
                while (a < - 2880) 
                    a += 5760;

                int ar = qwtAbs(a);
                if (ar < amin) 
                    amin = ar;
                if (ar > amax) 
                    amax = ar;

            }

            for (i = 0; i < d_scldiv.minCnt(); i++)
            {
                int a = transform(d_scldiv.minMark(i));
                
                while (a > 2880) 
                    a -= 5760;
                while (a < - 2880) 
                    a += 5760;

                int ar = qwtAbs(a);
                if (ar < amin) 
                    amin = ar;
                if (ar > amax) 
                    amax = ar;
            }

            double arc = double(amin) / 16.0 * M_PI / 180.0;
            r.setTop(qwtInt(d_yCenter - 
                (d_radius + double(d_majLen + d_vpad)) * cos(arc))
                 + fm.ascent() );

            arc = double(amax) / 16.0 * M_PI / 180.0;
            r.setBottom(qwtInt(d_yCenter - 
                (d_radius + double(d_majLen + d_vpad)) * cos(arc))
                + fm.height() );

            r.setLeft(d_xorg - d_majLen - d_hpad - wl);
            r.setWidth(d_len + 2*(d_majLen + d_hpad + wl));
            break;
        }
    }
    
    return r;
}

/*!
  \brief Adjust the baseline circle segment for round scales.

  The baseline will be drawn from min(angle1,angle2) to max(angle1, angle2).
  The settings have no effect if the scale orientation is not set to
  QwtScaleDraw::Round. The default setting is [ -135, 135 ].
  An angle of 0 degrees corresponds to the 12 o'clock position,
  and positive angles count in a clockwise direction.
  \param angle1
  \param angle2 boundaries of the angle interval in degrees.
  \warning <ul>
  <li>The angle range is limited to [-360, 360] degrees. Angles exceeding
      this range will be clipped.
  <li>For angles more than 359 degrees above or below min(angle1, angle2),
      scale marks will not be drawn.
  <li>If you need a counterclockwise scale, use QwtScaleDiv::setRange
  </ul>
*/
void QwtScaleDraw::setAngleRange(double angle1, double angle2)
{
    angle1 = qwtLim(angle1, -360.0, 360.0);
    angle2 = qwtLim(angle2, -360.0, 360.0);

    int amin = int(floor (qwtMin(angle1, angle2) * 16.0 + 0.5));
    int amax = int(floor (qwtMax(angle1, angle2) * 16.0 + 0.5));
    
    if (amin == amax)
    {
        amin -= 1;
        amax += 1;
    }
    
    d_minAngle = amin;
    d_maxAngle = amax;
    setIntRange(d_minAngle, d_maxAngle);
}


/*!
  \brief Set the number format for the major scale labels

  Format character and precision have the same meaning as for the
  QString class.
  \param f format character, 'g', 'e' or 'f'
  \param prec precision(f) or significance(g)
  \param fieldwidth fieldwidth
  \sa QString::sprintf in the Qt manual
*/
void QwtScaleDraw::setLabelFormat(char f, int prec, int fieldwidth)
{
    d_fmt = f;
    d_prec = prec;
        d_fieldwidth = fieldwidth;
}

/*!
  Format character and precision have the same meaning as for the
  QString class.
  \retval f format character 'g', 'e', or 'f'
  \retval prec precision(f), or siginificance(g)
  \return the number format for the major scale labels
  \sa QString::sprintf in the Qt manual
*/
void QwtScaleDraw::labelFormat(char &f, int &prec, int &fieldwidth) const
{
    f = d_fmt;
    prec = d_prec;
    fieldwidth = d_fieldwidth;
}

/*!
  \param fm QFontMetrics
  \return the maximum width of a label
*/
int QwtScaleDraw::maxLabelWidth(const QFontMetrics &fm) const
{
    int maxWidth = 0;

    if ( d_fieldwidth > 0 )
    {
        QString s;
        s.fill('0', d_fieldwidth);
        maxWidth = fm.width(s);
    }

    for (int i = 0; i < d_scldiv.majCnt(); i++)
    {
        double val = d_scldiv.majMark(i);

        // correct rounding errors if val = 0

        if ((!d_scldiv.logScale()) 
            && (qwtAbs(val) < step_eps * qwtAbs(d_scldiv.majStep())))
        {
            val = 0.0;
        }

        const int w = fm.width(label(val));
        if ( w > maxWidth )
            maxWidth = w;
    }

    return maxWidth;
}

/*!
  \brief Determine the minimum border distance

  This member function returns the minimum space
  needed to draw the mark labels at the scale's endpoints.

  \param fm QFontMetrics
  \return minimum border distance
*/
int QwtScaleDraw::minBorderDist( const QFontMetrics &fm ) const
{
  int rv = 0;
  if ( d_scldiv.majCnt() > 0 )
  {
    switch ( d_orient )
    {
      case Left:
      case Right:
        rv = ( fm.height() + 1 ) / 2;
        break;
      case Top:
      case Bottom:
      default:
        rv = ( maxLabelWidth( fm ) + 1 ) / 2;
        break;
    }
  }
  return rv;
}

/*!
  \param pen pen
  \param fm font metrics
  \return the minimum height required to draw the scale
          including the minimum border distance
*/
int QwtScaleDraw::minHeight( const QPen &pen, const QFontMetrics &fm ) const
{
    int rv = 0;
    const int pw = QMAX( 1, pen.width() );  // penwidth can be zero

    switch ( d_orient )
    {
        case Left:
        case Right:
        {
            // Compute the minimum scale length required to draw all ticks
            // plus the first and last major tick labels
            const int nticks = d_scldiv.majCnt() + d_scldiv.minCnt();
            rv = 2 * nticks * pw;
            if ( d_scldiv.majCnt() > 0 )
                rv += fm.height();

            // Compute the minimum scale length required to draw all major
            // tick labels for a vertical scale
            rv = QMAX( rv, d_scldiv.majCnt() * fm.height() );
            break;
        }
        case Round:
            // compute the radial thickness
            rv = pw + d_vpad + d_majLen + maxLabelWidth( fm );
            break;
        case Top:
        case Bottom:
        default:
            rv = pw + d_vpad + d_majLen + fm.height();
            break;
    }
    return rv;
}

/*!
  \param pen pen
  \param fm font metrics
  \return the minimum width required to draw the scale
          including the minimum border distance
*/
int QwtScaleDraw::minWidth( const QPen &pen, const QFontMetrics &fm ) const
{
    int rv = 0;
    const int pw = QMAX( 1, pen.width() );  // penwidth can be zero
    const int mlw = maxLabelWidth( fm );

    if ( d_orient == Left || d_orient == Right )
    {
        rv = pw + d_hpad + d_majLen + mlw;
    }
    else
    {
        // Compute the minimum scale length required to draw all ticks
        // plus the first and last major tick labels

        const int nticks = d_scldiv.majCnt() + d_scldiv.minCnt();
        rv = 2 * nticks * pw;
        if ( d_scldiv.majCnt() > 0 )
            rv += mlw;

        // Compute the minimum scale length required to draw all major
        // tick labels for a horizontal scale

        rv = QMAX( rv, d_scldiv.majCnt() * mlw );
    }
    return rv;
}

/*!
  \brief Convert a value into its representing label using the 
         labelFormat. 
  \param value Value
  \return Label string.
  \sa QwtScaleDraw::setLabelFormat()
*/

QString QwtScaleDraw::label(double value) const
{
    QString fmt;
    fmt.sprintf("%%%d.%d%c", d_fieldwidth, d_prec, d_fmt);

    QString text;
    text.sprintf(fmt, value);

    return text;
}

//! \return x origin
int QwtScaleDraw::x() const
{
    return d_xorg;
}

//! \return y origin
int QwtScaleDraw::y() const
{
    return d_yorg;
}
//! \return length
int QwtScaleDraw::length() const
{
    return d_len;
}

//! \return scale orientation 
QwtScaleDraw::Orientation QwtScaleDraw::orientation() const 
{ 
    return d_orient; 
}
