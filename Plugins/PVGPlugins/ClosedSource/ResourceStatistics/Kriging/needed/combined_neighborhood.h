/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_GRID_COMBINED_NEIGHBORHOOD_H__ 
#define __GSTLAPPLI_GRID_COMBINED_NEIGHBORHOOD_H__ 
 
 
#include "neighborhood.h"
#include "geostat_grid.h"

//#include <GsTL/utils/smartptr.h> // may need this?

#include <GsTL/geometry/covariance.h>
 
/** It is sometimes necessary to retrieve the neighbors of a given geovalue 
 * from several grids: in sequential gaussian simulation, the hard data can be 
 * a point set while we are simulating a cartesian grid. At each node, we need 
 * to retrieve the neighbors from both the pointset and the simulation grid, 
 * and put those neighbors into the same neighborhood. A Combined_neighborhood 
 * is designed to do just that. It retrieves the neighbors of a given geovalue  
 * from 2 different grids. The neighbors are first retrieved from the first 
 * grid, then from the second grid, up to the maximum number of neighbors 
 * required. 
 */ 
class Combined_neighborhood : public Neighborhood { 
  typedef Geostat_grid::location_type location_type;

 public: 
   /** covariance \c cov is copied by the neighborhood.
   */
  Combined_neighborhood( SmartPtr<Neighborhood> first, 
			                   SmartPtr<Neighborhood> second,
                         const Covariance<location_type>* cov = 0 ); 
   
  ~Combined_neighborhood();

  /** This function allows to change the property the SECOND 
   * neighborhood works on. It is not possible to change the 
   * behavior of the first neighborhood. 
   */  
  virtual bool select_property( const std::string& prop_name );  
  virtual const GsTLGridProperty* selected_property() const;
  
  virtual void includes_center( bool on );

  virtual void find_neighbors( const Geovalue& center ); 
  virtual void max_size( int s ); 
  virtual int max_size() const { return max_size_; } 
  virtual Geovalue center() const { return center_; } 
 
  virtual void set_neighbors( const_iterator begin, const_iterator end );

 protected: 
  SmartPtr<Neighborhood> first_; 
  SmartPtr<Neighborhood> second_;
  int max_size_; 
  Geovalue center_; 

  const Covariance<location_type>* cov_; 
}; 

class Combined_neighborhood_dedup : public Combined_neighborhood { 
  typedef Geostat_grid::location_type location_type;

 public: 
   /** covariance \c cov is copied by the neighborhood.
   */
  Combined_neighborhood_dedup( SmartPtr<Neighborhood> first, 
			                   SmartPtr<Neighborhood> second,
                         const Covariance<location_type>* cov = 0,
                         bool override_first = false ); 
   
  ~Combined_neighborhood_dedup(){}

  virtual void find_neighbors( const Geovalue& center );


 protected: 
   bool override_first_;
};

#endif 
