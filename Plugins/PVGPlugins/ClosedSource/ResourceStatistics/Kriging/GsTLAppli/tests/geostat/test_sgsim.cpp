#include <GsTLAppli/geostat/sgsim.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/grid_model/grid_initializer.h>

#include <fstream>
#include <string>

int main() {

  const int grid_size = 100;

  // Build kriging grid 
  Cartesian_grid* krig_grid = new Cartesian_grid( grid_size, grid_size, 1 );

  // Set up covariance
  Covariance<GsTLPoint> cov;
  cov.nugget(0.1);
  cov.add_structure( "Spherical" );
  cov.sill( 0, 0.9 );
  cov.set_geometry( 0, 20,20,20, 0,0,0 );


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

  float x,y,z;
  cout << "enter ellips. dims: " << endl;
  cin >> x >> y >> z;

  cout << endl 
       << "====================================" << endl
       << "    starting" << endl << endl;

  // initialize the algo
  {
    Sgsim algo;
    algo.simul_grid_ = krig_grid;
    algo.multireal_property_ =
      krig_grid->add_multi_realization_property( "simul" );
    algo.neighborhood_ = SmartPtr<Neighborhood>(
				krig_grid->neighborhood( x,y,z,
				  0,0,0,
				 &cov )
				);
    algo.neighborhood_->max_size( 12 );
    algo.seed_ = 14071789;
    algo.nb_of_realizations_ = 1;
    algo.covar_ = cov;
    algo.combiner_ = new KrigingCombiner( &combiner );
    algo.Kconstraints_ = new KrigingConstraints( &constraints );



    // Run and output the results
    algo.execute();
  }
  ofstream out ( "sgsim.out" );
  cout << "writing the simulated values:" << endl;
  GsTLGridProperty* prop1 = krig_grid->select_property( "simul_#0" );
  out << "sgsim\n1\nprop\n";
  for( int i=0; i< prop1->size(); i++ ) {
    if( prop1->is_informed( i ) )
      out << prop1->get_value( i ) << endl;
    else
      out << "-99" << endl;
  }
  
}
