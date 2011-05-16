/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "math" module of the Geostatistical Earth
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

#include <GsTLAppli/math/box.h>

GsTL_cube::GsTL_cube( const GsTLPoint& lower, const GsTLPoint& upper )
  : GsTL_box(),
    lower_( lower ),
    upper_( upper ) {}

GsTL_cube::GsTL_cube( const GsTL_cube& rhs ) 
  : GsTL_box(),
    lower_( rhs.lower_ ),
    upper_( rhs.upper_ ) {}

GsTL_cube& GsTL_cube::operator = ( const GsTL_cube& rhs ) {
  lower_ = rhs.lower_;
  upper_ = rhs.upper_;

  return *this;
}

void GsTL_cube::set_geometry( const GsTLPoint& lower, const GsTLPoint& upper ) {
  lower_ = lower;
  upper_ = upper;
}


void GsTL_cube::geometry( GsTLPoint& lower, GsTLPoint& upper ) {
  lower = lower_;
  upper = upper_;
}

bool GsTL_cube::contains( const GsTLPoint& P ) const {
  if( P.x() < lower_.x() ) return false;
  if( P.y() < lower_.y() ) return false;
  if( P.z() < lower_.z() ) return false;
  
  if( P.x() > upper_.x() ) return false;
  if( P.y() > upper_.y() ) return false;
  if( P.z() > upper_.z() ) return false;

  return true;
}
