#ifndef __SNESIM_PARAMETERS__
#define __SNESIM_PARAMETERS__


#include <cdf/categ_non_param_cdf.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


class Parameters{
 public:
  typedef float                                  PropertyType;
  typedef categ_non_param_cdf<PropertyType>               Cdf;
  typedef GsTLGridGeovalue                                GeoValue;
  

  Parameters(std::string parameter_file);
  ~Parameters();
  
  

 public:

  GsTLRGrid* simulation_grid;
  GsTLRGrid* training_image;

  std::vector<GeoValue> hard_data;

  GsTLGridTemplate  grid_template;
  int nb_of_facies, nb_of_multiple_grids;
  Cdf* ccdf; 
  Cdf* marginal_cdf; 

  int number_of_realizations;
  long int seed;

  double constraint_to_target_cdf;

  ofstream output;
};

#endif
