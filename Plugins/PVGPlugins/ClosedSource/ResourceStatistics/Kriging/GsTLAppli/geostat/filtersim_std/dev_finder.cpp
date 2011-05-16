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

#include "dev_finder.h"

//
// for hard data event
//

// -------categorical variable----

/*
 * this function is used to initialize the data event, and its associated
 * weights. Initially, the data event is set as uninformed and its weight
 * is assigned as "weight to other data"
 */
void HD_DEV_Finder_Categorical::
initialize_DEV( vector< vector<float> >& data_event, vector<float>& weight )
{
    vector< float > temp( neighbors_->max_size(), UNINFORMED );
    
    for(int i=0; i<nb_facies_; ++i) 
        data_event.push_back( temp );

    weight.resize( neighbors_->max_size(), data_weight_[2] );    
}


/*
 * this function is used to extract the data event and its weight at
 * the given node iterator. If the neighborhood of current node is
 * full, then return "true"; otherwise return false.
 */
/*
bool HD_DEV_Finder_Categorical::
operator() ( RGrid::random_path_iterator current_node_iter, 
               vector< vector<float> >& dev, vector<float>& weight )
{
    initialize_DEV( dev, weight );

    neighbors_->find_neighbors(*current_node_iter);
    bool find_neighbors = false;

    if ( !neighbors_->is_empty() )  
    {       
        find_neighbors = true;

        int data_loc = 0;

        // iterate all the neighbors, and increase its index
        for (Neighborhood::iterator neighbor_iter = neighbors_->begin(); 
                neighbor_iter != neighbors_->end(); neighbor_iter++, data_loc++)
        {
            if( neighbor_iter->is_informed() )  // node is informed (has value)
            {
                // transfer data value into probability (as 1 or 0)
                for (int j=0; j<nb_facies_; j++)
                    dev[j][data_loc] = ( (neighbor_iter->property_value()==j) ? 1 : 0 );
                
                // set data event weight according to data type
                if ( neighbor_iter->is_harddata() )
                    weight[data_loc] = data_weight_[0];
                else if ( patcher_->is_node_patched( *neighbor_iter ) )
                    weight[data_loc] = data_weight_[1];
            }
        }	
    }

    return find_neighbors;
}
*/

bool HD_DEV_Finder_Categorical::
operator() ( RGrid::random_path_iterator current_node_iter, 
               vector< vector<float> >& dev, vector<float>& weight )
{
    vector<int> type;
    return this->operator()(current_node_iter, dev, weight, type);
}

/*
 * this function is used to extract the data event and its weight at
 * the given node iterator. If the neighborhood of current node is
 * full, then return "true"; otherwise return false.
 * 
 * This is an overloaded function, type indicates the node are either
 * hard data or frozen data
 */
bool HD_DEV_Finder_Categorical::
operator() ( RGrid::random_path_iterator current_node_iter, 
             vector< vector<float> >& dev, vector<float>& weight, vector<int>& type )
{
    initialize_DEV( dev, weight );
    type.resize( weight.size(), 0 );

    neighbors_->find_neighbors(*current_node_iter);
    bool find_neighbors = false;

    if ( !neighbors_->is_empty() )  
    {       
        find_neighbors = true;

        int data_loc = 0;
        int nb_hard=0;
        int nb_patch=0;

        // iterate all the neighbors, and increase its index
        for (Neighborhood::iterator neighbor_iter = neighbors_->begin(); 
                neighbor_iter != neighbors_->end(); neighbor_iter++, data_loc++)
        {
            if( neighbor_iter->is_informed() )  // node is informed (has value)
            {
                // transfer data value into probability (as 1 or 0)
                for (int j=0; j<nb_facies_; j++)
                    dev[j][data_loc] = ( (neighbor_iter->property_value()==j) ? 1 : 0 );
                
                // set data event weight according to data type
                if ( neighbor_iter->is_harddata() )
                {
                    //weight[data_loc] = data_weight_[0];
                    nb_hard++;
                    type[data_loc] = 1;
                }
                else if ( patcher_->is_node_patched( *neighbor_iter ) )
                {
                    //weight[data_loc] = data_weight_[1];
                    type[data_loc] = 2;
                    nb_patch++;
                }
            }
        }	
    
        int nb_other = neighbors_->max_size() - nb_hard - nb_patch;

        float weight_hard = (nb_hard==0) ? 0 : data_weight_[0]/nb_hard;
        float weight_patch = (nb_patch==0) ? 0 : data_weight_[1]/nb_patch;
        float weight_other = (nb_other==0) ? 0 : data_weight_[2]/nb_other;

        for (data_loc=0; data_loc<type.size(); data_loc++)
        {
            if ( type[data_loc]==0 )
                weight[data_loc] = weight_other;
            else if ( type[data_loc]==1 )
                weight[data_loc] = weight_hard;
            else
                weight[data_loc] = weight_patch;
        }
    }

    return find_neighbors;
}


