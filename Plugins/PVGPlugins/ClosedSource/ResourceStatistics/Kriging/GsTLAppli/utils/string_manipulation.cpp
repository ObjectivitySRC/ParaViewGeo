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

#include <GsTLAppli/utils/string_manipulation.h>

#include <ctype.h>
#include <locale.h>


namespace String_Op {

  string_pair split_string(const std::string& str, 
			   const std::string& separator, bool unique ) {
    string_pair result;
    split_string( result, str, separator,  unique );
    return result;
  }



  void split_string( string_pair& result, 
		     const std::string& str, const std::string& separator,
		     bool unique ) {

    std::string::size_type pos = str.find( separator, 0 );
    result.first = str.substr(0,pos);

    if( pos < std::string::npos ) { 
      std::string::size_type skip = separator.size();
      
      if( !unique ) {
	std::string::size_type  separ_size = separator.size();
	std::string::size_type cur_pos = pos+separ_size;
#if defined(__GNUC__) && __GNUC__ < 3
	while( str.compare( separator, cur_pos, separ_size ) == 0 ) {
#else
	while( str.compare(cur_pos, separ_size, separator ) == 0 ) {
#endif
	  cur_pos += separ_size;
	  skip += separ_size;
	  if( cur_pos >= std::string::npos ) break;
	}
      }
      

      result.second = str.substr( pos + skip );
    }
    else {
      result.second = "";
    } 
  }




  std::vector< std::string > decompose_string( const std::string& str, 
					       const std::string& separator,
					       bool unique ) {
    
    std::vector< std::string > vec;
    decompose_string( vec, str, separator, unique );
    return vec;
  }




  void decompose_string(  std::vector< std::string >& vec,
			  const std::string& str, 
			  const std::string& separator,
			  bool unique ) {
    
    vec.clear();
    string_pair split = split_string( str, separator, unique );
    vec.push_back( split.first );

    while( !split.second.empty() ) {
      split_string( split, split.second, separator, unique );
      vec.push_back( split.first );
    }
    
  }



  bool contains( const std::string& str, const std::string& substr, 
		 bool cs ) {
    if( cs ) {
      return ( str.find( substr ) != std::string::npos );
    } 
    else {
      std::string str_copy( str );
      std::string substr_copy( substr );
      
      // turn str and substr into their lower case equivalents
      for( unsigned int i=0; i < str_copy.size(); i++ )
	str_copy[i] = tolower( str_copy[i] );

      for( unsigned int j=0; j < substr_copy.size(); j++ )
	substr_copy[j] = tolower( substr_copy[j] );

      return ( str_copy.find( substr_copy ) != std::string::npos );
    }
  }


  void replace( std::string& str, 
                const std::string& old, const std::string& new_str ) {
    std::string::size_type pos = 0;
    while( ( pos = str.find( old, pos ) ) != std::string::npos ) {
      str.replace( pos, old.size(), new_str );
      pos += new_str.size();
    }
  }


  bool is_number( const std::string& str ) {
    for( std::string::const_iterator it = str.begin() ; it != str.end() ; ++it ) {
      if( !isdigit( *it ) && *it != '.' ) return false;
    }
//    for( int i=0; i < std::string::npos; i++ ) {
//      if( !isdigit( str[i] ) ) return false;
//    }
    return true;
  }


  std::string simplify_white_space( std::string str ) 
  { 
      char const* separator = " \t\r\n";

      std::string::size_type pos = str.find_first_not_of(separator);
      str.erase(0,pos);

      pos = str.find_last_not_of(separator); 
      str.erase(pos+1);

      pos = 0;
      while( ( pos = str.find( "  ", pos ) ) != std::string::npos ) 
          str.replace( pos, 2, " " );

      return str;
  } 

} // namespace String_Op
