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

#ifndef __GSTLAPPLI_MATH_QPPLOT_H__
#define __GSTLAPPLI_MATH_QPPLOT_H__

#include <GsTLAppli/math/common.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <vector>
#include <algorithm>



class MATH_DECL QPplot {
  typedef unsigned int size_t;
  typedef std::vector<float>::const_iterator const_iterator;
  typedef std::vector<float>::iterator iterator;

public:

  enum Variable{ Xvar=0, Yvar=1 };
  enum AnalysisType{ QQplot=0, PPplot };

  QPplot();
  virtual ~QPplot() {};

  template< class InputIterator >
  void set_x_data( InputIterator first, InputIterator last ) {
    set_data( x_data_, first, last );
    clipped_ranges_[0].first  = x_data_.begin();
    clipped_ranges_[0].second = x_data_.end();
    compute_univ_stats( Xvar );
  }

  template< class InputIterator >
  void set_y_data( InputIterator first, InputIterator last ) {
    set_data( y_data_, first, last );
    clipped_ranges_[1].first  = y_data_.begin();
    clipped_ranges_[1].second = y_data_.end();
    compute_univ_stats( Yvar );
  }

  void analysis_type( AnalysisType type ) { analysis_type_ = type; }
  AnalysisType analysis_type() { return analysis_type_; }


  virtual std::pair<double*,double*> plotting_data( int& size );

  void low_clip( Variable var, float val );
  float low_clip( Variable var ) const { return *(clipped_ranges_[var].first); }
  void high_clip( Variable var, float val );
  float high_clip( Variable var ) const { return *(clipped_ranges_[var].second-1); }
  
  int data_count( Variable var );
  float mean( Variable var ) const { return means_[var]; }
  float var( Variable v ) const { return variances_[v]; }

  float min( Variable var );
  float max( Variable var );
  float percentile( Variable var, float p );
  float prob( Variable var, float val );

protected:
  void init();
  std::vector<float>* data_array( Variable var );

  template< class InputIterator >
  void set_data( std::vector<float>& target, 
                 InputIterator first, InputIterator last ) {
    target.clear();
    std::copy( first, last, std::back_inserter( target ) );
    std::sort( target.begin(), target.end() );
  }

  void compute_univ_stats( Variable var );
  void clear_plot_values();
  std::pair<double*,double*> qqplot( int& size );
  std::pair<double*,double*> ppplot( int& size );


protected:

  AnalysisType analysis_type_;

  std::vector<float> x_data_;
  std::vector<float> y_data_;

  std::pair<iterator,iterator> clipped_ranges_[2];

  float means_[2];
  float variances_[2];

  double* x_vals_;
  double* y_vals_;
};




//===================================================

inline float QPplot::min( Variable var ) {
  return (*data_array(var))[0];
} 

inline float QPplot::max( Variable var ) {
  appli_assert( !data_array(var)->empty() );
  return *( data_array(var)->end() - 1 );
} 


inline std::vector<float>* QPplot::data_array( Variable var ) {
  if( var == Xvar )
    return &x_data_;
  if( var == Yvar )
    return &y_data_;

  return 0;
}


inline int QPplot::data_count( Variable var ) {
  return std::distance( clipped_ranges_[var].first, clipped_ranges_[var].second );
}


inline float QPplot::percentile( Variable var, float p ) {
  return *(clipped_ranges_[var].first + int( (data_count(var)-1.0)*p ) ); 
}

inline float QPplot::prob( Variable var, float val ) {
  iterator it = 
    std::lower_bound( clipped_ranges_[var].first, clipped_ranges_[var].second,
                      val );
  return float(std::distance( clipped_ranges_[var].first, it )) / 
          float(data_count( var ));
}


#endif

