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

#include <GsTLAppli/gui/appli/general_display_pref_panel.h>
#include <GsTLAppli/gui/appli/oinv_project_view.h>
#include <GsTLAppli/gui/oinv_description/colormap_node.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/appli/project.h>

#include <GsTLAppli/gui/appli/SoQtGsTLViewer.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

//TL modified
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoScale2Dragger.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/fields/SoSFRotation.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <qfiledialog.h>
#include <qmessagebox.h>
using namespace std;

#include <qwidget.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qspinbox.h>

#include <string>

#define MAGICNUMBER 0x1FF8D74F

void myKeyCB(void * data, SoEventCallback * cb)
{
	appli_message("In");
	/*
	SoTransform* transf = (SoTransform *) data;
    transf->translation.setValue( -0.02, 0 ,0 );
	*/
    cb->setHandled();
}


General_display_pref_panel::
General_display_pref_panel( SoQtGsTLViewer* viewer,
			    SoSeparator* colormap_root_node,
			    Oinv_description_map* descriptions,
			    QWidget* parent, const char* name )
  : General_display_pref_panel_base( parent, name ),
    viewer_( viewer ),
    descriptions_( descriptions ),
    Z_scaling_( 0 ),
    scene_root_( 0 ),
    colormap_root_node_( colormap_root_node ),
    colorbar_root_( 0 ),
    colorbar_( 0 ) {

  //TL modified
  _transf = NULL;

  // set-up the property selector for the colorbar display
  GroupBox3->setOrientation( Qt::Vertical );
  GroupBox3->setColumns( 2 );
  object_selector_ = new PropertySelector( GroupBox3, "object_selector_" );

  if( viewer_ ) {
//    scene_root_ = dynamic_cast<SoGroup*>( viewer_->getSceneGraph() );
	  scene_root_ = (SoGroup*)( viewer_->getSceneGraph() );
    appli_assert( scene_root_ );
  }

  QObject::connect( z_scaling_box_, SIGNAL( valueChanged( int ) ),
		    this, SLOT( z_scaling( int ) ) );
  QObject::connect( perspective_button_, SIGNAL( clicked() ),
		    this, SLOT( use_perspective_camera() ) );
  QObject::connect( orthotropic_button_, SIGNAL( clicked() ),
		    this, SLOT( use_orthotropic_camera() ) );
  QObject::connect( white_background_checkbox_, SIGNAL( toggled( bool ) ),
  		    this, SLOT( white_background( bool ) ) );
  QObject::connect( show_colobar_checkbox_, SIGNAL( toggled( bool ) ),
		    this, SLOT( display_colorbar( bool ) ) );

  QObject::connect( object_selector_, 
		    SIGNAL( object_changed( const QString& ) ),
		    this,
		    SLOT( update_colorbar() ) );
  QObject::connect( object_selector_, 
		    SIGNAL( property_changed( const QString& ) ),
		    this,
		    SLOT( update_colorbar() ) );
  QObject::connect( object_selector_, 
		    SIGNAL( new_object_available( const QString& ) ),
		    this,
		    SLOT( enable_colorbar_display() ) );

  //TL modified
  QObject::connect( _numticks, SIGNAL(valueChanged(int)),
			this,
			SLOT(redrawTicks(int)));

  QObject::connect( _horizontal, SIGNAL(toggled(bool)),
			this,
			SLOT(rotate(bool)));

  QObject::connect( _save, SIGNAL(clicked()),
			this,
			SLOT(save_colorbar()));
  QObject::connect( _load, SIGNAL(clicked()),
			this,
			SLOT(load_colorbar()));

}


//TL modified
void General_display_pref_panel::save_colorbar()
{
  SbVec3f vec;

  std::string grid_name = object_selector_->selectedGrid().latin1();
  std::string property_name = object_selector_->selectedProperty().latin1();
  if( property_name.empty() || grid_name.empty() ) return;

  if( !colorbar_root_ ) 
    init_colormap_node();

  QString s = QFileDialog::getSaveFileName(
	  QString::null,
	  QString::null,
	  this,
	  "save file dialog");

  ofstream of(s);
  of << MAGICNUMBER << std::endl;
  vec = _transf->translation.getValue();
  of << vec[0] << " " << vec[1] << " " << vec[2] << std::endl;
  vec = _transf->scaleFactor.getValue();
  of << vec[0] << " " << vec[1] << " " << vec[2] << std::endl;
  of << _numticks->value() << std::endl;
  if (_horizontal->isChecked())
	  of << 1 << std::endl;
  else
	  of << 0 << std::endl;
  of.close();
}

