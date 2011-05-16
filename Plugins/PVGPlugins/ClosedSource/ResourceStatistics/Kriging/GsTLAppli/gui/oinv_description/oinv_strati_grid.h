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

#ifndef __GSTLAPPLI_GUI_OINV_STRATI_GRID_H__ 
#define __GSTLAPPLI_GUI_OINV_STRATI_GRID_H__ 
 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/oinv_description/oinv_common.h> 
#include <GsTLAppli/gui/oinv_description/oinv_description.h> 
 
#include <qstring.h>

#include <vector> 
#include <list> 

class Oinv_slice; 
 
 
class GUI_DECL Oinv_strati_grid : public Oinv_description { 
 public: 
  enum Display_mode { RENDERING=0, FULL=1, SLICES=2, BBOX=3 }; 
 
 public: 
  Oinv_strati_grid() : Oinv_description() {} 
  virtual ~Oinv_strati_grid() {} 
  virtual int add_slice( Oinv::Axis axis, int position, bool visible ) = 0; 
  virtual void remove_slice( int slice_id ) = 0; 
   
  virtual void display_mode( Display_mode mode ) = 0; 
  virtual void property_display_mode( Oinv::Property_display_mode mode ) = 0; 
 
  virtual void show_bounding_box( bool on ) = 0;
  virtual void hide_volume( bool on ) = 0;
  virtual void set_transparency( const QString& str ) = 0;
  virtual void set_transparency() = 0;
  virtual QString transparency_intervals( const std::string& property_name ) const =0;

  virtual Oinv_slice* slice( int slice_id ) = 0 ; 
 
}; 
 
#endif 
