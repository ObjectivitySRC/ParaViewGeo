#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>

int main() {

  //-----------------
  // Build a grid

  RGrid rgrid;
  RGrid_geometry* geom = new Simple_RGrid_geometry();
  geom->set_size( 0, 5 );
  geom->set_size( 1, 5 );
  geom->set_size( 2, 1 );
  
  rgrid.set_geometry( geom );
  rgrid.add_property( "poro" );

  Strati_grid* grid = &rgrid;
  GsTLGridProperty* prop = grid->select_property("poro");
  grid->set_level(1);

  //-----------------



  //-----------------
  // Initialize the grid
  for( int i=0; i< prop->size() ; i++ )
    prop->set_value( (float) i, i );

  for( int i=0; i< prop->size() ; i++ )
    cout << prop->get_value( i ) << endl;
  cout << endl;
  //-----------------



  //-----------------
  // Create a geovalue and use it to modify the grid
  cout << "-------------------------------" << endl
       << "Getting a geovalue (index=2)" << endl << endl;
  Geovalue gval = rgrid.geovalue( 2 );
  cout << "location: " << gval.location() << endl;
  cout << "is informed? " << gval.is_informed() << endl;
  gval.set_property_value( 2.3 ); 
  cout << "value changed to : " << gval.property_value() << endl;

  cout << endl << "this is the new grid" << endl;
  for( int i=0; i< prop->size() ; i++ )
    cout << prop->get_value( i ) << endl;
  cout << endl;
  //----------------
  

  //-----------------
  // Create a geovalue and use it to modify the grid
  cout << "-------------------------------" << endl
       << "Iterating on the grid and changing the values" << endl << endl;
  for( RGrid::iterator it = rgrid.begin() ; it!= rgrid.end(); ++it) {
    it->set_property_value( 10+ it->property_value() );
    cout << "location: " << it->location() 
	 << "   val= " << it->property_value() << endl;
  }
  cout << endl << "this is the new grid" << endl;
  for( int i=0; i< prop->size() ; i++ )
    cout << prop->get_value( i ) << endl;
  cout << endl;

  //------------------


  //-----------------
  // Work on a coarser grid
  cout << "-------------------------------" << endl
       << "Working on coarse grid 2" << endl << endl;
  rgrid.set_level( 2 );
  RGrid::iterator it = rgrid.begin();
  RGrid::iterator end = rgrid.end();
  for(  ; it!= end; ++it) {
    it->set_property_value( 100+ it->property_value() );
    cout << "location: " << it->location() 
    	 << "   val= " << it->property_value() << endl;
  }
  cout << endl << "this is the new grid" << endl;
  for( int i=0; i< prop->size() ; i++ )
    cout << prop->get_value( i ) << endl;
  cout << endl;


  cout << "-------------------------------" << endl
       << "Working on coarse grid 3" << endl << endl;
  rgrid.set_level( 3 );
  it = rgrid.begin();
  end = rgrid.end();
  for(  ; it!= end; ++it) {
    it->set_property_value( 300+ it->property_value() );
    cout << "location: " << it->location() 
    	 << "   val= " << it->property_value() << endl;
  }
  cout << endl << "this is the new grid" << endl;
  for( int i=0; i< prop->size() ; i++ )
    cout << prop->get_value( i ) << endl;
  cout << endl;
  //------------------


  rgrid.add_property( "perm" );
  std::list<std::string> names = rgrid.property_list();
  cout << endl 
       << "list of properties: " << endl;
  for( std::list<std::string>::iterator it = names.begin(); it!= names.end(); ++it)
    cout << *it << endl;
}
