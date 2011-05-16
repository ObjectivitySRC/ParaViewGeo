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

#include <GsTLAppli/gui/oinv_description/oinv_cgrid.h>
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
 


Named_interface* Oinv_cgrid::create_new_interface( std::string& ) {
  return new Oinv_cgrid;
}

bool Oinv_cgrid::requires_manual_override( float x, float y, float z ) {
	const float min_ratio = 0.05;
  if( x/y < min_ratio || y/x < min_ratio ) return true;
  if( x/z < min_ratio || z/x < min_ratio ) return true;
  if( z/y < min_ratio || y/z < min_ratio ) return true;

	return false;
}

Oinv_cgrid::Oinv_cgrid() 
  : Oinv_strati_grid(),
    grid_( 0 ),
    voxel_data_( 0 ), initialized_(false),
    volume_data_( 0 ),
    display_switch_( 0 ),
    bbox_switch_(0) {

}
 


void Oinv_cgrid::setup_clipplanes() {
  /*
  GsTLCoordVector cell_dims = grid_->cell_dimensions();
  GsTLPoint origin = grid_->origin();

  float x_min=origin.x();
  float y_min=origin.y();
  float z_min=origin.z();
  float x_max=origin.x()+float(grid_->nx())*cell_dims.x();
  float y_max=origin.y()+float(grid_->ny())*cell_dims.y();
  float z_max=origin.z()+float(grid_->nz())*cell_dims.z();

  
  
  SoClipPlaneManip * clipplane_z1 = new SoClipPlaneManip;
  clipplane_switch_->addChild( clipplane_z1 );

  SbBox3f clip_box_z1( x_min, y_min, z_min,
                       x_max, y_max, z_min+cell_dims.z() );
  clipplane_z1->setValue(clip_box_z1, SbVec3f(0, 0, 1.0f), 0.5f );


  clipplane_switch_->whichChild = 0;
*/
  /*
  GsTL_SoClipPlaneManip * clipplane_x1 = new GsTL_SoClipPlaneManip;
//  clipplane_switch_->addChild( clipplane_x1 );
  GsTL_SoClipPlaneManip * clipplane_x2 = new GsTL_SoClipPlaneManip;
//  clipplane_switch_->addChild( clipplane_x2 );
  GsTL_SoClipPlaneManip * clipplane_y1 = new GsTL_SoClipPlaneManip;
//  clipplane_switch_->addChild( clipplane_y1 );
  GsTL_SoClipPlaneManip * clipplane_y2 = new GsTL_SoClipPlaneManip;
//  clipplane_switch_->addChild( clipplane_y2 );
  GsTL_SoClipPlaneManip * clipplane_z1 = new GsTL_SoClipPlaneManip;
  clipplane_switch_->addChild( clipplane_z1 );
  GsTL_SoClipPlaneManip * clipplane_z2 = new GsTL_SoClipPlaneManip;
//  clipplane_switch_->addChild( clipplane_z2 );

  // Setup the clipplanes last, as its size must be adjusted according
  // to the scenegraph bounding box.

  SbBox3f clip_box_x1( x_min, y_min, z_min,
                       x_min+cell_dims.x(), y_max, z_max );
  clipplane_x1->setValue(clip_box_x1, SbVec3f(1.0f, 0, 0), 0.5f );
  SbBox3f clip_box_x2( x_max, y_min, z_min,
                       x_max+cell_dims.x(), y_max, z_max );
  clipplane_x2->setValue(clip_box_x2, SbVec3f(-1.0f, 0, 0), 0.5f );
  SbBox3f clip_box_y1( x_min, y_min, z_min,
                       x_max, y_min+cell_dims.y(), z_max );
  clipplane_y1->setValue(clip_box_y1, SbVec3f(0, 1.0f, 0), 0.5f );
  SbBox3f clip_box_y2( x_min, y_max, z_min,
                       x_max, y_max+cell_dims.y(), z_max );
  clipplane_y2->setValue(clip_box_y2, SbVec3f(-1.0f, 0, 0), 0.5f );
  SbBox3f clip_box_z1( x_min, y_min, z_min,
                       x_max, y_max, z_min+cell_dims.z() );
  clipplane_z1->setValue(clip_box_z1, SbVec3f(0, 0, 1.0f), 0.5f );
  SbBox3f clip_box_z2( x_min, y_min, z_max,
                       x_max, y_max, z_max+cell_dims.z() );
  clipplane_z2->setValue(clip_box_z2, SbVec3f(0, 0, -1.0f), 0.5f );


//  clipplane_switch_->whichChild = SO_SWITCH_NONE;
  clipplane_switch_->whichChild = 0;
*/
}



