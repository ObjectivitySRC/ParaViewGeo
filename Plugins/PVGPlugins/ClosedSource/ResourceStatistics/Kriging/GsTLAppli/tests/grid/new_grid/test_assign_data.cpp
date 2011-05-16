#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/grid_initializer.h>

int main() {

  //-----------------
  // Build a grid

  Cartesian_grid rgrid;
  RGrid_geometry* geom = new Simple_RGrid_geometry();
  geom->set_size( 0, 9 );
  geom->set_size( 1, 9 );
  geom->set_size( 2, 1 );
  
  rgrid.set_geometry( geom );
  rgrid.add_property( "poro" );

  Strati_grid* grid = &rgrid;
  GsTLGridProperty* prop = grid->select_property("poro");
  grid->set_level(1);


  //-----------------
  // Initialize the grid
  for( int i=0; i< prop->size() ; i++ )
    prop->set_value( (float) i, i );



  //-----------------
  // Build harddata grid
  const int pointset_size = 5;
  Point_set* harddata = new Point_set( pointset_size );
  std::vector<GsTLPoint> locations;
  locations.push_back( GsTLPoint( 0,0,0 ) );
  locations.push_back( GsTLPoint( 1,5,0 ) );
  locations.push_back( GsTLPoint( 8,8,0 ) );
  locations.push_back( GsTLPoint( 5,2,0 ) );
  locations.push_back( GsTLPoint( 5,3,0 ) );
  harddata->point_locations( locations );
  GsTLGridProperty* hard_prop = harddata->add_property( "poro" );
  
  for( int j=0; j<pointset_size; j++ ) {
    hard_prop->set_value( 100+i, j );
  }

  harddata->select_property( "poro" );


  std::cout << "enter coarse grid level" << std::endl;
  int level = 3;
  //  cin >> level;
  grid->set_level( level );


  std::cout << "before: " << std::endl;
  for( int kk=8; kk>=0 ; kk-- ) {
    for( int ll=0; ll <9; ll++ ) {
      std::cout << prop->get_value( 9*kk+ll ) << "\t";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl << std::endl << std::endl;

  Grid_initializer ginit( &rgrid );
  ginit.does_overwrite( true );
  ginit.assign( prop, harddata, "poro");

  for( int k=8; k>=0 ; k-- ) {
    for( int l=0; l <9; l++ ) {
      std::cout << prop->get_value( 9*k+l ) << "\t";
    }
    std::cout << std::endl;
  }

  return 0;
}
