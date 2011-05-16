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

#include "patch_helper.h"

/*
 * constructor, from simulation grid, use region concept
 * only the nodes in the current active regions can be patched
 */
Patch_Helper::Patch_Helper( RGrid* simul_grid, vector<bool>& region, bool has_hard_data )
{
    //patch_.resize( simul_grid->nxyz(), false );
    patch_.resize( simul_grid->size(), false );

    nb_data_ = 0;

    // if has hard data, then set hard data as patched
    if ( has_hard_data )
    {
        for (RGrid::iterator current_node_iter = simul_grid->begin(); 
             current_node_iter != simul_grid->end(); current_node_iter++) 
        {
            if ( current_node_iter->is_harddata() ) 
            {
                patch_[ current_node_iter->node_id() ] = true;
                nb_data_ ++;
            }
        }
    }

    // only the nodes in current active regions can be patched later
    for ( int i=0; i<region.size(); i++ )
    {
        if ( !region[i] )   
            patch_[i] = true;
    }
}


/*
 * constructor, from simulation grid
 */
Patch_Helper::Patch_Helper( RGrid* simul_grid, bool has_hard_data )
{
    //patch_.resize( simul_grid->nxyz(), false );
    patch_.resize( simul_grid->size(), false );

    nb_data_ = 0;

    // if has hard data, then set hard data as patched
    if ( has_hard_data )
    {
        for (RGrid::iterator current_node_iter = simul_grid->begin(); 
             current_node_iter != simul_grid->end(); current_node_iter++) 
        {
            if ( current_node_iter->is_harddata() ) 
            {
                patch_[ current_node_iter->node_id() ] = true;
                nb_data_ ++;
            }
        }
    }
}



/*
 * constructor, from property
 */
Patch_Helper::Patch_Helper( GsTLGridProperty* prop )
{
    patch_.resize( prop->size(), false );
    nb_data_ = 0;

    // if has hard data, then set hard data as patched
    for ( int i=0; i<patch_.size(); i++ ) 
    {
        if ( prop->is_informed( i ) ) 
        {
            patch_[ i ] = true;
            nb_data_ ++;
        }
    }
}


/*
 * instructor, from an exist Patch_Helper rhs
 * rhs must be of same size as *this
 */
Patch_Helper::Patch_Helper( Patch_Helper& rhs )
{
    patch_.resize( rhs.total_node_patched(), false );
    nb_data_ = 0;

    for (int i=0; i<patch_.size(); i++)
    {
        if ( rhs.is_node_patched(i) )
        {
            patch_[i] = true;
            nb_data_ ++;
        }
    }
}

/*
 * set current geovalue as patched
 */
void Patch_Helper::set_node_patched( Geovalue& gval )
{
    patch_[ gval.node_id() ] = true;
    nb_data_ ++;
}

/*
 * set current node as patched
 */
void Patch_Helper::set_node_patched( int node_id )
{
    patch_[ node_id ] = true;
    nb_data_++;
}


/*
 * return patch information of current geovalue
 */
bool Patch_Helper::is_node_patched( Geovalue& gval )
{
    return patch_[ gval.node_id() ];
}

/*
 * return patch informatoin of current node
 */
bool Patch_Helper::is_node_patched( int node_id )
{
    return patch_[ node_id ];
}