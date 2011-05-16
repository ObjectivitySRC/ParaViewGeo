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
#include "Postsim.h"

#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <GsTLAppli/geostat/utilities.h>

//#include <GsTLAppli/utils/gstl_plugins.h>

#include <algorithm>


bool Postsim::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) 
{

	std::cout << "initializing algorithm Postsim \n";

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

  std::string prop_str = parameters->value( "props.value" );
  if(prop_str.empty()) {
    errors->report("props","No property specified");
    return false;
  }
	std::vector< std::string > prop_name = 
            String_Op::decompose_string(prop_str, ";" );
	
	for(std::vector< std::string >::iterator it_str = prop_name.begin(); 
    it_str != prop_name.end(); it_str++ ) {
      GsTLGridProperty* prop = grid_->property( *it_str );
      errors->report(!prop, "props","No property exist with that name");
      props_.push_back(prop);
  }

	etype_ =  parameters->value( "etype.value" ) == "1";
	iqr_ =   parameters->value( "iqr.value" ) == "1";
	cond_var_ =   parameters->value( "cond_var.value" ) == "1";
	mean_above_ =   parameters->value( "mean_above.value" ) == "1";
	mean_below_ =   parameters->value( "mean_below.value" ) == "1";
	prob_above_ =   parameters->value( "prob_above.value" ) == "1";
	prob_below_ =   parameters->value( "prob_below.value" ) == "1";
  quantile_ =   parameters->value( "quantile.value" ) == "1";

  if( iqr_ || cond_var_ || mean_above_ || mean_below_ || quantile_ ) {
    if( props_.size() <= 1 ) {
      errors->report( "Hard_Data", "Must have more than one property for the statistics requested" );
      return false;
    }
  }

  if(etype_)
    check_input_string(errors,parameters,"etype_prop");
  if(cond_var_)
   check_input_string(errors,parameters,"cond_var_prop");
  if(iqr_) 
  	check_input_string(errors,parameters,"iqr_prop");
  if(quantile_){
		check_input_string(errors,parameters,"quantile_prop");
    check_input_string(errors,parameters,"quantile_vals",0.,1.);
  }
  if(mean_above_) {
		check_input_string( errors,parameters,"mean_above_prop");
    check_input_string(errors,parameters,"mean_above_vals");
  }
	if(mean_below_) {
		check_input_string( errors,parameters,"mean_below_prop");
    check_input_string(errors,parameters,"mean_below_vals");
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

  if(etype_)
		initialize_operation(etype_prop_,errors,parameters,"etype");

  if(cond_var_) 
		initialize_operation(cond_var_prop_,errors,parameters,"cond_var");

  if(iqr_) 
  	initialize_operation(iqr_prop_,errors,parameters,"iqr");

  if(quantile_) {
		initialize_operation(quantile_props_, quantile_vals_, errors,parameters,"quantile");
    std::sort(quantile_vals_.begin(),quantile_vals_.end());
    if( quantile_vals_.front() < 0 && quantile_vals_.back() > 1 )
      errors->report("quantile","Quantiles must be between 0 and 1");
  }

	if(mean_above_)
		initialize_operation( mean_above_props_, mean_above_vals_,errors,parameters,"mean_above");

	if(mean_below_)
		initialize_operation( mean_below_props_, mean_below_vals_,errors,parameters,"mean_below");

	if(prob_above_)
		initialize_operation( prob_above_props_, prob_above_vals_,errors,parameters,"prob_above");

	if(prob_below_)
		initialize_operation( prob_below_props_, prob_below_vals_,errors,parameters,"prob_below");

  if( !errors->empty() ) 
    return false;

	return true;
}


int Postsim::execute( GsTL_project* ) { 


	bool flag;
	std::vector< GsTLGridProperty* >::const_iterator it_prop;;
	int nprop = props_.size();
	double sum;
	std::vector< float > values( props_.size() );

	for(int node_id=0; node_id < grid_->size(); ++node_id ) {
		flag=true;
		sum=0;
		for(int k = 0; k < props_.size(); ++k ) {
			flag = flag&& props_[k]->is_informed( node_id );
			if(flag) values[k] = props_[k]->get_value( node_id );
		}
		if(flag) {
		if(etype_ ) 
			etype_prop_->set_value
			( std::accumulate(values.begin(),values.end(),0.0)/props_.size(), node_id );

		if(cond_var_)
		{
			float s2 = std::accumulate(values.begin(),values.end(),0.0,accumulate_square)/values.size();
			float s1 = std::accumulate(values.begin(),values.end(),0.0)/values.size();
			cond_var_prop_->set_value( s2 - std::pow(s1,2),node_id);
		}

		if(iqr_ || mean_above_ || mean_below_ || prob_above_ || prob_below_ || quantile_)
			std::sort(values.begin(),values.end());

		if(iqr_)
		{
			int q25  = props_.size()/4;
			int q75 = 3*q25;
			iqr_prop_->set_value( values[q75] - values[q25] , node_id);
		}

		if(quantile_)
		{
			std::vector<GsTLGridProperty*>::iterator it_prop = quantile_props_.begin();
			std::vector<float>::iterator it_val = quantile_vals_.begin();

			for(;it_prop != quantile_props_.end(); ++it_prop, ++it_val)
			{
        int id = (*it_val)*values.size();
        (*it_prop)->set_value( values[id], node_id);
			}
		}

		if(mean_above_)
		{
			std::vector<GsTLGridProperty*>::iterator it_prop = mean_above_props_.begin();
			std::vector<float>::iterator it_val = mean_above_vals_.begin();
			for(;it_prop != mean_above_props_.end(); ++it_prop, ++it_val)
			{
				std::vector<float>::iterator it_bnd = std::lower_bound(values.begin(),values.end(),*it_val);
        if( (values.end()-it_bnd) > 0 )  
				  (*it_prop)->set_value( std::accumulate(it_bnd,values.end(),0.0)/(values.end()-it_bnd), node_id);
        else 
          (*it_prop)->set_not_informed( node_id);
			}
		}

		if(mean_below_)
		{
			std::vector<GsTLGridProperty*>::iterator it_prop = mean_below_props_.begin();
			std::vector<float>::iterator it_val = mean_below_vals_.begin();
			for(;it_prop != mean_below_props_.end(); ++it_prop, ++it_val)
			{
  			std::vector<float>::iterator it_bnd = std::upper_bound(values.begin(),values.end(),*it_val);
        if( it_bnd-values.begin() > 0 ) 
				  (*it_prop)->set_value( std::accumulate(values.begin(),it_bnd,0.0)/(it_bnd-values.begin()),node_id);
        else 
          (*it_prop)->set_not_informed( node_id);
			}
		}

		if(prob_above_)
		{
			std::vector<GsTLGridProperty*>::iterator it_prop = prob_above_props_.begin();
			std::vector<float>::iterator it_val = prob_above_vals_.begin();
			for(;it_prop != prob_above_props_.end(); ++it_prop, ++it_val)
			{
				std::vector<float>::iterator it_bnd = std::lower_bound(values.begin(),values.end(),*it_val);
				(*it_prop)->set_value( float(values.end()-it_bnd)/values.size(), node_id);
			}
		}

		if(prob_below_)
		{
			std::vector<GsTLGridProperty*>::iterator it_prop = prob_below_props_.begin();
			std::vector<float>::iterator it_val = prob_below_vals_.begin();
			for(;it_prop != prob_below_props_.end(); ++it_prop, ++it_val)
			{
				std::vector<float>::iterator it_bnd = std::upper_bound(values.begin(),values.end(),*it_val);
				(*it_prop)->set_value( float(it_bnd-values.begin())/values.size(), node_id);
			}
		}
		}

	}

	return 0;
}

void Postsim::initialize_operation(std::vector<GsTLGridProperty*>& props_, 
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
		  props_.push_back(geostat_utils::add_property_to_grid( grid_,prop_name.str() ));
  		 
	  }
  }
}


void Postsim::initialize_operation(GsTLGridProperty*& prop,Error_messages_handler* errors, 
	const Parameters_handler* parameters, std::string tag)
{
  std::string prop_name = parameters->value( tag+"_prop.value" );
  errors->report(prop_name.empty(), tag+"_prop", "No name specified" );
  if( errors->empty() ) 
    prop =  geostat_utils::add_property_to_grid( grid_,prop_name );
}

void Postsim::check_input_string(Error_messages_handler* errors, 
  const Parameters_handler* parameters, std::string input) {
  std::string input_string = parameters->value( input+".value" );
  errors->report(input_string.empty(), input, "No input specified" );
}

void Postsim::check_input_string(Error_messages_handler* errors, 
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

Postsim::Postsim() {
	grid_ = 0;
    etype_ = false;
	iqr_ = false;
	cond_var_ = false;
	mean_above_ = false;
	mean_below_ = false;
	prob_above_ = false;
	prob_below_ = false;
}



Named_interface* Postsim::create_new_interface( std::string& ) {
  return new Postsim;
}

