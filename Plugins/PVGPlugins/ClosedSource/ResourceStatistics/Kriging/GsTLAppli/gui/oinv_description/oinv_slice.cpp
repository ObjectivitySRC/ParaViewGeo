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

#include <GsTLAppli/gui/oinv_description/oinv_slice.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/nodes/SoTextureScalePolicy.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>

#include <VolumeViz/nodes/SoOrthoSlice.h>

#include <iostream>
#include <numeric>



void slice_translation_callback( void* data, SoDragger* dragger ) {
  Oinv_slice* slice = (Oinv_slice*) data;
  float x,y,z;
  SoTranslate1Dragger *dragger1D = dynamic_cast<SoTranslate1Dragger *>(dragger);
  dragger1D->translation.getValue().getValue(x,y,z);

  if( x > slice->max_pos_ - slice->initial_pos_ )
    x = slice->max_pos_ - slice->initial_pos_;
  if( x < - slice->initial_pos_ )
    x= - slice->initial_pos_;

  slice->translate( static_cast<int>( x - slice->pos_ ) );
  dragger1D->translation.setValue( static_cast<int>(x), 0, 0);
}

//==========================


Oinv_slice::Oinv_slice( const GsTLVector<int>& grid_dimensions,
                        const uint8_t* std_values, const bool* initialized,
                        Colormap*& cmap,
                        Oinv::Axis axis, int pos ) 
  :  grid_dimensions_(grid_dimensions),
     std_values_( std_values ), initialized_(initialized),
     cmap_(cmap),
     axis_( axis ), pos_( pos ),
     slice_normal_( 0, 0, 0 ) {

  slice_normal_[axis] = 1;
  max_pos_ = grid_dimensions_[axis]-1;

  // make sure 0 <= pos <= max_pos
  pos_ = std::min( pos_, max_pos_ );
  pos_ = std::max( pos_, 0 );
  initial_pos_ = pos_;

  cursor_ = new SGrid_cursor( grid_dimensions_[0],
                              grid_dimensions_[1],
                              grid_dimensions_[2] );

  // compute the bound indices of the back plane
  GsTLVector<int> vec(0,0,0);
  for( int i=0; i < 3 ; i++ ) {
    vec[i] = (1-slice_normal_[i]) * grid_dimensions_[i];
  }
  GsTLVector<int> start= pos*slice_normal_;
  GsTLVector<int> stop = vec + (pos+1)*slice_normal_;

  texture_ = new Texture_node( std::make_pair( start, stop ),
                               std_values_, initialized_, cmap, *cursor_ );

  root_ = new GsTL_SoNode;
  root_->ref();

  slice_root_ = new SoSeparator;
  root_->addChild( slice_root_ );
}
    


Oinv_slice::~Oinv_slice() {
  root_->unref();
}



void Oinv_slice::property_display_mode( Oinv::Property_display_mode mode ) {
 texture_->property_display_mode( mode );
}








//===============================================


Oinv_slice_cgrid::
Oinv_slice_cgrid( Oinv::Axis axis, int pos,
                  const GsTLVector<int>& grid_dimensions,
                  const uint8_t* std_values, const bool* initialized,
                  Colormap*& cmap,
			      	    float xsize, float ysize, float zsize,
				          float xmn, float ymn, float zmn )
  :  Oinv_slice( grid_dimensions, std_values, initialized, cmap, axis, pos ),
     xmn_(xmn), ymn_(ymn), zmn_(zmn),
     xsize_( xsize ), ysize_( ysize ), zsize_( zsize ) {

  int nx = grid_dimensions.x();
  int ny = grid_dimensions.y();
  int nz = grid_dimensions.z();

  global_translation_ = new SoTransform;
  slice_root_->insertChild( global_translation_, 0 );

  if( axis_ == Oinv::X_AXIS ) {
    plane_ = new X_plane( ny, nz, ysize_, zsize_,
                          xmn_ - 0.5*xsize_ + pos_*xsize_,
                          ymn_ - 0.5*ysize_, 
                          zmn_ - 0.5*zsize_ );
  }
  if( axis_ == Oinv::Y_AXIS ) {
    plane_ = new Y_plane( nx, nz, xsize_, zsize_,
                          xmn_ - 0.5*xsize_,
                          ymn_ - 0.5*ysize_ + pos_*ysize_,
                          zmn_ - 0.5*zsize_ );
  }
  if( axis_ == Oinv::Z_AXIS ) {
    plane_ = new Z_plane( nx, ny, xsize_, ysize_,
                          xmn_ - 0.5*xsize_,
                          ymn_ - 0.5*ysize_,
                          zmn_ - 0.5*zsize_ + pos_*zsize_ );
  }

  setup_slice_root();
}


