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

#include <GsTLAppli/geostat/cosisim.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_property_manager.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/math/random_numbers.h>
#include <GsTLAppli/appli/utilities.h>

#include <GsTL/cdf_estimator/median_ik_cdf_estimator.h>
#include <GsTL/cdf_estimator/full_ik_cdf_estimator.h>
#include <GsTL/cdf/categ_non_param_cdf.h>
#include <GsTL/cdf/non_param_cdf.h>
#include <GsTL/sampler/monte_carlo_sampler.h>
#include <GsTL/univariate_stats/build_cdf.h>
#include <GsTL/utils/debug_tools.h>


int Cosisim::execute( GsTL_project* ) {
  // Initialize the global random number generator
  Global_random_number_generator::instance()->seed( seed_ );

  // Set up a progress notifier	
  int total_steps = simul_grid_->size() * (nb_of_realizations_);
  int frequency = std::max( total_steps / 20, 1 );
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running CoSisim", 
			    total_steps, frequency );


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

    typedef Geostat_grid::random_path_iterator RandomPathIterator;
    RandomPathIterator path_begin = simul_grid_->random_path_begin();
    RandomPathIterator path_end = simul_grid_->random_path_end();

    int status = 0;
    if( do_median_ik_ )
      status = median_ik( path_begin, path_end, progress_notifier.raw_ptr() );
    else
      status = full_ik( path_begin, path_end, progress_notifier.raw_ptr() );

    reset_indicator_properties();

    // check if the simulation was aborted
    if( status == 1 ) {
      clean( prop );
      return 1;
    }

  } // end loop on realizations

  remove_temporary_properties();
  return 0;
}
 



int Cosisim::median_ik( RandomPath begin, RandomPath end,
                        Progress_notifier* progress_notifier ) {

  typedef MedianIK_cdf_estimator< geostat_utils::CoKrigingConstraints,
                                  geostat_utils::CoKrigingCombiner, 
                                  MarkovBayesCovariance > MedianIkEstimator;
  MedianIkEstimator cdf_estimator( marginal_->p_begin(),marginal_->p_end(),
                                   covariances_[0],
                                   combiners_.begin(), combiners_.end(),
                                   *kconstraints_ );

  // set up the sampler
  Random_number_generator gen;
  Monte_carlo_sampler_t< Random_number_generator > sampler( gen );

 
  // this vector will contain the indicies of the indicators to be estimated
  // at a given grid node
  std::vector< unsigned int > unestimated_indicators;

  for( ; begin != end ; ++begin ) {
    if( !progress_notifier->notify() ) return 1;

    get_current_local_cdf( *begin, unestimated_indicators );
    if( unestimated_indicators.empty() ) {
      sampler( *begin, *ccdf_ );
      continue;
    }
     
    geostat_utils::NeighborhoodVector neighborhoods =
      median_ik_find_neighbors( *begin, unestimated_indicators );
    
    DEBUG_PRINT_LOCATION( "center", begin->location() );
    DEBUG_PRINT_NEIGHBORHOOD( "hard", &neighborhoods[0] );
    DEBUG_PRINT_NEIGHBORHOOD( "soft", &neighborhoods[1] );

    clear_ccdf();
    int status = cdf_estimator( *begin,
                                neighborhoods.begin(), neighborhoods.end(),
                                *ccdf_ );
 
    sampler( *begin, *ccdf_ );
    code_into_indicators( *begin );
  }
 
  return 0;
}



