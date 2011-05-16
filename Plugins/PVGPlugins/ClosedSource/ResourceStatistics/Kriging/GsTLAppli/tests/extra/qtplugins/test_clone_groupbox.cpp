#include "clone_groupbox.h"

#include <qapplication.h>

#include "test.h"


int main( int argc, char **argv ) {

  QApplication app( argc, argv );
    
  Form1* form = new Form1;
  form->show();
  
  return app.exec();
}
