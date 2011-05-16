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

#include <GsTLAppli/math/histogram.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <GsTL/univariate_stats/utils.h>

#include <numeric>
#include <iterator>
#include <cmath>


Histogram::Histogram() {
  init();
  frequencies_ = true;
  logscale_ = false;
}

Histogram::~Histogram() {
  clear_plot_values();
}

 
void Histogram::init() {
  if( data_.empty() ) data_.push_back( 0.0 );

  start_ = data_.begin();
  end_ = data_.end();
  low_clip_ = data_[0];
  high_clip_ = data_[ data_.size() - 1 ];

  x_vals_ = 0;
  y_vals_ = 0;
  bins_count_ = 0;
  recompute_ = true;
}

void Histogram::compute_stats() {

  var_ = GsTL::variance( start_, end_, &mean_ );
  /*
  float data_count = static_cast<float>( std::distance( start_, end_ ) );
  mean_ =
    std::accumulate( start_, end_, 0.0 ) / data_count ;
  float sqsum = std::inner_product( start_, end_, 
                                    start_, 0.0 );
  var_ = sqsum/data_count - mean_*mean_;
  */
}


void Histogram::low_clip( float val ) {
  low_clip_ = val;
  recompute_ = true;
  if( val > high_clip_ )
    start_ = end_;
  else
    start_ = std::lower_bound( data_.begin(), data_.end(), low_clip_ );
  compute_stats();
  bins( bins_count_ );
}

void Histogram::high_clip( float val ) {
  high_clip_ = val;
  recompute_ = true;
  if( val < low_clip_ ) 
    end_ = start_;
  else
    end_ = std::upper_bound( data_.begin(), data_.end(), high_clip_ );
  compute_stats();
  bins( bins_count_ );
}

void Histogram::clipping_values( std::pair<float,float> vals ) {
  recompute_ = true;

  if( vals.second < vals.first )
    vals.second = vals.first;

  low_clip_ = vals.first;
  high_clip_ = vals.second;
  start_ = std::lower_bound( data_.begin(), data_.end(), low_clip_ );
  end_ = std::upper_bound( data_.begin(), data_.end(), high_clip_ );
  compute_stats();
  bins( bins_count_ );
}

/*
void Histogram::bins( int bins ) {
  if( data_.empty() ) return;

//  float step = ( *(end_-1) - *start_ ) / float(bins-1);
  float step = ( *(end_-1) - *start_ ) / float(bins);
  float start_value = (*start_) + step;
  histogram_.resize( bins );

  std::vector<float>::iterator start_pos = start_;
  for( int i=0 ; i < histogram_.size(); i++ ) {
    std::vector<float>::iterator pos = 
      std::lower_bound( start_pos, end_, start_value );  
    histogram_[i] = std::distance( start_pos, pos );

    start_pos = pos;
    start_value += step;
  }
}
*/


float Histogram::percentile( float p ) const { 
  float data_count = static_cast<float>( std::distance( start_, end_ ) );
  return *(start_ + int( (data_count-1.0)*p ) ); 
}


void Histogram::set_use_logscale( bool on ) {
  if( on == logscale_ ) return;

  logscale_ = on;
  recompute_ = true;
  bins( bins_count_ );
}

void Histogram::set_use_frequencies( bool on ) {
  if( on == frequencies_ || bins_count_ <= 0 ) return;

  frequencies_ = on;
  if( on )
    std::transform( y_vals_, y_vals_+bins_count_+1, y_vals_,
                    std::bind2nd( std::divides<double>(), double( data_count() ) ) );
  else
    std::transform( y_vals_, y_vals_+bins_count_+1, y_vals_,
                    std::bind2nd( std::multiplies<double>(),
                                  double( data_count() ) ) 
                       );
}


void Histogram::clear_plot_values() {
  delete [] x_vals_;
  delete [] y_vals_;
}

std::pair<double*,double*> Histogram::plotting_data() {
  return std::make_pair( x_vals_, y_vals_ );
}

void Histogram::bins( int n ) {
  if( n <= 0 ) return;
  if( n == bins_count_ && !recompute_ ) return;

  recompute_ = false;
  clear_plot_values();

  bins_count_ = n;

  x_vals_ = new double[n+1];
  y_vals_ = new double[n+1];

  if( start_ == end_ ) {
    for( int j=0; j < n+1; j++ ) {
      x_vals_[j] = 0;
      y_vals_[j] = 0;
    }
    return;
  }

  // compute the sizes of the bins
  if( logscale_ ) 
    compute_log_bin_sizes( x_vals_, n );
  else
    compute_linear_bin_sizes( x_vals_, n );

  // the previous functions only computed n elements
  x_vals_[n] = *(end_-1);

  std::vector<float>::iterator start_pos = start_;
  for( int i = 1; i <= n; i++ ) {
    std::vector<float>::iterator pos = 
      std::upper_bound( start_pos, end_, x_vals_[i] );  
    y_vals_[i-1] = std::distance( start_pos, pos );
    start_pos = pos;
  }

  y_vals_[n] = y_vals_[n-1];

  if( frequencies_ ) {
    for( int i = 0; i <= n; i++ ) 
      y_vals_[i] /= float( data_count() );
  }

}


void Histogram::compute_log_bin_sizes( double* result, int size ) {
  // ignore all data lesser than 0
  const_iterator actual_start = std::upper_bound( start_, end_, 0.0 ); 
  appli_assert( *actual_start > 0 );

  float logmin = std::log10( *actual_start );
  float logmax = std::log10( *(end_-1) );
  float step = (logmax-logmin) / float( size );

  for( int i=0; i < size; i++ ) {
    result[i] = std::pow( float(10.0), logmin + float(i)*step );
  }
}


void Histogram::compute_linear_bin_sizes( double* result, int size ) {
  float step = ( *(end_-1) - *start_ ) / float(size);

  for( int i=0; i < size; i++ ) {
    result[i] = *start_ + float(i)*step ;
  }
}


int Histogram::rawDataSize()
{
	iterator itr;
	int num = 0;
	for (itr = start_; itr != end_; ++itr,++num);
	return num;
}

std::map<float,int> & Histogram::frequency()
{	
	iterator itr;
	_frequency.clear();

	for (itr = start_; itr != end_; ++itr)
		_frequency[*itr]++;

	return _frequency;
}
