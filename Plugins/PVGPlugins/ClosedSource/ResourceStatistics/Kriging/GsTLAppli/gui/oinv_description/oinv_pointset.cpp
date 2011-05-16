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

#include <GsTLAppli/gui/oinv_description/oinv_pointset.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/gui/utils/colormap.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoFaceSet.h>

Named_interface* Oinv_pointset::create_new_interface( std::string& ) {
  return new Oinv_pointset;
}

Oinv_pointset::Oinv_pointset() 
  : Oinv_description(),
    grid_( 0 ), redgrid_(NULL) {
}


Oinv_pointset::~Oinv_pointset() {
}



void Oinv_pointset::init( const Geostat_grid* grid ) {
  geostat_grid_ = grid;
  grid_ = dynamic_cast<const Point_set*>( grid );
  
  //TL modified
  if (!grid_)
	  redgrid_ = dynamic_cast<const Reduced_grid*>( grid );

  SoSeparator* root = new SoSeparator;
  oinv_node_->addChild( root );
  

  // The nodes for the color of the points
  // We create to groups: one to use when no property should be painted
  // and another one to use when the property is to be painted.
  // These 2 groups are inserted into an SoSwitch

  material_switch_ = new SoSwitch;
  
  root->addChild( material_switch_ );

  // group 1
  SoGroup* not_painted_group = new SoGroup;

  SoMaterialBinding* not_painted_bind_node = new SoMaterialBinding;
  not_painted_bind_node->value = SoMaterialBinding::OVERALL;
  not_painted_group->addChild( not_painted_bind_node );
  SoMaterial* no_color_node = new SoMaterial;
  no_color_node->diffuseColor.setValue( Oinv::nodata_color.red(),
					Oinv::nodata_color.green(),
					Oinv::nodata_color.blue() );
  not_painted_group->addChild( no_color_node );
  material_switch_->addChild( not_painted_group );


  // group 2
  SoGroup* painted_group = new SoGroup;

  SoMaterialBinding* painted_bind_node = new SoMaterialBinding;
  painted_bind_node->value = SoMaterialBinding::PER_PART;
  painted_group->addChild( painted_bind_node );
  colors_node_ = new SoMaterial;
  painted_group->addChild( colors_node_ );
  material_switch_->addChild( painted_group );
  

  // Insert the node containing the coordinates of the points 
  SoCoordinate3* coords_node = new SoCoordinate3;
  if (grid_) {
    const std::vector<Point_set::location_type> & locations = grid_->point_locations();  
    float (*coords)[3] = new float[ locations.size() ][3];
	for( int i=0; i< int( locations.size() ); i++ ) {
		coords[i][0] = locations[i].x();
		coords[i][1] = locations[i].y();
		coords[i][2] = locations[i].z();
	}
    coords_node->point.setValues(0, locations.size(), coords); 
    delete [] coords;
  }
  else {
	  //const std::vector<Point_set::location_type> & locations = redgrid_->psIJK();  

	  float (*coords)[3] = new float[ redgrid_->size() ][3];
	  for( int i=0; i< int( redgrid_->size() ); i++ ) {
		  Point_set::location_type node = redgrid_->location(i);
		  coords[i][0] = node[0];
		  coords[i][1] = node[1];
		  coords[i][2] = node[2];
	  }
      coords_node->point.setValues(0, redgrid_->size(), coords); 
      delete [] coords;
  }
  root->addChild( coords_node );


  draw_style_node_ = new SoDrawStyle;
  //TL modified
  if (!grid_)
	  draw_style_node_->pointSize = 10;
  else
  	  draw_style_node_->pointSize = 4;

  root->addChild( draw_style_node_ );
  root->addChild( new SoPointSet );

  property_display_mode( Oinv::NOT_PAINTED );

}


void Oinv_pointset::property_deleted( const std::string& prop_name ) {
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


void Oinv_pointset::property_display_mode( Oinv::Property_display_mode mode ) {
  property_display_mode_ = mode;
  material_switch_->whichChild = int( mode );

}


void Oinv_pointset::point_size( int size ) {
draw_style_node_->pointSize = size;
}


void Oinv_pointset::refresh() {
  if( cmap_ && current_property_ ) {
    int S;
	if (grid_)
		S = grid_->size();
	else
		S = redgrid_->size();

    float (*colors)[3] = new float[ S][3];

    for( int i=0; i< int( S ); i++ ) {
      if( current_property_->is_informed( i ) ) {
        float r,g,b;
        cmap_->color( current_property_->get_value( i ), r,g,b );
        colors[i][0] = r;
        colors[i][1] = g;
        colors[i][2] = b;
      }
      else {
		  appli_message("uninformed at " << i);
        colors[i][0] = Oinv::nodata_color.red();
        colors[i][1] = Oinv::nodata_color.green();
        colors[i][2] = Oinv::nodata_color.blue();
      }
    }
    colors_node_->diffuseColor.setValues(0, S, colors); 
    delete [] colors;
  }
}
