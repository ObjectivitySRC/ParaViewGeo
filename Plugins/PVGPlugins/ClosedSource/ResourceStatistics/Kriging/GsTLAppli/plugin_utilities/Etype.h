#ifndef __GSTLAPPLI_GEOSTAT_PLUGIN_ETYPE_H__
#define __GSTLAPPLI_GEOSTAT_PLUGIN_ETYPE_H__


#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/math/gstlpoint.h>


class Geostat_grid;

typedef std::vector< GsTLGridProperty* > prop_vecT;

class __declspec(dllexport) Etype : public Geostat_algo {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
	 Etype();
	virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /** Runs the algorithm. 
   * @return 0 if the run was successful
   */
  
	virtual int execute();
  /** Tells the name of the algorithm
   */
	virtual std::string name() const { return "Etype"; }
   
  private:

	Geostat_grid* grid_;
	prop_vecT props;
	GsTLGridProperty*  E_prop;

};

#endif