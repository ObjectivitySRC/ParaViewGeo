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

#include <GsTLAppli/geostat/indicator_kriging.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_property_manager.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/appli/utilities.h>

#include <GsTL/kriging/kriging_weights.h>
#include <GsTL/kriging/kriging_combiner.h>
#include <GsTL/kriging/SK_constraints.h>
#include <GsTL/cdf/non_param_cdf.h>
#include <GsTL/cdf/categ_non_param_cdf.h>



Named_interface* Indicator_kriging::create_new_interface( std::string& ) {
  return new Indicator_kriging;
}


Indicator_kriging::Indicator_kriging() {
  simul_grid_ = 0;
  multireal_property_ = 0;
  hdata_grid_ = 0;
  ccdf_ = 0;
  min_neigh_ = 0;
}

Indicator_kriging::~Indicator_kriging() {

}



int Indicator_kriging::execute( GsTL_project* ) {
  // Set up a progress notifier	
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running Indicator Kriging", 
			    simul_grid_->size() + 1,  
			    std::max( simul_grid_->size() / 100, 1 ) );

  if( do_median_ik_ ) 
    return median_ik( progress_notifier.raw_ptr() );
  else
    return full_ik( progress_notifier.raw_ptr() );
}


int Indicator_kriging::median_ik( Progress_notifier* progress_notifier ) {
  bool ok = true;
  
  // create all the properties we will populate
  std::vector< GsTLGridProperty* > simul_properties;
  for( int thres = 0; thres < thres_count_; thres++ ) {
    GsTLGridProperty* prop = multireal_property_->new_realization();
    simul_properties.push_back( prop );
  }

  std::vector<double> krig_weights;
  SK_constraints Kconstraints;
  typedef std::vector<double>::const_iterator weight_iterator;
  typedef SK_combiner< weight_iterator, Neighborhood > SKCombiner;

  // the following line could probably be omitted
  simul_grid_->select_property( simul_properties[0]->name() );

  Geostat_grid::iterator begin = simul_grid_->begin();
  Geostat_grid::iterator end = simul_grid_->end();
  
  
  for( ; begin != end; ++begin ) {
    if( !progress_notifier->notify() ) return 1;

    if( begin->is_informed() ) continue;
      
    neighborhood_->find_neighbors( *begin );
//    if( neighborhood_->is_empty() ){
    if( neighborhood_->size() < min_neigh_ ){
      //if we don't have any conditioning data, skip the node
      continue;
    }
    else {
      int status = kriging_weights( krig_weights,
				    begin->location(), 
				    *(neighborhood_.raw_ptr()),
				    covar_, Kconstraints );
      
      if(status == 0) {
	      // the kriging system could be solved
	      // Since we're using the same covariance and the 
	      // same neighborhood for all thresholds, we can re-use the same
	      // weights for all thresholds 

       	GsTLInt node_id = begin->node_id();
      	Non_parametric_cdf<float>::p_iterator p_it = ccdf_->p_begin();

      	for( int thres = 0; thres < thres_count_; thres++, ++p_it ) {

      	  // tell the neighbors to work on the correct property
	        for( Neighborhood::iterator it = neighborhood_->begin();
	             it != neighborhood_->end(); it++ ) {
      	    it->set_property_array( hdata_properties_[ thres ] );
	        }

    	    SKCombiner combiner( marginal_probs_[thres] );
	        double estimate = combiner( krig_weights.begin(), 
				                              krig_weights.end(),
				                              *(neighborhood_.raw_ptr()) );
	        *p_it = estimate;
	      }

      	// make sure the ccdf is a valid cdf
      	ccdf_->make_valid();

      	// output the ccdf probabilities to the grid properties
      	p_it = ccdf_->p_begin();
      	for( int thres2 = 0; thres2 < thres_count_; thres2++, ++p_it ) {
      	  simul_properties[ thres2 ]->set_value( *p_it, node_id );
      	}
      }
      else {
      	// the kriging system could not be solved, issue a warning and skip the
      	// node
        ok = false;
      }
    }
  }

  if( !ok )
    GsTLcerr << "The kriging system could not be solved for every node\n" << gstlIO::end; 


  return 0;
}




