#include "moving_window.h"
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/utils/gstl_plugins.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <GsTLAppli/geostat/utilities.h>
#include <algorithm>


Moving_window::Moving_window() {
	filters_ = NULL;
	neigh_ = NULL;
}


Moving_window::~Moving_window( void) {
  if(filters_) delete filters_;
  if(filters_) delete neigh_;
}


bool Moving_window::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) {

	std::cout << "initializing algorithm Moving_window \n";

	std::string grid_name = parameters->value( "Input_data.grid" );
	errors->report( grid_name.empty(),"Input_data", "No grid specified" );
 
	if( !grid_name.empty() ) {
  	bool ok = geostat_utils::create( grid_, grid_name,
				 "Grid_Name", errors );
		if( !ok || !grid_ ) return false;
	}
	else 
		return false;

  bool is_neigh_rect = parameters->value( "is_neigh_rect.value" )=="1";
  std::string type = parameters->value( "filter_type.value" );


  if(type == "User defined" ||  type == "Default Filtersim filters"  || type == "Sobel" ) {
    if( !dynamic_cast<RGrid*>( grid_ ) ) 
      errors->report( "Input_data", "The selected filter requires a regular grid" );
  }
  if( is_neigh_rect && !dynamic_cast<RGrid*>( grid_ ) ) 
    errors->report( "Input_data", "A rectangular neighborhood requires a regular grid" );
  if(!errors->empty()) return false;

  std::string prop_name = parameters->value("Input_data.property");
  GsTLGridProperty* prop_input = grid_->property( prop_name );
  grid_->select_property( prop_name );
  if( !prop_input ) {
    errors->report("Input_data","No input property selected");
    return false;
  }

	if( type == "User defined" ) 
	{
    std::string filename = parameters->value( "filter_filename.value");
    Filtersim_filters user_def_filters(filename);
    Grid_template* window_tpl = user_def_filters.get_geometry();
    neigh_ = new Rgrid_window_neighborhood (*window_tpl, dynamic_cast<RGrid*>( grid_ ),grid_->selected_property());
    filters_ = new Rasterized_filter<Smart_Neighborhood,Filtersim_filters>( user_def_filters );
	}
  else if( type == "Default Filtersim filters" ) 
  {
    int nx = String_Op::to_number<int>(parameters->value("Filtsim_def_size_x.value"));
    int ny = String_Op::to_number<int>(parameters->value("Filtsim_def_size_y.value"));
    int nz = String_Op::to_number<int>(parameters->value("Filtsim_def_size_z.value"));
    Filtersim_filters default_Filtersim_filters(nx, ny, nz );
    Grid_template* window_tpl = default_Filtersim_filters.get_geometry();
    neigh_ = new Rgrid_window_neighborhood (*window_tpl, dynamic_cast<RGrid*>( grid_ ),grid_->selected_property());
    filters_ = new Rasterized_filter<Smart_Neighborhood,Filtersim_filters>( default_Filtersim_filters );
  }
  else if( type == "Sobel" ) {
    filters_ = new Rasterized_filter<Smart_Neighborhood,Sobel_weights>( Sobel_weights() );
    Grid_template tpl;
    if( parameters->value("Sobel_orientation.value")=="XY") tpl = create_neigh_template(1,1,0);
    else if( parameters->value("Sobel_orientation.value")=="XZ" ) tpl = create_neigh_template(1,0,1);
    else if( parameters->value("Sobel_orientation.value")=="YZ" ) tpl = create_neigh_template(0,1,1);
    else errors->report("Sobel_orientation","An orientation must be selected");

    neigh_ = new Rgrid_window_neighborhood (tpl, dynamic_cast<RGrid*>( grid_ ),prop_input);
  }
  else {
    if( !is_neigh_rect ) {
      GsTLTriplet ranges, angles;
      geostat_utils::extract_ellipsoid_definition( ranges, 
					angles,"neigh_ellipsoid.value",parameters,errors);  

      neigh_ = grid_->neighborhood( ranges, angles );
    } else {
      int nx = String_Op::to_number<int>(parameters->value("size_x.value"));
      int ny = String_Op::to_number<int>(parameters->value("size_y.value"));
      int nz = String_Op::to_number<int>(parameters->value("size_z.value"));
      Grid_template tpl = create_neigh_template(nx,ny,nz);
      neigh_ = new Rgrid_window_neighborhood (tpl, dynamic_cast<RGrid*>( grid_ ),prop_input );
    }

    neigh_->max_size(1000000);


    if( type == "Gaussian low pass" ) {
	  std::string sigma_str = parameters->value("sigma.value");
		if ( sigma_str.empty() ) {
			errors->report("sigma","A scaling value is needed");
			return false;
		}
      float sigma = String_Op::to_number<float>( sigma_str );
	  	if ( sigma <= 0 ) {
			errors->report("sigma","Sigma must be positive");
			return false;
		}
      Gaussian_kernel g_kernel(sigma);
      filters_ = new Functional_filter<Smart_Neighborhood,Gaussian_kernel>( g_kernel  );
    }
    else if( type == "Moving Average" ) filters_ = new Moving_average<Smart_Neighborhood>();
    else if( type == "Moving Variance" ) filters_ = new Moving_variance<Smart_Neighborhood>();
  }
  if(!errors->empty()) return false;


   std::string prefix = parameters->value("prefix_out.value");

  for(int i=0; i<filters_->number_filters(); i++ ) {
    std::string filter_name = prefix + filters_->name(i);
    props_.push_back( geostat_utils::add_property_to_grid( grid_, filter_name ) );
  }

  grid_->select_property( prop_input->name() );
  neigh_->select_property( prop_input->name() );

	return true;
}


int Moving_window::execute(GsTL_project *) { 

  Geostat_grid::iterator it_gval = grid_->begin();
  std::vector< float > scores;

  for(; it_gval != grid_->end(); ++it_gval ) {
    neigh_->find_neighbors ( *it_gval );
    for(int i=0; i<props_.size(); i++ ) 
      props_[i]->set_value((*filters_)(*neigh_, i ),it_gval->node_id() );

 //   (*filters_)(*neigh_, i );
    
 //   if(scores.size() != props_.size() ) continue;

 //   for(int i=0; i<scores.size(); i++ ) 
 //     props_[i]->set_value(scores[i],it_gval->node_id() );
  }
  return 0;
}


Grid_template Moving_window::create_neigh_template( int nx, int ny, int nz ) 
{
  Grid_template tpl;
  for(int k =-nz; k<=nz; ++k ) {
    for(int j =-ny; j<=ny; ++j ) {
      for(int i =-nx; i<=nx; ++i ) tpl.add_vector(i,j,k);
    }
  }
  return tpl;
}


Named_interface* Moving_window::create_new_interface( std::string& ) {
  return new Moving_window;
}




//GEOSTAT_PLUGIN(Moving_window)
