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

#include <GsTLAppli/gui/appli/scatterplot_control_panel.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>

#include <qgroupbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcheckbox.h>

Scatterplot_control_panel::
Scatterplot_control_panel( GsTL_project* proj,
                           QWidget* parent, const char* name ) 
  : Scatterplot_control_panel_base( parent, name ) {
  
  grid_selector_box_->setOrientation( Qt::Vertical );
  grid_selector_box_->setColumns( 1 );
  grid_selector_box_->setInsideMargin( 0 );
  grid_selector_box_->setInsideSpacing( 0 );
  object_selector_ = new GridSelector( grid_selector_box_, "object_selector_" );

  // set-up the 2 property selectors
  property_selector_box_1_->setOrientation( Qt::Vertical );
  property_selector_box_1_->setColumns( 1 );
  property_selector_box_1_->setInsideMargin( 0 );
  property_selector_box_1_->setInsideSpacing( 0 );
  prop_selector_var1_ = 
    new SinglePropertySelector( property_selector_box_1_, "prop_selector_1" );

  property_selector_box_2_->setOrientation( Qt::Vertical );
  property_selector_box_2_->setColumns( 1 );
  property_selector_box_2_->setInsideMargin( 0 );
  property_selector_box_2_->setInsideSpacing( 0 );
  prop_selector_var2_ = 
    new SinglePropertySelector( property_selector_box_2_, "prop_selector_2" );

  init( proj );

  // forward signals
  QObject::connect( lsfit_checkbox_,
                    SIGNAL( toggled( bool ) ),
                    SIGNAL( show_ls_fit( bool ) ) );

  QObject::connect( object_selector_,
                    SIGNAL( activated( const QString& ) ),
                    prop_selector_var1_,
                    SLOT( show_properties( const QString& ) ) );
  QObject::connect( object_selector_,
                    SIGNAL( activated( const QString& ) ),
                    prop_selector_var2_,
                    SLOT( show_properties( const QString& ) ) );


  QObject::connect( (QObject*) minval_edit_1_, SIGNAL( returnPressed() ),
                    this, SLOT( forward_var1_low_clip_changed() ) );
  QObject::connect( (QObject*) maxval_edit_1_, SIGNAL( returnPressed() ),
                    this, SLOT( forward_var1_high_clip_changed() ) );
  QObject::connect( (QObject*) reset_clips_button_1_, SIGNAL( clicked() ),
                    SIGNAL( reset_var1_clipping_values_clicked() ) );

  QObject::connect( (QObject*) minval_edit_2_, SIGNAL( returnPressed() ),
                    this, SLOT( forward_var2_low_clip_changed() ) );
  QObject::connect( (QObject*) maxval_edit_2_, SIGNAL( returnPressed() ),
                    this, SLOT( forward_var2_high_clip_changed() ) );
  QObject::connect( (QObject*) reset_clips_button_2_, SIGNAL( clicked() ),
                    SIGNAL( reset_var2_clipping_values_clicked() ) );

  QObject::connect( prop_selector_var1_,
                    SIGNAL( activated( const QString& ) ),
                    this,
                    SLOT( forward_var1_changed( const QString& ) ) );

  QObject::connect( prop_selector_var2_,
                    SIGNAL( activated( const QString& ) ),
                    this,
                    SLOT( forward_var2_changed( const QString& ) ) );

}




Scatterplot_control_panel::~Scatterplot_control_panel() {
}

void Scatterplot_control_panel::init( GsTL_project* project ) {
  object_selector_->init( project );
}


bool Scatterplot_control_panel::ls_fit_shown() const {
  return lsfit_checkbox_->isChecked(); 
}


void Scatterplot_control_panel::set_var1_clipping_values( float low, float high ) {
  QString val;
  val.setNum( low );
  minval_edit_1_->setText( val );
  val.setNum( high );
  maxval_edit_1_->setText( val );
}

void Scatterplot_control_panel::set_var2_clipping_values( float low, float high ) {
  QString val;
  val.setNum( low );
  minval_edit_2_->setText( val );
  val.setNum( high );
  maxval_edit_2_->setText( val );
}



GsTLGridProperty*
Scatterplot_control_panel::get_property( const SinglePropertySelector* prop_selector ) {
  std::string grid_name( object_selector_->currentText().latin1() );
  if( grid_name.empty() ) return 0;

  Geostat_grid* grid = dynamic_cast<Geostat_grid*>(
                Root::instance()->interface(
                                            gridModels_manager + "/" + grid_name
                                            ).raw_ptr()
                );

  appli_assert( grid );

  if( prop_selector->currentText().isEmpty() ) return 0;

  std::string prop_name = std::string( prop_selector->currentText().latin1() );
  GsTLGridProperty* prop = grid->property( prop_name );
  appli_assert( prop );
  return prop;
}
 


void Scatterplot_control_panel::forward_var1_changed( const QString& ) {
  emit var1_changed( get_property( prop_selector_var1_ ) );
}

void Scatterplot_control_panel::forward_var2_changed( const QString& ) {
  emit var2_changed( get_property( prop_selector_var2_ ) );
}


void Scatterplot_control_panel::forward_var1_low_clip_changed() {
  emit var1_low_clip_changed( minval_edit_1_->text().toFloat() );
}
void Scatterplot_control_panel::forward_var1_high_clip_changed() {
  emit var1_high_clip_changed( maxval_edit_1_->text().toFloat() );
}

void Scatterplot_control_panel::forward_var2_low_clip_changed() {
  emit var2_low_clip_changed( minval_edit_2_->text().toFloat() );
}
void Scatterplot_control_panel::forward_var2_high_clip_changed() {
  emit var2_high_clip_changed( maxval_edit_2_->text().toFloat() );
}



void Scatterplot_control_panel::show_lsfit_coeffs( float slope, float intercp ) {
  slope_label_->setNum( slope );
  intercept_label_->setNum( intercp );
}


void Scatterplot_control_panel::
enable_lsfit( bool on ) {
  if( !on ) {
    lsfit_checkbox_->setChecked( false );
  }
  lsfit_groupbox_->setEnabled( on );
}