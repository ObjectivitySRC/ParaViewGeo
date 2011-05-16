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


#include "pattern_paster.h"


/*
 * instructor (class Pattern_Paster)
 * nxdt, nydt, nzdt are the template size in finest grid
 */
Pattern_Paster::
Pattern_Paster( RGrid* grid, Window_neighborhood* neighbors, 
                RGrid* TI_grid, Window_neighborhood* TI_neighbors, 
                Patch_Helper* patcher, int nxdt, int nydt, int nzdt ):
        grid_(grid), neighbors_(neighbors), TI_grid_(TI_grid), 
        TI_neighbors_(TI_neighbors), patcher_(patcher)
{ 
    // set the half template size
    nxdt_ = (int) ( (nxdt -1)/2 );
    nydt_ = (int) ( (nydt -1)/2 );
    nzdt_ = (int) ( (nzdt -1)/2 );

    geom_ = neighbors_->geometry(); 
    prop = grid_->selected_property();
    cursor = grid_->cursor() ;
}


/*
 * operator() function (class Pattern_Paster)
 */
void Pattern_Paster::
operator()( int node_id, int pattern_id )
{
    // use cursor to access simulation grid node within the template
    GsTLInt i,j,k;
    cursor->coords( node_id, i,j,k ); 
    GsTLGridNode center_location( i,j,k );
    
    TI_neighbors_->find_neighbors( TI_grid_->geovalue( pattern_id ) );
    Window_neighborhood::iterator TI_neighbor_iter = TI_neighbors_->begin();
    Grid_template::iterator geom_itr = geom_.begin();

    // loop over each node within the template in the simulation grid
    for ( ; geom_itr != geom_.end(); geom_itr++, TI_neighbor_iter++)
    {
        // find the node id
        GsTLGridNode p = center_location + (*geom_itr);
        GsTLInt node_id = cursor->node_id( p[0], p[1], p[2] );
        
        // if the node is not patched and in the simulation region
        if ( node_id>-1 && !patcher_->is_node_patched(node_id) )
        {
            prop->set_value( TI_neighbor_iter->property_value(), node_id );

            // for applying servosysytem correctly
            // current node must be within the patch template
            if( abs( geom_itr->x() )<=nxdt_ && 
                abs( geom_itr->y() )<=nydt_ &&
                abs( geom_itr->z() )<=nzdt_ )
            {
                patcher_->set_node_patched( node_id );
            }
        }
    }
}


/*
 * operator() function (class Pattern_Paster_Categorical)
 */
void Pattern_Paster_Categorical::
operator()( RGrid::random_path_iterator current_node_iter, int pattern_id, 
           vector<float>& histogram, int& nb_data )
{
    nb_data = 0;    // recording the number of patched data

    // use cursor to access simulation grid node within the template
    GsTLInt i,j,k;
    cursor->coords( current_node_iter->node_id(), i,j,k ); 
    GsTLGridNode center_location( i,j,k );
    
    TI_neighbors_->find_neighbors( TI_grid_->geovalue( pattern_id ) );
    Window_neighborhood::iterator TI_neighbor_iter = TI_neighbors_->begin();
    Grid_template::iterator geom_itr = geom_.begin();

    // loop over each node within the template in the simulation grid
    for ( ; geom_itr != geom_.end(); geom_itr++, TI_neighbor_iter++)
    {
        // get the node id
        GsTLGridNode p = center_location + (*geom_itr);
        GsTLInt node_id = cursor->node_id( p[0], p[1], p[2] );
        
        // if the node is not patched and in the simulation region
        if ( node_id>-1 && !patcher_->is_node_patched(node_id) )
        {
            prop->set_value( TI_neighbor_iter->property_value(), node_id );

            // for applying servosysytem correctly
            // current node must be within the patch template
            if( abs( geom_itr->x() )<=nxdt_ && 
                abs( geom_itr->y() )<=nydt_ &&
                abs( geom_itr->z() )<=nzdt_ )
            {
                patcher_->set_node_patched( node_id );
                histogram[ (int)TI_neighbor_iter->property_value() ] += 1;
                nb_data++;
            }
        }
    }
}


/*
 * operator function (class Pattern_Paster_Continuous)
 */
void Pattern_Paster_Continuous::
operator()( RGrid::random_path_iterator current_node_iter, int pattern_id, 
           float& pattern_mean, int& nb_data )
{
    nb_data = 0;    // recording the number of patched data

    // use cursor to access simulation grid node within the template
    GsTLInt i,j,k;
    cursor->coords( current_node_iter->node_id(), i,j,k ); 
    GsTLGridNode center_location( i,j,k );
    
    TI_neighbors_->find_neighbors( TI_grid_->geovalue( pattern_id ) );
    Window_neighborhood::iterator TI_neighbor_iter = TI_neighbors_->begin();
    Grid_template::iterator geom_itr = geom_.begin();

    // loop over each node within the template in the simulation grid
    for ( ; geom_itr != geom_.end(); geom_itr++, TI_neighbor_iter++)
    {
        // find the node id
        GsTLGridNode p = center_location + (*geom_itr);
        GsTLInt node_id = cursor->node_id( p[0], p[1], p[2] );
        
        // if the node is not patched and in the simulation region
        if ( node_id>-1 && !patcher_->is_node_patched(node_id) )
        {
            prop->set_value( TI_neighbor_iter->property_value(), node_id );

            // for applying servosysytem correctly
            // current node must be within the patch template
            if( abs( geom_itr->x() )<=nxdt_ && 
                abs( geom_itr->y() )<=nydt_ &&
                abs( geom_itr->z() )<=nzdt_ )
            {
                patcher_->set_node_patched( node_id );
                pattern_mean += TI_neighbor_iter->property_value();
                nb_data++;
            }
        }
    }
}