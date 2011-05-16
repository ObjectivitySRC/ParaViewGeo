#ifndef __GSTLAPPLI_GEOSTAT_PLUGIN_GRIDSAMPLING_H__
#define __GSTLAPPLI_GEOSTAT_PLUGIN_GRIDSAMPLING_H__

#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <GsTLAppli/math/random_numbers.h>

class Geostat_grid;
class Point_set;


inline bool prop_greater ( pair<int, double> v1, pair<int, double> v2 ) 
{     return ( v1.second < v2.second );   }


class  gridSampling : public Geostat_algo {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
	virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /** Runs the algorithm. 
   * @return 0 if the run was successful
   */
  
	virtual int execute(GsTL_project* proj=0);
	//virtual int execute();
  /** Tells the name of the algorithm
   */
	virtual std::string name() const { return "gridSampling"; }
   
 private:
	virtual int randomSampling();
	virtual int stratRandomSampling();
	virtual int regularSampling();
	virtual int histogramSampling();
	virtual int extremeSampling();
	virtual int output();

	const Grid_template* gridSampling::fillTemplate(int sizeWin[3]);
	Geostat_grid* grid_;
	bool do_output;
	int nSamples;
	GsTLGridProperty *propSamp, *propHard;
	std::string samplingType, outFileName;
	const Parameters_handler* param_global;
};

#endif
