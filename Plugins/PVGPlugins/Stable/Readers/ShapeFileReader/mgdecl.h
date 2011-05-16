////////////////////////////////////////////////////////////////////////////////
//
//  File:        mgdecl.h
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

#ifndef __MGDECL_H__
#define __MGDECL_H__

// MS VC++ warning 4114 disabled
#ifdef WIN32
	#pragma warning(disable: 4114)
    #pragma warning(disable: 4786)
    #pragma warning(disable: 4788)
#endif


//////////////////////////////////////////////////////////////////////
// C includes
//////////////////////////////////////////////////////////////////////
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


//////////////////////////////////////////////////////////////////////
// C++ includes
//////////////////////////////////////////////////////////////////////
// none


#include "stldecl.h"	// includes files STL


//////////////////////////////////////////////////////////////////////
// definitions of basic types
//////////////////////////////////////////////////////////////////////

// definintion of MGNEW which does not throw exception
#define MGNEW new			// old standard
//#define MGNEW new(nothrow)	// recent standard

// definition of basic types
typedef double 		MGFloat;
typedef int			MGInt;
typedef string		MGString;


typedef vector<MGFloat>	MGFloatArr;
typedef vector<MGInt>	MGIntArr;


//////////////////////////////////////////////////////////////////////
// auxiliary objects 
//////////////////////////////////////////////////////////////////////
#include "mgexcept.h"	// file with objects and macros used for exception handling
#include "mgconst.h"	// file with definitions of constant values
#include "dimension.h"	// file with definition of dimension


#endif	// __MGDECL_H__

