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

#include <GsTLAppli/geostat/cokriging.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/combined_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/grid_initializer.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/property_copier.h>
#include <GsTLAppli/appli/utilities.h>

#include <GsTL/kriging/cokriging_weights.h>
#include <GsTL/kriging/LMC_covariance.h>
#include <GsTL/kriging/MM1_covariance.h>
#include <GsTL/kriging/MM2_covariance.h>
#include <GsTL/matrix_library/tnt/cmat.h>

Named_interface* Cokriging::create_new_interface( std::string& ) {
  return new Cokriging;
}


Cokriging::Cokriging() 
  : kriging_weights_( 40 ) {
  Kconstraints_ = 0;
  combiner_ = 0;
  simul_grid_ = 0;
  prim_harddata_grid_ = 0;
  sec_harddata_grid_ = 0;
  covar_ = 0;
  assign_ = false;
}
 

Cokriging::~Cokriging() {
  delete Kconstraints_;
  delete combiner_;
  delete covar_;
  if(assign_) simul_grid_->remove_property(primary_variable_);
}


int Cokriging::execute( GsTL_project* proj) {
  // those flags will be used to signal if some of the nodes could not be
  // informed
  bool issue_singular_system_warning = false;
  bool issue_no_conditioning_data_warning = false;

	// Set up a progress notifier	
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running Cokriging", 
			    simul_grid_->size() + 1,  
			    std::max( simul_grid_->size() / 50, 1 ) );

  // create property for kriging variance
  std::string var_prop_name = property_name_ + "_krig_var";
  GsTLGridProperty* var_prop = 
    geostat_utils::add_property_to_grid( simul_grid_, var_prop_name );
  appli_assert( var_prop );


  simul_grid_->select_property( property_name_ );
  
  typedef Geostat_grid::iterator iterator;
  iterator begin = simul_grid_->begin();
  iterator end = simul_grid_->end();
  
  for( ; begin != end; ++begin ) {
//    std::cout << "center: " << begin->location() << "  ";
    if( !progress_notifier->notify() ) {
      simul_grid_->remove_property( property_name_ );
      simul_grid_->remove_property( var_prop_name );
      if(assign_) simul_grid_->remove_property(primary_variable_);
      return 1;
    }

    if( begin->is_informed() ) continue;
      
    neighborhood_vector_[0].find_neighbors( *begin );
    neighborhood_vector_[1].find_neighbors( *begin );
    
//    if( neighborhood_vector_[0].is_empty() &&
//      	neighborhood_vector_[1].is_empty() ) {
      if( neighborhood_vector_[0].size() < min_neigh_1_ &&
      	neighborhood_vector_[1].size() < min_neigh_2_ ) {
      //if we don't have any conditioning data, skip the node
      issue_no_conditioning_data_warning = true;
      continue;
    }
/*
    std::cout << "neighbors: " ;
    for( Neighborhood::iterator it = neighborhood_vector_[0].begin();
	   it != neighborhood_vector_[0].end(); ++it ) {
      std::cout << "(" << it->location() << "):" << it->property_value() << "  ";
    }
*/      
    double variance = -1;
    int status = cokriging_weights( kriging_weights_, variance,
                                    begin->location(),
                                    neighborhood_vector_.begin(), 
                                    neighborhood_vector_.end(),
                                    *covar_, *Kconstraints_ );
      
    if(status == 0) {
      // the kriging system could be solved
      double estimate = (*combiner_)( kriging_weights_.begin(), 
				      kriging_weights_.end(),
				      neighborhood_vector_.begin(), 
				      neighborhood_vector_.end() );
//    std::cout << "  estimate: " << estimate << std::endl;
      begin->set_property_value( estimate );
      var_prop->set_value( variance, begin->node_id() );
    }
    else {
      // the kriging system could not be solved
      issue_singular_system_warning = true;
    }
  }

  if( issue_singular_system_warning )
    GsTLcerr << "Some nodes could not be estimated The kriging system could not be solved for every node\n" 
             << gstlIO::end; 
  if( issue_no_conditioning_data_warning )
    GsTLcerr << "Cokriging could not be performed at some locations because\n"
             << "the neighborhood of those locations was empty.\n"
             << "Try increasing the size of the search ellipsoid.\n"
             << gstlIO::end; 

  if(assign_) simul_grid_->remove_property(primary_variable_);
  return 0;
}





