/**********************************************************************
** Author: Ting Li
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
#include <GsTLAppli/grid/mgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

MgridNeighborhood::MgridNeighborhood( RGrid* grid, 
		GsTLGridProperty* property, 
		GsTLInt max_radius, GsTLInt mid_radius, GsTLInt min_radius, 
		double x_angle, double y_angle, double z_angle, 
		int max_neighbors , 
		const Covariance<GsTLPoint>* cov ) :
	Rgrid_ellips_neighborhood(  grid,  property,  max_radius,  mid_radius,  min_radius, 
								x_angle,  y_angle, z_angle,  max_neighbors,cov )
	{}

void MgridNeighborhood::find_neighbors( const Geovalue& center ) 
{
  
  _mcursor = dynamic_cast<EGridCursor*>(grid_->cursor());
  appli_assert(_mcursor);

  neighbors_.clear();
  if( !property_ ) return;

  center_ = center;
//  center_.set_property_array( property_ );

  // "already_found" is the number of neighbors already found
  int already_found=0;

  // loc will store the i,j,k coordinates of the center, node_id is the 
  // center's node-id. They will be computed differently, whether "center"
  // and *this both refer to the same grid or not.
  GsTLGridNode loc;
  GsTLInt node_id = -1;

  if( center.grid() != grid_ ) {
    // "center" and "*this" do not refer to the same grid
    bool ok = grid_->geometry()->grid_coordinates( loc, center.location() );
    if( !ok ) return;

	if( includes_center_ ) {
      GsTLInt id = _mcursor->node_id( loc[0], loc[1], loc[2] );
	  if (id == -1) return;
	}
  }
  else {
    // "center" and "*this" both refer to the same grid
    _mcursor->coords( center.node_id(), loc[0], loc[1], loc[2] ); 
    node_id = center.node_id();  

		/*
	appli_message("center node id : " << center.node_id() << ", loc: " << loc[0] <<
		"," << loc[1] << "," << loc[2] << ". Id: " << _mcursor->node_id(loc[0],loc[1],loc[2]) 
		<<  "\n");
	*/
  }

  
  if( includes_center_ && property_->is_informed( node_id ) ) {
    neighbors_.push_back( Geovalue( grid_, property_, node_id ) );
    already_found++;
  }
 
  
  // Visit each node defined by the window ("geom_")
  // For each node, check if the node is inside the grid.
  // If it is and it contains a data value, add it to the list of
  // neighbors
  Grid_template::const_iterator it = geom_.begin();
  Grid_template::const_iterator end = geom_.end();

  while( it != end && already_found < max_neighbors_ ) {
    GsTLGridNode node = loc + (*it);
    GsTLInt node_id = _mcursor->node_id( node[0], node[1], node[2] );
    
	
    if( node_id < 0 ) {
      // The node does not belong to the grid: skip it
      it++;
      continue;
    }
	
    if( property_->is_informed( node_id ) ) {
      // The node is informed: get the corresponding geovalue and add it
      // to the list of neighbors
      neighbors_.push_back( Geovalue( grid_, property_, node_id ) );
      already_found++;
    }

    it++;
  }
}

