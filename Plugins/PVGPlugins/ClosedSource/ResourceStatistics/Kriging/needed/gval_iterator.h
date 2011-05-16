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

#ifndef __GSTLAPPLI_GRID_GVAL_ITERATORS_H__ 
#define __GSTLAPPLI_GRID_GVAL_ITERATORS_H__ 
 
 
#include "geovalue.h" 
#include "grid_property.h"
#include "geostat_grid.h"
 
#include <vector> 

class LinearMapIndex { 
public: 
    LinearMapIndex(){} 
    ~LinearMapIndex(){} 
    LinearMapIndex( const LinearMapIndex& ) {} 
    inline GsTLInt map( GsTLInt i) { 
        return i; 
    } 
}; 
 
/* Should be very careful when using this class. 
 * vec_ is a vector*, not a vector. The vector vec_ points to 
 * is not owned by TabularMapIndex, and if it disappears before 
 * TabularMapIndex, we have a big problem. 
 */ 
class TabularMapIndex { 
public: 
    TabularMapIndex() : vec_(0) {} 
    TabularMapIndex(const std::vector<GsTLInt>* vec) : vec_(vec){} 
    TabularMapIndex(const TabularMapIndex& t ) : vec_(t.vec_){} 
    ~TabularMapIndex() {} 
    inline GsTLInt map( GsTLInt i) { 
        return (*vec_)[i]; 
    } 
private: 
    const std::vector<GsTLInt>* vec_; 
}; 
 
 
 
 
/** Gval_iterator is an iterator on a set of geovalues. Note that it visits
* every geovalue in the set, whether the geovalue is informed or not.
* Gval_iterator models the ForwardIterator concept of the STL and the 
* GeovalueIterator concept of GsTL.
*/
template < class MapIndex > 
class Gval_iterator { 
 
  //-------------- 
  // STL requirement for an iterator 
 public: 
  typedef std::forward_iterator_tag iterator_category; 
  typedef Geovalue value_type; 
  typedef int difference_type; 
  typedef Geovalue* pointer; 
  typedef Geovalue& reference; 
   
  //-------------- 
 
 public: 
  Gval_iterator(); 
  Gval_iterator( Geostat_grid* grid, GsTLGridProperty* prop, 
		 int index, int max_index, MapIndex map ); 
  Gval_iterator( const Gval_iterator<MapIndex>& it ); 
 
   
  void init( Geostat_grid* grid, GsTLGridProperty* prop, 
	     int index, int max_index, MapIndex map ); 
 
 
 public: 
   
  Gval_iterator<MapIndex>& operator=( const Gval_iterator<MapIndex>& it ); 
 
  Geovalue& operator * () { return gval_; }  
  Geovalue* operator -> () {return &gval_; } 
 
  inline Gval_iterator<MapIndex>& operator ++ () ; 
  inline Gval_iterator<MapIndex>& operator ++ ( int ) ; 
  int operator - ( const Gval_iterator& it ) const { return index_ - it.index_; } 
  bool operator != ( const Gval_iterator& it ) const { return index_ != it.index_; } 
  bool operator == ( const Gval_iterator& it ) const { return index_ == it.index_; } 
 
 
 private: 
  Geostat_grid* grid_; 
  int index_, max_index_; 
  MapIndex map_; 
 
  Geovalue gval_; 
}; 
 
 

template < class MapIndex > 
class Gval_const_iterator { 
 
  typedef Gval_const_iterator<MapIndex> Self;
  //-------------- 
  // STL requirement for an iterator 
 public: 
  typedef std::forward_iterator_tag iterator_category; 
  typedef Geovalue value_type; 
  typedef int difference_type; 
//  typedef const Geovalue* pointer; 
//  typedef const Geovalue& reference; 
  typedef Const_geovalue* pointer; 
  typedef Const_geovalue& reference; 
   
  //-------------- 
 
 public: 
  Gval_const_iterator(); 
  Gval_const_iterator( const Geostat_grid* grid, 
                       const GsTLGridProperty* prop, 
		                   int index, int max_index, MapIndex map ); 
  Gval_const_iterator( const Self& it ); 
 
   
  void init( const Geostat_grid* grid, const GsTLGridProperty* prop, 
	     int index, int max_index, MapIndex map ); 
 
 
 public: 
   
  Self& operator=( const Self& it ); 
 
  reference operator * () const { return gval_; }  
  pointer operator -> () { return &gval_; } 
 
  inline Self& operator ++ () ; 
  inline Self& operator ++ ( int ) ; 
  int operator - ( const Self& it ) const { return index_ - it.index_; } 
  bool operator != ( const Self& it ) const { return index_ != it.index_; } 
  bool operator == ( const Self& it ) const { return index_ == it.index_; } 
 
 
 private: 
  const Geostat_grid* grid_; 
  int index_, max_index_; 
  MapIndex map_; 
 
  Const_geovalue gval_; 
}; 
 



//=============================================
// Inline functions
 
