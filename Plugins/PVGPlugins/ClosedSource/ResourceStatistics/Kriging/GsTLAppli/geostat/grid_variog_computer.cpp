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

#include <GsTLAppli/geostat/grid_variog_computer.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h>
#include <GsTLAppli/math/discrete_function.h>
#include <GsTLAppli/math/correlation_measure.h>
#include <GsTLAppli/utils/progress_notifier.h>

#include <GsTL/geometry/geometry_algorithms.h>
#include <GsTL/math/math_functions.h>

#include <algorithm>
#include <math.h>
#include <numeric>
#include <iterator>



Grid_variog_computer::Grid_variog_computer() {
  grid_ = 0; 
  head_prop_ = 0;
  tail_prop_ = 0;
  standardize_ = false;
}

Grid_variog_computer::
Grid_variog_computer( Strati_grid *grid, 
                      GsTLGridProperty* head_prop, 
                      GsTLGridProperty* tail_prop ) {
  grid_ = grid; 
  head_prop_ = head_prop;
  tail_prop_ = tail_prop;
  standardize_ = false;
}



std::vector<int> 
Grid_variog_computer::
compute_variogram_values( Discrete_function &experim_variog,
                          GsTLVector<double> direction,
                          int lags_count,
                          Correlation_measure* correl_measure_prototype,
                          Progress_notifier* progress ) {

  std::vector<int> num_pairs;
  if( !grid_ || !head_prop_ || !tail_prop_ ) return num_pairs;

  const int nx = grid_->nx();
  const int ny = grid_->ny();
  const int nz = grid_->nz();

  SGrid_cursor cursor = *( grid_->cursor() );
  cursor.set_multigrid_level(1);

  double covar = 1.0;
  /* Disabling standardize because it doesn't always work in the case of
   * indicator variograms: the covariance used is always the covariance of
   * the property while it should be the covariance of the indicators in the 
   * case of indicator variograms

  if( standardize_ ) 
    covar = compute_covariance();
  */
  std::vector<double> x_values;
  std::vector<double> y_values;

  for ( int lag = 0 ;  lag < lags_count; lag++ ) {

    Correlation_measure* correl_measure = correl_measure_prototype->clone();
    GsTLVector<int> step = double(lag+1) * direction;

    for( int u = 0 ; u < nx ; u++ ) {
      for( int v = 0 ; v < ny ; v++ ) {
        for( int w = 0 ; w < nz ; w++ ) {
          
          if( progress ) {
            if( !progress->notify() ) {
              num_pairs.clear();
              return num_pairs;
            }
          }

          int tail_id = cursor.node_id( u,v,w );
          if( !head_prop_->is_informed( tail_id ) ) continue;
          if( !tail_prop_->is_informed( tail_id ) ) continue;
  
          int head_id = cursor.node_id( u+step.x(), v+step.y(), w+step.z() );
          if( !head_prop_->is_informed( head_id ) ) continue;
          if( !tail_prop_->is_informed( head_id ) ) continue;

          Correlation_measure::ValPair head_prop_pair = 
            std::make_pair( head_prop_->get_value(head_id), head_prop_->get_value(tail_id) );
          Correlation_measure::ValPair tail_prop_pair = 
            std::make_pair( tail_prop_->get_value(head_id), tail_prop_->get_value(tail_id) );
          
          correl_measure->add_pair( head_prop_pair, tail_prop_pair );
        }
      }
    }
      
    x_values.push_back( euclidean_norm( step ) );
    double correl_value = correl_measure->correlation();
    if( !GsTL::equals( correl_value, Correlation_measure::NaN, 0.0001 ) )
      y_values.push_back( correl_measure->correlation() / covar ); 
    else
      y_values.push_back( correl_measure->correlation() );

    num_pairs.push_back( correl_measure->pair_count() );

    delete correl_measure;
  }

  experim_variog.set_no_data_value( Correlation_measure::NaN );
  experim_variog.set_x_values( x_values );
  experim_variog.set_y_values( y_values );
  
  return num_pairs;
}



double Grid_variog_computer:: compute_covariance() const {
  double covar = 0;
  double head_mean=0;
  double tail_mean=0;
  int count=0;

  for( int j=0; j < head_prop_->size() ; j++ ) {

    if( !head_prop_->is_informed(j) || !tail_prop_->is_informed(j) ) continue;

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

  return covar;
}






	
	
