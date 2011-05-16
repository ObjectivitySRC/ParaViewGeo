#include <GsTLAppli/utils/gstl_types.h>
#include <GsTLAppli/utils/clock.h>
#include <GsTLAppli/grid/grid_topology.h>
#include <GsTLAppli/grid/rgrid.h>
#include <GsTLAppli/grid/rgridgeometry.h>
#include <GsTLAppli/grid/grid_iterators.h>
#include <GsTLAppli/grid/grid_neighborhood.h>
#include <GsTLAppli/grid/grid_gval_accessor.h>

#include <iostream.h>




#include <cdf_estimator/search_tree.h>
#include <simulation/sequential_simulation.h>
#include "parameters.h"
#include "parameters.cpp"
#include <servo_system.h>

#include <iostream>


template <class Iterator, class Cdf,  class ServoSystem , class RandomNumberGenerator>
bool run_snesim(
    GsTLRGrid* simulation_grid,
    const std::string& simul_prop,
    GsTLRGrid* training_image,
    const std::string& training_prop,
    Iterator harddata_first,
    Iterator harddata_end,
    GsTLGridTemplate* grid_template,
    const Cdf& marginal_cdf, Cdf& ccdf,
    int nb_of_multiple_grids,
    int nb_of_facies,
    RandomNumberGenerator& generator,
    ServoSystem& servo_syst
) {
    
    GsTLGridProperty* p1 = simulation_grid->find_property(simul_prop);
    if( p1 == 0 ) {
        return false;
    }
    
    GsTLGridProperty* p2 = training_image->find_property(training_prop);
    if( p2 == 0 ) {
        return false;
    }
    
    
    simulation_grid->select_property(simul_prop);
    training_image->select_property(training_prop);
    
    for( int level = nb_of_multiple_grids; level >=1; --level ) {
        training_image->set_level(level);
        GsTLGridNeighborhood training_neighbors(training_image,  training_prop, "", *grid_template);
        
        GsTLGridGeoValueIterator begin = training_image->begin();
        GsTLGridGeoValueIterator end   = training_image->end();
        search_tree<int> tree(begin, end, training_neighbors, grid_template->size(), nb_of_facies);
        
        // To Do reproject data from fine to coarse grid ...
        // and the harddata ....
        //
        
        
        simulation_grid->set_level(level);
        GsTLGridNeighborhood neighbors(simulation_grid,  simul_prop, "", *grid_template);
        
        simulation_grid->init_random_path(generator);
        
        GsTLGridGeoValueRandomIterator random_begin = simulation_grid->random_path_begin();
        GsTLGridGeoValueRandomIterator random_end = simulation_grid->random_path_end();
        
        
        for( GsTLGridGeoValueRandomIterator it = random_begin; it !=random_end; it++ ) {
            if ( it->is_informed() ) continue;
            GsTLPoint current_loc = it->location();
            neighbors.find_neighbors(current_loc);

            tree(current_loc, neighbors, ccdf);
	    servo_syst(ccdf);

            float val =  ccdf.inverse(generator());          
            it->set_property_value(val);

	    servo_syst.update(val);
        }
    }
    return true;
}



int main( int argc, char* argv[]) {
    std::string parameter_file;
    
    if( argc!=2 ) {
        cerr << "No parameter file name supplied...   Using snesim.par" << endl << endl;
        parameter_file = "snesim.par";
    }
    else
    parameter_file = argv[1];
    
    Parameters param(parameter_file);
    
    stl_rand48_generator generator(param.seed);
    
    typedef servo_system<Parameters::Cdf> ServoSystem;
    ServoSystem servo_syst(*param.marginal_cdf, param.constraint_to_target_cdf ,
			   param.hard_data.begin(), param.hard_data.end());


    std::string simul_prop_name = "facies";
    std::string image_prop_name = "facies";
    
    USClock *c1 = new USClock;
    run_snesim(
        param.simulation_grid, simul_prop_name, 
        param.training_image, image_prop_name,
        param.hard_data.begin(), param.hard_data.end(),
        &param.grid_template,
        *param.marginal_cdf, *param.ccdf,
        param.nb_of_multiple_grids, 
        param.nb_of_facies,
        generator, servo_syst
    );

    delete c1;
    
    cout << "writting result" << endl;
    // write gslib header
    param.output << "snesim" << endl
    << "1" << endl
    << "facies" << endl;
    
    param.simulation_grid->set_level(1);
    GsTLGridProperty* prop =  param.simulation_grid->find_property(simul_prop_name);
    GsTLFloatDataArray* data = prop->data();

    // write values
    for(int i = 0; i < data->size(); ++i )
       param.output << data->get_value(i) << endl;
    return 0;
}




