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

#include <GsTLAppli/grid/grid_model/grid_initializer.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h>

#include <GsTL/geometry/geometry_algorithms.h>

#include <vector>
#include <map>


/*
Grid_initializer::Grid_initializer( Geostat_grid* target,
                                    bool from_scratch , bool overwrite,
                                    bool mark_as_hard )
  : source_( 0 ) {

  target_grid( target );

  from_scratch_ = from_scratch;
  overwrite_ = overwrite;
  mark_as_hard_ = mark_as_hard;

}

 
void Grid_initializer::target_grid( Geostat_grid* target ) {
  target_ = dynamic_cast<Cartesian_grid*>( target ); 
  appli_assert( target && 
	       "Grid_initializer currently only works with cartesian grids" );
}
*/


/** Implementation note:
 * A data is assigned to a grid cell if it lies inside the grid bounding box.
 * That is easy to implement for a cartesian grid, but for a stratigraphic
 * grid, it is more difficult to describe the bounding box. 
 * We actually don't need to know the bbox. All we have to do is test if the
 * point lies within the cell it is closest to. 
 */
/*
bool Grid_initializer::assign( GsTLGridProperty* target_property,
			                         const Geostat_grid* source,
			                         const std::string& property_name ) {

  const Point_set* source_pset = dynamic_cast< const Point_set* >( source );
  appli_assert( source_pset &&
	       "Grid_initializer currently only works with point sets" );

  // check if we alrady worked with "source" and "property_name" 
  // If that's the case and we're not required to do the assignement
  // from scratch, use what we did last time.
  if( !from_scratch_ && source == source_ &&
      property_name == source_property_name_ ) {
    
    const GsTLGridProperty* source_property = source_pset->property( property_name );
    for( unsigned int i=0; i < last_assignements_.size() ; i++ ) {
      float val = source_property->get_value( last_assignements_[i].first );
      target_property->set_value( val, last_assignements_[i].second );
      if( mark_as_hard_ )
      	target_property->set_harddata( true, last_assignements_[i].second );
    } 
    return true;
  }
  


  last_assignements_.clear();
  source_ = source;
  source_property_name_ = property_name;

  typedef Point_set::location_type location_type;
  typedef std::vector<location_type> Location_vector;
  typedef std::vector<location_type>::const_iterator const_iterator;

  // We will need to obtain the coordinates of a grid node from its
  // node id. Hence we need a grid-cursor, set to multigrid level 1.
  SGrid_cursor cursor = *target_->cursor();
  cursor.set_multigrid_level( 1 );

  const GsTLGridProperty* source_property = source_pset->property( property_name );
  const Location_vector& locations = source_pset->point_locations();

  GsTL_cube bbox = target_->bounding_box();
  GsTLInt current_id = 0;

  // Use a map to record what point was assigned to which grid node
  // This map is used in case multiple points could be assigned to the
  // same grid node: in that case the new point is assigned if it is closer
  // to the grid node than the previously assigned node was.
  typedef std::map<GsTLInt,location_type>::iterator map_iterator;
  std::map<GsTLInt,location_type> already_assigned;


  for( const_iterator loc_ptr = locations.begin(); loc_ptr != locations.end(); 
       ++loc_ptr, current_id++ ) {

    if( !source_property->is_informed( current_id ) ) continue;

    // only consider the points inside the target's bounding box
    if( !bbox.contains( *loc_ptr ) ) continue;


    GsTLInt node_id = target_->closest_node( *loc_ptr );
    appli_assert( node_id >=0 && node_id < target_property->size() );
    appli_assert( current_id < source_property->size() );
            
    // If there is already a property value (not assigned by the
    // grid initializer), and we don't want to overwrite, leave it alone
    if( !overwrite_ && target_property->is_informed( node_id ) ) continue;


    bool perform_assignment = true;

    // check if a point was already assigned to that node
    map_iterator it = already_assigned.find( node_id );
    if( it != already_assigned.end() ) {
    	int i,j,k;
    	GsTLCoordVector sizes = target_->cell_dimensions();
     	cursor.coords( node_id, i,j,k );
     	location_type node_loc( float(i)*sizes.x(),
			float(j)*sizes.y(),
			float(k)*sizes.z() );

     	// if the new point is further away to the grid node than
     	// the already assigned node, don't assign the new point
     	if( square_euclidean_distance( node_loc, *loc_ptr ) > 
          square_euclidean_distance( node_loc, it->second ) ) 
        perform_assignment = false;
    }
	
    if( perform_assignment ) {
    	// store the node id of the source and of the target:
     	last_assignements_.push_back( std::make_pair( current_id, node_id ) );
      float val = source_property->get_value( current_id ); 
      target_property->set_value( val, node_id );
      already_assigned[node_id] = *loc_ptr;
      if( mark_as_hard_ )
        target_property->set_harddata( true, node_id );
    }
  }
  
  return true;
}

*/

