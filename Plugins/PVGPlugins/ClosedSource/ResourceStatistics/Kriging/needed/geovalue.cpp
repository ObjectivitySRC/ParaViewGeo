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

#include "geovalue.h"


const Geovalue::location_type::coordinate_type Geovalue::invalid_coord_ = -9.9e30;


Geovalue::Geovalue() 
  : grid_(0), property_array_(0), values_array_(0),
    node_id_(-1),
    loc_( invalid_coord_, invalid_coord_, invalid_coord_ ) {
  
}

Geovalue::Geovalue( const Geovalue& rhs ) 
  : grid_( rhs.grid_ ), 
    property_array_( rhs.property_array_ ),
    values_array_( rhs.values_array_ ),
    node_id_( rhs.node_id_ ),
    loc_( rhs.loc_ ) {
}


Geovalue::Geovalue( Geostat_grid* grid, GsTLGridProperty* prop,
		    int node_id )
  : grid_(grid), property_array_(prop), 
    node_id_(node_id),
    loc_( invalid_coord_, invalid_coord_, invalid_coord_ ) {

  //appli_assert( prop );
  values_array_ = prop->data() ;
  if( !values_array_ ) {
    prop->swap_to_memory();
    values_array_ = prop->data() ;
  }
  //appli_assert( values_array_ );
}

Geovalue& Geovalue::operator = ( const Geovalue& rhs ) {
  if( *this != rhs ) {
  grid_ = rhs.grid_ ;
  property_array_ = rhs.property_array_ ;
  values_array_ = rhs.values_array_ ;
  node_id_ = rhs.node_id_ ;
  loc_ = rhs.loc_ ;
  }

  return *this;
}


void Geovalue::init( Geostat_grid* grid, GsTLGridProperty* prop,
		     int node_id) {
  //appli_assert( prop );
  grid_ = grid;
  property_array_ = prop;
  values_array_ = prop->data();
  if( !values_array_ ) {
    prop->swap_to_memory();
    values_array_ = prop->data() ;
  }
  //appli_assert( values_array_ );

  node_id_ = node_id;  
  //loc_ = grid->location( node_id );
}


void Geovalue::set_property_array( GsTLGridProperty* prop ) {
  //appli_assert( prop );
  property_array_ = prop;
  values_array_ = prop->data();
  if( !values_array_ ) {
    prop->swap_to_memory();
    values_array_ = prop->data() ;
  }
  //appli_assert( values_array_ );
}

void Geovalue::set_node_id( int id ) {
  if( node_id_ != id ) {
    node_id_ = id;
    //loc_ = grid_->location( id );

    if( loc_.x() != invalid_coord_ ) {
      loc_.x() = invalid_coord_;
      loc_.y() = invalid_coord_;
      loc_.z() = invalid_coord_;
    }
  }
}


bool Geovalue::operator == ( const Geovalue& rhs ) const {
  return ( grid_ == rhs.grid_ &&
	   property_array_ == rhs.property_array_ &&
	   values_array_ == rhs.values_array_ &&
	   node_id_ == rhs.node_id_ ); //&&
	   //loc_ == rhs.loc_ );
}


bool Geovalue::operator != ( const Geovalue& rhs ) const {
  return !( *this == rhs );
}





//===================================

const Const_geovalue::location_type::coordinate_type 
  Const_geovalue::invalid_coord_ = -9.9e30;

Const_geovalue::Const_geovalue() 
  : grid_(0), property_array_(0), values_array_(0),
    node_id_(-1),
    loc_( invalid_coord_, invalid_coord_, invalid_coord_ ) {
  
}

Const_geovalue::Const_geovalue( const Const_geovalue& rhs ) 
  : grid_( rhs.grid_ ), 
    property_array_( rhs.property_array_ ),
    values_array_( rhs.values_array_ ),
    node_id_( rhs.node_id_ ),
    loc_( rhs.loc_ ) {
}


Const_geovalue::Const_geovalue( const Geostat_grid* grid,
                                const GsTLGridProperty* prop,
		                            int node_id )
  : grid_(grid), property_array_(prop), 
    node_id_(node_id),
    loc_( invalid_coord_, invalid_coord_, invalid_coord_ ) {

  //appli_assert( prop );
  values_array_ = prop->data() ;
  if( !values_array_ ) {
    prop->swap_to_memory();
    values_array_ = prop->data() ;
  }
  //appli_assert( values_array_ );
}

Const_geovalue& Const_geovalue::operator = ( const Const_geovalue& rhs ) {
  if( *this != rhs ) {
    grid_ = rhs.grid_ ;
    property_array_ = rhs.property_array_ ;
    values_array_ = rhs.values_array_ ;
    node_id_ = rhs.node_id_ ;
    loc_ = rhs.loc_ ;
  }

  return *this;
}


void Const_geovalue::init( const Geostat_grid* grid, const GsTLGridProperty* prop,
		                 int node_id) {
  //appli_assert( prop );
  grid_ = grid;
  property_array_ = prop;
  values_array_ = prop->data();
  if( !values_array_ ) {
    prop->swap_to_memory();
    values_array_ = prop->data() ;
  }
  //appli_assert( values_array_ );

  node_id_ = node_id;  
  //loc_ = grid->location( node_id );
}


void Const_geovalue::set_property_array( const GsTLGridProperty* prop ) {
  //appli_assert( prop );
  property_array_ = prop;
  values_array_ = prop->data();
  if( !values_array_ ) {
    prop->swap_to_memory();
    values_array_ = prop->data() ;
  }
  //appli_assert( values_array_ );
}

void Const_geovalue::set_node_id( int id ) {
  if( node_id_ != id ) {
    node_id_ = id;
    //loc_ = grid_->location( id );

    if( loc_.x() != invalid_coord_ ) {
      loc_.x() = invalid_coord_;
      loc_.y() = invalid_coord_;
      loc_.z() = invalid_coord_;
    }
  }
}


bool Const_geovalue::operator == ( const Const_geovalue& rhs ) const {
  return ( grid_ == rhs.grid_ &&
	   property_array_ == rhs.property_array_ &&
	   values_array_ == rhs.values_array_ &&
	   node_id_ == rhs.node_id_ ); //&&
	   //loc_ == rhs.loc_ );
}


bool Const_geovalue::operator != ( const Const_geovalue& rhs ) const {
  return !( *this == rhs );
}
