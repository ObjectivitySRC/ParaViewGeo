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

#include <GsTLAppli/math/scatterplot.h>

#include <GsTL/univariate_stats/utils.h>
#include <GsTL/math/random_number_generators.h>

#include <numeric>
#include <cmath>


class Dataset_iterator {
  typedef std::vector<float>::iterator iterator;

public:
  Dataset_iterator( iterator it, iterator end, 
                    float low_clip=0, float high_clip=0, 
                    float no_data_value = -99999 );
  Dataset_iterator( iterator it );
  Dataset_iterator& operator ++ ();
  Dataset_iterator& operator ++ ( int );
  float operator * () { return *it_ ; }  
  bool operator == ( const Dataset_iterator& rhs ) const { return it_ == rhs.it_; }
  bool operator != ( const Dataset_iterator& rhs ) const { return it_ != rhs.it_; }

private:
  bool is_valid() const;

private:
  iterator it_, end_;
  float low_clip_, high_clip_, no_data_value_;
};

Dataset_iterator::Dataset_iterator( iterator it, iterator end, 
                                    float low_clip, float high_clip, 
                                    float no_data_value ) {
  it_ = it;
  end_ = end;
  low_clip_ = low_clip;
  high_clip_ = high_clip;
  no_data_value_ = no_data_value;

  while( !is_valid() ) {
    ++it_;
    if( it_ == end_ ) break;
  }
}

Dataset_iterator::Dataset_iterator( iterator it ) {
  it_ = it;
  end_ = it_;
  low_clip_ = 0;
  high_clip_ = 0;
  no_data_value_ = 0;  
}

Dataset_iterator& Dataset_iterator::operator ++ () {
  do {
    ++it_;
    if( it_ == end_ ) break;
  } while( !is_valid() );
  return *this;
}

Dataset_iterator& Dataset_iterator::operator ++ ( int ) {
  do {
    ++it_;
    if( it_ == end_ ) break;
  } while( !is_valid() );  
  return *this;
}

bool Dataset_iterator::is_valid() const {
  float val = *it_;
  return val >= low_clip_ && val <= high_clip_ && val != no_data_value_;
}



//========================================

Scatter_plot::Scatter_plot() {
  init();
}


Scatter_plot::~Scatter_plot() {
  clear_plot_values();
}

void Scatter_plot::clear_plot_values() {
  delete [] x_vals_;
  delete [] y_vals_;
  x_vals_ = 0;
  y_vals_ = 0;
}


void Scatter_plot::init() {
  low_clips_[0] = low_clips_[1] = 0;
  high_clips_[0] = high_clips_[1] = 0;

  no_data_value_[0] = no_data_value_[1] = -9999;
  means_[0] = means_[1] = variances_[0] = variances_[1] = 0;
  mins_[0] = mins_[1] = maxs_[0] = maxs_[1] = 0;

  correl_ = rank_correl_ = -2;

  x_vals_ = 0;
  y_vals_ = 0;
  max_pairs_ = 1000000;
}


std::pair<double*,double*> Scatter_plot::plotting_data( int& size ) {
  if( !data_set_modified_ ) 
    return std::make_pair( x_vals_, y_vals_ );

  if( x_data_.size() != y_data_.size() )
    return std::pair<double*,double*>( 0,0 );

  size = std::min( max_pairs_, int(x_data_.size()) );
  clear_plot_values();
  x_vals_ = new double[size];
  y_vals_ = new double[size]; 

  
  int count = 0;

  // if size < x_data_.size(), we need to randomly select "size" data pairs
  if( size < x_data_.size() ) {
    int* ids = new int[x_data_.size()];
    for( int i=0; i < x_data_.size() ; i++ ) ids[i] = i;
    
    // could use a local random number generator to control the seed
    Rand48_generator gen( 14071789 );
    STL_generator_adaptor_t<Rand48_generator> stl_gen(gen);
    std::random_shuffle( ids, ids+x_data_.size(), stl_gen );

    int id = 0;
    while( count < size ) {
      if( is_valid_data_pair( ids[id] ) ) {
        x_vals_[count] = x_data_[ ids[id] ];
        y_vals_[count] = y_data_[ ids[id] ];
        count++;
      }
      if( ++id >= x_data_.size() ) break;
    }   
    
    delete [] ids;
  }
  else {
    for( int j=0; j < size ; j++ ) {
      if( is_valid_data_pair( j ) ) {
        x_vals_[count] = x_data_[j];
        y_vals_[count] = y_data_[j];
        count++;
      }
    }
  }

  size = count;
  return std::make_pair( x_vals_, y_vals_ );
}


