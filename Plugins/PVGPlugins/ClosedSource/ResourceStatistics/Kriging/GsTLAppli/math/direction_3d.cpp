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


#include <GsTLAppli/math/direction_3d.h>

#include <GsTL/math/math_functions.h>
#include <GsTL/geometry/geometry_algorithms.h>

#include <cmath>
#include <numeric>

Direction_3d::Direction_3d() {
  use_tolerance_ = false;
  angle_tol_ = 0;
  cone_height_ = 0;
  bandwidth_sq_ = 0;
}
 
void Direction_3d::set_direction( const GsTLVector<float>& v ) {
  dir_ = v;

  // normalize dir_
  float norm = euclidean_norm( dir_ );
  dir_.x() = dir_.x() / norm;
  dir_.y() = dir_.y() / norm;
  dir_.z() = dir_.z() / norm;
}
 
void Direction_3d::set_direction( float alpha, float beta ) {
  dir_.x() = cos( beta ) * cos( alpha );
  dir_.y() = cos( beta ) * sin( alpha );
  dir_.z() = sin( beta );
}

void Direction_3d::set_tolerance( float angle_tol, float cone_height ) {
  use_tolerance_ = true;
  angle_tol_ = angle_tol;
  if( angle_tol >= GsTL::PI/2 ) {
    cone_height_ = -1;
    bandwidth_sq_ = -1;
  }
  else {
    cone_height_ = cone_height;
    bandwidth_sq_ = cone_height_ * tan( angle_tol );
    bandwidth_sq_ *= bandwidth_sq_; //square it
  }
}

bool Direction_3d::is_colinear( const GsTLVector<float>& v ) const {
  if( use_tolerance_ ) {
    if( angle_tol_ >= GsTL::PI/2 ) return true;

    float h = dir_ * v;
    float d2 = square_euclidean_norm( v ) - h*h;
    if( h > cone_height_ ) 
      return d2 < bandwidth_sq_;
    else {
      float dmax = h * tan( angle_tol_ );
      return d2 < dmax*dmax;
    }
  }
  else {
    float l = v.x() / dir_.x();
    if( ! GsTL::equals( l, v.y() / dir_.y() ) ) return false; 
    if( ! GsTL::equals( l, v.z() / dir_.z() ) ) return false;
    return true;
  }
    
  return false;
}
