/**********************************************************************
** Author: Alexandre Boucher
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
#include "trans.h"
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTL/cdf/cdf_basics.h>
#include <GsTL/cdf/gaussian_cdf.h>
#include <GsTL/cdf/First2_moments_cdf.h>
#include <GsTL/cdf/non_param_cdf.h>
#include <GsTL/univariate_stats/build_cdf.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/grid/grid_model/property_copier.h>

#include <fstream>

#include <algorithm>



int trans::execute( GsTL_project* ) {

	for(int i=0; i<props_.size(); i++ ) {
		if(is_local_cond_) cdf_transform_weighted(props_[i]);
		else cdf_transform(props_[i]);
	}

	return 0;
}

/*
Initialize the parameters
*/
bool trans::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) {
	std::cout << "initializing algorithm Trans \n";

	std::string grid_name = parameters->value( "grid.value" );
	errors->report( grid_name.empty(), 
			"grid", "No grid specified" );
	
	// Get the simulation grid from the grid manager
	if( !grid_name.empty() ) {
		bool ok = geostat_utils::create( grid_, grid_name,
				 "grid", errors );
		if( !ok ) return false;
	}
	else 
		return false;

	std::vector< std::string > prop_names = String_Op::decompose_string( 
						parameters->value( "props.value" ), ";" );
	errors->report( prop_names.empty(), "props", "No property specified" );
	std::string suffix = parameters->value( "out_suffix.value" );
  if( !errors->empty() ) return false;


  cdf_source_ = get_cdf( parameters,errors, "source");
  cdf_target_ = get_cdf( parameters,errors, "target");
  if(cdf_source_==NULL || cdf_target_ == NULL) return false;

 
	is_local_cond_ = ( parameters->value( "is_cond.value" ) == "1" );
	if(is_local_cond_) {
		std::string cond_prop_str = parameters->value( "cond_prop.value" );
		errors->report( cond_prop_str.empty(),"cond_prop", "No property specified" );
		std::string wgt_factor_str = parameters->value( "weight_factor.value" );
		errors->report( wgt_factor_str.empty(),"weight_factor", "No weight factor specified" );
		float wgt_factor = String_Op::to_number< float >( wgt_factor_str );

		GsTLGridProperty* cond_prop = grid_->property( cond_prop_str );

		//weights_.insert(weights_.begin(), cond_prop->begin(),cond_prop->end() );
        for (int i=0; i<cond_prop->size(); i++)
        {
            if ( cond_prop->is_informed(i) )
                weights_.push_back( cond_prop->get_value(i) );
            else
                weights_.push_back( -999 );
        }

		wgth_iterator it_max  = std::max_element(weights_.begin(),weights_.end());
		for(wgth_iterator it = weights_.begin() ; it!= weights_.end(); ++it )
        {
            if ( *it<0.0 )
                *it = 1.0;
            else
                *it = std::pow( (*it)/(*it_max) ,wgt_factor);
            //*it = std::pow( *it*(*it_max) ,wgt_factor);
        }
	}
  if(!errors->empty()) return false;

// Set up the property Copier 
	Property_copier* prop_copier;
	Point_set* pset = dynamic_cast< Point_set* >( grid_ );
    Cartesian_grid* cgrid = dynamic_cast< Cartesian_grid* >( grid_ );
	if( pset ) prop_copier = new Pset_to_pset_copier();
	else if(cgrid) prop_copier = new Cgrid_to_cgrid_copier();

	for(int j=0; j<prop_names.size() ; j++ ) {
		GsTLGridProperty* sourceProp = grid_->property( prop_names[j] );
		errors->report( sourceProp==NULL, "props", "Non existing properties" );
		GsTLGridProperty* targetProp = 
			geostat_utils::add_property_to_grid( grid_, prop_names[j]+suffix );

		prop_copier->copy(grid_,sourceProp,grid_,targetProp);

		props_.push_back( targetProp );
	}
  if(!errors->empty()) {
    for( int i=0; i < props_.size(); ++i ) 
      grid_->remove_property(props_[i]->name());
    return false;
  }

	return true;
}

