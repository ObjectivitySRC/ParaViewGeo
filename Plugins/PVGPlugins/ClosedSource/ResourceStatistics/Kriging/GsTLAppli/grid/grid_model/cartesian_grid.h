/**********************************************************************
** Author: Nicolas Remy
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

#ifndef __GSTLAPPLI_CARTESIAN_GRID_H__ 
#define __GSTLAPPLI_CARTESIAN_GRID_H__ 
 
 
#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/grid/grid_model/rgrid.h> 
#include <GsTLAppli/grid/grid_model/rgrid_geometry.h> 
#include <GsTLAppli/math/box.h> 
 
 
 
class GRID_DECL Cartesian_grid : public RGrid { 
 
 public: 
   
  Cartesian_grid()  
    : RGrid() { 
  }  
   
  Cartesian_grid(int nx, int ny, int nz) ; 
   
  virtual void set_geometry( RGrid_geometry* geom ); 
  void set_dimensions( int nx, int ny, int nz ); 
   
  void set_dimensions( int nx, int ny, int nz, 
		       float xsize, float ysize, float zsize ); 
  GsTLCoordVector cell_dimensions() const { return geometry_->cell_dims(); } 
 
  void origin( const GsTLPoint& origin ) { geometry_->set_origin( origin ); } 
  GsTLPoint origin() const { return geometry_->origin(); } 
   
  virtual std::string classname() const { return "Cgrid"; } 
  
  virtual GsTLInt closest_node( const location_type& P ); 
 
  GsTL_cube bounding_box() const; 
 
 
 protected: 
  Simple_RGrid_geometry* geometry_; 
}; 
 
 
 
//================================ 
// creation function 
Named_interface* create_Cgrid( std::string& ); 
 
 
 
#endif 
