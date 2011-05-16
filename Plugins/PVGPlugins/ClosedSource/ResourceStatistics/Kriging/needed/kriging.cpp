/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
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

#include "kriging.h"

#include "geostat_grid.h"
#include "combined_neighborhood.h"
#include "gval_iterator.h"

#include "grid_initializer.h"
#include "cartesian_grid.h"
#include "point_set.h"

#include <GsTL/kriging/kriging_weights.h>
#include <GsTL/geometry/Block_covariance.h>


Named_interface* Kriging::create_new_interface( std::string& ) {
  return new Kriging;
}


Kriging::Kriging() {
  Kconstraints_ = 0;
  combiner_ = 0;
  simul_grid_ = 0;
  neighborhood_ = 0;
  min_neigh_ = 0;
}
 

Kriging::~Kriging() {
  if( Kconstraints_ )
    delete Kconstraints_;

  if( combiner_ )
    delete combiner_;

 // if(blk_covar_)
 //   delete blk_covar_;
}



int Kriging::execute( GsTL_project* ) {
  // those flags will be used to signal if some of the nodes could not be
  // informed
  bool issue_singular_system_warning = false;
  bool issue_no_conditioning_data_warning = false;
 
  // Set up a progress notifier	
  int total_steps = simul_grid_->size();
  int frequency = std::max( total_steps / 20, 1 );
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running Kriging", 
			    total_steps, frequency );

  // create the property
  appli_message("creating new property: " << property_name_ << "..." );
  GsTLGridProperty* prop = 
    geostat_utils::add_property_to_grid( simul_grid_, property_name_ );
  simul_grid_->select_property( prop->name() );
  
  // create property for kriging variance
  std::string var_prop_name = prop->name() + "_krig_var";
  GsTLGridProperty* var_prop = 
    geostat_utils::add_property_to_grid( simul_grid_, var_prop_name );


  typedef Geostat_grid::iterator iterator;
  iterator begin = simul_grid_->begin();
  iterator end = simul_grid_->end();
  
  for( ; begin != end; ++begin ) {
    if( !progress_notifier->notify() ) {
      clean( property_name_ );
      return 1;
    }

    if( begin->is_informed() ) continue;
      
    neighborhood_->find_neighbors( *begin );
    
 //   if( neighborhood_->is_empty() ) {
    if( neighborhood_->size() < min_neigh_ ) {
      //if we don't have any conditioning data, skip the node
      issue_no_conditioning_data_warning = true;
      continue;
    }

    double variance;
    int status = kriging_weights_2( kriging_weights_, variance,
                      				    begin->location(), *neighborhood_,
                      				    covar_,*rhs_covar_, *Kconstraints_ );
      
    if(status == 0) {
    	// the kriging system could be solved
    	double estimate = (*combiner_)( kriging_weights_.begin(), 
		                            			kriging_weights_.end(),
					                            *neighborhood_ );
      begin->set_property_value( estimate );
      var_prop->set_value( variance, begin->node_id() );
    }
    else {
    	// the kriging system could not be solved, issue a warning and skip the
    	// node
      issue_singular_system_warning = true;
        
    }
  }

  if( issue_singular_system_warning )
    GsTLcerr << "Kriging could not be performed at some locations because\n"
             << "the kriging system was singular\n" 
             << gstlIO::end; 
  if( issue_no_conditioning_data_warning )
    GsTLcerr << "Kriging could not be performed at some locations because\n"
             << "the neighborhood of those locations was empty.\n"
             << "Try increasing the size of the search ellipsoid.\n"
             << gstlIO::end; 


  return 0;
}


void Kriging::clean( const std::string& prop ) {
  simul_grid_->remove_property( prop );
}




