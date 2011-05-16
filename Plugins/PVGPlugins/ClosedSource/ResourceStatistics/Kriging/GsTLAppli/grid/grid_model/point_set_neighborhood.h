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

#ifndef __GSTLAPPLI_POINT_SET_NEIGHBORHOOD_H__ 
#define __GSTLAPPLI_POINT_SET_NEIGHBORHOOD_H__ 
 
 
#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/math/gstlpoint.h> 
#include <GsTLAppli/grid/grid_model/neighborhood.h> 
#include <GsTLAppli/grid/grid_model/point_set.h> 
#include <GsTLAppli/grid/grid_model/superblock.h> 
#include <GsTLAppli/math/gstlvector.h> 
 
#include <GsTL/geometry/geometry_algorithms.h> 
#include <GsTL/geometry/covariance.h> 
#include <GsTL/utils/KDTree.h> 
 
#include <vector> 
 
//#undef USE_ANN_KDTREE
#if !defined(USE_KDTREE) && !defined(USE_ANN_KDTREE)
#define USE_SUPERBLOCKS
#endif
#define USE_KDTREE
#undef USE_ANN_KDTREE

template<class A, class B, class C>
class ANNkd_tree;


class GRID_DECL Point_set_neighborhood : public Neighborhood{ 
public:
  typedef Geovalue value_type; 
  typedef Neighborhood::iterator iterator; 
  typedef Neighborhood::const_iterator const_iterator; 
 
public: 
  typedef Point_set::location_type location_type; 
    Point_set_neighborhood(double x,double y,double z, 
			   double ang1,double ang2,double ang3, 
			   int max_neighbors, 
			   Point_set* pset,  
			   GsTLGridProperty* property, 
			   const Covariance<location_type>* cov = 0); 
 
  virtual ~Point_set_neighborhood(); 
  virtual bool select_property( const std::string& prop_name );  
  virtual const GsTLGridProperty* selected_property() const { return property_; }
 
  virtual void find_neighbors(const Geovalue& center); 
  virtual void max_size( int s );
  virtual int max_size() const { return max_neighbors_; } 
  virtual Geovalue center() const { return center_; } 
 
  virtual void set_neighbors( const_iterator begin, const_iterator end );


protected: 
  Point_set* pset_;   
  GsTLGridProperty* property_; 
 
  double a_,b_,c_,ang1_,ang2_,ang3_; 
  int max_neighbors_; 
  std::vector<int> candidates_; 
  Geovalue center_; 
 
  Anisotropic_norm_3d<GsTLCoordVector> norm_;

  typedef Anisotropic_transform_3d<location_type>  CoordTransform;
  CoordTransform* coord_transform_;

  Covariance<location_type>* cov_; 

#ifdef USE_SUPERBLOCKS
  Superblock* sblock_; 
#endif

#ifdef USE_KDTREE
  KDTree<GsTLPoint, int> kdtree_;
#endif

#ifdef USE_ANN_KDTREE
  typedef ANNkd_tree<GsTLPoint::coordinate_type, float, int> KDTree_type;
  KDTree_type* kdtree_;
  typedef GsTLPoint::coordinate_type CoordT;
  CoordT** p_coords_;
  std::vector<GsTLPoint> locs_;
  std::vector<unsigned int> node_ids_;
  int* neigh_ids_ ;
  float* dists_ ;

  void select_informed_pset_locations();
#endif
};     
 
 
 
         
#endif     
     
