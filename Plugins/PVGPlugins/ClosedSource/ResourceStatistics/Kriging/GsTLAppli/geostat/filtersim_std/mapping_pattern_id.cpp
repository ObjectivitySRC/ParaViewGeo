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

#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>

#include "distance.h"
#include "mapping_pattern_id.h"


/*
 * constructor function, from original simulation grid and TI grid
 */
Pattern_Node_Map::Pattern_Node_Map( RGrid* grid, RGrid* TI_grid )
{
    grid_ = grid;
    TI_grid_ = TI_grid;
    cursor_ = grid_->cursor(); 
    TI_cursor_ = TI_grid_->cursor();
}


/*
 * constructor function, from an existing Pattern_Node_Map
 */
Pattern_Node_Map::Pattern_Node_Map( vector< pair<int, int> >& mapping ) 
{ 
    data_.reserve( mapping.size() );
    for (int i=0; i<mapping.size(); i++)
        data_[i] = make_pair( mapping[i].first, mapping[i].second );
}


/*
 * function to totally clean the mapping table
 */
void Pattern_Node_Map::reset_mapping_table()  
{ 
    data_.clear(); 
    loc_.clear();
}


/*
 * function to add the TI neighborhood for pattern searching
 */
void Pattern_Node_Map::set_TI_neighborhood( Window_neighborhood* TI_neighbors )
{
    TI_neighbors_ = TI_neighbors;
    calculate_data_size();    // computer the size of the mapping table
}


/*
 * function to erase the old mapping table, and recalculate the current mapping table size
 */
void Pattern_Node_Map::remove_previous_recording()
{
    loc_.clear();
    data_.erase( data_.begin(), data_.begin()+nsize_ );
    nsize_ = data_.size();
}


/*
 * function to reset the grids and grid cursors
 */
void Pattern_Node_Map::set_grid( RGrid* grid, RGrid* TI_grid )
{
    grid_ = grid;
    TI_grid_ = TI_grid;

    cursor_ = grid_->cursor();
    TI_cursor_ = TI_grid_->cursor();
}


/*
 * function to calculate the (i,j,k) location of the simulated nodes
 * and the corresponding patterns' id
 */
void Pattern_Node_Map::record_node_location()
{  
    GsTLInt i,j,k;

    for ( int ip=0; ip<nsize_; ip++ )
    {
        // get the node i,j,k locations
        cursor_->coords( data_[ip].first, i,j,k ); 

        vector<int> temp(4,0);
        temp[0] = i;
        temp[1] = j;
        temp[2] = k;
        temp[3] = data_[ip].second;
        loc_.push_back( temp );
    }
}


/*
 * function to find the pattern id of current simulated node u_1=(ix,iy,iz), by:
 * (1) find the node (u_2) closest to the simulated node u_1 with some distance function;
 * (2) find the pattern id (p_2) of node u_2;
 * (3) find a pattern id p_1 by shift pattern p_2 with the reverse offsets from (1);
 * (4) if p_1 is invalid, return p_2 as an approximation
 * (5) otherwise, return p_1
 */
int Pattern_Node_Map::find_closest_pattern_id( int ix, int iy, int iz )
{
    float sum = 999999;
    int best_loc = 0;   // the optimal location

    // find the closest simulated node u_2
    for ( int loc=0; loc<nsize_; loc++ )
    {
        vector<int>& temp=loc_[loc];
        //float dist = abs(temp[0]-ix)+abs(temp[1]-iy)+abs(temp[2]-iz);       // L1 distance
        float dist = std::sqrt( (float)(sqr(temp[0]-ix)+sqr(temp[1]-iy)+sqr(temp[2]-iz)) );  // L2 distance
        if ( dist <sum )
        {
            best_loc = loc;
            sum = dist;
        }
    }

    // the i,j,k location of the pattern id p_2
    GsTLInt pi, pj, pk;
    TI_cursor_->coords( loc_[best_loc][3], pi, pj, pk ); 

    // shift p_2 to p_1 with the reverse offsets from (1)
    vector<int>& temp = loc_[best_loc];
    pi = pi-temp[0] + ix;
    pj = pj-temp[1] + iy;
    pk = pk-temp[2] + iz;

    // find the nb of neighbors of current pattern using a TI neighborhood
    TI_neighbors_->find_neighbors( TI_grid_->geovalue( pi, pj, pk ) );

    // correct the offset 
    if ( TI_neighbors_->size() == TI_neighbors_->max_size() )       // p_1 is a valid pattern id
        return TI_grid_->geovalue( pi, pj, pk ).node_id();
    else
        return temp[3];      // approximate p_1 with p_2
}