Oinv_slice_cgrid::~Oinv_slice_cgrid() { 
  delete plane_;
}


void Oinv_slice_cgrid::setup_slice_root() {
  SoShapeHints* hints = new SoShapeHints; 
  hints->vertexOrdering = SoShapeHints::CLOCKWISE;
  //hints->faceType = SoShapeHints::CONVEX;
  //hints->creaseAngle = 0;
  //hints->shapeType = SoShapeHints::SOLID; 
  slice_root_->addChild( hints );

  slice_root_->addChild( texture_->oinv_node() );
  slice_root_->addChild( plane_->oinv_node() );

}



void Oinv_slice_cgrid::update() {
  texture_->update_texture();
}



void Oinv_slice_cgrid::translate( int step ) {
  if( step == 0 ) return;

  int new_pos = pos_ + step;
  new_pos = std::min( new_pos, max_pos_ );
  new_pos = std::max( new_pos, 0 );

  // The translation vector for a SoTransform is the translation from the initial
  // position, not from the current position.
  //   transl is the translation from the current position
  //   transl2 is the translation from the initial position
  GsTLVector<int> transl = ( new_pos - pos_ ) * slice_normal_ ;
  GsTLVector<int> transl2 = ( new_pos - initial_pos_ ) * slice_normal_ ;

  //  global_translation_->translation.setValue( transl2[0], transl2[1], transl2[2] );
  global_translation_->translation.setValue( transl2[0] * xsize_,
                                             transl2[1] * ysize_,
                                             transl2[2] * zsize_ );
  texture_->translate( transl );

  pos_ = new_pos;
}



void Oinv_slice_cgrid::position( int new_pos ) {
  new_pos = std::min( new_pos, max_pos_ );
  new_pos = std::max( new_pos, 0 );
 
  if( new_pos == pos_ ) return;

  // The translation vector for a SoTransform is the translation from the initial
  // position, not from the current position.
  //   transl is the translation from the current position
  //   transl2 is the translation from the initial position
  GsTLVector<int> transl = ( new_pos - pos_ ) * slice_normal_ ;
  GsTLVector<int> transl2 = ( new_pos - initial_pos_ ) * slice_normal_ ;

  global_translation_->translation.setValue( transl2[0] * xsize_,
                                             transl2[1] * ysize_,
                                             transl2[2] * zsize_ );
  texture_->translate( transl );

  pos_ = new_pos;

}


//================================================

Slice_plane::Slice_plane( int vrow, int vcol ) {
  plane_node_ = new SoSeparator;

  //------------------
  // define the texture coordinates
  SoTextureCoordinate2* tex_coords = new SoTextureCoordinate2;
  tex_coords->point.set1Value( 0, SbVec2f(0,0) );
  tex_coords->point.set1Value( 1, SbVec2f(1,0) );
  tex_coords->point.set1Value( 2, SbVec2f(1,1) );
  tex_coords->point.set1Value( 3, SbVec2f(0,1) );

  plane_node_->addChild( tex_coords );

  SoTextureCoordinateBinding* tbind = new SoTextureCoordinateBinding;
  tbind->value.setValue( SoTextureCoordinateBinding::PER_VERTEX );
  plane_node_->addChild( tbind );
  //----------------
  
  SoFaceSet* faceset = new SoFaceSet;
  plane_node_->addChild( faceset );
  faceset->numVertices.set1Value( 0, 4 );

  plane_node_->ref();
}


