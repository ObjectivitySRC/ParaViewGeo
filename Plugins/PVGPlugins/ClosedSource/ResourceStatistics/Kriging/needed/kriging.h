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

#ifndef __GSTLAPPLI_GEOSTAT_ALGO_BLOCKKRIGING_H__ 
#define __GSTLAPPLI_GEOSTAT_ALGO_BLOCKKRIGING_H__ 
 
#include "utilities.h"
 
#include <GsTL/geometry/covariance.h> 
//#include <GsTL/geometry/Block_covariance.h>
 
#include <string> 
 
class Neighborhood; 
class RGrid;
 
 
class Kriging { 
 public: 
  Kriging(); 
  ~Kriging(); 
   
  virtual bool initialize( const Parameters_handler* parameters, 
			   Error_messages_handler* errors ); 
  virtual int execute( GsTL_project* proj=0 ); 
  virtual std::string name() const { return "kriging"; } 
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 protected:
   void clean( const std::string& prop ); 

 protected: 
  typedef Geostat_grid::location_type Location; 
  typedef std::vector<double>::const_iterator weight_iterator; 
  typedef Kriging_combiner< weight_iterator, Neighborhood > KrigingCombiner; 
  typedef Kriging_constraints< Neighborhood, Location > KrigingConstraints; 
 
  Geostat_grid* simul_grid_; 
  std::string property_name_; 
   
  Geostat_grid* harddata_grid_; 
  GsTLGridProperty* hdata_prop_;
  GsTLGridProperty* blk_hdata_prop_;
  std::string harddata_property_name_;
   
  Neighborhood* neighborhood_; 
 
  Covariance<Location> covar_;
  Covariance<Location>*  rhs_covar_;
//Block_covariance<Location>*  rhs_covar_;
  KrigingCombiner* combiner_; 
  KrigingConstraints* Kconstraints_; 
 
  std::vector<double> kriging_weights_;

  int min_neigh_;
  GsTLVector<int> nblock_pts_;
/* 
 protected: 
  bool set_kriging_parameters( const std::string& kriging_type, 
			       const Parameters_handler* parameters, 
			       Error_messages_handler* errors ); 
*/
}; 

 
#endif 
