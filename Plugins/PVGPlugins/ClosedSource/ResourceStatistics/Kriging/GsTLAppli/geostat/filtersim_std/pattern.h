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


#ifndef __filtersim_pattern_type_H__
#define __filtersim_pattern_type_H__

#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/geostat/common.h>
#include <vector>
#include <cmath>

#include "filters.h"

using namespace std;

const float UNINFORMED = -9966699;  // uninformed data
const float EPSILON = 0.000001;     // a small number

// score type
typedef vector<float> PatternType;  // template pixel
typedef pair< int, vector<float> > OneScoreType;    // node_id, template pixel at a node
typedef vector< OneScoreType > ScoresType;      // all score value

/*
 * function to normalize a score  into range [p_start, p_end] in each dimension
 */
template<typename T>
void find_score_extreme( vector< pair< int, vector<T> > >& score, vector<T>& max_value, vector<T>& min_value )
{
    int i, j;
    int nb_replicates = score.size();
    int nb_score = score[0].second.size();
    max_value.resize( nb_score, (T)(-99999) );
    min_value.resize( nb_score, (T)(99999) );

    // find the min and max
    for (i=0; i<nb_replicates; i++)
    {
        vector<T>& one_score = score[i].second;
        for (j=0; j<nb_score; j++)
        {
            if ( max_value[j] < one_score[j] )   max_value[j] =one_score[j];
            if ( min_value[j] > one_score[j] )   min_value[j] =one_score[j];
        }
    }
}

/*
 * function to normalize a score  into range [p_start, p_end] in each dimension
 */
template<typename T>
void normalize_score( vector< pair< int, vector<T> > >& score, vector<T>& max_value, 
                     vector<T>& min_value, T p_start=(T)(0), T p_end=(T)(1) )
{
    int i, j;
    int nb_replicates = score.size();
    int nb_score = score[0].second.size();

    vector<T> denominator( nb_score );
    vector<T> multiplier( nb_score );

    // find the min and max
    find_score_extreme(score, max_value, min_value);

    // calculate the multiplier
    for (j=0; j<nb_score; j++)
    {
        denominator[j] = max_value[j]-min_value[j];

        if ( denominator[j] == (T)(0) )
            multiplier[j] = (T)(0);
        else
            multiplier[j] = (p_end-p_start)/denominator[j];
    }

    // normalize the score values in each score dimension
    for (i=0; i<nb_replicates; i++)
    {
        vector<T>& one_score = score[i].second;
        for (j=0; j<nb_score; j++)
            one_score[j] = p_start + ( one_score[j] - min_value[j] )*multiplier[j];
    }
}


/*
 * create filter scores for categorical variable
 * totally, there are nb_facies*nb_filter scores. However, because
 * data redundancy, we only keep (nb_facies-1)*nb_filter scores for
 * partition. If the user want to view the scores in the training image
 * grid, then the program output all nb_facies*nb_filter scores in
 * Training image grid.
 */
inline GEOSTAT_DECL
void create_filter_cate_scores( RGrid* training_image_, 
                          ScoresType& score, Filter* my_filters_, 
                          int ncoarse, int is_viewscore_, int nb_facies, 
                          string training_property_name_,
                          vector<GsTLGridProperty*>& scoreProps_, int nreal,
                          vector<float>& max_value, vector<float>& min_value )
{
	int loc=0;          // node location with score values
    int nb_filter = my_filters_->get_total_filter_number();
	
    // get filter template, neighborhood & weight
    Grid_template* geom = my_filters_->get_window_geometry(ncoarse);
    SmartPtr<Neighborhood> neighborhood = training_image_->window_neighborhood( *geom );
    neighborhood->select_property( training_property_name_ );

    vector< vector< float > > weight ;
    for (int cur_filter=0; cur_filter<nb_filter; cur_filter++) 
        weight.push_back( my_filters_->get_weights(cur_filter) );

    for( Geostat_grid::iterator node_iter = training_image_->begin(); 
                node_iter != training_image_->end();  node_iter++, loc++ ) 
    {
        // find the neighbors of the current node
        neighborhood->find_neighbors( *node_iter );

        // if the current node does not have full neighbors, skip it
        if( neighborhood->size() != neighborhood->max_size() ) continue;

        vector<float> temp_score(nb_filter*nb_facies, 0.);

        for(int cur_filter=0; cur_filter<nb_filter; cur_filter++) 
        {
            vector<float>::const_iterator weight_iter = weight[cur_filter].begin();
            vector<float> sum(nb_facies, 0.);

            // calculate filter score
            for( Neighborhood::iterator neighbor_iter = neighborhood->begin();
                neighbor_iter != neighborhood->end() ; neighbor_iter++, weight_iter++)	
            {
                sum[ neighbor_iter->property_value() ] += *weight_iter;
            }

            // score is ordered first in filter id, then in facies id
            for(int cur_facies=0; cur_facies<nb_facies; cur_facies++)
                temp_score[cur_facies*nb_filter+cur_filter] = sum[cur_facies];
        }

        score.push_back( make_pair(loc, temp_score) );
    }

    // only output score view in the fineset grid for realization 1 if required
    if( is_viewscore_==1 && nreal==1 && ncoarse==1 ) 
    {
        for(int j=0; j<nb_filter*nb_facies; j++) 
        {
            GsTLGridProperty * prop = training_image_->select_property ( scoreProps_[ j ]->name() );

            for (int i=0; i<score.size(); i++)
                prop->set_value( score[i].second[j], score[i].first );
        }
    }

    //remove the redundent score values for the last facies
    int remove_loc = (nb_facies-1)*nb_filter;
    for (int i=0; i<score.size(); i++)
    {
        vector< float >& one_score = score[i].second;
        one_score.erase( one_score.begin()+remove_loc, one_score.end() );
    }
    
    // normalize score to be [0, 1]
    normalize_score( score, max_value, min_value );

    delete geom;
}



