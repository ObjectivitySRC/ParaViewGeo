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

#ifndef __GSTLAPPLI_GRID_MODEL_NEIGHBORHOOD_H__ 
#define __GSTLAPPLI_GRID_MODEL_NEIGHBORHOOD_H__ 
 
#include "gstlvector.h"
#include "geovalue.h" 
 
#include <GsTL/utils/smartptr_interface.h> 
 
 
#include <vector> 
 
 
class Geostat_grid; 
class GsTLGridProperty; 
 
 
 
/** Neighborhood is the base class for all neighborhood types. 
 */ 
class Neighborhood : public SmartPtr_interface<Neighborhood> { 
 public: 
  typedef Geovalue value_type; 
  typedef std::vector<Geovalue>::iterator iterator; 
  typedef std::vector<Geovalue>::const_iterator const_iterator; 
   
 
 public: 
  Neighborhood() : includes_center_( true ) {}
  virtual ~Neighborhood() {} 
 
  virtual void find_neighbors( const Geovalue& center ) = 0 ; 
  virtual Geovalue center() const = 0; 
 
  /** The neighborhood will work on the selected property, ie the  
   * geovalues retrieved by find_neighbors(...) will be geovalues on 
   * property "prop_name". 
   */ 
  virtual bool select_property( const std::string& prop_name ) = 0;  
  virtual const GsTLGridProperty* selected_property() const = 0;  
 
  bool includes_center() const { return includes_center_; }
  /** If \c on is true, then the center of the neighborhood will be considered
  * when searching for neighbors: if u and v are 2 geovalues defined at the
  * same location, v working on the property selected by the neighborhood,
  * a call to \c find_neighbors(u) will result in a neighborhood that might
  * contain v if v is informed.
  */
  virtual void includes_center( bool on ) { includes_center_ = on; }
  
  iterator begin() { return neighbors_.begin(); } 
  iterator end() { return neighbors_.end(); } 
  const_iterator begin() const { return neighbors_.begin(); } 
  const_iterator end() const { return neighbors_.end(); } 
 
  virtual int size() const { return neighbors_.size(); } 
  virtual void max_size( int s ) = 0; 
  virtual int max_size() const = 0; 
 
  bool is_empty() const { return neighbors_.empty(); } 
  void clear() { neighbors_.clear(); }

  /** Set the neighbors to geovalues in range [begin,end). Geovalues of range
  * [begin,end) that are not compatible with the neighborhood are ignored 
  * (a geovalue g is deemed compatible with a neighborhood N if there exists 
  * a geovalue g' such that N.find_neighbors(g') yields a neighborhood 
  * containing g. For example, g could be incompatible with N if g and N 
  * were not refering to the same grid).
  */
  virtual void set_neighbors( const_iterator begin, const_iterator end ) = 0;

 protected: 
  std::vector<Geovalue> neighbors_; 
  bool includes_center_;
}; 
 
 
 
/** GsTL_neighborhood is a handle class for neighborhoods that complies 
 * with the GsTL Neighborhood requirements. It allows to have heterogenous 
 * containers of neighborhoods. 
 * NeighborhoodPtr is a type which behaves like a pointer to a Neighborhood 
 * (e.g. Neighborhood* or SmartPtr<Neighborhood> ).
 * GsTL_neighborhood does not perform deep copies: 
 * \code 
 *   GsTL_neighborhood<T> neigh1( ... );
 *   GsTL_neighborhood<T> neigh2 = neigh1;
 * \endcode
 * \c neigh2 and \c neigh1 both refer to the same neighborhood.
 */  
template< class NeighborhoodPtr > 
class GsTL_neighborhood {
  typedef GsTL_neighborhood< NeighborhoodPtr > Self_;

 public: 
  typedef Geovalue value_type; 
  typedef std::vector<Geovalue>::iterator iterator; 
  typedef std::vector<Geovalue>::const_iterator const_iterator; 
   
  GsTL_neighborhood( NeighborhoodPtr neighborhood = 0 )  
    : neighborhood_( neighborhood ) {} 
  GsTL_neighborhood( const Self_& rhs ) : neighborhood_( rhs.neighborhood_ ) {}
  
  void set_neighborhood_ptr( NeighborhoodPtr neighborhood ) { 
    neighborhood_ = neighborhood; 
  } 
 
  void find_neighbors( const Geovalue& center ) { 
    neighborhood_->find_neighbors( center ); 
  } 
  Geovalue center() const { return neighborhood_->center(); } 
  bool select_property( const std::string& prop_name ) { 
    return neighborhood_->select_property( prop_name ); 
  } 
  const GsTLGridProperty* selected_property() const {
    return neighborhood_->selected_property();
  }
  iterator begin() { return neighborhood_->begin(); } 
  iterator end() { return neighborhood_->end(); } 
  const_iterator begin() const { return neighborhood_->begin(); } 
  const_iterator end() const { return neighborhood_->end(); } 
 
  int size() const { return neighborhood_->size(); } 
  void max_size( int s ) { neighborhood_->max_size( s ); } 
  int max_size() const { return neighborhood_->max_size(); } 
 
  bool is_empty() const { return neighborhood_->is_empty(); } 

  void set_neighbors( const_iterator begin, const_iterator end ) {
    neighborhood_->set_neighbors( begin, end );
  }
   
