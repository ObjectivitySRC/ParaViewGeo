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

#include <GsTLAppli/grid/grid_model/property_copier.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

#include <GsTL/geometry/geometry_algorithms.h>
#include <qmessagebox.h>

#include <map>

std::string propertyCopier_manager = gridObject_manager + "/PropertyCopier";


SmartPtr<Property_copier> 
Property_copier_factory::get_copier( const Geostat_grid* server, 
                                     const Geostat_grid* client ) {
  std::string type = create_type_name( server->classname(),
                                       client->classname() );
 
  SmartPtr<Named_interface> ni =
    Root::instance()->new_interface( type, propertyCopier_manager + "/" );
  
  Property_copier* copier = dynamic_cast< Property_copier* >( ni.raw_ptr() );
  return SmartPtr<Property_copier>( copier );

}
 
  
bool Property_copier_factory::add_method( const std::string& server_type, 
                                          const std::string& client_type,
                                          CallBackType method ) {
  std::string type = create_type_name( server_type, client_type );
  
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( propertyCopier_manager );

  Manager* mng = dynamic_cast< Manager* >( ni.raw_ptr() );
  
  if( !mng ) {
    SmartPtr<Named_interface> ni2 = 
      Root::instance()->new_interface( "directory", propertyCopier_manager ); 
    mng = dynamic_cast< Manager* >( ni2.raw_ptr() );
  }

  if( !mng ) return false;
  
  return mng->factory( type, method );
}


std::string 
Property_copier_factory::create_type_name( const std::string& server_type, 
                                           const std::string& client_type) {
  return ( server_type + ";;" + client_type );
}





//==========================================

Property_copier::Property_copier() {
  from_scratch_ = true;
  overwrite_ = false;
  mark_as_hard_ = true;
  undo_enabled_ = false;
}



//==========================================
Mask_to_mask_copier::Mask_to_mask_copier()
{
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  
  unset_harddata_flag_ = true;

}

bool Mask_to_mask_copier::undo_copy()
{
	return true;
}

bool Mask_to_mask_copier::copy( const Geostat_grid* server, 
                                 const GsTLGridProperty* server_prop,
                                 Geostat_grid* client, 
                                 GsTLGridProperty* client_prop ) 
{
	Reduced_grid* to_grid = dynamic_cast< Reduced_grid* >( client );
	const Reduced_grid* from_grid = dynamic_cast< const Reduced_grid* >( server );

	if( !from_grid || !to_grid ) return false;

	if (from_grid != to_grid)
		to_grid->copyStructure(from_grid);

	for( int i=0; i < server_prop->size() ; i++ ) {
		if( server_prop->is_informed( i ) ) {
			client_prop->set_value( server_prop->get_value( i ), i );
			if( mark_as_hard_ )   //added by Yongshe
				client_prop->set_harddata( true, i); //added by Yongshe
		}
		else if(overwrite_)
			client_prop->set_not_informed( i );
	}
	return true;
}
								 
									 
//===========================================

Cgrid_to_pset_copier::Cgrid_to_pset_copier() 
  : Property_copier() {
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  
  unset_harddata_flag_ = true;

}