int Indicator_kriging::full_ik( Progress_notifier* progress_notifier ) {
  bool ok = true;
  bool order_relation_problems = false;

  // create all the properties we will populate
  std::vector< GsTLGridProperty* > simul_properties;
  for( int thres = 0; thres < thres_count_; thres++ ) {
    GsTLGridProperty* prop = multireal_property_->new_realization();
    simul_properties.push_back( prop );
  }

  std::vector<double> krig_weights;
  SK_constraints Kconstraints;
  typedef std::vector<double>::const_iterator weight_iterator;
  typedef SK_combiner< weight_iterator, Neighborhood > SKCombiner;

  // the following line could probably be omitted
  simul_grid_->select_property( simul_properties[0]->name() );

  Geostat_grid::iterator begin = simul_grid_->begin();
  Geostat_grid::iterator end = simul_grid_->end();

  // loop over the grid

  for( ; begin != end; ++begin ) {
    if( !progress_notifier->notify() ) return 1;

    if( begin->is_informed() ) continue;
    

    // for each threshold / class:
    Non_parametric_cdf<float>::p_iterator p_it = ccdf_->p_begin();
    for( int thres = 0; thres < thres_count_; thres++, ++p_it ) {
      neighborhoods_vector_[thres]->find_neighbors( *begin );
 
      if( neighborhoods_vector_[thres]->is_empty() ){
        //if we don't have any conditioning data, use the marginal
        *p_it = marginal_probs_[thres];
        continue;
      }
      
      int status = kriging_weights( krig_weights,
	                          			  begin->location(), 
				                            *(neighborhoods_vector_[thres].raw_ptr()),
				                            covar_vector_[thres], Kconstraints );
      
      if( status != 0 ) {
        // the kriging system could not be solved, issue a warning and skip the
        // node
        ok = false;
        *p_it = marginal_probs_[thres];
        continue;
      }
      
      SKCombiner combiner( marginal_probs_[thres] );
      double estimate = combiner( krig_weights.begin(), 
		                          		krig_weights.end(),
			  	                        *(neighborhoods_vector_[thres].raw_ptr()) );
      *p_it = estimate;
    }

    // make sure the ccdf is a valid cdf
    if( !ccdf_->make_valid() ) {
      // there was a problem making the cdf a valid cdf:
      // leave the node un-estimated and set the flag so that an error will
      // be reported
      order_relation_problems = true; 
      continue;
    }

    GsTLInt node_id = begin->node_id();

    // output the ccdf probabilities to the grid properties
    p_it = ccdf_->p_begin();
    for( int thres2 = 0; thres2 < thres_count_; thres2++, ++p_it ) {
  	  simul_properties[ thres2 ]->set_value( *p_it, node_id );
    }
  }


  if( !ok )
    GsTLcerr << "The kriging system could not be solved for every node\n" 
             << gstlIO::end; 
  if( order_relation_problems ) {
    GsTLcerr << "A cdf could not be estimated for all nodes because of major "
             << "order-relation problems (all probabilities < 0 )" 
             << gstlIO::end;
  }

  return 0;
}



/*
int Indicator_kriging::full_ik() {

  std::vector<double> krig_weights;
  SK_constraints Kconstraints;
  typedef std::vector<double>::const_iterator weight_iterator;
  typedef SK_combiner< weight_iterator, Neighborhood > SKCombiner;

  // Do one ordinary kriging for each threshold
  // full_ik() could be made faster by re-using the neighborhood at a given
  // node for the different threshold instead of re-computing it every time.
  // Re-using the neighborhood is not straight-forward if at a given node,
  // we have a property value for one threshold but not for another threshold
  // (as is the case with inequality constraints)

  for( int thres = 0; thres < thres_count_; thres++ ) {

    SKCombiner combiner( marginal_probs_[thres] );

    // Get a new property
    GsTLGridProperty* prop = multireal_property_->new_realization();
    simul_grid_->select_property( prop->name() );

    Geostat_grid::iterator begin = simul_grid_->begin();
    Geostat_grid::iterator end = simul_grid_->end();
  
    for( ; begin != end; ++begin ) {
      std::cout << "center: " << begin->location() << "  ";
      if( begin->is_informed() ) continue;
      
      neighborhood_->find_neighbors( *begin );
    
      if( neighborhood_->is_empty() ){
	//if we don't have any conditioning data, skip the node
	continue;
      }
      else {
	std::cout << "neighbors: " ;
	for( Neighborhood::iterator it = neighborhood_->begin();
	     it != neighborhood_->end(); ++it ) {
	  std::cout << "(" << it->location() << "):" << it->property_value() << "  ";
	}
	
	int status = kriging_weights( krig_weights,
				      begin->location(), 
				      *(neighborhood_.raw_ptr()),
				      covar_vector_[thres], Kconstraints );
      
	if(status == 0) {
	  // the kriging system could be solved
	  // Since we're using the same covariance and the 
	  // same neighborhood for all thresholds, we can re-use the same
	  // weights for all thresholds 
	
	  double estimate = combiner( krig_weights.begin(), 
				      krig_weights.end(),
				      *(neighborhood_.raw_ptr()) );
	  begin->set_property_value( estimate );
	}
	else {
	  // the kriging system could not be solved, issue a warning and skip the
	  // node
	  GsTLcerr << "The kriging system could not be solved, skipping node\n" << gstlIO::end; 
	}
      }
    }
  }


  // make sure the estimated cdf are valid cdf's. Correct if necessary

  return 0;
}
*/





