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


#ifndef __filtersim_Tau_updating_H__
#define __filtersim_Tau_updating_H__


#include <GsTL/math/math_functions.h>

#include <vector>

using namespace std;

/** A Tau_updating is an adaptor that models concept Cdf Estimator.
* Its aim is to update the probability P(A|B) computed by some cdf estimator 
* into the joint probability P(A|B,C), given the prior knowledge P(A|C).
* For example, P(A|B) could be the probability of observing channel given some
* well data as computed by a search tree, and P(A|C) the probability of 
* observing channel given some seismic information, eg obtained by classification.
* P(A|B,C) is the probability of observing a channel given both the
* well data and the seismic signature.
*
* The two probabilities P(A|B) and P(A|C) are combined using the permanence of 
* ratios formula proposed by Journel (2002), which amounts to a conditional
* independance hypothesis (hence the name: Conditional Independance Bayes 
* updating).
*
* A Tau_updating requires the knowledge of probabilities 
* P(A|C) for every category (i.e. every possible outcome of A). Actually, if A
* has k possible outcomes, only (k-1) probabilities are needed, the probability
* corresponding to category k is deducted by complement: 
*    P(A=k|B,C) = sum[i=1 to k-1] P(A=i|B,C) 
*/


/*
 * class Filtersim_Tau_updating
 * is used to update the probability using Tau model, 
 * it is only used for categorical variable with soft data
 */
class Filtersim_Tau_updating
{
 public:
     // constructor
     Filtersim_Tau_updating( const vector<float> target_cpdf, const float tau1, 
                             const float tau2, float no_data_value=-9966699 )
          : target_cpdf_(target_cpdf), tau1_(tau1), tau2_(tau2), UNINFORMED(no_data_value)
	 {
		 tau0_ = 1 - tau1_ -tau2_;

         // save the common data into a_ (from target proportion)
         a_.resize( target_cpdf_.size(), 0.0 );
         for (int i=0; i<a_.size(); i++)
             a_[i]=pow( (1-target_cpdf_[i])/max(target_cpdf_[i], EPSILON), tau0_ );	
	 }

	 // operator function, to update the data event according to the soft DEV
	 inline void operator()( vector< vector<float> >& data_event, 
							 const vector< vector<float> >& soft_data_event ) 
	 {
         for (int i=0; i<data_event.size(); i++)    // nb_facies
         {
             for (int j=0; j<data_event[0].size(); j++)     // nb_template
             {
                 if ( !GsTL::equals( data_event[i][j], UNINFORMED ) ) // node is informed
                 {
                     if( GsTL::equals( soft_data_event[i][j], float(0) ) || 
                         GsTL::equals( soft_data_event[i][j], float(1) ) ) 
                     {   // if p_ac=0 or 1, then p_abc = p_ac
                         data_event[i][j] = soft_data_event[i][j];
                     }
                     else if ( !GsTL::equals( data_event[i][j], float(0) ) &&
                               !GsTL::equals( data_event[i][j], float(1) ) )
                     {
                         // x = a^tau0 * b^tau1 * c^tau2,	(a from target proportion)
                         float b=(1-data_event[i][j])/max(data_event[i][j], EPSILON);
                         float c=(1-soft_data_event[i][j])/max(soft_data_event[i][j], EPSILON);
                         
                         float x = a_[i] * pow(b, tau1_) * pow(c, tau2_);
                         data_event[i][j] = 1.0/(1+x);

                         if (data_event[i][j]>1.0)    
                             data_event[i][j] = 1.0;

                         if (data_event[i][j]<0.0)    
                             data_event[i][j] = 0.0;
                     }
                     // else if p_ab = 0 or 1; p_abc = p_ab.
                 }
             }
         }
	 }
	 
 private:
     vector<float> target_cpdf_; 
     float tau0_;
     vector<float> a_;

     float tau1_;
     float tau2_;

     float UNINFORMED;	 
}; // end of class Tau_updating


#endif	//__filtersim_Tau_updating_H__