void General_display_pref_panel::load_colorbar_fromfile(QString & s)
{
	int magic = -1, num;
	float input[3];
	float angle;

	std::string grid_name = object_selector_->selectedGrid().latin1();
	std::string property_name = object_selector_->selectedProperty().latin1();
	if( property_name.empty() || grid_name.empty() ) return;

	if( !colorbar_root_ ) 
		init_colormap_node();

	ifstream in(s);
	if (!in.is_open()) {
		QMessageBox::warning(
			this,
			"Failed to open file",
			"Unable to open this file",
			QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}

	in >> magic;
	if (magic != MAGICNUMBER) {
		in.close();
		QMessageBox::warning(
			this,
			"Wrong file format",
			"The file doesn't appear to be a colorbar file",
			QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	for (int i = 0; i < 3; ++i)
		in >> input[i];
	_transf->translation.setValue(input);
	for (int i = 0; i < 3; ++i)
		in >> input[i];
	_transf->scaleFactor.setValue(input);
	in >> num;
	_numticks->setValue(num);
	in >> num;
	_horizontal->setChecked(num);
	in.close();

}

void General_display_pref_panel::load_colorbar()
{
  int magic = -1, num;
  float input[3];
  float angle;

  std::string grid_name = object_selector_->selectedGrid().latin1();
  std::string property_name = object_selector_->selectedProperty().latin1();
  if( property_name.empty() || grid_name.empty() ) return;

  if( !colorbar_root_ ) 
    init_colormap_node();

  QString s = QFileDialog::getOpenFileName(
	  QString::null,
	  QString::null,
	  this,
	  "open file dialog");

  ifstream in(s);
  if (!in.is_open()) {
	  QMessageBox::warning(
		  this,
		  "Failed to open file",
		  "Unable to open this file",
		  QMessageBox::Ok, QMessageBox::NoButton);
	  return;
  }

  in >> magic;
  if (magic != MAGICNUMBER) {
	  in.close();
	  QMessageBox::warning(
		  this,
		  "Wrong file format",
		  "The file doesn't appear to be a colorbar file",
		  QMessageBox::Ok, QMessageBox::NoButton);
	  return;
  }
  for (int i = 0; i < 3; ++i)
	  in >> input[i];
  _transf->translation.setValue(input);
  for (int i = 0; i < 3; ++i)
	  in >> input[i];
  _transf->scaleFactor.setValue(input);
  in >> num;
  _numticks->setValue(num);
  in >> num;
  _horizontal->setChecked(num);
  in.close();
}

void General_display_pref_panel::rotate(bool hori)
{
	if (!_transf) return;

	if (hori)
		_transf->rotation.setValue(SbVec3f(0, 0, 1),-M_PI/2.0);
	else
		_transf->rotation.setValue(SbVec3f(0, 0, 1),0.0);

}

void General_display_pref_panel::redrawTicks(int num)
{
	if (!colorbar_)
		return;
	colorbar_->update_ticks(num);
}

General_display_pref_panel::~General_display_pref_panel() {
  //  When scene_root_ is deleted, it should remove all its children.
  // If that's indeed the case, the following lines should be unecessary.
  /* if( Z_scaling_ )
      scene_root_->removeChild( Z_scaling_ ); 

  if( colorbar_root_ ) {
    colormap_root_node_->removeChild( (SoNode*) colorbar_root_ );
    delete colorbar_;
  }
  */
  if( colorbar_root_ )
    delete colorbar_;
}



void General_display_pref_panel::init( GsTL_project* project ) {
  object_selector_->init( project ) ;
}



void General_display_pref_panel::use_perspective_camera() {
  viewer_->toggleCameraType();
}
void General_display_pref_panel::use_orthotropic_camera() {
  viewer_->toggleCameraType();
}


void General_display_pref_panel::white_background( bool on ) {
  SbColor background_color;
  if( on )
    background_color.setValue( 1.0, 1.0, 1.0 );
  else
    background_color.setValue( .0, .0, .0 );

  viewer_->setBackgroundColor( background_color );
  
  if( colorbar_ )
    colorbar_->background_color( background_color );
  else {
    SoGLRenderAction render( viewer_->getViewportRegion() );
    render.apply( viewer_->getSceneGraph() );
  }
}


void General_display_pref_panel::z_scaling( int factor ) {
  if( !viewer_ )
    return;

  if( !Z_scaling_ ) {
    Z_scaling_ = new SoTransform;
    scene_root_->insertChild( Z_scaling_, 0 );
  }

  Z_scaling_->scaleFactor.setValue( 1.0, 1.0, float(factor) );
}



void General_display_pref_panel::enable_colorbar_display() {
  if( !show_colobar_checkbox_->isEnabled() )
    show_colobar_checkbox_->setEnabled( true );
}



void General_display_pref_panel::display_colorbar( bool on ) {

  if( !viewer_ )
    return;
  
  // If the user did not select an object or a property, return.
  std::string grid_name = object_selector_->selectedGrid().latin1();
  std::string property_name = object_selector_->selectedProperty().latin1();
  if( property_name.empty() || grid_name.empty() ) return;


  if( !colorbar_root_ ) {
    init_colormap_node();
  }

  
  // Before showing the colorbar, we have to update it. If we want to
  // hide it, no update is needed.
  if( on == false ) {
    colorbar_root_->visible = on;
	viewer_->setColorbarStatus(false);
    return ;
  }

  update_colorbar();
  colorbar_root_->visible = on;
}

void General_display_pref_panel::update_colorbar() {
  // Get the description of the object specified by the user
  // ask it for the colormap used for the property input by the
  // user and change the Oinv_colormap_node correspondingly

  std::string grid_name = object_selector_->selectedGrid().latin1();
  std::string property_name = object_selector_->selectedProperty().latin1();
  if( property_name.empty() || grid_name.empty() ) {
	if( colorbar_root_ ){
	  viewer_->setColorbarStatus(false);
      colorbar_root_->visible = false;
	}
    return;
  }

  if( !colorbar_root_ ) 
    init_colormap_node();
    

  Oinv_description_map::pair desc_pair = descriptions_->description( grid_name );
  const Colormap* cmap = desc_pair.second->colormap( property_name );
  if( !cmap ) {
    show_colobar_checkbox_->setChecked(false);
    colorbar_root_->visible = false;
	viewer_->setColorbarStatus(false);
    return;
  }

  viewer_->setColorbarStatus(true);
  colorbar_root_->visible = show_colobar_checkbox_->isChecked();
  colorbar_->colormap( *cmap );

}



void General_display_pref_panel::init_colormap_node() {
  
  // If the user did not select an object or a property, return.
  std::string grid_name = object_selector_->selectedGrid().latin1();
  std::string property_name = object_selector_->selectedProperty().latin1();
  if( property_name.empty() || grid_name.empty() ) return;

  colorbar_root_ = new GsTL_SoNode;
  colormap_root_node_->addChild( colorbar_root_ );

  SoSeparator* colorbar_scene = new SoSeparator;
  colorbar_root_->addChild( colorbar_scene );
  
  // Get a colormap
  Oinv_description_map::pair desc_pair = descriptions_->description( grid_name );
  const Colormap* cmap = desc_pair.second->colormap( property_name );
  colorbar_ = new Oinv_colormap_node( *cmap, _numticks->value(), _horizontal->isChecked());
    
  // Add a new camera and set it up
  SoOrthographicCamera * second_camera = new SoOrthographicCamera;
  second_camera->position = SbVec3f(0, 0, 10);
  second_camera->nearDistance = 0.1;
  second_camera->farDistance = 1000;
  second_camera->viewportMapping = SoCamera::LEAVE_ALONE;
  colorbar_scene->addChild( second_camera );
 
 
  // Compute the bounding box of the colorbar
  SoGetBoundingBoxAction bbox_action( viewer_->getViewportRegion() );
  bbox_action.apply( colorbar_->oinv_node() );
  SbBox3f bbox = bbox_action.getBoundingBox();
  float dx, dy, dz;
  bbox.getSize( dx, dy, dz );


  const float new_dx = 0.3;

  // Translate the colorbar so that it is close to the border of the window
  _transf = new SoTransform;
  _transf->translation.setValue( 1-(new_dx+0.05), 0.0 ,0 );
  _transf->scaleFactor.setValue( new_dx/dx, 0.7/dy, 1 );  
  colorbar_scene->addChild( _transf );

 
  viewer_->setColorbarTransform(_transf);

  colorbar_scene->addChild( colorbar_->oinv_node() );
        
  colorbar_->background_color( viewer_->getBackgroundColor() );
}
