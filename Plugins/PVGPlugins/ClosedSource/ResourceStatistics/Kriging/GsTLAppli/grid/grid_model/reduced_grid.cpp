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

#include <GsTLAppli/grid/grid_model/reduced_grid.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/mgrid_neighborhood.h>

Named_interface* create_reduced_grid( std::string& size_str){
	if( size_str.empty() )
		return new Reduced_grid();
	else {
		int size = String_Op::to_number<int>( size_str );
		appli_assert( size >= 0 );
		return new Reduced_grid( size );
	}
}

Reduced_grid::Reduced_grid(int size) : _rSize(size) {
	property_manager_.set_prop_size( size );
}

Reduced_grid::Reduced_grid() : _rSize(0)  {}

Reduced_grid::~Reduced_grid() { 
}

Neighborhood* Reduced_grid::neighborhood( double x, double y, double z,
				   double ang1, double ang2, double ang3,
				   const Covariance<location_type>* cov,
				   bool only_harddata ) {

  // The constructor of Rgrid_ellips_neighborhood expects the dimensions
  // of the search ellipsoid to be in "number of cells", and the covariance
  // ranges to be expressed in term of "number of cells".

  int nx = GsTL::round( x /geom_->cell_dims()[0] );
  int ny = GsTL::round( y /geom_->cell_dims()[1] );
  int nz = GsTL::round( z /geom_->cell_dims()[2] );

  // The ranges of the covariance of a Neighborhood must be expressed
  // in "number of cells", while they are supplied to the rgrid in 
  // "actual" unit. So do the convertion. 
  Covariance<location_type>* cov_copy = 0;

  if( cov ) {
    cov_copy = new Covariance<location_type>( *cov );
    for( int i=0; i < cov_copy->structures_count() ; i++ ) {
      double R1,R2,R3;
      cov_copy->get_ranges( i, R1,R2,R3 );
      cov_copy->set_ranges( i,
		       R1/geom_->cell_dims()[0],
		       R2/geom_->cell_dims()[1],
		       R3/geom_->cell_dims()[2] );
    }
  }

  if( only_harddata )
    return new MgridNeighborhood_hd( this, 
					     property_manager_.selected_property(),
					     nx,ny,nz, ang1,ang2,ang3,
					     20, cov_copy );
  else
    return new MgridNeighborhood( this, 
					  property_manager_.selected_property(),
					  nx,ny,nz, ang1,ang2,ang3,
					  20, cov_copy );

  delete cov_copy;
}



Neighborhood* Reduced_grid::neighborhood( const GsTLTripletTmpl<double>& dim,
				   const GsTLTripletTmpl<double>& angles,
				   const Covariance<location_type>* cov,
				   bool only_harddata ) {
  int nx = GsTL::round( dim[0] /geom_->cell_dims()[0] );
  int ny = GsTL::round( dim[1] /geom_->cell_dims()[1] );
  int nz = GsTL::round( dim[2] /geom_->cell_dims()[2] );

  Covariance<location_type>* cov_copy = 0;

  if( cov ) {
    cov_copy = new Covariance<location_type>( *cov );
    for( int i=0; i < cov_copy->structures_count() ; i++ ) {
      double R1,R2,R3;
      cov_copy->get_ranges( i, R1,R2,R3 );
      cov_copy->set_ranges( i,
		       R1/geom_->cell_dims()[0],
		       R2/geom_->cell_dims()[1],
		       R3/geom_->cell_dims()[2] );
    }
  }

  if( only_harddata )
    return new MgridNeighborhood_hd( this, 
					     property_manager_.selected_property(),
					     nx,ny,nz,
					     angles[0], angles[1], angles[2],
					     20, cov_copy );
  else
    return new MgridNeighborhood( this, 
					  property_manager_.selected_property(),
					  nx,ny,nz,
					  angles[0], angles[1], angles[2],
					  20, cov_copy );
}

