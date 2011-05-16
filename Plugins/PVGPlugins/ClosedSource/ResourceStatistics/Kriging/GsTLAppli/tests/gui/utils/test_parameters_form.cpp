#include <GsTLAppli/gui/utils/parameters_form.h>

#include <qapplication.h>


int main(int argc, char **argv) {
  
  QApplication app( argc, argv );

  QWidget* appli = new 
  app.setMainWidget( appli );

  
  
  appli->init();
  appli->show();
 
  return app.exec();
}