int Cosisim::full_ik( RandomPath begin, RandomPath end,
                      Progress_notifier* progress_notifier ) {
  typedef FullIK_cdf_estimator< geostat_utils::CoKrigingConstraints,
                                geostat_utils::CoKrigingCombiner, 
                                MarkovBayesCovariance > FullIkEstimator;
  FullIkEstimator cdf_estimator( marginal_->p_begin(),marginal_->p_end(),
                                 covariances_.begin(), covariances_.end(),
                                 combiners_.begin(), combiners_.end(),
                                 *kconstraints_ );

  // set up the sampler
  Random_number_generator gen;
  Monte_carlo_sampler_t< Random_number_generator > sampler( gen );

  
  // this vector will contain the indicies of the indicators to be estimated
  // at a given grid node
  std::vector< unsigned int > unestimated_indicators;
  
  for( ; begin != end ; ++begin ) {
    if( !progress_notifier->notify() ) return 1;

    get_current_local_cdf( *begin, unestimated_indicators );
    if( unestimated_indicators.empty() ) {
      sampler( *begin, *ccdf_ ); 
      continue;
    }
   
    geostat_utils::NeighborhoodVector neighborhoods =
      full_ik_find_neighbors( *begin, unestimated_indicators );

    DEBUG_PRINT_LOCATION( "center", begin->location() );
    DEBUG_PRINT_NEIGHBORHOOD( "hard1", &neighborhoods[0] );
    DEBUG_PRINT_NEIGHBORHOOD( "soft1", &neighborhoods[1] );
    DEBUG_PRINT_NEIGHBORHOOD( "hard2", &neighborhoods[2] );
    DEBUG_PRINT_NEIGHBORHOOD( "soft2", &neighborhoods[3] );

    GsTLPoint watch( 52, 95, 13 );
    if( begin->location() == watch ) {
      std::cout << "found" << std::endl;
    }

    clear_ccdf();
    int status = cdf_estimator( *begin,
                                neighborhoods.begin(), neighborhoods.end(),
                                *ccdf_ );

    sampler( *begin, *ccdf_ );
    code_into_indicators( *begin );
  }

  return 0;
}


  

void Cosisim::
get_current_local_cdf( const Geovalue& g,
                       std::vector< unsigned int >& unestimated_indicators ) {
  unestimated_indicators.clear();
  int node_id = g.node_id();
  Ccdf_type::p_iterator prob_it = ccdf_->p_begin();

  for( unsigned int i = 0 ; i < indicators_.size() ; ++prob_it, ++i ) {
    if( indicators_[i]->is_harddata( node_id ) && 
        indicators_[i]->is_informed( node_id ) )
      *prob_it = indicators_[i]->get_value( node_id );
    else
      unestimated_indicators.push_back( i );
  }
}



void Cosisim::code_into_indicators( Geovalue& g ) {
  float val = g.property_value();
  int node_id = g.node_id();

  for( unsigned int i= 0 ; i < indicators_.size(); i++ ) {
    if( !indicators_[i]->is_harddata( node_id ) ) {
      float thresh = *(marginal_->z_begin() + i);
      indicators_[i]->set_value( indicator_coder_( val, thresh ), node_id ); 
    }
  }
}



void Cosisim::clear_ccdf() {
  for( Ccdf_type::p_iterator it = ccdf_->p_begin() ; it != ccdf_->p_end() ; ++it )
    *it = Ccdf_type::NaN;
}



geostat_utils::NeighborhoodVector Cosisim::
median_ik_find_neighbors( const Geovalue& center,
                         std::vector< unsigned int >& unestimated_indicators ) {
  geostat_utils::NeighborhoodVector neighborhoods;

  // search the neighbors. Since we're doing median ik, we actually
  // perform the search for the first property only, the other neighborhoods
  // are simply copied. The resulting neighborhoods are stored into vector
  // "neighborhoods"
  unsigned int first_indicator = unestimated_indicators[0];
  hard_neighborhoods_[ first_indicator ].find_neighbors( center );
  soft_neighborhoods_[ first_indicator ].find_neighbors( center );
  
  neighborhoods.push_back( hard_neighborhoods_[ first_indicator ] );
  neighborhoods.push_back( soft_neighborhoods_[ first_indicator ] );

  Neighborhood::const_iterator hard_begin = 
    hard_neighborhoods_[ first_indicator ].begin();
  Neighborhood::const_iterator hard_end = 
    hard_neighborhoods_[ first_indicator ].end();
  Neighborhood::const_iterator soft_begin = 
    soft_neighborhoods_[ first_indicator ].begin();
  Neighborhood::const_iterator soft_end = 
    soft_neighborhoods_[ first_indicator ].end();

  for( unsigned int i = 1; i < unestimated_indicators.size(); i++ ) {
    unsigned int indicator_id = unestimated_indicators[i];
    hard_neighborhoods_[indicator_id].set_neighbors( hard_begin, hard_end );
    soft_neighborhoods_[indicator_id].set_neighbors( soft_begin, soft_end );
    
    neighborhoods.push_back( hard_neighborhoods_[indicator_id] );
    neighborhoods.push_back( soft_neighborhoods_[indicator_id] );
  }

  return neighborhoods;
}




