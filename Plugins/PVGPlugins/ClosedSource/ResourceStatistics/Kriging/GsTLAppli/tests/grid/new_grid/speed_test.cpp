#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>


void exec( Geostat_grid* grid ) {
  float count=0;
  Geostat_grid::iterator it = grid->begin();
  Geostat_grid::iterator end = grid->end();
  for(  ; it != end; ++it) {
    if( it->is_informed() )
      it->set_property_value( 10+ it->property_value() );
    else
      it->set_property_value( count++ );
  }
}

int main() {

  //-----------------
  // Build a grid

  GsTLRGrid rgrid;
  GsTLRGridGeometry* geom = new GsTLSimpleRGridGeometry();
  geom->set_size( 0, 1000 );
  geom->set_size( 1, 1000 );
  geom->set_size( 2, 10 );
  
  rgrid.set_geometry( geom );
  rgrid.add_property( "poro", typeid(double) );

  Strati_grid* grid = &rgrid;
  GsTLGridProperty* prop = grid->select_property("poro");
  grid->set_level(1);

  //-----------------



  //-----------------
  // Create a geovalue and use it to modify the grid
  cout << "-------------------------------" << endl
       << "Iterating on the grid and changing the values" << endl << endl;
  exec( grid );

  //------------------

}
