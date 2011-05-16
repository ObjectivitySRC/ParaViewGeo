/**********************************************************************
** Author: Jianbing Wu, Alexandre Boucher
**
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


#ifndef __filtersim_Tau_updating_H__
#define __filtersim_Tau_updating_H__


#include <GsTLAppli/geostat/common.h>
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

// only for categorical variable with soft data
class GEOSTAT_DECL Tau_updating
{
 public:
     Tau_updating( vector<double> target_pdf, const double tau1, const double tau2,
                   float no_data_value=-9966699 )
          : target_pdf_(target_pdf), tau1_(tau1), tau2_(tau2), UNINFORMED(no_data_value)
	 {
		 tau0_ = 1 - tau1_ -tau2_;
	 }
	 
	 inline void operator()( vector< vector<float> >& data_event, 
							 const vector< vector<float> >& soft_data_event ) 
	 {
         for (int i=0; i<soft_data_event.size(); i++)
         {
             for (int j=0; j<soft_data_event[0].size(); j++)
             {
                 if( fabs( soft_data_event[i][j] - UNINFORMED) > EPSILON )	// node is informed
                 {
                     if( GsTL::equals( soft_data_event[i][j], double(0) ) || 
                         GsTL::equals( soft_data_event[i][j], double(1) ) ) 
                     {   // if p_ac=0 or 1, then p_abc = p_ac
                         data_event[i][j] = soft_data_event[i][j];
                     }
                     else if ( !GsTL::equals( data_event[i][j], double(0) ) &&
                               !GsTL::equals( data_event[i][j], double(1) ) )
                     {
                         // x = a^tau0 * b^tau1 * c^tau2,	(a from target proportion)
                         double a=(1-target_cpdf_[j])/max(target_cpdf_[j], EPSILON);	
                         double b=(1-data_event[i][j])/max(data_event[i][j], EPSILON);
                         double c=(1-soft_data_event[i][j])/max(soft_data_event[i][j], EPSILON);
                         
                         double x=pow(a, tau0_) * pow(b, tau1_) * pow(c, tau2_);
                         data_event[i][j] = 1.0/(1+x);
                     }
                     // else if p_ab = 0 or 1; p_abc = p_ab.
                 }
             }
         }
	 }
	 
 private:
     vector<double> target_pdf_; 
     double tau0_;
     double tau1_;
     double tau2_;

     float UNINFORMED;	 
}; // end of class Tau_updating


#endif	//__filtersim_Tau_updating_H__