geostat_utils::NeighborhoodVector Cosisim::
full_ik_find_neighbors( const Geovalue& center,
                        std::vector< unsigned int >& unestimated_indicators ) {
  geostat_utils::NeighborhoodVector neighborhoods;

  // search the neighbors for each unestimated indicator
  
  for( unsigned int i = 0 ; i < unestimated_indicators.size() ; i++ ) {
    unsigned int indicator_id = unestimated_indicators[i];
    hard_neighborhoods_[indicator_id].find_neighbors( center );
    soft_neighborhoods_[indicator_id].find_neighbors( center );
    
    neighborhoods.push_back( hard_neighborhoods_[indicator_id] );
    neighborhoods.push_back( soft_neighborhoods_[indicator_id] );
  }

  return neighborhoods;
}



void Cosisim::reset_indicator_properties() {
  for( unsigned int i = 0; i < indicators_.size() ; i++ ) {
    GsTLGridProperty* prop = indicators_[i];
    for( unsigned int j = 0; j < prop->size() ; j++ ) {
      if( !prop->is_harddata( j ) )
        prop->set_not_informed( j );
    }
  }
}



void Cosisim::remove_temporary_properties() {
  if( !simul_grid_ ) return;

  for( int i = 0; i < indicators_.size() ; i++ ) {
    if( indicators_[i] )
      simul_grid_->remove_property( indicators_[i]->name() );
    indicators_[i] = 0;
  }
}


