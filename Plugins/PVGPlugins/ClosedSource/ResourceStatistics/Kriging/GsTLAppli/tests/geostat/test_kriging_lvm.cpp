#include <GsTLAppli/geostat/kriging.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/grid_model/grid_initializer.h>

#include <fstream>
#include <string>

int main() {

  const int grid_size = 10;
  // Build grid with locally varying mean
  Cartesian_grid* lvm_grid = new Cartesian_grid( grid_size, grid_size, 1 );
  GsTLGridProperty* prop = lvm_grid->add_property( "trend", typeid( float ) );
  
  for( int i=0; i< grid_size*grid_size/2 ; i++ ) {
    prop->set_value( 0.0, i );
  }
  for( int i=grid_size*grid_size/2; i< grid_size*grid_size ; i++ ) {
    prop->set_value( 10.0, i );
  }
  Colocated_neighborhood* coloc_neigh = 
    dynamic_cast<Colocated_neighborhood*>( 
	  lvm_grid->colocated_neighborhood( "trend" )
       );


  // Build kriging grid 
  Cartesian_grid* krig_grid = new Cartesian_grid( grid_size, grid_size, 1 );
  GsTLGridProperty* krig_prop = 
    krig_grid->add_property( string("krig"), typeid( float )  );
  krig_grid->select_property( "krig");

  // Build harddata grid
  const int pointset_size = 4;
  Point_set* harddata = new Point_set( pointset_size );
  std::vector<GsTLPoint> locations;
  locations.push_back( GsTLPoint( 0,0,0 ) );
  locations.push_back( GsTLPoint( 1,5,0 ) );
  locations.push_back( GsTLPoint( 8,8,0 ) );
  locations.push_back( GsTLPoint( 5,2,0 ) );
  harddata->point_locations( locations );
  GsTLGridProperty* hard_prop = harddata->add_property( "poro" );
  
  for( int i=0; i<pointset_size; i++ ) {
    hard_prop->set_value( i, i );
  }

  harddata->select_property( "poro" );


  // Set up covariance
  Covariance<GsTLPoint> cov;
  cov.nugget(0.1);
  cov.add_structure( "Spherical" );
  cov.sill( 0, 0.9 );
  cov.set_geometry( 0, 10,10,10, 0,0,0 );

  Grid_initializer initializer;
  initializer.assign( krig_grid,
		      krig_prop,
		      harddata,
		      "poro" );
	
  for( int i=0; i< krig_prop->size(); i++ ) {
    if( krig_prop->is_harddata( i ) )
      cout << "value at " << i << ": " 
	   << krig_prop->get_value( i ) << endl;
  }

  krig_grid->select_property( "krig");

  Neighborhood* neighborhood = krig_grid->neighborhood( 20, 20, 1, 0,0,0,
							&cov, true );

  typedef GsTLPoint Location;
  typedef std::vector<double>::const_iterator weight_iterator;
  typedef SKConstraints_impl< Neighborhood, Location > SKConstraints;
  typedef SK_local_mean_combiner<weight_iterator, Neighborhood,
                                 Colocated_neighborhood> LVM_combiner;
  typedef Kriging_constraints< Neighborhood, Location > KrigingConstraints;
  typedef Kriging_combiner< weight_iterator, Neighborhood > KrigingCombiner;


  LVM_combiner combiner( *coloc_neigh );
  SKConstraints constraints;


  // initialize the algo
  Kriging algo;
  algo.simul_grid_ = krig_grid;
  algo.property_name_ = "krig";
  algo.harddata_grid_ = 0;
  algo.neighborhood_ = neighborhood;
  algo.covar_ = cov;
  algo.combiner_ = new KrigingCombiner( &combiner );
  algo.Kconstraints_ = new KrigingConstraints( &constraints );


  // Run and output the results
  algo.execute();

  ofstream ofile( "result.out" );
  if( !ofile ) {
    cerr << "can't create file result.out" << endl;
    return 1;
  }

  GsTLGridProperty* prop1 = krig_grid->select_property( "krig" );
  ofile << "kriging" << endl << "1" << endl << "krig" << endl ;
  for( int i=0; i< prop1->size(); i++ ) {
    if( prop1->is_informed( i ) )
      ofile << prop1->get_value( i ) << endl;
    else
      ofile << "-99" << endl;
  }
}