// -------continuous variable----

/*
 * this function is used to initialize the data event, and its associated
 * weights. If data event is full, then do nothing; if not full, then the 
 * data event is set as uninformed and its weight is assigned as 
 * "weight to other data"
 */
void HD_DEV_Finder_Continuous::
initialize_DEV( vector<float>& data_event, vector<float>& weight )
{
    if ( data_event.size() == 0)
        data_event.resize( neighbors_->max_size(), UNINFORMED );    
    
    weight.resize( neighbors_->max_size(), data_weight_[2] ); 
}


/*
 * this function is used to extract the data event and its weight at
 * the given node iterator. If the neighborhood of current node is
 * full, then return "true"; otherwise return false.
 */
/*
bool HD_DEV_Finder_Continuous::
operator() ( RGrid::random_path_iterator current_node_iter, 
                vector<float>& dev, vector<float>& weight )
{
    initialize_DEV( dev, weight );

    neighbors_->find_neighbors(*current_node_iter);
    bool find_neighbors = false;

	if ( !neighbors_->is_empty() ) 
    {
        find_neighbors = true;

        int data_loc = 0; 

        // iterate all the neighbors, and increase its index
        for (Neighborhood::iterator neighbor_iter = neighbors_->begin(); 
                neighbor_iter != neighbors_->end(); neighbor_iter++, data_loc++)
        {
            if( neighbor_iter->is_informed() ) 
            {
                dev[data_loc] = neighbor_iter->property_value();
                
                // set data event weight according to data type
                if (neighbor_iter->is_harddata())
                    weight[data_loc] = data_weight_[0];
                else if ( patcher_->is_node_patched( *neighbor_iter ) )
                    weight[data_loc] = data_weight_[1];
            }
        }	
    }

    return find_neighbors;
}
*/

bool HD_DEV_Finder_Continuous::
operator() ( RGrid::random_path_iterator current_node_iter, 
                vector<float>& dev, vector<float>& weight )
{
    vector<int> type;
    return this->operator()( current_node_iter, dev, weight, type );
}


bool HD_DEV_Finder_Continuous::
operator() ( RGrid::random_path_iterator current_node_iter, 
                vector<float>& dev, vector<float>& weight, vector<int>& type )
{
    initialize_DEV( dev, weight );
    type.resize( weight.size(), 0 );

    neighbors_->find_neighbors(*current_node_iter);
    bool find_neighbors = false;

	if ( !neighbors_->is_empty() ) 
    {
        find_neighbors = true;

        int data_loc = 0;
        int nb_hard=0;
        int nb_patch=0;

        // iterate all the neighbors, and increase its index
        for (Neighborhood::iterator neighbor_iter = neighbors_->begin(); 
                neighbor_iter != neighbors_->end(); neighbor_iter++, data_loc++)
        {
            if( neighbor_iter->is_informed() ) 
            {
                dev[data_loc] = neighbor_iter->property_value();
                
                // set data event weight according to data type
                if (neighbor_iter->is_harddata())
                {
                    nb_hard++;
                    type[data_loc] = 1;
                }
                else if ( patcher_->is_node_patched( *neighbor_iter ) )
                {
                    type[data_loc] = 2;
                    nb_patch++;
                }
            }
        }	

        int nb_other = neighbors_->max_size() - nb_hard - nb_patch;

        float weight_hard = (nb_hard==0) ? 0 : data_weight_[0]/nb_hard;
        float weight_patch = (nb_patch==0) ? 0 : data_weight_[1]/nb_patch;
        float weight_other = (nb_other==0) ? 0 : data_weight_[2]/nb_other;

        for (data_loc=0; data_loc<type.size(); data_loc++)
        {
            if ( type[data_loc]==0 )
                weight[data_loc] = weight_other;
            else if ( type[data_loc]==1 )
                weight[data_loc] = weight_hard;
            else
                weight[data_loc] = weight_patch;
        }
    }

    return find_neighbors;
}

