////////////////////////////////////////////////////////////////////////////////
//
//  File:        hrect.h
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

#ifndef __HHRect_H__
#define __HHRect_H__

#include "mgdecl.h"
#include "vect.h"

class HRect
{
public:
	HRect()	{}
	HRect( const MGFloat& xmin, const MGFloat& ymin,
		  const MGFloat& xmax, const MGFloat& ymax) 
		  : mvMin( Vect2D(xmin,ymin)), mvMax( Vect2D(xmax,ymax))	{}

	~HRect()	{}

	void ExportTEC( FILE *f);

	bool	IsInside( const Vect2D& vct) const;
	bool	IsOverlapping( const HRect& rec) const;
	
	Vect2D	Center()				{ return (mvMin + mvMax)/2.0;}

	const Vect2D&	VMin() const	{ return mvMin;}
		  Vect2D&	rVMin() 		{ return mvMin;}
		  
	const Vect2D&	VMax() const	{ return mvMax;}
		  Vect2D&	rVMax() 		{ return mvMax;}

	const MGFloat&	XMin() const	{ return mvMin.X();}
	const MGFloat&	YMin() const	{ return mvMin.Y();}
	const MGFloat&	XMax() const	{ return mvMax.X();}
	const MGFloat&	YMax() const	{ return mvMax.Y();}
		  MGFloat&	rXMin() 		{ return mvMin.rX();}
		  MGFloat&	rYMin() 		{ return mvMin.rY();}
		  MGFloat&	rXMax() 		{ return mvMax.rX();}
		  MGFloat&	rYMax() 		{ return mvMax.rY();}

	
protected:
	Vect2D	mvMin;
	Vect2D	mvMax;
};

inline bool HRect::IsInside( const Vect2D& vct) const
{
	if ( vct.X() <= mvMax.X() && vct.X() >= mvMin.X() &&
		 vct.Y() <= mvMax.Y() && vct.Y() >= mvMin.Y() )

		return true;
	else
		return false;
}

inline bool HRect::IsOverlapping( const HRect& rec) const
{
	if ( rec.IsInside( Vect2D( XMin(), YMin() )) ||
		 rec.IsInside( Vect2D( XMax(), YMin() )) ||
		 rec.IsInside( Vect2D( XMin(), YMax() )) ||
 		 rec.IsInside( Vect2D( XMax(), YMax() )) )
	{
		return true;
	}
	else if ( IsInside( Vect2D( rec.XMin(), rec.YMin() )) ||
			  IsInside( Vect2D( rec.XMax(), rec.YMin() )) ||
			  IsInside( Vect2D( rec.XMin(), rec.YMax() )) ||
 			  IsInside( Vect2D( rec.XMax(), rec.YMax() )) )
	{
		return true;
	}
	else if ( rec.XMin() > XMin() && rec.XMax() < XMax() &&
			  rec.YMin() < YMin() && rec.YMax() > YMax() )
	{
		return true;
	}
	else if ( rec.XMin() < XMin() && rec.XMax() > XMax() &&
			  rec.YMin() > YMin() && rec.YMax() < YMax() )
	{
		return true;
	}
	else
		return false;
}

inline void HRect::ExportTEC( FILE *f)
{
	fprintf( f, "VARIABLES = \"X\",\"Y\"\n");
	fprintf( f, "ZONE I=%d, F=POINT\n", 5);
	fprintf( f, "%lg %lg\n", mvMin.X(), mvMin.Y() );
	fprintf( f, "%lg %lg\n", mvMax.X(), mvMin.Y() );
	fprintf( f, "%lg %lg\n", mvMax.X(), mvMax.Y() );
	fprintf( f, "%lg %lg\n", mvMin.X(), mvMax.Y() );
	fprintf( f, "%lg %lg\n", mvMin.X(), mvMin.Y() );
}
#endif // __HHRect_H__
