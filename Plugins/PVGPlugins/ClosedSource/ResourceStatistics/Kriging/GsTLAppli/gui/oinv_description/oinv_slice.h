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

#ifndef __GSTLAPPLI_GUI_OINV_SLICE_H__ 
#define __GSTLAPPLI_GUI_OINV_SLICE_H__ 

/*
// There is a problem, even if defined in preprocessor
// SIMVOLEON_DLL is not recognized from there
// This should be fixed
#ifndef SIMVOLEON_DLL
  #define SIMVOLEON_DLL
#endif
*/ 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/oinv_description/oinv_common.h> 
#include <GsTLAppli/math/gstlvector.h> 
 
#include <Inventor/nodes/SoSwitch.h> 
 
#include <vector> 
#include <utility> 
 
class SGrid_cursor; 
class GsTL_SoNode;
class Colormap;
 
class SoSeparator; 
class SoMaterial; 
class SoCoordinate3; 
class SoTransform; 
class SoDragger; 
class SoTexture2;
class SoTexture2Transform;
 
class Texture_node; 
class SoOrthoSlice; 
 
 
void slice_translation_callback( void* data, SoDragger* ); 
 
 
class GUI_DECL Oinv_slice { 
   
  friend void slice_translation_callback( void* data, SoDragger* ); 
 
 public: 
 
  Oinv_slice( const GsTLVector<int>& grid_dimensions,
              const uint8_t* std_values_, const bool* initialized,
              Colormap*& cmap, 
              Oinv::Axis axis, int pos ); 
  virtual ~Oinv_slice() ; 
 
  void property_display_mode( Oinv::Property_display_mode mode ); 
  virtual GsTL_SoNode* oinv_node() { return root_; } 
  virtual void update() = 0; 
  virtual void translate( int step ) = 0; 
  virtual void position( int new_pos ) = 0; 
 
  int position() const { return pos_; } 
  int max_position() const { return max_pos_; } 
 
 protected: 
  GsTLVector<int> grid_dimensions_;
  const Oinv::Axis axis_; 
  int pos_, initial_pos_; 
  int max_pos_; 

  GsTLVector<int> slice_normal_;
  SGrid_cursor* cursor_;

  const uint8_t* std_values_;
  const bool* initialized_;
  Colormap*& cmap_;
  Texture_node* texture_;

  GsTL_SoNode* root_; 
  SoSeparator* slice_root_; 
 
}; 
 
 
 
class Slice_plane; 
 
class GUI_DECL Oinv_slice_cgrid : public Oinv_slice { 
 public: 
  /** @param nx, ny, nz are the number of faces in the slice 
   * @param xsize, etc are the dimension of a face 
   * @param xmn, ymn, zmn are the coordinates of the center of the first face 
   */ 
  Oinv_slice_cgrid( Oinv::Axis axis, int pos,
                    const GsTLVector<int>& grid_dimensions,
                    const uint8_t* std_values, const bool* initialized,
                    Colormap*& cmap,
		                float xsize, float ysize, float zsize, 
		                float xmn, float ymn, float zmn ); 
 
  virtual ~Oinv_slice_cgrid(); 
 
  virtual void update(); 
  virtual void translate( int step ); 
  virtual void position( int new_pos ); 
 
 protected: 
  float xsize_, ysize_, zsize_;
  float xmn_, ymn_, zmn_;
  
  SoTransform* global_translation_;
  Slice_plane* plane_;


 private: 
  void setup_slice_root(); 
}; 
 
 
 
 
class GUI_DECL Slice_plane { 
 public: 
  Slice_plane( int vrow=0, int vcol=0 ); 
  virtual ~Slice_plane(); 
  virtual SoSeparator* oinv_node() { return plane_node_; } 

 protected: 
  SoSeparator* plane_node_; 
}; 
 
 
class GUI_DECL X_plane : public Slice_plane { 
 public: 
  /** @param nx, ny, nz are the number of faces in each direction  
   */ 
  X_plane( int ny, int nz, 
	   float ysize, float zsize, 
	   float x_start, float y_start, float z_start ); 
}; 
 
class GUI_DECL Y_plane : public Slice_plane { 
public: 
  Y_plane( int nx, int nz, 
	   float xsize, float zsize, 
	   float x_start, float y_start, float z_start ); 
}; 
 
class GUI_DECL Z_plane : public Slice_plane { 
public: 
  Z_plane( int nx, int ny, 
	   float xsize, float ysize, 
	   float x_start, float y_start, float z_start ); 
}; 
 
 
 
 
class GUI_DECL Texture_node { 
 
 public: 
  Texture_node( const std::pair<GsTLVector<int>, GsTLVector<int> >& bound_pairs,  
		            const uint8_t* std_values, const bool* initialized,
                Colormap*& cmap,
		            const SGrid_cursor& cursor ); 
  ~Texture_node(); 
 
  void property_display_mode( Oinv::Property_display_mode mode ) ; 
  /** Recomputes the SoMaterial for the new position 
   * In a single slice, all the texture nodes are translated by the 
   * SAME vector.  
   */ 
  void translate( const GsTLVector<int>& vec ); 
  SoSwitch* oinv_node() { return material_switch_; } 
 
  void update_texture(); 
 
 protected:
   void compute_image();
   void compute_one_pixel( unsigned char* image, 
                           int i, int j, int k, int x, int y );
   std::pair<int,int> init_image( int nx, int ny );
   int closest_power_of_2( int x );
   int pixel_id( int i, int j, int nx );

 private: 
  SoSwitch* material_switch_; 
 
  /** The first vector contains the starting (x,y,z) indices for the slice 
   * The second vector contains the stoping indices. 
   */ 
  std::pair<GsTLVector<int>, GsTLVector<int> > bounds_; 
   
  const uint8_t* std_values_; 
  const bool* initialized_;

  Colormap*& cmap_;
  const SGrid_cursor& cursor_; 

  std::pair<int,int> new_dims_;
  std::pair<int,int> old_dims_;

  int components_;
  unsigned char* image_;

  SoTexture2* texture_;
  SoTexture2Transform* texture_transf_;
}; 
 
#endif 
