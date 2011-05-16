#ifndef __GSTLAPPLI_GEOSTAT_PLUGIN_TRANS_H__
#define __GSTLAPPLI_GEOSTAT_PLUGIN_TRANS_H__

#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/math/gstlpoint.h>


class Geostat_grid;


class GEOSTAT_DECL trans : public Geostat_algo {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
	virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /** Runs the algorithm. 
   * @return 0 if the run was successful
   */
  
	virtual int execute( GsTL_project* proj=0 );
  /** Tells the name of the algorithm
   */
	virtual std::string name() const { return "trans"; }
   
 private:

	Geostat_grid* grid_;
		
	std::vector< float > reference_;

	bool is_local_cond_;
	
	std::vector<GsTLGridProperty*> props_;
	GsTLGridProperty* cond_prop_;
	std::vector<float> weights_;
	typedef std::vector<float>::iterator wgth_iterator;

	Cdf<float>* cdf_source_;
	Cdf<float>* cdf_target_;

	void cdf_transform(GsTLGridProperty* prop);
	void  cdf_transform_weighted( GsTLGridProperty* prop  );

	Cdf<float>* get_cdf( const Parameters_handler* parameters,
		Error_messages_handler* errors, std::string suffix );


};

#endif

