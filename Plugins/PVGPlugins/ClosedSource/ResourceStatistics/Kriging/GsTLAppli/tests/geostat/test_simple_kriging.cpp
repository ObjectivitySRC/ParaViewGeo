#include <GsTLAppli/geostat/kriging.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/grid_model/grid_initializer.h>

#include <fstream>
#include <string>

int main() {

  const int grid_size = 1;

  // Build kriging grid 
  Cartesian_grid* krig_grid = new Cartesian_grid( grid_size, grid_size, 1 );
  krig_grid->origin( GsTLPoint( 0.5, 0.5, 0.0 ) );

  // Build harddata grid
  ifstream infile( "cluster.txt");
  if( !infile ) return 1;  

  const int pointset_size = 140;
  Point_set* harddata = new Point_set( pointset_size );
  std::vector<GsTLPoint> locations;
  GsTLGridProperty* hard_prop = harddata->add_property( "poro" );
  
  for( int i=0; i<pointset_size; i++) {
    GsTLPoint loc;
    infile >> loc.x() >> loc.y();
    loc.z() = 0;
    locations.push_back( loc );

    float val;
    infile >> val;
    hard_prop->set_value( val, i );
  }
  harddata->point_locations( locations );

  harddata->select_property( "poro" );




  // Set up covariance
  Covariance<GsTLPoint> cov;
  cov.nugget(0.1);
  cov.add_structure( "Spherical" );
  cov.sill( 0, 0.9 );
  cov.set_geometry( 0, 10,10,10, 0,0,0 );



  krig_grid->select_property( "krig");

  Neighborhood* neighborhood = harddata->neighborhood( 20, 20, 1, 0,0,0,
						       &cov );
  neighborhood->max_size(4);

  typedef GsTLPoint Location;
  typedef std::vector<double>::const_iterator weight_iterator;
  typedef SKConstraints_impl< Neighborhood, Location > SKConstraints;
  typedef SK_local_mean_combiner<weight_iterator, Neighborhood,
                                 Colocated_neighborhood> LVM_combiner;
  typedef Kriging_constraints< Neighborhood, Location > KrigingConstraints;
  typedef Kriging_combiner< weight_iterator, Neighborhood > KrigingCombiner;
  typedef SK_combiner<weight_iterator, Neighborhood> SKCombiner;

  SKCombiner combiner( 0.0 );
  SKConstraints constraints;

  cout << endl 
       << "====================================" << endl
       << "    starting" << endl << endl;

  // initialize the algo
  {

    GsTLGridProperty* krig_prop = 
      krig_grid->add_property( "krig1", typeid( float )  );
    krig_grid->select_property( "krig1");

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
  }

  {
    krig_grid->add_property( "krig2", typeid( float )  );
    krig_grid->select_property( "krig2");
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
  }
  /*
  GsTLGridProperty* prop1 = krig_grid->select_property( "krig" );
  for( int i=0; i< prop1->size(); i++ ) {
    if( prop1->is_informed( i ) )
      cout << prop1->get_value( i ) << endl;
    else
      cout << "-99" << endl;
  }
  */
}
