#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/utils/singleton_holder.h>


#include <iostream>
#include <string>
#include <cassert>

class Surface : public Named_interface {
public:
  Surface() {};
};

Named_interface* new_directory() {
  return new Manager;
}
Named_interface* new_surface() {
  return new Surface;
}


typedef Singleton_holder<Manager> Root; 

int main() {
  
  Root::instance()->factory("directory", new_directory);
  Root::instance()->new_interface("directory", "/GridObjects");
  Root::instance()->new_interface("directory", "/Algorithms");
  Root::instance()->new_interface("directory", "/QtWidgets");

  cout << "===============================" << endl
       << "    First step" << endl << endl;

  Root::instance()->list_all(std::cout);
  cout << endl << endl;


  SmartPtr<Named_interface> sub = Root::instance()->interface("/GridObjects");
  Manager* subdir = dynamic_cast<Manager*>(sub.raw_ptr());
  assert( subdir != 0);
  bool ok = subdir->factory("directory", new_directory);
  assert( ok );

  Root::instance()->new_interface("directory", "/GridObjects/3Dsurf");
  Root::instance()->new_interface("directory", "/GridObjects/CartesianGrid");
  
  sub = Root::instance()->interface("/GridObjects/3Dsurf");
  
  subdir = dynamic_cast<Manager*>(sub.raw_ptr());
  assert(subdir != 0);
  subdir->factory("surface", new_surface);
   
  Root::instance()->new_interface("surface", "/GridObjects/3Dsurf/surf1");
  Root::instance()->new_interface("surface", "/GridObjects/3Dsurf/surf2");
  Root::instance()->new_interface("surface", "/GridObjects/3Dsurf/surf3");
  

  cout << endl << endl
       << "===============================" << endl
       << "    Second step" << endl << endl;

  Root::instance()->list_all(std::cout); 
  cout << endl << endl;



  
  sub = Root::instance()->interface("/GridObjects/3Dsurf/surf2");
  
  SmartPtr<Named_interface> sub2 = sub;
  cout << "references on surf2: " << sub->references() << endl;
  
  SmartPtr<Named_interface> surf2 = Root::instance()->interface("/GridObjects/3Dsurf/surf2");
  cout << "references on surf2: " << sub->references() << endl;

  cout << "deleting surf1" << endl;
  Root::instance()->delete_interface("/GridObjects/3Dsurf/surf1");

  cout << endl << endl
       << "===============================" << endl
       << "    Third step" << endl << endl;

  Root::instance()->list_all(std::cout); 
  cout << endl << endl;



  
  
  Root::instance()->delete_interface("/GridObjects/3Dsurf");
  cout << endl << endl
       << "deleting /GridObjects/3Dsurf" << endl
       << "===============================" << endl
       << "    Fourth step" << endl << endl;

  Root::instance()->list_all(std::cout); 
  cout << endl << endl;
  
  cout << "references on surf2: " << sub->references() << endl;
}
 
