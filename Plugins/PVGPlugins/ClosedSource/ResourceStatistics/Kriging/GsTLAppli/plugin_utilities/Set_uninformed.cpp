#include "Set_uninformed.h"
#include <GsTLAppli/geostat/parameters_handler.h>
//#include <GsTLAppli/utils/gstl_plugins.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/geostat/utilities.h>


#include <GsTLAppli/math/gstlpoint.h>
#include <algorithm>



Set_uninformed::Set_uninformed( void )
{
	
}

bool Set_uninformed::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) {

	std::cout << "initializing algorithm Set_uninformed \n";

	std::string grid_name = parameters->value( "Grid.value" );
	errors->report( grid_name.empty(), "grid_name", "No Grid  specified" );

	// Get the simulation grid from the grid manager
	if( !grid_name.empty() ) {
		grid_ = 0;
		bool ok = geostat_utils::create( grid_, grid_name,
				 "Grid_Name", errors );
		if( !ok ) return false;
	}
	else 
		return false;

	std::vector< std::string > prop_names = String_Op::decompose_string( 
						parameters->value( "props.value" ), ";" );


	errors->report( prop_names.empty(),"props", "No property selected" );

	props.reserve( prop_names.size() );
	GsTLGridProperty *tmpProp;
	for(int j=0; j<prop_names.size() ; j++ ) {
		tmpProp = grid_->property( prop_names[j] );
		errors->report( tmpProp==NULL, "prop_names", "Non existing properties" );
		props.push_back( tmpProp );
	}
	// Select on what to set uninformed locations
	is_set_uninf_on_prop = (parameters->value("set_uninf.value") == "0");

	if(is_set_uninf_on_prop)
	{
		base_prop = grid_->property( parameters->value("base_prop.value") );
		errors->report( base_prop==NULL, "base_prop", "Non existing properties" );
	}
	else
	{
		std::string logical_op_str = parameters->value("logical_op.value");
		if( logical_op_str == "Equal to" ) logical_op = 0;
		else if(logical_op_str == "Greater than") logical_op =1;
		else if(logical_op_str == "Less than") logical_op =2;
        uninf_val =  String_Op::to_number<float>(parameters->value("uninf_val.value"));
		//errors->report( uninf_val==NULL, "set_uninf_val", "Non existing properties" );
	}
	return true;
}

int Set_uninformed::execute( void )
{
	prop_vec_itT it_prop, it_prop_end = props.end();

//	grid_->select_property( base_prop->name() );
	Geostat_grid::iterator it_grid, it_grid_end = grid_->end();
	if(is_set_uninf_on_prop)
	{
		for(it_grid= grid_->begin(base_prop); it_grid != it_grid_end; ++it_grid )
		{
			if( it_grid->is_informed() ) continue;
			for(it_prop = props.begin() ; it_prop != it_prop_end ; ++it_prop)
				(*it_prop)->set_not_informed (it_grid->node_id());
		}
	}
	else
	{
		for(it_grid= grid_->begin(); it_grid != it_grid_end; ++it_grid ) {
			int node_id = it_grid->node_id();
			for(it_prop = props.begin() ; it_prop != it_prop_end ; ++it_prop) {
				if( is_logical( (*it_prop)->get_value(node_id) ) )
					(*it_prop)->set_not_informed (node_id);
			}
		}
	}
/*	for(int node_id=0; node_id < grid_->size(); ++node_id )
	{
		if( node_id % 250000 == 0 ) std::cout<<node_id<<" nodes processed"<<std::endl;
		if( base_prop->is_informed(node_id) ) continue;
		for(it_prop = props.begin() ; it_prop != it_prop_end ; ++it_prop)
			if( (*it_prop)->is_informed(node_id) ) (*it_prop)->set_not_informed(node_id);
	}*/
	return 0;
}

bool Set_uninformed::is_logical( float val )
{
	if(logical_op==0) return val==uninf_val;
	if(logical_op==1) return val>uninf_val;
	if(logical_op==2) return val<uninf_val;
	else return false;
}
Named_interface* Set_uninformed::create_new_interface( std::string& ) {
  return new Set_uninformed;
}

//GEOSTAT_PLUGIN(Set_uninformed)
/*
extern "C" __declspec(dllexport) int Set_uninformed_init() { 
	  GsTLlog << "nn registering plugin geoRS_detect_changes" << "n"; 
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( geostatAlgo_manager ); 
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() ); 
  if( !dir ) { 
    GsTLlog << "Directory " << geostatAlgo_manager << " does not exist n"; 
    return 1; 
  } 
  Set_uninformed toto; 
  dir->factory( toto.name(), Set_uninformed::create_new_interface ); 
  return 0; 
}
*/