/*
 * create filter scores for continuous variable
 */
inline  GEOSTAT_DECL
void create_filter_cont_scores( RGrid* training_image_, 
                          ScoresType& score, Filter* my_filters_, 
                          int ncoarse, int is_viewscore_,
                          string training_property_name_,
                          vector<GsTLGridProperty*>& scoreProps_, int nreal,
                          vector<float>& max_value, vector<float>& min_value )
{
	int loc=0;          // node location with score values
    int nb_filter = my_filters_->get_total_filter_number();
	
    // get filter template, neighborhood & weight
    Grid_template* geom = my_filters_->get_window_geometry(ncoarse);
    SmartPtr<Neighborhood> neighborhood = training_image_->window_neighborhood( *geom );
    neighborhood->select_property( training_property_name_ );

    vector< vector< float > > weight ;
    for (int cur_filter=0; cur_filter<nb_filter; cur_filter++) 
        weight.push_back( my_filters_->get_weights(cur_filter) );

    for( Geostat_grid::iterator node_iter = training_image_->begin(); 
                node_iter != training_image_->end();  node_iter++, loc++ ) 
    {
        // find the neighbors of the current node
        neighborhood->find_neighbors( *node_iter );

        // if the current node does not have full neighbors, skip it
        if( neighborhood->size() != neighborhood->max_size() ) continue;

        vector<float> temp_score(nb_filter, 0.);

        for(int cur_filter=0; cur_filter<nb_filter; cur_filter++) 
        {
            vector<float>::const_iterator weight_iter = weight[cur_filter].begin();

            // calculate filter score
            for( Neighborhood::iterator neighbor_iter = neighborhood->begin();
                 neighbor_iter != neighborhood->end() ; neighbor_iter++, weight_iter++)	
            {
                temp_score[cur_filter] += (*weight_iter)*neighbor_iter->property_value();
            }
        }

        score.push_back( make_pair(loc, temp_score) );
    }

    // only output score view in the fineset grid for realization 1 if required
    if( is_viewscore_==1 && nreal==1 && ncoarse==1 ) 
    {
        for(int cur_filter=0; cur_filter<nb_filter; cur_filter++) 
        {
            GsTLGridProperty * prop = training_image_->select_property ( scoreProps_[ cur_filter ]->name() );

            for (int i=0; i<score.size(); i++)
                prop->set_value( score[i].second[cur_filter], score[i].first );
        }
    }
    
    // normalize score to be [0, 1]
    normalize_score( score, max_value, min_value );

    delete geom;
}



/*
 * create filter scores depending on the variable type
 */
inline  GEOSTAT_DECL
void create_filter_scores( RGrid* training_image_, 
                          ScoresType& score, Filter* my_filters_, 
                          int ncoarse, int is_viewscore_,
                          int treat_cate_as_cont, int nb_facies, 
                          string training_property_name_,
                          vector<GsTLGridProperty*>& scoreProps_, int nreal,
                          vector<float>& max_value, vector<float>& min_value )
{
    // for continuous variable
    if ( treat_cate_as_cont == 1 )
        create_filter_cont_scores( training_image_, score,
                my_filters_, ncoarse, is_viewscore_, 
                training_property_name_, scoreProps_, nreal,
                max_value, min_value);
    else    // for categorical variable
        create_filter_cate_scores( training_image_, score, 
                my_filters_, ncoarse, is_viewscore_, nb_facies, 
                training_property_name_, scoreProps_, nreal,
                max_value, min_value);
}


#endif //	__filtersim_pattern_type_H__
