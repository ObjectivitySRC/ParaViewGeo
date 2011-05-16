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

#include <GsTLAppli/gui/oinv_description/colormap_node.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoTransform.h>

#include <qstring.h>
  

Oinv_colormap_node::Oinv_colormap_node()
  : cmap_( 0 ),
    colors_node_( 0 ),
    text_sep_( 0 ),
    tickmarks_color_( 0 ), 
    text_color_(1,1,1) {


  nb_colors_ = 0;

  root_ = new SoSeparator;
  root_->ref();

  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root_->addChild( hints );

  SoMaterialBinding* bind = new SoMaterialBinding;
  bind->value = SoMaterialBinding::PER_FACE;
  root_->addChild( bind );
  
  colors_node_ = new SoMaterial;
  root_->addChild( colors_node_ );

  coords_node_ = new SoCoordinate3;
  root_->addChild( coords_node_ );

  mesh_ = new SoQuadMesh;
  root_->addChild( mesh_ );
}



Oinv_colormap_node::Oinv_colormap_node( const Colormap& cmap, int num, bool hori)
  : cmap_( 0 ),
    colors_node_( 0 ),
    text_sep_( 0 ),
    tickmarks_color_( 0 ), 
    text_color_(1,1,1) {


  //TL modified
  _numticks = num;
  _horizontal = hori;

  nb_colors_ = 0;

  root_ = new SoSeparator;
  root_->ref();

  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root_->addChild( hints );

  SoMaterialBinding* bind = new SoMaterialBinding;
  bind->value = SoMaterialBinding::PER_FACE;
  root_->addChild( bind );
  
  colors_node_ = new SoMaterial;
  root_->addChild( colors_node_ );

  coords_node_ = new SoCoordinate3;
  root_->addChild( coords_node_ );

  mesh_ = new SoQuadMesh;
  root_->addChild( mesh_ );


  Oinv_colormap_node::colormap( cmap );
}


Oinv_colormap_node::~Oinv_colormap_node() {
  root_->unref();
}


void Oinv_colormap_node::colormap( const Colormap& cmap) {
  
  if( cmap_ == cmap ) return;

  if( cmap_.upper_bound() != cmap.upper_bound() || 
      cmap_.lower_bound() != cmap.lower_bound() ) {
    draw_tickmarks( cmap, _numticks );
  }
    

  cmap_ = cmap;
  int nb_colors = cmap.color_scale()->colors_count();
  set_mesh_coordinates( nb_colors ); 
  
  float (*colors)[3] = new float[nb_colors][3];

  for( int i=0; i < nb_colors ; i++ ) {
    float r,g,b;
    cmap.color_scale()->color(i, r,g,b);
    colors[i][0] = r;
    colors[i][1] = g;
    colors[i][2] = b;
  }

  colors_node_->diffuseColor.setValues( 0, nb_colors, colors );
  
  delete [] colors;
}


void Oinv_colormap_node::set_mesh_coordinates( int nb_colors ) {
  // We make a colormap of height 1 and width 0.2, starting at -0.5
  // The height is divided into nb_colors rectangles

  if( nb_colors == nb_colors_ ) return;

  const float height = 1;
  const float width = 0.2;
  nb_colors_ = nb_colors;
  float (*coords)[3] = new float[2*(nb_colors+1)][3];

  float step = height / float(nb_colors);
  int count = 0;
  for( int j= 0 ; j <= nb_colors ; j++ ) 
    for( int i=0; i< 2; i++ ) {
      coords[count][0] = i*width;
      coords[count][1] = -0.5+float(j)*step;
      coords[count][2]= 0.0;
      count ++;
    }

  coords_node_->point.setValues(0, 2*(nb_colors+1), coords) ;
  delete [] coords;

  mesh_->verticesPerRow = 2;
  mesh_->verticesPerColumn = nb_colors+1;
  
}


//TL modified
void Oinv_colormap_node::update_ticks(int num)
{
	if (num == _numticks)
		return;
	draw_tickmarks(cmap_, num);
}

void Oinv_colormap_node::draw_tickmarks( const Colormap& cmap, int num ) {

  //TL modified
  _numticks = num;

  if( text_sep_ )
    root_->removeChild( text_sep_ );


  text_sep_ = new SoSeparator;

  // Choose a font
  SoFont *myFont = new SoFont;
  myFont->size.setValue(10.0);
  text_sep_->addChild(myFont);

  tickmarks_color_ = new SoMaterial;
  float r,g,b;
  text_color_.getValue(r,g,b);
  tickmarks_color_->diffuseColor.setValue( r,g,b );
  text_sep_->addChild( tickmarks_color_ );

  // Draw a frame around the colorbar
  SoSeparator* frame_sep = new SoSeparator;
  SoCoordinate3* frame_coords = new SoCoordinate3;
  frame_coords->point.set1Value(0,SbVec3f(-0.005,-0.505,0));
  frame_coords->point.set1Value(1,SbVec3f(-0.005,0.505,0));
  frame_coords->point.set1Value(2,SbVec3f(0.205,0.505,0));
  frame_coords->point.set1Value(3,SbVec3f(0.205,-0.505,0));
  frame_coords->point.set1Value(4,SbVec3f(-0.005,-0.505,0));
  frame_sep->addChild( frame_coords );
  
  SoDrawStyle* frame_style = new SoDrawStyle;
  frame_style->lineWidth = 2;

  SoLineSet* rectangle = new SoLineSet;
  rectangle->numVertices = 5;
  rectangle->startIndex = 0;
  frame_sep->addChild( rectangle);

  text_sep_->addChild( frame_sep );


  // Draw the values corresponding to the colors
  int subdivisions = _numticks-1;
  float step = 1.0 / (subdivisions+1);
  float val_step = 
    ( cmap.upper_bound() - cmap.lower_bound() ) / float( subdivisions+1 );

  SoTransform* text_transl = new SoTransform;
  text_transl->translation.setValue( .34, -0.5, .0 );
  text_sep_->addChild( text_transl );
  
  //TL modified
  SoSeparator * tick_sep = new SoSeparator;
  SoCoordinate3 * tcoord = new SoCoordinate3;
  SoLineSet * tickbar = new SoLineSet;
  tcoord->point.set1Value(0,SbVec3f(-0.135,0.0,0));
  tcoord->point.set1Value(1,SbVec3f(-0.096,0.0,0));
  tcoord->point.set1Value(2,SbVec3f(-0.135,0.0,0));
  rectangle->numVertices = 3;
  rectangle->startIndex = 0; 

  tick_sep->addChild(tcoord);
  tick_sep->addChild(tickbar);


  for( int i=0; i<= subdivisions+1; i++ ) {
    SoTransform* transl = new SoTransform;
    transl->translation.setValue( 0.0, step, .0 );

    SoText2* text_node = new SoText2;
    QString val_str;
    float val = cmap.lower_bound() + float(i)*val_step;  
    val_str.setNum( val, 'g', 4 );
    text_node->string = val_str.latin1();
    text_sep_->addChild(tick_sep);
    text_sep_->addChild( text_node );
    text_sep_->addChild( transl );
  }

  
  root_->addChild( text_sep_ );

}



void Oinv_colormap_node::background_color( const SbColor& color ) {
  if( !tickmarks_color_ ) return;
  
  float r,g,b;
  color.getValue( r,g,b );
  tickmarks_color_->diffuseColor.setValue( 1.0-r, 1.0-g, 1.0-b );

  SbColor contrast_color( 1.0-r, 1.0-g, 1.0-b );
  text_color_ = contrast_color;
}
