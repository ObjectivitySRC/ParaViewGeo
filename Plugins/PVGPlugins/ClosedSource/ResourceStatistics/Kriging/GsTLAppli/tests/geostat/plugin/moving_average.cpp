#include "moving_average.h"
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/utils/gstl_plugins.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>

#include <iostream>
#include <sstream>




bool MovingAverage::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) {
  
  std::cout << "initializing algorithm Moving Average \n";

  // get the name of the grid we will work on
  std::string grid_name = 
    parameters->value( "Simul_Grid.value" );

  // use utility function "create" to initialize our grid_ pointer.
  // "create" will retrieve the address of grid "grid_name" and set pointer
  // grid_ to point to the requested grid. If the requested grid does not 
  // exist, the function return false;
  bool ok = geostat_utils::create( grid_, grid_name, "Simul_Grid", errors );
  if( !ok ) return false;

  // get the name of the property we will work on
  std::string property_name = 
    parameters->value( "Hdata_Property.value" );

  // get the name of the new property we will create
  std::string new_property_name = 
    parameters->value( "Simul_Property.value" );
  
  // tell the grid to add a new property
  // If the name of the new property is already used, the grid won't add
  // the new property. Utility function "add_property_to_grid" makes sure
  // that the name of the new property is unique (if the name input by the 
  // user is already used, "add_property_to_grid" modifies that name to make
  // it unique).
  GsTLGridProperty* new_property = 
    geostat_utils::add_property_to_grid( grid_, new_property_name );

  // get the 3 anisotropy ranges. The 3 numbers are in a single string and
  // need to be converted into numbers (float).
  std::string ranges_str = 
    parameters->value( "Ranges.value" );

  // function "to_number" will convert the string into a vector of numbers
  // vector ranges will then contain all the number in string "ranges_str"
  std::vector<float> ranges = String_Op::to_numbers<float>( ranges_str );
  
  std::cout << "ranges values: " << ranges[0] << " " << ranges[1] << " " 
            << ranges[2] << std::endl;

  // tell the grid to create a new neighborhood
  neighborhood_ = grid_->neighborhood( ranges[0], ranges[1], ranges[2],
				                               0,0,0 );

  // set-up the neighborhood: max_size() set the maximum number of neighbors
  // to consider, select_property tells the neighborhood which property to
  // work on, and includes_center tells the neighborhood to consider the center
  // of the neighborhood as a neighbor: if we search for neighbors of node u,
  // u will be one of the neighbors of u.
  neighborhood_->max_size( 100000 );
  neighborhood_->select_property( property_name );
  neighborhood_->includes_center( true );

  // tell the grid to work on the new property 
  grid_->select_property( new_property->name() );

  return true;
}




int MovingAverage::execute() {

	std::cout << "Running algo moving average - revised edition - \n"; 

  // iterate through the grid
  for( Geostat_grid::iterator current_node_ptr = grid_->begin() ; 
       current_node_ptr != grid_->end() ; ++current_node_ptr ) {

    // find the neighbors of the current node
    neighborhood_->find_neighbors( *current_node_ptr );
    
    // if the current node has no neighbors, skip it
    if( neighborhood_->is_empty() ) continue;


    // compute the average of the neighboring values.
    float sum = 0.0;
    for( Neighborhood::iterator neighbor_ptr = neighborhood_->begin();
	       neighbor_ptr != neighborhood_->end() ; ++neighbor_ptr ) {
      sum += neighbor_ptr->property_value();
    }

    // compute the average: divide the sum by the number of values
    sum /= static_cast<float>( neighborhood_->size() );

    // assign the computed average to the current node
    current_node_ptr->set_property_value( sum );
  }
  return 0;
}




Named_interface* MovingAverage::create_new_interface( std::string& ) {
  return new MovingAverage;
}

GEOSTAT_PLUGIN(MovingAverage);
