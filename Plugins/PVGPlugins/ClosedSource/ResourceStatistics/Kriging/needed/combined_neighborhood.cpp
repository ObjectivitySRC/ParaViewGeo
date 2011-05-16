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

#include "combined_neighborhood.h"

#include <GsTL/geometry/geometry_algorithms.h>
#include <GsTL/math/math_functions.h>

#include <algorithm>


// Compares 2 geovalues g1, g2:
// g1 < g2 if the covariance between g1 and the center is higher.
// The "center" is a given reference point
class Geovalue_covariance_comparator {
public:
  typedef Geovalue::location_type location_type;

  Geovalue_covariance_comparator( const location_type& center, 
    const Covariance<location_type>& cov ) 
    : center_( center ), cov_( cov ) {}

  // g1 < g2 if the covariance between g1 and the center is higher
  bool operator() ( const Geovalue& g1, const Geovalue& g2 ) const {
    return cov_( g1.location(), center_ ) > cov_(g2.location(), center_ );
  }

private:
  location_type center_;
  const Covariance<location_type>& cov_; 
};


// Compares 2 geovalues g1, g2:
// g1 < g2 if the euclidean distance between g1 and the center is smaller.
// The "center" is a given reference point
class Geovalue_comparator {
public:
  typedef Geovalue::location_type location_type;

  Geovalue_comparator( const location_type& center ) 
    : center_( center ) {}

  // g1 < g2 if the covariance between g1 and the center is higher
  bool operator() ( const Geovalue& g1, const Geovalue& g2 ) const  {
    return square_euclidean_distance( g1.location(), center_ ) < 
           square_euclidean_distance(g2.location(), center_ );
  }

private:
  location_type center_;
};

// Compares the location of geovalues g1, with center:
// g1.location == center return true
// the "center" is a given reference point
class Location_comparator {
public:
  typedef Geovalue::location_type location_type;

  Location_comparator( const location_type& center ) 
    : center_( center ) {}

  // g1 < g2 if the covariance between g1 and the center is higher
  bool operator() (const Geovalue& g1 ) const {
    location_type g1_loc = g1.location();
   bool is_same_loc = GsTL::equals( center_.x(),g1_loc.x() ) 
   && GsTL::equals( center_.y(),g1_loc.y() )
      && GsTL::equals( center_.z(),g1_loc.z() );
    return is_same_loc;
  }

private:
  location_type center_;
};


//============================================

Combined_neighborhood::
Combined_neighborhood( SmartPtr<Neighborhood> first,
                       SmartPtr<Neighborhood> second,
                       const Covariance<location_type>* cov )
  : first_( first ), second_(second) {
  max_size_ = first_->max_size() + second_->max_size();

  cov_ = 0;
  if( cov ) 
    cov_ = new Covariance<location_type>(*cov);
}

Combined_neighborhood::~Combined_neighborhood() {
  delete cov_;
}


bool Combined_neighborhood::
select_property( const std::string& prop_name ) {
  return second_->select_property( prop_name );
}

const GsTLGridProperty* Combined_neighborhood::
selected_property() const {
  return second_->selected_property();
}


void Combined_neighborhood::includes_center( bool on ) {
  first_->includes_center( on );
  second_->includes_center( on );
}


void Combined_neighborhood::find_neighbors( const Geovalue& center ) {
  neighbors_.clear();
  center_ = center;

  /* this version was not good
  first_->find_neighbors( center );
  if( first_->size() < max_size_ ) {
    second_->max_size( max_size_ - first_->size() );
    second_->find_neighbors( center );
  }
  
  neighbors_.resize( first_->size() + second_->size() );
  iterator end = std::copy( first_->begin(), first_->end(), neighbors_.begin() );
  std::copy( second_->begin(), second_->end(), end );
  */

  // ask both neighborhoods to search neighbors
  first_->find_neighbors( center );
  second_->find_neighbors( center );

  neighbors_.resize( first_->size() + second_->size() );
  // select those that are closest to "center" from both neighborhoods
  if( cov_ )
    std::merge( first_->begin(), first_->end(), 
                second_->begin(), second_->end(), 
                neighbors_.begin(), 
                Geovalue_covariance_comparator(center.location(), *cov_)  );
  else
    std::merge( first_->begin(), first_->end(), 
                second_->begin(), second_->end(), 
                neighbors_.begin(), Geovalue_comparator(center.location() )  );

  if( neighbors_.size() > max_size_ )
    neighbors_.erase( neighbors_.begin() + max_size_, neighbors_.end() );
}



void Combined_neighborhood::max_size( int s ) {
  max_size_ = s;
  if( first_->max_size() > s ) {
    first_->max_size(s);
  }
}


/* A problem arises with this function because Combined_neighborhood mixes
* geovalues that can refer to two different grids. We want each geovalue
* in range [begin,end) to be inserted into the neighborhood and made to
* refer to the property either "first_" or "second_" works with.
* 
*/
void Combined_neighborhood::
set_neighbors( const_iterator begin, const_iterator end ) {
  neighbors_.clear();

  first_->set_neighbors( begin, end );
  second_->set_neighbors( begin, end );

  neighbors_.resize( first_->size() + second_->size() );
  iterator copy_end = 
    std::copy( first_->begin(), first_->end(), neighbors_.begin() );
  std::copy( second_->begin(), second_->end(), copy_end );
}


/* Derived class 
 *
*/

Combined_neighborhood_dedup::Combined_neighborhood_dedup(
                         SmartPtr<Neighborhood> first, 
			                   SmartPtr<Neighborhood> second,
                         const Covariance<location_type>* cov,
                         bool override_first)
   :Combined_neighborhood(first,second,cov),override_first_(override_first) {};


void Combined_neighborhood_dedup::find_neighbors( const Geovalue& center ){
  neighbors_.clear();
  center_ = center;

  // ask both neighborhoods to search neighbors
  first_->find_neighbors( center );
  second_->find_neighbors( center );
 
  // remove the colocated neighbors
  iterator end_1st = first_->end();
  iterator end_2nd = second_->end();
  if(!first_->is_empty() && !second_->is_empty()) {
    if(override_first_ ) {
      for(iterator it= second_->begin(); it!=second_->end(); ++it ){
        iterator new_last = std::remove_if(first_->begin(),end_1st,
                                  Location_comparator(it->location()) );
        end_1st = new_last;
      }
      
    } else {
      for(iterator it= first_->begin(); it!=first_->end(); ++it ){
        iterator new_last = std::remove_if(second_->begin(),end_2nd,
                                  Location_comparator(it->location()) );
        end_2nd = new_last;
      }
    }
  }

  //neighbors_.resize( first_->size() + second_->size() );
  neighbors_.resize( std::distance(first_->begin(),end_1st) 
                      + std::distance(second_->begin(),end_2nd) );
  // select those that are closest to "center" from both neighborhoods
  if( cov_ )
    std::merge( first_->begin(), end_1st, 
                second_->begin(), end_2nd, 
                neighbors_.begin(), 
                Geovalue_covariance_comparator(center.location(), *cov_)  );
  else
    std::merge( first_->begin(), end_1st, 
                second_->begin(), end_2nd, 
                neighbors_.begin(), Geovalue_comparator(center.location() )  );


  if( neighbors_.size() > max_size_ )
    neighbors_.erase( neighbors_.begin() + max_size_, neighbors_.end() );
}




   
