/**********************************************************************
** Author: Alexandre Boucher, Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#ifndef __GSTLAPPLI_GEOSTAT_ALGO_DSSIM_H__ 
#define __GSTLAPPLI_GEOSTAT_ALGO_DSSIM_H__ 
 
 
#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/geostat/geostat_algo.h> 
#include <GsTLAppli/geostat/utilities.h> 
#include <GsTLAppli/grid/grid_model/geostat_grid.h> 
#include <GsTLAppli/grid/grid_model/property_copier.h> 
 
#include <GsTL/geometry/covariance.h> 
#include <GsTL/cdf/First2_moments_cdf.h>
#include <GsTL/kriging/kriging_constraints.h> 
#include <GsTL/kriging/kriging_combiner.h> 
#include <GsTL/utils/smartptr.h>
 
#include <string> 
 
class Neighborhood; 
class Parameters_handler; 
class Error_messages_handler; 
//class Grid_initializer; 

 
class GEOSTAT_DECL dssim : public Geostat_algo { 
 public: 
  dssim(); 
  ~dssim(); 
   
  virtual bool initialize( const Parameters_handler* parameters, 
			   Error_messages_handler* errors ); 
  virtual int execute( GsTL_project* proj=0 ); 
  virtual std::string name() const { return "dssim"; } 
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 
 protected: 
  typedef Geostat_grid::location_type Location; 

  
  Geostat_grid* simul_grid_; 
  MultiRealization_property* multireal_property_; 
   
  Geostat_grid* harddata_grid_;   
  GsTLGridProperty* harddata_property_; 
  std::string harddata_property_name_; 
//  Grid_initializer* initializer_; 
  SmartPtr<Property_copier> property_copier_;
 
  SmartPtr<Neighborhood> neighborhood_; 
  
  long int seed_; 
  int nb_of_realizations_;
 
  Covariance<Location> covar_; 
  geostat_utils::KrigingCombiner* combiner_; 
  geostat_utils::KrigingConstraints* Kconstraints_;

  Cdf< float >* marginal_;
  //First2_moments_cdf* marginal_;
  First2_moments_cdf* ccdf_;
  
   
 // bool use_target_hist_; 
 // geostat_utils::NonParametricCdfType target_cdf_; 
 
 
 protected: 

	void get_cdf( const Parameters_handler* parameters,
		Error_messages_handler* errors );

//	 void set_extrapolation_tail(const Parameters_handler* parameters,
//	Error_messages_handler* errors, Non_param_cdf<>* nparam_cdf);
/*
  GsTLGridProperty* transform_variable( GsTLGridProperty* prop,
                                        Geostat_grid* grid,
                                        const Parameters_handler* parameters,
                                        const std::string& from_file_parameter_name,
                                        const std::string& file_parameter_name,
                                        Error_messages_handler* errors );

  bool setup_target_cdf( const Parameters_handler* parameters,
                         const std::string& from_file_parameter_name,
                         const std::string& file_parameter_name,
                         Error_messages_handler* errors );
*/
  
  void clean( GsTLGridProperty* prop = 0 );
}; 
 
 
 
 
 
 
 
#endif 
