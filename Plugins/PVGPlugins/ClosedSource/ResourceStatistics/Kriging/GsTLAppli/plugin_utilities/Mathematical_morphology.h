#ifndef __sgems_math_morphology_H__
#define __sgems_math_morphology_H__

#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/math/gstlpoint.h> 


#define NDV -999
using namespace std;
#include <string>
#include <vector>
#include <fstream>

class Geostat_grid;
class Neighborhood;
class GsTLGridProperty;

class math_morphology : public Geostat_algo {
public:
  // initializes the parameters of the algorithm
  virtual bool initialize( const Parameters_handler* parameters,
                           Error_messages_handler* errors );

  // Runs the algorithm.
  virtual int execute() ;

  // Tells the name of the algorithm
  virtual std::string name() const { return "math_morphology"; }

public:
  static Named_interface* create_new_interface(std::string&);

private:
  // declare here all the parameters used by the algorithm
  // (i.e. the execute function)
  Geostat_grid* grid_;
  GsTLGridProperty* prop_;
  GsTLGridProperty* prev_prop_;
  Neighborhood* neighborhood_;

  int transformation, facies_category;
  vector<double> radius;

  void dilation_erosion(int facies_index);
  void opening_closing(int faices_idx1,int faices_idx2);
  void alternate_filtering(int faices_idx1,int faices_idx2, int &radius_idx);
  void update_prop_of_neighborhood();
  void update_size_of_neighborhood(double new_radius);
};

#endif