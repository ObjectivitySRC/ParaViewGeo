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

#ifndef __GSTLAPPLI_GRID_GRID_CURSOR_H__ 
#define __GSTLAPPLI_GRID_GRID_CURSOR_H__ 

#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/utils/gstl_types.h> 

#include <GsTL/math/math_functions.h> 

#include <cmath> 
#include <iostream> 
#include <vector>

using namespace std;

/** A SGrid_cursor is in charge of keeping track of the location 
* of a given grid-node, identified by its node_id. 
* The location changes depending on the multigrid we're working on. 
* Given a node_id, a SGrid_cursor can tell the node location (in the  
* current multigrid) and conversely, given the coordinates in the  
* current multigrid, it can tell the node_id. 
* 
* The metaphore behind the design of SGrid_cursor is a cursor that 
* can be moved around within the current multigrid. When it is moved, 
* it can be asked its current coodinates, its current node_id, etc. 
*/ 

class GRID_DECL SGrid_cursor { 
public: 
	enum Dir { X = 0, Y = 1, Z = 2 }; 
public: 

	/** Default constructor 
	*/ 
	SGrid_cursor() 
	{ 
		max_dim_[0] = 1; 
		max_dim_[1] = 1; 
		max_dim_[2] = 1; 
		max_nxy_ = 1; 
		max_size_ = 1; 
        use_anistropic_ = false;
		set_multigrid_level(1); 
	} 

	/** Constructor. 
	* @param nx, ny and nz are the number of cells of the grid in the x,y and z 
	* directions 
	* @param level is the coarse grid level. Level 1 corresponds to the full 
	* grid.  
	*/ 
	SGrid_cursor( GsTLInt nx, GsTLInt ny, GsTLInt nz, GsTLInt level = 1, bool use_anistropic=false ) 
	{ 
		max_dim_[0] = nx; 
		max_dim_[1] = ny; 
		max_dim_[2] = nz; 
		max_nxy_ = nx * ny; 
		max_size_ = nx * ny * nz; 
        use_anistropic_ = use_anistropic;
		set_multigrid_level(level); 
	} 
	
	
	SGrid_cursor( const SGrid_cursor& gc ) 
	{ 
		max_dim_[0] = gc.max_dim_[0]; 
		max_dim_[1] = gc.max_dim_[1]; 
		max_dim_[2] = gc.max_dim_[2]; 
		
		multigrid_level_ = gc.multigrid_level_; 
		multigrid_spacing_ = gc.multigrid_spacing_; 
		multigrid_spacing_x_ = gc.multigrid_spacing_x_; 
		multigrid_spacing_y_ = gc.multigrid_spacing_y_; 
		multigrid_spacing_z_ = gc.multigrid_spacing_z_; 

		spacing_x_ = gc.spacing_x_; 
		spacing_y_ = gc.spacing_y_; 
		spacing_z_ = gc.spacing_z_; 

        use_anistropic_ = gc.use_anistropic_;
		
		max_size_ = gc.max_size_; 
		nxy_ = gc.nxy_; 
		
		one_step_[0] =gc.one_step_[0]; 
		one_step_[1] =gc.one_step_[1]; 
		one_step_[2] =gc.one_step_[2]; 
		
		max_iter_[0] = gc.max_iter_[0]; 
		max_iter_[1] = gc.max_iter_[1]; 
		max_iter_[2] = gc.max_iter_[2]; 
		
		max_index_ = gc.max_index_; 
		max_nxy_ = gc.max_nxy_; 
	} 
	
	
	