//
// for soft data event
//

// -------categorical variable----

/*
 * this function is used to initialize the soft data event, 
 * initially the soft data event is set as uninformed.
 */
void ST_DEV_Finder_Categorical::initialize_DEV( vector< vector<float> >& soft_dev )
{
    vector< float > temp( soft_neighbors_->max_size(), UNINFORMED );

    for(int i=0; i<nb_facies_; ++i) 
        soft_dev.push_back( temp );
}


/*
 * this function is used to extract the data event at the given node iterator. 
 */
void ST_DEV_Finder_Categorical::
operator()( RGrid::random_path_iterator current_node_iter, vector< vector<float> >& soft_dev)
{
    initialize_DEV( soft_dev );

    soft_neighbors_->find_neighbors(*current_node_iter);

    // soft data neighborhood always has some data
    for ( int j=0; j<nb_facies_; j++ )  // for each filter
    {
        // get the right soft property
        GsTLGridProperty* prop = softDataProps_[j];
        soft_neighbors_->select_property( prop->name() );
        
        int data_loc=0;

        // iterate all the neighbors, and increase its index
        for (Neighborhood::iterator neighbor_iter = soft_neighbors_->begin(); 
                neighbor_iter != soft_neighbors_->end(); neighbor_iter++, data_loc++) 
        {
            if ( neighbor_iter->is_informed() )
                soft_dev[j][data_loc] = prop->get_value( neighbor_iter->node_id() );
        }
    }
}

// -------continuous variable----

/*
 * this function is used to initialize the soft data event, 
 * initially the soft data event is set as uninformed.
 */
void ST_DEV_Finder_Continuous::initialize_DEV( vector<float>& soft_dev )
{
    soft_dev.resize( soft_neighbors_->max_size(), UNINFORMED );    
}


/*
 * this function is used to extract the data event at the given node iterator. 
 */
void ST_DEV_Finder_Continuous::
operator()( RGrid::random_path_iterator current_node_iter, vector<float>& soft_dev )
{
    initialize_DEV( soft_dev );

    // get the soft property data
    soft_neighbors_->find_neighbors(*current_node_iter);   
    soft_neighbors_->select_property( softDataProps_->name() );
    
    int data_loc=0;

    // iterate all the neighbors, and increase its index
    for (Neighborhood::iterator neighbor_iter = soft_neighbors_->begin(); 
            neighbor_iter != soft_neighbors_->end(); neighbor_iter++, data_loc++) 
    {
        if ( neighbor_iter->is_informed() )
            soft_dev[data_loc] = softDataProps_->get_value( neighbor_iter->node_id() );
    }
}


// -------------------------------------------

/*
 * constructor of DEV_Finder_Categorical_Score_based
 */
DEV_Finder_Categorical_Score_based::
DEV_Finder_Categorical_Score_based( RGrid* simu_grid, Window_neighborhood* neighbors, 
                        Filter* my_filters, Pattern_Node_Map& mapping, int nb_facies, 
                        vector<float>& max_value, vector<float>& min_value, int treat_cate_as_cont ): 
                        simu_grid_(simu_grid), neighbors_(neighbors), mapping_(mapping), nb_facies_(nb_facies), 
                        max_value_(max_value), min_value_(min_value), treat_cate_as_cont_(treat_cate_as_cont)
{
    nb_filter_ = my_filters->get_total_filter_number();
    total_score_ = nb_filter_*(nb_facies_-1);
    cursor_ = simu_grid_->cursor();

    int i;

    // record the nodal weights within the filter template
    for (i=0; i<nb_filter_; i++) 
        weight_.push_back( my_filters->get_weights(i) );

    // set up the multiple for standardize the filter scores
    multiplier_.resize( max_value_.size(), 0.0 );
    for (i=0; i<max_value_.size(); i++)
    {
        float denominator = max_value[i]-min_value[i];
        if ( denominator!=0. )    multiplier_[i] = 1.0/denominator;
    }
}


/*
 * this function is used to extract the data event and then calculate its score value
 * for the categorical variable
 */
