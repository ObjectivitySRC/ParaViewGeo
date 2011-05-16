#include <GsTLAppli/grid/grid_model/rgrid.h>

#include <stdlib.h>
#include <iostream>


template <class Iterator>
void simulate(Iterator begin, Iterator end) {
  int i=0;
  for( ; begin != end ; ++begin ) {
    begin->set_property_value( float( i++ ) );
    
  }
}



template <class Iterator>
void output(Iterator begin, Iterator end) {
  for( ; begin != end ; ++begin ) {
    cout << begin->property_value() << endl;
  }
}


int main() {
  RGrid rgrid;
  RGrid_geometry* geom = new Simple_RGrid_geometry();
  geom->set_size( 0, 5 );
  geom->set_size( 1, 5 );
  geom->set_size( 2, 1 );
  
  rgrid.set_geometry( geom );
  rgrid.add_property( "poro", typeid(double) );
  rgrid.add_property( "perm", typeid(double) );
  rgrid.add_property( "super", typeid(double) );

  Strati_grid* grid = &rgrid;
  grid->select_property("poro");
  grid->set_level(1);

  grid->init_random_path();
  simulate( grid->begin(), grid->end() );
  grid->select_property("perm");
  simulate( grid->begin(), grid->end() );
  grid->select_property("super");
  simulate( grid->begin(), grid->end() );

  //output( grid->random_path_begin(), grid->random_path_end() );
  
  grid->remove_property( "super" );
  
  grid->select_property("perm");
  output( grid->begin(), grid->end() );
 
}
