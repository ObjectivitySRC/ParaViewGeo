/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "utils" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_STRING_MANIPULATION_H__ 
#define __GSTLAPPLI_STRING_MANIPULATION_H__ 
 
#include <GsTLAppli/utils/common.h>

#include <string> 
#include <utility> 
#include <vector> 
#include <sstream> 
#include <iterator> 
#include <algorithm> 
 
 
/** The String_Op namespace contains various common string operations, such
 * as splittig strings, converting strings to numbers or searching substrings
 * in a string.
 */ 
 
namespace String_Op { 
 
 
 
  //======================================== 
  //   Splitting 
 
 
  typedef std::pair< std::string, std::string > string_pair; 
  typedef std::vector< std::string > string_vector; 
  /** Splits a string in 2 where a given separator is first encountered 
   * When "unique" is false, repeated contiguous separators count as  
   * one. Ex: if the separator is "/", string "part1///part2" is decomposed 
   * into "part1" and "part2" if unique is set to false. 
   * If set to true, the string would be decomposed into "part1" and 
   * "//part2". 
   */ 
  UTILS_DECL string_pair split_string( const std::string& str,  
			    const std::string& separator, bool unique = true ); 
 
  /** This overloaded function puts the result into the provided 
   * string_pair instead of returning a new one. 
   */ 
  UTILS_DECL void split_string( string_pair& result,  
		     const std::string& str, const std::string& separator, 
		     bool unique = true );  
 
  /** Decomposes a string into all its components. 
   * Ex: if the separator is "/", string "part1/part2//part3" would 
   * be decomposed into: 
   *   - if unique=TRUE: "part1", "part2", "", "part3" 
   *   - if unique=FALSE: "part1", "part2", "part3" 
   */ 
  UTILS_DECL std::vector< std::string > decompose_string( const std::string& str,  
					       const std::string& separator, 
					       bool unique = true); 
 
  /** This overloaded function puts the result into the provided 
   * vector of strings instead of returning a new one. 
   */ 
  UTILS_DECL void decompose_string( std::vector< std::string >& vec, 
			                              const std::string& str,  
			                              const std::string& separator, 
			                              bool unique = true ); 
 
 
 
 
  //======================================== 
  //   Conversions 
   
  /** checks whether string \a str is the string representation of
  * a number.
  */
  UTILS_DECL bool is_number( const std::string& str ) ;

    /** Removes leading and trailing whitespace
   */ 
  UTILS_DECL std::string simplify_white_space( std::string str );

  /** Converts a string into a number 
   */ 
  template <class T> 
  T to_number( const std::string& str ){ 
    if( str.empty() ) return static_cast<T>( 0 );

    std::istringstream sin( str ); 
    T t; 
    sin >> t ; 
    return t; 
  } 
 
  /** Converts a number into a string 
   */ 
  template<class T> 
  std::string to_string( const T& val ) { 
    std::ostringstream so; 
    so << val; 
    return so.str(); 
  } 
 
  /** Converts a string of numbers into a vector of numbers 
   * The numbers can be separated by spaces, tabs, or newlines. If the 
   * separator is different, use the \c split_string or \c decompose_string
   * functions, then apply \c to_number to each sub-string.
   */ 
  template <class T> 
  std::vector<T> to_numbers( const std::string& str ){ 
    std::istringstream sin( str ); 
    std::vector<T> vec; 
    std::copy( std::istream_iterator<T>( sin ), std::istream_iterator<T>(), 
	       std::back_inserter( vec ) ); 
    return vec; 
  } 
 
  //======================================== 
  //   Search 
   
  /** This functions returns true if string \a str contains sub-string \a substr. 
  * If \a cs is true, the search is case sensitive.
  */
  UTILS_DECL bool contains( const std::string& str, const std::string& substr,  
		 bool cs = true );  
 
  /** This function modifies string \a str by replacing sub-string \a old
  * with string \a new_str. If \a old is not a sub-string of \a str, nothing
  * is done.
  */
  UTILS_DECL void replace( std::string& str, 
                           const std::string& old, const std::string& new_str );
 


} // namespace String_Op 
 
#endif 