bool DEV_Finder_Categorical_Score_based::
operator() ( RGrid::random_path_iterator current_node_iter, vector<float>& dev_score, int& pattern_id )
{
    dev_score.resize( total_score_, 0. );    
    neighbors_->find_neighbors(*current_node_iter);

    if ( neighbors_->size() == neighbors_->max_size() )      // the DEV is fully informed
    {
        int i;

        if ( treat_cate_as_cont_==1 )    
        {  // treat categorical variables as continuous variables
            for(i=0; i<nb_filter_; i++) 
            {
                vector<float>::const_iterator weight_iter = weight_[i].begin();

                // calculate filter score value
                for( Neighborhood::iterator neighbor_iter = neighbors_->begin();
                        neighbor_iter != neighbors_->end() ; neighbor_iter++, weight_iter++)	
                {
                        dev_score[ i ] += (*weight_iter)*neighbor_iter->property_value();
                }
            }
        }
        else   
        {  // handle the categorical variables directly
            for(i=0; i<nb_filter_; i++) 
            {
                vector<float>::const_iterator weight_iter = weight_[i].begin();
                vector<float> sum(nb_facies_, 0.);

                // calculate filter score
                for( Neighborhood::iterator neighbor_iter = neighbors_->begin();
                        neighbor_iter != neighbors_->end() ; neighbor_iter++, weight_iter++)	
                {
                        sum[ neighbor_iter->property_value() ] += *weight_iter;
                }

                // NOTE: filter score dimension loops first in filter id, then in facies id
                for(int j=0; j<nb_facies_-1; j++)
                    dev_score[ j*nb_filter_ + i ] = sum[j];
            }
        }

        // standardize the filter score values with the multiplier
        for (i=0; i<total_score_; i++)
            dev_score[i] = ( dev_score[i] - min_value_[i] )*multiplier_[i];

        return true;
    }
    else   // the simulated node is along the boundary
    {  // find the pattern id from the mapping table
        int i, j, k;
        cursor_->coords( current_node_iter->node_id(), i, j, k ); 
        pattern_id = mapping_.find_closest_pattern_id( i, j, k );

        return false;
    }
}



/*
 * constructor of DEV_Finder_Continuous_Score_based
 */
DEV_Finder_Continuous_Score_based::
DEV_Finder_Continuous_Score_based( RGrid* simu_grid, Window_neighborhood* neighbors, Filter* my_filters, 
                        Pattern_Node_Map& mapping, vector<float>& max_value, vector<float>& min_value ): 
                        simu_grid_(simu_grid), neighbors_(neighbors), mapping_(mapping), 
                        max_value_(max_value), min_value_(min_value)
{
    cursor_ = simu_grid_->cursor();
    nb_filter_ = my_filters->get_total_filter_number();

    int i;

    // record the nodal weights within the filter template
    for (i=0; i<nb_filter_; i++) 
        weight_.push_back( my_filters->get_weights(i) );

    // set up the multiple for standardize the filter scores
    multiplier_.resize( max_value_.size(), 0.0 );
    for (i=0; i<max_value_.size(); i++)
    {
        float denominator = max_value[i]-min_value[i];
        if ( denominator!=0. )    multiplier_[i] = 1.0/denominator;
    }
}

/*
 * this function is used to extract the data event and then calculate its score value
 * for the continuous variable
 */
bool DEV_Finder_Continuous_Score_based::
operator() ( RGrid::random_path_iterator current_node_iter, vector<float>& dev_score, int& pattern_id)
{
    dev_score.resize( nb_filter_, 0. );    
    neighbors_->find_neighbors(*current_node_iter);

    if ( neighbors_->size() == neighbors_->max_size() )  // the DEV is fully informed
    {
        int i;

        // calculate the DEV score by applying filters
        for(i=0; i<nb_filter_; i++) 
        {
            vector<float>::const_iterator weight_iter = weight_[i].begin();

            // calculate filter score
            for( Neighborhood::iterator neighbor_iter = neighbors_->begin();
                    neighbor_iter != neighbors_->end() ; neighbor_iter++, weight_iter++)	
            {  
                dev_score[ i ] += (*weight_iter)*neighbor_iter->property_value();
            }
        }

        // standardize the filter score values with the multiplier
        for (i=0; i<nb_filter_; i++)
            dev_score[i] = ( dev_score[i] - min_value_[i] )*multiplier_[i];

        return true;
    }
    else   // the simulated node is along the boundary
    {  // find the pattern id from the mapping table
        int i, j, k;
        cursor_->coords( current_node_iter->node_id(), i, j, k ); 
        pattern_id = mapping_.find_closest_pattern_id( i, j, k );

        return false;
    }
}
