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

#ifndef __GSTL_APPLI_UTILS_GSTL_DADA_ARRAY_H__ 
#define __GSTL_APPLI_UTILS_GSTL_DADA_ARRAY_H__ 
 
#ifdef __GNUC__ 
#pragma interface 
#endif 
 
/* The classes in this file are not used anymore. 
 * This file will be delete eventually. 
 */ 
 
#include <GsTLAppli/utils/common.h>
#include <GsTLAppli/utils/gstl_types.h> 
 
#include <iostream> 
#include <typeinfo> 
#include <iterator> 
 
 
#define GSTLDATA_ARRAY_INIT(builtin_type) \ 
builtin_type##_ptr_ = 0; \ 
if( typeid(builtin_type) == var_type ) { \ 
    int msize = n*sizeof(builtin_type) ; \ 
    buffer_ = new char [msize]; \ 
    builtin_type##_ptr_ = (builtin_type*)buffer_; \ 
    set_value_func_ = &GsTLDataArray::set_##builtin_type##_value; \ 
    get_value_func_ = &GsTLDataArray::get_##builtin_type##_value; \ 
    return; \ 
} \ 
 
#define GSTLDATA_ARRAY_DECLARE(builtin_type) \ 
    builtin_type * builtin_type##_ptr_;\ 
    void set_##builtin_type##_value( R val, int i ); \ 
    R get_##builtin_type##_value(int i);\ 
 
 
 
 
#define GSTLDATA_ARRAY_IMPLEMENT(builtin_type) \ 
    template <class R> \ 
    void GsTLDataArray<R>::set_##builtin_type##_value ( R val, int i ) { \ 
        builtin_type##_ptr_[i] = static_cast<builtin_type>(val); \ 
    } \ 
    template <class R> \ 
    R GsTLDataArray<R>::get_##builtin_type##_value(int i) {\ 
        return (static_cast<R>(builtin_type##_ptr_[i]));\ 
    }\ 
 
 
 
template < class R > 
class DataArrayIterator; 
 
 
template <class R> 
class UTILS_DECL GsTLDataArray { 
public: 
 
    typedef DataArrayIterator<R> const_iterator; 
     
    GsTLDataArray(int n, const type_info& var_type) { 
        buffer_ = 0; 
        size_ = n; 
        GSTLDATA_ARRAY_INIT(GsTLChar) 
        GSTLDATA_ARRAY_INIT(GsTLUChar) 
        GSTLDATA_ARRAY_INIT(GsTLBool) 
        GSTLDATA_ARRAY_INIT(GsTLShort) 
        GSTLDATA_ARRAY_INIT(GsTLInt) 
        GSTLDATA_ARRAY_INIT(GsTLLong) 
        GSTLDATA_ARRAY_INIT(GsTLUShort) 
        GSTLDATA_ARRAY_INIT(GsTLUInt) 
        GSTLDATA_ARRAY_INIT(GsTLULong) 
        GSTLDATA_ARRAY_INIT(GsTLFloat) 
        GSTLDATA_ARRAY_INIT(GsTLDouble) 
    } 
     
    ~GsTLDataArray() { 
        delete [] buffer_; 
    } 
     
    void set_value( R  val, int i ) { 
        (this->*set_value_func_)(val, i); 
    } 
     
    R  get_value(int i) { 
        return ((this->*get_value_func_)(i)); 
    } 
 
    int size() const { 
        return size_; 
    } 
 
    const_iterator begin(); 
    const_iterator end(); 
     
     
protected: 
 
    GSTLDATA_ARRAY_DECLARE(GsTLChar) 
    GSTLDATA_ARRAY_DECLARE(GsTLUChar) 
    GSTLDATA_ARRAY_DECLARE(GsTLBool) 
    GSTLDATA_ARRAY_DECLARE(GsTLShort) 
    GSTLDATA_ARRAY_DECLARE(GsTLInt) 
    GSTLDATA_ARRAY_DECLARE(GsTLLong) 
    GSTLDATA_ARRAY_DECLARE(GsTLUShort) 
    GSTLDATA_ARRAY_DECLARE(GsTLUInt) 
    GSTLDATA_ARRAY_DECLARE(GsTLULong) 
    GSTLDATA_ARRAY_DECLARE(GsTLFloat) 
    GSTLDATA_ARRAY_DECLARE(GsTLDouble) 
private: 
    void (GsTLDataArray::*set_value_func_)(R, int); 
    R    (GsTLDataArray::*get_value_func_)(int); 
    char* buffer_; 
    int size_; 
}; 
 
 
template < class R > 
class UTILS_DECL DataArrayIterator { 
   
  // STL requirements 
 public: 
  typedef std::random_access_iterator_tag iterator_category; 
  typedef R                               value_type; 
  typedef int                             difference_type; 
  typedef R*                              pointer; 
  typedef R&                              reference; 
 
 
 public: 
  DataArrayIterator( GsTLDataArray<R>* array, GsTLInt pos ); 
  DataArrayIterator( const DataArrayIterator<R>& rhs ); 
  DataArrayIterator<R>& operator = ( const DataArrayIterator<R>& rhs ); 
   
  R operator * () { return array_->get_value( pos_ ); } 
  const R* operator -> () { return &array_->get_value( pos_ ); } 
  DataArrayIterator<R>& operator ++ () { pos_ ++ ; return *this ; } 
  DataArrayIterator<R>& operator ++ ( int ) { pos_ ++ ; return *this ; } 
  DataArrayIterator<R>& operator -- () { pos_ -- ; return *this ; } 
  DataArrayIterator<R>& operator -- ( int ) { pos_ -- ; return *this ; } 
  DataArrayIterator<R> operator + ( int i ) { return DataArrayIterator<R>(array_, pos_+i) ; } 
  DataArrayIterator<R> operator - ( int i ) { return DataArrayIterator<R>(array_, pos_-i) ; } 
  DataArrayIterator<R>& operator += ( int i ) { pos_ += i; return *this ; } 
  DataArrayIterator<R>& operator -= ( int i ) { pos_ += i; return *this ; } 
 
  bool operator < ( const DataArrayIterator<R>& rhs ) { return pos_ < rhs.pos_; } 
  bool operator == ( const DataArrayIterator<R>& rhs ); 
  bool operator != ( const DataArrayIterator<R>& rhs ); 
  int operator - ( const DataArrayIterator<R>& rhs ) { return pos_ - rhs.pos_; } 
 
 private: 
  GsTLDataArray<R>* array_; 
  GsTLInt pos_; 
}; 
 
 
 
#if defined(__GNUC__) || defined(WIN32) 
#include "gstl_data_array.hpp" 
#elif defined(__xlC__) 
#pragma implementation("gstl_data_array.hpp") 
#endif 
 
#endif // __GSTL_APPLI_UTILS_GSTL_DADA_ARRAY_H__ 
 
