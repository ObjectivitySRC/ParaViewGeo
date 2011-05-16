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

#ifndef __GSTLAPPLI_MATH_SCATTERPLOT_H__
#define __GSTLAPPLI_MATH_SCATTERPLOT_H__

#include <GsTLAppli/math/common.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <vector>
#include <algorithm>


class MATH_DECL Scatter_plot {
  typedef unsigned int size_t;
  typedef std::vector<float>::const_iterator const_iterator;
  typedef std::vector<float>::iterator iterator;

public:

  enum Variable{ Xvar=0, Yvar=1 };

  Scatter_plot();
  virtual ~Scatter_plot();

  template< class InputIterator >
  void set_data( Variable var, 
            InputIterator first, InputIterator last, float no_data_value ) {
    std::vector<float>* array = data_array( var );
    array->clear();
//    std::copy( first, last, std::back_inserter( *array ) );
    for( ; first != last ; ++first )
      array->push_back( *first );
    no_data_value_[var] = no_data_value;

    data_set_modified_ = true;

    std::pair<float,float> min_max = find_min_max( var );
    mins_[var] = min_max.first;
    maxs_[var] = min_max.second;
    low_clips_[var] = min_max.first;
    high_clips_[var] = min_max.second;
    compute_univ_stats( var );
    compute_correlations();
  }

  virtual std::pair<double*,double*> plotting_data( int& size );
  int max_pairs() const { return max_pairs_; }
  void max_pairs( int n ) { max_pairs_ = n; }

  void low_clip( Variable var, float val );
  float low_clip( Variable var ) const { return low_clips_[var]; }
  void high_clip( Variable var, float val );
  float high_clip( Variable var ) const { return high_clips_[var]; }
  
  int data_count( Variable var );
  float mean( Variable var ) const { return means_[var]; }
  float var( Variable v ) const { return variances_[v]; }
  
  float correlation() const { return correl_; }
  float rank_correlation() const { return rank_correl_; }

  std::pair<float,float> least_sq_fit() const;

  float min( Variable var ) { return mins_[var]; }
  float max( Variable var ) { return maxs_[var]; }


protected:
  void init();
  std::vector<float>* data_array( Variable var );
  std::pair<float,float> find_min_max( Variable var );
  
  void compute_univ_stats( Variable var );
  void compute_correlations();
  void clear_plot_values();
  bool is_valid_data_pair( unsigned int i ) const ;
  bool is_valid( Variable var, float val ) const ;

protected:

  bool data_set_modified_;

  std::vector<float> x_data_;
  std::vector<float> y_data_;

  float low_clips_[2];
  float high_clips_[2];

  float no_data_value_[2];

  float means_[2];
  float variances_[2];
  float mins_[2];
  float maxs_[2];

  double correl_;
  float rank_correl_;
  mutable std::pair<float,float> ls_fit_;

  double* x_vals_;
  double* y_vals_;
  int max_pairs_;
};




//===================================================

inline std::vector<float>* Scatter_plot::data_array( Variable var ) {
  if( var == Xvar )
    return &x_data_;
  if( var == Yvar )
    return &y_data_;

  return 0;
}


inline bool Scatter_plot::is_valid_data_pair( unsigned int i ) const {
  return x_data_[i] != no_data_value_[0] && y_data_[i] != no_data_value_[1] &&
         x_data_[i] >= low_clips_[0] && x_data_[i] <= high_clips_[0] &&
         y_data_[i] >= low_clips_[1] && y_data_[i] <= high_clips_[1] ;
}


inline bool Scatter_plot::is_valid( Variable var, float val) const {
  return val != no_data_value_[var] && 
         val >= low_clips_[var] && val <= high_clips_[var];
}
#endif