Slice_plane::~Slice_plane() {
  plane_node_->unref();
}



X_plane::X_plane( int ny, int nz,
		  float ysize, float zsize,
		  float x_start, float y_start, float z_start )
  : Slice_plane() {

  float x_end = x_start;
  float y_end = y_start + ny * ysize;
  float z_end = z_start + nz * zsize;

  SoCoordinate3* coords_node_ = new SoCoordinate3;

  coords_node_->point.set1Value( 0, SbVec3f( x_start, y_start, z_start ) );
  coords_node_->point.set1Value( 1, SbVec3f( x_start, y_end, z_start ) );
  coords_node_->point.set1Value( 2, SbVec3f( x_start, y_end, z_end ) );
  coords_node_->point.set1Value( 3, SbVec3f( x_start, y_start, z_end ) );

  plane_node_->insertChild( coords_node_, 0 );


}





Y_plane::Y_plane( int nx, int nz,
		  float xsize, float zsize,
		  float x_start, float y_start, float z_start )
  : Slice_plane( nx+1, nz+1 ) {

  float x_end = x_start + nx * xsize;
  float y_end = y_start;
  float z_end = z_start + nz * zsize;

  SoCoordinate3* coords_node_ = new SoCoordinate3;

  coords_node_->point.set1Value( 0, SbVec3f( x_start, y_start, z_start ) );
  coords_node_->point.set1Value( 1, SbVec3f( x_end, y_start, z_start ) );
  coords_node_->point.set1Value( 2, SbVec3f( x_end, y_start, z_end ) );
  coords_node_->point.set1Value( 3, SbVec3f( x_start, y_start, z_end ) );

  plane_node_->insertChild( coords_node_, 0 );
}



Z_plane::Z_plane( int nx, int ny,
		  float xsize, float ysize,
		  float x_start, float y_start, float z_start )
  : Slice_plane( nx+1, ny+1 ) {

  float x_end = x_start + nx * xsize;
  float y_end = y_start + ny * ysize;
  float z_end = z_start;

  SoCoordinate3* coords_node_ = new SoCoordinate3;

  coords_node_->point.set1Value( 0, SbVec3f( x_start, y_start, z_start ) );
  coords_node_->point.set1Value( 1, SbVec3f( x_end, y_start, z_start ) );
  coords_node_->point.set1Value( 2, SbVec3f( x_end, y_end, z_start ) );
  coords_node_->point.set1Value( 3, SbVec3f( x_start, y_end, z_start ) );

  plane_node_->insertChild( coords_node_, 0 );
}




