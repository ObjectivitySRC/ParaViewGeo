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

#ifndef __GSTLAPPLI_GUI_OINV_DESCRIPTION_H__ 
#define __GSTLAPPLI_GUI_OINV_DESCRIPTION_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/utils/named_interface.h> 
#include <GsTLAppli/gui/oinv_description/oinv_common.h> 
 
#include <list> 
#include <vector>
#include <map> 
 
class Geostat_grid; 
class GsTLGridProperty; 
class GsTL_SoNode; 
class Colormap; 
class QString;
 
 
/** Oinv_description is the base class for all Open Inventor descriptions 
 * of grid models. Its aim is to describe a grid model (eg a cartesian grid  
 * or a triangulated surface) in Open Inventor terms, so that the grid can 
 * be displayed by Open Inventor.   
 */ 
 
class GUI_DECL Oinv_description : public Named_interface { 
 public: 
  Oinv_description(); 
  virtual ~Oinv_description(); 
  virtual void init( const Geostat_grid* grid ) = 0; 
  virtual const Geostat_grid* described_grid() const {return geostat_grid_; } 
 
  virtual GsTL_SoNode* oinv_node() { return oinv_node_; } 
 
  /** Update the description of the geostat grid.  
   * @param property_name is a hint to which grid property has changed.  
   * If "property_name" is empty, the description is updated, even if it 
   * was not necessary. 
   */ 
//  virtual void update( const std::string& property_name, 
//		                   Oinv::Update_reason reason ) = 0; 
  virtual void update( Oinv::Update_reason reason ,
    const std::vector<std::string>* properties = 0 );     
    //const std::string& property_name = "" ); 
 
 
  /** Changes the colormap for the current property. The display is then 
   * refreshed. If there is no current property, nothing happens. 
   */ 
  virtual void set_colormap( const std::string& map_name ); 
  virtual void set_colormap( const Colormap& cmap ); 
 
  /** Returns the current colormap. If there is no current colormap,  
   * returns 0. 
   */ 
  virtual const Colormap* colormap() const { return cmap_; } 
   
  /** Returns the colormap associated to property "prop_name". If 
   * no colormap exist for that property, it tries to create one. If that 
   * fails, the funtion returns 0. 
   */ 
  virtual const Colormap* colormap( const std::string& prop_name ); 
 
  /** Resets the current colormap min and max values to the 
   * min and max of the current property. If there is no current colormap, 
   * nothing happens 
   */ 
  virtual void reset_colormap_bounds(); 
 
  /** Changes the current property and refreshes the display 
   */ 
  virtual void set_property( const std::string& property_name ) ; 
  virtual std::string current_property() const { return current_property_name_; } 
 
  /** Turn on/off the painting of the property. 
   */ 
  virtual void property_display_mode( Oinv::Property_display_mode mode )= 0; 
  virtual Oinv::Property_display_mode property_display_mode() const { 
    return property_display_mode_; 
  } 
 
 
  //TL modified
  virtual bool update_desc(QString &, QString &);

 protected: 
  virtual void refresh() = 0; 
  virtual void property_deleted( const std::string& prop_name ) = 0;


 protected: 
  const Geostat_grid* geostat_grid_; 
  const GsTLGridProperty* current_property_; 
  std::string current_property_name_; 
 
 
  GsTL_SoNode* oinv_node_; 
  Colormap* cmap_; 
  typedef std::pair<float,float> Bounds_pair; 
  typedef std::map<std::string, std::pair<Bounds_pair, Colormap*> > CmapMap; 
  // cmap_map associates 2 elements to each property: 
  //   - the min and max values of the property 
  //   - a colormap to use to paint the property. The bounds of the colormap 
  //     need not be the min and max of the property 
  CmapMap cmap_map_; 
 
  Oinv::Property_display_mode property_display_mode_; 
 
 
 protected: 
  bool compute_min_max( std::pair<float, float>& min_max,  
			const GsTLGridProperty* property ); 
 
}; 
 
#endif 
