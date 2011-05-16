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

#include <GsTLAppli/gui/appli/histogram_control_panel.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>

#include <qgroupbox.h>
#include <qlineedit.h>
#include <qspinbox.h>


Histogram_control_panel::
Histogram_control_panel( GsTL_project* proj,
                         QWidget* parent, const char* name ) 
  : Histogram_control_panel_base( parent, name ) {
  
  property_selector_box_->setOrientation( Qt::Vertical );
  property_selector_box_->setColumns( 1 );
  property_selector_box_->setInsideMargin( 0 );
  property_selector_box_->setInsideSpacing( 0 );
  object_selector_ = new PropertySelector( property_selector_box_,
                                           "object_selector_" );

  //TL modified
  _curves_box->setOrientation( Qt::Vertical );
  _curves_box->setColumns( 1 );
  _combo = new QComboBox(_curves_box, "combo");
  _combo->insertItem("pdf");
  _combo->insertItem("cdf");
  _combo->insertItem("pdf+cdf");

  init( proj );

  // forward signals
   QObject::connect( object_selector_,
                    SIGNAL( property_changed( const QString& ) ),
                    this,
                    SLOT( forward_var_changed( const QString& ) ) );

  QObject::connect( (QObject*) minval_edit_, SIGNAL( returnPressed() ),
                    this, SLOT( forward_low_clip_changed() ) );
  QObject::connect( (QObject*) maxval_edit_, SIGNAL( returnPressed() ),
                    this, SLOT( forward_high_clip_changed() ) );
  QObject::connect( (QObject*) reset_clips_button_, SIGNAL( clicked() ),
                    SIGNAL( reset_clipping_values_clicked() ) );

  QObject::connect( bins_spinbox_, SIGNAL( valueChanged( int ) ),
                    SIGNAL( bins_count_changed( int ) ) );

  QObject::connect( _combo, SIGNAL( activated( const QString & ) ),
                    SIGNAL( comboChanged( const QString & ) ) );


}




Histogram_control_panel::~Histogram_control_panel() {
}

void Histogram_control_panel::init( GsTL_project* project ) {
  object_selector_->init( project );
}

int Histogram_control_panel::bins_count() const {
  return bins_spinbox_->value();
} 



void Histogram_control_panel::set_clipping_values( float low, float high ) {
  QString val;
  val.setNum( low );
  minval_edit_->setText( val );
  val.setNum( high );
  maxval_edit_->setText( val );
}


GsTLGridProperty*
Histogram_control_panel::get_property( const PropertySelector* object_selector ) {
  if( object_selector->selectedGrid().isEmpty() ||
      object_selector->selectedProperty().isEmpty() ) return 0;
  
  std::string grid_name( object_selector->selectedGrid().latin1() );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>(
                Root::instance()->interface(
                                            gridModels_manager + "/" + grid_name
                                            ).raw_ptr()
                );

  appli_assert( grid );

  std::string prop_name( object_selector->selectedProperty().latin1() );
  GsTLGridProperty* prop = grid->property( prop_name );
  appli_assert( prop );
  return prop;
}
 


void Histogram_control_panel::forward_var_changed( const QString& ) {
  emit var_changed( get_property( object_selector_ ) );
}

void Histogram_control_panel::forward_low_clip_changed() {
  emit low_clip_changed( minval_edit_->text().toFloat() );
}
void Histogram_control_panel::forward_high_clip_changed() {
  emit high_clip_changed( maxval_edit_->text().toFloat() );
}


