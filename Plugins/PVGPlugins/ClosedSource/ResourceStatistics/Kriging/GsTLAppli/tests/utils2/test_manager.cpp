#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/utils/singleton_holder.h>

#include <GsTLAppli/tests/test_libs/surface.h>
#include <GsTLAppli/tests/test_libs/parser/parser.h>

#include <GsTLAppli/geostat/geostat_library.h>

#include <iostream>
#include <string>
#include <cassert>


Named_interface* new_directory() {
  return new Manager;
}


typedef Singleton_holder<Manager> Root; 

int main() {

  
  Parser parser;
  Root::instance()->factory("directory", new_directory);

  Geostat_library::init_lib();

  cout << endl
       << "===============================" << endl;
  
  Root::instance()->list_all(std::cout); 
  cout << endl << endl
       << "Loading surfaces" << endl << endl;
  Surface::init();
  Root::instance()->list_all(std::cout);

  Root::instance()->new_interface("Surface", "/GridObjects/surf1");
  Root::instance()->new_interface("Surface", "/GridObjects/surf2");
  Root::instance()->new_interface("Surface", "/GridObjects/surf3");
  
  cout << endl
       << "===============================" << endl;
  Root::instance()->list_all(std::cout);
  
  std::string command;
  while(command != "quit") {
    cout << "> " ;
    cin >> command;
    //cout << "you typed: " << command << std::endl;
    if(command == "quit")
      break;
    else
      parser.execute(command);
  }
}
 
