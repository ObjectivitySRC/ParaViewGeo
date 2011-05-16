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

#ifndef __GSTLAPPLI_MATH_BOX_H__ 
#define __GSTLAPPLI_MATH_BOX_H__ 
 
#include <GsTLAppli/math/common.h>
#include <GsTLAppli/math/gstlpoint.h> 
 
class MATH_DECL GsTL_box { 
 public: 
  typedef GsTLPoint location_type; 
   
 public: 
  virtual bool contains( const GsTLPoint& P ) const = 0; 
  virtual ~GsTL_box() {} 
}; 
 
 
 
class MATH_DECL GsTL_cube : public GsTL_box { 
 public: 
  GsTL_cube( const GsTLPoint& lower, const GsTLPoint& upper ); 
  GsTL_cube( const GsTL_cube& rhs ); 
  GsTL_cube& operator = ( const GsTL_cube& rhs ); 
  virtual ~GsTL_cube() {} 
 
  void set_geometry( const GsTLPoint& lower, const GsTLPoint& upper ); 
  void geometry( GsTLPoint& lower, GsTLPoint& upper ); 
   
  virtual bool contains( const GsTLPoint& P ) const; 
 
 protected: 
  GsTLPoint lower_; 
  GsTLPoint upper_; 
}; 
 
#endif 
