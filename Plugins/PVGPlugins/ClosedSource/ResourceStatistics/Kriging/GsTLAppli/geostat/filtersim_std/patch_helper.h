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


#ifndef __filtersim_patch_H__
#define __filtersim_patch_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/grid/grid_model/geovalue.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/gstl_types.h> 

#include <algorithm>
#include <vector>

using namespace std;

/*
 * class Patch_Helper
 * record whether the node is patched or not
 */
class Patch_Helper
{
public:
    Patch_Helper( RGrid* simu_grid, vector<bool>& region, bool has_hard_data=false );
    Patch_Helper( RGrid* simu_grid, bool has_hard_data=false );
    Patch_Helper( GsTLGridProperty* prop );
    Patch_Helper( Patch_Helper& rhs );
    ~Patch_Helper(){};

    // mark the patched data
    void set_node_patched( Geovalue& gval );
    void set_node_patched( int node_id );

    bool is_node_patched( Geovalue& gval );
    bool is_node_patched( int node_id );

    int total_node_patched() { return nb_data_; }

private:
    vector<bool> patch_;

    int nb_data_;
};


#endif // __filtersim_patch_H__
