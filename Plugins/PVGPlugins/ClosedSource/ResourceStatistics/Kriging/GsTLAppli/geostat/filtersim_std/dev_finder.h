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


#ifndef __filtersim_DEV_finder_H__
#define __filtersim_DEV_finder_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>

#include <algorithm>

#include "pattern.h"
#include "prototype.h"
#include "patch_helper.h"
#include "mapping_pattern_id.h"


/*
 * class HD_DEV_Finder_Categorical
 * is used to find the (hard) data event, and its weight at a given node
 */
class GEOSTAT_DECL HD_DEV_Finder_Categorical
{
public:
    HD_DEV_Finder_Categorical( RGrid* simu_grid, Window_neighborhood* neighbors, 
                               Patch_Helper* patcher, int nb_facies, const vector< float >& data_weight):
                simu_grid_(simu_grid), neighbors_(neighbors), 
                patcher_(patcher), nb_facies_(nb_facies), data_weight_(data_weight) {}

    ~HD_DEV_Finder_Categorical(){}

    bool operator()( RGrid::random_path_iterator current_node_iter, 
                     vector< vector<float> >& dev, vector<float>& weight );
    bool operator()( RGrid::random_path_iterator current_node_iter, 
                     vector< vector<float> >& dev, vector<float>& weight, vector<int>& type );

private:
    void initialize_DEV( vector< vector<float> >& data_event, vector<float>& weight );

private:
    RGrid* simu_grid_;
    Window_neighborhood* neighbors_;
    Patch_Helper* patcher_;
    int nb_facies_;

    const vector< float > data_weight_;
	// [0]   weight_hard_data_;
	// [1]   weight_patch_data_;
	// [2]   weight_other_data_;
};


/*
 * class HD_DEV_Finder_Continuous
 * is used to find the (hard) data event, and its weight at a given node
 */
class GEOSTAT_DECL HD_DEV_Finder_Continuous
{
public:
    HD_DEV_Finder_Continuous( RGrid* simu_grid, Window_neighborhood* neighbors, 
                              Patch_Helper* patcher, const vector< float >& data_weight ):
                simu_grid_(simu_grid), neighbors_(neighbors), 
                patcher_(patcher), data_weight_(data_weight) {}

    ~HD_DEV_Finder_Continuous(){}

    bool operator()( RGrid::random_path_iterator current_node_iter, 
                     vector<float>& dev, vector<float>& weight );
    
    bool operator() ( RGrid::random_path_iterator current_node_iter, 
                    vector<float>& dev, vector<float>& weight, vector<int>& type );
private:
    void initialize_DEV( vector<float>& data_event, vector<float>& weight );

private:
    RGrid* simu_grid_;
    Window_neighborhood* neighbors_;
    Patch_Helper* patcher_;

    const vector< float > data_weight_;
	// [0]   weight_hard_data_;
	// [1]   weight_patch_data_;
	// [2]   weight_other_data_;
};



/*
 * class ST_DEV_Finder_Categorical
 * is used to find the soft data event, and its weight at a given node
 */
class GEOSTAT_DECL ST_DEV_Finder_Categorical
{
public:
    ST_DEV_Finder_Categorical(RGrid* simu_grid, Window_neighborhood* neighbors, 
                                vector<GsTLGridProperty*>& softDataProps, 
                                int nb_facies, const vector< float >& data_weight):
                    simu_grid_(simu_grid), soft_neighbors_(neighbors), 
                    softDataProps_(softDataProps), nb_facies_(nb_facies), data_weight_(data_weight) {}

    ~ST_DEV_Finder_Categorical(){}

    void operator()( RGrid::random_path_iterator current_node_iter, vector< vector<float> >& soft_dev );

private:
    void initialize_DEV( vector< vector<float> >& soft_dev );

private:
    RGrid* simu_grid_;
    Window_neighborhood* soft_neighbors_;
    vector<GsTLGridProperty*> softDataProps_;
    int nb_facies_;

    const vector< float > data_weight_;
	// [0]   weight_hard_data_;
	// [1]   weight_patch_data_;
	// [2]   weight_other_data_;
};


/*
 * class ST_DEV_Finder_Continuous
 * is used to find the soft data event, and its weight at a given node
 */
class GEOSTAT_DECL ST_DEV_Finder_Continuous
{
public:
    ST_DEV_Finder_Continuous(RGrid* simu_grid, Window_neighborhood* neighbors,
                             GsTLGridProperty* softDataProps, 
                             const vector< float >& data_weight ):
                simu_grid_(simu_grid), soft_neighbors_(neighbors), 
                softDataProps_(softDataProps), data_weight_(data_weight) {}

    ~ST_DEV_Finder_Continuous(){}

    void operator()( RGrid::random_path_iterator current_node_iter, vector<float>& soft_dev );

private:
    void initialize_DEV( vector<float>& soft_dev );

private:
    RGrid* simu_grid_;
    Window_neighborhood* soft_neighbors_;
    GsTLGridProperty* softDataProps_;

    const vector< float > data_weight_;
	// [0]   weight_hard_data_;
	// [1]   weight_patch_data_;
	// [2]   weight_other_data_;
};


// --------------------------------------------------
/*
 * class HD_DEV_Finder_Categorical_Score_based
 * is used to find the data event based on score distance
 */
class GEOSTAT_DECL DEV_Finder_Categorical_Score_based
{
public:
    DEV_Finder_Categorical_Score_based( RGrid* simu_grid, 
                                                                 Window_neighborhood* neighbors, 
                                                                 Filter* my_filters, 
                                                                 Pattern_Node_Map& mapping, 
                                                                 int nb_facies,
                                                                 vector<float>& max_value, 
                                                                 vector<float>& min_value,
                                                                 int treat_cate_as_cont=0 );
    ~DEV_Finder_Categorical_Score_based(){}

    bool operator()( RGrid::random_path_iterator current_node_iter, vector<float>& dev_score, int& pattern_id);

private:
    RGrid* simu_grid_;
    Window_neighborhood* neighbors_;
    SGrid_cursor* cursor_;
    Pattern_Node_Map mapping_;           // mapping table to record pattern location

    vector<float> max_value_;        // the max. value of the filter score
    vector<float> min_value_;         // the min. value of the filter score
    vector<float> multiplier_;

    int nb_filter_;
    int nb_facies_;
    int treat_cate_as_cont_;
    int total_score_;

    vector< vector< float > > weight_;     // filter weights
};


/*
 * class HD_DEV_Finder_Categorical_Score_based
 * is used to find the data event based on score distance
 */
class GEOSTAT_DECL DEV_Finder_Continuous_Score_based
{
public:
    DEV_Finder_Continuous_Score_based( RGrid* simu_grid, 
                                                                  Window_neighborhood* neighbors, 
                                                                  Filter* my_filters, 
                                                                  Pattern_Node_Map& mapping, 
                                                                  vector<float>& max_value, 
                                                                  vector<float>& min_value );
    ~DEV_Finder_Continuous_Score_based(){}

    bool operator()( RGrid::random_path_iterator current_node_iter, vector<float>& dev_score, int& pattern_id);

private:
    RGrid* simu_grid_;
    Window_neighborhood* neighbors_;
    SGrid_cursor* cursor_;
    Pattern_Node_Map mapping_;           // mapping table to record pattern location

    vector<float> max_value_;        // the max. value of the filter score
    vector<float> min_value_;         // the min. value of the filter score
    vector<float> multiplier_;
    
    int nb_filter_;

    vector< vector< float > > weight_;     // filter weights
};


#endif // __filtersim_DEV_finder_H__