template<class MapIndex> 
Gval_iterator<MapIndex>::Gval_iterator()  
  : grid_(0), index_(-1), max_index_(-1), map_(), gval_() { 
   
} 
 
 
template<class MapIndex> 
Gval_iterator<MapIndex>::Gval_iterator( Geostat_grid* grid, GsTLGridProperty* prop, 
					int index, int max_index, 
					MapIndex map ) 
  : grid_(grid), index_(index), max_index_( max_index ), 
    map_(map), gval_() { 
 
    if( index >=0 && index_ < max_index_ ) 
	gval_.init( grid, prop, grid_->node_id( map_.map(index_ ) ) ); 
} 
 
 
template<class MapIndex> 
Gval_iterator<MapIndex>::Gval_iterator( const Gval_iterator& it )  
  : grid_( it.grid_ ), index_( it.index_ ), max_index_( it.max_index_ ), 
    map_( it.map_ ), gval_( it.gval_ ) { 
   
} 
 
   
template<class MapIndex> 
void 
Gval_iterator<MapIndex>::init( Geostat_grid* grid, GsTLGridProperty* prop, 
			       int index, int max_index,  
			       MapIndex map ) {  
  grid_ = grid; 
  index_ = index; 
  max_index_ = max_index_; 
  map_ = map; 
  
  gval_.init( grid_, prop, grid_->node_id( map_.map(index_ ) ) ); 
} 
 
template <class MapIndex> 
Gval_iterator<MapIndex>&  
Gval_iterator<MapIndex>::operator=( const Gval_iterator<MapIndex>& it ) { 
  if( *this != it ) { 
    grid_ = it.grid_; 
    index_ = it.index_; 
    max_index_ = it.max_index_; 
    map_ = it.map_; 
 
    gval_ = it.gval_; 
  } 
  return *this; 
} 
 

template<class MapIndex> 
inline Gval_iterator<MapIndex>& 
Gval_iterator<MapIndex>::operator++() { 
  index_ ++; 
  if( index_ < max_index_ ) { 
    int id = grid_->node_id( map_.map(index_ ) ); 
    gval_.set_node_id( id ); 
  } 
  return *this; 
} 
 
template<class MapIndex> 
inline Gval_iterator<MapIndex>& 
Gval_iterator<MapIndex>::operator++( int ) { 
  index_ ++; 
  if( index_ < max_index_ ) { 
    int id = grid_->node_id( map_.map(index_ ) ); 
    gval_.set_node_id( id ); 
  } 
  return *this; 
} 
 


//=================================

template<class MapIndex> 
Gval_const_iterator<MapIndex>::Gval_const_iterator()  
  : grid_(0), index_(-1), max_index_(-1), map_(), gval_() { 
   
} 
 
 
template<class MapIndex> 
Gval_const_iterator<MapIndex>::
Gval_const_iterator( const Geostat_grid* grid, const GsTLGridProperty* prop, 
            				int index, int max_index, 
					          MapIndex map ) 
  : grid_(grid), index_(index), max_index_( max_index ), 
    map_(map), gval_() { 
 
    if( index >=0 && index_ < max_index_ ) 
	gval_.init( grid, prop, grid_->node_id( map_.map(index_ ) ) ); 
} 
 
 
template<class MapIndex> 
Gval_const_iterator<MapIndex>::
Gval_const_iterator( const Gval_const_iterator& it )  
  : grid_( it.grid_ ), index_( it.index_ ), max_index_( it.max_index_ ), 
    map_( it.map_ ), gval_( it.gval_ ) { 
   
} 
 
   
template<class MapIndex> 
void 
Gval_const_iterator<MapIndex>::
init( const Geostat_grid* grid, const GsTLGridProperty* prop, 
      int index, int max_index,  
      MapIndex map ) {  
  grid_ = grid; 
  index_ = index; 
  max_index_ = max_index_; 
  map_ = map; 
  
  gval_.init( grid_, prop, grid_->node_id( map_.map(index_ ) ) ); 
} 
 
template <class MapIndex> 
Gval_const_iterator<MapIndex>&  
Gval_const_iterator<MapIndex>::operator=( const Gval_const_iterator<MapIndex>& it ) { 
  if( *this != it ) { 
    grid_ = it.grid_; 
    index_ = it.index_; 
    max_index_ = it.max_index_; 
    map_ = it.map_; 
 
    gval_ = it.gval_; 
  } 
  return *this; 
} 
 

template<class MapIndex> 
inline Gval_const_iterator<MapIndex>& 
Gval_const_iterator<MapIndex>::operator++() { 
  index_ ++; 
  if( index_ < max_index_ ) { 
    int id = grid_->node_id( map_.map(index_ ) ); 
    gval_.set_node_id( id ); 
  } 
  return *this; 
} 
 
template<class MapIndex> 
inline Gval_const_iterator<MapIndex>& 
Gval_const_iterator<MapIndex>::operator++( int ) { 
  index_ ++; 
  if( index_ < max_index_ ) { 
    int id = grid_->node_id( map_.map(index_ ) ); 
    gval_.set_node_id( id ); 
  } 
  return *this; 
} 
 



//===========================================

template< class GeovalueIterator >
struct Is_not_informed {
  typedef GeovalueIterator argument_type;
  typedef bool result_type;

  bool operator () ( const GeovalueIterator& it ) {
    return !( it->is_informed() );
  }
};
 
template< class GeovalueIterator >
struct Is_not_harddata {
  typedef GeovalueIterator argument_type;
  typedef bool result_type;

  bool operator () ( const GeovalueIterator& it ) {
    return !( it->is_harddata() );
  }
};
 
#endif  
 
