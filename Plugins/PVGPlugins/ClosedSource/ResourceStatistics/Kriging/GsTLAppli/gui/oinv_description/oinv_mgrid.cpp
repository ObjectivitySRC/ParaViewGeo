/**********************************************************************
** Author: Ting Li
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

#include <GsTLAppli/gui/oinv_description/oinv_mgrid.h>
#include <GsTLAppli/gui/oinv_description/oinv_slice.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoClipPlaneManip.h>
#include <GsTLAppli/gui/utils/colorscale.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <GsTL/math/math_functions.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/manips/SoClipPlaneManip.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/manips/SoHandleBoxManip.h>

#include <VolumeViz/nodes/SoTransferFunction.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoVolumeSkin.h>
#include <VolumeViz/nodes/SoOrthoSlice.h>

#include <qstring.h>
#include <qstringlist.h>

#include <iterator>
#include <algorithm>
#include <sstream> 



#include <fstream>

Named_interface* Oinv_mgrid::create_new_interface( std::string& ) {
  return new Oinv_mgrid;
}




Oinv_mgrid::Oinv_mgrid() : Oinv_cgrid()
{
}
Oinv_mgrid::~Oinv_mgrid(){}

int Oinv_mgrid::add_slice( Oinv::Axis axis, int position, 
			   bool showed ) {

  GsTLCoordVector cell_dims = grid_->cell_dimensions();

  // notice that the origin returned by the grid is the center 
  // of the origin cell, not its corner (cell-centered grid)
  GsTLPoint origin = grid_->origin();
  
  GsTLVector<int> grid_sizes( grid_->nx(), grid_->ny(), grid_->nz() );
  int max_pos = grid_sizes[axis]-1;

  Oinv_slice_cgrid* new_slice = 
    new Oinv_slice_cgrid( axis, position, grid_sizes, 
                          voxel_data_, &initialized_, cmap_,
			                    cell_dims[0], cell_dims[1], cell_dims[2],
			                    origin.x(), origin.y(), origin.z() );

  new_slice->property_display_mode( property_display_mode_ );
  GsTL_SoNode* node = new_slice->oinv_node();
  node->visible = showed;
  //new_slice->oinv_node()->visible = showed;

  slices_.push_back( new_slice );
  slices_node_->addChild( new_slice->oinv_node() );

  return slices_.size()-1;
}

void Oinv_mgrid::init( const Geostat_grid* grid ) {
  geostat_grid_ = grid;
  grid_ = dynamic_cast<const Reduced_grid*>( grid );

  SbVec3s dim = SbVec3s( grid_->nx(), grid_->ny(), grid_->nz() );

  voxel_data_ = new uint8_t[ grid_->trueSize() ];
  initialized_ = false;


  painted_switch_ = new SoSwitch;
  oinv_node_->addChild( painted_switch_ );

  // display the bounding box if no property is painted
  GsTLCoordVector cell_dims = grid_->cell_dimensions();
  GsTLPoint origin = grid_->origin();

  OinvBBox* bbox = 
    new OinvBBox( origin.x()-cell_dims.x()/2, origin.y()-cell_dims.y()/2,
				          origin.z()-cell_dims.z()/2,
                  float(grid_->nx())*cell_dims.x(), 
                  float(grid_->ny())*cell_dims.y(),
                  float(grid_->nz())*cell_dims.z() );
  painted_switch_->addChild( bbox->oinv_node() );



  /* When a property is painted, we can show:
   * 1- the bounding box
   * 2- the outer planes of the volume ("full volume")
   * 3- volume rendering
   * 4- slices
   * Options (2) and (3) are mutually exclusive.
   * All these options are contained inside the same separator,
   * called 'painted_main_separator'
  */ 
  SoSeparator* painted_main_separator = new SoSeparator;
  painted_switch_->addChild( painted_main_separator );


  //-------------------------
  // bounding box node
  bbox_switch_ = new GsTL_SoNode;
  painted_main_separator->addChild( bbox_switch_ );
  bbox_switch_->addChild( bbox->oinv_node() );
  bbox_switch_->visible = false;


  display_switch_ = new SoSwitch;
  painted_main_separator->addChild( display_switch_ );


  //-------------------------
  // Volume rendering node

  SoSeparator * vol_rendering_and_slices_root = new SoSeparator;
  display_switch_->addChild( vol_rendering_and_slices_root );

  SoSeparator * vol_rendering_root = new SoSeparator;
  vol_rendering_and_slices_root->addChild( vol_rendering_root );

  clipplane_switch_ = new SoSwitch;
  vol_rendering_root->addChild( clipplane_switch_ );
  setup_clipplanes();

  /* 
  SbBox3f vol_size( origin.x(), origin.y(), origin.z(),
                    origin.x()+float(grid_->nx())*cell_dims.x(),
                    origin.y()+float(grid_->ny())*cell_dims.y(),
                    origin.z()+float(grid_->nz())*cell_dims.z() );
  */
  SbBox3f vol_size( origin.x()-cell_dims.x()/2,
                    origin.y()-cell_dims.y()/2,
                    origin.z()-cell_dims.z()/2,
                    origin.x()-cell_dims.x()/2+float(grid_->nx())*cell_dims.x(),
                    origin.y()-cell_dims.y()/2+float(grid_->ny())*cell_dims.y(),
                    origin.z()-cell_dims.z()/2+float(grid_->nz())*cell_dims.z() );
 
  // Add SoVolumeData to scene graph
  volume_data_ = new SoVolumeData();
  volume_data_->setVolumeData(dim, voxel_data_, SoVolumeData::UNSIGNED_BYTE);
  volume_data_->setVolumeSize( vol_size );
  vol_rendering_root->addChild(volume_data_);
  
  // Add TransferFunction (color map) to scene graph
  volrend_colormap_ = new SoTransferFunction();  
  volrend_colormap_->predefColorMap.setValue(SoTransferFunction::NONE);
  volrend_colormap_->colorMapType.setValue(SoTransferFunction::RGBA);
  vol_rendering_root->addChild( volrend_colormap_ );
  
  // Add VolumeRender to scene graph
  rendering_switch_ = new SoSwitch;
  SoVolumeRender * volrend = new SoVolumeRender();
  volrend->interpolation = SoVolumeRender::NEAREST;
  if( requires_manual_override( float(grid_->nx())*cell_dims.x(), 
                                float(grid_->ny())*cell_dims.y(),
                                float(grid_->nz())*cell_dims.z() ) ) {
	  volrend->numSlicesControl.setValue(SoVolumeRender::MANUAL);
    int num_slices = int( float(grid_->nx())*cell_dims.x() );
    num_slices += int( float(grid_->ny())*cell_dims.y() );
    num_slices += int( float(grid_->nz())*cell_dims.z() );
	  volrend->numSlices.setValue(num_slices);
  }
  
  rendering_switch_->addChild(volrend);
  rendering_switch_->whichChild = 0;
  
  vol_rendering_root->addChild(rendering_switch_);


  //-------------------------
  // Slices

  SoSeparator * slices_root = new SoSeparator;
  vol_rendering_and_slices_root->addChild( slices_root );

  slices_node_ = new SoGroup;
  slices_root->addChild( slices_node_ );



  //-------------------------
  // Full volume node

  SoSeparator * full_vol_root = new SoSeparator;
  display_switch_->addChild( full_vol_root );
  
  full_volume_ = 
    new Full_volume( grid_->nx(), grid_->ny(), grid_->nz(),
                     voxel_data_, &initialized_, cmap_, 
                     cell_dims.x(), cell_dims.y(), cell_dims.z(),
                     origin.x(), origin.y(), origin.z() );

  full_vol_root->addChild( full_volume_->oinv_node() );

  //-------------------------
  display_switch_->whichChild = 1;
  //painted_switch_->whichChild = 0;


  property_display_mode( Oinv::NOT_PAINTED );

}

