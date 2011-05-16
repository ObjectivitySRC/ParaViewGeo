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

#include <GsTLAppli/geostat/sisim.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/progress_notifier.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/appli/utilities.h>

#include <GsTL/cdf/non_param_cdf.h>
#include <GsTL/cdf/categ_non_param_cdf.h>
#include <GsTL/sampler/monte_carlo_sampler.h>
#include <GsTL/cdf_estimator/indicator_cdf_estimator.h>
#include <GsTL/cdf_estimator/soft_indicator_cdf_estimator.h>
#include <GsTL/simulation/sequential_simulation.h>
#include <GsTL/simulation/sequential_cosimulation.h>

#include <GsTLAppli/grid/grid_model/combined_neighborhood.h>

#include <vector>
#include <numeric>


Named_interface* Sisim::create_new_interface( std::string& ) {
  return new Sisim;
}
 

Sisim::Sisim() {
  simul_grid_ = 0;
  multireal_property_ = 0;
  harddata_grid_ = 0;
  ccdf_ = 0;
  marginal_ = 0;
}


Sisim::~Sisim() {
  delete ccdf_;
  delete marginal_;
}



int Sisim::execute( GsTL_project* ) {

  // Initialize the global random number generator
  Global_random_number_generator::instance()->seed( seed_ );
  
  // Set up a progress notifier	
  int total_steps = simul_grid_->size() * (nb_of_realizations_);
  int frequency = std::max( total_steps / 20, 1 );
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running Sisim", 
			    total_steps, frequency );

  // work on the fine grid
  if( dynamic_cast<Strati_grid*>( simul_grid_ ) ) {
    Strati_grid* sgrid = dynamic_cast<Strati_grid*>( simul_grid_ );
    sgrid->set_level( 1 );
  }

  // compute the random path
  simul_grid_->init_random_path();

  if( do_median_ik_ ) {
    return median_ik( progress_notifier.raw_ptr() );
  }
  else
    return full_ik( progress_notifier.raw_ptr() );
}



int Sisim::median_ik( Progress_notifier* progress_notifier ) {

  // set up the sampler
  Random_number_generator gen;
  Monte_carlo_sampler_t< Random_number_generator > sampler( gen );
  

  // loop on all realizations
  for( int nreal = 0; nreal < nb_of_realizations_ ; nreal ++ ) {
    // update the progress notifier
    progress_notifier->message() << "working on realization " 
                                 << nreal << gstlIO::end;
    if( !progress_notifier->notify() ) return 1;


    // Create a new property to hold the realization and tell the simulation 
    // grid to use it as the current property 
    appli_message( "Creating new realization" );
    GsTLGridProperty* prop = multireal_property_->new_realization();
    simul_grid_->select_property( prop->name() );
    neighborhood_->select_property( prop->name() );

    // initialize the new realization with the hard data
    if( property_copier_ ) {
      property_copier_->copy( harddata_grid_, harddata_property_,
                              simul_grid_, prop );
    }
    //if( initializer_ )
    //  initializer_->assign( prop, harddata_grid_, harddata_property_->name() );
    

    // do the simulation
    appli_message( "Doing simulation" );
    int status = 
      sequential_simulation( simul_grid_->random_path_begin(),
			     simul_grid_->random_path_end(),
			     *(neighborhood_.raw_ptr()),
			     *ccdf_,
			     *cdf_estimator_,
			     *marginal_,
			     sampler, progress_notifier
			     );

    if( status == -1 ) {
      simul_grid_->remove_property( prop->name() );
      return 1;
    }
    
  }

  return 0;
}



