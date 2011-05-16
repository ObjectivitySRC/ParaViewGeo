#ifndef __GSTLAPPLI_GEOSTAT_PLUGIN_MOVING_WINDOW_H__
#define __GSTLAPPLI_GEOSTAT_PLUGIN_MOVING_WINDOW_H__

#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <GsTL/matrix_library/tnt/cmat.h>
#include "GsTL_filters.h"
#include "Filtersim_filters.h"


//typedef std::vector< std::vector< double > > vec_vec_double;

typedef std::pair< GsTLGridProperty*, GsTLGridProperty* > pair_propT;


class Geostat_grid;
class Point_set;


class  Moving_window : public Geostat_algo {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:

	Moving_window();
	~Moving_window();

	virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /** Runs the algorithm. 
   * @return 0 if the run was successful
   */
  
	virtual int execute(GsTL_project *);
  /** Tells the name of the algorithm
   */
	virtual std::string name() const { return "Moving_window"; }
   
  private:

  typedef Neighborhood Smart_Neighborhood;
  //typedef SmartPtr<Neighborhood> Smart_Neighborhood;

	Geostat_grid* grid_;
	GsTL_filter<Smart_Neighborhood>* filters_;
  Smart_Neighborhood* neigh_;

  std::vector< GsTLGridProperty* > props_;

  Grid_template create_neigh_template( int nx, int ny, int nz );

};


#endif