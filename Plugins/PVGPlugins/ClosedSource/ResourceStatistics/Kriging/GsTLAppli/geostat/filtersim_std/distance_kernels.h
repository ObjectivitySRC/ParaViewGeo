/**********************************************************************
** Author: Jianbing Wu, Alexandre Boucher
** Contributor: Tuanfeng Zhang
**
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Stanford Geostatistical 
** Earth Modeling Software (SGEMS)
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


#ifndef __distance_kernels_H__
#define __distance_kernels_H__

#include <GsTLAppli/geostat/common.h>
#include <vector>
#include <algorithm>

/*
 * class radial_kernel
 * to calculate distance between two scores
 * the distance function is exponential of the square different
 */
class GEOSTAT_DECL radial_kernel 
{
public :
	radial_kernel() 
	{
		sigma_ = 1.0;
		a_ = 1;
	}
	~radial_kernel(){};

	template< class Scores > float operator() (Scores score1, Scores score2 )
    {
		float sq_dist = 0.0;

		for(int i = 0; i < score1.size(); ++i ) 
			sq_dist += std::pow((score1[i] - score2[i]),2);

		return std::exp(-sq_dist/sigma_);
	}

private :
	float sigma_;
	float a_;
};


// The followings are not used in current version

/*
 * class poly_kernel
 * to calculate distance between two scores
 * the distance function is polynominal of other d_
 */
class GEOSTAT_DECL poly_kernel 
{
public :
	poly_kernel() 
	{
		a_ = 1.0;
		b_ = 1.0;
		d_ = 2;
	}
	~poly_kernel();

	poly_kernel(int d, float a=1, float b=1 ): a_(a),b_(b), d_(d){};

	template< class Scores > float operator() (Scores score1, Scores score2 )
    {
		float inner_prod  = std::inner_product(score1.begin(),score1.end(),score2.begin(),0.0);
		return std::pow(a_*inner_prod + b_, d_);
	}

private :
	float a_;
	float b_;
	int d_;
};


/*
 * class sigmoid_kernel
 * to calculate distance between two scores
 * the distance function is hyperbolic tangent
 */
class GEOSTAT_DECL sigmoid_kernel 
{
public :
	sigmoid_kernel() 
	{
		a_ = 1.0; 
		b_=2;
	}

	~sigmoid_kernel();

	sigmoid_kernel(float a, float b ): a_(a), b_(b){};

	template< class Scores > float operator() (Scores score1, Scores score2 )
    {
		float inner_prod  = std::inner_product(score1.begin(),score1.end(),score2.begin(),0.0);
		return std::tanh(a_*inner_prod + b_);
	}

private :
	float a_;
	float b_;
};


/*
 * template function to calculate the square difference
 */
template < class type >
GEOSTAT_DECL type square_diff( type x, type y )
{
	return std::pow((x - y),2);
}

#endif

