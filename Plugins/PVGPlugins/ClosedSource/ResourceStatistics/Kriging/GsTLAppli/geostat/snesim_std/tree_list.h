/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
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
**
** Modified by Jianbing Wu, SCRF, Stanford University, June 2004
**
**********************************************************************/

#ifndef __GSTLAPPLI_Snesim_Std_TREE_LIST_H__
#define __GSTLAPPLI_Snesim_Std_TREE_LIST_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTL/cdf/categ_non_param_cdf.h>
#include <GsTL/cdf_estimator/search_tree.h>
#include <GsTL/cdf_estimator/standard_allocator.h>

#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTLAppli/utils/gstl_types.h>

#include <vector>
#include <string>


//=================================
/** Specialization for RGrid's.
*/

template< 
class UnaryFunction, 
class T=int, 
class allocator=pool_allocator<int>  >
class GEOSTAT_DECL Tree_list 
{
	//  typedef typename UnaryFunction::argument_type location;
	typedef UnaryFunction SearchTreeLocator;
	
	// public functions
 public:
	 template<class TiIterator, class GeomIterator, class ScanNbd>
		 Tree_list(TiIterator ti_begin, TiIterator ti_end,
		 ScanNbd* nbd,
		 GeomIterator geom_begin, GeomIterator geom_end,
		 SearchTreeLocator* st_loc,
		 std::vector<double>& angles, 
		 std::vector<double>& affx, 
		 std::vector<double>& affy, 
		 std::vector<double>& affz,
		 int ncoarse,
		 unsigned int nb_of_categories, 
		 int templ_size,
		 int cmin,
         std::vector< std::vector<int> >& expansion_factor ) 
	 {
		 st_loc_ = st_loc;
		 num_rot_ = int(angles.size());
		 num_aff_ = int(affx.size());
		 store_geometry(geom_begin,geom_end);
		 ncoarse_ = ncoarse;

         if ( expansion_factor.size() != 0)
             set_anisotropic_expansion_factor( expansion_factor[ncoarse-1] );
         else
             set_isotropic_expansion_factor( ncoarse );
		 
		 for(int ir =0;ir<num_rot_;ir++)
		 {
			 for(int ia =0;ia<num_aff_;ia++)
			 {
				 // creating search nbd for the current multiple grid and rotation category
				 multgrid_template(ncoarse,geom_begin,geom_end,angles[ir],affx[ia],affy[ia],affz[ia]);
				 nbd->set_geometry(geom_begin,geom_end);
				 
				 appli_message("building search tree..."  );
				 
				 // creating a vector of search tree pointers corresponding to different rotation angles
				 search_tree<T,allocator>* new_mptree = 
					 new search_tree<T,allocator>( ti_begin,ti_end,*nbd,
					 templ_size,
					 nb_of_categories, cmin );
				 GsTLcout << "Search tree size = " << new_mptree->size() << "   ";
				 mptree.push_back( new_mptree );
			 }
		 }
         GsTLcout << gstlIO::end;
	 }
	 
	 ~Tree_list()
	 {
		 for(int ist =0;ist<num_rot_*num_aff_;ist++)
			 delete mptree[ist];
		 
		 mptree.clear();
		 
		 geometry_[0].clear();
		 geometry_[1].clear();
		 geometry_[2].clear();
	 }
	 
	 template<class Geovalue_, class neighborhood, class non_param_cdf>
		 int operator()(const Geovalue_& u, neighborhood& neighbors, non_param_cdf& ccdf)
	 {
		 int coloc_angle =0;
		 if(st_loc_->first != NULL)
			 coloc_angle = static_cast<int>( (st_loc_->first)->get(u) );
		 
		 int coloc_aff =0;
		 if(st_loc_->second !=NULL)
			 coloc_aff = static_cast<int>( (st_loc_->second)->get(u) );

		 int status = (*(mptree[coloc_angle*num_aff_ + coloc_aff]))( u, neighbors, ccdf );

		 return 0;
	 }
	 
     // overloaded function also return the total number of nodes dropped
	 template<class Geovalue_, class neighborhood, class non_param_cdf>
		 int operator()(const Geovalue_& u, neighborhood& neighbors, non_param_cdf& ccdf, int& nodes_drop)
	 {
		 int coloc_angle =0;
		 if(st_loc_->first != NULL)
			 coloc_angle = static_cast<int>( (st_loc_->first)->get(u) );
		 
		 int coloc_aff =0;
		 if(st_loc_->second !=NULL)
			 coloc_aff = static_cast<int>( (st_loc_->second)->get(u) );

         // nodes_drop is the total number of nodes droped
		 nodes_drop = (*(mptree[coloc_angle*num_aff_ + coloc_aff]))( u, neighbors, ccdf );
		 return 0;
	 }

     void set_anisotropic_expansion_factor( std::vector<int>& factor)
     {
         use_anisotropic_expansion_ = true;

         spacing_x_ = factor[0];
         spacing_y_ = factor[1];
         spacing_z_ = factor[2];
     }

     void set_isotropic_expansion_factor( int mult_grid_level )
     {
         use_anisotropic_expansion_ = false;

         spacing_x_ = (int) pow(2.0, double(mult_grid_level-1) );
         spacing_y_ = spacing_x_;
         spacing_z_ = spacing_x_;
     }

 // private functions and variables
 private:
	 
	 template<class GeomIterator>
		 void multgrid_template(int mult_grid_level,GeomIterator gmit_begin,
		 GeomIterator gmit_end, double angle, double affx, double affy, double affz)
	 {
		 double theta = 3.14*angle/180.0;
		 int count = 0;

         for(GeomIterator iter = gmit_begin;iter!=gmit_end;iter++)
		 {
			 // !!!!!!!! check for colocated !!!!!!!!
			 int x,y,z;
			 x = (geometry_[0])[count];
			 y = (geometry_[1])[count];
			 z = (geometry_[2])[count];
			 x = spacing_x_*x;
			 y = spacing_y_*y;
			 z = spacing_z_*z;			// may use anisotropic expansion
			 int x1,y1,z1;
			 x1 = Nint( affx*(double(x)*cos(theta) + double(y)*sin(theta)) );
			 y1 = Nint( affy*(-double(x)*sin(theta) + double(y)*cos(theta)) );
			 z1 = Nint( affz*z );
			 (*iter).x() = x1;
			 (*iter).y() = y1;
			 (*iter).z() = z1;
			 count++;
		 }
	 }
	 
	 template<class GeomIterator> 
		 void store_geometry(GeomIterator gmit_begin, GeomIterator gmit_end)
	 {
		 geometry_[0].clear();
		 geometry_[1].clear();
		 geometry_[2].clear();
		 GeomIterator iter;
		 for(iter = gmit_begin; iter!=gmit_end; iter++)
		 {
			 int x,y,z;
			 x = (*iter).x();
			 y = (*iter).y();
			 z = (*iter).z();
			 geometry_[0].push_back(x);
			 geometry_[1].push_back(y);
			 geometry_[2].push_back(z);
		 }
	 }
	 
	 int Nint(double x)
	 {
		 return x>=0 ? static_cast<int> (x + 0.5) : static_cast<int> (x - 0.5) ;
	 }
	 
	 int ncoarse_;
	 int num_rot_;
	 int num_aff_;
	 std::vector< int> geometry_[3];
	 std::vector< search_tree<T,allocator>* > mptree;
     SearchTreeLocator* st_loc_;

     // added for anisotropic expansion
     bool use_anisotropic_expansion_;
     int spacing_x_; 
     int spacing_y_; 
     int spacing_z_; 
};

#endif