Window_neighborhood* Reduced_grid::window_neighborhood( const Grid_template& templ) {
  return new MgridWindowNeighborhood( templ, this,
					property_manager_.selected_property() );
}
  

// new methods
//====================================
void Reduced_grid::copyStructure(const Reduced_grid * from)
{
	GsTLCoordVector v = from->cell_dimensions();
	set_dimensions(from->nx(), from->ny(), from->nz(), v.x(), v.y(), v.z());
	origin(from->origin());

	_reduced2original = from->_reduced2original;
	_original2reduced = from->_original2reduced;
	_psc = from->_psc;
	_maskColumn = from->_maskColumn;
	grid_cursor_ = from->grid_cursor_;
	_rSize = from->_rSize;

	_mask = from->_mask;
}


bool Reduced_grid::populate(QDataStream& stream, std::vector< char* > & prop_names)
{
	int i,j;

	for (i = 0; i < geom_->size(); ++i)
		_mask[i] = false;

	int nxy = geom_->dim(0)*geom_->dim(1);
	for (i = 0; i < _rSize; ++i) {
		int x,y,z,index;
		stream >> x >> y >> z; // these are really i,j,k values
		index = z*nxy+y*geom_->dim(0)+x;

		_mask[index] = true;
		_reduced2original[i] = index;
		_original2reduced[index] = i;
		_psc.push_back(GsTLGridNode(x,y,z));	
	}

	grid_cursor_.init(&_original2reduced, &_reduced2original, _rSize, &_mask);

	for (i = 0; i < prop_names.size(); ++i) {
		std::string prop_name = prop_names[i];
		GsTLGridProperty * d = property_manager_.add_property(prop_name);

		if (!d) return false;

		for (j = 0; j < _rSize; ++j) {
			float buf;
			stream >> buf;
			d->set_value(buf,j);
		}
	}
	return true;
}


// returns nx*ny*nz
GsTLInt Reduced_grid::trueSize() const {
	return RGrid::size();
}

//number of active cells
GsTLInt Reduced_grid::numActive() const {
	return _reduced2original.size();
}


void Reduced_grid::buildIJK()
{
	int i;
	if (_mask.empty()) return;
	_psc.clear();
	for (i = 0; i < _rSize; ++i) 
		_psc.push_back(Reduced_grid::ijkValue(i));	
}


const std::vector<GsTLGridNode> & Reduced_grid::psIJK() const { 
	return _psc;
}


const std::string Reduced_grid::maskColumn() const { 
	if (_maskColumn.empty()) return MASK;
	return _maskColumn; 
}


const int Reduced_grid::full2reduced(int idInFullGrid)	const {
	std::map<int,int>::const_iterator itr = _original2reduced.find(idInFullGrid);
	if (itr == _original2reduced.end()) return -1;
	return itr->second;
}

const int Reduced_grid::reduced2full(int idInReducedGrid) const{
	std::map<int,int>::const_iterator itr = _reduced2original.find(idInReducedGrid);
	if (itr == _reduced2original.end()) return -1;
	return itr->second;
}

//==================================
// methods from rgrid that use grid_cursor_.
// need to make sure the right cursor is referenced
inline bool Reduced_grid::contains(GsTLInt i, GsTLInt j, GsTLInt k) { 
	return (grid_cursor_.check_triplet(i, j, k)); 
} 


inline bool Reduced_grid::contains(const GsTLGridNode& gn) { 
	return (grid_cursor_.check_triplet(gn[0], gn[1], gn[2])); 
} 



inline void Reduced_grid::set_level( int level) { 
	grid_cursor_.set_multigrid_level(level); 
} 


inline int Reduced_grid::current_level() const { 
	return grid_cursor_.multigrid_level(); 
} 


Geovalue Reduced_grid::geovalue( GsTLInt gindex ) { 
	int i,j,k; 
	grid_cursor_.coords( gindex, i,j,k ); 
	accessor_->set_geovalue( gindex, geom_->coordinates( i,j,k ) ); 
	return (*accessor_->node()); 
} 