Cdf< float >* trans::get_cdf( const Parameters_handler* parameters,
		Error_messages_handler* errors, std::string suffix )
{
	Cdf<float>* cdf;
	bool is_nonparam = ( parameters->value( "ref_type_"+suffix+".value" ) == "Non Parametric" );
	bool is_Gaussian = ( parameters->value( "ref_type_"+suffix+".value" ) == "Gaussian" );
	bool is_logNormal = ( parameters->value( "ref_type_"+suffix+".value" ) == "Log Normal" );
	bool is_uniform = ( parameters->value( "ref_type_"+suffix+".value" ) == "Uniform" );

  if( is_nonparam ) {
    Non_param_cdf<>* cdf_non_param = new Non_param_cdf<>();
    bool ok = geostat_utils::get_non_param_cdf(*cdf_non_param,parameters,
              errors,"nonParamCdf_"+suffix);
    cdf = dynamic_cast<Cdf<float>*>(cdf_non_param);

  }

	else if(is_Gaussian)
	{
		std::string mean_str = parameters->value( "G_mean_"+suffix+".value" );
	    std::string var_str = parameters->value( "G_variance_"+suffix+".value" );
		errors->report( mean_str.empty()|| var_str.empty(), "GaussianBox_"+suffix, 
				"No mean or variance provided" );

			cdf = new Gaussian_cdf(String_Op::to_number<float>(mean_str),
				String_Op::to_number<float>(var_str));
		
	} 
	else if(is_logNormal ) 
	{
		std::string mean_str = parameters->value( "LN_mean_"+suffix+".value" );
	    std::string var_str = parameters->value( "LN_variance_"+suffix+".value" );
		errors->report( mean_str.empty()|| var_str.empty(), "LogNormalBox_"+suffix, 
				"No mean or variance provided" );
		errors->report( String_Op::to_number<float>(mean_str) <= 0, "LogNormalBox_"+suffix, 
				"The mean of a logNormal distribution must be positive" );

		cdf = new LogNormal_cdf(String_Op::to_number<float>(mean_str),
				String_Op::to_number<float>(var_str));
	} 
	else if(is_uniform)
	{
		std::string min_str = parameters->value( "Unif_min_"+suffix+".value" );
	    std::string max_str = parameters->value( "Unif_max_"+suffix+".value" );
		errors->report( min_str.empty()|| max_str.empty(), "UniformBox_"+suffix, 
				"No min and//or max provided" );
		float unif_min = String_Op::to_number<float>(min_str);
		float unif_max = String_Op::to_number<float>(max_str);
		errors->report( unif_min >= unif_max, "UniformBox_"+suffix, 
				"The lower bound must be greater than the upper bound" );

		cdf = new Uniform_cdf(unif_min,unif_max);
	}
	
	return cdf;
}

void  trans::cdf_transform( GsTLGridProperty* prop )
{
	grid_->select_property( prop->name() );
	for( Geostat_grid::iterator it = grid_->begin(); it != grid_->end(); ++it )
	{
		if( it->is_informed() ) {
			double p = cdf_source_->prob(it->property_value());
			it->set_property_value( cdf_target_->inverse(p) );
		}
	}
}


void  trans::cdf_transform_weighted( GsTLGridProperty* prop  )
{
	grid_->select_property( prop->name() );
	wgth_iterator it_wt = weights_.begin();
	for( Geostat_grid::iterator it = grid_->begin(); it != grid_->end(); ++it, ++it_wt )
	{
		if( it->is_informed() ) {
			float val = it->property_value();
			float zval = cdf_target_->inverse( cdf_source_->prob( val ) );
			it->set_property_value( val - *it_wt*(val-zval) );
		}
	}
}

Named_interface* trans::create_new_interface( std::string& ) {
  return new trans;
}