MgridNeighborhood_hd::MgridNeighborhood_hd( RGrid* grid, 
		GsTLGridProperty* property, 
		GsTLInt max_radius, GsTLInt mid_radius, GsTLInt min_radius, 
		double x_angle, double y_angle, double z_angle, 
		int max_neighbors , 
		const Covariance<GsTLPoint>* cov ):
	Rgrid_ellips_neighborhood_hd(  grid,  property,  max_radius,  mid_radius,  min_radius, 
		 x_angle, y_angle,  z_angle, 	 max_neighbors , cov  )
{}
void MgridNeighborhood_hd::find_neighbors( const Geovalue& center ) {
  appli_assert( center.grid() == grid_ );

  _mcursor = dynamic_cast<EGridCursor*>(grid_->cursor());
  appli_assert(_mcursor);

  // This is exactly the same function as 
  // Rgrid_ellips_neighborhood::find_neighbors, except that the condition
  // for a node to be a neighbor is that it contains a hard-data

  
  neighbors_.clear();
  if( !property_ ) return;

  center_ = center;
  center_.set_property_array( property_ );

  Grid_template::const_iterator it = geom_.begin();
  Grid_template::const_iterator end = geom_.end();
  
  GsTLGridNode loc;
  _mcursor->coords( center.node_id(), loc[0], loc[1], loc[2] );

  // "already_found" is the number of neighbors already found
  int already_found=0;

  if( includes_center_ && center_.is_harddata() ) {
    neighbors_.push_back( center_ );
    already_found++;
  }

  while( it != end && already_found < max_neighbors_ ) {
    GsTLGridNode node = loc + (*it);
    GsTLInt node_id = _mcursor->node_id( node[0], node[1], node[2] );

    if( node_id < 0 ) {
      it++;
      continue;
    }

    if( property_->is_harddata( node_id ) ) {
      neighbors_.push_back( Geovalue( grid_, property_, node_id ) );
      already_found++;
    }

    it++;
  }
}

MgridWindowNeighborhood::MgridWindowNeighborhood( const Grid_template& geom, RGrid* grid, 
												 GsTLGridProperty* prop  ) :
		Rgrid_window_neighborhood(geom,grid,prop)
		{}

void MgridWindowNeighborhood::set_grid( RGrid* grid ) {
  appli_assert(dynamic_cast<Reduced_grid*>(grid));
  grid_ = grid;
  if( grid ) {
    cursor_ = *( grid->cursor() );
	_mcursor = dynamic_cast<EGridCursor*>(grid->cursor());
  }
}

void MgridWindowNeighborhood::find_neighbors( const Geovalue& center ) {
  size_ = -1;
  center_ = center;
  center_.set_property_array( property_ );

  _mcursor = dynamic_cast<EGridCursor*>(grid_->cursor());
  neighbors_.clear();
  if( !property_ ) return;

  //SGrid_cursor cursor( *grid_->cursor() );
  GsTLInt i,j,k;
  _mcursor->coords( center.node_id(), i,j,k ); 
  GsTLGridNode center_location( i,j,k );

  if( geom_.size() == 0 ) return;

  Grid_template::iterator begin = geom_.begin();
  Grid_template::iterator bound = geom_.end()-1;

  while (bound != begin-1) {
    GsTLGridNode p = center_location + (*bound);
    GsTLInt node_id = _mcursor->node_id( p[0], p[1], p[2] );
    if( node_id < 0 ) {
      bound--;
      continue;
    }
    if( property_->is_informed( node_id ) )
      break;
    else
      bound--;
  }

  for( ; begin != bound+1 ; ++begin ) {
    GsTLGridNode node = center_location + (*begin);
	if (_mcursor->node_id( node[0], node[1], node[2] ) < 0)
		continue;
    neighbors_.push_back( Geovalue( grid_, property_, 
				    _mcursor->node_id( node[0], node[1],
						     node[2] ) )
			  );
  }
}

void MgridWindowNeighborhood::find_all_neighbors( const Geovalue& center ) {
  size_ = -1;
  center_ = center;
  center_.set_property_array( property_ );

  _mcursor = dynamic_cast<EGridCursor*>(grid_->cursor());

  neighbors_.clear();
  if( !property_ ) return;

  //SGrid_cursor cursor( *grid_->cursor() );
  GsTLInt i,j,k;
  _mcursor->coords( center.node_id(), i,j,k ); 
  GsTLGridNode center_location( i,j,k );

  if( geom_.size() == 0 ) return;

  Grid_template::iterator begin = geom_.begin();
  Grid_template::iterator bound = geom_.end()-1;

  for( ; begin != bound+1 ; ++begin ) {
    GsTLGridNode node = center_location + (*begin);
	if (_mcursor->node_id( node[0], node[1], node[2] ) < 0)
		continue;
    neighbors_.push_back( Geovalue( grid_, property_, 
				    _mcursor->node_id( node[0], node[1],
						     node[2] ) )
			  );
  }
}
