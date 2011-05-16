#ifndef __GSTLAPPLI_GEOSTAT_NUMODEL_H__
#define __GSTLAPPLI_GEOSTAT_NUMODEL_H__


#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/math/gstlpoint.h>

#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/utils/error_messages_handler.h>

#include <vector>

class Geostat_grid;
class GsTLGridProperty;

class  NuTauModel : public Geostat_algo {
 
 public:

	virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /* Runs the algorithm. 
    @return 0 if the run was successful*/
	virtual int execute(GsTL_project *);

	virtual std::string name() const { return "NuTauModel"; }

   public:
     static Named_interface* create_new_interface(std::string&);


  private:

	Geostat_grid* grid_;
	std::vector< GsTLGridProperty* > props_;
	GsTLGridProperty* redun_prop_;
  GsTLGridProperty* nu_values_prop_;
  std::vector< GsTLGridProperty* > tau_values_props_;
  bool is_redun_prop_;
  bool is_nu_;
	float nu0_;
  std::vector<float> tau_;
  float x0_;

  float NuModel_compute(std::vector<float>& probs);
  float TauModel_compute(std::vector<float>& probs);
};


#endif