/*
void Oinv_mgrid::init( const Geostat_grid* grid ) {
  geostat_grid_ = grid;
  grid_ = dynamic_cast<const Reduced_grid*>( grid );

  SbVec3s dim = SbVec3s( grid_->nx(), grid_->ny(), grid_->nz() );

  voxel_data_ = new uint8_t[ grid_->trueSize() ];

  initialized_ = false;

  painted_switch_ = new SoSwitch;
  oinv_node_->addChild( painted_switch_ );

  // display the bounding box if no property is painted
  GsTLCoordVector cell_dims = grid_->cell_dimensions();
  GsTLPoint origin = grid_->origin();

  OinvBBox* bbox = 
    new OinvBBox( origin.x()-cell_dims.x()/2, origin.y()-cell_dims.y()/2,
				          origin.z()-cell_dims.z()/2,
                  float(grid_->nx())*cell_dims.x(), 
                  float(grid_->ny())*cell_dims.y(),
                  float(grid_->nz())*cell_dims.z() );
  painted_switch_->addChild( bbox->oinv_node() );

  SoSeparator* painted_main_separator = new SoSeparator;
  painted_switch_->addChild( painted_main_separator );

 //-------------------------
  // bounding box node
  bbox_switch_ = new GsTL_SoNode;
  painted_main_separator->addChild( bbox_switch_ );
  bbox_switch_->addChild( bbox->oinv_node() );
  bbox_switch_->visible = false;

  // What to display if a property is painted
  display_switch_ = new SoSwitch;
  painted_switch_->addChild( display_switch_ );


  //-------------------------
  // Volume rendering node

  SoSeparator * vol_rendering_and_slices_root = new SoSeparator;
  display_switch_->addChild( vol_rendering_and_slices_root );

  SoSeparator * vol_rendering_root = new SoSeparator;
  vol_rendering_and_slices_root->addChild( vol_rendering_root );

  clipplane_switch_ = new SoSwitch;
  vol_rendering_root->addChild( clipplane_switch_ );
  setup_clipplanes();

  SbBox3f vol_size( origin.x()-cell_dims.x()/2,
                    origin.y()-cell_dims.y()/2,
                    origin.z()-cell_dims.z()/2,
                    origin.x()-cell_dims.x()/2+float(grid_->nx())*cell_dims.x(),
                    origin.y()-cell_dims.y()/2+float(grid_->ny())*cell_dims.y(),
                    origin.z()-cell_dims.z()/2+float(grid_->nz())*cell_dims.z() );
 
  // Add SoVolumeData to scene graph
  volume_data_ = new SoVolumeData();
  volume_data_->setVolumeData(dim, voxel_data_, SoVolumeData::UNSIGNED_BYTE);
  volume_data_->setVolumeSize( vol_size );
  vol_rendering_root->addChild(volume_data_);
  
  // Add TransferFunction (color map) to scene graph
  volrend_colormap_ = new SoTransferFunction();  
  volrend_colormap_->predefColorMap.setValue(SoTransferFunction::NONE);
  volrend_colormap_->colorMapType.setValue(SoTransferFunction::RGBA);
  vol_rendering_root->addChild( volrend_colormap_ );
  
  // Add VolumeRender to scene graph
  rendering_switch_ = new SoSwitch;
  SoVolumeRender * volrend = new SoVolumeRender();
  volrend->interpolation = SoVolumeRender::NEAREST;
  if( requires_manual_override( float(grid_->nx())*cell_dims.x(), 
                                float(grid_->ny())*cell_dims.y(),
                                float(grid_->nz())*cell_dims.z() ) ) {
	  volrend->numSlicesControl.setValue(SoVolumeRender::MANUAL);
    int num_slices = int( float(grid_->nx())*cell_dims.x() );
    num_slices += int( float(grid_->ny())*cell_dims.y() );
    num_slices += int( float(grid_->nz())*cell_dims.z() );
	  volrend->numSlices.setValue(num_slices);
  }
  
  rendering_switch_->addChild(volrend);
  rendering_switch_->whichChild = 0;
  
  vol_rendering_root->addChild(rendering_switch_);


  //-------------------------
  // Slices

  SoSeparator * slices_root = new SoSeparator;
  vol_rendering_and_slices_root->addChild( slices_root );

  slices_node_ = new SoGroup;
  slices_root->addChild( slices_node_ );



  //-------------------------
  // Full volume node

  SoSeparator * full_vol_root = new SoSeparator;
  display_switch_->addChild( full_vol_root );
  
  full_volume_ = 
    new Full_volume( grid_->nx(), grid_->ny(), grid_->nz(),
                     voxel_data_, &initialized_, cmap_, 
                     cell_dims.x(), cell_dims.y(), cell_dims.z(),
                     origin.x(), origin.y(), origin.z() );

  full_vol_root->addChild( full_volume_->oinv_node() );

  //-------------------------
  display_switch_->whichChild = 1;
  //painted_switch_->whichChild = 0;


  property_display_mode( Oinv::NOT_PAINTED );

}
*/
void Oinv_mgrid::refresh() {

  int low=1;
  int high=65535;
  int true_size;

  if( current_property_ && cmap_ ) {
    // recompute the voxel data
    if( !voxel_data_ ) 
      voxel_data_ = new uint8_t[grid_->trueSize() ];

    float min = cmap_->lower_bound();
    float max = cmap_->upper_bound();
    
  
    for( int i=0; i < grid_->trueSize()  ; i++ ) {
		GsTLInt rindex = grid_->full2reduced(i);
		if (rindex == -1) {voxel_data_[i] = 0; continue;}
		if (current_property_->is_informed(rindex)) {
			float pval= std::max( std::min( current_property_->get_value(rindex), max ), min );
			uint8_t val = (pval - min) / (max-min)*254+1;        
			voxel_data_[i] = val; 
		}
		else
			voxel_data_[i] = 0;
	}

    initialized_ = true;   // voxel_data_ is initialized

    // set the TransferFunctions

   	const Color_scale* scale = cmap_->color_scale();
    for( int j=0; j < scale->colors_count(); j++ ) {
      float r,g,b;
      scale->color( j, r,g,b );
      volrend_colormap_->colorMap.set1Value(4*j, r); 
      volrend_colormap_->colorMap.set1Value(4*j+1, g); 
      volrend_colormap_->colorMap.set1Value(4*j+2, b);
      // the transparency field is omitted on purpose. It will be taken
      // care of by the call to set_transparency(...)

  	}
    set_transparency();

  }

  volume_data_->touch();
  full_volume_->update();
  for( SliceList_iterator it = slices_.begin(); it != slices_.end(); ++it ) {
    (*it)->update();
  }

  volrend_colormap_->reMap(low, high);

}

