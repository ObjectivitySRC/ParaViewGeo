/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "math" module of the Geostatistical Earth
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

#include <GsTLAppli/math/qpplot.h>

#include <GsTL/univariate_stats/utils.h>

#include <numeric>
#include <cmath>



//==============================================


QPplot::QPplot() {
  init();
}


void QPplot::init() {
  means_[0] = means_[1] = 0;
  variances_[0] = variances_[1] = 0;

  analysis_type_ = QQplot;
  x_vals_ = 0;
  y_vals_ = 0;

  clipped_ranges_[0].first  = x_data_.begin();
  clipped_ranges_[0].second = x_data_.end();
  clipped_ranges_[1].first  = y_data_.begin();
  clipped_ranges_[1].second = y_data_.end();
}


void QPplot::compute_univ_stats( Variable var ) {
  iterator start = clipped_ranges_[var].first;
  iterator end = clipped_ranges_[var].second;

  variances_[var] = GsTL::variance( start, end, &means_[var] );
  /*
  float data_count = float( std::distance( start, end ) );

  means_[var] = std::accumulate( start, end, 0.0 );
  means_[var] /= data_count;

  variances_[var] = std::inner_product( start, end, start, 0.0 );
  variances_[var] = variances_[var]/data_count - means_[var]*means_[var];
*/
}



void QPplot::low_clip( Variable var, float val ) {
  clipped_ranges_[var].first = 
    std::lower_bound( data_array(var)->begin(), data_array(var)->end(), val );
  compute_univ_stats( var );
}

void QPplot::high_clip( Variable var, float val ) {
  clipped_ranges_[var].second = 
    std::upper_bound( data_array(var)->begin(), data_array(var)->end(), val );
  compute_univ_stats( var );
}

 
std::pair<double*,double*> QPplot::plotting_data( int& size ) {
  if( x_data_.empty() || y_data_.empty() )
    return std::pair<double*,double*>(0,0);

  switch( analysis_type_ ) {
  case QQplot:
    return qqplot( size );
    break;
  case PPplot:
    return ppplot( size );
    break;
  default:
    return std::pair<double*,double*>(0,0);
  }
}


std::pair<double*,double*> QPplot::qqplot( int& size ) {
  size = 51;
  clear_plot_values();
  x_vals_ = new double[size];
  y_vals_ = new double[size];

  float multiplier = 1.0/float(size-1);
  for( int i = 0; i < size ; i++ ) {
    x_vals_[i] = percentile( Xvar, multiplier*float(i) );  
    y_vals_[i] = percentile( Yvar, multiplier*float(i) );  
  }

  return std::make_pair( x_vals_, y_vals_ );
}

std::pair<double*,double*> QPplot::ppplot( int& size ) {
  size = 51;
  clear_plot_values();
  x_vals_ = new double[size];
  y_vals_ = new double[size];

  if( clipped_ranges_[0].first == clipped_ranges_[0].second ||
      clipped_ranges_[1].first == clipped_ranges_[1].second ) {
    return std::make_pair( (double*) 0, (double*) 0);
  }

  float range_min = std::min( *(clipped_ranges_[0].first), *(clipped_ranges_[1].first) );
  
  float max_x = x_data_[x_data_.size()-1];
  if( clipped_ranges_[0].second != x_data_.end() )
    max_x = *(clipped_ranges_[0].second);
  float max_y = y_data_[y_data_.size()-1];
  if( clipped_ranges_[1].second != y_data_.end() )
    max_y = *(clipped_ranges_[1].second);
  float range_max = std::max( max_x, max_y );
  
  float step = (range_max - range_min)/float(size+1);

  for( int i=0; i< size ; i++ ) {
    x_vals_[i] = prob( Xvar, range_min+(i+1)*step );  
    y_vals_[i] = prob( Yvar, range_min+(i+1)*step );  
  }

  return std::make_pair( x_vals_, y_vals_ );
}


void QPplot::clear_plot_values() {
  delete [] x_vals_;
  delete [] y_vals_;
}