 private: 
  NeighborhoodPtr neighborhood_; 
}; 
 
 
 
 
/** A collocated neighborhood consists of a single "neighbor": 
 * the collocated neighborhood of geovalue (u,p) (u is the location 
 * and p is the property value) is geovalue (u,q) (same location u). 
 */ 
class Colocated_neighborhood : public Neighborhood { 
 public: 
  Colocated_neighborhood( Geostat_grid* grid, 
			  const std::string& property_name ); 
  virtual ~Colocated_neighborhood() {}; 
  virtual bool select_property( const std::string& prop_name );  
  virtual const GsTLGridProperty* selected_property() const { return property_; }

  virtual void find_neighbors( const Geovalue& center ); 
  virtual Geovalue center() const;// { return center_; } 
 
  virtual void max_size( int s ) {} 
  virtual int max_size() const { return 1; } 

  virtual void set_neighbors( const_iterator begin, const_iterator end );
 
 private: 
  Geostat_grid* grid_; 
  GsTLGridProperty* property_;  
  //  Geovalue center_; 
  // const GsTLGridProperty* center_property_; 
}; 
 
 
 

/** A DummyNeighborhood is a neighorhood that does nothing. It is there in case 
* we have to provide a neighborhood but don't actually need it.
*/
class DummyNeighborhood : public Neighborhood { 

 public: 
  DummyNeighborhood() {}
  virtual ~DummyNeighborhood() {} 
 
  virtual void find_neighbors( const Geovalue&  ) {} ; 
  virtual Geovalue center() const { return Geovalue(); }
 
  virtual bool select_property( const std::string& ) { return true; }  
  virtual const GsTLGridProperty* selected_property() const { return 0; }  
 
  virtual void max_size( int ) {} 
  virtual int max_size() const { return 0; } 
 
  virtual void set_neighbors( const_iterator, const_iterator ) {};
}; 




//============================================ 
//  Specific to strati_grid's 
// 
 
class Grid_template { 
 public: 
  typedef GsTLGridNode::difference_type Euclidean_vector; 
  typedef std::vector<Euclidean_vector>::iterator iterator; 
  typedef std::vector<Euclidean_vector>::const_iterator const_iterator; 
   
 
  Grid_template(); 
  Grid_template( iterator begin, iterator end ); 
  Grid_template( const Grid_template& rhs ); 
 
  Grid_template& operator = ( const Grid_template& rhs ); 
 
  void init( iterator begin, iterator end ); 
 
  /** Adds an euclidean vector to the template. \c pos indicates the position
  * in the template definition where the new euclidean vector should be 
  * inserted. If \c pos is < 0 or is greater than the current size of the
  * template, the vector is added at the end of the definition. \c pos=0 means
  * the vector should be inserted at the begining.
  */
  void add_vector( GsTLInt i, GsTLInt j, GsTLInt k, int pos = -1 ); 
  void add_vector( const Euclidean_vector& vec, int pos = -1 ); 

  void remove_vector( int pos );

  void clear() { templ_.clear(); }

  GsTLInt size() { return templ_.size() ; }  
  void max_size( int s ) { max_size_ = s; } 
  int max_size() const { return max_size_; } 
 
  void set_geometry( const std::vector<Euclidean_vector>& templ );
  iterator begin() { return templ_.begin() ; } 
  iterator end();
  const_iterator begin() const { return templ_.begin() ; } 
  const_iterator end() const { return templ_.end() ; } 
   
  /** Scales each vectorof the template by factor \c s. Note that the scaling
  * is not cummulative: the result of any previous scaling is undone before
  * a new scaling is applied.
  */
  void scale( int s );

  /** Using this function is equivalent to calling function \c scale(1)
  */
  void undo_scaling();

 private: 
  std::vector<Euclidean_vector> templ_; 
  std::vector<Euclidean_vector> original_; 
  int max_size_; 
  int scale_;
}; 
 
 
 
/** A Window_neighborhood is a neighborhood defined by an arbitrary geometry 
 * (the "window", or "template"). When searching neighbors, the 
 * Window_neighborhood retrieves each geovalue inside the window, whether
 * they are informed or not. Trailing un-informed geovalues are discarded.
 * By default, the center of the neighborhood is not considered, unless
 * the window includes offset vector (0,0,0).
*/
class Window_neighborhood : public Neighborhood { 
 public: 
   Window_neighborhood() : Neighborhood() { includes_center_ = false; }
  virtual ~Window_neighborhood() {} 
 
  virtual void set_geometry( Grid_template::iterator begin,  
			                       Grid_template::iterator end ) = 0; 
  virtual void add_geometry_vector( const Grid_template::Euclidean_vector& )=0;
  virtual void clear_geometry() = 0;

  virtual Grid_template& geometry() { return geom_; } 
  virtual const Grid_template& geometry() const { return geom_; } 
    
  void includes_center( bool on ) {
    appli_warning( "it does not make much sense to use "
                   << "includes_center(...) on a window neighborhood\n"
                   << "Consider adding a vector to the geometry instead" );
  }

  virtual void find_all_neighbors( const Geovalue& center )=0;

 protected:
  Grid_template geom_;
}; 
 
#endif 
