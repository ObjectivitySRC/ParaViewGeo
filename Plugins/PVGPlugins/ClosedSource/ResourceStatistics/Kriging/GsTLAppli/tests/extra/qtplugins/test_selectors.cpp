#include <qapplication.h>
#include <qwidgetfactory.h>
#include <iostream>


int main( int argc, char **argv ) {

  QApplication app( argc, argv );
    
  QWidgetFactory factory;
  QWidget* wid = factory.createWidget( "OrderPropertySelector", 0 , 0 );
  if( wid ) {
    app.setMainWidget( wid );
    wid->show();
  }
  else 
    std::cerr << "merde" << std::endl;
  
  return app.exec();
}
