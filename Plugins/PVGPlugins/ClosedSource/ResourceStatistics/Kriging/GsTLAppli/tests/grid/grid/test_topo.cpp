
#include <GsTLAppli/utils/gstl_types.h>
#include <GsTLAppli/grid/grid_topology.h>
#include <GsTLAppli/grid/rgrid.h>
#include <GsTLAppli/grid/rgridgeometry.h>

#include <iostream.h>




int main( int argc, char* argv[]) {
    GsTLInt nx = 101;
    GsTLInt ny = 101;
    GsTLInt nz = 10;
    
    GsTLInt nxyz = nx*ny*nz;
   

    GsTLRGrid rgrid1("grid1");
    GsTLRGridGeometry* geom = new GsTLSimpleRGridGeometry();
    geom->set_size(0, nx);
    geom->set_size(1, ny);
    geom->set_size(2, nz);

    rgrid1.set_geometry(geom);
    
    GsTLGridTopology* gt = rgrid1.topology();
    
    
    for( GsTLInt i = 0; i < nxyz; i++ ) {
        if( !gt->full_connection(i) ) {
            gt->print_flag(cout, i);
            bool ni = gt->next_i(i);
            bool nj = gt->next_j(i);
            bool nk = gt->next_k(i);
            bool pi = gt->prev_i(i);
            bool pj = gt->prev_j(i);
            bool pk = gt->prev_k(i);
       }
    }
    
    
    
    
    
    return 0;
}
