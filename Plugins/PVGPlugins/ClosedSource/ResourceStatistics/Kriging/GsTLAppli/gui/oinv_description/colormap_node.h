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

#ifndef __GSTLAPPLI_OINV_COLORMAP_NODE_H__ 
#define __GSTLAPPLI_OINV_COLORMAP_NODE_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/colormap.h> 
 
#include <Inventor/SbColor.h> 
 
class SoSeparator; 
class SoCoordinate3; 
class SoMaterial; 
class SoQuadMesh; 
 
 
class GUI_DECL Oinv_colormap_node { 
 public: 
  Oinv_colormap_node(); 
  Oinv_colormap_node( const Colormap& cmap , int, bool); 
  ~Oinv_colormap_node(); 
  void colormap( const Colormap& cmap ); 
 
  SoSeparator* oinv_node() { return root_; } 
   
  /** Sets the color over which the colormap will be displayed. 
   * This function is useful if the background color of the viewer is 
   * changed.  
   */ 
  void background_color( const SbColor& color ); 

  //TL modified
  void draw_tickmarks( const Colormap& cmap, int ); 
  void update_ticks(int);
 
 
 private: 
  void set_mesh_coordinates( int nb_colors ); 
 
 private: 
  Colormap cmap_; 
  int nb_colors_; 
 
  SoSeparator* root_; 
  SoCoordinate3* coords_node_; 
  SoMaterial* colors_node_; 
  SoQuadMesh* mesh_;  
 
  SoSeparator* text_sep_; 
  SoMaterial* tickmarks_color_; 

  SbColor text_color_;

  //TL modified
  int _numticks;
  bool _horizontal;
};  
 
#endif 