void Oinv_cgrid::init( const Geostat_grid* grid ) {
  geostat_grid_ = grid;
  grid_ = dynamic_cast<const Cartesian_grid*>( grid );

  SbVec3s dim = SbVec3s( grid_->nx(), grid_->ny(), grid_->nz() );

  voxel_data_ = new uint8_t[ grid_->size() ];
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


Oinv_cgrid::~Oinv_cgrid() {
  delete [] voxel_data_;
}





void Oinv_cgrid::property_deleted( const std::string& prop_name ) {
  // If a property has been deleted, we have to:
  //   - remove its corresponding colormap entry if it exists, 
  //   - if the deleted property was being displayed, paint the object
  //     in the default color
  
  CmapMap::iterator found = cmap_map_.find( prop_name );
  if( found == cmap_map_.end() ) return;

  delete found->second.second;
  cmap_map_.erase( found );
  
  if( current_property_name_ == prop_name ) {
    // the deleted property was being displayed
    current_property_name_ = "";
    current_property_ = 0;
    cmap_ = 0;

    property_display_mode( Oinv::NOT_PAINTED );
  }
  
}





int Oinv_cgrid::add_slice( Oinv::Axis axis, int position, 
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


void Oinv_cgrid::remove_slice( int slice_id ) {
  
  if( slice_id < 0 || slice_id >= int( slices_.size() ) ) {
    appli_warning( "Slice id " << slice_id << " is out of bounds" );
    return;
  }
  
  slices_node_->removeChild( slice_id );

  std::list< Oinv_slice* >::iterator it = slices_.begin();
  std::advance( it, slice_id ); 
  delete *it;
  slices_.erase( it );
}



void Oinv_cgrid::display_mode( Display_mode mode ) {
  display_switch_->whichChild = int( mode );
}


void Oinv_cgrid::property_display_mode( Oinv::Property_display_mode mode ) {
  property_display_mode_ = mode;
  full_volume_->property_display_mode( mode );
  for( SliceList_iterator it = slices_.begin(); it != slices_.end(); ++it ) {
    (*it)->property_display_mode( mode );
  }
  painted_switch_->whichChild = int(mode);
}



Oinv_slice* Oinv_cgrid::slice( int slice_id ) {

  if( slice_id < 0 || slice_id >= int( slices_.size() ) ) {
    appli_warning( "Slice id " << slice_id << " is out of bounds" );
    return 0;
  }
  
  std::list< Oinv_slice* >::iterator it = slices_.begin();
  std::advance( it, slice_id ); 
  return *it;
}

 

void Oinv_cgrid::show_bounding_box( bool on ) {
  bbox_switch_->visible = on;
}


void Oinv_cgrid::hide_volume(bool on) {
  if( on )
    rendering_switch_->whichChild = SO_SWITCH_NONE ;
  else
    rendering_switch_->whichChild = 0;
}



void Oinv_cgrid::set_transparency( const QString& str ) {
  for( int i = 0; i < 256; i++ ) {
    volrend_colormap_->colorMap.set1Value( 4*i+3, 1.0f );
  }

  //if( !cmap_ || str.isEmpty() ) return;
  if( !cmap_ ) return;
  
  float min = cmap_->lower_bound();
  float max = cmap_->upper_bound();
  if( min == max ) return;

  QString cleaned_intervals;
  const float alpha = 0.01f;

  QStringList intervals_str = QStringList::split( ";", str);
  QStringList::Iterator it = intervals_str.begin(); 
  for ( ; it != intervals_str.end(); ++it ) {
    QStringList interval_str = QStringList::split( ",", *it );
    if( interval_str.size() != 2 ) continue; 

    cleaned_intervals += interval_str[0] + " , " + interval_str[1] + " ; ";
    float low = std::max( interval_str[0].stripWhiteSpace().toFloat(), min );
    float high= std::min( interval_str[1].stripWhiteSpace().toFloat(), max );
    
    int a= (low  - min) / ( max - min ) * 255;
    int b= (high - min) / ( max - min ) * 255;

    appli_assert( a >=0 && b < 256 );
    for( int i = a; i <= b; i++ ) {
      volrend_colormap_->colorMap.set1Value( 4*i+3, alpha );
    }
  }

  cleaned_intervals = cleaned_intervals.simplifyWhiteSpace();

  std::map< std::string, QString >::iterator found = 
    transparency_map_.find( current_property_name_ );
  if( found == transparency_map_.end() )
    transparency_map_.insert( 
      std::make_pair( current_property_name_, cleaned_intervals) 
     );
  else {
    found->second = cleaned_intervals;
  }
}


void Oinv_cgrid::set_transparency() {
  for( int i = 0; i < 256; i++ ) {
    volrend_colormap_->colorMap.set1Value( 4*i+3, 1.0f );
  }
  if( !cmap_ ) return;
  
  float min = cmap_->lower_bound();
  float max = cmap_->upper_bound();
  if( min == max ) return;

  const float alpha = 0.01f;

  std::map< std::string, QString >::iterator found = 
    transparency_map_.find( current_property_name_ );
  if( found == transparency_map_.end() ) return;
  if( found->second.isEmpty() ) return;

  QStringList intervals_str = QStringList::split( ";", found->second);
  QStringList::Iterator it = intervals_str.begin(); 
  for ( ; it != intervals_str.end(); ++it ) {
    QStringList interval_str = QStringList::split( ",", *it );
    if( interval_str.size() != 2 ) continue; 
    float low = std::max( interval_str[0].stripWhiteSpace().toFloat(), min );
    float high= std::min( interval_str[1].stripWhiteSpace().toFloat(), max );
    
    int a= (low  - min) / ( max - min ) * 255;
    int b= (high - min) / ( max - min ) * 255;

    appli_assert( a >=0 && b < 256 );
    for( int i = a; i <= b; i++ ) {
      volrend_colormap_->colorMap.set1Value( 4*i+3, alpha );
    }
  }

}



QString Oinv_cgrid::transparency_intervals( const std::string& property_name ) const {
  std::map< std::string, QString >::const_iterator found = 
    transparency_map_.find( property_name );

  if( found == transparency_map_.end() )
    return "";

  return found->second;
}


void Oinv_cgrid::refresh() {

  int low=1;
  int high=65535;
  int true_size;


  if( current_property_ && cmap_ ) {
    // recompute the voxel data
    if( !voxel_data_ ) 
      voxel_data_ = new uint8_t[grid_->size() ];

    float min = cmap_->lower_bound();
    float max = cmap_->upper_bound();
    
  
    for( int i=0; i < grid_->size()  ; i++ ) {
		if( current_property_->is_informed( i ) ) {
			// scale all the property values between 1 and 255. 0 is reserved
			// for no-data value
			// ensure that all property values are between min and max by hard thresholding
			float pval= std::max( std::min( current_property_->get_value(i), max ), min );
			uint8_t val = (pval - min) / (max-min)*254+1;        
			voxel_data_[i] = val; 
		}
		else {
			voxel_data_[i] = 0;
		}
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

  


//======================================================



std::string OinvBBox::tostring( const float array[3] ) const {
  std::ostringstream so; 
  so << "(" << array[0] << "," << array[1] << "," << array[2] << ")";
  return so.str();
}

OinvBBox::OinvBBox( float ox, float oy, float oz,
				   float xsize, float ysize, float zsize ) {

	root_ = new SoSeparator;
	root_->ref();

  float x_start= ox;
  float y_start= oy;
  float z_start= oz;
  float x_end = ox + xsize;
  float y_end = oy + ysize;
  float z_end = oz + zsize;

  const float vertices[8][3] = {
    { x_start, y_start, z_start},
    { x_start, y_end,   z_start },
    { x_end,   y_end,   z_start },
    { x_end,   y_start, z_start },
    { x_start, y_start, z_end},
    { x_start, y_end,   z_end },
    { x_end,   y_end,   z_end },
    { x_end,   y_start, z_end }
  };

  SoLineSet* bbox = new SoLineSet;
  SoCoordinate3* coords_node = new SoCoordinate3;

  coords_node->point.set1Value( 0, SbVec3f( vertices[0] ) );
  coords_node->point.set1Value( 1, SbVec3f( vertices[1] ) );
  coords_node->point.set1Value( 2, SbVec3f( vertices[2] ) );
  coords_node->point.set1Value( 3, SbVec3f( vertices[3] ) );
  coords_node->point.set1Value( 4, SbVec3f( vertices[0] ) );

  coords_node->point.set1Value( 5, SbVec3f( vertices[4] ) );
  coords_node->point.set1Value( 6, SbVec3f( vertices[5] ) );
  coords_node->point.set1Value( 7, SbVec3f( vertices[6] ) );
  coords_node->point.set1Value( 8, SbVec3f( vertices[7] ) );
  coords_node->point.set1Value( 9, SbVec3f( vertices[4] ) );

  coords_node->point.set1Value( 10, SbVec3f( vertices[0] ) );
  coords_node->point.set1Value( 11, SbVec3f( vertices[4] ) );

  coords_node->point.set1Value( 12, SbVec3f( vertices[1] ) );
  coords_node->point.set1Value( 13, SbVec3f( vertices[5] ) );

  coords_node->point.set1Value( 14, SbVec3f( vertices[2] ) );
  coords_node->point.set1Value( 15, SbVec3f( vertices[6] ) );

  coords_node->point.set1Value( 16, SbVec3f( vertices[3] ) );
  coords_node->point.set1Value( 17, SbVec3f( vertices[7] ) );


  root_->addChild( coords_node );
 
  bbox->numVertices.set1Value( 0, 5 );
  bbox->numVertices.set1Value( 1, 5 );
  bbox->numVertices.set1Value( 2, 2 );
  bbox->numVertices.set1Value( 3, 2 );
  bbox->numVertices.set1Value( 4, 2 );
  bbox->numVertices.set1Value( 5, 2 );
  root_->addChild( bbox );


  /** Commented out for now. Need to scale the text to be proportional 
   *  to the grid size
   */
/*
  SoSeparator* text_root = new SoSeparator;
  
  for( int i=0 ; i < 8 ; i++ ) {
    SoSeparator* temp = new SoSeparator;
    SoTranslation* transl = new SoTranslation;
	  transl->translation.setValue( vertices[i] );
    SoText2* text = new SoText2;
    text->string = tostring( vertices[i] ).c_str();
    temp->addChild( transl );
    temp->addChild( text) ;
	  text_root->addChild( temp );
  }

  root_->addChild( text_root );
  */
}




//======================================================

	
OinvVolumeSkin::OinvVolumeSkin( unsigned int nx, unsigned int ny, unsigned int nz ) {
  skin_root_ = new SoSeparator;

  SoOrthoSlice* x_plane_start = new SoOrthoSlice;
  x_plane_start->axis = SoOrthoSlice::X;
  x_plane_start->sliceNumber = 0;
  x_plane_start->alphaUse = SoOrthoSlice::ALPHA_OPAQUE;

  SoOrthoSlice* x_plane_end = new SoOrthoSlice;
  x_plane_end->axis = SoOrthoSlice::X;
  x_plane_end->sliceNumber = nx-1;
  x_plane_end->alphaUse = SoOrthoSlice::ALPHA_OPAQUE;


  SoOrthoSlice* y_plane_start = new SoOrthoSlice;
  y_plane_start->axis = SoOrthoSlice::Y;
  y_plane_start->sliceNumber = 0;
  y_plane_start->alphaUse = SoOrthoSlice::ALPHA_OPAQUE;

  SoOrthoSlice* y_plane_end = new SoOrthoSlice;
  y_plane_end->axis = SoOrthoSlice::Y;
  y_plane_end->sliceNumber = ny;
  y_plane_end->alphaUse = SoOrthoSlice::ALPHA_OPAQUE;


  SoOrthoSlice* z_plane_start = new SoOrthoSlice;
  z_plane_start->axis = SoOrthoSlice::Z;
  z_plane_start->sliceNumber = 0;
  z_plane_start->alphaUse = SoOrthoSlice::ALPHA_OPAQUE;

  SoOrthoSlice* z_plane_end = new SoOrthoSlice;
  z_plane_end->axis = SoOrthoSlice::Z;
  z_plane_end->sliceNumber = 0;
  z_plane_end->alphaUse = SoOrthoSlice::ALPHA_OPAQUE;

}


SoSeparator* OinvVolumeSkin::oinv_node() {
  return skin_root_;
}



//======================================================


Cgrid_probe::Cgrid_probe( SbBox3f box ) {
  
  
  root_ = new SoGroup;
  root_->ref();

  const float pi = GsTL::PI;

  SoGroup* sep_x1 = new SoGroup;
  SoTransform* transf_x1 = new SoTransform;
  transf_x1->translation.setValue( 1.0f, 0, 0 );
  SoClipPlane* clipplane_x1 = new SoClipPlane;
  sep_x1->addChild( transf_x1 );
  sep_x1->addChild( clipplane_x1 );

  SoGroup* sep_x2 = new SoGroup;
  SoTransform* transf_x2 = new SoTransform;
  transf_x2->translation.setValue( -1.0f, 0, 0 );
  transf_x2->rotation.setValue(SbVec3f(0, 0, 1), pi);
  SoClipPlane* clipplane_x2 = new SoClipPlane;
  sep_x2->addChild( transf_x2 );
  sep_x2->addChild( clipplane_x2 );


  SoGroup* sep_y1 = new SoGroup;
  SoTransform* transf_y1 = new SoTransform;
  transf_y1->translation.setValue( 1.0f, 0, 0 );
  transf_y1->rotation.setValue(SbVec3f(0, 0, 1), pi/2);
  SoClipPlane* clipplane_y1 = new SoClipPlane;
  sep_y1->addChild( transf_y1 );
  sep_y1->addChild( clipplane_y1 );

  SoGroup* sep_y2 = new SoGroup;
  SoTransform* transf_y2 = new SoTransform;
  transf_y2->translation.setValue( -1.0f, 0, 0 );
  transf_y2->rotation.setValue(SbVec3f(0, 0, 1), -pi/2);
  SoClipPlane* clipplane_y2 = new SoClipPlane;
  sep_y2->addChild( transf_y2 );
  sep_y2->addChild( clipplane_y2 );


  SoGroup* sep_z1 = new SoGroup;
  SoTransform* transf_z1 = new SoTransform;
  transf_z1->translation.setValue( 1.0f, 0, 0 );
  transf_z1->rotation.setValue(SbVec3f(0, 1, 0), pi/2);
  SoClipPlane* clipplane_z1 = new SoClipPlane;
  sep_z1->addChild( transf_z1 );
  sep_z1->addChild( clipplane_z1 );

  SoGroup* sep_z2 = new SoGroup;
  SoTransform* transf_z2 = new SoTransform;
  transf_z2->translation.setValue( -1.0f, 0, 0 );
  transf_z2->rotation.setValue(SbVec3f(0, 1, 0), -pi/2);
  SoClipPlane* clipplane_z2 = new SoClipPlane;
  sep_z2->addChild( transf_z2 );
  sep_z2->addChild( clipplane_z2 );


  SoHandleBoxManip* main_transform = new SoHandleBoxManip;
  main_transform->scaleFactor.setValue( 20,20,5 );

  root_->addChild( main_transform );
  root_->addChild( sep_x1 );
  root_->addChild( sep_x2 );
  root_->addChild( sep_y1 );
  root_->addChild( sep_y2 );
  root_->addChild( sep_z1 );
  root_->addChild( sep_z2 );

}



Cgrid_probe::~Cgrid_probe() { 
  root_->unref(); 
}






//======================================================

Full_volume::Full_volume( int nx, int ny, int nz,
			  const uint8_t* std_values, const bool* initialized, 
        Colormap*& cmap,
			  float xsize, float ysize, float zsize,
			  float xmn, float ymn, float zmn ) {
  
  cursor_ = new SGrid_cursor( nx, ny, nz );
  
  //compute the textures:
  textures_[0] = new Texture_node( std::make_pair( GsTLVector<int>(0,0,0), 
						   GsTLVector<int>(1,ny,nz) ),
				   std_values, initialized, cmap, *cursor_ );
  textures_[1] = new Texture_node( std::make_pair( GsTLVector<int>(nx-1, 0, 0), 
						   GsTLVector<int>(nx,ny,nz) ),
				   std_values, initialized, cmap, *cursor_  );

  textures_[2] = new Texture_node( std::make_pair( GsTLVector<int>(0,0,0), 
						   GsTLVector<int>(nx,1,nz) ),
				   std_values, initialized, cmap, *cursor_  );
  textures_[3] = new Texture_node( std::make_pair( GsTLVector<int>(0, ny-1, 0), 
						   GsTLVector<int>(nx,ny,nz) ),
				   std_values, initialized, cmap, *cursor_  );

  textures_[4] = new Texture_node( std::make_pair( GsTLVector<int>(0,0,0), 
						   GsTLVector<int>(nx,ny,1) ),
				   std_values, initialized, cmap, *cursor_  );
  textures_[5] = new Texture_node( std::make_pair( GsTLVector<int>(0, 0, nz-1), 
						   GsTLVector<int>(nx,ny,nz) ),
				   std_values, initialized, cmap, *cursor_  );

  // Compute the planes:
  planes_[0].first = new X_plane( ny, nz, ysize, zsize, 
				  xmn - 0.5*xsize,
				  ymn - 0.5*ysize,
				  zmn - 0.5*zsize );
  planes_[0].second = GsTLCoordVector( nx*xsize, 0, 0 );

  planes_[1].first = new Y_plane( nx, nz, xsize, zsize, 
				  xmn - 0.5*xsize,
				  ymn - 0.5*ysize,
				  zmn - 0.5*zsize );
  planes_[1].second = GsTLCoordVector( 0, ny*ysize, 0 );

  planes_[2].first = new Z_plane( nx, ny, xsize, ysize,
				  xmn - 0.5*xsize,
				  ymn - 0.5*ysize,
				  zmn - 0.5*zsize );
  planes_[2].second = GsTLCoordVector( 0, 0, nz*zsize );
   

  root_ = new SoSeparator;
  root_->ref();


  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::CLOCKWISE;
  //hints->faceType = SoShapeHints::CONVEX;
  //hints->creaseAngle = 0;
  root_->addChild( hints );

  for( int i=0; i<3; i++ ) {
    SoSeparator* plane_1 = new SoSeparator;
    
    plane_1->addChild( textures_[2*i]->oinv_node() );
    plane_1->addChild( planes_[i].first->oinv_node() );
    
    SoSeparator* plane_2 = new SoSeparator;
    SoTransform* transform = new SoTransform;
    GsTLCoordVector vec = planes_[i].second;
    transform->translation.setValue( vec[0], vec[1], vec[2] );
    plane_2->addChild( transform );

    plane_2->addChild( textures_[2*i+1]->oinv_node() );
    plane_2->addChild( planes_[i].first->oinv_node() );
     

    root_->addChild( plane_1 );
    root_->addChild( plane_2 );
    
 }
}


Full_volume::~Full_volume() {

  delete cursor_;
  
  root_->unref();
  delete planes_[0].first;
  delete planes_[1].first;
  delete planes_[2].first;

  for( int i=0; i<6 ; i++ )
    delete textures_[i];
}


void Full_volume::update() {
  for( int i=0; i<6; i++ )
    textures_[i]->update_texture();
}



void Full_volume::property_display_mode( Oinv::Property_display_mode mode ) {
  for( int i=0; i<6 ; i++ ) {
    textures_[i]->property_display_mode( mode );
  }
}
