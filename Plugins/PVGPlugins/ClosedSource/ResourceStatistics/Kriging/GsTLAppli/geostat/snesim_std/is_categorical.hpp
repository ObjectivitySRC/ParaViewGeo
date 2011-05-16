/**********************************************************************
** Author: Jianbing Wu
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
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


#ifndef __GSTLAPPLI_is_categorical_H__
#define __GSTLAPPLI_is_categorical_H__

// c/c++
#include <iterator>  // for distance
#include <set>  // for set
#include <cmath> // for ceil and floor

// if the input string is composed of numbers (including +/-/.),
// then return true; otherwise return false
bool is_number( const std::string& str ) 
{
    for( std::string::const_iterator it = str.begin() ; it != str.end() ; ++it ) 
    {
        if( !isdigit( *it ) && *it != '.' && *it != '-' && *it != '+' ) return false;
    }

    return true;
}


#include <GsTLAppli/grid/grid_model/grid_property.h>    // for GsTLGridProperty

template <typename T> inline T round(const T& _value) 
{
    return ( _value >= static_cast<T>( 0 ) )
        ? static_cast<T>( std::floor( _value + 0.5 ) )
        : static_cast<T>( std::ceil( _value - 0.5 ) );
} 

// --- is_categorical ---------------------------------------------------------
template <class T>  inline bool is_categorical(T value) 
{   
    if ( ( value < 0 ) || ( value != round( value ) ) ) 
        return false;
    else
        return true;
} // is_categorical


// --- check whether the current property is integer ----------------------------
bool is_integer_prop(GsTLGridProperty* prop, int& nb_cat)
{
    set<int> categories;

    for (int i=0; i<prop->size(); i++)
    {
        if ( prop->is_informed(i) )
        {
            if ( !is_categorical( prop->get_value(i) ) )
                return false;
            else
                categories.insert( prop->get_value(i) );
        }
    }

    // total number of categories
    nb_cat = categories.size();

    return true;
}

// --- check whether the current property is indicator  -------------------------
// a valid indicator must be integer, and gradually increase from 0
// the maximun indicator must be "nb_cat-1"
bool is_indicator_prop(GsTLGridProperty* prop, int& nb_cat)
{

    int i, value;
    set<int> categories;

    for (i=0; i<prop->size(); i++)
    {
        if ( prop->is_informed(i) )
        {
            if ( !is_categorical( prop->get_value(i) ) )
                return false;
            else
                categories.insert( prop->get_value(i) );
        }
    }
    
    for (i=0; i<prop->size(); i++)
    {
        if ( prop->is_informed(i) )
        {
            value = prop->get_value(i);
            if ( value != std::distance(categories.begin(), categories.find(value) ) )
                return false;
        }
    }

    nb_cat = categories.size();

    return true;
}


#endif // __GSTLAPPLI_is_categorical_H__
