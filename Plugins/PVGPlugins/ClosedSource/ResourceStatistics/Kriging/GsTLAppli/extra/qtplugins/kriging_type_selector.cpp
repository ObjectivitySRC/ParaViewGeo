/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Geostatistical Earth
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

#include <GsTLAppli/extra/qtplugins/kriging_type_selector.h>
#include "selectors.h"

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlayout.h>

#include <utility>


TrendComponents::TrendComponents( QWidget* parent, const char* name )
  : QWidget( parent, name ) {
    
  QGridLayout* layout = new QGridLayout( this );
  layout->setAlignment( Qt::AlignTop );

  x_ = new QCheckBox( "X", this, "x_" );
  y_ = new QCheckBox( "Y", this, "y_" );
  z_ = new QCheckBox( "Z", this, "z_" );
  xy_ = new QCheckBox( "X.Y", this, "xy_" );
  xz_ = new QCheckBox( "XZ", this, "xz_" );
  yz_ = new QCheckBox( "YZ", this, "yz_" );
  x2_ = new QCheckBox( "X^2", this, "x2_" );
  y2_ = new QCheckBox( "Y^2", this, "y2_" );
  z2_ = new QCheckBox( "Z^2", this, "z2_" );

  layout->addWidget( x_, 0, 0 );
  layout->addWidget( y_, 0, 1 );
  layout->addWidget( z_, 0, 2 );
  layout->addWidget( xy_, 1, 0 );
  layout->addWidget( xz_, 1, 1 );
  layout->addWidget( yz_, 1, 2 );
  layout->addWidget( x2_, 2, 0 );
  layout->addWidget( y2_, 2, 1 );
  layout->addWidget( z2_, 2, 2 );
}



std::vector<bool> TrendComponents::trendComponents() const {
  std::vector<bool> flags;
  flags.push_back( x_->isChecked() );
  flags.push_back( y_->isChecked() );
  flags.push_back( z_->isChecked() );
  flags.push_back( xy_->isChecked() );
  flags.push_back( xz_->isChecked() );
  flags.push_back( yz_->isChecked() );
  flags.push_back( x2_->isChecked() );
  flags.push_back( y2_->isChecked() );
  flags.push_back( z2_->isChecked() );

  return flags;
}

void TrendComponents::setTrendComponents( const std::vector<bool>& flags ) {
  x_->setChecked( flags[0] );
  y_->setChecked( flags[1] );
  z_->setChecked( flags[2] );
  xy_->setChecked( flags[3] );
  xz_->setChecked( flags[4] );
  yz_->setChecked( flags[5] );
  x2_->setChecked( flags[6] );
  y2_->setChecked( flags[7] );
  z2_->setChecked( flags[8] );
}


//======================================================

KrigingTypeSelector::KrigingTypeSelector( QWidget* parent, const char* name )
  : Kriging_type_selector_base( parent, name ) {

  trend_components_box_->hide();
  local_mean_box_->hide();

  QObject::connect( kriging_type_, SIGNAL( activated( int ) ),
		    this, SLOT( promptForParameters( int ) ) );
  QObject::connect( kriging_type_, SIGNAL( activated( const QString& ) ),
		    this, SIGNAL( krigingTypeChanged( const QString& ) ) );
}


void KrigingTypeSelector::promptForParameters( int id ) {
  switch( id ) {
  case 0:   // Simple kriging
    sk_mean_box_->show();
    trend_components_box_->hide();
    local_mean_box_->hide();
    break;

  case 1:  // Ordinary kriging
    sk_mean_box_->hide();
    trend_components_box_->hide();
    local_mean_box_->hide();
    break;

  case 2:   // Kriging with trend
    sk_mean_box_->hide();
    trend_components_box_->show();
    local_mean_box_->hide();
    break;

  case 3:   // Simple kriging with varying mean
    sk_mean_box_->hide();
    trend_components_box_->hide();
    local_mean_box_->show();
    break;

  }
}


float KrigingTypeSelector::skMean() const {
  QString val = sk_mean_edit_->text();
  return val.toFloat();
}

void KrigingTypeSelector::setSkMean( float m ) {
  QString val;
  val.setNum( m );
  sk_mean_edit_->setText( val );
  emit krigingParametersChanged();
}


std::vector<bool> KrigingTypeSelector::trendComponents() const {
  std::vector<bool> flags;
  flags.push_back( x_->isChecked() );
  flags.push_back( y_->isChecked() );
  flags.push_back( z_->isChecked() );
  flags.push_back( x2_->isChecked() );
  flags.push_back( y2_->isChecked() );
  flags.push_back( z2_->isChecked() );
  flags.push_back( xy_->isChecked() );
  flags.push_back( xz_->isChecked() );
  flags.push_back( yz_->isChecked() );

  return flags;
}

void KrigingTypeSelector::setTrendComponents( const std::vector<bool>& flags ) {
  x_->setChecked( flags[0] );
  y_->setChecked( flags[1] );
  z_->setChecked( flags[2] );
  x2_->setChecked( flags[3] );
  y2_->setChecked( flags[4] );
  z2_->setChecked( flags[5] );
  xy_->setChecked( flags[6] );
  xz_->setChecked( flags[7] );
  yz_->setChecked( flags[8] );
  
  emit krigingParametersChanged();
}

std::pair<QString,QString> KrigingTypeSelector::LocalMeanProperty() const {
  return std::make_pair( local_mean_property_->selectedGrid(),
			 local_mean_property_->selectedProperty() );
}

void
KrigingTypeSelector::setLocalMeanProperty( const std::pair<QString,QString>& prop ) {
  local_mean_property_->setSelectedGrid( prop.first);
  local_mean_property_->setSelectedProperty( prop.second ); 

  emit krigingParametersChanged();
}


void KrigingTypeSelector::setKrigingType( const QString& type ) {

  if( type.contains( "LVM", false ) || type.contains( "local", false ) ) {
    kriging_type_->setCurrentItem( 3 );
    emit krigingTypeChanged( kriging_type_->currentText() );
    promptForParameters( 3 );
    return;
  }

  if( type.contains( "SK", false ) || type.contains( "simple", false ) ) {
    kriging_type_->setCurrentItem( 0 );
    emit krigingTypeChanged( kriging_type_->currentText() );
    promptForParameters( 0 );
    return;
  }

  if( type.contains( "OK", false ) || type.contains( "ordinary", false ) ) {
    kriging_type_->setCurrentItem( 1 );
    emit krigingTypeChanged( kriging_type_->currentText() );
    promptForParameters( 1 );
    return;
  }

  if( type.contains( "KT", false ) || type.contains( "trend", false ) ) {
    kriging_type_->setCurrentItem( 2 );
    emit krigingTypeChanged( kriging_type_->currentText() );
    promptForParameters( 2 );
    return;
  }
  
}

QString KrigingTypeSelector::krigingType() const {
  return kriging_type_->currentText();
}

