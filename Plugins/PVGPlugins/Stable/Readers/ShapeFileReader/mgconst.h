////////////////////////////////////////////////////////////////////////////////
//
//  File:        mgconst.h
//  Created by:  Jerzy Majewski - jmajewsk@meil.pw.edu.pl
//  Modified by: 
//
// Copyright notice:
//    Copyright (C) 2000-2003 Jerzy Majewski 
// 
//    This is free software. You can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public License
//    as published by the Free Software Foundation.
//
//    This software is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY. See the GNU Lesser General Public License 
//    for more details.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MGCONST_H__
#define __MGCONST_H__


// floating point zero
extern const MGFloat ZERO;


// constants below have to be defined by preprocessor command #define
// because of dupliate naming in UNIX system; In UNIX those constants are 
// already defined using preprocessor
#ifndef M_E
#define M_E         2.7182818284590452354
#endif
#ifndef M_LOG2E
#define M_LOG2E     1.4426950408889634074
#endif
#ifndef M_LOG10E
#define M_LOG10E    0.43429448190325182765
#endif
#ifndef M_LN2
#define M_LN2       0.69314718055994530942
#endif
#ifndef M_LN10
#define M_LN10      2.30258509299404568402
#endif
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923
#endif
#ifndef M_1_PI
#define M_1_PI      0.31830988618379067154
#endif
#ifndef M_PI_4
#define M_PI_4      0.78539816339744830962
#endif
#ifndef M_2_PI
#define M_2_PI      0.63661977236758134308
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI  1.12837916709551257390
#endif
#ifndef M_SQRT2
#define M_SQRT2     1.41421356237309504880
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2   0.70710678118654752440
#endif

#ifndef PI                      // as in stroustrup 
#define PI  M_PI
#endif
#ifndef PI2
#define PI2  M_PI_2
#endif



#endif // __MGCONST_H__
