/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "math" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_MATH_GSTLVECTOR_H__ 
#define __GSTLAPPLI_MATH_GSTLVECTOR_H__ 
 
#include <GsTLAppli/math/common.h>
#include <GsTLAppli/utils/gstl_types.h> 
 
#include <iostream> 
 
 
/** A GsTLVector represents a 3D (euclidean) vector. The difference
* between GsTLTripletTmpl and GsTLVector is mostly semantic:
* GsTLTripletTmpl is a point, GsTLVector a vector, i.e. the result
* of the substraction of two points. Example: if A and B are two
* GsTLTripletTmpl, vector AB is B-A. 
*/
template <class R> 
class GsTLVector { 
public: 
    typedef R coordinate_type; 
    static const int dimension = 3; 
public: 
    GsTLVector() { 
        coords_[0] = 0; 
        coords_[1] = 0; 
        coords_[2] = 0; 
    } 
    GsTLVector( 
        R x, R y, R z 
    )  { 
        coords_[0] = x; 
        coords_[1] = y; 
        coords_[2] = z; 
    } 
 
    GsTLVector( 
        const GsTLVector<R>& gp 
    )  { 
        coords_[0] = gp.x(); 
        coords_[1] = gp.y(); 
        coords_[2] = gp.z(); 
    } 
     
    // Conversion constructor 
    template<class T> 
    GsTLVector( 
        const GsTLVector<T>& gp 
    )  { 
        coords_[0] = static_cast<R>( gp.x() ); 
        coords_[1] = static_cast<R>( gp.y() ); 
        coords_[2] = static_cast<R>( gp.z() ); 
    } 
 
    ~GsTLVector() {} 
 
 
 
    R& x() { return coords_[0];} 
    R& y() { return coords_[1];} 
    R& z() { return coords_[2];} 
 
    const R& x() const { return coords_[0];} 
    const R& y() const { return coords_[1];} 
    const R& z() const { return coords_[2];} 
 
 
    // GsTL Requirements : 
 
    bool operator == (const GsTLVector<R>& v ) const { 
        if ( (coords_[0] == v[0]) && (coords_[1] == v[1]) && (coords_[2] == v[2]) )  { 
             return true; 
        } 
        return false; 
    } 
 
    GsTLVector<R>& operator = (const GsTLVector<R>& v ) { 
        if (this != &v) { 
            coords_[0] = v.coords_[0]; 
            coords_[1] = v.coords_[1]; 
            coords_[2] = v.coords_[2]; 
        } 
        return *this; 
    } 
 
    const R& operator [] (GsTLInt i) const { 
        return coords_[i]; 
    } 
     
    R& operator [] (GsTLInt i) { 
        return coords_[i]; 
    } 
 
 
protected: 
    R coords_[3]; 
}; 
 
 
 
template <class R> 
inline GsTLVector<R> operator * (R coeff, const GsTLVector<R>& v)  { 
    return GsTLVector<R>( coeff * v.x(),
                          coeff * v.y(),
                          coeff * v.z() ); 
} 
 
template <class R> 
inline GsTLVector<R> operator * ( const GsTLVector<R>& v, R coeff )  { 
    return GsTLVector<R>( coeff * v.x(),
                          coeff * v.y(),
                          coeff * v.z() ); 
}


// Dot product 
template <class R> 
inline GsTLCoord operator * (const GsTLVector<R>& v0, const GsTLVector<R>& v1) { 
    return (v0.x()*v1.x() + v0.y()*v1.y() + v0.z()*v1.z()); 
} 
 
template <class R> 
inline GsTLVector<R> operator + (const GsTLVector<R>& v0, const GsTLVector<R>& v1) { 
    return GsTLVector<R>(v0.x()+v1.x(), v0.y()+v1.y(), v0.z()+v1.z()); 
} 
 
template <class R> 
inline GsTLVector<R> operator - (const GsTLVector<R>& v0, const GsTLVector<R>& v1) { 
    return GsTLVector<R>(v0.x()-v1.x(), v0.y()-v1.y(), v0.z()-v1.z()); 
} 
 
template <class R> 
inline GsTLVector<R> operator ^ (const GsTLVector<R>& v0, const GsTLVector<R>& v1)  { 
    R xx = v0.y()*v1.z() - v0.z()*v1.y(); 
    R yy = v0.z()*v1.x() - v0.x()*v1.z(); 
    R zz = v0.x()*v1.y() - v0.y()*v1.x(); 
    return GsTLVector<R>(xx, yy,  zz); 
} 
 
 
typedef GsTLVector<GsTLCoord> GsTLCoordVector; 
 
 
template<class R> 
std::ostream& operator << (std::ostream& os, const GsTLVector<R>& vec ) { 
  os << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")"; 
  return os; 
} 
 
#endif 
