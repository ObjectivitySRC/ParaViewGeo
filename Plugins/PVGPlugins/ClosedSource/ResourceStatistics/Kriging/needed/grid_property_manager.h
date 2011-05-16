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

#ifndef __GSTLAPPLI_GRID_PROPERTY_MANAGER_H__ 
#define __GSTLAPPLI_GRID_PROPERTY_MANAGER_H__ 
 
#include "gstl_types.h" 
 
#include <string> 
#include <vector> 
#include <map> 
#include <iostream> 
 
#include <qstring.h>
 
class GsTLGridProperty; 
class Grid_property_manager; 
 
 
/** A MultiRealization_property is a property that can have multiple  
 * realizations. This class mainly serves as an interface to create  
 * and access the different realizations.  
 * Each realization is a classical GsTLGridProperty. Each time a new 
 * realization is added, the previous one is swapped to disk. 
 */  
class MultiRealization_property { 
 public: 
  static const std::string separator; 
 
 public: 
  MultiRealization_property(); 
  MultiRealization_property( const std::string& name, 
			     Grid_property_manager* manager ); 
  MultiRealization_property( const MultiRealization_property& rhs ); 
  MultiRealization_property& operator = ( const MultiRealization_property& rhs ); 
 
 
  GsTLGridProperty* new_realization(); 
 
  GsTLGridProperty* realization( int id ); 
  const GsTLGridProperty* realization( int id ) const; 
 
  int size() const { return size_; } 
 
 private: 
  std::string name_; 
  int size_; 
  Grid_property_manager* prop_manager_; 
}; 
 
 
class Const_multiRealization_property { 
 public: 
  Const_multiRealization_property( const std::string& name, 
				   const Grid_property_manager* manager ); 
  const GsTLGridProperty* realization( int id ) const; 
 
  int size() const { return size_; } 
 
 private: 
  std::string name_; 
  int size_; 
  const Grid_property_manager* prop_manager_; 
}; 
 
 
 
//========================================== 
 
/** A Grid_property_manager keeps track of multiple properties, possibly 
 * of different type. 
 * Each property is identified by an integer id and a name. Knowing the id  
 * allows faster access to the property. The id's are automatically assigned, 
 * starting from 0 for the first property. 
 */ 
class Grid_property_manager { 
 public: 
  Grid_property_manager( GsTLInt size = 0 ); 
  void set_prop_size( GsTLInt size ) { size_ = size; } 
  ~Grid_property_manager(); 
 
  /** Adds a new property 
   * @return a pointer to the new property. The pointer is 0 if add_property  
   * failed (most probably  because a property with the same name already  
   * existed ).  
   */ 
  GsTLGridProperty* add_property( const std::string& name ); 


  /** Removes a property, but does not change the properties' id: 
   * if there are 3 properties, "prop0", "prop1", and "prop2", with id 
   * respectively 0,1,2, when "prop1" is deleted, "prop0" still has id 0, 
   * and "prop2" id 2. No new property will ever have id 1. 
   */ 
  bool remove_property( const std::string& name ); 
 
  /** Retrieve a property 
   * @return 0 if the property can not be retrived 
   */ 
  inline GsTLGridProperty* get_property( const std::string& name ); 
  inline const GsTLGridProperty* get_property( const std::string& name ) const; 
  /** Retrieve a property. Overloaded to enable faster access. 
   * @return 0 if the property can not be retrived 
   */ 
  inline GsTLGridProperty* get_property( int prop_id ); 
  inline const GsTLGridProperty* get_property( int prop_id ) const; 
 
  /** Selects a property. That property becomes the default property 
   * @return 0 if the property can not be retrived 
   */ 
  inline GsTLGridProperty* select_property( const std::string& name ); 
  inline GsTLGridProperty* select_property( int prop_id ); 
  inline GsTLGridProperty* selected_property(); 
  inline const GsTLGridProperty* selected_property() const; 
 
  class Property_name_iterator; 
  Property_name_iterator names_begin() const ; 
  Property_name_iterator names_end() const ; 
   
 
  // Multi-realization properties 
  /** Creates a new multi-realization property. 
   * If the name of the new multi-realization property is already taken, 
   * "_0" is appended to the name.  
   * The function returns a pointer to the newly created property.  
   */ 
  MultiRealization_property* new_multireal_property( const std::string& name ); 
 