int Sisim::full_ik( Progress_notifier* progress_notifier ) {
    // set up the sampler
  Random_number_generator gen;
  Monte_carlo_sampler_t< Random_number_generator > sampler( gen );
  

  // loop on all realizations
  for( int nreal = 0; nreal < nb_of_realizations_ ; nreal ++ ) {
    // update the progress notifier
    progress_notifier->message() << "working on realization " 
                                 << nreal << gstlIO::end;
    if( !progress_notifier->notify() ) return 1;

 
    // Create a new property to hold the realization and tell the simulation 
    // grid to use it as the current property 
    appli_message( "Creating new realization" );
    GsTLGridProperty* prop = multireal_property_->new_realization();
    simul_grid_->select_property( prop->name() );
    for( unsigned int i = 0 ; i < neighborhoods_vector_.size() ; i++ ) 
      neighborhoods_vector_[i].select_property( prop->name() );

    // initialize the new realization with the hard data
    if( property_copier_ ) {
      property_copier_->copy( harddata_grid_, harddata_property_,
                              simul_grid_, prop );
    }
    //if( initializer_ )
    //  initializer_->assign( prop, harddata_grid_, harddata_property_->name() );
     

    // do the simulation
    appli_message( "Doing simulation" );
    int status = 
      sequential_cosimulation( simul_grid_->random_path_begin(),
			                         simul_grid_->random_path_end(),
			                         neighborhoods_vector_.begin(), 
                               neighborhoods_vector_.end(),
                    			     *ccdf_,
			                         *cdf_estimator_,
			                         *marginal_,
			                         sampler, progress_notifier
			                    );
    if( status == -1 ) {
      simul_grid_->remove_property( prop->name() );
      return 1;
    }

  }

  return 0;
}