std::pair<float,float> Scatter_plot::least_sq_fit() const {
  if( x_data_.size() != y_data_.size() )
    return std::make_pair( 0.0, 0.0 );

  if( !data_set_modified_ )
    return ls_fit_;

  float sq_x_sum = 0;
  float y_sum = 0;
  float x_sum = 0;
  float xy_sum = 0;
  int count = 0;

  for( unsigned int i = 0; i < x_data_.size(); i++ ) {
    if( is_valid_data_pair( i ) ) {
      x_sum += x_data_[i];
      y_sum += y_data_[i];
      xy_sum += x_data_[i] * y_data_[i];
      sq_x_sum += x_data_[i] * x_data_[i];
      count++;
    }
  }

  ls_fit_.second = 
    (x_sum*xy_sum - y_sum*sq_x_sum) / (x_sum*x_sum - float(count)*sq_x_sum);
  ls_fit_.first = 
    (xy_sum - x_sum*ls_fit_.second) / sq_x_sum;

  return ls_fit_;
}


void Scatter_plot::low_clip( Variable var, float val ) {
  if( val == low_clips_[var] ) return;
  if( ( val < mins_[var] ) && ( low_clips_[var] < mins_[var] ) ) {
    low_clips_[var] = val;
    return;
  }

  low_clips_[var] = val;
  data_set_modified_ = true;
  compute_univ_stats( var );
  compute_correlations();
}


void Scatter_plot::high_clip( Variable var, float val ) {
  if( val == high_clips_[var] ) return;
  if( ( val > maxs_[var] ) && ( high_clips_[var] > maxs_[var] ) ) {
    high_clips_[var] = val;
    return;
  }
  high_clips_[var] = val;

  data_set_modified_ = true;
  compute_univ_stats( var );
  compute_correlations();
}


int Scatter_plot::data_count( Variable var ) {
//  if( x_data_.size() != y_data_.size() ) return 0;
  std::vector<float>* vec = data_array( var );
  if( vec->empty() ) return 0;

  int id = 0;
  int count = 0;
  while( id < int( vec->size() ) ) {
    if( is_valid( var, (*vec)[id++] ) )
      count ++;
  }
  return count;
}


void Scatter_plot::compute_univ_stats( Variable var ) {
  std::vector<float>* vec = data_array( var );
  Dataset_iterator begin( vec->begin(), vec->end(),
                          low_clips_[var], high_clips_[var],
                          no_data_value_[var] );
  Dataset_iterator end( vec->end() );

  variances_[var] = GsTL::variance( begin, end, &means_[var] );
}


void Scatter_plot::compute_correlations() {
  if( x_data_.size() != y_data_.size() ) {
    correl_ = -99;
    rank_correl_ = -99;
    return;
  }

  double mean_x = 0;
  double mean_y = 0;
  correl_ = 0;

  int count = 0;
  for( unsigned int i = 0; i < x_data_.size(); i++ ) {
    if( is_valid_data_pair( i ) ) {
      correl_ += x_data_[i]*y_data_[i];
      mean_x += x_data_[i];
      mean_y += y_data_[i];

      count ++;
    }
  }

  mean_x /= double(count);
  mean_y /= double(count);
  correl_ /= double( count - 1 );
  double correction_factor = double(count)/double(count-1);
  correl_ -= correction_factor * mean_x * mean_y;
  correl_ /= std::sqrt( variances_[0]*variances_[1] );

  /*
  mean_x /= float( count );
  mean_y /= float( count );
  correl_ /= float( count );
  correl_ -= mean_x * mean_y;
  correl_ /= std::sqrt( variances_[0]*variances_[1] );
  */
}
  

std::pair<float,float> Scatter_plot::find_min_max( Variable var ) {
  std::vector<float>* vec = data_array( var );
  
  // go to the first valid value in vec:
  unsigned int i = 0;
  while( (*vec)[i] == no_data_value_[var] ) { 
    if( i++ >= vec->size() ) return std::make_pair( 0.0, 0.0 );   
  }


  std::pair< float, float > min_max( (*vec)[i], (*vec)[i] );

  for( ; i < vec->size() ; i++ ) {
    float val = (*vec)[i];
    if( (*vec)[i] != no_data_value_[var] ) {
      min_max.first = std::min( min_max.first, val );
      min_max.second = std::max( min_max.second, val );
    }
  }
  return min_max;
}