bool Kriging::initialize( const Parameters_handler* parameters, Error_messages_handler* errors ) {
  
  //-----------------
  // Extract the parameters input by the user from the parameter handler
  // Extract the parameters input by the user from the parameter handler
  std::string simul_grid_name = parameters->value( "Grid_Name.value" );
  errors->report( simul_grid_name.empty(), 
		  "Grid_Name", "No grid selected" );
  property_name_ = parameters->value( "Property_Name.value" );
  errors->report( property_name_.empty(), 
		  "Property_Name", "No property name specified" );


  // Get the simulation grid from the grid manager

  if( !simul_grid_name.empty() ) {
    bool ok = geostat_utils::create( simul_grid_, simul_grid_name,
				 "Grid_Name", errors );
    if( !ok ) return false;
  }
  else 
    return false;

  std::string harddata_grid_name = parameters->value( "Hard_Data.grid" );
  errors->report( harddata_grid_name.empty(), 
		  "Hard_Data", "No hard data specified" );
  harddata_property_name_ = parameters->value( "Hard_Data.property" );
  errors->report( harddata_property_name_.empty(), 
		  "Hard_Data", "No property name specified" );

  int max_neigh = 
    String_Op::to_number<int>( parameters->value( "Max_Conditioning_Data.value" ) );

  min_neigh_ = 
    String_Op::to_number<int>( parameters->value( "Min_Conditioning_Data.value" ) );
  errors->report( min_neigh_ >= max_neigh, 
		  "Min_Conditioning_Data", "Min must be less than Max" );

  // set-up the covariance
  bool init_cov_ok =
    geostat_utils::initialize_covariance( &covar_, "Variogram", 
	                        		            parameters, errors );
  if( !init_cov_ok ) return false;
  std::string test = parameters->value("do_block_kriging.value");
  if( parameters->value("do_block_kriging.value") == "1") {

    RGrid* block_grid = dynamic_cast<RGrid*>(simul_grid_);
    if(!block_grid) {
      errors->report("Grid_Name","Must be a Cartesian grid to use the Block Kriging Option");
      return false;
    }

    nblock_pts_[0] = String_Op::to_number<int>(parameters->value( "npoints_x.value" ) );
    nblock_pts_[1] = String_Op::to_number<int>(parameters->value( "npoints_y.value" ) );
    nblock_pts_[2] = String_Op::to_number<int>(parameters->value( "npoints_z.value" ) );

    errors->report(nblock_pts_[0] <= 0,"npoints_x","At least one point is necessary");
    errors->report(nblock_pts_[1] <= 0,"npoints_y","At least one point is necessary");
    errors->report(nblock_pts_[2] <= 0,"npoints_z","At least one point is necessary");
    if(!errors->empty()) return false;

    rhs_covar_ = new Block_covariance<Location>(covar_,nblock_pts_,block_grid->geometry()->cell_dims());
  }
  else rhs_covar_ = new Covariance<Location>(covar_);

/*  bool init_blk_cok_ok =  initialize_blk_covariance( &blk_covar_, 
            nblock_pts_,covar_, simul_grid_->geometry->cell_dims());
  if( !init_blk_cok_ok ) {
    errors->errors("Variogram",
      "Bad intialization of the block variogram, check the discretization and/or variogram");
    return false;
  }
  */

  // More complicated stuff related to the cokriging covaraince setup

 
  // Get the harddata grid from the grid manager
  if( !harddata_grid_name.empty() ) {
    bool ok = geostat_utils::create( harddata_grid_, harddata_grid_name,
				 "Hard_Data", errors );
    if( !ok ) return false;
  }
  else 
    return false;


  GsTLGridProperty* prop =
    harddata_grid_->select_property( harddata_property_name_ );
  if( !prop ) {
    std::ostringstream error_stream;
    error_stream << harddata_grid_name <<  " does not have a property called " 
	            	 << harddata_property_name_;
    errors->report( "Hard_Data", error_stream.str() );
  }


  //-------------
  // Set up the search neighborhood

  GsTLTriplet ellips_ranges;
  GsTLTriplet ellips_angles;
  bool extract_ok = 
    geostat_utils::extract_ellipsoid_definition( ellips_ranges, ellips_angles,
	                                    				   "Search_Ellipsoid.value",
					                                       parameters, errors );
  if( !extract_ok ) return false;

  extract_ok = geostat_utils::is_valid_range_triplet( ellips_ranges );
  errors->report( !extract_ok,
                  "Search_Ellipsoid",
                  "Ranges must verify: major range >= " 
                  "medium range >= minor range >= 0" );
  if( !extract_ok ) return false;


  neighborhood_ = 
    harddata_grid_->neighborhood( ellips_ranges, ellips_angles, &covar_ );
  neighborhood_->select_property( harddata_property_name_ );
  neighborhood_->max_size( max_neigh );

  kriging_weights_.reserve( 2 * max_neigh );


  
  //-----------------
  // The kriging constraints and combiner

  //  std::string kriging_type = parameters->value( "Kriging_Type.type" );
  //  set_kriging_parameters( kriging_type, parameters, errors );

  geostat_utils::KrigTagMap tags_map;
  tags_map[ geostat_utils::SK  ] = "Kriging_Type/parameters.mean";
  tags_map[ geostat_utils::KT  ] = "Kriging_Type/parameters.trend";
  tags_map[ geostat_utils::LVM ] = "Kriging_Type/parameters.property";
//  tags_map[ geostat_utils::LVM ] = "Kriging_Type/prop_mean_grid;harddata_grid_name;Kriging_Type/prop_mean_hdata";

  geostat_utils::Kriging_type ktype = 
    geostat_utils::kriging_type( "Kriging_Type.type", parameters, errors );
  geostat_utils::initialize( ktype, combiner_, Kconstraints_,
                             tags_map,
                             parameters, errors,
                             simul_grid_ );


  if( !errors->empty() )
    return false;
 

  return true;
}