Geovalue Reduced_grid::geovalue( GsTLInt i, GsTLInt j, GsTLInt k ) { 
	accessor_->set_geovalue( grid_cursor_.node_id( i,j,k ), 
		geom_->coordinates( i,j,k ) ); 
	return (*accessor_->node()); 
} 


Geovalue Reduced_grid::geovalue(const GsTLGridNode& gn ) { 
	return geovalue( gn[0], gn[1], gn[2] ); 
} 


void Reduced_grid::set_cursor(SGrid_cursor cursor){
	grid_cursor_ = cursor; 
}


bool Reduced_grid::is_informed( GsTLInt i,  GsTLInt j, GsTLInt k ) { 
	int ind = grid_cursor_.node_id( i,j,k ); 
	if ( ind < 0 )  
		return false; 

	return property_manager_.selected_property()->is_informed( ind ); 

} 

Geostat_grid::iterator Reduced_grid::begin( GsTLGridProperty* prop ) { 
	GsTLGridProperty* property = prop;
	if( !prop )
		property = property_manager_.selected_property();

	return iterator( this, property, 0, grid_cursor_.max_index(),  
		LinearMapIndex() ); 
} 


Geostat_grid::iterator Reduced_grid::end( GsTLGridProperty* prop ) { 
	GsTLGridProperty* property = prop;
	if( !prop )
		property = property_manager_.selected_property();

	return iterator( this, property,
		grid_cursor_.max_index(), grid_cursor_.max_index(), 
		LinearMapIndex() ); 
} 


Geostat_grid::const_iterator Reduced_grid::begin( const GsTLGridProperty* prop ) const { 
	const GsTLGridProperty* property = prop;
	if( !prop )
		property = property_manager_.selected_property();

	return const_iterator( this, property, 0, grid_cursor_.max_index(),  
		LinearMapIndex() ); 
} 


Geostat_grid::const_iterator Reduced_grid::end( const GsTLGridProperty* prop ) const { 
	const GsTLGridProperty* property = prop;
	if( !prop )
		property = property_manager_.selected_property();

	return const_iterator( this, property,
		grid_cursor_.max_index(), grid_cursor_.max_index(), 
		LinearMapIndex() ); 
} 


Geostat_grid::random_path_iterator Reduced_grid::random_path_begin( GsTLGridProperty* prop ) { 
	if( int(grid_path_.size()) != grid_cursor_.max_index() )  
		init_random_path(); 

	GsTLGridProperty* property = prop;
	if( !prop )
		property = property_manager_.selected_property();

	return random_path_iterator( this, property, 
		0, grid_cursor_.max_index(),  
		TabularMapIndex(&grid_path_) ); 
} 

Geostat_grid::random_path_iterator Reduced_grid::random_path_end( GsTLGridProperty* prop ) { 
	if( int(grid_path_.size()) != grid_cursor_.max_index() )  
		init_random_path(); 

	GsTLGridProperty* property = prop;
	if( !prop )
		property = property_manager_.selected_property();

	return random_path_iterator( this, property, 
		grid_path_.size(),  
		grid_path_.size(), 
		TabularMapIndex(&grid_path_) ); 
} 

bool Reduced_grid::is_informed( const GsTLGridNode& p ) { 
	return is_informed( p[0], p[1], p[2] ); 
} 

GsTLInt Reduced_grid::node_id ( GsTLInt index ) const {  
	return grid_cursor_.node_id( index );  
} 

void Reduced_grid::init_random_path( bool from_scratch) {
	if( int( grid_path_.size() ) !=  grid_cursor_.max_index() ) {
		grid_path_.resize( grid_cursor_.max_index() );
		for( int i=0; i < int( grid_path_.size() ); i++ ) 
			grid_path_[i] = i;
	}
	else {
		if( from_scratch ) {
			for( int i=0; i < int( grid_path_.size() ); i++ ) 
				grid_path_[i] = i;
		}
	}

	STL_generator gen;
	std::random_shuffle( grid_path_.begin(), grid_path_.end(), gen );

}

