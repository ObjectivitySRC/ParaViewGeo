
#include <GsTLAppli/utils/gstl_plugins.h>

//#include "Set_uninformed.h"
#include "gridSampling.h"
//#include "ImageProcess.h"

//#include "grid_geometry_manipulation.h"
//#include "mathematical_morphology.h"
//#include "TauUpdating.h"

//#include "gstl_kmeans.h"
//#include "ML_classifier.h"
//#include "gstl_NN.h"

extern "C" __declspec(dllexport) int plugin_init() {

  //BIND_GEOSTAT_ALGO( Set_uninformed );
  BIND_GEOSTAT_ALGO( gridSampling );
  //BIND_GEOSTAT_ALGO( ImageProcess );
  //BIND_GEOSTAT_ALGO( dssim);
  //BIND_GEOSTAT_ALGO( grid_geom_manip );
  //BIND_GEOSTAT_ALGO( math_morphology );
  //BIND_GEOSTAT_ALGO( TauUpdating );
  //BIND_GEOSTAT_ALGO( trans);
  //BIND_GEOSTAT_ALGO( gstl_kmeans );
  //BIND_GEOSTAT_ALGO( gstl_ML_classifier );
  //BIND_GEOSTAT_ALGO( gstl_NN );
  return 0;
}