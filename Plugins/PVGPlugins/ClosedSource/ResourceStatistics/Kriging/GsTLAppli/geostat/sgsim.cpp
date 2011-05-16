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

#include <GsTLAppli/geostat/sgsim.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/combined_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/math/random_numbers.h>
#include <GsTLAppli/appli/utilities.h>

#include <GsTL/cdf/gaussian_cdf.h>
#include <GsTL/sampler/monte_carlo_sampler.h>
#include <GsTL/cdf_estimator/gaussian_cdf_Kestimator.h>
#include <GsTL/simulation/sequential_simulation.h>
#include <GsTL/univariate_stats/cdf_transform.h>
#include <GsTL/univariate_stats/build_cdf.h>

#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>

#include <GsTLAppli/grid/grid_model/reduced_grid.h>

int Sgsim::execute( GsTL_project* ) {
  
  // Initialize the global random number generator
  Global_random_number_generator::instance()->seed( seed_ );
  
  
  // Set up a progress notifier	
  int total_steps = simul_grid_->size() * (nb_of_realizations_);
  int frequency = std::max( total_steps / 20, 1 );
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running Sgsim", 
			    total_steps, frequency );

  // In sequential gaussian simulation, the marginal is a Gaussian cdf, 
  // with mean 0 and variance 1.
  Gaussian_cdf marginal( 0.0, 1.0 );
  Gaussian_cdf ccdf;

  // work on the fine grid
  if( dynamic_cast<Strati_grid*>( simul_grid_ ) ) {
    Strati_grid* sgrid = dynamic_cast<Strati_grid*>( simul_grid_ );
    sgrid->set_level( 1 );
  }

  // set up the cdf-estimator
  typedef Gaussian_cdf_Kestimator< Covariance<Location>,
                                   Neighborhood,
                                   geostat_utils::KrigingConstraints
                                  >    Kriging_cdf_estimator;
  Kriging_cdf_estimator cdf_estimator( covar_,
				       *Kconstraints_,
				       *combiner_ );

  // set up the sampler
  Random_number_generator gen;
  Monte_carlo_sampler_t< Random_number_generator > sampler( gen );
  

  bool from_scratch = true;
  // loop on all realizations
  for( int nreal = 0; nreal < nb_of_realizations_ ; nreal ++ ) {

    // compute the random path
    simul_grid_->init_random_path(from_scratch);
    from_scratch = false;

    // update the progress notifier
    progress_notifier->message() << "working on realization " 
                                 << nreal+1 << gstlIO::end;
    if( !progress_notifier->notify() ) return 1;


    // Create a new property to hold the realization and tell the simulation 
    // grid to use it as the current property 
    appli_message( "Creating new realization" );
    GsTLGridProperty* prop = multireal_property_->new_realization();
    simul_grid_->select_property( prop->name() );
    neighborhood_->select_property( prop->name() );

    // initialize the new realization with the hard data, if that was requested 
    if( property_copier_ ) {
      property_copier_->copy( harddata_grid_, harddata_property_,
                              simul_grid_, prop );
      //initializer_->assign( prop, harddata_grid_, harddata_property_->name() );
    }


    appli_message( "Doing simulation" );

	
    // do the simulation
    int status = 
      sequential_simulation( simul_grid_->random_path_begin(),
			     simul_grid_->random_path_end(),
			     *(neighborhood_.raw_ptr()),
			     ccdf,
			     cdf_estimator,
			     marginal,
			     sampler, progress_notifier.raw_ptr()
			     );
    if( status == -1 ) {
      clean( prop );
      return 1;
    }
    // back-transform if needed
    if( use_target_hist_ ) {
      cdf_transform( prop->begin(), prop->end(), 
		                 marginal, target_cdf_ );
    }

  }

  clean();

  return 0;
}