bool Sisim::initialize( const Parameters_handler* parameters,
			                  Error_messages_handler* errors ) {
 
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
  bool ok = geostat_utils::create( simul_grid_, simul_grid_name,
			       "Grid_Name", errors );
  if( !ok ) return false;
  
  // create  a multi-realization property
  multireal_property_ =
    simul_grid_->add_multi_realization_property( property_name );
  appli_assert( multireal_property_ );

  //-------------
  // Miscellaneous simulation parameters

  nb_of_realizations_ = 
    String_Op::to_number<int>( parameters->value( "Nb_Realizations.value" ) );
  
  seed_ = String_Op::to_number<int>( parameters->value( "Seed.value" ) );



  //-------------
  // The hard data parameters

  std::string harddata_grid_name = parameters->value( "Hard_Data_Grid.value" );

  // Get the harddata grid from the grid manager and select the 
  // hard data property
  if( !harddata_grid_name.empty() ) {
    geostat_utils::create( harddata_grid_, harddata_grid_name, 
		       "Hard_Data_Grid", errors );

    std::string hdata_property_name = 
      parameters->value( "Hard_Data_Property.value" );
    errors->report( hdata_property_name.empty(), 
		    "Hard_Data_Property", "No properties specified" );

    // Get the harddata property from the grid
    harddata_property_ = harddata_grid_->property( hdata_property_name );
  }


  bool  assign_harddata = 
      String_Op::to_number<bool>( parameters->value( "Assign_Hard_Data.value" ) );
  if( harddata_grid_ == NULL ) assign_harddata=false; 
  else if( harddata_grid_ == simul_grid_ ) assign_harddata=true;

 
  if( assign_harddata ) {

    assign_harddata = true;
  //    String_Op::to_number<bool>( parameters->value( "Assign_Hard_Data.value" ) );
  
//    if( assign_harddata ) {
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
      // initializer_ = new Grid_initializer( simul_grid_, false );
  }


  is_data_coded_ = false;
 // Geostat_grid* coded_grid;
  std::string coded_grid_name = parameters->value( "coded_grid.value" );
  if( !coded_grid_name.empty() ) {    
    geostat_utils::create( coded_grid_, coded_grid_name, 
		       "coded_grid", errors );
  }
  
  std::vector<GsTLGridProperty*> coded_props;
  std::string coded_prop_names = parameters->value( "coded_props.value" );
  if(!coded_prop_names.empty() && coded_grid_ ) {
    is_data_coded_ = true;
    std::vector<std::string> coded_names = 
      String_Op::decompose_string(coded_prop_names,";");
    for(int i=0; i<coded_names.size() ; ++i ) {
      coded_props.push_back( coded_grid_->property( coded_names[i] ) );
      errors->report(coded_props[i]==NULL,"coded_props","A property does not exist");
    }
  }


  //-------------
  // The cdf parameters (# of thresholds, marginal, ...)
  int nb_indicators = 
    String_Op::to_number<int>( parameters->value( "Nb_Indicators.value" ) );

  if(is_data_coded_ && coded_props.size() != nb_indicators ) {
    std::ostringstream message;
    message << "enter exactly " << nb_indicators << " of soft data";
    errors->report( "coded_props", message.str() );
    return false;
  }

  bool categorical = 
    String_Op::to_number<bool>( 
		    parameters->value( "Categorical_Variable_Flag.value" )
	    );
  
  std::string marginal_probabilities_string = 
    parameters->value( "Marginal_Probabilities.value" );
  std::vector<double> marginal_probs = 
    String_Op::to_numbers<double>( marginal_probabilities_string );

  if( marginal_probs.size() != nb_indicators ) {
    std::ostringstream message;
    message << "enter exactly " << nb_indicators << " probabilities";
    errors->report( "Marginal_Probabilities", message.str() );
    return false;
  }

  if( categorical ) {
    // we ignore the input threshold values. The categories are numbered
    // from 0 to k (if there are k+1 categories).
    ccdf_ = new Categ_non_param_cdf<float>( marginal_probs.size() );
    marginal_ = new Categ_non_param_cdf<float>( marginal_probs.size(),
						marginal_probs.begin() );

    errors->report( !is_valid_cdf( marginal_probs.begin(), marginal_probs.end(),
                                   GsTL::discrete_variable_tag() ),
                    "Marginal_Probabilities", 
                    "Values must be between 0 and 1 and sum up to 1" );

  }
  else {
    std::vector<float> thresholds = 
      String_Op::to_numbers<float>( parameters->value( "Thresholds.value" ) );
    
    if( thresholds.size() != nb_indicators ) {
      std::ostringstream message;
      message << "enter exactly " << nb_indicators << " thresholds";
      errors->report( "Thresholds", message.str() );
      return false;
    }
    
    errors->report( !GsTL::is_sorted( thresholds.begin(), thresholds.end() ),
                    "Thresholds", 
                    "Threshold values must be sorted in ascending order" );


    ccdf_ = new Non_param_cdf<>( thresholds.begin(), thresholds.end() );
    marginal_ = new Non_param_cdf<>( thresholds.begin(), 
				     thresholds.end(),
				     marginal_probs.begin() );

    errors->report( !is_valid_cdf( marginal_probs.begin(), marginal_probs.end(),
                                   GsTL::continuous_variable_tag() ),
                    "Marginal_Probabilities", 
                    "The values entered do not define a valid cdf" );

    geostat_utils::set_cdf_extrapolation_tail(parameters,errors, 
     *((Non_param_cdf<>*) ccdf_), "lowerTailCdf", "upperTailCdf");
    geostat_utils::set_cdf_extrapolation_tail(parameters,errors, 
     *((Non_param_cdf<>*) marginal_), "lowerTailCdf", "upperTailCdf");

  }


  // report errors found so far
  if( !errors->empty() ) 
    return false;


  //-------------
  // The Ik parameter (median or full)
  do_median_ik_ = 
    String_Op::to_number<bool>( parameters->value( "Median_Ik_Flag.value" ) );
 

  //-------------
  // The search neighborhood parameters

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


  //-------------
  // Variogram (covariance) and search ellipsoid initialization 
  if( do_median_ik_ ) {

    // We're doing median ik: we only need one covariance and one 
    // search neighborhood

    covar_vector_.resize( 1 );
    bool init_cov_ok =
      geostat_utils::initialize_covariance( &covar_vector_[0], 
                                            "Variogram_Median_Ik", 
	                        		              parameters, errors );
    if( !init_cov_ok ) return false;

	if( !harddata_grid_ || assign_harddata ) {
		neighborhood_ = SmartPtr<Neighborhood>( 
						simul_grid_->neighborhood( ranges, angles, &covar_vector_[0] ) 
						);
	}
	else {
		Neighborhood* simul_neigh = 
		simul_grid_->neighborhood( ranges, angles, &covar_vector_[0] );
		Neighborhood* harddata_neigh = 
		harddata_grid_->neighborhood( ranges, angles, &covar_vector_[0] );
		harddata_neigh->max_size( max_neigh  );
		harddata_neigh->select_property( harddata_property_->name() );

		neighborhood_ = 
		SmartPtr<Neighborhood>( new Combined_neighborhood( harddata_neigh,
																		simul_neigh, &covar_vector_[0]) );
	}


    neighborhood_->max_size( max_neigh );
  }
  else {
    // we're doing full ik: we need as many covariances and neighborhoods as
    // indicators

    covar_vector_.resize( ccdf_->size() );
    neighborhoods_vector_.resize( ccdf_->size() );

    // initialize all the covariances
    bool init_cov_ok =
      geostat_utils::initialize_covariance( &covar_vector_[0], 
                                            "Variogram_Full_Ik", 
	                        		              parameters, errors );
    if( !init_cov_ok ) return false;

    for( int i=1; i < ccdf_->size(); i++ ) {
      std::string tagname = "Variogram_Full_Ik_" + String_Op::to_string( i+1 );
      init_cov_ok =
        geostat_utils::initialize_covariance( &covar_vector_[i], tagname, 
			                                        parameters, errors );
      if( !init_cov_ok ) return false;
    }
    

    // initialize all the neighborhoods. 

    for( int j=0; j < ccdf_->size(); j++ ) {
    
     SmartPtr<Neighborhood> coded_neigh;
      if( is_data_coded_ ) {
        coded_neigh = SmartPtr<Neighborhood>(
                   coded_grid_->neighborhood(ranges, angles, &covar_vector_[j] ));
        coded_neigh->select_property( coded_props[j]->name() );
        coded_neigh->max_size( max_neigh );
      } else {
        coded_neigh = SmartPtr<Neighborhood>( new DummyNeighborhood() );
      }


		if( !harddata_grid_ || assign_harddata ) {

        SmartPtr<Neighborhood> simul_neigh = SmartPtr<Neighborhood>(
            simul_grid_->neighborhood( ranges, angles, &covar_vector_[j] ));

			  SmartPtr<Neighborhood>  neighborhood = 
			  SmartPtr<Neighborhood>( new Combined_neighborhood_dedup( coded_neigh,
																			  simul_neigh, &covar_vector_[j],true) );
        neighborhood->max_size( max_neigh );
        neighborhoods_vector_[j] = NeighborhoodHandle( neighborhood );

		}
		else {

      Neighborhood* simul_neigh = 
			  simul_grid_->neighborhood( ranges, angles, &covar_vector_[j] );
      simul_neigh->max_size( max_neigh );
			Neighborhood* harddata_neigh = 
			  harddata_grid_->neighborhood( ranges, angles, &covar_vector_[j] );
			harddata_neigh->max_size( max_neigh );
			harddata_neigh->select_property( harddata_property_->name() );

        SmartPtr<Neighborhood> hard_soft_data_neigh = SmartPtr<Neighborhood>(
                    new Combined_neighborhood( harddata_neigh,
												coded_neigh, &covar_vector_[j] ) );
        hard_soft_data_neigh->max_size( max_neigh );
			  SmartPtr<Neighborhood> neighborhood = 
			  SmartPtr<Neighborhood>( new Combined_neighborhood_dedup( hard_soft_data_neigh,
																			simul_neigh, &covar_vector_[j],false ) );

        neighborhood->max_size( max_neigh );
        neighborhoods_vector_[j] = NeighborhoodHandle( neighborhood );
		}

		
   }
  }



  //-------------
  // Set-up the cdf estimator

  if( categorical ) {
    Indicator<double> indicator( new Class_indicator_function<double> );
/*    cdf_estimator_ = 
      new CdfEstimator( covar_vector_.begin(), covar_vector_.end(),
			                  marginal_->p_begin(), marginal_->p_end(),
			                  indicator );*/
    cdf_estimator_ =
      new CdfSoftEstimator( covar_vector_.begin(), covar_vector_.end(),
			                  marginal_->p_begin(), marginal_->p_end(),
			                  indicator,harddata_property_ );
  }
  else {
    // The user has no pre-coded data;
/*    if( !is_data_coded_ ) {
      Indicator<double> indicator( new Indicator_function<double> );
      cdf_estimator_ = 
        new CdfEstimator( covar_vector_.begin(), covar_vector_.end(),
                  			marginal_->p_begin(), marginal_->p_end(),
			                  indicator );
    }
    // There is some pre-coded data
    else {*/
      Indicator<double> indicator( new Indicator_function<double> );
      //cdf_soft_estimator_ = 
      cdf_estimator_ = 
        new CdfSoftEstimator( covar_vector_.begin(), covar_vector_.end(),
                  			marginal_->p_begin(), marginal_->p_end(),
			                  indicator,harddata_property_ );
   // }
      
  }
    

  //-------------------------------
  // Done!

  if( !errors->empty() ) 
    return false;

  return true;
}


