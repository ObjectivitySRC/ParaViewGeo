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

#ifndef __GSTLAPPLI_GRID_MODEL_STRATI_GRID_H__ 
#define __GSTLAPPLI_GRID_MODEL_STRATI_GRID_H__ 
 
#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/utils/gstl_types.h> 
#include <GsTLAppli/grid/grid_model/geostat_grid.h> 
 
 
class Window_neighborhood; 
class Grid_template; 
class SGrid_cursor; 
 
 
/** A Strati_grid is a stratigraphic grid, ie a grid with a catesian 
 * topology. 
 */ 
class GRID_DECL Strati_grid : public Geostat_grid { 
 
 public: 
  virtual ~Strati_grid() {} 
 
  /** Number of cells in the x direction 
   */ 
  virtual GsTLInt nx() const {return 0; } 
  /** Number of cells in the y direction 
   */ 
  virtual GsTLInt ny() const {return 0; } 
  /** Number of cells in the z direction 
   */ 
  virtual GsTLInt nz() const {return 0; } 
  
  /** In a stratigraphic grid, it is possible to define "multigrids" 
   * The original grid is multigrid level 1. Multigrid level 2 has 8 times 
   * less nodes than level 1 (it is obtained by discarding every other node,  
   * in the x, y, and z directions).  
   */ 
  virtual void set_level( int ) {} 

  /** Returns the current multi-grid level of the grid. Set the multi-grid 
  * level with \c set_level(i)
  */
  virtual int current_level() const { return 1; } 
 
  /** Access the grid cursor used by the stratigraphic grid. 
   */ 
  virtual const SGrid_cursor* cursor() const { return 0; } 
 
  /** Makes the conversion between grid coordinates (i,j,k) (i,j,k are integers) 
   * and "real-world" coordinates. 
   * @return the real coordinates of node (i,j,k) 
   */ 
  virtual location_type location( int i, int j, int k ) const = 0; 
 
  /** Create a window neighborhood of geometry \a templ. 
   * @param templ defines the geometry of the window
   * \attention Notice: it is the user's responsability to \c delete the neighborhood 
   * when it is not needed anymore.
   */ 
  virtual Window_neighborhood* window_neighborhood( const Grid_template& templ )=0; 
   
}; 
 
 
#endif 
 