bool Indicator_kriging::initialize( const Parameters_handler* parameters,
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
  if( !simul_grid_name.empty() ) {
    bool ok = geostat_utils::create( simul_grid_, simul_grid_name,
				 "Grid_Name", errors );
    if( !ok ) return false;
  }
  
  multireal_property_ =
    simul_grid_->add_multi_realization_property( property_name );



  //-------------
  // The hard data parameters

  std::string harddata_grid_name = parameters->value( "Hard_Data_Grid.value" );
  errors->report( harddata_grid_name.empty(), 
		  "Hard_Data_Grid", "No hard data grid specified" );

  //  int hdata_prop_count = 
  //    String_Op::to_number<int>( parameters->value( "Hard_Data_Property.count" ) );
  std::string hdata_property_string = 
    parameters->value( "Hard_Data_Property.value" );
  errors->report( hdata_property_string.empty(), 
		  "Hard_Data_Property", "No properties specified" );
  
  // The hard-data properties are separated by one semi-colon
  std::vector< std::string > hdata_property_names =
    String_Op::decompose_string( hdata_property_string, ";", false );
  thres_count_ = hdata_property_names.size();

  // Get the harddata grid from the grid manager
  if( !harddata_grid_name.empty() ) {
    std::cout << " Get the harddata grid from the grid manager " << std::endl;
    geostat_utils::create( hdata_grid_, harddata_grid_name, 
		                       "Hard_Data", errors );
  }

  if( !hdata_grid_ ) {
    errors->report( "Hard_Data", "No valid hard data grid specified" );
    return false;
  }


  for( int i=0; i < thres_count_ ; i++ ) {
    GsTLGridProperty* prop = hdata_grid_->property( hdata_property_names[i] );
    hdata_properties_.push_back( prop );
  }



  //-------------
  // The search neighborhood parameters

  int max_neigh = 
    String_Op::to_number<int>( parameters->value( "Max_Conditioning_Data.value" ) );
  int min_neigh_ = 
    String_Op::to_number<int>( parameters->value( "Min_Conditioning_Data.value" ) );
  errors->report( min_neigh_ >= max_neigh, 
		  "Min_Conditioning_Data", "Min must be less than Max" );
  
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
  // The cdf parameters (caategorical/continuous variable, marginal, ...)

  bool categorical = 
    String_Op::to_number<bool>( 
		    parameters->value( "Categorical_Variable_Flag.value" )
		    );
  
  std::string marginal_probabilities_string = 
    parameters->value( "Marginal_Probabilities.value" );
  marginal_probs_ = 
    String_Op::to_numbers<double>( marginal_probabilities_string );

  if( categorical ) {
    ccdf_ = new Categ_non_param_cdf<float>( marginal_probs_.size() );
  }
  else {
    // we don't need to know the thresholds of the cdf
    std::vector<float> dummy_thresholds( marginal_probs_.size(), 0.0 );
    
    ccdf_ = new Non_param_cdf<>( dummy_thresholds.begin(), 
				 dummy_thresholds.end() );
  }



  //-------------
  // The Ik parameter (median or full)
  do_median_ik_ = 
    String_Op::to_number<bool>( parameters->value( "Median_Ik_Flag.value" ) );
 

  //-------------
  // Variogram (covariance) and search ellipsoid initialization 
  if( do_median_ik_ ) {
    bool init_cov_ok =
      geostat_utils::initialize_covariance( &covar_, "Variogram_Median_Ik", 
	                        		              parameters, errors );
    if( !init_cov_ok ) return false;

    neighborhood_ = 
      SmartPtr<Neighborhood>( 
        hdata_grid_->neighborhood( ranges, angles, &covar_ )
       );
    neighborhood_->max_size( max_neigh );
	neighborhood_->select_property( hdata_properties_[0]->name() );
  }
  else {
    covar_vector_.resize( thres_count_ );
    neighborhoods_vector_.resize( thres_count_ );

    // initialize all the covariances
    bool init_cov_ok =
      geostat_utils::initialize_covariance( &covar_vector_[0], "Variogram_Full_Ik", 
	                        		              parameters, errors );
    if( !init_cov_ok ) return false;

    for( int i=1; i < thres_count_; i++ ) {
      std::string tagname = "Variogram_Full_Ik_" + String_Op::to_string( i+1 );
      init_cov_ok =
        geostat_utils::initialize_covariance( &covar_vector_[i], tagname, 
			                                        parameters, errors );
      if( !init_cov_ok ) return false;
    }
    

    // initialize all the neighborhoods

    for( int j=0; j < thres_count_; j++ ) {
      hdata_grid_->select_property( hdata_property_names[j] );

      neighborhoods_vector_[j] = 
        SmartPtr<Neighborhood>(
		      hdata_grid_->neighborhood( ranges, angles, &covar_vector_[j] )
		    );
      neighborhoods_vector_[j]->max_size( max_neigh );

    }
  }

  

  //-------------------------------------------------------
  // Initialization done. Check if errors were reported and return

  if( !errors->empty() )
    return false;

  return true;
}
