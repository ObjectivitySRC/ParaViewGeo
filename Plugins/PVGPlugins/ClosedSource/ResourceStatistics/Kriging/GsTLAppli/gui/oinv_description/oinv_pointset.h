/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_OINV_DESCRIPTION_POINTSET_H__ 
#define __GSTLAPPLI_OINV_DESCRIPTION_POINTSET_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/oinv_description/oinv_description.h> 
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

class Point_set; 
class SoMaterial; 
class SoSwitch; 
class SoDrawStyle; 
 
class GUI_DECL Oinv_pointset : public Oinv_description { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Oinv_pointset(); 
  ~Oinv_pointset(); 
 
  virtual void init( const Geostat_grid* grid ); 
 
  virtual void property_display_mode( Oinv::Property_display_mode mode ); 
  virtual void point_size( int size ); 
  virtual void property_deleted( const std::string& prop_name );
 
 protected: 
  virtual void refresh(); 
 
 protected: 
  const Point_set* grid_; 
  //TL modified
  const Reduced_grid * redgrid_;

  SoSwitch* material_switch_; 
  SoMaterial* colors_node_; 
  SoDrawStyle* draw_style_node_; 
}; 
 
#endif 