	virtual ~SGrid_cursor() { 
	} 
	
	
	virtual SGrid_cursor& operator = (const SGrid_cursor& gc ) 
	{ 
		if( this != &gc ) 
		{ 
			max_dim_[0] = gc.max_dim_[0]; 
			max_dim_[1] = gc.max_dim_[1]; 
			max_dim_[2] = gc.max_dim_[2]; 
			
			multigrid_level_ = gc.multigrid_level_; 
			multigrid_spacing_ = gc.multigrid_spacing_; 
            multigrid_spacing_z_ = gc.multigrid_spacing_z_; 
            multigrid_spacing_x_ = gc.multigrid_spacing_x_; 
            multigrid_spacing_y_ = gc.multigrid_spacing_y_; 

            spacing_x_ = gc.spacing_x_; 
            spacing_y_ = gc.spacing_y_; 
            spacing_z_ = gc.spacing_z_; 

			use_anistropic_ = gc.use_anistropic_;
			
			max_size_ = gc.max_size_; 
			nxy_ = gc.nxy_; 
			
			one_step_[0] =gc.one_step_[0]; 
			one_step_[1] =gc.one_step_[1]; 
			one_step_[2] =gc.one_step_[2]; 
			
			max_iter_[0] = gc.max_iter_[0]; 
			max_iter_[1] = gc.max_iter_[1]; 
			max_iter_[2] = gc.max_iter_[2]; 
			
			max_index_ = gc.max_index_; 
			max_nxy_ = gc.max_nxy_; 
		} 
		return *this; 
	} 

	/** Use anistropic expansion in Z direction
	*/
    virtual void set_anistropic_expansion( std::vector< std::vector<int> >& factor )
	{
        use_anistropic_ = true;

        spacing_x_.clear();
        spacing_y_.clear();
        spacing_z_.clear();

        //cout << "User input template expansion factors are:  \n";
        for ( unsigned int i=0; i<factor.size(); i++)
        {
            std::vector<int>& one = factor[i];
            spacing_x_.push_back( one[0] );
            spacing_y_.push_back( one[1] );
            spacing_z_.push_back( one[2] );

            //cout << "          grid " << i << " :    ( " << spacing_x_[i] << " , " << spacing_y_[i] << " , " << spacing_z_[i] << " )\n";
        }

        set_multigrid_level( 1 );
	}
	
    /** Use istropic expansion in Z direction
	*/
    virtual void set_istropic_expansion()
	{
        use_anistropic_ = false;

        set_multigrid_level( 1 );
	}

	/** Change the multigrid level to \c level 
	*/ 
	virtual void set_multigrid_level( GsTLInt level) 
	{ 
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
		max_index_ = nxy_ * max_iter_[2]; 
	} 
	
	/** Access the current multigrid level 
	*/ 
	int multigrid_level() const { 
		return multigrid_level_; 
	} 
	
	/** Access the spacing between 2 "consecutive" nodes: if the coarse grid 
	* level is 1, the spacing is 1. If it is 3, the spacing is 4 (ie 2^(3-1) ).  
	*/ 
	GsTLInt multigrid_spacing() const { 
		return multigrid_spacing_; 
	} 
	
	GsTLInt multigrid_spacing_x() const { 
		return multigrid_spacing_x_; 
	} 

	GsTLInt multigrid_spacing_y() const { 
		return multigrid_spacing_y_; 
	} 

	GsTLInt multigrid_spacing_z() const { 
		return multigrid_spacing_z_; 
	} 
	
	/** Returns the number of cells in the \c dir direction for the current 
	* multigrid level 
	*/ 
	GsTLInt max_iter( SGrid_cursor::Dir dir ) const { 
		return max_iter_[dir]; 
	} 
	
	/** Returns the number of cells in the current coarse grid. 
	*/ 
	GsTLInt max_index() const { return max_index_; } 
	
	
	/** checks if a point in the current multigrid can have coordinate 
	* i in direction dir. 
	*/ 
	bool check_coord( SGrid_cursor::Dir dir, GsTLInt i ) const { 
		return ( (i >= 0) && (i < max_iter_[dir]) ); 
	} 
	
