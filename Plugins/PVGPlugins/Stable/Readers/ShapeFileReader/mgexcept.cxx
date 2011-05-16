////////////////////////////////////////////////////////////////////////////////
//
//  File:        mgexcept.cxx
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

#include "mgdecl.h"

// constants used for exception handling
const MGInt EX_FILE_CODE		= 101;
const MGInt EX_MATH_CODE		= 102;
const MGInt EX_MEMORY_CODE		= 103;
const MGInt EX_INTERNAL_CODE	= 104;
const MGInt EX_ASSERT_CODE		= 105;
const MGInt EX_REXP_CODE		= 106;

const char EX_FILE_STR[]		= "FILE";
const char EX_MATH_STR[]		= "MATH";
const char EX_MEMORY_STR[]		= "MEM";
const char EX_INTERNAL_STR[]	= "INTERNAL";
const char EX_ASSERT_STR[]		= "ASSERT";
const char EX_REXP_STR[]		= "REXP";


// name of file used for tracing
const char TRACE_FILE_NAME[]	= "trace.txt";
