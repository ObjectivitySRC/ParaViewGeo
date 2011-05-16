/**********************************************************************
** Author: Jianbing Wu, Alexandre Boucher
** Contributor: Tuanfeng Zhang
**
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Stanford Geostatistical 
** Earth Modeling Software (SGEMS)
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



#ifndef _filtersim__filters_H
#define _filtersim__filters_H

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <GsTL/math/math_functions.h>

#include "is_categorical.h"

#include <vector>
#include <set>
#include <string>
#include <cmath>
#include <numeric>

using namespace std;

//#define max(X,Y) ( X > Y ? X : Y )

/*
 * class Filter
 * is the base filter class for both categorical and continuous variables
 */
class GEOSTAT_DECL Filter
{
public:
    Filter(int nxdt, int nydt, int nzdt)
                :nxdt_(nxdt),nydt_(nydt),nzdt_(nzdt){}
    virtual ~Filter();

    // return the searching template geometry
    Grid_template* get_window_geometry(int ncoarse=1);
    Grid_template* get_window_geometry(int ncoarse, int nx, int ny, int nz);
    Grid_template* get_dual_window_geometry(int ncoarse, int nx, int ny, int nz);

    int get_total_filter_number() { return nfilter_; }
    string get_filter_name(int cur_filter) { return filter_name[cur_filter]; }

    // return the score load weights for a certain filter
    vector< float >& get_weights(int cur_filter){ return weight[cur_filter]; }

    // return the weight associated with all filters
    vector< float >& get_filter_weights(){ return filter_weight; }

protected:
	int nfilter_;

    vector< vector<float> > weight; // weight assign to each node pixel
    vector< float > filter_weight;  // weight assign to each filter score
    vector< string > filter_name;

    int nxdt_;
    int nydt_;
    int nzdt_;

    Grid_template* offset;
};


/*
 * class Filters_default
 * create the default filters automatically:
 *      6 filters in 2D for each categorical facies or a continuous variable
 *      9 filters in 3D for each categorical facies or a continuous variable
 */
class GEOSTAT_DECL Filters_default: public Filter 
{
public:
    Filters_default(int nxdt, int nydt, int nzdt, int nfilter);
    virtual ~Filters_default(){}
    
private:
    //void set_base_filter_template();

    void save_pixel_weight();
    void save_filter_name();
    void save_filter_weight();
    
    // create average filters 
    vector<float> filter_average_NS();  // north-south direction
    vector<float> filter_average_EW();  // east-west direction
    vector<float> filter_average_TB();  // top-bottom direction
    
    // create gradient filters 
    vector<float> filter_gradient_NS();
    vector<float> filter_gradient_EW();
    vector<float> filter_gradient_TB();
    
    // create curvature filters 
    vector<float> filter_curvature_NS();
    vector<float> filter_curvature_EW();
    vector<float> filter_curvature_TB();
};


/*
 * class Filters_user_define
 * create filters according to user's request (from input data file)
 */
class GEOSTAT_DECL Filters_user_define: public Filter 
{
public:
    Filters_user_define(int nxdt, int nydt, int nzdt, string filename);
    
    virtual ~Filters_user_define(){}
};


/*
 * class Transcat_Filter_default
 * create one default filter for transcat program
 */
class GEOSTAT_DECL  Transcat_Filter_default: public Filter 
{
public:
    Transcat_Filter_default(int nxdt, int nydt, int nzdt, int nfilter=1);
    virtual ~Transcat_Filter_default(){}
    
private:
    void create_one_filter_weight();
};


#endif  // _filtersim__filters_H

