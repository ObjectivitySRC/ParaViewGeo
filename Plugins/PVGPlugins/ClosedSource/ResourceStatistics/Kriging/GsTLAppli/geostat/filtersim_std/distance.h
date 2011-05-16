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


#ifndef __filtersim_distance_definition__
#define __filtersim_distance_definition__

#include <GsTLAppli/geostat/common.h>
#include <GsTL/math/math_functions.h>

#include <iterator>

// return the square value 
template <typename T>
inline T
sqr(const T& value) 
{
    return value * value;
}   // sqr


// return the absolute value
template <typename T>
inline T
abs(const T& value) 
{
    return ( value < 0 ) ? ( -1 * value ) : value;
}   // abs


/*
 * template class Manhattan_Distance
 * calculate the Manhattan distance between two vector of same length
 * if the vector contains UNINFORMED data, then it is ignored.
 */
template 
< 
    class InputIterator1, 
    class InputIterator2=InputIterator1, 
    class WeightIterator=InputIterator1
>
class GEOSTAT_DECL Manhattan_Distance 
{
public:
    typedef typename std::iterator_traits<InputIterator1>::value_type result_type;

public:
    Manhattan_Distance( result_type no_data_value=-9966699): UNINFORMED(no_data_value) {};

    // overloaded function, retun the distance between two vectors
    result_type operator() ( InputIterator1 p1_begin, InputIterator1 p1_end, 
                             InputIterator2 p2_begin)
	{
        result_type dist=(result_type)(0);

        InputIterator1 itr1=p1_begin;
        InputIterator2 itr2=p2_begin;

		for (; itr1!=p1_end; itr1++, itr2++)
        {   
            // only count the informed nodes
            if ( !GsTL::equals( *itr1, UNINFORMED ) && !GsTL::equals( *itr2, UNINFORMED ) )
                dist += abs( *itr1 - *itr2 );
        }

        return dist;
	}

    // overloaded function, retun the distance between two vectors
    result_type operator() ( InputIterator1 p1_begin, InputIterator1 p1_end, 
                             InputIterator2 p2_begin, WeightIterator weight_begin)
	{
        result_type dist=(result_type)(0);

        InputIterator1 itr1 = p1_begin;
        InputIterator2 itr2 = p2_begin;
        WeightIterator wtr = weight_begin;

		for (; itr1!=p1_end; itr1++, itr2++, wtr++)
        {
            // only count the informed nodes
            if ( !GsTL::equals( *itr1, UNINFORMED ) && !GsTL::equals( *itr2, UNINFORMED ) )
                dist += abs( ( *itr1 - *itr2 )*(*wtr) );
        }

        return dist;
	}
    

    // overloaded function, retun the distance between two vectors
    bool operator() ( InputIterator1 p1_begin, InputIterator1 p1_end, 
                      InputIterator2 p2_begin, result_type& shortest_dist)
	{
        result_type dist=(result_type)(0);

        InputIterator1 itr1=p1_begin;
        InputIterator2 itr2=p2_begin;

		for (; itr1!=p1_end; itr1++, itr2++)
        {
            // only count the informed nodes
            if ( !GsTL::equals( *itr1, UNINFORMED ) && !GsTL::equals( *itr2, UNINFORMED ) )
            {
                dist += abs( *itr1 - *itr2 );
                if(dist>shortest_dist)  return false;
            }
        }

        shortest_dist = dist;
        return true;
	}

    // overloaded function, retun the distance between two vectors,
    // the calculation will be terminated if current distance is larger
    // than the given shortest distance
    bool operator() ( InputIterator1 p1_begin, InputIterator1 p1_end, 
                      InputIterator2 p2_begin, WeightIterator weight_begin,
                      result_type& shortest_dist )
	{
        result_type dist=(result_type)(0);

        InputIterator1 itr1 = p1_begin;
        InputIterator2 itr2 = p2_begin;
        WeightIterator wtr = weight_begin;

		for (; itr1!=p1_end; itr1++, itr2++, wtr++)
        {
            // only count the informed nodes
            if ( !GsTL::equals( *itr1, UNINFORMED ) && !GsTL::equals( *itr2, UNINFORMED ) )
            {
                dist += abs( ( *itr1 - *itr2 )*(*wtr) );
                if(dist>shortest_dist)  return false;
            }
        }

        shortest_dist = dist;
        return true;
	}

private:
    result_type UNINFORMED;
};


