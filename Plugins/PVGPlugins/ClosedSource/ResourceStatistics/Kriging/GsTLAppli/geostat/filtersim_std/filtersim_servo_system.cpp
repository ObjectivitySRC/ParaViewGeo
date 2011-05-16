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



#include "filtersim_servo_system.h"


//
//-- servosystem for the categorical data
//

/*
 * instructor function
 * initialize from target proportion, constraint, Geovalue iterators
 */
template < class RandNumberGenerator >
template < class ForwardIterator >
Filtersim_Servosystem_Cate< RandNumberGenerator >::
Filtersim_Servosystem_Cate( const vector<float> target,
                            float constraint,
                            ForwardIterator first, ForwardIterator last,
                            const RandNumberGenerator& rand, int nb_templ) 
            : target_pdf_( target ), current_histogram_( target.size(), 0 ), 
              gen_(rand), nb_templ_(nb_templ)
{
    
    // constraint can vary from 0 to 1
    mu_ = constraint / ( 1.00001 - constraint);
    
    nb_of_categories_ = target.size();
    nb_of_data_ = 0;
    
    // Compute the pdf of range [first, last)
    for( ; first != last ; ++first) 
    {
        if( ! first->is_informed() )    continue;
        
        current_histogram_[ int( first->property_value() ) ] ++;
        nb_of_data_ ++;
    }
}


/*
 * instructor function
 * initialize from target proportion, constraint, initial histogram
 */
template < class RandNumberGenerator >
Filtersim_Servosystem_Cate< RandNumberGenerator >::
Filtersim_Servosystem_Cate( const vector<float> target,
                            float constraint,
                            const vector<float> initial_histogram,
                            const RandNumberGenerator& rand, int nb_templ ) 
            : target_pdf_( target ), current_histogram_( initial_histogram ), 
              gen_(rand), nb_templ_(nb_templ)
{
    // constraint can vary from 0 to 1
    mu_ = constraint / ( 1.00001 - constraint);
    
    nb_of_categories_ = target.size();
    nb_of_data_ = std::accumulate( initial_histogram.begin(), initial_histogram.end(), 0 );
}


/*
 * operator function
 * return a pattern id drawn from a prototype
 */
template < class RandNumberGenerator >
template < class Prototype >
int 
Filtersim_Servosystem_Cate< RandNumberGenerator >::
operator () ( Prototype& prototype ) 
{
    // randomly draw a pattern id from the prototype
	int pattern_id = floor( gen_()* (prototype.get_replicates()-1) );
	return (prototype.get_score()).operator[](pattern_id).first;
}

/* // there is no more servo-system
template < class RandNumberGenerator >
template < class Prototype >
int 
Filtersim_Servosystem_Cate< RandNumberGenerator >::
operator () ( Prototype& prototype ) 
{
    float lambda;
    float omega;
    vector<float> v1;
	vector<float> v2;

    // total number of replicates of current prototype
    int nb_replicates = prototype.get_replicates();

    // facies proportions for each replicates
    vector< vector<float> > each_pattern_prop = prototype.each_pattern_prop();
    int nb_facies = each_pattern_prop[0].size();

    // traning image only has two facies
    // refer to Tuanfeng's thesis for the detailed algorithm
    if( nb_facies == 2 ) 
    {
        lambda = target_pdf_[1]/std::max(EPSILON, current_histogram_[1]/nb_of_data_);

        for(int repl = 0; repl < nb_replicates; repl++) 
            v1.push_back( each_pattern_prop[repl][1] );
    }
    else    // more than two facies
    {
        for(int repl = 0; repl < nb_replicates; repl++) 
        { 
            vector<float> temp_vec(nb_facies,0.0);
            for(int i=0; i<nb_facies; i++)
            {
                temp_vec[i] = fabs( ( nb_templ_*each_pattern_prop[repl][i] +
                                      current_histogram_[i] )/(nb_of_data_ + nb_templ_) 
                                    - target_pdf_[i] );
            }
            
            // use the maximum absolute value as indicator
            v1.push_back( *max_element(temp_vec.begin(), temp_vec.end()) );
        }

        float a = *max_element( v1.begin(), v1.end() );
        float b = *min_element( v1.begin(), v1.end() );
        lambda = b/std::max(EPSILON, a);
    }

    v2=v1;
	sort( v2.begin(), v2.end(), less<float>() );
    omega = pow(lambda, mu_);

    // randomly draw a pattern according to current proportion
	int k = floor( pow( gen_(), 1.0/omega )* (nb_replicates-1) );

    // find the location in vector v1
	int repl =0 ;
    for( ; repl < nb_replicates; repl++)
	{
		if( fabs(v2[k] - v1[repl])<EPSILON ) break;
	}

    return prototype.get_score().operator[]( repl ).first;
}
*/



/*
 * operator function
 * return a pattern id drawn randonly from a prototype list
 */
