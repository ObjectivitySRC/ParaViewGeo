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
#include "PostKriging.h"

#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTL/cdf/interpolators.h>
#include <iostream>

//#include <GsTLAppli/utils/gstl_plugins.h>

#include <algorithm>


bool PostKriging::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) 
{

	std::cout << "initializing algorithm PostKriging \n";

	std::string harddata_grid_name = parameters->value( "Hard_Data.value" );
	errors->report( harddata_grid_name.empty(), 
			"Hard_Data", "No hard data specified" );

	if( !harddata_grid_name.empty() ) {
		grid_ = 0;
		bool ok = geostat_utils::create( grid_, harddata_grid_name,
				 "Grid_Name", errors );
		if( !ok || !grid_ ) return false;
	}
	else 
		return false;

    is_non_param_cdf_  = parameters->value( "is_non_param_cdf.value" ) == "1";

	if(is_non_param_cdf_) 
	{
		std::vector< std::string > prop_name = String_Op::decompose_string( 
							parameters->value( "props.value" ), ";" );
		for(std::vector< std::string >::iterator it_str = prop_name.begin(); 
									it_str != prop_name.end(); it_str++ )
			props.push_back(grid_->property( *it_str ));

		std::vector< float > thresh = String_Op::to_numbers<float>( parameters->value( "marginals.value" ) );
		errors->report( thresh.size() != props.size() , "marginals.value", 
			"The marginal must have the same lengths than the indicator properties" );

		non_param_cdf_ = new Non_param_cdf<>(thresh.begin(),thresh.end());

		//Set the tail extrapolation functions
    geostat_utils::set_cdf_extrapolation_tail(parameters,errors, 
      *non_param_cdf_,"lowerTailCdf", "upperTailCdf");

	} 
	else {
		std::string prop_name = parameters->value( "gaussian_mean_prop.value" );
		props.push_back( grid_->property( prop_name ));
		errors->report( props[0] == NULL , "mean_prop.value", "That property does not exist" );

		prop_name = parameters->value( "gaussian_var_prop.value" );
		props.push_back( grid_->property( prop_name ) );
		errors->report( props[1] == NULL , "var_prop.value", "That property does not exist" );

		gaussian_cdf_ = new Gaussian_cdf();
	}

	mean_ =  parameters->value( "mean.value" ) == "1";
	iqr_ =   parameters->value( "iqr.value" ) == "1";
	cond_var_ =   parameters->value( "cond_var.value" ) == "1";
	quantile_ =   parameters->value( "quantile.value" ) == "1";
	prob_above_ =   parameters->value( "prob_above.value" ) == "1";
	prob_below_ =   parameters->value( "prob_below.value" ) == "1";

  if(mean_)
    check_input_string(errors,parameters,"mean_prop");
  if(cond_var_)
   check_input_string(errors,parameters,"cond_var_prop");
  if(iqr_) 
  	check_input_string(errors,parameters,"iqr_prop");
  if(quantile_){
		check_input_string(errors,parameters,"quantile_prop",0.,1.);
    check_input_string(errors,parameters,"quantile_vals",0.,1.);
  }
  if(prob_above_) {
		check_input_string( errors,parameters,"prob_above_prop");
    check_input_string(errors,parameters,"prob_above_vals");
  }
  if(prob_below_) {
		check_input_string( errors,parameters,"prob_below_prop");
    check_input_string(errors,parameters,"prob_below_vals");
  }

  if( !errors->empty() ) return false;

	if(mean_) 
    initialize_operation(mean_prop_,errors,parameters,"mean");

	if(cond_var_) 
    initialize_operation(cond_var_prop_,errors,parameters,"cond_var");

	if(iqr_)  
    initialize_operation(iqr_prop_,errors,parameters,"iqr");

  if(quantile_)  {
    initialize_operation( quantile_props_, quantile_vals_,errors,parameters,"quantile");
    std::sort(quantile_vals_.begin(),quantile_vals_.end());
    if( quantile_vals_.front() < 0 && quantile_vals_.back() > 1 )
      errors->report("quantile","Quantiles must be between 0 and 1");
  }

	if(prob_above_)  
		initialize_operation( prob_above_props_, prob_above_vals_,errors,parameters,"prob_above");

	if(prob_below_) 
		initialize_operation( prob_below_props_, prob_below_vals_,errors,parameters,"prob_below");


  if( !errors->empty() ) 
    return false;

	return true;
}


