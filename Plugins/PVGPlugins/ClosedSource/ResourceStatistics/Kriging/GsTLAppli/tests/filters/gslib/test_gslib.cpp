#include <GsTLAppli/filters/library_filters_init.h>
#include <GsTLAppli/grid/library_grid_init.h>
#include <GsTLAppli/filters/gslib/gslib_filter.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qapplication.h>
#include <qmessagebox.h>

#include <fstream>


class QTscribe : public Scribe {
public:
  virtual void write( const std::string& str ) {
    QMessageBox::critical( 0, "Invalid Parameters", 
			   QString( str.c_str() ), 
			   QMessageBox::Ok, QMessageBox::NoButton );
    
  }
};



int main(int argc, char **argv) {

  QTscribe ess2;
  ess2.subscribe( GsTLcerr );

  QApplication a(argc, argv);

  libGsTLAppli_grid_init();
  libGsTLAppli_filters_init();
  
  cout << "init filter" << endl;

  Gslib_infilter filter;

  cout << "filter initialized" << endl;

  ifstream infile( "pset.gslib" );
  if( !infile ) {
    cerr << "Can not open train.dat" << endl; 
    return 0;
  }
  cout << "reading file" << endl;

  Geostat_grid* grid = filter.read( infile );
  Root::instance()->list_all( cerr );
  cout << "done" << endl;

  if( grid ) {
    std::list<std::string> properties = grid->property_list();
    for( std::list<std::string>::iterator it = properties.begin();
	 it != properties.end() ; ++it )
      cout << *it << endl;
    for( Geostat_grid::iterator it = grid->begin(); it !=grid->end(); ++it ) {
      cout << it->location() << "    " << it->property_value() << endl;
    }
  }  

  return a.exec();
}
