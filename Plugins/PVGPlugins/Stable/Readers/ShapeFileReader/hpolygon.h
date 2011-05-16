////////////////////////////////////////////////////////////////////////////////
//
//  File:        hpolygon.h
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

#ifndef __HPOLYGON_H__
#define __HPOLYGON_H__

#include "mgdecl.h"
#include "vect.h"

//////////////////////////////////////////////////////////////////////
// class HTri
//////////////////////////////////////////////////////////////////////
class HTri
{
public:
	const MGInt&	Index( const MGInt& i) const	{ return mtabInd[i];}
	MGInt&			rIndex( const MGInt& i)			{ return mtabInd[i];}

public:
	MGInt	mtabInd[3];
};

//////////////////////////////////////////////////////////////////////
// class HPolygon
//////////////////////////////////////////////////////////////////////
class HPolygon
{
public:
	HPolygon()		{}
	~HPolygon()		{}

	void	Init( const char name[]);
	void	Triangulate();
	void	WriteTEC( const char name[]);

	vector<Vect2D>	mtabPnt;
	vector<MGInt>	mtabSize;
	vector<HTri>	mtabCell;
};

#endif // __HPOLYGON_H__