inline Geostat_grid::location_type Reduced_grid::location( int node_id ) const { 
	std::map<int,int>::const_iterator itr = _reduced2original.find(node_id);
	node_id = itr->second;
	GsTLInt max_nxy = geom_->dim(0)*geom_->dim(1);
	GsTLInt inxy = node_id % max_nxy; 
	GsTLInt k = (node_id - inxy)/max_nxy; 
	GsTLInt j = (inxy - node_id%geom_->dim(0))/geom_->dim(0); 
	GsTLInt i = inxy%geom_->dim(0);

	return geom_->coordinates( i,j,k ); 
} 

const SGrid_cursor* Reduced_grid::cursor() const { return &grid_cursor_; }
SGrid_cursor* Reduced_grid::cursor() { return &grid_cursor_; }

// location specified in full grid

GsTLInt Reduced_grid::closest_node( const location_type & P ){
	Neighborhood::iterator itr;
	Neighborhood * n;
	GsTLInt center, radius;
	location_type origin = geometry_->origin();
	location_type P0;		

	P0.x() = P.x() - origin.x();
	P0.y() = P.y() - origin.y();
	P0.z() = P.z() - origin.z();

	GsTLCoordVector cell_sizes = geometry_->cell_dims();
	int spacing = grid_cursor_.multigrid_spacing();

	// Account for the multi-grid spacing
	cell_sizes.x() = cell_sizes.x() * spacing;
	cell_sizes.y() = cell_sizes.y() * spacing;
	cell_sizes.z() = cell_sizes.z() * spacing;

	GsTLInt i = std::max( GsTL::floor( P0.x()/cell_sizes.x() + 0.5 ), 0 );
	GsTLInt j = std::max( GsTL::floor( P0.y()/cell_sizes.y() + 0.5 ), 0 );
	GsTLInt k = std::max( GsTL::floor( P0.z()/cell_sizes.z() + 0.5 ), 0 );

	// The function will return a valid node even if P is outside the
	// grid's bounding box  
	if( i >= grid_cursor_.max_iter( SGrid_cursor::X ) )
		i = grid_cursor_.max_iter( SGrid_cursor::X ) - 1;
	if( j >= grid_cursor_.max_iter( SGrid_cursor::Y ) )  
		j = grid_cursor_.max_iter( SGrid_cursor::Y ) - 1;
	if( k >= grid_cursor_.max_iter( SGrid_cursor::Z ) )
		k = grid_cursor_.max_iter( SGrid_cursor::Z ) - 1;

	center = grid_cursor_.node_id( i, j, k );
	if (center >= 0 && is_informed(i,j,k))
		return center;
	else if (center < 0)
		return -1;

	GsTLCoordVector v = cell_dimensions();
	radius = nx()*v.x()/DIV;
	if (radius < 1)
		radius = 1;

	appli_message("radius: " << radius);
	n = neighborhood(radius,radius,radius,0,0,0);
	n->includes_center(false);

	// returns index in reduced grid
	n->find_neighbors(Geovalue(this,NULL,center));

	if (n->size() == 0) {
		delete n;
		return -1;
	}
	for (itr = n->begin(); itr != n->end(); ++itr) 
		if (itr->is_informed())
			return itr->node_id();

	delete n;
	return -1;
}

//==============================================

//==============================================
// The following override the functions inherited
void Reduced_grid::set_geometry(RGrid_geometry* geom) {
	if( geom_ != geom ) {
		delete geom_;
		geom_ = geom->clone();
		topology_is_updated_ = false;
	}
	grid_cursor_.setDims(nx(), ny(), nz());
	geometry_ = dynamic_cast<Simple_RGrid_geometry*>( geom_ );

	for (int i = 0; i < geom_->size(); ++i)
		_mask.push_back(false);
}


GsTLInt Reduced_grid::size() const { 
	return _rSize; 
} 

