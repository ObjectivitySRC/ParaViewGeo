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



#ifndef __filtersim_Servo_system_sampler_H__
#define __filtersim_Servo_system_sampler_H__

#include <GsTLAppli/geostat/common.h>

#include <algorithm>
#include <functional>
#include <vector>
#include <list>

#include "pattern.h"
#include "prototype.h"


/*
 * template class Filtersim_Servosystem_Cate
 * is used to draw a pattern id from a categorical prototype
 */
template < class RandNumberGenerator >
class GEOSTAT_DECL Filtersim_Servosystem_Cate 
{
 public:
     // constructors
     template<class ForwardIterator >
         Filtersim_Servosystem_Cate( const vector<float> target,
                                     float constraint,
                                     ForwardIterator first, ForwardIterator last,
                                     const RandNumberGenerator& rand, int nb_templ );
     
     Filtersim_Servosystem_Cate( const vector<float> target,
                                 float constraint,
                                 const vector<float> initial_histogram,
                                 const RandNumberGenerator& rand, int nb_templ );
     
     // return the "best" pattern ID
     template< class Prototype > int operator()( Prototype& prototype ); 
     // retrun a random pattern ID
     template< class Prototype > int operator()( list<Prototype>& prototype ); 

     void update_current_histogram( vector<float>& histogram, int nb_patched );
     
 private:
     int nb_templ_;         // searching template size
     float nb_of_data_;    // number of nodes simulated or informed
     int nb_of_categories_;

     vector<float> target_pdf_;           // marginal distribution
     vector<float> current_histogram_;    // # of data per facies

     float mu_;
     RandNumberGenerator gen_;
}; // end of class Filtersim_Servo_system_sampler



/*
 * template class Filtersim_Servosystem_Cate
 * is used to draw a pattern id from a continuous prototype
 */
template < class RandNumberGenerator >
class GEOSTAT_DECL Filtersim_Servosystem_Cont 
{
 public:
     // constructors
     template<class ForwardIterator >
         Filtersim_Servosystem_Cont( const float target, float constraint,
                                     ForwardIterator first, ForwardIterator last,
                                     const RandNumberGenerator& rand, int nb_templ );
     
      Filtersim_Servosystem_Cont( const float target, float constraint,
                                  const float initial_mean, int nb_of_data,
                                  const RandNumberGenerator& rand, int nb_templ );
     
      // return the "best" pattern ID
     template<class Prototype > int operator()( Prototype& prototype ); 
     // retrun a random pattern ID
     template< class Prototype > int operator()( list<Prototype>& prototype ); 

     void update_current_histogram( float current_mean_value, int nb_patched );
     
 private:
     int nb_templ_;         // searching template size
     float nb_of_data_;    // number of nodes simulated or informed

     float target_mean_;           // marginal distribution
     float current_mean_;    // # of data per facies
     
     float mu_;
     RandNumberGenerator gen_;
}; // end of class Filtersim_Servo_system_sampler_cont


#include "filtersim_servo_system.cpp"

#endif  // __filtersim_Servo_system_sampler_H__
