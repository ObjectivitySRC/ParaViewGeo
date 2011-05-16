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

#ifndef QWT_H
#define QWT_H

#include "qwt_global.h"

// kobasoft:
// What's this for?
#undef Left
#undef Right
#undef Top
#undef Bottom
#undef Center

/*!
  Some constants for use within Qwt.
*/
class QWT_EXPORT Qwt 
{
public:
  /*!
    \brief The position of a widget.
    These constants can be OR'ed together.
  */
  enum Position {
    Left = 0x01, 
    Right = 0x02, 
    Top = 0x04, 
    Bottom = 0x08, 
    Center = 0x10
  };
  
  /*!
    \todo Documentation
  */
  enum Shape
    {
      HLine,
      VLine,
      Cross,
      DiagCross,
      Rect,
      Ellipse,
      Triangle,
      UTriangle,
      DTriangle,
      LTriangle,
      RTriangle
    };
  
  /*!
    \todo Documentation
  */
  enum Errors {
    ErrNone, // No Error
    ErrNoMem, // Not enough memory
    ErrMono // Non-monotonic sequence
  };
};

#endif
