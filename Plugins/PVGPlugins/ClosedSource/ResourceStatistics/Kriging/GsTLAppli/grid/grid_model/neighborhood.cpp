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

#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>

#include <algorithm>



Colocated_neighborhood::
Colocated_neighborhood( Geostat_grid* grid,
			const std::string& property_name ) {
  grid_ = grid;
  property_ = grid->property( property_name );
//  center_property_ = 0;
}


void Colocated_neighborhood::find_neighbors( const Geovalue& center ) {
  if( neighbors_.empty() ) {
    neighbors_.push_back( center );
    neighbors_[0].set_property_array( property_ );
  }
  else {
    neighbors_[0] = center;
    neighbors_[0].set_property_array( property_ );
  }

  //neighbors_[0].set_node_id( center.node_id() );
  //neighbors_[0].set_location( center.location() );
  
}

Geovalue Colocated_neighborhood::center() const {
  return neighbors_[0];
  //Geovalue( grid_, 
		   //grid_->property( center_property_->name() ), 
		   //neighbors_[0].node_id() );
}


bool Colocated_neighborhood::
select_property( const std::string& prop_name ) {
  property_ = grid_->property( prop_name );
  return ( property_ != 0 );
}


void Colocated_neighborhood::
set_neighbors( const_iterator begin, const_iterator end ) {
  if( begin == end ) return;

  const Geostat_grid* g_grid = begin->grid();
  if( g_grid != grid_ ) return;

  Colocated_neighborhood::find_neighbors( *begin );
}


 
//======================================

Grid_template::Grid_template() 
  : max_size_( 0 ) {
  scale_ = 1;
}

Grid_template::Grid_template( iterator begin, iterator end ) 
  : templ_(begin, end) {  
  max_size_ = int( templ_.size() );
}

Grid_template::Grid_template( const Grid_template& rhs ) 
  : templ_( rhs.templ_ ), original_( rhs.original_ ), 
    max_size_( rhs.max_size_ ) {
}

Grid_template& Grid_template::operator = ( const Grid_template& rhs ) {
  if( this != &rhs ) {
    templ_ = rhs.templ_;
    original_ = rhs.original_;
    max_size_ = rhs.max_size_;
  }
  return *this;
}

void Grid_template::init( iterator begin, iterator end ) {
  templ_.clear();
  std::copy( begin, end, std::back_inserter( templ_ ) );
  max_size_ = int( templ_.size() );

  if( !original_.empty() ) {
    original_.clear();
    std::copy( begin, end, std::back_inserter( original_ ) );
  }
}

void Grid_template::add_vector( GsTLInt i, GsTLInt j, GsTLInt k, int pos ) {
  add_vector( Euclidean_vector(i,j,k), pos );
/*  
  if( pos < 0 || pos >= templ_.size() )
    templ_.push_back( Euclidean_vector(i,j,k) );
  else
    templ_.insert( templ.begin()+pos, 
  max_size_++;
  */
}

void Grid_template::add_vector( const Euclidean_vector& vec, int pos ) {
  if( pos < 0 || pos >= templ_.size() ) {
    templ_.push_back( vec );
    if( !original_.empty() )
      original_.push_back( vec );
  }
  else {
    templ_.insert( templ_.begin()+pos, vec );
    if( !original_.empty() )
      original_.insert( original_.begin()+pos, vec );
  }

  max_size_++;
}


void Grid_template::remove_vector( int pos ) {
  if( pos < 0 || pos >= templ_.size() ) return;

  templ_.erase( templ_.begin() + pos );  
  if( !original_.empty() )
    original_.erase( original_.begin() + pos );
}


Grid_template::iterator Grid_template::end() {
  if( max_size_ >= int( templ_.size() ) ) 
    return templ_.end();
  else
    return templ_.begin()+max_size_;
}



void Grid_template::scale( int s ) {
  undo_scaling();
  scale_ = s;
  if( s == 1 ) return;

  if( original_.empty() ) {
    original_.resize( templ_.size() );
    std::copy( templ_.begin(), templ_.end(), original_.begin() );
  }

  typedef std::vector<Euclidean_vector>::iterator iterator;
  for( iterator it = templ_.begin(); it != templ_.end() ; ++it ) {
    *it = s * (*it);
  }
}


void Grid_template::undo_scaling() {
  if( !original_.empty() && scale_ != 1 )
    std::copy( original_.begin(), original_.end(), templ_.begin() );
}


void Grid_template::set_geometry( const std::vector<Euclidean_vector>& templ ) { 
  templ_ = templ; 
  original_ = templ;
} 

