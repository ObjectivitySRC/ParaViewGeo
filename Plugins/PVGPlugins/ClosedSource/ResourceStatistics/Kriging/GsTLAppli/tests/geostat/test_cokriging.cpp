#include <GsTLAppli/geostat/cokriging.h>
#include <GsTLAppli/geostat/parameters_handler_impl.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/library_grid_init.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>


#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>


class ErrorChannel : public Scribe {
public:
  virtual void write( const std::string& str ) {
    std::cerr << str << endl;
  }
};



int main() {

  libGsTLAppli_grid_init();
  ErrorChannel chan;
  chan.subscribe( GsTLcerr );


  //----------------------------
  // set-up a new grid
  int nx=100;
  int ny=100;
  int nz=1;
  int cell_size = 1;

  SmartPtr<Named_interface> ni = 
      Root::instance()->new_interface( "cgrid", "/GridObject/Model/grid" );
  
  appli_assert( ni.raw_ptr() );
  Cartesian_grid* grid = dynamic_cast< Cartesian_grid* >( ni.raw_ptr() );

  appli_assert( grid != 0 );
  grid->set_dimensions( nx, ny, nz,
			cell_size, cell_size, cell_size );



  //------------------------
  // load training image

  ifstream infile( "train.dat" );
  if( ! infile ) {
    cerr << "train.dat: no such file or directory" << endl;
    return 1;
  }
  GsTLGridProperty* prop = grid->add_property( string("train") );
  
  string buffer;
  getline(infile, buffer,'\n');
  getline(infile, buffer,'\n');
  getline(infile, buffer,'\n');
  for( int i=0; i< 100*100 ; i++ ) {
    int val;
    infile >> val;
    prop->set_value( val, i );
  }


  //--------------------------
  // read the parameter file

  ifstream inparam( "cokriging.par" );
  if( !inparam ) {
    cerr << "cokriging.par: no such file or directory" << endl;
    return 1;
  }
  /*  ostringstream ostr;
  std::copy( istream_iterator<std::string>( inparam ), istream_iterator<std::string>(),
	     ostream_iterator<std::string>( ostr, " " ) );
  std::string parameters = ostr.str();
  */

  ostringstream ostr;
  std::string buff;
  while( inparam ) {
    getline( inparam, buff, '\n' );
    ostr << buff << "\n";
  }
  std::string parameters = ostr.str();
  cout << "parameters read: " << endl 
       << parameters << endl << endl;

  //-----------------------------
  //  run snesim
  Parameters_handler_xml param_handler( parameters );
  Error_messages_handler_xml errors;

  Cokriging algo;
  algo.initialize( &param_handler, &errors );
  algo.execute();



  //-------------------------
  // output the result

  ofstream ofile( "result.out" );
  if( !ofile ) {
    cerr << "can't create file result.out" << endl;
    return 1;
  }
  ofile << "snesim" << endl << "1" << endl << "facies" << endl ;

  int nb_reals = String_Op::to_number<int>( param_handler.value( "Nb_Realizations.value" ) );
  std::string prefix = param_handler.value( "Property_Name_Sim.value" );

  for( int i=1; i<=nb_reals; i++ ) {
    std::string prop_name = prefix + "_" + String_Op::to_string( i );
    GsTLGridProperty* prop1 = grid->select_property( prop_name );
    appli_assert( prop1 );
    std::copy( prop1->data(), prop1->data()+prop1->size(), 
	       ostream_iterator<float>( ofile, "\n" ) );
  }


  return 0;
}
