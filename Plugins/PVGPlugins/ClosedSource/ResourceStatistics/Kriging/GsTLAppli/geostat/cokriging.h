/**********************************************************************
** Author: Nicolas Remy
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

#ifndef __GSTLAPPLI_GEOSTAT_ALGO_COKRIGING_H__ 
#define __GSTLAPPLI_GEOSTAT_ALGO_COKRIGING_H__ 
 
 
#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/geostat/geostat_algo.h> 
#include <GsTLAppli/geostat/utilities.h> 
#include <GsTLAppli/grid/grid_model/neighborhood.h> 
 
#include <GsTL/geometry/covariance.h> 
#include <GsTL/kriging/covariance_set.h> 
#include <GsTL/kriging/cokriging_combiner.h> 
#include <GsTL/kriging/cokriging_constraints.h> 
 
#include <string> 
 
class Neighborhood; 
class Parameters_handler; 
class Error_messages_handler; 
 
 
class GEOSTAT_DECL Cokriging : public Geostat_algo { 
 public: 
  Cokriging(); 
  ~Cokriging(); 
   
  virtual bool initialize( const Parameters_handler* parameters, 
			   Error_messages_handler* errors ); 
  virtual int execute( GsTL_project* proj=0 ); 
  virtual std::string name() const { return "cokriging"; } 
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 
 protected: 
  typedef Geostat_grid::location_type Location; 
  typedef Covariance_set< Covariance<Location> > CovarianceSet; 
 
  typedef GsTL_neighborhood< SmartPtr<Neighborhood> > NeighborhoodHandle; 
 
  typedef std::vector< NeighborhoodHandle >::const_iterator NeighIterator; 
  typedef std::vector<double>::const_iterator WeightIterator; 
  typedef CoKriging_combiner< WeightIterator, NeighIterator > CoKrigingCombiner; 
  typedef CoKriging_constraints< NeighIterator, Location > KrigingConstraints; 
 
 
 protected:   
  Geostat_grid* simul_grid_; 
  std::string property_name_; 
   
  Geostat_grid* prim_harddata_grid_;  
  Geostat_grid* sec_harddata_grid_;  
  std::string primary_variable_; 
  std::string secondary_variable_; 
   
  bool assign_;
   
  std::vector< NeighborhoodHandle > neighborhood_vector_; 
   
  CovarianceSet* covar_; 
  CoKrigingCombiner* combiner_; 
  KrigingConstraints* Kconstraints_; 
 
  std::vector<double> kriging_weights_; 

  int min_neigh_1_, min_neigh_2_;
 
 

/*  virtual bool finish_covset_and_neighborhood( Covariance<Location>& C11, 
					       geostat_utils::Cokriging_type type, 
					       const Parameters_handler* parameters, 
					       Error_messages_handler* errors ); */
}; 
 
 
 
 
 
 
 
#endif 