bool Cgrid_to_pset_copier::copy( const Geostat_grid* server, 
                                 const GsTLGridProperty* server_prop,
                                 Geostat_grid* client, 
                                 GsTLGridProperty* client_prop ) {
  Cartesian_grid* cgrid = dynamic_cast< Cartesian_grid* >( client );
  const Point_set* pset = dynamic_cast< const Point_set* >( server );

  if( !cgrid || !pset ) return false;

  typedef GsTLGridProperty::property_type Property_type;

  // check if we already worked with "source" and "property_name" 
  // If that's the case and we're not required to do the assignement
  // from scratch, use what we did last time.
  if( !from_scratch_ && server == server_ &&
      server_prop == server_prop_ && client == client_ ) {
    
    for( unsigned int i = 0 ; i < last_assignement_.size() ; i++ ) {
      Property_type val = server_prop->get_value( last_assignement_[i].first );
      client_prop->set_value( val, last_assignement_[i].second );
      if( mark_as_hard_ )
      	client_prop->set_harddata( true, last_assignement_[i].second );
    } 
    return true;
  }
  
  last_assignement_.clear();
  backup_.clear();
  server_ = server;
  server_prop_ = server_prop;
  client_ = client;
  client_property_ = client_prop;

  typedef Point_set::location_type location_type;
  typedef std::vector<location_type> Location_vector;
  typedef std::vector<location_type>::const_iterator const_iterator;

  // We will need to obtain the coordinates of a grid node from its
  // node id. Hence we need a grid-cursor, set to multigrid level 1.
  SGrid_cursor cursor = *( cgrid->cursor() );
  cursor.set_multigrid_level( 1 );

  const Location_vector& locations = pset->point_locations();

  GsTL_cube bbox = cgrid->bounding_box();
  GsTLInt current_id = 0;

  // Use a map to record what point was assigned to which grid node
  // This map is used in case multiple points could be assigned to the
  // same grid node: in that case the new point is assigned if it is closer
  // to the grid node than the previously assigned node was.
  typedef std::map<GsTLInt,location_type>::iterator map_iterator;
  std::map<GsTLInt,location_type> already_assigned;

  overwrite_ = true;   // added by Jianbing Wu, 03/20/2006

  for( const_iterator loc_ptr = locations.begin(); loc_ptr != locations.end(); 
       ++loc_ptr, current_id++ ) {

    if( !server_prop->is_informed( current_id ) ) continue;

    // only consider the points inside the target's bounding box
    if( !bbox.contains( *loc_ptr ) ) continue;


    GsTLInt node_id = cgrid->closest_node( *loc_ptr );
    appli_assert( node_id >=0 && node_id < client_prop->size() );
    appli_assert( current_id < server_prop->size() );

    // If there is already a property value (not assigned by the
    // grid initializer), and we don't want to overwrite, leave it alone
    if( !overwrite_ && client_prop->is_informed( node_id ) ) continue;

//    bool perform_assignment = true;

    // check if a point was already assigned to that node
    map_iterator it = already_assigned.find( node_id );
    if( it != already_assigned.end() ) {
    	int i,j,k;
    	GsTLCoordVector sizes = cgrid->cell_dimensions();
     	cursor.coords( node_id, i,j,k );
     	location_type node_loc( float(i)*sizes.x(),
			                        float(j)*sizes.y(),
			                        float(k)*sizes.z() );

     	// if the new point is further away to the grid node than
     	// the already assigned node, don't assign the new point
     	if( square_euclidean_distance( node_loc, *loc_ptr ) > 
          square_euclidean_distance( node_loc, it->second ) ) 
        continue; //perform_assignment = false;
    }

//    if( perform_assignment ) {
  	// store the node id of the source and of the target and make a backup of the
    // property value of the client property
   	last_assignement_.push_back( std::make_pair( current_id, node_id ) );

    if( !undo_enabled_ ) {
      Property_type backup_val = GsTLGridProperty::no_data_value;
      if( client_prop->is_informed( node_id ) )
        backup_val = client_prop->get_value( node_id );
      backup_.push_back( std::make_pair( node_id, backup_val ) );
    }

    Property_type val = server_prop->get_value( current_id ); 
    client_prop->set_value( val, node_id );
    already_assigned[node_id] = *loc_ptr;
    if( mark_as_hard_ )
        client_prop->set_harddata( true, node_id );

    }

  overwrite_ = false;   // added by Jianbing Wu, 03/20/2006

  unset_harddata_flag_ = mark_as_hard_;
  return true;
}




bool Cgrid_to_pset_copier::undo_copy() {
  //if( !undo_enabled_ ) return false;

  for( unsigned int i = 0 ; i < backup_.size() ; i++ ) {
    client_property_->set_value( backup_[i].second, backup_[i].first );
    if( unset_harddata_flag_ ) 
      client_property_->set_harddata( false, backup_[i].first );
  }

  return true;
}






//=========================================

Cgrid_to_cgrid_copier::Cgrid_to_cgrid_copier() 
  : Property_copier() {
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  
  unset_harddata_flag_ = true;

}



