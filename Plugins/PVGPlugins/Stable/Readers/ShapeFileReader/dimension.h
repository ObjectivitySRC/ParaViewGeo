////////////////////////////////////////////////////////////////////////////////
//
//  File:        dimension.h
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

#ifndef __DIMENSION_H__
#define __DIMENSION_H__

enum Dimension
{
	DIM_NONE	= 0,
	DIM_1D		= 1,
	DIM_2D		= 2,
	DIM_3D		= 3,
	DIM_4D		= 4
};



//////////////////////////////////////////////////////////////////////
// class GlobDim
//////////////////////////////////////////////////////////////////////
class GlobDim
{
public:
	GlobDim()						{ mDim = DIM_NONE;}
	GlobDim( const Dimension& dim)	{ ASSERT( mDim == DIM_NONE); mDim = dim;}
	
	static const Dimension&	Dim()	{ return mDim;}
	static Dimension&		rDim()	{ return mDim;}
	
protected:
	static Dimension	mDim;
};


#endif // __DIMENSION_H__
