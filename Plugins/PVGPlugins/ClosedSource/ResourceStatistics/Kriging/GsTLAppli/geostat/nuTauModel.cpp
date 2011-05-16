#include "nuTauModel.h"

#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/utils/gstl_plugins.h>

#include <algorithm>
#include <numeric>



bool NuTauModel::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) 
{
	// Extract the parameters input by the user from the parameter handler

	std::cout << "initializing algorithm Nu/Tau Model \n";

   //get the name of the grid we will work on
	std::string grid_name = parameters->value( "Grid.value" );

	errors->report( grid_name.empty(), 
			"Hard_Data", "No hard data specified" );

	if( !grid_name.empty() ) {
		bool ok = geostat_utils::create( grid_, grid_name,
				 "Grid", errors );
		if( !ok || !grid_ ) return false;
	}
	else 
		return false;

   //get the property we will work on 
	std::vector< std::string > prop_name = String_Op::decompose_string( 
						parameters->value( "props.value" ), ";" );
  if( prop_name.size() <= 1 ) {
	  errors->report( "props", "Must specify at least two properties" );
    return false;
  }
	
	for(std::vector< std::string >::iterator it_str = prop_name.begin(); 
    it_str != prop_name.end(); it_str++ ) {
      GsTLGridProperty* temp = grid_->property( *it_str );
    if( !temp ) {
	    errors->report( "props", "One property name does not exist" );
      for( int i=0; i < props_.size(); i++ ) grid_->remove_property(props_[i]->name());
      return false;
    }
		props_.push_back(temp);
  }

  is_nu_ = parameters->value( "is_nu.value" ) == "1";

	// get the Nu values.
  is_redun_prop_ = parameters->value( "redun_specific.value" ) == "1";
  if(!is_redun_prop_) {
    std::string redun_str = parameters->value( "redun_input.value" );
    if(redun_str.empty() ) {
      errors->report( "redun_input", "Cannot be left empty");
      return false;
    }

    std::vector<float> redun_params = String_Op::to_numbers<float>( redun_str );
    if( !is_nu_  ) {
	    if( redun_params.size() != props_.size() ) {
      std::stringstream error_mes;
      error_mes<<"There must be "<<props_.size()<<" tau values";
      errors->report( "redun_input", error_mes.str());
      return false;
      }
      tau_.insert(tau_.begin(), redun_params.begin(), redun_params.end() );
    }
    else {
      if( redun_params.size() == 1 ) {
        nu0_ = redun_params[0];
        errors->report( nu0_<=0,"redun_input", "Nu0 must be positive");
      }
      else {
	      if( redun_params.size() != props_.size() ) {
        std::stringstream error_mes;
        error_mes<<"There must be "<<props_.size()<<" nu values or one nu0";
        errors->report( "redun_input", error_mes.str());
        }
        if( *(std::min_element(redun_params.begin(),redun_params.end())) <= 0 ) 
          errors->report( "Nu_values", "All values must be positive");

        nu0_ = std::accumulate(redun_params.begin(),redun_params.end(),1,std::multiplies<float>() );
      }
      if(!errors->empty()) return false;

    }
  } 
  else  {
    if( is_nu_  ) {
      std::vector<std::string> nu_names = 
        String_Op::decompose_string(parameters->value( "redun_props.value" ),";");
	  if(nu_names.size() != 1) {
		errors->report( "redun_props", "Only one property containing Nu0 must be selected");
		return false;
	  }
      nu_values_prop_ = grid_->property( nu_names[0] );
      errors->report( !nu_values_prop_,"redun_props", "Property does not exist");
    }
    else {
      std::vector<std::string> tau_names = 
        String_Op::decompose_string(parameters->value( "redun_props.value" ),";");
	  if( tau_names.size() != props_.size() ) {
		std::stringstream error_mes;
		error_mes<<"There must be "<<props_.size()<<" properties selected";
		errors->report( "redun_props", error_mes.str());
		return false;
      }
      for( int i=0; i<tau_names.size(); i++ ) {
        GsTLGridProperty* temp_prop = grid_->property(tau_names[i]);
        if( !temp_prop ) {
          errors->report("redun_prop","Non existing property" );
          return false;
        }
        tau_values_props_.push_back( temp_prop );
      }
    }
  }

  std::string marginal_str = parameters->value( "marginal.value" );
  errors->report( marginal_str.empty(),"marginal_input", "The marginal is required");
  float marginal = String_Op::to_number<float>( marginal_str );
  errors->report( (marginal <= 0.0 || marginal >= 1.0 ),
    "marginal", "The marginal must be between 0 and 1");

  std::string nu_prop_str = parameters->value( "redun_prop.value" );
  errors->report(nu_prop_str.empty(), "redun_prop", "A property name is required");
  if( !errors->empty() ) return false; 

  redun_prop_ = geostat_utils::add_property_to_grid( grid_, nu_prop_str );
  x0_ = (1.-marginal)/marginal;

  return true;

}


