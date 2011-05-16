#include <iostream>
#include <Inventor/Qt/SoQt.h>

#include <GsTLAppli/gui/appli/oinv_project_view.h>

#include <GsTLAppli/grid/library_grid_init.h>
#include <GsTLAppli/filters/library_filters_init.h>
#include <GsTLAppli/actions/library_actions_init.h>

#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>


#include <qapplication.h>

#include <string>


void new_cgrid( std::string name ) {
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "cgrid", gridModels_manager + "/" + name );

  Cartesian_grid* grid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
  if( grid ) {
    grid->add_property( "prop1" );
    grid->add_property( "prop2" );
    grid->add_property( "prop3" );
  }
}


int main(int argc, char **argv) {

  QApplication app(argc, argv);
  //app.setMainWidget( 0 );
  
  //  new_cgrid( "grid1" );
  //  new_cgrid( "grid2" );

  
  
  QWidget *main_window = SoQt::init(argv[0]);
  if (main_window == NULL) exit(1);

  libGsTLAppli_grid_init();
  libGsTLAppli_filters_init();
  libGsTLAppli_actions_init();
  

  //  Oinv_view* view = new Oinv_view(0, main_window);
  Project_view_gui* view = new Project_view_gui(main_window);

  SoQt::show(main_window);
  SoQt::mainLoop();
  
  return 0;
  //return app.exec();
}

