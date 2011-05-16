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

#ifndef __GSTLAPPLI_SUPERBLOCK_H__ 
#define __GSTLAPPLI_SUPERBLOCK_H__ 
 
 
#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/math/gstlpoint.h> 

#include <GsTL/math/math_functions.h> 
#include <GsTL/geometry/geometry_algorithms.h> 
 
#include <vector> 
  
class Geostat_grid;
class Simple_hash_multimap; 
class SGrid_cursor;
 
class GRID_DECL Superblock{ 
 
 public: 
     
  Superblock() : grid_(0) {} 
  Superblock( Geostat_grid *grid,
              double range_x, double range_y, double range_z,
              double angle_1, double angle_2, double angle_3,
              int total_blocks = 100 ); 
  ~Superblock();
  
  void get_candidates(std::vector<int>& candidates, const GsTLPoint& point); 
 

 private:
  void init_superblocks_grid( int total_blocks );
  GsTLGridNode find_containing_block( const GsTLPoint& p );
  bool is_valid_block( const GsTLGridNode& block );

 private:
  Geostat_grid *grid_; 

  //Cartesian_grid* superblocks_;
  GsTLPoint lower_left_corner_, upper_right_corner_;
  float blocksize_[3];
  int nx_, ny_, nz_;
  SGrid_cursor* cursor_;

  typedef GsTLGridNode::difference_type Euclidean_vector;
  std::vector< Euclidean_vector > search_template_;
  Simple_hash_multimap* sblock_contents_;

}; 
 
 
	 
  
#endif 
 
 
 
 
   
 
     
		 
     
     
