#ifndef __TEST_LIB_SURFACE_H__
#define __TEST_LIB_SURFACE_H__

#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <iostream>

Named_interface* create_surface();


class Surface : public Named_interface {
  public:
    Surface() {}
    void exec() { cout << "I am a surface" << endl;}
    static void init();
};


#endif
