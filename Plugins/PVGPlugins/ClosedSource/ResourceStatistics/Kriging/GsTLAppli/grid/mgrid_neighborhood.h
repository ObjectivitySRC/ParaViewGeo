/**********************************************************************
** Author: Ting Li
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
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

#ifndef MGRID_NEIGHBORHOOD_H
#define MGRID_NEIGHBORHOOD_H

#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/egridcursor.h>

class GRID_DECL MgridNeighborhood : public Rgrid_ellips_neighborhood
{
public:
	MgridNeighborhood( RGrid* grid, 
		GsTLGridProperty* property, 
		GsTLInt max_radius, GsTLInt mid_radius, GsTLInt min_radius, 
		double x_angle, double y_angle, double z_angle, 
		int max_neighbors = 20, 
		const Covariance<GsTLPoint>* cov = 0 ); 
	~MgridNeighborhood() {}
	 virtual void find_neighbors( const Geovalue& center ); 
protected:
	 EGridCursor * _mcursor;

};

class GRID_DECL MgridWindowNeighborhood : public Rgrid_window_neighborhood
{
public:
	 MgridWindowNeighborhood( const Grid_template& geom, RGrid* grid=0, 
			     GsTLGridProperty* prop = 0 );
	 ~MgridWindowNeighborhood(){}
	 virtual void find_neighbors( const Geovalue& center ); 
	 virtual void set_grid(RGrid *);
     virtual void find_all_neighbors( const Geovalue& center );

protected:
	 EGridCursor * _mcursor;
};

class GRID_DECL MgridNeighborhood_hd : public Rgrid_ellips_neighborhood_hd
{
public:
	MgridNeighborhood_hd( RGrid* grid, 
		GsTLGridProperty* property, 
		GsTLInt max_radius, GsTLInt mid_radius, GsTLInt min_radius, 
		double x_angle, double y_angle, double z_angle, 
		int max_neighbors = 20, 
		const Covariance<GsTLPoint>* cov = 0 ); 
	~MgridNeighborhood_hd() {}
	virtual void find_neighbors( const Geovalue& center ); 
protected:
	EGridCursor * _mcursor;
};
#endif