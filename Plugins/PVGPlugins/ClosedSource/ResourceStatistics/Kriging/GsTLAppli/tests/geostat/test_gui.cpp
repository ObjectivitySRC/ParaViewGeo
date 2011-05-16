#include "input_widget.h"
#include <GsTLAppli/gui/QT_wrappers/wrapper_library.h>

#include <qapplication.h>


int main(int argc, char **argv) {

  
  QApplication a(argc, argv);
  QTWrapper_library::init_lib();
  
  Input_widget widget("./snesim.ui");
  //  widget.resize(220,500);
  a.setMainWidget( &widget );

  widget.show();
  
  return a.exec();
}
