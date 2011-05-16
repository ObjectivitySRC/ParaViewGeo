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

#ifndef REDUCED_GRID_H
#define REDUCED_GRID_H


#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/egridcursor.h>
#include <GsTLAppli/math/gstlvector.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/math/gstlpoint.h>

#include <qdatastream.h>

#include <string>
#include <map>
#include <vector>

#define DIV 5.0  /* neighbor search radius = dim/DIV */

/* mask in a cartesian grid will be assigned this name by default */
#define MASK "__MASK_OF_ACTIVE_CELLS__"

Named_interface* create_reduced_grid( std::string& size_str);


class GRID_DECL Reduced_grid : public Cartesian_grid
{
public:
	Reduced_grid(int size); 
	Reduced_grid();   
	virtual ~Reduced_grid(); 

	virtual std::string classname() const { return "Masked_grid"; } 

	//====================================
	// new methods
	// this is based on point set read function

	void setConversionPair(int orig, int masked) {
		_reduced2original[masked] = orig;
		_original2reduced[orig] = masked;
		_mask[orig]  = true;
	}
	
	void insertLocation(GsTLGridNode loc){
		_psc.push_back(loc);
	}

	void setMaskName(std::string & s) { _maskColumn = s; }

	void initMaskedGrid(bool regular) {
		long int count = 0;
		for (int i = 0; i < _mask.size(); ++i)
			if (_mask[i] == true)
				++count;
		appli_assert(count);
		_rSize = count;
		property_manager_.set_prop_size( _rSize );
		grid_cursor_.init(&_original2reduced, &_reduced2original, _rSize, &_mask);
		if (regular)
			buildIJK();
	}

	void copyStructure(const Reduced_grid *);


	// read IJK-format input from a file
	bool populate(QDataStream& stream, std::vector< char* > & prop_names);

	// returns nx*ny*nz
	GsTLInt trueSize() const;

	//number of active cells
	GsTLInt numActive() const;

	// from id to cell location
	inline GsTLGridNode ijkValue(int node_id) const;
	inline bool isActive(int idInFullGrid) const ;

	// construct a vector of active cell coordinates(IJK's)
	void buildIJK();

	// return the vector of coordinates
	const std::vector<GsTLGridNode> & psIJK() const;

	// name of the mask column
	const std::string maskColumn() const ;

	// given a node id in full grid, return the corresponding id in masked grid
	// returns -1 if not applicable.
	const int full2reduced(int idInFullGrid) const ;
	const int reduced2full(int idInReducedGrid) const;


	//==================================
	// methods from rgrid that use grid_cursor_.
	// Because grid_cursor_ is defined as an object, all functions that
	// depend on this cursor will end up calling the cursor base class methods.
	// The following functions are from rgrid and cartesian_grid classes. They
	// all reference the grid_cursor_ object.  We need to override them here.
	virtual Neighborhood* neighborhood( double x, double y, double z, 
		double ang1, double ang2, double ang3, 
		const Covariance<location_type>* cov=0, 
		bool only_harddata = false  ); 
	virtual Neighborhood* neighborhood( const GsTLTripletTmpl<double>& dim, 
		const GsTLTripletTmpl<double>& angles, 
		const Covariance<location_type>* cov=0, 
		bool only_harddata = false  ); 
	virtual Window_neighborhood* window_neighborhood( const Grid_template& templ ); 

	inline bool contains(GsTLInt i, GsTLInt j, GsTLInt k);
	inline bool contains(const GsTLGridNode& gn) ;
	inline void set_level( int level) ;
	inline int current_level() const ;
	Geovalue geovalue( GsTLInt gindex );
	Geovalue geovalue( GsTLInt i, GsTLInt j, GsTLInt k ) ;
	Geovalue geovalue(const GsTLGridNode& gn ) ;
	void set_cursor(SGrid_cursor cursor);
	bool is_informed( GsTLInt i,  GsTLInt j, GsTLInt k );
	iterator begin( GsTLGridProperty* prop ) ;
	iterator end( GsTLGridProperty* prop ) ;
	const_iterator begin( const GsTLGridProperty* prop ) const ;
	const_iterator end( const GsTLGridProperty* prop ) const;
	random_path_iterator random_path_begin( GsTLGridProperty* prop ) ;
	random_path_iterator random_path_end( GsTLGridProperty* prop ) ;
	bool is_informed( const GsTLGridNode& p ) ;
	GsTLInt node_id ( GsTLInt index ) const ;
	void init_random_path( bool from_scratch = true) ;
	inline location_type location( int node_id ) const ;
	const SGrid_cursor* cursor() const ;
	SGrid_cursor* cursor() ;
	virtual GsTLInt closest_node( const location_type & P );// location specified in full grid
	//==============================================

	//==============================================
	// The following override the functions inherited from rgrid
	void set_geometry(RGrid_geometry* geom) ;
	GsTLInt size() const ;
	//===============================================

protected:
	
	// translates from an id in reduced grid to an id in the full grid
	std::map<int,int> _reduced2original;

	// the other direction
	std::map<int,int> _original2reduced;

	// stores coordinates of active cells
	std::vector<GsTLGridNode> _psc;

	// name of the mask column in a cartesian grid
	std::string _maskColumn;

	EGridCursor grid_cursor_;  // overrides parent

	// mask for active cells
	bit_vector _mask;

	// number of active cells
	GsTLInt _rSize;

};



inline GsTLGridNode Reduced_grid::ijkValue(int node_id) const {
	std::map<int,int>::const_iterator itr = _reduced2original.find(node_id);
	node_id = itr->second;
	GsTLInt max_nxy = geom_->dim(0)*geom_->dim(1);
	GsTLInt inxy = node_id % max_nxy; 
	GsTLInt k = (node_id - inxy)/max_nxy; 
	GsTLInt j = (inxy - node_id%geom_->dim(0))/geom_->dim(0); 
	GsTLInt i = inxy%geom_->dim(0);
	return GsTLGridNode(i,j,k);
}

inline bool Reduced_grid::isActive(int idInFullGrid) const {return _mask[idInFullGrid];}


#endif
