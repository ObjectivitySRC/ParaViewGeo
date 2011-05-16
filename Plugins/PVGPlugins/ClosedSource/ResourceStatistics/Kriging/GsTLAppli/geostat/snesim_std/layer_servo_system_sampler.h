/* GsTL: the Geostatistics Template Library
 * 
 * Author: Nicolas Remy
 * Copyright (c) 2000 The Board of Trustees of the Leland Stanford Junior University
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *   1.Redistributions of source code must retain the above copyright notice, this 
 *     list of conditions and the following disclaimer. 
 *   2.Redistributions in binary form must reproduce the above copyright notice, this 
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution. 
 *   3.The name of the author may not be used to endorse or promote products derived 
 *     from this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ** Modified by Jianbing Wu, SCRF, Stanford University, June 2004
 *
 */

#ifndef __GSTL_Layer_servo_system_sampler_H__
#define __GSTL_Layer_servo_system_sampler_H__

#include <GsTL/univariate_stats/build_cdf.h>
#include <GsTL/cdf/categ_non_param_cdf.h>
#include <GsTL/cdf/non_param_cdf.h>
#include <GsTLAppli/grid/grid_model/geovalue.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>

#include <algorithm>
#include <functional>
#include <vector>

using namespace std;

// this template mimic the functions of Servo_system_sampler, then only difference is:
//		Servo_system_sampler uses the global target proportion
//		Layer_servo_system_sampler uses the local proportion for each layer

template < class RandNumberGenerator, class ComputeLayerIndex >
class Layer_servo_system_sampler {

  typedef Layer_servo_system_sampler<RandNumberGenerator, ComputeLayerIndex> Self_;

public:

	/** Constructs a servo system
	* @param target is the target cdf 
	* @param constraint allows to make the servo system more or less lenient.
	* It varies between 0 and 1. If constraint=1, the servo system ensures that
	* the target cdf is exactly reproduced.
	* @param  [first, last) is a range of geovalues.
	*/
	template<class ForwardIterator, class CategNonParamCdf>
	  Layer_servo_system_sampler( const CategNonParamCdf& target,
				      double constraint,
				      ForwardIterator first, ForwardIterator last,
				      const RandNumberGenerator& rand,
				      const ComputeLayerIndex& getLayerIndex);
	
	/** Constructs a servo system
	* @param target is the target cdf 
	* @param constraint allows to make the servo system more or less lenient.
	* It varies between 0 and 1. If constraint=1, the servo system ensures that
	* the target cdf is exactly reproduced.
	* @param initial_histogram is a vector containing the initial number of geovalue
	* in each class.  
	*/
	template< class CategNonParamCdf >
	  Layer_servo_system_sampler( const CategNonParamCdf& target,
				      double constraint,
				      const vector< vector<double> >& initial_histogram,
				      const RandNumberGenerator& rand,
				      const ComputeLayerIndex& getLayerIndex);
	
	/** Copy constructor
	*/
	Layer_servo_system_sampler( const Self_& rhs );



	/** Draws a realization from ccdf and assigns it to gval.
	* @return 0 if a value was successfully assigned to gval, 1 if
	* gval has not been changed 
	*/
	template<class GeoValue, class CategNonParamCdf>
	  int operator()(GeoValue& gval, const CategNonParamCdf& ccdf); 

    void set_grid_level( int level )
    {
        getLayerIndex_.set_grid_level( level );
    }

    /*
     * function to unsign simulated value at some locations
     */
    void removeSimulatedNode( GsTLGridProperty* prop, vector<int>& grid_path );
    void removeSimulatedNode( RGrid* grid, GsTLGridProperty* prop, vector<int>& grid_path );

private:

	int nb_of_categories_;
	vector< vector<double> > target_pdf_;
	vector< vector<double> > current_histogram_;
	vector< double> nb_of_data_;

	double mu_;

	RandNumberGenerator gen_;
	ComputeLayerIndex getLayerIndex_;
}; // end of class Servo_system

template 
<
	class CategNonParamCdf,
	class RandNumberGenerator,
	class ForwardIterator,
	class ComputeLayerIndex
>
  Layer_servo_system_sampler<RandNumberGenerator, ComputeLayerIndex>
Servo_system(  const CategNonParamCdf& target,
				double constraint,
				ForwardIterator first, ForwardIterator last,
				const RandNumberGenerator& rand,
				const ComputeLayerIndex& getLayerIndex)
{
  return 
    Layer_servo_system_sampler
    <
      RandNumberGenerator, 
      ComputeLayerIndex
    > ( target, constraint, first, last, rand, getLayerIndex);
}


#include "layer_servo_system_sampler.cpp"

#endif
