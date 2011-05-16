/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
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

#include <GsTLAppli/grid/grid_model/rgrid_geometry.h>

#include <GsTL/math/math_functions.h>

RGrid_geometry::~RGrid_geometry() {
}


//***********************************************


Simple_RGrid_geometry::Simple_RGrid_geometry() 
  : RGrid_geometry(), 
    origin_( 0,0,0 ) {
}


Simple_RGrid_geometry::~Simple_RGrid_geometry() {
}

RGrid_geometry* Simple_RGrid_geometry::clone() const {
  Simple_RGrid_geometry* geom = new Simple_RGrid_geometry;
  geom->set_size( 0, n_[0] );
  geom->set_size( 1, n_[1] );
  geom->set_size( 2, n_[2] );
  geom->set_origin( origin_ );
  geom->set_cell_dims( cell_dims_ );

  return geom;
}


GsTLPoint Simple_RGrid_geometry::coordinates(GsTLInt i, GsTLInt j, GsTLInt k) {
  GsTLCoord x = origin_.x() + GsTLCoord(i)*cell_dims_.x() ; 
  GsTLCoord y = origin_.y() + GsTLCoord(j)*cell_dims_.y() ; 
  GsTLCoord z = origin_.z() + GsTLCoord(k)*cell_dims_.z() ; 

  return GsTLPoint(x, y, z);
}


bool Simple_RGrid_geometry::
grid_coordinates( GsTLGridNode& ijk, const GsTLPoint& p ) const {
  GsTLPoint::difference_type translated = p - origin_;
  ijk[0] = GsTL::floor( translated.x() / float( cell_dims_.x() ) );
  ijk[1] = GsTL::floor( translated.y() / float( cell_dims_.y() ) );
  ijk[2] = GsTL::floor( translated.z() / float( cell_dims_.z() ) );

  return true;
}
 
