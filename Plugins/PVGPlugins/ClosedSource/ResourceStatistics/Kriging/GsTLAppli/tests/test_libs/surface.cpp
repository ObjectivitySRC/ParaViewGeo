#include "surface.h"
#include <GsTLAppli/appli/manager_repository.h>

#include <string>

Named_interface* create_surface(std::string& ) {
  return new Surface;
}

void Surface::init() {
  Root::instance()->new_interface("directory", "/GridObjects");
  SmartPtr<Named_interface> sub = Root::instance()->interface("/GridObjects");
  Manager* subdir = dynamic_cast<Manager*>(sub.raw_ptr());
  assert( subdir != 0);
  bool ok = subdir->factory("Surface", create_surface);
}