  /** Retrieve a multi-realization property from its name. 
   * Returns a pointer to the requested property, or 0 if no property 
   * could be retrieved. 
   */ 
  MultiRealization_property* multireal_property( const std::string& name ); 
   
 //TL modified
  bool reNameProperty(std::string, std::string);

protected:
  bool could_conflict( const std::string& name ); 
 
 protected: 
  typedef std::map< std::string, int > Property_map; 
 
  GsTLInt size_; 
  std::map< std::string, int > properties_map_; 
  std::vector< GsTLGridProperty* > properties_; 
  int selected_property_; 
 
  typedef std::map< std::string, MultiRealization_property > MultirealProperty_map; 
  MultirealProperty_map multireal_properties_; 
 
 private: 
  Grid_property_manager( const Grid_property_manager& rhs ); 
  Grid_property_manager& operator = ( const Grid_property_manager& rhs ); 

}; 
 
 
 
class Grid_property_manager::Property_name_iterator { 
private: 
  typedef std::map< std::string, int >::const_iterator const_iterator; 
 
 public: 
  Property_name_iterator( const_iterator it ) : curr_(it) {} 
  Property_name_iterator& operator++(int) { ++curr_; return *this; } 
  Property_name_iterator& operator++()    { curr_++; return *this; } 
  Property_name_iterator& operator--(int) { --curr_; return *this; } 
  Property_name_iterator& operator--()    { curr_--; return *this; } 
  bool operator!=(const Property_name_iterator& rhs) { return rhs.curr_ != curr_; } 
  bool operator==(const Property_name_iterator& rhs) { return rhs.curr_ == curr_; } 
  const std::string& operator*() { return curr_->first; } 
  const std::string* operator->() { return &curr_->first; } 
 
 private: 
  const_iterator curr_; 
}; 
 
 
//================================================= 
//   Definition of inline functions 
 
 
inline GsTLGridProperty*  
Grid_property_manager::get_property( const std::string& name ) { 
  Property_map::iterator it = properties_map_.find(name); 
  if( it != properties_map_.end() )  
    return properties_[ it->second ]; 
  else 
    return 0; 
} 
 
inline const GsTLGridProperty*  
Grid_property_manager::get_property( const std::string& name ) const { 
  if( name.empty() ) return 0; 
 
  Property_map::const_iterator it = properties_map_.find(name); 
  if( it != properties_map_.end() )  
    return properties_[ it->second ]; 
  else 
    return 0; 
} 
 
inline GsTLGridProperty*  
Grid_property_manager::get_property( int prop_id ) { 
  //appli_assert( prop_id < (int) properties_.size() ); 
  return properties_[prop_id]; 
} 
 
inline const GsTLGridProperty*  
Grid_property_manager::get_property( int prop_id ) const { 
  //appli_assert( prop_id < (int) properties_.size() ); 
  return properties_[prop_id]; 
} 
 
inline GsTLGridProperty*  
Grid_property_manager::select_property( const std::string& name ) { 
  if( name.empty() ) return 0; 
 
  Property_map::iterator it = properties_map_.find(name); 
  if( it != properties_map_.end() ) { 
    selected_property_ = it->second; 
    return properties_[ it->second ]; 
  } 
  else 
    return 0; 
} 
 
inline GsTLGridProperty*  
Grid_property_manager::select_property( int prop_id ) { 
  //appli_assert( prop_id < (int) properties_.size() ); 
  selected_property_ = prop_id; 
  return properties_[prop_id]; 
} 
 
inline GsTLGridProperty*  
Grid_property_manager::selected_property() { 
  if( selected_property_ >=0 ) 
    return properties_[ selected_property_ ] ; 
  else 
    return 0; 
} 
 
inline const GsTLGridProperty*  
Grid_property_manager::selected_property() const { 
  if( selected_property_ >=0 ) 
    return properties_[ selected_property_ ] ; 
  else 
    return 0; 
} 
 
inline  
Grid_property_manager::Property_name_iterator 
Grid_property_manager::names_begin() const { 
  return Property_name_iterator( properties_map_.begin() ); 
} 
 
inline   
Grid_property_manager::Property_name_iterator 
Grid_property_manager::names_end() const { 
  return Property_name_iterator( properties_map_.end() ); 
} 
   
 
#endif 
