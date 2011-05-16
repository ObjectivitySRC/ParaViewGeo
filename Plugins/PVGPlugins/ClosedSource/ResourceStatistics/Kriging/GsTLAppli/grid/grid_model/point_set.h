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

#ifndef __GSTLAPPLI_POINTSET_H__ 
#define __GSTLAPPLI_POINTSET_H__ 
 
 
 
#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h> 
#include <GsTLAppli/math/gstlpoint.h> 
#include <GsTLAppli/grid/grid_model/geostat_grid.h> 
#include <GsTLAppli/grid/grid_model/grid_property_manager.h> 
 
#include <string> 
#include <vector> 
 
#include <qstring.h>

class GsTLGridProperty; 
class Point_set_neighborhood; 
 
 
 
class GRID_DECL Point_set : public Geostat_grid { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  //typedef Gval_iterator<LinearMapIndex> iterator; 
  Point_set(){} 
  Point_set(int size); 
   
  virtual ~Point_set(); 
   
  void point_locations( const std::vector<location_type>& locations ) { 
    point_loc_ = locations; 
  } 
  const std::vector<location_type>& point_locations() const { return point_loc_;} 
 
  // Returns the most specific name of the current class 
  virtual std::string classname() const { return "Point_set"; } 


  //----------------------------
  // Properties management 
  virtual GsTLGridProperty* add_property( const std::string& name ); 
 
 
  virtual bool remove_property( const std::string& name ); 
  
 
  /** Selects one of the properties of the grid. That property becomes 
   * the property the grid operates on by default: iterators returned 
   * by begin(), end() will iterate on the default property. 
   */ 
  virtual GsTLGridProperty* select_property( const std::string& name ); 
  virtual const GsTLGridProperty* selected_property() const; 
  virtual GsTLGridProperty* selected_property(); 
 
  virtual const GsTLGridProperty* property( const std::string& name ) const; 
  virtual GsTLGridProperty* property( const std::string& name ); 
 
  virtual std::list<std::string> property_list() const ; 
 
  virtual MultiRealization_property*  
    add_multi_realization_property( const std::string& name ); 
 
 //--------------------------- 
  // Region management 
 
  virtual bool add_region( const std::string& region_name ) { 
    appli_warning("Not implemented"); return false; 
  } 
  virtual bool remove_region( const std::string& region_name ) { 
    appli_warning("Not implemented"); return false; 
  } 
 
  /** Selects a region. After calling this function, any operation 
   * that can be restricted to a region will only operate on the 
   * selected region. For example, an iterator returned by begin() 
   * will only iterate on the selected region. 
   */ 
  virtual bool select_region( const std::string& region_name ) { 
    appli_warning("Not implemented");return false; 
  } 
 
  //-------------------------------- 
  // neighborhood definition 
  virtual Neighborhood* neighborhood( double x, double y, double z, 
                                      double ang1, double ang2, double ang3, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false ); 
 
  virtual Neighborhood* neighborhood( const GsTLTripletTmpl<double>& dim, 
                                      const GsTLTripletTmpl<double>& angles, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false ); 
 
 
   
 
  //---------------------------  
  // iterators 
 
  /** returns an iterator to the begining of the current region, 
   * iterating on the current selected property. 
   */ 
  virtual iterator begin( GsTLGridProperty* prop = 0 ); 
  virtual iterator end( GsTLGridProperty* prop = 0 ); 
  virtual const_iterator begin( const GsTLGridProperty* prop = 0 ) const; 
  virtual const_iterator end( const GsTLGridProperty* prop = 0 ) const ; 
 
  virtual void init_random_path( bool from_scratch = true );
  virtual random_path_iterator random_path_begin( GsTLGridProperty* prop = 0 );
  virtual random_path_iterator random_path_end( GsTLGridProperty* prop = 0 );
 
 
  /** Computes the location of a node, given its node_id. 
   */ 
  virtual location_type location( int node_id ) const; 
 
  virtual GsTLInt node_id( GsTLInt index ) const { return index; } 
   
  virtual GsTLInt size() const {return point_loc_.size();} 
 
  Geovalue geovalue(int node_id); 
 
  virtual GsTLInt closest_node( const location_type& P ) { 
    appli_warning( "NOT YET IMPLEMENTED"); 
    return -1; 
  } 
 
   
  //TL modified
  virtual bool reNameProperty(std::string , std::string);

 private: 
  std::vector<location_type> point_loc_; 
  Grid_property_manager point_prop_; 
  
  std::vector<GsTLInt> grid_path_; 
 
 private: 
  Point_set( const Point_set& ); 
  Point_set& operator = ( const Point_set& ); 
};  
 
 
 
 
//========================= 
 
inline 
Geovalue Point_set:: geovalue(int node_id) 
{ 
      Geovalue temp(this,point_prop_.selected_property(),node_id); 
      return temp; 
} 
 
inline 
GsTLGridProperty* Point_set::select_property( const std::string& name ) 
{ 
   GsTLGridProperty* prop = point_prop_.select_property( name ); 
   return prop; 
} 
 
 
inline const GsTLGridProperty* Point_set::selected_property() const { 
  return point_prop_.selected_property(); 
} 
 
inline GsTLGridProperty* Point_set::selected_property() { 
  return point_prop_.selected_property(); 
} 
 
 
inline 
Point_set::location_type Point_set::location( int node_id ) const { 
     appli_assert( node_id >= 0 && node_id <(signed int)( point_loc_.size()) ); 
     return point_loc_[node_id]; 
  } 
 
 
 
 
inline 
const GsTLGridProperty* Point_set::property( const std::string& name ) const 
{ 
    return point_prop_.get_property( name ); 
} 
 
 
inline 
GsTLGridProperty* Point_set::property( const std::string& name )  
{ 
    return point_prop_.get_property( name ); 
} 
 
 
 
 
inline 
Point_set::iterator Point_set::begin( GsTLGridProperty* prop ){ 
  GsTLGridProperty* property = prop;
  if( !prop )
    property = point_prop_.selected_property();

  return iterator( this, property, 
		               0,point_loc_.size(), LinearMapIndex() ); 
} 
inline 
Point_set::iterator Point_set::end( GsTLGridProperty* prop ){ 
  GsTLGridProperty* property = prop;
  if( !prop )
    property = point_prop_.selected_property();

  return iterator( this, property, 
              		 point_loc_.size(), point_loc_.size(), LinearMapIndex() ); 
} 
 

inline 
Point_set::const_iterator Point_set::begin( const GsTLGridProperty* prop ) const { 
  const GsTLGridProperty* property = prop;
  if( !prop )
    property = point_prop_.selected_property();

  return const_iterator( this, property, 
		  0,point_loc_.size(), 
                   LinearMapIndex() ); 
} 
inline 
Point_set::const_iterator Point_set::end( const GsTLGridProperty* prop ) const { 
  const GsTLGridProperty* property = prop;
  if( !prop )
    property = point_prop_.selected_property();

  return const_iterator( this, property, 
	                    	 point_loc_.size(),point_loc_.size(), 
                         LinearMapIndex() ); 
} 

#endif 