void Cosisim::clean( GsTLGridProperty* prop ) {
  simul_grid_->remove_property( prop->name() );
  remove_temporary_properties();
}

  
bool Cosisim::initialize( const Parameters_handler* parameters, 
                          Error_messages_handler* errors ) {

  OPEN_DEBUG_STREAM( "cosisim.dbg" );

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



  int nb_indicators =
    String_Op::to_number<int>( parameters->value( "Nb_Indicators.value" ) );

    
  //-------------
  // The cdf parameters (# of thresholds, marginal, ...)

  bool categorical = 
    String_Op::to_number<bool>( 
		    parameters->value( "Categorical_Variable_Flag.value" )
		  );
  
  std::string marginal_probabilities_string = 
    parameters->value( "Marginal_Probabilities.value" );
  std::vector<double> marginal_probs = 
    String_Op::to_numbers<double>( marginal_probabilities_string );

    errors->report( marginal_probs.size() != nb_indicators,
                    "Marginal_Probabilities", 
                    "Enter one probability value per indicator" );

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

    // Set up the extrapolation tails
    geostat_utils::set_cdf_extrapolation_tail(parameters,errors, 
     *((Non_param_cdf<>*) ccdf_), "lowerTailCdf", "upperTailCdf");
    geostat_utils::set_cdf_extrapolation_tail(parameters,errors, 
     *((Non_param_cdf<>*) marginal_), "lowerTailCdf", "upperTailCdf");
  }

  

  //-------------
  // The hard data parameters

  std::string prim_harddata_grid_name = 
    parameters->value( "Primary_Harddata_Grid.value" );

  Geostat_grid* prim_harddata_grid = 0;
  Geostat_grid* sec_harddata_grid = 0;

  if( !prim_harddata_grid_name.empty() ) {
    // Get the harddata grid from the grid manager
    bool ok = geostat_utils::create( prim_harddata_grid, prim_harddata_grid_name, 
      	                           	 "Primary_Harddata_Grid", errors );
    if( !ok ) return false;
    
    if( !dynamic_cast<Point_set*>(prim_harddata_grid) && 
        prim_harddata_grid != simul_grid_ ) {
      std::ostringstream error_message;
      error_message << "the grid should either be the same as the simulation grid "
                    << "or be a set of points";
      errors->report( std::string( "Primary_Harddata_Grid" ), 
                      error_message.str() );
    }

    // get the properties
    std::string primary_indicators_str =
      parameters->value( "Primary_Indicators.value" );
    std::vector<std::string> primary_indicators_names = 
      String_Op::decompose_string( primary_indicators_str, ";", false );

    for( unsigned int i= 0; i < primary_indicators_names.size() ; i++ ) {
      primary_indicators_.push_back( 
          prim_harddata_grid->property( primary_indicators_names[i] )
        );
    }

    if( nb_indicators != primary_indicators_names.size() ) {
      std::ostringstream message;
      message << "Specify exactly " << nb_indicators << " properties";
      errors->report( std::string("Primary_Indicators"), message.str() );
    }
  }


  std::string sec_harddata_grid_name = 
    parameters->value( "Secondary_Harddata_Grid.value" );
  if( !sec_harddata_grid_name.empty() ) {

    // Get the harddata grid from the grid manager
    bool ok2 = geostat_utils::create( sec_harddata_grid, sec_harddata_grid_name, 
  	                           	     "Secondary_Harddata_Grid", errors );
    if( !ok2 ) return false;

    if( !dynamic_cast<Point_set*>( sec_harddata_grid ) && 
        sec_harddata_grid != simul_grid_ ) {
      std::ostringstream error_message;
      error_message << "the grid should either be the same as the simulation grid "
                    << "or be a set of points";
      errors->report( std::string( "Secondary_Harddata_Grid" ),
                      error_message.str() );
    }

    
    std::string secondary_indicators_str =
      parameters->value( "Secondary_Indicators.value" );
    errors->report( secondary_indicators_str.empty(), 
                    "Secondary_Indicators", "No property names specified" );

    std::vector<std::string> secondary_indicators_names = 
      String_Op::decompose_string( secondary_indicators_str, ";", false );

    for( unsigned int i= 0; i < secondary_indicators_names.size() ; i++ ) {
      secondary_indicators_.push_back( 
          sec_harddata_grid->property( secondary_indicators_names[i] )
        );
    }

    if( nb_indicators != secondary_indicators_names.size() ) {
      std::ostringstream message;
      message << "Specify exactly " << nb_indicators << " properties";
      errors->report( std::string("Secondary_Indicators"), message.str() );
    }
  }


  // check if errors were reported so far
  if( !errors->empty() ) return false;


  //--------------------
  // Create K new properties on the simulation grid. Those are the properties
  // the simulation algorithm will be working on for each realization
  for( unsigned int j = 0 ; j < nb_indicators ; j++ ) {
    std::ostringstream prop_name;
    prop_name << "__tmp_indicator_" << j;
    GsTLGridProperty* prop = 
      geostat_utils::add_property_to_grid( simul_grid_, prop_name.str() );
    indicators_.push_back( prop );
  }

  //--------------------
  // Currently, we always assign the hard data to the simulation grid
  if( prim_harddata_grid ) {
    property_copier_ = 
      Property_copier_factory::get_copier( prim_harddata_grid, simul_grid_ );

    if( !property_copier_ ) {
      std::ostringstream message;
      message << "It is currently not possible to copy a property from a "
              << prim_harddata_grid->classname() << " to a " 
              << simul_grid_->classname() ;
      errors->report( !property_copier_, "Assign_Hard_Data", message.str() );
      return false;
    }
    
    //initializer_ = new Grid_initializer( simul_grid_, false );
    for( unsigned int i = 0 ; i < primary_indicators_.size() ; i++ ) {
      property_copier_->copy( prim_harddata_grid, primary_indicators_[i],
                              simul_grid_, indicators_[i] );
    }
  }


  //-------------
  // Number of realizations and random number seed
  nb_of_realizations_ = 
    String_Op::to_number<int>( parameters->value( "Nb_Realizations.value" ) );
  
  seed_ = String_Op::to_number<int>( parameters->value( "Seed.value" ) );




  //-------------
  // Set-up the indicator coding function

  if( categorical ) 
    indicator_coder_ = Indicator<float>( new Class_indicator_function<float> );
  else 
    indicator_coder_ = Indicator<float>( new Indicator_function<float> );

  

  //-------------
  // The Ik parameter (median or full)
  do_median_ik_ = 
    String_Op::to_number<bool>( parameters->value( "Median_Ik_Flag.value" ) );
 


  //-------------
  // The search neighborhood parameters

  int prim_max_neigh = 
    String_Op::to_number<int>( 
      parameters->value( "Max_Conditioning_Data_Primary.value" )
    );
  
  GsTLTriplet prim_ellips_ranges;
  GsTLTriplet prim_ellips_angles;
  bool prim_extract_ok =
    geostat_utils::extract_ellipsoid_definition( prim_ellips_ranges, 
                                                 prim_ellips_angles,
		  			                                     "Search_Ellipsoid_Primary.value",
			  		                                     parameters, errors );
  if( !prim_extract_ok ) return false;


  GsTLTriplet sec_ellips_ranges;
  GsTLTriplet sec_ellips_angles;
  int sec_max_neigh = 0;
  if( sec_harddata_grid ) {
    sec_max_neigh = 
      String_Op::to_number<int>( 
        parameters->value( "Max_Conditioning_Data_Secondary.value" ) 
      );
  
    bool sec_extract_ok = 
      geostat_utils::extract_ellipsoid_definition( sec_ellips_ranges, 
                                                   sec_ellips_angles,
		  	  		                                     "Search_Ellipsoid_Secondary.value",
			  	  	                                     parameters, errors );
  
    if( !sec_extract_ok ) return false;
  }


  //-------------
  // Variogram (covariance) initialization 
  // We distinguish two cases: 
  //   - median ik: there is only one covariance-set 
  //   - full ik  : there are as many covariances as indicators

  std::vector< CovarianceType > covar_vector;
  std::vector<double> Bz_values;
  if( sec_harddata_grid ) {
    std::string Bz_string = parameters->value( "Bz_Values.value" );
    Bz_values = String_Op::to_numbers<double>( Bz_string );
  }
  else
    Bz_values.assign( nb_indicators, 0.0 );

  
  if( do_median_ik_ ) {
    CovarianceType cov;
    bool init_cov_ok =
      geostat_utils::initialize_covariance( &cov, "Variogram_Median_Ik", 
	  		                                    parameters, errors );
    if( !init_cov_ok ) return false;

    covar_vector.assign( nb_indicators, cov );
    errors->report( Bz_values.empty(), "Bz_Values", "No values for Bz" );
    covariances_.push_back( MarkovBayesCovariance(covar_vector[0], Bz_values[0]) );

  }
  else {
    // We are doing full ik

    covar_vector.resize( nb_indicators );
    if( Bz_values.size() != nb_indicators ) {
      errors->report( "Bz_Values", "Enter one Bz value per indicator" );
      return false;
    }

    // initialize all the covariances
    bool init_cov_ok =
      geostat_utils::initialize_covariance( &covar_vector[0], "Variogram_Full_Ik", 
	  	                      	              parameters, errors );
    if( !init_cov_ok ) return false;
    covariances_.push_back( MarkovBayesCovariance(covar_vector[0], Bz_values[0]) );


    for( int i=1; i < nb_indicators ; i++ ) {
      std::ostringstream tagname;
      tagname << "Variogram_Full_Ik_"  << i+1 ;
      init_cov_ok =
        geostat_utils::initialize_covariance( &covar_vector[i], tagname.str(), 
                        			                parameters, errors );
      if( !init_cov_ok ) return false;
      covariances_.push_back( MarkovBayesCovariance( covar_vector[i], 
                                                     Bz_values[i] )   );
    }
  }


  //--------------------
  // neighborhoods initialization
  init_neighborhoods( simul_grid_, sec_harddata_grid,
                      prim_ellips_ranges, prim_ellips_angles,
                      sec_ellips_ranges, sec_ellips_angles,
                      prim_max_neigh, sec_max_neigh,
                      covar_vector );


  //---------------------
  // Kriging constraints and kriging combiner
  geostat_utils::Kriging_type ktype = 
    geostat_utils::kriging_type( "Kriging_Type.value", parameters, errors );  
  
  if( ktype == geostat_utils::SK ) {
    for( int i = 0; i < nb_indicators ; i++ ) {
      typedef geostat_utils::WeightIterator Iter1;
      typedef geostat_utils::NeighIterator  Iter2;
      typedef CoSK_combiner<Iter1,Iter2> CoSKCombiner;

      std::vector<double> means;
      // use the same mean for the primary and the secondary data: they are both
      // P(I(u)=1)
      means.assign( 2, *(marginal_->p_begin() + i) );
      combiners_.push_back( 
        geostat_utils::CoKrigingCombiner( new CoSKCombiner( means ) )
       );
    }

    Co_SKConstraints_impl< geostat_utils::NeighIterator, 
                           geostat_utils::Location > sk_constr;
    kconstraints_ = new geostat_utils::CoKrigingConstraints( &sk_constr );
  }
  else {
    combiners_.resize( nb_indicators );
    kconstraints_ = new geostat_utils::CoKrigingConstraints;
  }


  //-------------------------------
  // Done!

  if( !errors->empty() )
    return false;

  return true;
}





