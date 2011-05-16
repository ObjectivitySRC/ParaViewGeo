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

#include <GsTLAppli/grid/grid_model/grid_property_manager.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/utils/string_manipulation.h> 

#include <stdlib.h>


const std::string MultiRealization_property::separator( "__real" );

MultiRealization_property::MultiRealization_property()
  : size_( 0 ),
    prop_manager_( 0 ) {
}

MultiRealization_property::MultiRealization_property( const std::string& name,
				  Grid_property_manager* manager )
  : name_( name ),
    prop_manager_( manager ) {
  size_ = 0;
}

MultiRealization_property::
MultiRealization_property( const MultiRealization_property& rhs ) {
  name_ = rhs.name_;
  size_ = rhs.size_;
  prop_manager_ = rhs.prop_manager_;
}

MultiRealization_property& 
MultiRealization_property::operator = ( const MultiRealization_property& rhs ) {
  name_ = rhs.name_;
  size_ = rhs.size_;
  prop_manager_ = rhs.prop_manager_;

  return *this;
}


GsTLGridProperty* MultiRealization_property::new_realization() {
  // if there was already a realization, don't keep it loaded in memory
  // and swap it to disk
  if( size_ > 0 ) {
    GsTLGridProperty* previous_real = 
      prop_manager_->get_property( name_ + separator + 
				   String_Op::to_string( size_-1 ) );
    previous_real->swap_to_disk();
  }
  
  GsTLGridProperty* new_real = 
    prop_manager_->add_property( name_ + separator +
				 String_Op::to_string( size_ ) );
  if( new_real )
    size_++;
  
  return new_real;
}

GsTLGridProperty* MultiRealization_property::realization( int id ) {
  if( id >= size_ )
    return 0;

  return prop_manager_->get_property( name_ + MultiRealization_property::separator +
                                      String_Op::to_string( id ) );
}

const GsTLGridProperty* MultiRealization_property::realization( int id ) const {
  if( id >= size_ )
    return 0;

  return prop_manager_->get_property( name_ + MultiRealization_property::separator +
                                      String_Op::to_string( id ) );

}




//=================================================

bool Grid_property_manager::reNameProperty(std::string oldName, std::string newName)
{
	if( oldName.empty() ) return false; 

	Property_map::iterator it_new = properties_map_.find(newName); 	
	Property_map::iterator it = properties_map_.find(oldName); 

	if (it_new != properties_map_.end()) return false;

	if( it != properties_map_.end() )  {
		int id = it->second;
		properties_map_.erase(it);
		properties_map_[newName] = id;
		return true; 
	}
	else 
		return false; 
}

Grid_property_manager::Grid_property_manager( GsTLInt size ) {
  size_ = size;
  selected_property_ = -1;
}

Grid_property_manager::~Grid_property_manager() {
  std::vector< GsTLGridProperty* >::iterator it = properties_.begin();
  for( ; it != properties_.end() ; ++it )
    if( *it != 0 )
      delete *it;
}


GsTLGridProperty* 
Grid_property_manager::add_property( const std::string& name ) {

  appli_assert( size_ != 0 );
  Property_map::iterator it = properties_map_.find( name );
  if( it == properties_map_.end() ) {
    int new_prop_id = properties_.size();
    properties_map_[name] = new_prop_id;
    properties_.push_back( new GsTLGridProperty( size_, name ) );

    // if no property was selected before, select the first one in the list
    if( selected_property_ == -1 )
      selected_property_ = new_prop_id;

    return properties_[ new_prop_id ];
  }
  else
    return 0 ; 
}



bool 
Grid_property_manager::remove_property( const std::string& name ) {
  Property_map::iterator it = properties_map_.find( name );
  if( it != properties_map_.end() ) {
    // delete the propery but don't modify the vector of property*
    // ex: if property 2 is deleted, property 3 does not become property 2,
    // but remains property 3.
    int ind = it->second;
    delete properties_[ind];
    properties_[ind] = 0;
    properties_map_.erase( it );
    return true;
  }
  else
    return false;
}


MultiRealization_property* 
Grid_property_manager::new_multireal_property( const std::string& name ) {
  // Make sure the requested name does not conflict with another one
  // If it does, append "_0" to the requested name
  const std::string suffix = MultiRealization_property::separator;

  std::string new_name = name;
  std::string conflict = new_name + suffix;
  while( could_conflict( conflict ) ) { //properties_map_.find( conflict ) != properties_map_.end() ) {
    new_name += "_0";
    conflict = new_name + suffix;
  }

  multireal_properties_[ new_name ] = MultiRealization_property( new_name, this );
  MultirealProperty_map::iterator found = multireal_properties_.find( new_name );
  appli_assert( found != multireal_properties_.end() );
  
  return &found->second;
}


MultiRealization_property* 
Grid_property_manager::multireal_property( const std::string& name ) {
  MultirealProperty_map::iterator found = multireal_properties_.find( name );
  if( found != multireal_properties_.end() )
    return &found->second;
  else 
    return 0;
}
  


bool Grid_property_manager::could_conflict( const std::string& name ) {
  // loop through all the string keys and check if they contain "name" as
  // a sub-string
  std::map< std::string, int >::const_iterator it = properties_map_.begin();
  for( ; it != properties_map_.end() ; ++it ) {
    if( it->first.find( name ) != std::string::npos ) return true;
  }

  return false;
}