	/** Checks if (i,j,k) is are valid coordinates in the current  
	* multigrid. 
	*/ 
	bool check_triplet( GsTLInt i, GsTLInt j, GsTLInt k) const 
	{ 
		if(!check_coord(X, i)) return false;  
		if(!check_coord(Y, j)) return false;  
		if(!check_coord(Z, k)) return false;  

		return true; 
	} 
	
	/** checks if node "id" belongs to the current multigrid. 
	*/ 
	GsTLBool check_node_id( GsTLInt id ) const 
	{ 
		GsTLInt inxy = id % max_nxy_; 
		GsTLInt k = (id - inxy)/max_nxy_; 
		GsTLInt j = (inxy - id%max_dim_[0])/max_dim_[0]; 
		GsTLInt i = inxy%max_dim_[0]; 

		return  (i % multigrid_spacing_x_ == 0) && 
				(j % multigrid_spacing_y_ == 0) && 
				(k % multigrid_spacing_z_ == 0) ; 
	} 
	
	/** Returns the node_id, given the coordinates in the finest grid. 
	* Returns -1 if there is no node (i,j,k) 
	*/ 
	virtual GsTLInt node_id( GsTLInt i, GsTLInt j, GsTLInt k ) const 
	{ 
		if (!check_triplet(i, j, k))	return -1; 
		
		return i*one_step_[0] + j*one_step_[1] + k*one_step_[2]; 
	} 
	
	/** Returns the node_id, given the index in the finest grid  
	*/ 
	virtual GsTLInt node_id( GsTLInt index )  const
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
	
	
	
	
	/** The location in the current 
	* multigrid coordinate system of node-id "node_id" is computed and 
	* output to x,y,z. 
	*/ 
	virtual void coords( const GsTLInt node_id, int& x, int& y, int& z ) const { 
		// compute the coordinates (i,j,k) in the fine grid. 
		GsTLInt inxy = node_id % max_nxy_; 
		GsTLInt k = (node_id - inxy)/max_nxy_; 
		GsTLInt j = (inxy - node_id%max_dim_[0])/max_dim_[0]; 
		GsTLInt i = inxy%max_dim_[0]; 
		
		// The coordinates in the current multigrid are obtained 
		// by dividing by multigrid_spacing_.  
		// Nicolas, 01/22/04:
		// should use ceil instead ? x = ceil( i / multigrid_spacing_ )
		// if in the fine grid i=2,j=2, if multigrid_spacing_=3, x=0,y=0,
		// while the closest multigrid node is x=1,y=1
		x = i / multigrid_spacing_x_; 
		y = j / multigrid_spacing_y_; 
		z = k / multigrid_spacing_z_; 
	} 
	
	
	/** Compute the (i,j,k) coordinates in the current multigrid coordinate system 
	* of node whose index in the current multigrid is \c index 
	* @param index is the index of the node in the current multiple grid 
	* WARNING: THIS FUNCTION HAS NOT BEEN TESTED!!!!!!!1 
	*/ 
	virtual void local_coords( const GsTLInt index, int& i, int& j, int& k ) const { 
		GsTLInt inxy = index % nxy_; 
		k = (index - inxy)/nxy_; 
		j = (inxy - index%max_iter_[0])/max_iter_[0]; 
		i = inxy%max_iter_[0]; 
	}  
	
	
protected: 

	int max_dim_[3]; 
	GsTLInt max_nxy_; 
	GsTLInt max_size_; 

	GsTLInt nxy_; 
	GsTLInt max_index_; 

	int multigrid_spacing_; 
	int multigrid_spacing_x_; 
	int multigrid_spacing_y_; 
	int multigrid_spacing_z_; 
	int multigrid_level_; 

    std::vector<int> spacing_x_; 
	std::vector<int> spacing_y_; 
	std::vector<int> spacing_z_; 

	GsTLInt one_step_[3]; 
	GsTLInt max_iter_[3]; 

	bool use_anistropic_;
}; 

#endif 
