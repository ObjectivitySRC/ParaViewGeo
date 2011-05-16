#include <GsTLAppli/extra/qtplugins/kriging_type_selector.h>

#include <qwidgetfactory.h>
#include <qapplication.h>

#include <iostream>


int main( int argc, char **argv ) {

  QApplication app( argc, argv );
  
  QWidgetFactory factory;
  QWidget* wid = factory.createWidget( "KrigingTypeSelector", 0 , 0 );
  
  if( wid ) {
    app.setMainWidget( wid );
    wid->show();
  }
  else 
    cerr << "merde" << endl;
  

  return app.exec();
}