bool Cokriging::initialize( const Parameters_handler* parameters,
			  Error_messages_handler* errors ) {

  // Extract the parameters input by the user from the parameter handler
  
  //-------------
  // The "simulation" grid parameters
  
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

  GsTLGridProperty* new_prop = 
    geostat_utils::add_property_to_grid( simul_grid_, property_name_ );
  errors->report( new_prop == 0, 
                  "Property_Name", "Property already exists. Choose another name" );
  property_name_ = new_prop->name();

  

  //-------------
  // The hard data parameters

  std::string prim_harddata_grid_name = 
    parameters->value( "Primary_Harddata_Grid.value" );
  errors->report( prim_harddata_grid_name.empty(), 
		  "Primary_Harddata_Grid", "No hard data grid specified" );

  primary_variable_ = parameters->value( "Primary_Variable.value" );
  errors->report(  primary_variable_.empty(), 
		  "Primary_Variable", "No property name specified" );


  std::string sec_harddata_grid_name = 
    parameters->value( "Secondary_Harddata_Grid.value" );
  errors->report( sec_harddata_grid_name.empty(), 
		  "Secondary_Harddata_Grid", "No hard data grid specified" );

  secondary_variable_ = parameters->value( "Secondary_Variable.value" );
  errors->report( secondary_variable_.empty(), 
		  "Secondary_Variable", "No property name specified" );

  if( !errors->empty() )
    return false;


  // Get the harddata grids from the grid manager

  if( !prim_harddata_grid_name.empty() ) {
    bool ok = geostat_utils::create( prim_harddata_grid_, prim_harddata_grid_name, 
				                             "Primary_Harddata_Grid", errors );
    if( !ok ) return false;
  }

  if( !sec_harddata_grid_name.empty() ) {
    bool ok = geostat_utils::create( sec_harddata_grid_, sec_harddata_grid_name, 
				                             "Secondary_Harddata_Grid", errors );
    if( !ok ) return false;
  }

/*
  assign_ = 
    String_Op::to_number<bool>( parameters->value( "Assign_Hard_Data.value" ) );

  if( assign_ ) {
    SmartPtr<Property_copier> property_copier_ = 
      Property_copier_factory::get_copier( prim_harddata_grid_, simul_grid_ );
    if( !property_copier_ ) {
      std::ostringstream message;
      message << "It is currently not possible to copy a property from a "
              << prim_harddata_grid_->classname() << " to a " 
              << simul_grid_->classname() ;
      errors->report( !property_copier_, "Assign_Hard_Data", message.str() );
      return false;
    }

    GsTLGridProperty* new_hard_data_prop = 
    geostat_utils::add_property_to_grid( simul_grid_, "__hard_data_prop_temporary__" );

    property_copier_->copy( prim_harddata_grid_, 
                            prim_harddata_grid_->property( primary_variable_ ),
                            simul_grid_, new_hard_data_prop );

    prim_harddata_grid_ = simul_grid_;
    primary_variable_ = new_hard_data_prop->name();
    //primary_variable_ = property_name_;
  }
*/

  //-------------
  // Primary variable variogram (covariance), C11(h), initialization 
  Covariance<Location> C11;
  bool init_cov_ok =
    geostat_utils::initialize_covariance( &C11, "Variogram_C11",
	                                        parameters, errors );
  if( !init_cov_ok ) return false;


  //-------------
  // Initialize the covariance set

  geostat_utils::Cokriging_type cok_type = 
    geostat_utils::cokriging_type( "Cokriging_Type.value",
			                             parameters, errors );

  geostat_utils::CokrigTagMap cokrig_tagmap;
  cokrig_tagmap[ geostat_utils::FULL ] = "Variogram_C12 Variogram_C22";
  cokrig_tagmap[ geostat_utils::MM1 ] = "Correl_Z1Z2.value Var_Z2.value";
  cokrig_tagmap[ geostat_utils::MM2 ] = "MM2_Correl_Z1Z2.value MM2_Variogram_C22";
  
  covar_ = 
    geostat_utils::init_covariance_set( cok_type, C11, cokrig_tagmap,
                                        parameters, errors );

  if( !errors->empty() ) return false;


  //-------------
  // The primary variable search neighborhood parameters
  
  int max_neigh_1 = 
    String_Op::to_number<int>(parameters->value( "Max_Conditioning_Data.value" ));

  min_neigh_1_ = 
    String_Op::to_number<int>(parameters->value( "Min_Conditioning_Data.value" ));
  errors->report( min_neigh_1_ >= max_neigh_1, 
		  "Min_Conditioning_Data", "Min must be less than Max" );
  
  GsTLTriplet ranges_1;
  GsTLTriplet angles_1;
  bool extract_ok = 
    geostat_utils::extract_ellipsoid_definition( ranges_1, angles_1, 
	  				                                     "Search_Ellipsoid_1.value",
                                     					   parameters, 
			  		                                     errors );
  if( !extract_ok ) return false;
  extract_ok = geostat_utils::is_valid_range_triplet( ranges_1 );
  errors->report( !extract_ok,
                  "Search_Ellipsoid_1",
                  "Ranges must verify: major range >= " 
                  "medium range >= minor range >= 0" );
  if( !extract_ok ) return false;


 // Assign hard data to the kriging grid if required
// A dummy property is created for the hard data

  assign_ = 
    String_Op::to_number<bool>( parameters->value( "Assign_Hard_Data.value" ) );

  if( assign_ ) {
    SmartPtr<Property_copier> property_copier_ = 
      Property_copier_factory::get_copier( prim_harddata_grid_, simul_grid_ );
    if( !property_copier_ ) {
      std::ostringstream message;
      message << "It is currently not possible to copy a property from a "
              << prim_harddata_grid_->classname() << " to a " 
              << simul_grid_->classname() ;
      errors->report( !property_copier_, "Assign_Hard_Data", message.str() );
      return false;
    }

    GsTLGridProperty* new_hard_data_prop = 
    geostat_utils::add_property_to_grid( simul_grid_, "__hard_data_prop_temporary__" );

    property_copier_->copy( prim_harddata_grid_, 
                            prim_harddata_grid_->property( primary_variable_ ),
                            simul_grid_, new_hard_data_prop );

    prim_harddata_grid_ = simul_grid_;
    primary_variable_ = new_hard_data_prop->name();
    //primary_variable_ = property_name_;
  }




  SmartPtr<Neighborhood> prim_neighborhood = 
    prim_harddata_grid_->neighborhood( ranges_1, angles_1, &C11 );
  prim_neighborhood->select_property( primary_variable_ );
  prim_neighborhood->max_size( max_neigh_1 );
  
  neighborhood_vector_.push_back( NeighborhoodHandle(prim_neighborhood) );







  //-------------
  // Initialize the secondary variable neighborhood

  const GsTLGridProperty* secondary_prop = 
    sec_harddata_grid_->property( secondary_variable_ );

  NeighborhoodHandle sec_neighborhood = 
    geostat_utils::init_secondary_neighborhood( cok_type, sec_harddata_grid_, 
                                                secondary_prop,
                                                parameters, errors, 
                                                "Max_Conditioning_Data_2.value",
                                                "Search_Ellipsoid_2.value",
                                                "Variogram_C22" );
  
  neighborhood_vector_.push_back( sec_neighborhood );

  int max_neigh_2 = 
    String_Op::to_number<int>(parameters->value( "Max_Conditioning_Data_2.value" ));
  min_neigh_2_ = 
    String_Op::to_number<int>(parameters->value( "Min_Conditioning_Data_2.value" ));
  errors->report( min_neigh_2_ >= max_neigh_2, 
		  "Min_Conditioning_Data", "Min must be less than Max" );

//  finish_covset_and_neighborhood( C11, cok_type, 
//				  parameters, errors );
  


  //-----------------
  // The kriging constraints and combiner
  
  geostat_utils::Kriging_type ktype = geostat_utils::kriging_type( "Kriging_Type.value",
							   parameters, errors );

  
  //-----------------
  // parameter SK_Means will only be used if ktype is SK
  geostat_utils::initialize( combiner_, Kconstraints_,
			 ktype, "SK_Means.value",
			 parameters, errors );


  
  //-----------------
  // Additional checks
  
  if( cok_type == geostat_utils::MM1 || 
      cok_type == geostat_utils::MM2 ) {
    errors->report( sec_harddata_grid_ != simul_grid_,
                    "Secondary_Harddata_Grid", 
                    "For MM1 and MM2 options, the secondary variable must "
                    "be defined on the simulation grid" );
  }




  if( !errors->empty() ) return false;


  return true;
}






