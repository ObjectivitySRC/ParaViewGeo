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

#include "rgrid_neighborhood.h"
#include "rgrid.h"
#include "random_numbers.h"

#include <GsTL/math/math_functions.h>



Named_interface* create_Rgrid( std::string& ) {
  return new RGrid;
}

//=======================================================


//TL modified
bool RGrid::reNameProperty(std::string oldName, std::string newName)
{
	return property_manager_.reNameProperty(oldName, newName);
}

RGrid::RGrid( ) 
  :  geom_(0),
     topology_(0),
     topology_is_updated_(false), property_manager_(),
     accessor_(0) {
}

RGrid::~RGrid() {
    delete geom_;
    delete topology_;
    delete accessor_;
}


void RGrid::set_geometry(RGrid_geometry* geom) {
  if( geom_ != geom ) {
    delete geom_;
    geom_ = geom->clone();
    topology_is_updated_ = false;
  }
  grid_cursor_ = SGrid_cursor(nx(), ny(), nz(), 1);
  property_manager_.set_prop_size( geom->size() );
}

bool RGrid::select_region(const std::string& region_name) {
  return true;
}


void RGrid::set_accessor(RGrid_gval_accessor* accessor) {
  if(accessor_ != accessor) {
    delete accessor_;
    accessor_ = accessor;
  }
}


GsTLGridProperty* RGrid::add_property( const std::string& name ) {
				
  return property_manager_.add_property( name );
}

bool RGrid::remove_property( const std::string& name ) {
  return property_manager_.remove_property( name );
}

GsTLGridProperty* RGrid::select_property(const std::string& prop_name) {
  GsTLGridProperty* prop = property_manager_.select_property( prop_name );

  if (accessor_) delete accessor_;

  if( prop )
    accessor_ = new RGrid_gval_accessor(this, prop);
  else
    accessor_ = 0;

  return prop;
}



std::list<std::string> RGrid::property_list() const {

  std::list<std::string> result;

  Grid_property_manager::Property_name_iterator it = 
    property_manager_.names_begin();
  Grid_property_manager::Property_name_iterator end = 
    property_manager_.names_end();
  for( ; it != end ; ++it )
    result.push_back( *it );

  return result;
}


MultiRealization_property* 
RGrid::add_multi_realization_property( const std::string& name ) {
  return property_manager_.new_multireal_property( name );
}

 
Neighborhood* RGrid::neighborhood( double x, double y, double z,
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
    return new Rgrid_ellips_neighborhood_hd( this, 
					     property_manager_.selected_property(),
					     nx,ny,nz, ang1,ang2,ang3,
					     20, cov_copy );
  else
    return new Rgrid_ellips_neighborhood( this, 
					  property_manager_.selected_property(),
					  nx,ny,nz, ang1,ang2,ang3,
					  20, cov_copy );

  delete cov_copy;
}



Neighborhood* RGrid::neighborhood( const GsTLTripletTmpl<double>& dim,
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
    return new Rgrid_ellips_neighborhood_hd( this, 
					     property_manager_.selected_property(),
					     nx,ny,nz,
					     angles[0], angles[1], angles[2],
					     20, cov_copy );
  else
    return new Rgrid_ellips_neighborhood( this, 
					  property_manager_.selected_property(),
					  nx,ny,nz,
					  angles[0], angles[1], angles[2],
					  20, cov_copy );
}

Window_neighborhood* RGrid::window_neighborhood( const Grid_template& templ) {
  return new Rgrid_window_neighborhood( templ, this,
					property_manager_.selected_property() );
}
  

void RGrid::init_random_path( bool from_scratch ) {
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
