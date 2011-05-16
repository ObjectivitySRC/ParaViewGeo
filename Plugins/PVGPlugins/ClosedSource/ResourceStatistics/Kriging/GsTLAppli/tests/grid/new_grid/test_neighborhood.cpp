#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>

#include <GsTL/geometry/geometry_algorithms.h>

int main() {

  //-----------------
  // Build a grid

  RGrid rgrid;
  RGrid_geometry* geom = new Simple_RGrid_geometry();
  geom->set_size( 0, 100 );
  geom->set_size( 1, 100 );
  geom->set_size( 2, 1 );
  
  rgrid.set_geometry( geom );
  rgrid.add_property( "poro" );
  rgrid.add_property( "perm" );

  Strati_grid* grid = &rgrid;
  GsTLGridProperty* prop = grid->select_property("poro");
  grid->set_level(1);

  //-----------------



  //-----------------
  // Initialize the grid
  for( int i=0; i< prop->size() ; i++ )
    prop->set_value( (float) i, i );

  /*
  for( int i=0; i< prop->size() ; i++ )
    cout << prop->get_value( i ) << endl;
  cout << endl;
  */

  GsTLGridProperty* prop2 = grid->select_property("perm");
  for( int i=0; i< prop->size() ; i++ )
    prop2->set_value( (float) 2*i, i );
  grid->select_property("poro");
  //-----------------

  /*
  //-----------------
  // Test window neighborhood
  cout << "-------------------------------" << endl
       << "Testing window neighborhood. Using coarse grid 1" << endl << endl;
  rgrid.set_level( 1 );
  Grid_template templ;
  templ.add_vector( 0, 1, 0);
  templ.add_vector( 1, 0, 0);
  templ.add_vector( 0, -1, 0);
  templ.add_vector( -1, 0, 0);
  
  Window_neighborhood* window = rgrid.window_neighborhood( templ );
  Geostat_grid::iterator it = rgrid.begin();
  Geostat_grid::iterator end = rgrid.end();
  for(  ; it!= end; ++it) {
    window->find_neighbors( *it );
    Neighborhood::iterator neigh_begin = window->begin();
    Neighborhood::iterator neigh_end = window->end();
    cout << "neighbors of (" << it->location() << ") :    " ;
    for( ; neigh_begin != neigh_end ; ++neigh_begin ){
      if( neigh_begin->is_informed() )
	cout << neigh_begin->property_value() << "  "; 
      else
	cout << "-99" << "  " ;
    }
    cout << endl;
  }

  

  delete window;
  //-----------------


  //-----------------
  // Test colocated neighborhood
  Neighborhood* coloc_neighborhood = rgrid.colocated_neighborhood( "perm" );
  it = rgrid.begin();
  for(  ; it!= end; ++it) {
    cout << "center: " << it->location()
	 << " value: " << it->property_value() << "  ";
    coloc_neighborhood->find_neighbors( *it );
    cout << "colocated: " << coloc_neighborhood->begin()->location() 
	 << "  value: " << coloc_neighborhood->begin()->property_value() 
	 << endl;
  }
  */

  //-----------------
  // Test ellipsoid neighborhood
  cout << " enter x,y,z" << endl;
  int x,y,z;
  cin >> x >> y >> z;

  SmartPtr<Neighborhood> ellips = rgrid.neighborhood( x,y,z, 0,0,0 ) ;
  ellips->max_size( 1000 );

  Geovalue center = rgrid.geovalue( 4,4,0 );
  ellips->find_neighbors( center );
  Neighborhood::iterator neigh_begin = ellips->begin();
  Neighborhood::iterator neigh_end = ellips->end();
  cout << "found " << ellips->size() 
       << " neighbors of (" << center.location() << ") : " << endl ;
  /*  for( ; neigh_begin != neigh_end ; ++neigh_begin ){
    if( neigh_begin->is_informed() )
      cout << neigh_begin->location() << "   d="
	   << euclidean_distance( neigh_begin->location(), center.location() )
	   << endl; 
    else
      cout << "-99" << "  " ;
  }
  cout << endl;
  */
}
