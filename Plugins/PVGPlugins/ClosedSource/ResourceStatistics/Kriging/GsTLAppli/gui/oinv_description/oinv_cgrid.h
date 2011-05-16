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

#ifndef __GSTLAPPLI_GUI_OINV_CGRID_H__ 
#define __GSTLAPPLI_GUI_OINV_CGRID_H__ 
  
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/oinv_description/oinv_strati_grid.h> 
#include <GsTLAppli/gui/oinv_description/oinv_slice.h> 
#include <GsTLAppli/grid/grid_model/cartesian_grid.h> 
 
#include <Inventor/SbBox.h>

#include <map> 
 
class Full_volume; 
class SoVolumeData;
class SoTransferFunction;
class SoSeparator;
class SoGroup;




class GUI_DECL Oinv_cgrid : public Oinv_strati_grid { 
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
  static bool requires_manual_override( float x, float y, float z );

 public: 
  Oinv_cgrid(); 
  virtual ~Oinv_cgrid(); 
 
  virtual void init( const Geostat_grid* grid ); 
 
//  virtual void update( const std::string& property_name, 
//		                   Oinv::Update_reason reason ); 
 
  virtual int add_slice( Oinv::Axis axis, int position, bool visible ); 
  virtual void remove_slice( int slice_id ); 
  virtual void display_mode( Display_mode mode ); 
  virtual void property_display_mode( Oinv::Property_display_mode mode ); 

  virtual void show_bounding_box( bool on );
  virtual void hide_volume( bool on ) ;
  virtual void set_transparency( const QString& str ) ;
  virtual void set_transparency() ;
  virtual QString transparency_intervals( const std::string& property_name ) const;
  virtual Oinv_slice* slice( int slice_id ); 
 

  
 protected: 
  virtual void refresh();  
  virtual void property_deleted( const std::string& prop_name );

 
 protected: 
  typedef std::list< Oinv_slice* >::iterator SliceList_iterator; 
   
  const Cartesian_grid* grid_; 

  uint8_t* voxel_data_;
  bool initialized_;
  
  std::list< Oinv_slice* > slices_;
  Full_volume* full_volume_;

  SoVolumeData* volume_data_;
  SoTransferFunction * volrend_colormap_;
  
  // a transparency vector for each property
  std::map< std::string, QString > transparency_map_; 
  
  SoGroup* slices_node_; 
  SoSwitch* display_switch_;       // to alternate between different view modes
  SoSwitch* painted_switch_;       // to alternate between painted/not painted
  GsTL_SoNode* bbox_switch_;          // to show/hide the bounding box
  SoSwitch* clipplane_switch_; 
  SoSwitch* rendering_switch_; 
  void setup_clipplanes();

}; 
 
 
 
//======================================= 
 
/** This class draws a wireframe "cube"
*/
class GUI_DECL OinvBBox {
public:
	OinvBBox( float ox, float oy, float oz,
		float xsize, float ysize, float zsize );

	SoSeparator* oinv_node() { return root_; }

private:
	std::string tostring( const float array[3] ) const;

private:
	SoSeparator* root_;
};



class GUI_DECL OinvVolumeSkin {
public:
  OinvVolumeSkin( unsigned int nx, unsigned int ny, unsigned int nz );
  SoSeparator* oinv_node();

private:
  SoSeparator* skin_root_;
};



class GUI_DECL Cgrid_probe {
public:
  Cgrid_probe(SbBox3f box );
  ~Cgrid_probe();

  SoGroup* oinv_node() { return root_; }

private:
  SoGroup* root_;
};


//================================



class GUI_DECL Full_volume { 
 public: 
  Full_volume( int nx, int ny, int nz, 
	       const uint8_t* std_values, const bool* initialized,
         Colormap*& cmap, 
	       float xsize, float ysize, float zsize, 
	       float xmn, float ymn, float zmn ); 
  ~Full_volume(); 
  SoSeparator* oinv_node() { return root_; } 
  void update(); 
  void property_display_mode( Oinv::Property_display_mode mode ) ; 
 
 private: 
  SoSeparator* root_; 
  Texture_node* textures_[6]; 
  // the GsTLCoordVector part of the pair is the translation vector to  
  // get the other end of the slice.  
  std::pair<Slice_plane*, GsTLCoordVector> planes_[3]; 
 
  SGrid_cursor* cursor_; 
}; 
 
 
 
 
 
#endif 