bool Cgrid_to_cgrid_copier::copy( const Geostat_grid* server, 
                                  const GsTLGridProperty* server_prop,
                                  Geostat_grid* client, 
                                  GsTLGridProperty* client_prop ) {
  Cartesian_grid* to_grid = dynamic_cast< Cartesian_grid* >( client );
  const Cartesian_grid* from_grid = dynamic_cast< const Cartesian_grid* >( server );

  if( !from_grid || !to_grid ) return false;

  // if the 2 grids are identical, just copy the property
  if( are_identical_grids( from_grid, to_grid ) ) {
    appli_assert( server_prop->size() == client_prop->size() );
    for( int i=0; i < server_prop->size() ; i++ ) {
/*  This is a bug it still copy the property even if it is
    not informed
      if( !server_prop->is_informed( i ) ) 
        client_prop->set_not_informed( i );

      client_prop->set_value( server_prop->get_value( i ), i );
*/
		if( server_prop->is_informed( i ) ) {
		  client_prop->set_value( server_prop->get_value( i ), i );
		  if( mark_as_hard_ )   //added by Yongshe
              client_prop->set_harddata( true, i); //added by Yongshe
		}
	  else if(overwrite_)
		  client_prop->set_not_informed( i );

    }
    return true;
  }
  

  typedef GsTLGridProperty::property_type Property_type;

  // check if we already worked with "source" and "property_name" 
  // If that's the case and we're not required to do the assignement
  // from scratch, use what we did last time.
  if( !from_scratch_ && server == server_ &&
      server_prop == server_prop_ && client == client_ ) {
    
    for( unsigned int i = 0 ; i < last_assignement_.size() ; i++ ) {
      Property_type val = server_prop->get_value( last_assignement_[i].first );
      client_prop->set_value( val, last_assignement_[i].second );
      if( mark_as_hard_ )
      	client_prop->set_harddata( true, last_assignement_[i].second );
    } 
    return true;
  }
  
  last_assignement_.clear();
  backup_.clear();
  server_ = server;
  server_prop_ = server_prop;
  client_ = client;
  client_property_ = client_prop;

 
  // We will need to obtain the coordinates of a grid node from its
  // node id. Hence we need a grid-cursor, set to multigrid level 1.
  SGrid_cursor cursor = *( to_grid->cursor() );
  cursor.set_multigrid_level( 1 );

  GsTL_cube bbox = to_grid->bounding_box();
  GsTLInt current_id = 0;

  // Use a map to record what point was assigned to which grid node
  // This map is used in case multiple points could be assigned to the
  // same grid node: in that case the new point is assigned if it is closer
  // to the grid node than the previously assigned node was.
  typedef Cartesian_grid::location_type location_type;
  typedef std::map<GsTLInt,location_type>::iterator map_iterator;
  std::map<GsTLInt,location_type> already_assigned;

  Cartesian_grid::const_iterator grid_it = from_grid->begin( server_prop );

  for( ; grid_it != from_grid->end( server_prop ); ++grid_it, current_id++ ) {

    if( !grid_it->is_informed() ) continue;

    // only consider the points inside the target's bounding box
    location_type current_loc = grid_it->location();
    if( !bbox.contains( current_loc) ) continue;


    GsTLInt node_id = to_grid->closest_node( current_loc );
    appli_assert( node_id >=0 && node_id < client_prop->size() );
    appli_assert( current_id < server_prop->size() );
            
    // If there is already a property value (not assigned by the
    // grid initializer), and we don't want to overwrite, leave it alone
    if( !overwrite_ && client_prop->is_informed( node_id ) ) continue;


//    bool perform_assignment = true;

    // check if a point was already assigned to that node
    map_iterator it = already_assigned.find( node_id );
    if( it != already_assigned.end() ) {
    	int i,j,k;
    	GsTLCoordVector sizes = to_grid->cell_dimensions();
     	cursor.coords( node_id, i,j,k );
     	location_type node_loc( float(i)*sizes.x(),
			                        float(j)*sizes.y(),
			                        float(k)*sizes.z() );

     	// if the new point is further away to the grid node than
     	// the already assigned node, don't assign the new point
     	if( square_euclidean_distance( node_loc, current_loc ) > 
          square_euclidean_distance( node_loc, it->second ) ) 
        continue; //perform_assignment = false;
    }
	
//    if( perform_assignment ) {
  	// store the node id of the source and of the target and make a backup of the
    // property value of the client property
   	last_assignement_.push_back( std::make_pair( grid_it->node_id(), node_id ) );

    if( undo_enabled_ ) {
      Property_type backup_val = GsTLGridProperty::no_data_value;
      if( client_prop->is_informed( node_id ) )
        backup_val = client_prop->get_value( node_id );
      backup_.push_back( std::make_pair( node_id, backup_val ) );
    }

    Property_type val = grid_it->property_value();
    client_prop->set_value( val, node_id );
    already_assigned[node_id] = current_loc;
    if( mark_as_hard_ )
      client_prop->set_harddata( true, node_id );
    
  }
  
  unset_harddata_flag_ = mark_as_hard_;
  return true;
}




