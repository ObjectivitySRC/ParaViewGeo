/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
** Modeling Software (GEMS)
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

#include <GsTLAppli/grid/grid_model/rgrid_gval_accessor.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>


RGrid_gval_accessor::RGrid_gval_accessor(
    RGrid* rgrid,
    const std::string & prop_name
  ) 
  : current_geovalue_(rgrid, rgrid->property(prop_name), 0) {
  
}

RGrid_gval_accessor::RGrid_gval_accessor(
    RGrid* rgrid,
    GsTLGridProperty* prop
  ) 
  : current_geovalue_(rgrid, prop, 0) {
  
}
RGrid_gval_accessor::RGrid_gval_accessor(
   const RGrid_gval_accessor& a
   )  : current_geovalue_(a.current_geovalue_) {
   
}