//==============================================
/* The following implementation uses power of 2 texture sizes
*/
/*
Texture_node::
Texture_node( const std::pair< GsTLVector<int>,GsTLVector<int> >& bound_pairs,
              const std::vector<RGB_color>& color_block,
              const SGrid_cursor& cursor ) 
  : bounds_( bound_pairs ),
    color_block_( color_block ), 
    cursor_( cursor ) {
  
  new_dims_ = std::make_pair( 0, 0 );
  old_dims_ = std::make_pair( 0, 0 );
  components_ = 0;
  image_ = 0;

  size_ = 1;
  for( int i=0; i<3; i++ ) {
    size_ *= bounds_.second[i] - bounds_.first[i];
  }

  // set up the SoMaterial
  material_switch_ = new SoSwitch;
  material_switch_->ref();

  //-----------------------
  // set up the "texture" group displayed when no property is painted
  SoGroup* default_material_group = new SoGroup;
  SoMaterial* default_ = new SoMaterial;
  default_->diffuseColor.setValue( Oinv::nodata_color.red(), 
				                           Oinv::nodata_color.green(),
				                           Oinv::nodata_color.blue() );
  default_->ambientColor.setValue( .2, .2, .2 );
  default_->specularColor.setValue( .5, .5, .5 );
  default_->shininess = .5;
  
  default_material_group->addChild( default_ );


  SoMaterialBinding* default_binding = new SoMaterialBinding;
  default_binding->value = SoMaterialBinding::OVERALL ;

  default_material_group->addChild( default_binding );

  
  //-----------------------
  // set up the actual texture group
  SoGroup* texture_group = new SoGroup;
  
  // adding this material group allows to have white textures that look
  // actually white... It doesn't have to be the same material node than
  // the one set-up above; here the same one is used for convenience.
  texture_group->addChild( default_material_group );

  components_ = 3; // R,G,B, no transparency
  
  // set texture quality to 0.01 to disable interpolation
  SoComplexity* texture_complexity = new SoComplexity;
  texture_complexity->textureQuality = 0.01;
    
  texture_transf_ = new SoTexture2Transform;
  texture_ = new SoTexture2;
  
  texture_group->addChild( texture_complexity );
  texture_group->addChild( texture_transf_ );
  texture_group->addChild( texture_ );


  material_switch_->addChild( default_material_group );
  material_switch_->addChild( texture_group );

  // by default show the "default" material, ie no property painted
  material_switch_->whichChild = 0;

  update_texture();
}


Texture_node::~Texture_node() {
  material_switch_->unref();
  
  delete [] image_;
}


void Texture_node::property_display_mode( Oinv::Property_display_mode mode ) {
  material_switch_->whichChild = int(mode); 
}


void Texture_node::translate( const GsTLVector<int>& vec ) {

  // If the translation is (0,0,0), we're done.
  if( vec[0]==0 && vec[1]==0 && vec[2]==0 ) return;

  for( int i=0; i<3 ; i++ ) {
    bounds_.first[i] += vec[i];
    bounds_.second[i] += vec[i];
  }
  update_texture();
}


void Texture_node::update_texture() {
  if( color_block_.empty() ) return;

  compute_image();
  
  int new_width = new_dims_.first;
  int new_height = new_dims_.second;
  int width = old_dims_.first;
  int height = old_dims_.second;

  texture_transf_->scaleFactor.setValue( double(width)/double(new_width),
                                         double(height)/double(new_height) );

  texture_->image.setValue( SbVec2s( new_width, new_height ), 
                            components_, (unsigned char*) image_ );
    
}



void
Texture_node::compute_image() {
  int count = 0;
  std::pair<int,int> tmp;

  if( bounds_.first[0] +1  == bounds_.second[0] ) {
    old_dims_.first = bounds_.second[1] - bounds_.first[1];
    old_dims_.second = bounds_.second[2] - bounds_.first[2];
    new_dims_ = init_image( old_dims_.first, old_dims_.second );
    
    for( int k = bounds_.first[2] ; k < bounds_.second[2]; k++ ) 
      for( int j = bounds_.first[1] ; j < bounds_.second[1]; j++ ) {
        compute_one_pixel( image_, bounds_.first[0], j, k, 
                           j-bounds_.first[1], k-bounds_.first[2]);
      }    
    return;
  }


  if( bounds_.first[1] +1  == bounds_.second[1] ) {
    old_dims_.first = bounds_.second[0] - bounds_.first[0];
    old_dims_.second = bounds_.second[2] - bounds_.first[2];
    new_dims_ = init_image( old_dims_.first, old_dims_.second );

    for( int k = bounds_.first[2] ; k < bounds_.second[2]; k++ ) 
      for( int i = bounds_.first[0] ; i < bounds_.second[0]; i++ ) {
        compute_one_pixel( image_, i, bounds_.first[1], k, 
                           i-bounds_.first[0], k-bounds_.first[2] );
      }
    return;
  }


  old_dims_.first = bounds_.second[0] - bounds_.first[0];
  old_dims_.second = bounds_.second[1] - bounds_.first[1];
  new_dims_ = init_image( old_dims_.first, old_dims_.second );

  for( int j = bounds_.first[1] ; j < bounds_.second[1]; j++ ) 
    for( int i = bounds_.first[0] ; i < bounds_.second[0]; i++ ) {
      compute_one_pixel( image_, i, j, bounds_.first[2], 
                         i-bounds_.first[0], j-bounds_.first[1] );
    }

}


void Texture_node::
compute_one_pixel( unsigned char* image, 
                   int i, int j, int k, int x, int y ) {
  int id = cursor_.node_id( i, j, k );
  int pixel = pixel_id( x,y, new_dims_.first );
  image[pixel++] = float(255) * color_block_[id].red();
  image[pixel++] = float(255) * color_block_[id].green();
  image[pixel] = float(255) * color_block_[id].blue();	
}





std::pair<int,int> 
Texture_node::init_image( int nx, int ny ) {
  int nx2 = closest_power_of_2( nx );
  int ny2 = closest_power_of_2( ny );
  int size = nx2 * ny2;
  if( !image_ ) {
    image_ = new unsigned char[ size * components_ ];
    for( int i = 0 ; i < size*components_ ; i++ )
      image_[i] = static_cast<unsigned char>( 0 );
  }

  return std::make_pair( nx2, ny2 );
}


int Texture_node::closest_power_of_2( int x ) {
  if( x == 1 ) return 2;

  double approx = std::log( double(x) ) / std::log( 2.0 );
  return std::pow( 2.0, std::ceil( approx ) );
}


int Texture_node::pixel_id( int i, int j, int nx ) {
  return components_ * (nx * j + i);
}
*/




