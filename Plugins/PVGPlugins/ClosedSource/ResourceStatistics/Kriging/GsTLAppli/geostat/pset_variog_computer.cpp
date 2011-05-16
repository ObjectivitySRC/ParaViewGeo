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

#include <GsTLAppli/geostat/pset_variog_computer.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/math/discrete_function.h>
#include <GsTLAppli/math/gstlvector.h>
#include <GsTLAppli/math/correlation_measure.h>
#include <GsTLAppli/math/direction_3d.h>
#include <GsTLAppli/utils/progress_notifier.h>

#include <GsTL/geometry/geometry_algorithms.h>
#include <GsTL/math/math_functions.h>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <functional>

#include <cmath>



Pset_variog_computer::Pset_variog_computer() {
  pset_ = 0; 
  head_prop_ = 0;
  tail_prop_ = 0;
  standardize_ = false;
}

Pset_variog_computer::
Pset_variog_computer( Point_set *pset, 
                      GsTLGridProperty* head_prop, 
                      GsTLGridProperty* tail_prop ) {
  pset_ = pset; 
  head_prop_ = head_prop;
  tail_prop_ = tail_prop;
  standardize_ = false;
}
                        
    
std::vector<int> 
Pset_variog_computer::
compute_variogram_values( Discrete_function& experim_variog,
                          const std::vector<double>& lag_tol,
                          const Direction_3d& direction,
			                    Correlation_measure* correl_measure,
                          Progress_notifier* progress_notifier ) {

  std::vector<double> lags = experim_variog.x_values();

  std::vector<int> pairs_counts(lags.size(), 0);
  if( !pset_ || !head_prop_ || !tail_prop_ || !correl_measure ) 
    return pairs_counts;

  // get a correlation measure object for each lag
  std::vector<Correlation_measure*> correl_measures;
  for( unsigned int i = 0 ; i < lags.size() ; i++ )
    correl_measures.push_back( correl_measure->clone() );
                                             
  const std::vector<Point_set::location_type>& points = pset_->point_locations();
  for( unsigned int i = 0 ; i < points.size() ; i++ ) {
    for( unsigned int j = i+1 ; j < points.size() ; j++ ) {

      // report progress and abort variogram computation if requested
      if( progress_notifier ) {
        if( !progress_notifier->notify() ) {
          pairs_counts.clear();
          return pairs_counts;
        }
      }
      

      /* Check if the 2 points are in the correct direction.
       * If they are, determine which lag the pair belongs to
       */
      if( !head_prop_->is_informed(i) || !head_prop_->is_informed(j) ) continue; 
      if( !tail_prop_->is_informed(i) || !tail_prop_->is_informed(j) ) continue; 
      if( !direction.is_colinear( points[i]-points[j] ) ) continue;

      double d = euclidean_distance( points[i], points[j] );
      std::vector<double>::iterator pos;
      pos = std::lower_bound( lags.begin(), lags.end(), d );

      int up_dist = std::distance( lags.begin(), pos );
      int low_dist = up_dist-1;
      int lag_id = -1;

      if( up_dist == 0 ) {
        if( *pos-lag_tol[0] <= d ) lag_id = 0;
      }
      else if( pos == lags.end() ) {
        if( *(pos-1) + lag_tol[low_dist] >= d ) lag_id = low_dist;
      }
      else if( *(pos-1) + lag_tol[low_dist] >= d ) 
        lag_id = low_dist;
      else if( *pos - lag_tol[up_dist] <= d ) 
        lag_id = up_dist;  
      

      if( lag_id < 0 ) continue;  // the pair doesn't belong to a lag

      Correlation_measure::ValPair head_prop_pair = 
        std::make_pair( head_prop_->get_value(i), head_prop_->get_value(j) );
      Correlation_measure::ValPair tail_prop_pair = 
        std::make_pair( tail_prop_->get_value(i), tail_prop_->get_value(j) );

      correl_measures[lag_id]->add_pair( head_prop_pair, tail_prop_pair );
//      correl_measures[lag_id]->add_pair( head_prop_->get_value(j),
//                                         tail_prop_->get_value(i) );

    }
  }

  experim_variog.set_no_data_value( Correlation_measure::NaN );
  std::vector<double> correlations( lags.size() );
  for( unsigned int i=0 ; i < correlations.size() ; i++ ){
    correlations[i] = correl_measures[i]->correlation();
    pairs_counts[i] = correl_measures[i]->pair_count();
  }


  //------------------
  // standardize the sill if requested

  if( standardize_ ) {
    //compute the covariance
    double covar = 0;
    double head_mean=0;
    double tail_mean=0;
    int count=0;
    for( int j=0; j < head_prop_->size() ; j++ ) {
      if( !head_prop_->is_informed(j) || !tail_prop_->is_informed(j) )
        continue;

      covar += head_prop_->get_value(j) * (tail_prop_->get_value(j) );
      head_mean += head_prop_->get_value(j);
      tail_mean += tail_prop_->get_value(j);
      count++;
    }
    if( count == 0 ) 
      covar = 1.0;
    else {
      covar /= double(count);
      covar -= head_mean/double(count) * tail_mean/double(count);
    }

    // standardize:
    for( unsigned int i=0; i < correlations.size() ; i++ ) {
      if( !GsTL::equals( correlations[i], Correlation_measure::NaN, 0.0001 ) )
        correlations[i] /= covar;
    }
  }

  experim_variog.set_y_values( correlations );

  // free memory
  for( unsigned int i = 0 ; i < correl_measures.size() ; i++ )
    delete correl_measures[i];

  return pairs_counts;
}