/*
 * template class Euclidean_Distance
 * calculate the Euclidean distance between two vector of same length
 * if the vector contains UNINFORMED data, then it is ignored.
 */
template 
< 
    class InputIterator1, 
    class InputIterator2=InputIterator1, 
    class WeightIterator=InputIterator1
>
class GEOSTAT_DECL Euclidean_Distance 
{
public:
    typedef typename std::iterator_traits<InputIterator1>::value_type result_type;

public:
	Euclidean_Distance( result_type no_data_value=-9966699): UNINFORMED(no_data_value) {};

    // overloaded function, retun the distance between two vectors
    result_type operator() ( InputIterator1 p1_begin, InputIterator1 p1_end, 
                             InputIterator2 p2_begin)
	{
        result_type dist=(result_type)(0);

        InputIterator1 itr1=p1_begin;
        InputIterator2 itr2=p2_begin;

		for (; itr1!=p1_end; itr1++, itr2++)
        {
            // only count the informed nodes
            if ( !GsTL::equals( *itr1, UNINFORMED ) && !GsTL::equals( *itr2, UNINFORMED ) )
                dist += sqr( *itr1 - *itr2 );
        }

        return std::sqrt( dist );
	}

    // overloaded function, retun the distance between two vectors
    result_type operator() ( InputIterator1 p1_begin, InputIterator1 p1_end, 
                             InputIterator2 p2_begin, WeightIterator weight_begin)
	{
        result_type dist=(result_type)(0);

        InputIterator1 itr1 = p1_begin;
        InputIterator2 itr2 = p2_begin;
        WeightIterator wtr = weight_begin;

		for (; itr1!=p1_end; itr1++, itr2++, wtr++)
        {
            // only count the informed nodes
            if ( !GsTL::equals( *itr1, UNINFORMED ) && !GsTL::equals( *itr2, UNINFORMED ) )
                dist += sqr( ( *itr1 - *itr2 )*(*wtr) );
        }

        return std::sqrt( dist );
    }

    // overloaded function, retun the distance between two vectors
    bool operator() ( InputIterator1 p1_begin, InputIterator1 p1_end, 
                      InputIterator2 p2_begin, result_type& shortest_dist)
	{
        result_type dist=(result_type)(0);
        result_type opt_dist = sqrt( shortest_dist );

        InputIterator1 itr1=p1_begin;
        InputIterator2 itr2=p2_begin;

		for (; itr1!=p1_end; itr1++, itr2++)
        {
            // only count the informed nodes
            if ( !GsTL::equals( *itr1, UNINFORMED ) && !GsTL::equals( *itr2, UNINFORMED ) )
            {
                dist += sqr( *itr1 - *itr2 );
                if(dist>opt_dist)  return false;
            }
        }

        shortest_dist = dist;
        return true;
	}

    // overloaded function, retun the distance between two vectors,
    // the calculation will be terminated if current distance is larger
    // than the given shortest distance
    bool operator() ( InputIterator1 p1_begin, InputIterator1 p1_end, 
                      InputIterator2 p2_begin, WeightIterator weight_begin,
                      result_type& shortest_dist)
	{
        result_type dist=(result_type)(0);
        result_type opt_dist = sqrt( shortest_dist );

        InputIterator1 itr1 = p1_begin;
        InputIterator2 itr2 = p2_begin;
        WeightIterator wtr = weight_begin;

		for (; itr1!=p1_end; itr1++, itr2++, wtr++)
        {
            // only count the informed nodes
            if ( !GsTL::equals( *itr1, UNINFORMED ) && !GsTL::equals( *itr2, UNINFORMED ) )
            {
                dist += sqr( ( *itr1 - *itr2 )*(*wtr) );
                if(dist>opt_dist)  return false;
            }
        }

        shortest_dist = dist;
        return true;
	}
    
private:
    result_type UNINFORMED;
};


#endif  // __filtersim_distance_definition__
