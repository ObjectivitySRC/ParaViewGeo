#include <GsTLAppli/geostat/snesim.h>
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


int main() {

  libGsTLAppli_grid_init();

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

  ifstream inparam( "snesim.par" );
  if( !inparam ) {
    cerr << "snesim.par: no such file or directory" << endl;
    return 1;
  }
  std::ostringstream ostr;
  std::copy( istream_iterator<std::string>( inparam ), istream_iterator<std::string>(),
	     ostream_iterator<std::string>( ostr, " " ) );
  std::string parameters = ostr.str();


  //-----------------------------
  //  run snesim
  Parameters_handler_xml param_handler( parameters );
  Error_messages_handler_xml errors;

  Snesim_generic algo;
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
	       ostream_iterator<int>( ofile, "\n" ) );
  }


  return 0;
  /*
   Grid_template* window = new Grid_template;
  window->add_vector( 0, 1, 0);
  window->add_vector( 1, 0, 0);
  window->add_vector( 0, -1, 0);
  window->add_vector( -1, 0, 0);
  window->add_vector( 1, 1, 0);
  window->add_vector( -1, 1, 0);
  window->add_vector( 1, -1, 0);
  window->add_vector( -1, -1, 0);
  window->add_vector( 0, 2, 0);
  window->add_vector( 2, 0, 0);
  window->add_vector( 0, -2, 0);
  window->add_vector( -2, 0, 0);
  window->add_vector( 2, 1, 0);
  window->add_vector( 1, 2, 0);
  window->add_vector( 2, -1, 0);
  window->add_vector( -2, 1, 0);
  
  std::vector<double> p_values( 2 );
  p_values[0]=0.7;
  p_values[1]=0.3;
  Categ_non_param_cdf<int> marg( 2, p_values.begin() );
  

  Snesim_generic algo;
  algo.simul_grid_ = grid;
  algo.property_name_prefix_ = "simul";
  algo.training_image_ = grid;
  algo.training_property_name_ = "train";
  algo.window_geom_ = window;
  algo.nb_reals_ = 2;
  algo.seed_ = 211175;
  algo.nb_facies_ = 2;
  algo.nb_multigrids_ = 4;
  algo.marginal_ = marg;
  algo.ccdf_ = marg;

  algo.execute();

  ofstream ofile( "result.out" );
  if( !ofile ) {
    cerr << "can't create file result.out" << endl;
    return 1;
  }

  GsTLGridProperty* prop1 = grid->select_property( "simul_1" );
  ofile << "snesim" << endl << "1" << endl << "facies" << endl ;
  for( int i=0; i< prop1->size(); i++ ) {
    ofile << prop1->get_value( i ) << endl;
  }
  GsTLGridProperty* prop2 = grid->select_property( "simul_2" );
  
  for( int i=0; i< prop2->size(); i++ ) {
    ofile << prop2->get_value( i ) << endl;
  }
  */
}
