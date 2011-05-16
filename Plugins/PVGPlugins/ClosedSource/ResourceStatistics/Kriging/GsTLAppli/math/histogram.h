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

#ifndef __GSTLAPPLI_MATH_HISTOGRAM_H__
#define __GSTLAPPLI_MATH_HISTOGRAM_H__

#include <GsTLAppli/math/common.h>

#include <vector>
#include <algorithm>
#include <map>


/** The Histogram class provides facilities to compute some univariate
* statistics (mean, variance, percentiles, ...) of a range of values.
* A new range of values can be assigned using set_data(InputIterator, InputIterator).
* The range is then copied by the histogram.
* Histogram can bin the data into any number of bins with bins(int). The number of 
* data in each bin can then be accessed by histogram(), or histogram_frequencies(),
* the latter function returns the relative number of data in each bin.
* The data range can be clipped with low_clip(float) (values lesser than the 
* low-clip are ignored) and high_clip(float) (values above the high-clip are
* ignored).
*/

class MATH_DECL Histogram {
public:
  Histogram();
  ~Histogram();

  // TL modified
  int rawDataSize(); 
  std::vector<float> & rawData();

  template<class InputIterator>
  Histogram( InputIterator begin, InputIterator end ) {
    set_data( begin, end );
  }

  /** Returns true if a range of values has been attached to the histogram,
  * using set_data(InputIterator, InputIterator).
  */
  bool has_attached_data() const { return data_set_flag_; }

  /** Attaches ranges [begin,end) to Histogram. The range is copied
  * by Histogram.
  */
  template<class InputIterator>
  void set_data( InputIterator begin, InputIterator end ) {
    data_.clear();
    std::copy( begin, end, std::back_inserter( data_ ) );
    data_set_flag_ = !data_.empty();
    std::sort( data_.begin(), data_.end() );

    init();
    compute_stats();
    recompute_ = true;
  }

  /** Bins the data into \c n bins
  */
//  virtual void bins( int n );

  /** Returns the number of bins currently used.
  */
  int bins() const { return bins_count_; }

  void low_clip( float val );
  float low_clip() const { return low_clip_; }
  void high_clip( float val );
  float high_clip() const { return high_clip_; }
  void clipping_values( std::pair<float,float> vals );
  std::pair<float,float> clipping_values() const { 
    return std::make_pair( low_clip_, high_clip_ ); 
  }

  float min() const { return data_[0]; }
  float max() const { return data_[ data_.size()-1 ]; }

  /** compute the p-th percentile, p in [0,1]
  */
  float percentile( float p ) const;

  float mean() const { return mean_; }
  float var() const { return var_; }

  unsigned int data_count() const { return std::distance( start_, end_ ); }

  void set_use_logscale( bool on );
  void set_use_frequencies( bool on );
  void bins( int n );
  std::pair<double*,double*> plotting_data();

  std::map<float,int> & frequency();

protected:
  virtual void init();  
  virtual void compute_stats();
  void clear_plot_values();
  void compute_log_bin_sizes( double* result, int size );
  void compute_linear_bin_sizes( double* result, int size );


protected:
  typedef std::vector<float>::iterator iterator;
  typedef std::vector<float>::const_iterator const_iterator;

  std::vector<float> data_;
  bool data_set_flag_;

  float low_clip_, high_clip_;
  iterator start_, end_;

  float mean_;
  float var_;

  double* x_vals_;
  double* y_vals_;
  bool frequencies_, logscale_;
  bool recompute_;
  int bins_count_;

  //TL modified
  std::map<float, int> _frequency;

};

#endif

