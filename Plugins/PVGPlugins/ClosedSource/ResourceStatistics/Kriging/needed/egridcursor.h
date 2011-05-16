/** Author: Ting Li
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

// $Log: egridcursor.h,v $
// Revision 4.0  2008/04/23 02:42:03  nico97492
// BOOK RELEASE
//
// Revision 3.0  2007/06/25 01:12:02  nico97492
// NEW RELEASE
//
// Revision 1.3  2007/06/04 06:15:27  nico97492
// Fixes to compile on Ubuntu 7 - g++ 4.1.2
//
// Revision 1.2  2007/05/06 19:55:41  lting2001
// fixed a bug in masked grid cursor;  added two files to compute neighbors for a masked grid
//
// Revision 1.1  2006/12/07 01:08:41  lting2001
// *** empty log message ***
//

// cursor for grid w/ inactive cells

#ifndef EGRIDCURSOR_H
#define EGRIDCURSOR_H

#include "sgrid_cursor.h"
#include <map>
#include <vector>

//according to STL spec, this specialization uses bits for boolean values and
// bit-wise operations
typedef std::vector<bool> bit_vector;
typedef std::vector<GsTLInt> GINT;

class EGridCursor : public SGrid_cursor
{
public:
	EGridCursor() : _full2reduced(NULL), _reduced2full(NULL), _mask(NULL), _rSize(0) {}

	~EGridCursor() {}

	//EGridCursor& operator = (const EGridCursor& gc ) {

	// reimplement base class method
	void setDims( GsTLInt nx, GsTLInt ny, GsTLInt nz ) 
	{ 
		max_dim_[0] = nx; 
		max_dim_[1] = ny; 
		max_dim_[2] = nz; 
		max_nxy_ = nx * ny; 
		use_anistropic_ = false;
		set_multigrid_level(1); 
	} 

	void init(std::map<int,int> *orr, std::map<int,int> * ro, GsTLInt s, bit_vector * p) { 
		_full2reduced = orr; 
		_reduced2full = ro;
		_rSize = s;
		max_size_ = max_index_ = s;
		_mask = p;
	}


	// returns the node id on the current level in the full grid	
	GsTLInt full_node_id( GsTLInt index ) const 
	{ 
		if (!use_anistropic_ && multigrid_spacing_==1)    
			return index;         // isotropic expansion
		else if ( multigrid_spacing_x_==1 && multigrid_spacing_y_==1 && multigrid_spacing_z_==1)
			return index;         // anisotropic expansion
		else { 
			GsTLInt inxy = index % nxy_; 
			GsTLInt k = (index - inxy)/nxy_; 
			GsTLInt j = (inxy - index%max_iter_[0])/max_iter_[0]; 
			GsTLInt i = inxy%max_iter_[0]; 

			return i*one_step_[0] + j*one_step_[1] + k*one_step_[2]; 
		} 
	} 

	// returns node id in the reduced grid which is on the current level
	GsTLInt node_id(GsTLInt index) const
	{
		if (!use_anistropic_ && multigrid_spacing_==1)    
			return index;         // isotropic expansion
		else if ( multigrid_spacing_x_==1 && multigrid_spacing_y_==1 && multigrid_spacing_z_==1)
			return index;      
		else {
			const std::map<int,GINT>::const_iterator t = _levels.find(multigrid_level_);
			return (t->second)[index];
		}
	}

	GsTLInt node_id( GsTLInt i, GsTLInt j, GsTLInt k ) const 
	{ 
		int id;
		if (!check_triplet(i, j, k))	return -1; 

		id = i*one_step_[0] + j*one_step_[1] + k*one_step_[2];
		if (!(*_mask)[id])
			return -1;
		return (*_full2reduced)[id]; 
	} 

	EGridCursor & operator = (const SGrid_cursor & gc ) 
	{
		*this = gc;
		set_multigrid_level(multigrid_level_);
		return *this;
	}

	void set_multigrid_level( GsTLInt level) 
	{ 
		int i, max; 

		multigrid_level_ = level; 
		int ng =  level-1;
		multigrid_spacing_ = (int) pow(2.0, (double) ng); 

		if ( use_anistropic_ )
		{
			multigrid_spacing_x_ = spacing_x_[ng];
			multigrid_spacing_y_ = spacing_y_[ng];
			multigrid_spacing_z_ = spacing_z_[ng];
		}
		else
		{
			multigrid_spacing_x_ = multigrid_spacing_;
			multigrid_spacing_y_ = multigrid_spacing_;
			multigrid_spacing_z_ = multigrid_spacing_;
		}

		one_step_[0] = multigrid_spacing_x_; 
		one_step_[1] = multigrid_spacing_y_*max_dim_[0]; 
		one_step_[2] = multigrid_spacing_z_*max_dim_[0]*max_dim_[1]; 

		max_iter_[0] = GsTL::ceil( float(max_dim_[0]) / float(multigrid_spacing_x_) ); 
		max_iter_[1] = GsTL::ceil( float(max_dim_[1]) / float(multigrid_spacing_y_) ); 
		max_iter_[2] = GsTL::ceil( float(max_dim_[2]) / float(multigrid_spacing_z_) ); 

		if (max_iter_[0] < 1) max_iter_[0] = 1; 
		if (max_iter_[1] < 1) max_iter_[1] = 1; 
		if (max_iter_[2] < 1) max_iter_[2] = 1; 

		nxy_ = max_iter_[0]*max_iter_[1]; 

		if (level == 1) {
			max_index_ = _rSize;
			return;
		}
		if (_levels.find(level) != _levels.end()) {
			max_index_ = (_levels[level]).size();
			return;
		}

		bit_vector & mask = *_mask;

		max = max_iter_[0]*max_iter_[1]*max_iter_[2];

		// if we haven't associated the current level number with a list of 
		// active cell on this level, do so now
		for (i = 0; i < max; ++i)	{
			int id = full_node_id(i);
			if (mask[id])
					_levels[level].push_back((*_full2reduced)[id]);
		}
		max_index_ = _levels[multigrid_level_].size();
	} 


	void coords( const GsTLInt node_id, int& x, int& y, int& z ) const { 
		// compute the coordinates (i,j,k) in the fine grid. 
		GsTLInt real_node = (*_reduced2full)[node_id];
		GsTLInt inxy = real_node % max_nxy_; 
		GsTLInt k = (real_node - inxy)/max_nxy_; 
		GsTLInt j = (inxy - real_node%max_dim_[0])/max_dim_[0]; 
		GsTLInt i = inxy%max_dim_[0]; 

		x = i / multigrid_spacing_x_; 
		y = j / multigrid_spacing_y_; 
		z = k / multigrid_spacing_z_; 
	} 
	

	void local_coords( const GsTLInt index, int& i, int& j, int& k ) const { 
		GsTLInt real_node = (*_reduced2full)[index];
		GsTLInt inxy = real_node % nxy_; 
		k = (index - inxy)/nxy_; 
		j = (inxy - index%max_iter_[0])/max_iter_[0]; 
		i = inxy%max_iter_[0]; 
	}  
	

protected:

	// translates from a full grid index to reduced grid node id
	std::map<int,int> * _full2reduced;

	// the other way around
	std::map<int,int> * _reduced2full;

	// builds the correspondence between a level number and the list of active cells that
	// are actually on that level
	std::map<int,GINT> _levels;

	// indicates the active cells
	bit_vector * _mask;
	GsTLInt _rSize;  // # of active cells
};

#endif