int PostKriging::execute( GsTL_project* ) { 


	bool ok;
	std::vector< GsTLGridProperty* >::const_iterator it_prop;;
	int nprop = props.size();
	std::vector< float > values( props.size() );

	for(int node_id=0; node_id < grid_->size(); ++node_id ) {
		ok=true;
		if(is_non_param_cdf_) {

			for(int k = 0; k < props.size(); ++k ) {
				ok = ok&& props[k]->is_informed( node_id );
				if(ok) values[k] = props[k]->get_value( node_id );
			}
			if(ok) non_param_cdf_->p_set(values.begin(),values.end());
		}else {
			ok =  props[0]->is_informed( node_id ) && props[1]->is_informed( node_id );
			if(ok) {
				gaussian_cdf_->mean() = props[0]->get_value( node_id );
				gaussian_cdf_->variance() = props[1]->get_value( node_id );
				//gaussian_cdf_->mean( props[0]->get_value( node_id ) );
				//gaussian_cdf_->variance( props[1]->get_value( node_id ) );
			}

		}
		if(ok) {
		//	if( is_non_param_cdf_)	cdf_ = dynamic_cast< Cdf<double>* >( non_param_cdf_ );
		//	else cdf_ = dynamic_cast< Cdf<double>* >( gaussian_cdf_ );

			if(mean_ ) {
				if( is_non_param_cdf_ ) mean_prop_->set_value( non_param_cdf_->mean(), node_id );
				else mean_prop_->set_value( gaussian_cdf_->mean(), node_id );
			}

			if(cond_var_){
				if( is_non_param_cdf_ ) cond_var_prop_->set_value( non_param_cdf_->variance(), node_id );
				else cond_var_prop_->set_value( gaussian_cdf_->variance(), node_id );
			}


			if(iqr_)
				if( is_non_param_cdf_ ) 
					iqr_prop_->set_value( non_param_cdf_->inverse(0.75) -  non_param_cdf_->inverse(0.25), node_id);
				else  iqr_prop_->set_value( gaussian_cdf_->inverse(0.75) -  gaussian_cdf_->inverse(0.25), node_id);

			if(quantile_)
			{
				std::vector<GsTLGridProperty*>::iterator it_prop = quantile_props_.begin();
				std::vector<float>::iterator it_val = quantile_vals_.begin();
				for(;it_prop != quantile_props_.end(); ++it_prop, ++it_val)
				{
					if( is_non_param_cdf_ )
						(*it_prop)->set_value( non_param_cdf_->inverse(*it_val), node_id);
					else (*it_prop)->set_value( gaussian_cdf_->inverse(*it_val), node_id);
				}
			}

			if(prob_above_)
			{
				std::vector<GsTLGridProperty*>::iterator it_prop = prob_above_props_.begin();
				std::vector<float>::iterator it_val = prob_above_vals_.begin();
				for(;it_prop != prob_above_props_.end(); ++it_prop, ++it_val) {
					if( is_non_param_cdf_ )
						(*it_prop)->set_value( 1 - non_param_cdf_->prob(*it_val), node_id);
					else (*it_prop)->set_value( 1 - gaussian_cdf_->prob(*it_val), node_id);
				}
			}

			if(prob_below_)
			{
				std::vector<GsTLGridProperty*>::iterator it_prop = prob_below_props_.begin();
				std::vector<float>::iterator it_val = prob_below_vals_.begin();
				for(;it_prop != prob_below_props_.end(); ++it_prop, ++it_val){
					if( is_non_param_cdf_ )
						(*it_prop)->set_value( non_param_cdf_->prob(*it_val), node_id);
					else (*it_prop)->set_value( gaussian_cdf_->prob(*it_val), node_id);
				}
			}
		}

	}

	return 0;
}

void PostKriging::initialize_operation(std::vector<GsTLGridProperty*>& props, 
		std::vector<float>& vals,Error_messages_handler* errors, 
		const Parameters_handler* parameters, std::string tag)
{
  std::string base_prop_name = parameters->value( tag+"_prop.value" );
	errors->report(base_prop_name.empty(), tag+"_prop", "No name specified" );
  std::string val_names = parameters->value( tag+"_vals.value" );
	errors->report( val_names.empty(), tag+"_vals", "No values specified" );
  if( errors->empty() ) {
	  vals = String_Op::to_numbers<float>(val_names);

	  for(std::vector< float >::iterator it = vals.begin();it != vals.end(); ++it )
	  {
		  std::ostringstream prop_name;
		  prop_name << base_prop_name <<"_tr"<< *it;
		  props.push_back(geostat_utils::add_property_to_grid( grid_,prop_name.str() ));
  		 
	  }
  }
}

void PostKriging::initialize_operation(GsTLGridProperty*& prop,Error_messages_handler* errors, 
	const Parameters_handler* parameters, std::string tag)
{
  std::string prop_name = parameters->value( tag+"_prop.value" );
  errors->report(prop_name.empty(), tag+"_prop", "No name specified" );
  if( errors->empty() ) 
    prop =  geostat_utils::add_property_to_grid( grid_,prop_name );
}


void PostKriging::check_input_string(Error_messages_handler* errors, 
  const Parameters_handler* parameters, std::string input) {
  std::string input_string = parameters->value( input+".value" );
  errors->report(input_string.empty(), input, "No input specified" );
}

void PostKriging::check_input_string(Error_messages_handler* errors, 
  const Parameters_handler* parameters, std::string input, float min, float max) {
  std::string input_string = parameters->value( input+".value" );
  if(input_string.empty() ) errors->report( input, "No input specified" );
  else {
    std::vector<float> vals = String_Op::to_numbers<float>(input_string);
    std::ostringstream error_stream;
    error_stream << "Input must be between "<<min<<" and "<<max;
    for(int i=0; i< vals.size(); ++i )
      errors->report(vals[i] <= min || vals[i] >= max, input, error_stream.str());
  }

}


PostKriging::PostKriging() {
	grid_ = 0;
	is_non_param_cdf_ = true;
    mean_ = false;
	iqr_ = false;
	cond_var_ = false;
	quantile_ = false;
	prob_above_ = false;
	prob_below_ = false;
}



Named_interface* PostKriging::create_new_interface( std::string& ) {
  return new PostKriging;
}

//GEOSTAT_PLUGIN(PostKriging)
/*
extern "C" __declspec(dllexport) int PostKriging_init() { 
	  GsTLlog << "nn registering plugin PostKriging" << "n"; 
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( geostatAlgo_manager ); 
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() ); 
  if( !dir ) { 
    GsTLlog << "Directory " << geostatAlgo_manager << " does not exist n"; 
    return 1; 
  } 
  PostKriging toto; 
  dir->factory( toto.name(), PostKriging::create_new_interface ); 
  return 0; 
}
*/