template < class RandNumberGenerator >
template < class Prototype >
int 
Filtersim_Servosystem_Cate< RandNumberGenerator >::
operator () ( list<Prototype>& prototype ) 
{
    // randomly draw a prototype id from a prototype list
	int prototype_id = floor( gen_()* (prototype.size()-1) );

    // move to the selected prototype
    typename list<Prototype>::iterator itr = prototype.begin();
    for (int i=0; i<prototype_id; i++)
        itr++;

    // randomly draw a pattern id from the prototype
	int pattern_id = floor( gen_()* (itr->get_replicates()-1) );

	return (itr->get_score()).operator[](pattern_id).first;
}



/*
 * function "update_current_histogram"
 * update the facies proportion according to the patched data
 */
template < class RandNumberGenerator >
void 
Filtersim_Servosystem_Cate< RandNumberGenerator >::
update_current_histogram( vector<float>& histogram, int nb_patched )
{
    nb_of_data_ += nb_patched;

    for (int i=0; i<histogram.size(); i++)
        current_histogram_[i] += histogram[i];
}


//
//-- servosystem for the continuous data
//

/*
 * instructor function
 * initialize from target proportion, constraint, Geovalue iterators
 */
template < class RandNumberGenerator >
template < class ForwardIterator >
Filtersim_Servosystem_Cont< RandNumberGenerator >::
Filtersim_Servosystem_Cont( const float target, float constraint,
                            ForwardIterator first, ForwardIterator last,
                            const RandNumberGenerator& rand, int nb_templ )  
            : target_mean_( target ), current_mean_( 0. ), gen_(rand), nb_templ_(nb_templ) 
{
    // constraint can vary from 0 to 1
    mu_ = constraint / ( 1.00001 - constraint);
    
    nb_of_data_ = 0;
    
    // Compute the pdf of range [first, last)
    for( ; first != last ; ++first) 
    {
        if( ! first->is_informed() )    continue;
        
        current_mean_ += first->property_value();
        nb_of_data_ ++;
    }
}


/*
 * instructor function
 * initialize from target proportion, constraint, initial mean value
 */
template < class RandNumberGenerator >
Filtersim_Servosystem_Cont< RandNumberGenerator >::
Filtersim_Servosystem_Cont( const float target, float constraint,
                            float initial_mean, int nb_of_data,
                            const RandNumberGenerator& rand, int nb_templ ) 
            : target_mean_( target ), current_mean_( initial_mean ), 
              gen_(rand), nb_of_data_(nb_of_data), nb_templ_(nb_templ)
{
    // constraint can vary from 0 to 1
    mu_ = constraint / ( 1.00001 - constraint);
}


/*
 * operator function
 * return a pattern id drawn from a prototype
 */
template < class RandNumberGenerator >
template < class Prototype >
int 
Filtersim_Servosystem_Cont< RandNumberGenerator >::
operator () ( Prototype& prototype ) 
{
    // randomly draw a pattern id from the prototype
	int pattern_id = floor( gen_()* (prototype.get_replicates()-1) );
	return (prototype.get_score()).operator[](pattern_id).first;
}

/* // there is no more servo-system
template < class RandNumberGenerator >
template < class Prototype >
int 
Filtersim_Servosystem_Cont< RandNumberGenerator >::
operator () ( Prototype& prototype ) 
{
    vector<float> v1;
	vector<float> v2;

    // total number of replicates
    int nb_replicates = prototype.get_replicates();

    // mean value for each replicate
    vector<float> each_pattern_mean = prototype.each_pattern_prop();
    int repl = 0;
    for( ; repl < nb_replicates; repl++) 
    { 
        v1.push_back( each_pattern_mean[repl] );
    }

    v2=v1;
	sort( v2.begin(), v2.end(), less<float>() );

    float lambda = target_mean_/std::max(EPSILON, current_mean_/nb_of_data_);
    float omega = pow(lambda, mu_);

    // randomly draw a pattern according current mean value
	int k = floor( pow( gen_(), 1.0/omega )* (nb_replicates-1) );

    // find the correct location in vector v1
	
    for( repl = 0; repl < nb_replicates; repl++)
	{
		if( fabs(v2[k] - v1[repl])<EPSILON ) break;
	}

	return prototype.get_score().operator[]( repl ).first;
}
*/

/*
 * operator function
 * return a pattern id drawn from a prototype list
 */
template < class RandNumberGenerator >
template < class Prototype >
int 
Filtersim_Servosystem_Cont< RandNumberGenerator >::
operator () ( list<Prototype>& prototype ) 
{
    // randomly draw a prototype id from a prototype list
	int prototype_id = floor( gen_()* (prototype.size()-1) );

    // move to the selected prototype
    typename list<Prototype>::iterator itr = prototype.begin();
    for (int i=0; i<prototype_id; i++)
        itr++;

    // randomly draw a pattern id from the prototype
	int pattern_id = floor( gen_()* (itr->get_replicates()-1) );

	return (itr->get_score()).operator[](pattern_id).first;
}


/*
 * function "update_current_histogram"
 * update the mean value according to the patched data
 */
template < class RandNumberGenerator >
void 
Filtersim_Servosystem_Cont< RandNumberGenerator >::
update_current_histogram( float current_mean_value, int nb_patched )
{
    nb_of_data_ += nb_patched;
    current_mean_ += current_mean_value;
}

