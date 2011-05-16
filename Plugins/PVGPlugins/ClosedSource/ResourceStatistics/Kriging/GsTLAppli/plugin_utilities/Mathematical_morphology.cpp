#include "mathematical_morphology.h"
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/utils/gstl_plugins.h>

int math_morphology::execute() {

	int background_facies,radius_idx=0;
	GsTLInt node_id;

  // get the index of background facies

	if(transformation > 1){
		for(node_id=0; node_id < grid_->size() ; node_id++ ){
			if( prop_->get_value(node_id)!= facies_category){
				background_facies = prop_->get_value(node_id);
				break;
			}
		}
	}

	// do either dilation / erosion / lower filtering / upper filtering

	switch(transformation){
		case 1: // dilation
			dilation_erosion(facies_category);
			break;
		case 2: // erosion
			dilation_erosion(background_facies);
			break;
		case 3: // lower filtering
			alternate_filtering(background_facies,facies_category,radius_idx);
			grid_->remove_property("previous_run");
			break;
		case 4: // upper filtering
			alternate_filtering(facies_category,background_facies,radius_idx);
			grid_->remove_property("previous_run");
			break;
	}

	return 0;
}

void math_morphology::dilation_erosion(int facies_index) {

  // iterate through the grid
	GsTLInt node_id=0;
	GsTLGridProperty* filtered_prop = grid_->selected_property();
	for( Geostat_grid::iterator current_node_ptr = grid_->begin();
		current_node_ptr != grid_->end(); ++current_node_ptr) {

		// find the neighbors of the current node
		neighborhood_->find_neighbors( *current_node_ptr );
		
		// if the current node has no property, skip it
		if( !prop_->is_informed(node_id) ){
			filtered_prop->set_not_informed( node_id );
			node_id++;
			continue;
		}
		
		// if the current node is not the target, skip it
		if( prop_->get_value(node_id) == facies_index ){
				current_node_ptr->set_property_value( prop_->get_value(node_id) );
				node_id++;
				continue;
		}

		// decide on or against dilation/erosion based on the neighboring facies index.
		bool flag = false;
		
		for( Neighborhood::iterator neighbor_ptr = neighborhood_->begin();
				neighbor_ptr != neighborhood_->end() ; ++neighbor_ptr ) {
				
			if( neighbor_ptr->property_value() == facies_index ){
				flag=true;
				break;
			}
		}

		// dilate/erode the current node
		if(flag){
			current_node_ptr->set_property_value( facies_index );
		} else{
			current_node_ptr->set_property_value( prop_->get_value(node_id) );
		}
	
		node_id++;
	}

	// update prop_ for next run 
	
	if(transformation > 2)
		update_prop_of_neighborhood();

	return;
}

void math_morphology::alternate_filtering(int faices_idx1,int faices_idx2, int &radius_idx) {
/*
	lower filtering : facies_idx1 --> background facies, facies_idx2 --> foreground facies
	upper filtering	: facies_idx1 --> foreground facies, facies_idx2 --> background facies 
*/

	if(radius_idx == radius.size()){
		return;
	} else{
		opening_closing(faices_idx1,faices_idx2);
		opening_closing(faices_idx2,faices_idx1);
		
		radius_idx++;
		if(radius.size() > 1)
			update_size_of_neighborhood(radius[radius_idx]);

		alternate_filtering(faices_idx1,faices_idx2,radius_idx);
	}
}

void math_morphology::opening_closing(int faices_idx1,int faices_idx2) {

/*
	opening : facies_idx1 --> background facies, facies_idx2 --> foreground facies
	closing	: facies_idx1 --> foreground facies, facies_idx2 --> background facies 
*/
	dilation_erosion(faices_idx1);
	dilation_erosion(faices_idx2);

	return;
}

void math_morphology::update_prop_of_neighborhood() {
	
	for(GsTLInt node_id=0;node_id<grid_->size();node_id++)
		prev_prop_->set_value(grid_->selected_property()->get_value(node_id),node_id);

	prop_ = prev_prop_;
	neighborhood_->select_property(prop_->name());
	
	return;
}

void math_morphology::update_size_of_neighborhood(double new_radius) {

	// update the neighborhood
	neighborhood_->clear(); 
	
	neighborhood_ = grid_->neighborhood(new_radius,new_radius,new_radius,0,0,0);

	neighborhood_->max_size(100000);
	neighborhood_->select_property(prop_->name());
	neighborhood_->includes_center(true);
	
	return;
}

bool math_morphology::initialize( const Parameters_handler* parameters,
                        Error_messages_handler* errors ) {
	
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
	GsTLGridProperty* prop = grid_->property( property_name );
	errors->report( prop==0, "Hdata_Property", "no such property in selected grid" );
	prop_ = prop;

	// get the name of the new property that will contain the result
	std::string new_property_name =
		parameters->value( "Simul_Property.value" );
	errors->report( new_property_name.empty(),"radius", "No  specified" );

	// get the type of the transformation and convert it into an int

	std::string transformation_name =
		parameters->value( "transformation.value" );
	if (transformation_name=="dilation") transformation=1;
	else if (transformation_name=="erosion") transformation=2;
	else if	(transformation_name=="lower filtering") transformation=3;
	else if (transformation_name=="upper filtering") transformation=4;

	// create temporary property to store a previous run
	if(transformation > 2){
		prev_prop_ = geostat_utils::add_property_to_grid( grid_, "previous_run" );
	}

	// get the facies category on which we have to apply the transformation
	// facies check
	facies_category = 
		String_Op::to_number<int>( parameters->value( "facies_category.value" ) );
	
	// get the shape used for the transformation ; sphere or ellipsoid
	std::string shape =
		parameters->value( "shape.value" );
	
	if (shape=="sphere"){
		std::string radius_str = parameters->value( "radius.value"  );
		errors->report( radius_str.empty(),"radius", "No radius specified" );
		radius = String_Op::to_numbers<double>( radius_str );
		// tell the grid to create a new neighborhood
		neighborhood_ = grid_->neighborhood(radius[0], radius[0], radius[0], 0, 0, 0);
	}
	else if (shape=="ellipsoid"){
		GsTLTriplet ellips_ranges;
		GsTLTriplet ellips_angles;
		
		radius.push_back(-999.);

		bool extract_ok = 
		geostat_utils::extract_ellipsoid_definition( ellips_ranges, ellips_angles,
	                                    				   "Search_Ellipsoid.value",
					                                       parameters, errors );
	
		if( !extract_ok ) return false;

		 extract_ok = geostat_utils::is_valid_range_triplet( ellips_ranges );
		 errors->report( !extract_ok,
                  "Search_Ellipsoid",
                  "Ranges must verify: major range >= " 
                  "medium range >= minor range >= 0" );
		 if( !extract_ok ) return false;
		 neighborhood_ = grid_->neighborhood( ellips_ranges, ellips_angles );
	}
	// get the radius of the sphere that is used for the transformation 
	
	 
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
	// std::string ranges_str =
	//	parameters->value( "Ranges.value" );

	// function "to_number" will convert the string into a vector of numbers
	// vector ranges will then contain all the number in string "ranges_str"
	// std::vector<float> ranges = String_Op::to_numbers<float>( ranges_str );
	 
	// tell the grid to create a new neighborhood
	// neighborhood_ = grid_->neighborhood( ranges[0], ranges[1], ranges[2],
	//									0, 0, 0 );
		

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

	if( !errors->empty() ) return false;

	return true;
}

Named_interface* math_morphology::create_new_interface( std::string& ) {
  return new math_morphology;
}


//GEOSTAT_PLUGIN(math_morphology);