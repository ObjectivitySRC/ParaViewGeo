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

#ifndef __GSTLAPPLI_MATH_ANGLE_CONVENTION_H__
#define __GSTLAPPLI_MATH_ANGLE_CONVENTION_H__

#include <GsTL/math/math_functions.h>

/** Convert azimuth,dip,rake to the mathematical standard angles
* The input angles are over-written by the converted angles.
* The standard is:
*   - rotation about Z
*   - rotation about Y'
*   - rotation about X''
* All angles are measured counter-clockwise
* Input and output angles are in degrees
*/
template <class T_>
void 
convert_to_math_standard_angles_degree( T_& azimuth, T_& dip, T_& rake ) {
  azimuth = 90 - azimuth;
  dip     = - dip;
  rake    = rake;
}

template <class T_>
void 
convert_to_math_standard_angles_rad( T_& azimuth, T_& dip, T_& rake ) {
  azimuth = GsTL::PI / 2.0 - azimuth;
  dip     = - dip;
  rake    = rake;
}


template <class T_>
void 
convert_to_math_standard_angles_degree( T_& azimuth, T_& dip ) {
  azimuth = 90 - azimuth;
  dip     = - dip;
}

template <class T_>
void 
convert_to_math_standard_angles_rad( T_& azimuth, T_& dip ) {
  azimuth = GsTL::PI / 2.0 - azimuth;
  dip     = - dip;
}



#endif