bool Cgrid_to_cgrid_copier::undo_copy() {
  for( unsigned int i = 0 ; i < backup_.size() ; i++ ) {
    client_property_->set_value( backup_[i].second, backup_[i].first );
    if( unset_harddata_flag_ ) 
      client_property_->set_harddata( false, backup_[i].first );
  }

  return true;
}


bool Cgrid_to_cgrid_copier::
are_identical_grids( const Cartesian_grid* grid1, 
                     const Cartesian_grid* grid2 ) {
  if( grid1 == grid2 ) return true;

  if( grid1->cell_dimensions() == grid2->cell_dimensions() &&
//      grid1->origin() == grid2->origin() &&
      grid1->nx() == grid2->nx() && grid1->ny() == grid2->ny() &&
      grid1->nz() == grid2->nz() ) {
    return true;
  }

  return false;
}



Pset_to_mask_copier::Pset_to_mask_copier() 
  : Property_copier() {
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  

}

bool Pset_to_mask_copier::copy( const Geostat_grid* server, 
                                  const GsTLGridProperty* server_prop,
                                  Geostat_grid* client, 
                                  GsTLGridProperty* client_prop ) {
  Reduced_grid* to_grid = dynamic_cast< Reduced_grid* >( client );
  const Point_set* from_grid = dynamic_cast< const Point_set* >( server );
  int nx = to_grid->nx();
  int ny = to_grid->ny();
  int nz = to_grid->nz();
  GsTLPoint o = to_grid->origin();
  int xy = nx*ny;
  int id;

  if( !from_grid || !to_grid ) return false;

  // Copy the property
  Point_set::location_type l;
   for( int i=0; i < server_prop->size() ; i++ ) {
	   l = from_grid->location(i);
	   id = int(l[2])*xy/o[0]+int(l[1])*nx/o[1]+int(l[0])/o[2];
	   if (!to_grid->isActive(id)) continue;
	  if( server_prop->is_informed( i ) ) 
		  client_prop->set_value( server_prop->get_value( i ), to_grid->full2reduced(id) );
	  else if(overwrite_)
		  client_prop->set_not_informed( to_grid->full2reduced(id) );
    }
   return true;
}

Pset_to_pset_copier::Pset_to_pset_copier() 
  : Property_copier() {
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  
  unset_harddata_flag_ = true;

}


bool Pset_to_pset_copier::copy( const Geostat_grid* server, 
                                  const GsTLGridProperty* server_prop,
                                  Geostat_grid* client, 
                                  GsTLGridProperty* client_prop ) {
  Point_set* to_grid = dynamic_cast< Point_set* >( client );
  const Point_set* from_grid = dynamic_cast< const Point_set* >( server );

  if( !from_grid || !to_grid ) return false;

  //Only allow the copy on the same grid
  if( !are_identical_grids(from_grid,to_grid) ) return false;

  // Copy the property
   for( int i=0; i < server_prop->size() ; i++ ) {
	  if( server_prop->is_informed( i ) ) 
		  client_prop->set_value( server_prop->get_value( i ), i );
	  else if(overwrite_)
		  client_prop->set_not_informed( i );
    }
   return true;
}



