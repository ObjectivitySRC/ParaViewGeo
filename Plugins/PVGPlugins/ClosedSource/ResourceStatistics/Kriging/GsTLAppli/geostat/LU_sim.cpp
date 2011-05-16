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

#include <GsTLAppli/geostat/LU_sim.h>
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
#include <GsTL/univariate_stats/cdf_transform.h>
#include <GsTL/univariate_stats/build_cdf.h>
#include <GsTL/simulation/LU_simulator.h>

#include <GsTLAppli/utils/gstl_plugins.h>

#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>




int LU_sim::execute( GsTL_project* ) {
  
  // Initialize the global random number generator
  Global_random_number_generator::instance()->seed( seed_ );
  
  
  // Set up a progress notifier	
  int total_steps = simul_grid_->size() * (nb_of_realizations_);
  int frequency = std::max( total_steps / 20, 1 );
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running LU_sim", 
			    total_steps, frequency );

  // In LU simulation, the marginal is a Gaussian cdf, 
  // with mean 0 and variance 1.
  Gaussian_cdf marginal( 0.0, 1.0 );

  // work on the fine grid
  if( dynamic_cast<Strati_grid*>( simul_grid_ ) ) {
    Strati_grid* sgrid = dynamic_cast<Strati_grid*>( simul_grid_ );
    sgrid->set_level( 1 );
  }
  //typedef Geostat_grid::random_path_iterator iterator;
  typedef Gval_iterator< TabularMapIndex > iterator;

  std::vector< int > id_unknown;
  std::vector< int > id_data;

  //harddata_grid_->select_property(harddata_property_->name());
  if( harddata_grid_ ) {
    for( int i=0; i< harddata_property_->size(); i++ ) {
      if(!harddata_property_->is_informed(i) ) continue;
      id_data.push_back( i );
    }
  }
  for( int i=0; i< simul_grid_->size(); i++ ) {
    if( simul_grid_ == harddata_grid_ && harddata_property_->is_informed(i) ) continue;
    id_unknown.push_back( i );
  }

  iterator begin_d( harddata_grid_, harddata_property_ ,0,  
			      id_data.size(), TabularMapIndex(&id_data) );
  iterator end_d( harddata_grid_, harddata_property_ ,id_data.size(),  
			      id_data.size(), TabularMapIndex(&id_data) );

  GsTLGridProperty* prop = multireal_property_->new_realization();
  iterator begin_u( simul_grid_, prop ,0,  
			      id_unknown.size(), TabularMapIndex(&id_unknown) );
  iterator end_u( simul_grid_, prop ,id_unknown.size(),  
			      id_unknown.size(), TabularMapIndex(&id_unknown) );

  Random_number_generator gen;
  LU_simulator<
        Covariance<Location>,
        Random_number_generator,
        Geostat_grid> 
    lu_sim( covar_,gen, simul_grid_ );
  lu_sim.initialize_matrix(begin_u, end_u, begin_d, end_d);

  // loop on all realizations
  for( int nreal = 0; nreal < nb_of_realizations_ ; nreal ++ ) {

    // update the progress notifier
    progress_notifier->message() << "working on realization " 
                                 << nreal+1 << gstlIO::end;
    if( !progress_notifier->notify() ) return 1;


    // Create a new property to hold the realization and tell the simulation 
    // grid to use it as the current property 
    appli_message( "Creating new realization" );
    //GsTLGridProperty* prop = multireal_property_->new_realization();
    if(nreal>0) prop = multireal_property_->new_realization();
    simul_grid_->select_property( prop->name() );


    // initialize the new realization with the hard data, if that was requested 
    if( property_copier_ ) {
      property_copier_->copy( harddata_grid_, harddata_property_,
                              simul_grid_, prop );
    }


    iterator begin_unk( simul_grid_, prop ,0,  
			   id_unknown.size(), TabularMapIndex(&id_unknown) );
    iterator  end_unk( simul_grid_, prop ,id_unknown.size(),  
			   id_unknown.size(), TabularMapIndex(&id_unknown) );

    int status = lu_sim(begin_unk,end_unk,marginal);

    // initialize the new realization with the hard data, if that was requested 
  //  if( property_copier_ ) {
  //    property_copier_->copy( harddata_grid_, harddata_property_,
  //                            simul_grid_, prop );
      //initializer_->assign( prop, harddata_grid_, harddata_property_->name() );
  //  }


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


bool LU_sim::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) {

  OPEN_DEBUG_STREAM( "LU_sim.dbg" );

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
  // hard data grid and property.  We always assign the data if it belongs
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
	  bool ok = geostat_utils::get_non_param_cdf(target_cdf_,parameters, errors, "nonParamCdf");
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


  //----------------
  // Report errors if any

  if( !errors->empty() ) {
    clean();
    return false;
  }

  return true;
}



void LU_sim::clean( GsTLGridProperty* prop ) {
  if( prop ) 
    simul_grid_->remove_property( prop->name() );

  if( clear_temp_properties_ && harddata_property_ && harddata_grid_ ) {
    harddata_grid_->remove_property( harddata_property_->name() );
    harddata_property_ = 0;
  }
}


Named_interface* LU_sim::create_new_interface( std::string& ) {
  return new LU_sim;
}

LU_sim::LU_sim() {
//  Kconstraints_ = 0;
//  combiner_ = 0;
  simul_grid_ = 0;
  harddata_grid_ = 0;
  harddata_property_ = 0;
  multireal_property_ = 0;

  use_target_hist_ = false;
  clear_temp_properties_ = false;
}
 

LU_sim::~LU_sim() {
//  delete Kconstraints_;
 // delete combiner_;
  
  clean();
}

//extern "C" __declspec(dllexport) int plugin_init() { 
//  BIND_GEOSTAT_ALGO(LU_sim)
//  return 0; 
//}
//GEOSTAT_ALGO(LU_sim);

