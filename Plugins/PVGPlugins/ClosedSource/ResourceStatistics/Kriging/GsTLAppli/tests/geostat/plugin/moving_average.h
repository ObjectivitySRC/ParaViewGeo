#ifndef __GSTLAPPLI_GEOSTAT_PLUGIN_BIDON_H__
#define __GSTLAPPLI_GEOSTAT_PLUGIN_BIDON_H__

#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>

class Geostat_grid;
class Neighborhood;

class MovingAverage : public Geostat_algo {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /** Runs the algorithm. 
   * @return 0 if the run was successful
   */
  virtual int execute() ;
  
  /** Tells the name of the algorithm
   */
  virtual std::string name() const { return "moving_average"; }
   
 private:
  Geostat_grid* grid_;
  SmartPtr<Neighborhood> neighborhood_;
};

#endif
