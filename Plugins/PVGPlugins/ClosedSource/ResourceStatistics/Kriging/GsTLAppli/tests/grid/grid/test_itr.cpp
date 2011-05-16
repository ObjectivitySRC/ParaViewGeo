#include <GsTLAppli/utils/gstl_types.h>
#include <GsTLAppli/utils/clock.h>
#include <GsTLAppli/grid/grid_topology.h>
#include <GsTLAppli/grid/rgrid.h>
#include <GsTLAppli/grid/rgridgeometry.h>
#include <GsTLAppli/grid/grid_iterators.h>
#include <GsTLAppli/grid/grid_gval_accessor.h>

#include <iostream.h>
#include <algorithm>
#include <cmath>
#include <stdlib.h>

class stl_rand48_generator{
 public:
  stl_rand48_generator(long int seed = 21111975)  {srand48(seed);}
 
  void initialize(long int seed)  {srand48(seed);};
 
  template<class IntegerType>
  inline IntegerType operator()(IntegerType N) {
    double p = drand48();
    return static_cast<IntegerType>(p*N);
  }
 
  inline double operator()() {
    return drand48();
  }
};


int main( int argc, char* argv[]) {
    GsTLInt nx = 100;
    GsTLInt ny = 100;
    GsTLInt nz = 1;
    
    GsTLInt nxyz = nx*ny*nz;
   

    GsTLRGrid rgrid1("grid1");
    GsTLRGridGeometry* geom = new GsTLSimpleRGridGeometry();
    geom->set_size(0, nx);
    geom->set_size(1, ny);
    geom->set_size(2, nz);

    rgrid1.set_geometry(geom);
    
    GsTLGridTopology* gt = rgrid1.topology();

    rgrid1.add_property("toto", typeid(int));

    GsTLGridProperty* prop =  rgrid1.find_property("toto");
    rgrid1.select_property("toto");
    rgrid1.set_level(1);
    
    if (prop != 0) {
        std::cerr << "Property " << prop->name() << " found ...." << std::endl;
    }
   
    prop = rgrid1.find_property("titi");

    stl_rand48_generator grand(101);

    rgrid1.init_random_path(grand);

   
    if (prop == 0) {
        std::cerr << "Property titi not found ...." << std::endl;
    }
  
    USClock* c1 = new USClock();
    GsTLInt ii = 0;
    GsTLGridGeoValueRandomIterator end =  rgrid1.random_path_end();
    for (GsTLGridGeoValueRandomIterator it = rgrid1.random_path_begin(); it != end; it++) {
        ii++;
        float val = ii;
        it->set_property_value(val);
    }
    std::cerr << "Write " << std:: endl;
    delete c1;

    c1 = new USClock();
    {for (GsTLGridGeoValueRandomIterator it = rgrid1.random_path_begin(); it != end; it++) {
        float val  = it->property_value();
        GsTLPoint loc  = it->location();
        std::cerr << loc[0] << " " << loc[1] << " " << loc[2] << " " << val << endl;
    }}
    std::cerr << "Read " << std:: endl;
    delete c1; 
    
    return 0;
}