bool Sgsim::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) {

  OPEN_DEBUG_STREAM( "sgsim.dbg" );

  // Extract the parameters input by the user from the parameter handler
  
  //-------------
  // The "simulation" grid parameters
  
  std::string simul_grid_name = parameters->value( "Grid_Name.value" );
  errors->report( simul_grid_name.empty(), 
            		  "Grid_Name", "No grid selected" );
  std::string property_name = parameters->value( "Property_Name.value" );
  errors->report( property_name.empty(), 
            		  "Property_Name", "No property name specified" );

  // Get the simulation grid from the grid manager  
  if( simul_grid_name.empty() ) return false;

  bool ok = geostat_utils::create( simul_grid_, simul_grid_name,
                          				 "Grid_Name", errors );
  if( !ok ) return false;

  // create  a multi-realization property
  multireal_property_ =
    simul_grid_->add_multi_realization_property( property_name );



  //-------------
  // The hard data parameters

  std::string harddata_grid_name = parameters->value( "Hard_Data.grid" );
 
  if( !harddata_grid_name.empty() ) {
    std::string hdata_prop_name = parameters->value( "Hard_Data.property" );
    errors->report( hdata_prop_name.empty(), 
		                "Hard_Data", "No property name specified" );

    // Get the harddata grid from the grid manager
    bool ok = geostat_utils::create( harddata_grid_, harddata_grid_name, 
      	                           	  "Hard_Data", errors );
    if( !ok ) return false;

    harddata_property_ = harddata_grid_->property( hdata_prop_name );
    if( !harddata_property_ ) {
      	std::ostringstream error_stream;
      	error_stream <<  harddata_grid_name 
		                 <<  " does not have a property called " 
		                 << hdata_prop_name;
      	errors->report( "Hard_Data", error_stream.str() );
        return false;
    }

  }


  // hard data assignement and transform is only needed if we have a valid
  // hard data grid and property.  We always assigne the data if it belongs
  // the same grid

  
  bool  assign_harddata = 
      String_Op::to_number<bool>( parameters->value( "Assign_Hard_Data.value" ) );
  if( harddata_grid_ == NULL ) assign_harddata=false; 
  else if( harddata_grid_ == simul_grid_ ) assign_harddata=true;

  if( assign_harddata ) {
    property_copier_ = 
      Property_copier_factory::get_copier( harddata_grid_, simul_grid_ );
    if( !property_copier_ ) {
      std::ostringstream message;
      message << "It is currently not possible to copy a property from a "
              << harddata_grid_->classname() << " to a " 
              << simul_grid_->classname() ;
      errors->report( !property_copier_, "Assign_Hard_Data", message.str() );
      return false;
    }
  }
 

  //-------------
  // Target histogram

  use_target_hist_ = 
    String_Op::to_number<bool>( parameters->value( "Use_Target_Histogram.value" ) );

  if(use_target_hist_) {
	  bool ok = geostat_utils::get_non_param_cdf(target_cdf_,parameters, errors,"nonParamCdf");
	  if(!ok) return false;
	  if( harddata_property_ ) {
		  harddata_property_ = 
		    geostat_utils::gaussian_transform_property( harddata_property_, target_cdf_, harddata_grid_ );
		  if( !harddata_property_ ) return false;
      
      clear_temp_properties_ = true;
		  harddata_grid_->select_property( harddata_property_->name() );
	  }
  }


  //-------------
  // Number of realizations and random number seed

  nb_of_realizations_ = 
    String_Op::to_number<int>( parameters->value( "Nb_Realizations.value" ) );
  
  seed_ = String_Op::to_number<int>( parameters->value( "Seed.value" ) );




  //-------------
  // Variogram (covariance) initialization 

  bool init_cov_ok = 
    geostat_utils::initialize_covariance( &covar_, "Variogram", 
                                          parameters, errors );
  if( !init_cov_ok ) return false;



  //-------------
  // Set up the search neighborhood

  int max_neigh = 
    String_Op::to_number<int>( parameters->value( "Max_Conditioning_Data.value" ) );
  
  GsTLTriplet ranges;
  GsTLTriplet angles;
  bool extract_ok =
    geostat_utils::extract_ellipsoid_definition( ranges, angles, 
                                					       "Search_Ellipsoid.value",
		  			                                     parameters, errors );
  if( !extract_ok ) return false;
  extract_ok = geostat_utils::is_valid_range_triplet( ranges );
  errors->report( !extract_ok,
                  "Search_Ellipsoid",
                  "Ranges must verify: major range >= " 
                  "medium range >= minor range >= 0" );
  if( !extract_ok ) return false;

  // If the hard data are not "relocated" on the simulation grid,
  // use a "combined neighborhood", otherwise use a single 
  // neighborhood
  if( !harddata_grid_ || assign_harddata ) {
    neighborhood_ = SmartPtr<Neighborhood>( 
                      simul_grid_->neighborhood( ranges, angles, &covar_ ) 
                    );
  }
  else {
    Neighborhood* simul_neigh = 
      simul_grid_->neighborhood( ranges, angles, &covar_ );
    simul_neigh->max_size( max_neigh );
    Neighborhood* harddata_neigh = 
      harddata_grid_->neighborhood( ranges, angles, &covar_ );
    harddata_neigh->max_size( max_neigh );
    harddata_neigh->select_property( harddata_property_->name() );

    neighborhood_ = 
      SmartPtr<Neighborhood>( new Combined_neighborhood_dedup( harddata_neigh,
							                                           simul_neigh, &covar_, false) );
  }

  neighborhood_->max_size( max_neigh );



  //-----------------
  // The kriging constraints and combiner

  geostat_utils::KrigTagMap tags_map;
  tags_map[ geostat_utils::KT  ] = "Trend.value";
  tags_map[ geostat_utils::LVM ] = "Local_Mean_Property.value";

  geostat_utils::KrigDefaultsMap defaults;
  defaults[ geostat_utils::SK ] = "0.0";

  geostat_utils::Kriging_type ktype = 
    geostat_utils::kriging_type( "Kriging_Type.value", parameters, errors );
  geostat_utils::initialize( ktype, combiner_, Kconstraints_,
                             tags_map,
                             parameters, errors,
                             simul_grid_, defaults );



  //----------------
  // Report errors if any

  if( !errors->empty() ) {
    clean();
    return false;
  }

  return true;
}



void Sgsim::clean( GsTLGridProperty* prop ) {
  if( prop ) 
    simul_grid_->remove_property( prop->name() );

  if( clear_temp_properties_ && harddata_property_ && harddata_grid_ ) {
    harddata_grid_->remove_property( harddata_property_->name() );
    harddata_property_ = 0;
  }
}



Named_interface* Sgsim::create_new_interface( std::string& ) {
  return new Sgsim;
}


Sgsim::Sgsim() {
  Kconstraints_ = 0;
  combiner_ = 0;
  simul_grid_ = 0;
  harddata_grid_ = 0;
  harddata_property_ = 0;
  neighborhood_ = 0;
  multireal_property_ = 0;

  use_target_hist_ = false;
  clear_temp_properties_ = false;
}
 

Sgsim::~Sgsim() {
  delete Kconstraints_;
  delete combiner_;

  clean();
}

