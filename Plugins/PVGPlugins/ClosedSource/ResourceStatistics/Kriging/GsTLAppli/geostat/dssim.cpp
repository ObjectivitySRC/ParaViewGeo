/**********************************************************************
** Author: Alexandre Boucher, Nicolas Remy
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

#include <GsTLAppli/geostat/dssim.h>
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

//#include <GsTL/cdf/gaussian_cdf.h>
#include <GsTL/cdf/non_param_cdf.h>
#include <GsTL/sampler/monte_carlo_sampler.h>
#include <GsTL/cdf_estimator/first2_moments_cdf_Kestimator.h>
#include <GsTL/simulation/sequential_simulation.h>
//#include <GsTL/univariate_stats/cdf_transform.h>
#include <GsTL/univariate_stats/build_cdf.h>

#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>


int dssim::execute( GsTL_project* ) {
  
  // Initialize the global random number generator
  Global_random_number_generator::instance()->seed( seed_ );
  
  
  // Set up a progress notifier	
  int total_steps = simul_grid_->size() * (nb_of_realizations_);
  int frequency = std::max( total_steps / 20, 1 );
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running dssim", 
			    total_steps, frequency );

  // Initialize the marginal cdf for the direct sequential simulation
 // LogNormal_cdf marginal( 0.1, 1.0 );
 // LogNormal_cdf ccdf;

  // work on the fine grid
  if( dynamic_cast<Strati_grid*>( simul_grid_ ) ) {
    Strati_grid* sgrid = dynamic_cast<Strati_grid*>( simul_grid_ );
    sgrid->set_level( 1 );
  }

  // set up the cdf-estimator
  typedef First2_moments_cdf_Kestimator< Covariance<Location>,
                                   Neighborhood,
                                   geostat_utils::KrigingConstraints
                                  >    Kriging_cdf_estimator;
  Kriging_cdf_estimator cdf_estimator( covar_,
				       *Kconstraints_,
				       *combiner_ );

  // set up the sampler
  Random_number_generator gen;
  Monte_carlo_sampler_t< Random_number_generator > sampler( gen );
  
  // compute the random path
  simul_grid_->init_random_path();

  // loop on all realizations
  for( int nreal = 0; nreal < nb_of_realizations_ ; nreal ++ ) {

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
			     *ccdf_,
			     cdf_estimator,
			     *marginal_,
			     sampler, progress_notifier.raw_ptr()
			     );
    if( status == -1 ) {
      clean( prop );
      return 1;
    }
  }

  clean();

  return 0;
}





bool dssim::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) {

  OPEN_DEBUG_STREAM( "dssim.dbg" );

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
  // hard data grid and property

  bool assign_harddata = 
    String_Op::to_number<bool>( parameters->value( "Assign_Hard_Data.value" ) );
  if( harddata_grid_ == NULL ) assign_harddata=false; 
  else if( harddata_grid_ == simul_grid_ ) assign_harddata=true;

  if( harddata_grid_ ) {
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
    Neighborhood* harddata_neigh = 
      harddata_grid_->neighborhood( ranges, angles, &covar_ );
    harddata_neigh->max_size( max_neigh );
    harddata_neigh->select_property( harddata_property_->name() );

    neighborhood_ = 
      SmartPtr<Neighborhood>( new Combined_neighborhood_dedup( harddata_neigh,
							                                           simul_neigh, &covar_, false ) );
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
  if(ktype == geostat_utils::SK ) 
	  defaults[ geostat_utils::SK ] = parameters->value("SK_mean.value");
 
	
  geostat_utils::initialize( ktype, combiner_, Kconstraints_,
                             tags_map,
                             parameters, errors,
                             simul_grid_, defaults );


// Type and parametrization of the cdf
  get_cdf( parameters, errors  );
  //ccdf_ =  get_cdf( parameters, errors );




  //----------------
  // Report errors if any

  if( !errors->empty() )
    return false;

  return true;
}



void dssim::get_cdf( const Parameters_handler* parameters,
		Error_messages_handler* errors )
{
	//First2_moments_cdf* cdf;
  bool is_Deutsch = ( parameters->value( "cdf_type.value" ) == "Deutsch" );
	bool is_Soares = ( parameters->value( "cdf_type.value" ) == "Soares" );
	bool is_Uniform = ( parameters->value( "cdf_type.value" ) == "Uniform" );
	bool is_logNormal = ( parameters->value( "cdf_type.value" ) == "LogNormal" );

  if( is_Deutsch )
  {
    Non_param_cdf<>* cdf_non_param = new Non_param_cdf<>();
    bool ok = geostat_utils::get_non_param_cdf(*cdf_non_param,parameters,
              errors,"nonParamCdf_deutsch");
    int mean_bins = String_Op::to_number<int>(parameters->value("mean_bins.value"));
    int var_bins = String_Op::to_number<int>(parameters->value("var_bins.value"));
		Deutsch_cdf< Non_param_cdf<> >* deutsch_cdf = 
			new Deutsch_cdf< Non_param_cdf<> >( cdf_non_param, mean_bins,var_bins  );

		ccdf_ = dynamic_cast<First2_moments_cdf*>(deutsch_cdf);
		marginal_ = dynamic_cast<Cdf<float>*>(cdf_non_param);

  }
	else if( is_Soares ) 
	{
    Non_param_cdf<>* cdf_non_param = new Non_param_cdf<>();
    bool ok = geostat_utils::get_non_param_cdf(*cdf_non_param,parameters,
              errors,"nonParamCdf");
		Soares_cdf< Non_param_cdf<> >* soares_cdf = 
			new Soares_cdf< Non_param_cdf<> >( cdf_non_param );

		ccdf_ = dynamic_cast<First2_moments_cdf*>(soares_cdf);
		marginal_ = dynamic_cast<Cdf<float>*>(cdf_non_param);
	}
	else if(is_Uniform)
	{
		std::string a_str = parameters->value( "U_min.value" );
	    std::string b_str = parameters->value( "U_max.value" );
		errors->report( a_str.empty()|| b_str.empty(), "UniformBox", 
				"No mean or variance provided" );

		Uniform_cdf* unif_cdf = new Uniform_cdf;
		unif_cdf->set_ranges(String_Op::to_number<float>(a_str),
			String_Op::to_number<float>(b_str) );
		Uniform_cdf* unif_marginal = new Uniform_cdf;
		unif_marginal->set_ranges(String_Op::to_number<float>(a_str),
			String_Op::to_number<float>(b_str) );

		ccdf_ = dynamic_cast<First2_moments_cdf*>(unif_cdf);
		marginal_ = dynamic_cast<Cdf<float>*>(unif_marginal);

		
	} else if(is_logNormal ) 
	{
		std::string mean_str = parameters->value( "LN_mean.value" );
	    std::string var_str = parameters->value( "LN_variance.value" );
		errors->report( mean_str.empty()|| var_str.empty(), "LogNormalBox", 
				"No mean or variance provided" );
		errors->report( String_Op::to_number<float>(mean_str) <= 0, "LogNormalBox", 
				"The mean of a logNormal distribution must be positive" );

		LogNormal_cdf* logN_cdf = new LogNormal_cdf(String_Op::to_number<float>(mean_str),
				String_Op::to_number<float>(var_str));
		LogNormal_cdf* logN_marginal = new LogNormal_cdf(String_Op::to_number<float>(mean_str),
				String_Op::to_number<float>(var_str));
		ccdf_ = dynamic_cast<First2_moments_cdf*>(logN_cdf);
		marginal_ = dynamic_cast<Cdf<float>*>(logN_marginal);
	}
	
	//return cdf;
}
/*
void dssim::set_extrapolation_tail(const Parameters_handler* parameters,
	Error_messages_handler* errors, Non_param_cdf<>* nparam_cdf)
{
	//Set the tail extrapolation functions
	std::string LTI_function_str = parameters->value( "LTI.value" );
	std::string UTI_function_str = parameters->value( "UTI.value" );
	// If the LTI is exponential, do nothing, it is so per default.
	if(LTI_function_str == "Power") {
		std::string power_min = parameters->value( "LTI_min.value" );
		errors->report( power_min.empty() , "LTI_min", 
		"A minimum value must be specified for the Power extrapolation function" );
		double z_min = String_Op::to_number< double >( power_min );
		
		std::string power_omega = parameters->value( "LTI_omega.value" );
		errors->report( power_omega.empty() , "LTI_omega", 
		"An omega value must be specified for the Power extrapolation function" );
		double omega = String_Op::to_number< double >( power_omega );

		nparam_cdf->lower_tail_interpolator( Tail_interpolator( new Power_LTI(z_min,omega) ) );
	}
	
	std::string uti_omega_str = parameters->value( "UTI_omega.value" );
	errors->report( uti_omega_str.empty() , "UTI_omega", 
	"An omega value must be specified for the upper tail extrapolation functions" );
	double uti_omega = String_Op::to_number< double >( uti_omega_str );
	
	if(UTI_function_str == "Hyperbolic" )
		nparam_cdf->upper_tail_interpolator( Tail_interpolator( new Hyperbolic_UTI(uti_omega) ) );
	else if(UTI_function_str == "Power")
	{
		std::string power_max = parameters->value( "UTI_max.value" );
		errors->report( power_max.empty() , "UTI_max", 
		"A maximum value must be specified for the Power extrapolation function" );
		double z_max = String_Op::to_number< double >( power_max );
		nparam_cdf->upper_tail_interpolator( Tail_interpolator( new Power_UTI(z_max,uti_omega) ) );
	}
}

*/


void dssim::clean( GsTLGridProperty* prop ) {
  if( prop ) 
    simul_grid_->remove_property( prop->name() );

//  if( use_target_hist_ && harddata_property_ && harddata_grid_ ) {
//    harddata_grid_->remove_property( harddata_property_->name() );
//    harddata_property_ = 0;
//  }
}



Named_interface* dssim::create_new_interface( std::string& ) {
  return new dssim;
}


dssim::dssim() {
  Kconstraints_ = 0;
  combiner_ = 0;
  simul_grid_ = 0;
  harddata_grid_ = 0;
  harddata_property_ = 0;
  neighborhood_ = 0;
  multireal_property_ = 0;

//  use_target_hist_ = false;
}
 

dssim::~dssim() {
  delete Kconstraints_;
  delete combiner_;

  clean(0);
}

