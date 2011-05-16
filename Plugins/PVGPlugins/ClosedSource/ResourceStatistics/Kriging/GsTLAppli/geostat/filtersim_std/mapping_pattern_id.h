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


#ifndef __filtersim_mapping_pattern_id__
#define __filtersim_mapping_pattern_id__

#include <vector>
#include <utility>

using namespace std;

class RGrid;
class Window_neighborhood;
class SGrid_cursor;

/*
* class Pattern_Node_Map
* record the mapping between the simulation node_id in the fine grid 
*                                      and the pattern_id in the training image
*/
class  GEOSTAT_DECL Pattern_Node_Map
{
public:
    Pattern_Node_Map() {}
    Pattern_Node_Map( RGrid* grid, RGrid* TI_grid );
    Pattern_Node_Map( vector< pair<int, int> >& mapping );

    ~Pattern_Node_Map(){}

    void reset_mapping_table();

    void reset_node_location()  { loc_.clear(); }
    void calculate_data_size() { nsize_ = data_.size(); }

    // add one new mapping
    void add_mapping( int node_id, int pattern_id ) { data_.push_back( make_pair( node_id, pattern_id ) ); }
    void add_mapping( pair<int, int> one_pair ) { data_.push_back( one_pair ); }

    void set_TI_neighborhood( Window_neighborhood* TI_neighbors );
    void remove_previous_recording();
    void set_grid( RGrid* grid, RGrid* TI_grid );
    void record_node_location();
    int find_closest_pattern_id( int ix, int iy, int iz );

    int get_node_id(int i) { return data_[i].first; }
    int get_pattern_id(int i) { return data_[i].second; }
    int total_mapping_pair() { return data_.size(); }

private:
    vector< pair<int, int> > data_;       // the mapping between simulation node's id and the pattern id
    vector< vector<int> > loc_;          // the simulation i,j,k location+pattern_id
    int nsize_;     // the length of data_ and loc_;

    RGrid* grid_;
    SGrid_cursor* cursor_;      // grid cursor in the simulation grid

    RGrid* TI_grid_;
    Window_neighborhood* TI_neighbors_;
    SGrid_cursor* TI_cursor_;    // grid cursor in the TI grid
};


#endif // __filtersim_mapping_pattern_id__
