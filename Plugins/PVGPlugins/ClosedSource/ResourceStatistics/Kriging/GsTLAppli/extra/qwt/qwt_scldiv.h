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

#ifndef QWT_SCLDIV_H
#define QWT_SCLDIV_H

#include <qmemarray.h>
#include "qwt_global.h"

#if defined(QWT_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QWT_EXPORT QMemArray<double>;
// MOC_SKIP_END
#endif

/*!
  \brief A class for building scale divisions
  
  The QwtScaleDiv class can build
  linear and logarithmic scale divisions for specified
  intervals. It uses an adjustable algorithm to
  generate the major and minor step widths automatically.
  A scale division has a minimum value, a maximum value,
  a vector of major marks, and a vector of minor marks.

  \warning QwtScaleDiv uses implicit sharing for the mark vectors.
  \par Example
  To build a logarithmic scale division from 0.01 to 1000
  and print out the major and minor marks. \verbatim
#include "../include/qwt_scldiv.h>
#include <iostream.h>

main()
{
  int i,k;
  QwtScaleDiv sd;

  sd.rebuild(0.01, 100, 10, 10, TRUE, 0.0);

  k=0;
  for (i=0;i<sd.majCnt();i++)
  {
    while(k < sd.minCnt())
    {
      if (sd.minMark(k) < sd.majMark(i))
      {
        cout << " - " << sd.minMark(i) << "\n";
    k++;
      }
      else
        break;
    }
    cout << "-- " << sd.majMark(i) << "\n";
  }
  while(k < sd.minCnt())
  {
    cout << " - " << sd.minMark(i) << "\n";
    k++;
  }
}\endverbatim
*/

class QWT_EXPORT QwtScaleDiv
{
public:
    QwtScaleDiv ();
    QwtScaleDiv(const QwtScaleDiv& s);

    QwtScaleDiv& operator= (const QwtScaleDiv &s);
    int operator== (const QwtScaleDiv &s) const;
    int operator!= (const QwtScaleDiv &s) const;
    
    /*! 
        \return left bound
        \sa QwtScaleDiv::rebuild
     */
    double lBound() const { return d_lBound; }
    /*! 
        \return right bound
        \sa QwtScaleDiv::rebuild
     */
    double hBound() const { return d_hBound; }
    /*! 
        \return minor mark count
        \sa QwtScaleDiv::rebuild
     */
    int minCnt() const { return d_minMarks.size(); }
    /*! 
        \return major mark count
        \sa QwtScaleDiv::rebuild
     */
    int majCnt() const { return d_majMarks.size(); }

    /*! 
        \return TRUE id ths scale div is logarithmic
        \sa QwtScaleDiv::rebuild
     */
    bool logScale() const { return d_log; }

    //! \return major mark at position i
    double majMark(int i) const { return d_majMarks[i]; }
    //! \return minor mark at position i
    double minMark(int i) const { return d_minMarks[i]; }

    /*! 
        \return major step size
        \sa QwtScaleDiv::rebuild
     */
    double majStep() const { return d_majStep; }
    void reset();

    bool rebuild(double lBound, double hBound, int maxMaj, int maxMin,
             bool log, double step = 0.0, bool ascend = TRUE);
 
private:
    void copy(const QwtScaleDiv &s);
    bool buildLinDiv(int maxMajMark, int maxMinMark, double step = 0.0);
    bool buildLogDiv(int maxMajMark, int maxMinMark, double step = 0.0);
   
    double d_lBound;
    double d_hBound;
    double d_majStep;
    bool d_log;

    QMemArray<double> d_majMarks;
    QMemArray<double> d_minMarks;
};

#endif
