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

#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/grid_property_manager.h>
#include <GsTLAppli/grid/grid_model/point_set_neighborhood.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/math/random_numbers.h>

#include <GsTL/math/math_functions.h>

#include <algorithm>


Named_interface* Point_set::create_new_interface( std::string& size_str ) {
  if( size_str.empty() )
    return new Point_set();
  else {
    int size = String_Op::to_number<int>( size_str );
    appli_assert( size >= 0 );
    return new Point_set( size );
  }
}


//TL modified
bool Point_set::reNameProperty(std::string oldName, std::string newName)
{
	return point_prop_.reNameProperty(oldName, newName);
}

Point_set::Point_set(int size) 
  : point_loc_( size ) {
  
  point_prop_.set_prop_size(size);
}


Point_set::~Point_set()
{
   
}

GsTLGridProperty* Point_set::add_property( const std::string& name )
                                   
{
  return point_prop_.add_property( name );
}


bool Point_set::remove_property(const std::string& name)
{
  return point_prop_.remove_property( name);
}


std::list<std::string> Point_set::property_list() const {
  std::list<std::string> result;

  Grid_property_manager::Property_name_iterator it =
    point_prop_.names_begin();
  Grid_property_manager::Property_name_iterator end =
    point_prop_.names_end();
  for( ; it != end ; ++it )
    result.push_back( *it );

  return result;
}

MultiRealization_property* 
Point_set::add_multi_realization_property( const std::string& name ) {
  return point_prop_.new_multireal_property( name );
}


Neighborhood* Point_set::neighborhood( double x, double y, double z,
				       double ang1, double ang2, double ang3,
				       const Covariance<location_type>* cov,
				       bool ) {
  const int max_neighbors = 20;
  return new Point_set_neighborhood( x,y,z, ang1,ang2,ang3,
				     max_neighbors, this, 
				     point_prop_.selected_property(),
				     cov );
    
}


Neighborhood* Point_set::neighborhood( const GsTLTripletTmpl<double>& dim,
				       const GsTLTripletTmpl<double>& angles,
				       const Covariance<location_type>* cov,
				       bool ) {
  const int max_neighbors = 20;
  return new Point_set_neighborhood( dim[0], dim[1], dim[2],
				     angles[0], angles[1], angles[2], 
				     max_neighbors, this,
				     point_prop_.selected_property(),
				     cov );
}


void Point_set::init_random_path( bool from_scratch ) { 
  if( grid_path_.empty() ) {
    grid_path_.resize( point_loc_.size() );
    from_scratch = true;
  }

  if( from_scratch ) {
    for( int i=0; i < int( grid_path_.size() ); i++ ) 
      grid_path_[i] = i;
  }
  
  STL_generator gen;
  std::random_shuffle( grid_path_.begin(), grid_path_.end(), gen );

} 

Point_set::random_path_iterator 
Point_set::random_path_begin( GsTLGridProperty* prop ) {
  if( int(grid_path_.size()) != point_loc_.size() )  
    init_random_path( true ); 

  GsTLGridProperty* property = prop;
  if( !prop )
    property = point_prop_.selected_property();
 
  return random_path_iterator( this, property,
                  			       0, point_loc_.size(),
			                         TabularMapIndex(&grid_path_) ); 
}


Point_set::random_path_iterator 
Point_set::random_path_end( GsTLGridProperty* prop ) {
  if( int(grid_path_.size()) != point_loc_.size() )  
    init_random_path( true ); 

  GsTLGridProperty* property = prop;
  if( !prop )
    property = point_prop_.selected_property();

  return random_path_iterator( this, property,
                  			       point_loc_.size(), point_loc_.size(),
			                         TabularMapIndex(&grid_path_) ); 

}
 
