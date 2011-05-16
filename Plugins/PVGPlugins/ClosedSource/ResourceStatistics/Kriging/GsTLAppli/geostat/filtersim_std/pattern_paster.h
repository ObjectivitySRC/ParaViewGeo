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


#ifndef __filtersim_pattern_paste_H__
#define __filtersim_pattern_paste_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h>
#include <GsTLAppli/grid/grid_model/geovalue.h>

#include <algorithm>

#include "pattern.h"
#include "patch_helper.h"


/*
 * class Pattern_Paster
 * the base class for pattern paster
 */
class GEOSTAT_DECL Pattern_Paster
{
public:
    Pattern_Paster( RGrid* grid, Window_neighborhood* neighbors,
                    RGrid* TI_grid, Window_neighborhood* TI_neighbors, 
                    Patch_Helper* patcher, int nxdt, int nydt, int nzdt );

    ~Pattern_Paster(){}

    void operator()( int node_id, int pattern_id);

protected:
    int nxdt_;      // patch neighborhood sizes
    int nydt_;
    int nzdt_;

    RGrid* grid_;
    Window_neighborhood* neighbors_;
    RGrid* TI_grid_;
    Window_neighborhood* TI_neighbors_;
    Patch_Helper* patcher_;

    Grid_template geom_;
    GsTLGridProperty* prop;
    SGrid_cursor * cursor;
};


/*
 * class Pattern_Paster_Categorical
 */
class GEOSTAT_DECL Pattern_Paster_Categorical: public Pattern_Paster
{
public:
    Pattern_Paster_Categorical( RGrid* grid, Window_neighborhood* neighbors, 
                                RGrid* TI_grid, Window_neighborhood* TI_neighbors, 
                                Patch_Helper* patcher, int nxdt, int nydt, int nzdt )
            :Pattern_Paster( grid, neighbors, TI_grid, TI_neighbors, patcher, nxdt, nydt, nzdt ) {}

    ~Pattern_Paster_Categorical(){}

    void operator()( RGrid::random_path_iterator current_node_iter, int pattern_id, 
                     vector<float>& histogram, int& nb_data );
};


/*
 * class Pattern_Paster_Continuous
 */
class GEOSTAT_DECL Pattern_Paster_Continuous: public Pattern_Paster
{
public:
    Pattern_Paster_Continuous( RGrid* grid, Window_neighborhood* neighbors,
                               RGrid* TI_grid, Window_neighborhood* TI_neighbors, 
                               Patch_Helper* patcher, int nxdt, int nydt, int nzdt )
           :Pattern_Paster( grid, neighbors, TI_grid, TI_neighbors, patcher, nxdt, nydt, nzdt ) {}

    ~Pattern_Paster_Continuous(){}

    void operator()( RGrid::random_path_iterator current_node_iter, int pattern_id, 
                     float& histogram, int& nb_data );
};


#endif // __filtersim_pattern_paste_H__
