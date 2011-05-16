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

#ifndef __GSTLAPPLI_GRID_MODEL_GEOSTAT_GRID_H__ 
#define __GSTLAPPLI_GRID_MODEL_GEOSTAT_GRID_H__ 
 



//#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include "gstlpoint.h" 
 
#include <typeinfo> 
#include <list> 
//#include <string>
 
class GsTLGridProperty; 
class MultiRealization_property; 
template<class R> class Gval_iterator; 
template<class R> class Gval_const_iterator; 
class LinearMapIndex; 
class TabularMapIndex; 
class Neighborhood; 
template<class Location_> class Covariance; 


 
 
/** Geostat_grid is the root of the grid-models hierarchy. It is the 
 * base object on which any geostatistics algorithm can be applied. 
 *  
 * Every node of a geostat_grid is assigned a unique id (node_id). 
 * It is possible to re-order the nodes by re-defining function node_id(...). 
 */ 
 
class Geostat_grid { 
 
 public: 
 
  typedef float property_type;  
  typedef GsTLPoint location_type; 
 
  typedef Gval_iterator<LinearMapIndex> iterator; 
  typedef Gval_const_iterator<LinearMapIndex> const_iterator; 
  typedef Gval_iterator<TabularMapIndex> random_path_iterator; 
 
  
 public: 
 
  virtual ~Geostat_grid() {} 
   
  // Returns the most specific name of the current class 
  virtual std::string classname() const { return "Geostat_grid"; } 
 
 
  //------------------------------------ 
  // Properties management 
 
  /** Adds a new property called \a name. 
   * A pointer to the new property is returned. If \a add_property(...) failed, 
   * for example because the property already existed, a null pointer 
   * is returned. 
   */ 
  virtual GsTLGridProperty* add_property( const std::string& name ) = 0;  
	 
   
  /** Removes a given property from the property list 
   * @return false if the function failed, eg because the property 
   * did not exist 
   */ 
  virtual bool remove_property( const std::string& name ) = 0; 
 
  /** Selects one of the properties of the grid. That property becomes 
   * the property the grid operates on by default: iterators returned 
   * by begin(), end() will iterate on the default property.  
   */ 
  virtual GsTLGridProperty* select_property( const std::string& name ) = 0;
  
  /** Returns a pointer to the currently selected property. To select a 
  * property, use function \a select_property( name ).
  * By default, the first property that was added to the grid is the
  * selected property.
  */
  virtual const GsTLGridProperty* selected_property() const = 0; 
  virtual GsTLGridProperty* selected_property() = 0;
  
  /** Provides direct access to property \a name. The function returns
  * a pointer to the property array. If property \a name does not exist,
  * a null pointer is returned.
  */
  virtual const GsTLGridProperty* property( const std::string& name ) const = 0; 
  virtual GsTLGridProperty* property( const std::string& name ) = 0; 

  /** Gives the list of all the names of the properties currently in the grid.
  */
  virtual std::list<std::string> property_list() const = 0; 
 
  /** Adds a multi-realization property to the grid.
  */
  virtual MultiRealization_property*  
    add_multi_realization_property( const std::string& name ) = 0; 
 
  //--------------------------- 
  // Region management 
 
  virtual bool add_region( const std::string& region_name ) = 0; 
  virtual bool remove_region( const std::string& region_name ) = 0; 
 
  /** Selects a region. After calling this function, any operation  
   * that can be restricted to a region will only operate on the 
   * selected region. For example, an iterator returned by begin() 
   * will only iterate on the selected region.  
   */ 
  virtual bool select_region( const std::string& region_name ) = 0; 
 

  //-------------------------------- 
  // neighborhood definition 

  /** Creates a new ellipsoid neighborhood. The ellipsoid is defined by the 
  * 3 affinity factors \a x, \a y, \a z, and the 3 angles (in degrees) \a ang1,
  * \a ang2, \a ang3.
  * If covariance \a cov is not 0, it will be used by the neighborhood to sort
  * the neighbors it finds: the first neighbor will be the closest to the 
  * central point u, ie the point v such that cov(u,v) is highest.
  * Set \a only_harddata to true to force the neighborhood to only consider
  * hard-data.
  * \warning It is the user's responsability to \c delete the neighborhood 
  * when it is not needed anymore.}
  */
  virtual Neighborhood* neighborhood( double x, double y, double z, 
				      double ang1, double ang2, double ang3, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false )=0 ; 

  /** Creates a new ellipsoid neighborhood. Triplet \a dim defines the affinity 
  * ratios of the ellispoid, and triplet angles the rotation angles.
  * If covariance \a cov is not 0, it will be used by the neighborhood to sort
  * the neighbors it finds: the first neighbor will be the closest to the 
  * central point u, ie the point v such that cov(u,v) is highest.
  * Set \a only_harddata to true to force the neighborhood to only consider
  * hard-data.
  * \warning It is the user's responsability to \c delete the neighborhood 
  * when it is not needed anymore.
  */
  virtual Neighborhood* neighborhood( const GsTLTripletTmpl<double>& dim, 
				      const GsTLTripletTmpl<double>& angles, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false )=0; 
  virtual Neighborhood* colocated_neighborhood( const std::string& prop ); 
 
  //---------------------------   
  // iterators 
 
  /** returns an iterator to the begining of the current region, 
   * iterating on property \a prop. If no property is specified, the iterator
   * iterated on the current selected property. 
   */ 
  virtual iterator begin( GsTLGridProperty* prop = 0 ) = 0; 
  virtual iterator end( GsTLGridProperty* prop = 0 ) = 0; 
  virtual const_iterator begin( const GsTLGridProperty* prop = 0 ) const = 0; 
  virtual const_iterator end( const GsTLGridProperty* prop = 0 ) const = 0; 

  /** Call this function before any call to \a random_path_begin(), or 
  * \a random_path_end(), to initialize the random path. 
  * The new random path is obtained by shuffling the elements of the current
  * random path. 
  * If \a from_scratch is true, the new random path is obtained by shuffling 
  * the default path (ie the path that visits node 1 first, then node 2, ...).
  */
  virtual void init_random_path( bool from_scratch = true ) = 0; 

  /** Get the beginning of the current random path. To change the random path
  * use \a init_random_path().
  */
  virtual random_path_iterator random_path_begin( GsTLGridProperty* prop = 0 ) = 0; 
  virtual random_path_iterator random_path_end( GsTLGridProperty* prop = 0 ) = 0; 
 
 
  //---------------------------- 
  // Misc. 
   
  /** Computes the location of a node, given its node_id. 
   */ 
  virtual location_type location( int node_id ) const = 0; 
 
  /** Find the node of the grid which is closest to the input point 
   * The input point (geovalue) may not be a grid node.  
   * @return the node-id of the node closest to "P" 
   */ 
  virtual GsTLInt closest_node( const location_type& P ) = 0; 
 
  /** This function returns the node-id of the index-th node. 
   * This is useful if there is a re-mapping of the node-id, ie 
   * if we want that the first node has node-id 45, instead of the  
   * default id 1. (this is especially useful for multiple grids). 
   */ 
  virtual GsTLInt node_id( GsTLInt index ) const { return index; } 
 
  /** Returns the total number of nodes in the grid.
  */
  virtual GsTLInt size() const  = 0;

  //TL modified
  virtual bool reNameProperty(std::string, std::string) = 0;
}; 
 
 
#endif 
 
