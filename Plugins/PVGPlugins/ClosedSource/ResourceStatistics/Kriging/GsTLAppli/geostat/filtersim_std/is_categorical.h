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


#ifndef __filtersim_is_categorical_H__
#define __filtersim_is_categorical_H__

#include <GsTLAppli/geostat/common.h>

// c/c++
#include <iterator>  // for distance
#include <set>  // for set
#include <cmath> // for ceil and floor
#include <numeric>

#include <GsTLAppli/grid/grid_model/grid_property.h>    // for GsTLGridProperty

using namespace std;

// if the input string is composed of numbers (including +/-/.),
// then return true; otherwise return false
inline
GEOSTAT_DECL bool is_number( const std::string& str ) 
{
    for( std::string::const_iterator it = str.begin() ; it != str.end() ; ++it ) 
    {
        if( !isdigit( *it ) && *it != '.' && *it != '-' && *it != '+' ) return false;
    }

    return true;
}

// to cast a float/double value into an integer
template <typename T> 
inline 
GEOSTAT_DECL T round(const T& _value) 
{
    return ( _value >= static_cast<T>( 0 ) )
        ? static_cast<T>( std::floor( _value + 0.5 ) )
        : static_cast<T>( std::ceil( _value - 0.5 ) );
} 

// return true is input data is a categorical (integer);
// otherwise return false
template <class T>  
inline
GEOSTAT_DECL bool is_categorical(T value) 
{   
    if ( ( value < 0 ) || ( value != round( value ) ) ) 
        return false;
    else
        return true;
} // is_categorical


/*
 * function to check whether the current property is integer or not.
 * if the property is composed of integer numbers, then return true
 * and total number of categories (unique integer numbers)
 */
inline
GEOSTAT_DECL bool is_integer_prop(GsTLGridProperty* prop, int& nb_cat)
{
    set<int> categories;    // set<int> will remove the identical values

    for (int i=0; i<prop->size(); i++)
    {
        if ( prop->is_informed(i) )
        {
            if ( !is_categorical( prop->get_value(i) ) )
                return false;
            else
                categories.insert( static_cast<int>( prop->get_value(i) ) );
        }
    }

    // total number of categories
    nb_cat = categories.size();

    return true;
}

/*
 * function to check whether the current property is indicator or not.
 *      a valid indicator must be integer, and gradually increase from 0
 *      the maximun indicator must be "nb_cat-1"
 * If the property is composed of integer numbers, then return true
 * and total number of indicators
 */
inline
GEOSTAT_DECL bool is_indicator_prop(GsTLGridProperty* prop, int& nb_cat)
{
    int i, value;
    set<int> categories;    // set<int> will remove the identical values

    // check whether is integer or not
    for (i=0; i<prop->size(); i++)
    {
        if ( prop->is_informed(i) )
        {
            if ( !is_categorical( prop->get_value(i) ) )
                return false;
            else
                categories.insert( static_cast<int>(prop->get_value(i)) );
        }
    }
    
    for (i=0; i<prop->size(); i++)
    {
        if ( prop->is_informed(i) )
        {
            value = static_cast<int>( prop->get_value(i) );

            // this value should be on the value_{th} position of categories
            if ( value != std::distance(categories.begin(), categories.find(value) ) )
                return false;
        }
    }

    nb_cat = categories.size();

    return true;
}


/*
 * function to normalize a vector  into range [p_start, p_end]
 */
template<typename T>
inline
GEOSTAT_DECL void normalize_vector( vector<T>& value, T p_start=(T)(0), T p_end=(T)(1) )
{
    T max_value = *max_element(value.begin(), value.end());
    T min_value = *min_element(value.begin(), value.end());
    T denominator = (max_value-min_value);

    if ( denominator == (T)(0) )    return;
    T multiplier = (p_end-p_start)/denominator;

    for (int i=0; i<value.size(); i++)
        value[i] = p_start + ( value[i] - min_value )*multiplier;
}


/*
 * function to standardized input vector weight such that sum(weight)=1
 */
template<typename T>
inline
GEOSTAT_DECL void standardize_weight( vector<T>& value )
{
    T denominator = std::accumulate(value.begin(), value.end(), (T)(0));

    if ( denominator == (T)(0) )   return;

    for (int i=0; i<value.size(); i++)
        value[i] = value[i] / denominator;
}


#endif // __filtersim_is_categorical_H__
