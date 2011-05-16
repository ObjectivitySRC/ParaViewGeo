#include <GsTLAppli/extra/qtplugins/variogram_input.h>

#include <qwidgetfactory.h>
#include <qapplication.h>

#include <iostream>


int main( int argc, char **argv ) {

  QApplication app( argc, argv );
    
  QWidgetFactory factory;
  //QWidget* wid = factory.createWidget( "VariogramInput", 0 , 0 );
  QWidget* wid = factory.create( "form1.dui", 0, 0 , 0 );
  if( wid ) {
    app.setMainWidget( wid );
    wid->show();
  }
  else 
    cerr << "merde" << endl;
  

  return app.exec();
}
