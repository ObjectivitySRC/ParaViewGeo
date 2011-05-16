////////////////////////////////////////////////////////////////////////////////
//
//  File:        hpolygon.cxx
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

#include "hpolygon.h"
#include "hgrid.h"

void HPolygon::Init( const char name[])
{
	int		n, ni;
	double	x, y;

	FILE	*f = fopen( name, "rt");
	
	if ( !f)
		THROW_FILE( "can not open file", name);
	
	printf( "Reading file: %s\n", name );
		
	fscanf( f, "%d", &n);
	mtabSize.resize( n);

	for ( int i=0; i<n; ++i)
	{
		fscanf( f, "%d", &ni);
		mtabSize[i] = ni;

		for ( int j=0; j<ni; ++j)
		{
			fscanf( f, "%lg %lg", &x, &y);
			mtabPnt.insert( mtabPnt.end(), Vect2D( x, y) );
		}
	}
	fclose( f);
}

void HPolygon::Triangulate()
{
	printf( "Triangulation started\n" );

	HGrid	grid;

	grid.Init( mtabPnt, mtabSize);
	grid.Generate();

	vector<HTri>::iterator	itri;
	IterGCell	itr;

	for ( itr = grid.CellBegin(); itr != grid.CellEnd(); ++itr)
	{
		itri = mtabCell.insert( mtabCell.end(), HTri() );
		(*itri).rIndex( 0) = (*(*itr)->Node( 0))->Index();
		(*itri).rIndex( 1) = (*(*itr)->Node( 1))->Index();
		(*itri).rIndex( 2) = (*(*itr)->Node( 2))->Index();
	}
}

void HPolygon::WriteTEC( const char name[])
{
	FILE *f = fopen( name, "wt");
	fprintf( f, "TITLE = \"polygon\"\n");
	fprintf( f, "VARIABLES = \"X\", \"Y\"\n");
	fprintf( f, "ZONE T=\"TRIANGLES\", ");
	fprintf( f, "N=%2ld, ", mtabPnt.size() );
	fprintf( f, "E=%2ld, F=FEPOINT, ET=TRIANGLE C=BLACK\n ", mtabCell.size() );
	size_t		i;
	for ( i=0; i<mtabPnt.size(); ++i)
		fprintf( f, "%lg %lg\n", mtabPnt[i].X(), mtabPnt[i].Y() );
	for ( i=0; i<mtabCell.size(); ++i)
		fprintf( f, "%d %d %d\n", 1+mtabCell[i].Index(0), 1+mtabCell[i].Index(1), 1+mtabCell[i].Index(2) );
	fclose( f);
}