void Cosisim::init_neighborhoods( Geostat_grid* hard_grid, Geostat_grid* soft_grid, 
                                  const GsTLTripletTmpl<double>& hard_ranges, 
                                  const GsTLTripletTmpl<double>& hard_angles,
                                  const GsTLTripletTmpl<double>& soft_ranges, 
                                  const GsTLTripletTmpl<double>& soft_angles,
                                  int hard_max_neighbors, int soft_max_neighbors,
                                  std::vector< CovarianceType >& covariances ) {

  int nb_indicators = indicators_.size();

  // get the neighborhoods on the primary variable
  for( int i = 0 ; i < nb_indicators ; i++ ) {
    SmartPtr<Neighborhood> neighborhood = 
      SmartPtr<Neighborhood>( 
        hard_grid->neighborhood( hard_ranges, hard_angles,
                                 &covariances[i] )  );
    neighborhood->select_property( indicators_[i]->name() );
    neighborhood->max_size( hard_max_neighbors );
    hard_neighborhoods_.push_back( NeighorhoodType( neighborhood ) );
  }

  // get the neighborhoods on the secondary variable
  if( soft_grid ) {
    for( int j = 0 ; j < nb_indicators ; j++ ) {
      appli_assert( nb_indicators == secondary_indicators_.size() );
      
      SmartPtr<Neighborhood> neighborhood = 
        SmartPtr<Neighborhood>( 
          soft_grid->neighborhood( soft_ranges, soft_angles,
                                   &covariances[j] )  );
      neighborhood->select_property( secondary_indicators_[j]->name() );
      neighborhood->max_size( soft_max_neighbors );
      soft_neighborhoods_.push_back( NeighorhoodType( neighborhood ) );
    }
  }
  else {
    // there are no soft data. Use DummyNeighborhoods
    for( int j = 0 ; j < nb_indicators ; j++ ) {
      SmartPtr<Neighborhood> neighborhood( new DummyNeighborhood );
      soft_neighborhoods_.push_back( NeighorhoodType( neighborhood ) );
    }
  }
}



Cosisim::Cosisim() {
  simul_grid_ = 0;
  multireal_property_ = 0;
  ccdf_ = 0;
  marginal_ = 0;
  kconstraints_ = 0;

}


Cosisim::~Cosisim() {
  delete ccdf_;
  delete marginal_;
  delete kconstraints_;

  remove_temporary_properties();
}


Named_interface* Cosisim::create_new_interface( std::string& ) {
  return new Cosisim;
}
 
