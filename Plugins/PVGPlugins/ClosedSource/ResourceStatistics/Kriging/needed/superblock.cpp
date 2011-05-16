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

#include "superblock.h"
#include "geostat_grid.h"
#include "sgrid_cursor.h"
#include "gval_iterator.h"
#include "rgrid_neighborhood.h"



#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>


class Simple_hash_multimap {
public:
  typedef std::list<GsTLInt>::iterator iterator;
  typedef std::pair< iterator, iterator > iterator_pair;
  Simple_hash_multimap( int size = 0 );
  
  void insert( unsigned int key, GsTLInt elem );

  // find all the elements whose key are "key"
  iterator_pair equal_range( unsigned int key );

  void audit();


private:
  std::list< GsTLInt > elements_;
  std::vector< iterator > keys_;
};


Simple_hash_multimap::Simple_hash_multimap( int size ) 
: keys_( size + 1, elements_.end() ) {

}

void Simple_hash_multimap::insert( unsigned int key, GsTLInt elem ) {
  if( key >= keys_.size() ) {
    int old_size = keys_.size();
    keys_.resize( key + 2 );
    for( int i = old_size - 1; i < keys_.size() ; i++ ) 
      keys_[i] = elements_.end();
  }

  // if there is no element for the current key:
  if( keys_[key] == keys_[key+1] ) {
    elements_.insert( keys_[ key+1 ], elem );
    iterator old_it = keys_[key+1];
    iterator new_it = old_it;
    --new_it;

    int pos = key;
    while( keys_[ pos ] == old_it ) {
      keys_[ pos ] = new_it;
      pos--;
    }
  }
  else {
    elements_.insert( keys_[ key+1 ], elem );
  }

}


Simple_hash_multimap::iterator_pair 
Simple_hash_multimap::equal_range( unsigned int key ) {
  return std::make_pair( keys_[key], keys_[key+1] );
}


void Simple_hash_multimap::audit() {
  std::cout << "elements_:" << std::endl;
  std::copy( elements_.begin(), elements_.end(), 
    std::ostream_iterator<int>( std::cout, " " ) );
  std::cout << std::endl << std::endl;

  std::cout << "block contents\n";
  for( int i=0; i < keys_.size()-1; i++ ) {
    std::cout << "block " << i << "\n";
    for( iterator it = keys_[i]; it != keys_[i+1] ; ++it ){
      std::cout << *it << " ";
    }
    std::cout << std::endl << std::endl;
  }
}

//=============================================
  
Superblock::Superblock( Geostat_grid *grid,
                        double range_x, double range_y, double range_z,
                        double angle_1, double angle_2, double angle_3,
                        int total_blocks ) 
  : cursor_( 0 ) {

  grid_ = grid;
 

  init_superblocks_grid( total_blocks );
  cursor_ = new SGrid_cursor( nx_, ny_, nz_ );

  sblock_contents_ = new Simple_hash_multimap( nx_ * ny_ * nz_ );


  // Compute the superblock search template
  float ranges[3] = { range_x, range_y, range_z };
  for( int i = 0 ; i < 3 ; i++ )
    ranges[i] /= blocksize_[i];

  Ellipsoid_rasterizer rasterizer( nx_, ny_, nz_, 
                                   ranges[0], ranges[1], ranges[2], 
                                   angle_1, angle_2, angle_3 );
  search_template_.push_back( Euclidean_vector(0,0,0) );
  std::copy( rasterizer.rasterize().begin(), rasterizer.rasterize().end(),
             std::back_inserter( search_template_ ) );

#ifndef GSLTAPPLI_NDEBUG
  std::cout << "search template: \n"; 
  for( int j=0; j < search_template_.size(); j++ )
    std::cout << search_template_[j] << "\n";
  std::cout << "\n\n" << std::endl; 
#endif

  // compute the content of each superblock
  Geostat_grid::iterator begin = grid_->begin();
  Geostat_grid::iterator end = grid_->end();

  for( ; begin != end ; ++begin ) {
    if( !begin->is_informed() ) continue;

    GsTLGridNode block = find_containing_block( begin->location() );
    //appli_assert( is_valid_block( block ) );
    int block_id = cursor_->node_id( block.x(), block.y(), block.z() );
    sblock_contents_->insert( block_id, begin->node_id() );
  }

  sblock_contents_->audit();
}