//==============================================
/* The following implementation uses TextureScalePolicy to avoid using
*  textures of size a power of 2. 
* Unfortunately, off-screen rendering doesn't work properly with this
* implementation (as of v.2.2 of Coin)
*/

Texture_node::
Texture_node( const std::pair< GsTLVector<int>,GsTLVector<int> >& bound_pairs,
              const uint8_t* std_values, const bool* initialized,
              Colormap*& cmap,
              const SGrid_cursor& cursor ) 
  : bounds_( bound_pairs ),
    std_values_( std_values ), initialized_(initialized), cmap_(cmap),
    cursor_( cursor ) {
  
  new_dims_ = std::make_pair( 0, 0 );
  old_dims_ = std::make_pair( 0, 0 );
  components_ = 0;
  image_ = 0;

  // set up the SoMaterial
  material_switch_ = new SoSwitch;
  material_switch_->ref();

  //-----------------------
  // set up the "texture" group displayed when no property is painted
  SoGroup* default_material_group = new SoGroup;
  SoMaterial* default_ = new SoMaterial;
  default_->diffuseColor.setValue( Oinv::nodata_color.red(), 
				                           Oinv::nodata_color.green(),
				                           Oinv::nodata_color.blue() );
  default_->ambientColor.setValue( .2, .2, .2 );
  default_->specularColor.setValue( .5, .5, .5 );
  default_->shininess = .5;
  
  default_material_group->addChild( default_ );


  SoMaterialBinding* default_binding = new SoMaterialBinding;
  default_binding->value = SoMaterialBinding::OVERALL ;

  default_material_group->addChild( default_binding );

  
  //-----------------------
  // set up the actual texture group
  SoGroup* texture_group = new SoGroup;

  components_ = 3; // R,G,B, no transparency
  
  // use "Fracture" scale policy to avoid creating a texture of size a power of 2
  SoTextureScalePolicy* scale_policy = new SoTextureScalePolicy;
  scale_policy->policy = SoTextureScalePolicy::FRACTURE;
  
  // set texture quality to 0.01 to disable interpolation
  SoComplexity* texture_complexity = new SoComplexity;
  texture_complexity->textureQuality = 0.01;
    
  texture_transf_ = new SoTexture2Transform;
  texture_ = new SoTexture2;
  
  texture_group->addChild( scale_policy );
  texture_group->addChild( texture_complexity );
  texture_group->addChild( texture_transf_ );
  texture_group->addChild( texture_ );


  material_switch_->addChild( default_material_group );
  material_switch_->addChild( texture_group );

  // by default show the "default" material, ie no property painted
  material_switch_->whichChild = 0;

  update_texture();
}


Texture_node::~Texture_node() {
  material_switch_->unref();
  
  delete [] image_;
}


void Texture_node::property_display_mode( Oinv::Property_display_mode mode ) {
  material_switch_->whichChild = int(mode); 
}