/*
bool Grid_initializer::assign( GsTLGridProperty* target_property,
			                         const Geostat_grid* source,
			                         const std::string& property_name ) {

  const Point_set* source_pset = dynamic_cast< const Point_set* >( source );
  appli_assert( source_pset &&
	       "Grid_initializer currently only works with point sets" );

  // check if we alrady worked with "source" and "property_name" 
  // If that's the case and we're not required to do the assignement
  // from scratch, use what we did last time.
  if( !from_scratch_ && source == source_ &&
      property_name == source_property_name_ ) {
    
    const GsTLGridProperty* source_property = source_pset->property( property_name );
    for( unsigned int i=0; i < last_assignements_.size() ; i++ ) {
      float val = source_property->get_value( last_assignements_[i].first );
      target_property->set_value( val, last_assignements_[i].second );
      if( mark_as_hard_ )
      	target_property->set_harddata( true, last_assignements_[i].second );
    } 
    return true;
  }
  


  last_assignements_.clear();
  source_ = source;
  source_property_name_ = property_name;

  typedef Point_set::location_type location_type;
  typedef std::vector<location_type> Location_vector;
  typedef std::vector<location_type>::const_iterator const_iterator;

  // We will need to obtain the coordinates of a grid node from its
  // node id. Hence we need a grid-cursor, set to multigrid level 1.
  SGrid_cursor cursor = *target_->cursor();
  cursor.set_multigrid_level( 1 );

  const GsTLGridProperty* source_property = source_pset->property( property_name );
  const Location_vector& locations = source_pset->point_locations();

  GsTL_cube bbox = target_->bounding_box();
  GsTLInt current_id = 0;

  // Use a map to record what point was assigned to which grid node
  // This map is used in case multiple points could be assigned to the
  // same grid node: in that case the new point is assigned if it is closer
  // to the grid node than the previously assigned node was.
  typedef std::map<GsTLInt,location_type>::iterator map_iterator;
  std::map<GsTLInt,location_type> already_assigned;


  for( const_iterator loc_ptr = locations.begin(); loc_ptr != locations.end(); 
       ++loc_ptr, current_id++ ) {

    if( !source_property->is_informed( current_id ) ) continue;

    // only consider the points inside the target's bounding box
    if( bbox.contains( *loc_ptr ) ) {
      GsTLInt node_id = target_->closest_node( *loc_ptr );
      appli_assert( node_id >=0 && node_id < target_property->size() );
      appli_assert( current_id < source_property->size() );
            
      // If there is already a property value (not assigned by the
      // grid initializer), and we don't want to overwrite, leave it alone
      if( !overwrite_ && target_property->is_informed( node_id ) )
      	continue;


      bool perform_assignment = true;

      // check if a point was already assigned to that node
      map_iterator it = already_assigned.find( node_id );
      if( it != already_assigned.end() ) {
	int i,j,k;
	GsTLCoordVector sizes = target_->cell_dimensions();
	cursor.coords( node_id, i,j,k );
	location_type node_loc( float(i)*sizes.x(),
				float(j)*sizes.y(),
				float(k)*sizes.z() );

	// if the new point is further away to the grid node than
	// the already assigned node, don't assign the new point
	if( square_euclidean_distance( node_loc, *loc_ptr ) > 
	    square_euclidean_distance( node_loc, it->second ) ) 
	  perform_assignment = false;
      }
	
      if( perform_assignment ) {
	// store the node id of the source and of the target:
	last_assignements_.push_back( std::make_pair( current_id, node_id ) );
	float val = source_property->get_value( current_id ); 
	target_property->set_value( val, node_id );
	already_assigned[node_id] = *loc_ptr;
	if( mark_as_hard_ )
	  target_property->set_harddata( true, node_id );
      }
    }
  }

  return true;
}
*/
/*
bool Grid_initializer::undo_assign() {
  appli_warning( "NOT YET IMPLEMENTED" );
  return false;
}
*/