Superblock::~Superblock() {
  delete cursor_;
  delete sblock_contents_;
}


void Superblock::init_superblocks_grid( int total_blocks ) {
  Geostat_grid::iterator begin = grid_->begin();
  Geostat_grid::iterator end = grid_->end();
  
  for( ; begin != end ; ++begin ) {
    if( !begin->is_informed() ) continue;

    float x = begin->location().x();
    float y = begin->location().y();
    float z = begin->location().z();

    upper_right_corner_.x() = std::max( upper_right_corner_.x(), x );
    upper_right_corner_.y() = std::max( upper_right_corner_.y(), y );
    upper_right_corner_.z() = std::max( upper_right_corner_.z(), z );

    lower_left_corner_.x() = std::min( lower_left_corner_.x(), x );
    lower_left_corner_.y() = std::min( lower_left_corner_.y(), y );
    lower_left_corner_.z() = std::min( lower_left_corner_.z(), z );
  }

  // slightly enlarge (by 2%) the bounding box to avoid border-effect issues
  int dim = 3;
  int blocks[3] = {0,0,0};
  float box_sizes[3];
  for( int i =0; i < 3; i++ ) {
    float grow = ( upper_right_corner_[i] - lower_left_corner_[i] )/50.0;
    if( grow == 0.0 ) {
      dim--;
      grow = 0.5;
      blocks[i] = 1;
    }

    upper_right_corner_[i] += grow;
    lower_left_corner_[i] -= grow;
    
    box_sizes[i] = upper_right_corner_[i] - lower_left_corner_[i];
  }

  float blocksize;
  if( dim != 0 )
    blocksize = 
      std::pow( 
        box_sizes[0]*box_sizes[1]*box_sizes[2] / total_blocks, 
        float(1.0/dim) 
      );
  else
    blocksize = 1;


  // modify the block sizes so as not to avoid truncated blocks
  for( int j =0; j < 3; j++ ) {
    if( blocks[j] == 0 ) {
      blocks[j] = std::max( GsTL::round( 
                    (upper_right_corner_[j] - lower_left_corner_[j] ) / blocksize
                  ), 1 );
    }
    blocksize_[j] = (upper_right_corner_[j] - lower_left_corner_[j] ) / blocks[j];
  }

  nx_ = blocks[0];
  ny_ = blocks[1];
  nz_ = blocks[2];
  
}



GsTLGridNode Superblock::find_containing_block( const GsTLPoint& p ) {
  GsTLGridNode node;
  for( int i = 0 ; i < 3; i++ ) {
    if( blocksize_[i] == 0 ) 
      node[i] = 0;
    else
      node[i] = GsTL::floor( ( p[i] - lower_left_corner_[i] ) / blocksize_[i] );
  }
  return node;
}


bool Superblock::is_valid_block( const GsTLGridNode& block ) {
  return ( block.x() >= 0 && block.x() < nx_ &&
           block.y() >= 0 && block.y() < ny_ &&
           block.z() >= 0 && block.z() < nz_ );
}



void Superblock::get_candidates( std::vector<int>& candidates, 
				                         const GsTLPoint& point ) {
  candidates.clear();
  GsTLGridNode center = find_containing_block(point); 

  for( unsigned int i=0; i < search_template_.size() ; i++ ) {
    GsTLGridNode block = center + search_template_[i];
    if( !is_valid_block( block ) ) continue;

    int block_id = cursor_->node_id( block.x(), block.y(), block.z() );
    Simple_hash_multimap::iterator_pair content =
      sblock_contents_->equal_range( block_id );

    std::copy( content.first, content.second,
               std::back_inserter( candidates ) );
  }
}

