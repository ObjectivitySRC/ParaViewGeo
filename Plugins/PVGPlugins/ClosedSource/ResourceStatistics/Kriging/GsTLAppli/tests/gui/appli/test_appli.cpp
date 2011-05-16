#include <iostream>
#include <Inventor/Qt/SoQt.h>

#include <GsTLAppli/gui/appli/qt_sp_application.h>
#include <GsTLAppli/gui/appli/oinv_project_view.h>

#include <GsTLAppli/grid/library_grid_init.h>
#include <GsTLAppli/filters/library_filters_init.h>
#include <GsTLAppli/actions/library_actions_init.h>
#include <GsTLAppli/gui/library_gui_init.h>
#include <GsTLAppli/appli/library_appli_init.h>

#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>


#include <qapplication.h>

#include <string>




int main(int argc, char **argv) {
  
  QApplication app( argc, argv );

  // initialize libraries
  libGsTLAppli_grid_init();
  libGsTLAppli_filters_init();
  libGsTLAppli_actions_init();
  libGsTLAppli_gui_init();
  libGsTLAppli_appli_init();
  
  QSP_application* appli = new QSP_application( 0 );
  SoQt::init( appli );
  app.setMainWidget( appli );

  
  
  appli->init();
  appli->show();
  //  SoQt::show( appli );
  SoQt::mainLoop();
  
 
  return 0;
}

