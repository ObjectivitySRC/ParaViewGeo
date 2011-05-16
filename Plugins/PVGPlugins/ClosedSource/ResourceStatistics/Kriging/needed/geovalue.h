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

#ifndef __GSTLAPPLI_GRID_MODEL_GEOVALUE_H__ 
#define __GSTLAPPLI_GRID_MODEL_GEOVALUE_H__ 
 


#include "gstlpoint.h"
#include "grid_property.h" 
#include "geostat_grid.h"
 
// This macro makes sure that the geovalue is up-to-date, ie the location 
// it stores is indeed its location. 
//#ifdef GSTLAPPLI_NO_GVAL_DEBUG 
//#define GVAL_ASSERT 
//#else 
//#define GVAL_ASSERT appli_assert( \
//         loc_ == grid_->location(node_id_) ); 
//#endif 


class Geovalue;


class Const_geovalue { 
 
 public: 
  typedef float property_type; 
  typedef GsTLPoint location_type; 
 
 public: 
 
  Const_geovalue(); 
  Const_geovalue( const Const_geovalue& rhs ); 
  Const_geovalue( const Geostat_grid* grid, const GsTLGridProperty* prop,  
	                int node_id ); 
 
  Const_geovalue& operator = ( const Const_geovalue& rhs ); 

  void init( const Geostat_grid* grid, const GsTLGridProperty* prop, 
	           int node_id ); 
 
  const int& node_id() const { return node_id_; } 
  int& node_id() { return node_id_; } 
  void set_node_id( int id ) ; 
 
  void set_property_array( const GsTLGridProperty* prop ); 
  const GsTLGridProperty* property_array() const { return property_array_; } 

  const Geostat_grid* grid() const { return grid_; }
 
  /** This function is there mainly for internal use. It is mostly used to "recycle" a geovalue.
  * The geovalue caches its position in order to optimize calls to Geovalue::location()
  * set_cached_location modifies that location. In debug mode, Geovalue::location() actually
  * checks if its cache is in sync with the actual location. 
  */
  void set_cached_location( const location_type& loc ) { loc_ = loc; } 
 
  bool operator == ( const Const_geovalue& rhs ) const;  
  bool operator != ( const Const_geovalue& rhs ) const;  

  bool is_harddata() const { return property_array_->is_harddata( node_id_ ); }


  //---------- 
  // GsTL requirements for concept Geovalue 
 public: 
  bool is_informed() const {  
    if( node_id_ < 0 ) 
      return false; 
     
    //appli_assert( node_id_ < property_array_->size() ); 
    return ( values_array_[ node_id_ ] != GsTLGridProperty::no_data_value ); 
  } 
   
  property_type property_value() const {  
    //appli_assert( node_id_ >= 0 && node_id_ < property_array_->size() ); 
    return values_array_[ node_id_ ]; 
  } 
   
  const location_type& location() const {  
    if( loc_.x() == invalid_coord_ )
      loc_ = grid_->location( node_id_ );

    //GVAL_ASSERT
    return loc_; 
  } 
 
  //---------- 
 
 
 private: 
  static const location_type::coordinate_type invalid_coord_;
   
  const Geostat_grid* grid_; 
  const GsTLGridProperty* property_array_; 
  const float* values_array_; 
  int node_id_; 
  mutable location_type loc_; 
 
}; 




//========================================
 
/** Geovalue is a model of GsTL concept Geovalue. 
 * Geovalue is common to all types of grids. 
 * A geovalue refers to a given node of a grid. A non-valid 
 * geovalue is a geovalue that refers to a node which is not 
 * part of the grid. This is coded by a node_id < 0. 
 * A non-valid geovalue can only be queried by is_informed(). 
 * Members location(), property_value(), etc, would yield meaningless results. 
 * 
 * Implementation notes: Geovalue is designed to be used by geostatistics  
 * algorithm, hence is implemented so as to optimize performance. The grid 
 * property supplied to the Geovalue constructor has to be loaded into memory 
 * for the geovalue to work. Geovalue directly accesses the data array of the  
 * grid property to bypass the virtual function calls in GsTLGridProperty. 
 */    
 
class Geovalue { 
 
 public: 
  typedef float property_type; 
  typedef GsTLPoint location_type; 
 
 public: 
 
  Geovalue(); 
  Geovalue( const Geovalue& rhs ); 
  Geovalue( Geostat_grid* grid, GsTLGridProperty* prop,  
	    int node_id ); 
 
  Geovalue& operator = ( const Geovalue& rhs ); 
  operator Const_geovalue() { 
    return Const_geovalue( grid_, property_array_, node_id_ );
  }

  void init( Geostat_grid* grid, GsTLGridProperty* prop, 
	     int node_id ); 
 
  const int& node_id() const { return node_id_; } 
  int& node_id() { return node_id_; } 
  void set_node_id( int id ) ; 
 
  void set_property_array( GsTLGridProperty* prop ); 
  const GsTLGridProperty* property_array() const { return property_array_; } 

  const Geostat_grid* grid() const { return grid_; }
 
  void set_cached_location( const location_type& loc ) { loc_ = loc; } 
 
  bool operator == ( const Geovalue& rhs ) const;  
  bool operator != ( const Geovalue& rhs ) const;  

  bool is_harddata() const { return property_array_->is_harddata( node_id_ ); }

  void set_not_informed() { property_array_->set_not_informed(node_id_); }

  //---------- 
  // GsTL requirements for concept Geovalue 
 public: 
  bool is_informed() const {  
    if( node_id_ < 0 ) 
      return false; 
     
    //appli_assert( node_id_ < property_array_->size() ); 
    return ( values_array_[ node_id_ ] != GsTLGridProperty::no_data_value ); 
  } 
   
  property_type property_value() const {  
    //appli_assert( node_id_ >= 0 && node_id_ < property_array_->size() ); 
    return values_array_[ node_id_ ]; 
  } 
  void set_property_value( const property_type& val ) { 
    //appli_assert( node_id_ >= 0 && node_id_ < property_array_->size() ); 
    values_array_[ node_id_ ] = val;     
  } 
   
  const location_type& location() const {  
    if( loc_.x() == invalid_coord_ )
      loc_ = grid_->location( node_id_ );

    //GVAL_ASSERT 
    return loc_; 
  } 
 
  //---------- 
 
 
 private: 
  static const location_type::coordinate_type invalid_coord_;
   
  Geostat_grid* grid_; 
  GsTLGridProperty* property_array_; 
  float* values_array_; 
  int node_id_; 
  mutable location_type loc_; 
}; 
 



 
#endif 
