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

#ifndef __GSTLAPPLI_MATH_DIRECTION_3D_H__
#define __GSTLAPPLI_MATH_DIRECTION_3D_H__

#include <GsTLAppli/math/gstlvector.h>

/** All angles must be expressed in radian
*/
class MATH_DECL Direction_3d {
public:
  Direction_3d();

  void set_direction( const GsTLVector<float>& v );
  void set_direction( float alpha, float beta );

  void set_tolerance( float angle_tol, float cone_height );
  bool use_tolerance() const { return use_tolerance_; }
  void use_tolerance( bool ok ) { use_tolerance_ = ok; }

  bool is_colinear( const GsTLVector<float>& v ) const;

private:
  GsTLVector<float> dir_;
  bool use_tolerance_;
  float angle_tol_, cone_height_, bandwidth_sq_;
};

#endif