int NuTauModel::execute(GsTL_project *) 
{ 
	typedef std::vector< float >::iterator it_probT;

	std::vector< float > probs( props_.size() );
  bool issue_missing_value_warning = false;
  bool issue_prob_errors_warning = false;

	for(int node_id=0; node_id < grid_->size(); ++node_id ) {

		// get the probability vector for node_id
    bool is_full = true;
		for( int k = 0 ;k< props_.size();++k) 
		{
      if( !props_[k]->is_informed(node_id) ) {
        is_full = false;
        issue_missing_value_warning = true;
        continue;
      }
      probs[k]=props_[k]->get_value(node_id);
		}
    if( is_full ) {
      if(is_redun_prop_ && is_nu_) {
        if ( nu_values_prop_->is_informed(node_id) && nu_values_prop_->get_value(node_id) > 0) 
          nu0_ = nu_values_prop_->get_value(node_id); 
        else {
          issue_missing_value_warning = true;
          continue;
        }
      }
      else if(is_redun_prop_ && !is_nu_) {
        tau_.clear();
        for( int i=0; i< tau_values_props_.size(); i++ ) {
          if ( tau_values_props_[i]->is_informed(node_id) ) 
            tau_.push_back( tau_values_props_[i]->get_value(node_id) );
          else {
            issue_missing_value_warning = true;
            continue;
          } 
        }
      }
      float updated_prob; 
      if(is_nu_)  updated_prob = NuModel_compute(probs);
      else  updated_prob = TauModel_compute(probs);
      if(updated_prob < 0 ) issue_prob_errors_warning = true;
      else  redun_prop_->set_value(updated_prob,node_id);
    }

	}

  if( issue_missing_value_warning )
    GsTLcerr << "Nu model integration could not be performed at some locations because\n"
             << "there are some uninformed locations\n" 
             << gstlIO::end; 
  if( issue_prob_errors_warning )
    GsTLcerr << "Nu model integration could not be performed at some locations because\n"
             << "there are some errors/or ambiguity in the input probabilities\n"
             << gstlIO::end; 

	return 0;				
}


 float NuTauModel::NuModel_compute(std::vector<float>& probs)
{
//	  if(marginal_ == 0.0 || marginal_ == 1.0 ) return marginal_;

  std::vector<float>::iterator it_min, it_max;

  it_min = std::min_element(probs.begin(), probs.end());
  if( *it_min < 0 ) return -1;
  it_max = std::max_element(probs.begin(), probs.end());
  if( *it_max > 1 ) return -1;

  bool is_prob_zero  = *it_min == 0.; 
	bool is_prob_one = *it_max == 1.;
	if(is_prob_zero && is_prob_one ) return -1;
	if( is_prob_zero ) return 0.0;
	if( is_prob_one ) return 1.0;
	   
  for( int i=0; i< probs.size(); i++ ) probs[i] = (1. - probs[i])/probs[i]/x0_;

  float x =  x0_*std::accumulate(probs.begin(),probs.end(),nu0_,std::multiplies<float>() );

  return 1./(1.+x);
}


 float NuTauModel::TauModel_compute(std::vector<float>& probs)
{
  std::vector<float>::iterator it_min, it_max;

  it_min = std::min_element(probs.begin(), probs.end());
  if( *it_min < 0 ) return -1;
  it_max = std::max_element(probs.begin(), probs.end());
  if( *it_max > 1 ) return -1;

  bool is_prob_zero  = *it_min == 0.; 
	bool is_prob_one = *it_max == 1.;
	if(is_prob_zero && is_prob_one ) return -1;
	if( is_prob_zero ) return 0.0;
	if( is_prob_one ) return 1.0;

  float x = 1.;
	   
  for( int i=0; i< probs.size(); i++ ) x *= std::pow( ((float)1. - probs[i])/probs[i]/x0_, tau_[i] );

  return 1./(1.+x);
}


Named_interface* NuTauModel::create_new_interface( std::string& ) 
{
  return new NuTauModel;
}
