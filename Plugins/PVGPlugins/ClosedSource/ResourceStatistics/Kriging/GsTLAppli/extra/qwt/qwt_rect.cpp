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

#include "qwt_rect.h"

//! Constructor

QwtRect::QwtRect(): 
    QRect() 
{
}

//! Copy constructor
QwtRect::QwtRect(const QRect &r): 
    QRect(r) 
{
}

QRect QwtRect::cutTop(int h, int distTop, int distBottom)
{
    QRect rv;
    rv.setTop(top() + distTop);
    rv.setHeight(h);
    setTop(rv.bottom() + distBottom + 1);
    rv.setLeft(left());
    rv.setRight(right());
    return rv;
}


QRect QwtRect::cutBottom(int h, int distTop, int distBottom)
{
    QRect rv;
    setBottom(bottom() - h - distBottom - distTop);
    rv.setTop(bottom() + 1 + distTop);
    rv.setHeight(h);
    rv.setLeft(left());
    rv.setRight(right());
    return rv;
}

QRect QwtRect::cutLeft(int w, int distLeft, int distRight)
{
    QRect rv;
    rv.setLeft(left() + distLeft);
    rv.setWidth(w);
    setLeft(rv.right() + distRight + 1);
    rv.setTop(top());
    rv.setBottom(bottom());
    return rv;
}


QRect QwtRect::cutRight(int w, int distLeft, int distRight)
{
    QRect rv;
    setRight(right() - w - distRight - distLeft);
    rv.setLeft(right() + 1 + distLeft);
    rv.setWidth(w);
    rv.setTop(top());
    rv.setBottom(bottom());
    return rv;
}

const QwtRect& QwtRect::cutMargin(int mLeft, int mRight, int mTop, int mBottom)
{
    setHeight(height() - mTop - mBottom);
    setWidth(width() - mLeft - mRight);
    moveBy(mLeft, mTop);
    return *this;
}
