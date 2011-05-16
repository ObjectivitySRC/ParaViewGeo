#ifndef __GSTLAPPLI_GEOSTAT_PLUGIN_SET_UNINFORMED_H__
#define __GSTLAPPLI_GEOSTAT_PLUGIN_SET_UNINFORMED_H__


#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTL/utils/smartptr.h>
#include <vector>

class GsTLGridProperty;

typedef std::vector< GsTLGridProperty* > prop_vecT;
typedef std::vector< GsTLGridProperty* >::iterator prop_vec_itT;

//class __declspec(dllexport) Set_uninformed: public Geostat_algo {
class Set_uninformed: public Geostat_algo {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
	
	Set_uninformed( void );

	virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /** Runs the algorithm. 
   * @return 0 if the run was successful
   */
  
	virtual int execute();
  /** Tells the name of the algorithm
   */
	virtual std::string name() const { return "Set_uninformed"; }
   
  private:
	bool is_logical( float );

	Geostat_grid* grid_;
	GsTLGridProperty *base_prop;
	prop_vecT   props;
	bool is_set_uninf_on_prop;
	float uninf_val;
	int logical_op;
};

#endif
