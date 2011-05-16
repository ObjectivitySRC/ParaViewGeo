#include <qapplication.h>
#include <qwidgetfactory.h>
#include <iostream>

#include <GsTLAppli/extra/qtplugins/selectors.h>

int main( int argc, char **argv ) {

  QApplication app( argc, argv );
  QWidgetFactory factory;
  QWidget* wid = factory.createWidget( "PropertySelector", 0 , 0 );
  if( wid ) {
    app.setMainWidget( wid );
    wid->show();
  }
  else 
    cerr << "merde" << endl;

  return app.exec();
}
