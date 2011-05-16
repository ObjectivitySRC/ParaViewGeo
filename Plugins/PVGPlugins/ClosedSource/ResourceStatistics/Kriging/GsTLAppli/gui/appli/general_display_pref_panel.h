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

#ifndef __GSTLAPPLI_GENERAL_DISPLAY_PREF_PANEL_H__ 
#define __GSTLAPPLI_GENERAL_DISPLAY_PREF_PANEL_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/appli/general_display_pref_panel_base.h> 
#include <qstring.h>

class SoQtGsTLViewer; 
class SoTransform; 
class SoGroup; 
class GsTL_project; 
class Oinv_colormap_node; 
class Oinv_description_map; 
class GsTL_SoNode; 
class SoSeparator; 
class PropertySelector;
class SoTranslate2Dragger;
class SoScale2Dragger;
class SoNode;

class GUI_DECL General_display_pref_panel : public General_display_pref_panel_base { 
 
  Q_OBJECT 
 
 public: 
  General_display_pref_panel( SoQtGsTLViewer* viewer, 
			      SoSeparator* colormap_root_node, 
			      Oinv_description_map* descriptions, 
			      QWidget* parent = 0, const char* name = 0); 
  virtual ~General_display_pref_panel(); 
 
  void init( GsTL_project* project ); 
 
  void load_colorbar_fromfile(QString &);

 public slots: 
  void use_perspective_camera(); 
  void use_orthotropic_camera(); 
  void white_background( bool on ); 
  void z_scaling( int scale ); 
  void display_colorbar( bool on ); 
  void update_colorbar(); 
  void enable_colorbar_display(); 
  //TL modified
  void redrawTicks(int);
  void rotate(bool);
  void save_colorbar();
  void load_colorbar();
 
 protected: 
  PropertySelector* object_selector_;

  SoQtGsTLViewer* viewer_; 
  Oinv_description_map* descriptions_; 
 
  SoTransform* Z_scaling_; 
  SoGroup* scene_root_; 
 
  SoSeparator* colormap_root_node_; 
  GsTL_SoNode* colorbar_root_; 
  Oinv_colormap_node* colorbar_; 

  //TL modified
  SoTransform * _transf;

 protected: 
  void init_colormap_node(); 
}; 
 
 
#endif 