void Texture_node::translate( const GsTLVector<int>& vec ) {

  // If the translation is (0,0,0), we're done.
  if( vec[0]==0 && vec[1]==0 && vec[2]==0 ) return;

  for( int i=0; i<3 ; i++ ) {
    bounds_.first[i] += vec[i];
    bounds_.second[i] += vec[i];
  }
  update_texture();
}


void Texture_node::update_texture() {
  if( !(*initialized_) || !cmap_) return;

  compute_image();
  
  int new_width = new_dims_.first;
  int new_height = new_dims_.second;
  int width = old_dims_.first;
  int height = old_dims_.second;

  texture_transf_->scaleFactor.setValue( double(width)/double(new_width),
                                         double(height)/double(new_height) );

  texture_->image.setValue( SbVec2s( new_width, new_height ), 
                            components_, (unsigned char*) image_ );
    
}



void
Texture_node::compute_image() {
  int count = 0;
  std::pair<int,int> tmp;

  if( bounds_.first[0] +1  == bounds_.second[0] ) {
    old_dims_.first = bounds_.second[1] - bounds_.first[1];
    old_dims_.second = bounds_.second[2] - bounds_.first[2];
    new_dims_ = init_image( old_dims_.first, old_dims_.second );
    
    for( int k = bounds_.first[2] ; k < bounds_.second[2]; k++ ) 
      for( int j = bounds_.first[1] ; j < bounds_.second[1]; j++ ) {
        compute_one_pixel( image_, bounds_.first[0], j, k, 
                           j-bounds_.first[1], k-bounds_.first[2]);
      }    
    return;
  }


  if( bounds_.first[1] +1  == bounds_.second[1] ) {
    old_dims_.first = bounds_.second[0] - bounds_.first[0];
    old_dims_.second = bounds_.second[2] - bounds_.first[2];
    new_dims_ = init_image( old_dims_.first, old_dims_.second );

    for( int k = bounds_.first[2] ; k < bounds_.second[2]; k++ ) 
      for( int i = bounds_.first[0] ; i < bounds_.second[0]; i++ ) {
        compute_one_pixel( image_, i, bounds_.first[1], k, 
                           i-bounds_.first[0], k-bounds_.first[2] );
      }
    return;
  }


  old_dims_.first = bounds_.second[0] - bounds_.first[0];
  old_dims_.second = bounds_.second[1] - bounds_.first[1];
  new_dims_ = init_image( old_dims_.first, old_dims_.second );

  for( int j = bounds_.first[1] ; j < bounds_.second[1]; j++ ) 
    for( int i = bounds_.first[0] ; i < bounds_.second[0]; i++ ) {
      compute_one_pixel( image_, i, j, bounds_.first[2], 
                         i-bounds_.first[0], j-bounds_.first[1] );
    }

}


void Texture_node::
compute_one_pixel( unsigned char* image, 
                   int i, int j, int k, int x, int y ) {
  int id = cursor_.node_id( i, j, k );
  int pixel = pixel_id( x,y, new_dims_.first );

  float r = Oinv::nodata_color.red();
  float g = Oinv::nodata_color.green();
  float b = Oinv::nodata_color.blue();
  
  if( std_values_[id] != 0 ) {
    Color_scale* c_scale = cmap_->color_scale();
    c_scale->color( std_values_[id], r,g,b );
  }

  image[pixel++] = float(255) * r;
  image[pixel++] = float(255) * g;
  image[pixel] = float(255) * b;	
}





std::pair<int,int> 
Texture_node::init_image( int nx, int ny ) {
  int size = nx * ny;
  if( !image_ ) {
    image_ = new unsigned char[ size * components_ ];
    for( int i = 0 ; i < size*components_ ; i++ )
      image_[i] = static_cast<unsigned char>( 0 );
  }

  return std::make_pair( nx, ny );
}


int Texture_node::closest_power_of_2( int x ) {
  if( x == 1 ) return 2;

  double approx = std::log( double(x) ) / std::log( 2.0 );
  return std::pow( double(2), std::ceil( approx ) );
}


int Texture_node::pixel_id( int i, int j, int nx ) {
  return components_ * (nx * j + i);
}


