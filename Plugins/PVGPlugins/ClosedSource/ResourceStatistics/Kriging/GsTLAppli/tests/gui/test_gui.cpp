#include "input_widget.h"
#include <GsTLAppli/gui/library_gui_init.h>
#include <GsTLAppli/grid/library_grid_init.h>

#include <qapplication.h>


int main(int argc, char **argv) {

  
  QApplication a(argc, argv);

  library_gui_init::init_lib();
  library_grid_init::init_lib();
  
  std::cout << std::endl << "current interfaces" << std::endl;
  Root::instance()->list_all( std::cout );

  Root::instance()->new_interface( "cgrid", gridModels_manager + "/myCgrid1" );
  Root::instance()->new_interface( "cgrid", gridModels_manager + "/myCgrid2" );

  std::cout << std::endl << "current interfaces" << std::endl;
  Root::instance()->list_all( std::cout );
  
  Input_widget widget("dynamic.dui");
  //  widget.resize(220,500);
  a.setMainWidget( &widget );

  widget.show();
  
  return a.exec();

}
