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

#include <GsTLAppli/gui/utils/gstl_slider.h>

#include <qlineedit.h>
#include <qslider.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qpushbutton.h>


GsTL_slider::
GsTL_slider( double default_max, const QString& title, 
             QWidget* parent, const char* name )
  : QHBox( parent, name ) {

  default_max_ = default_max;
  init( title ); 
}

  
GsTL_slider::
GsTL_slider( double min_value, double max_value,
             double value, 
             double default_max, const QString& title, 
			       QWidget* parent, const char* name )
  : QHBox( parent, name ) {

  default_max_ = default_max;
  min_value_ = min_value;
  max_value_ = max_value;
  value_ = value;

  init( title );
  set_slider_value( value_ );

  QString value_str;
  value_str.setNum( value );
  lineedit_->setText( value_str );
}


void GsTL_slider::init( const QString& title ) {
  setSpacing( 4 );

  label_ = new QLabel( title, this, "label" );
  
  slider_ = new QSlider( Qt::Horizontal, this, "slider" );
  QSizePolicy slider_policy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  slider_->setSizePolicy( slider_policy );
  
  QSizePolicy policy( QSizePolicy::Minimum, QSizePolicy::Fixed );

  lineedit_ = new QLineEdit( this, "lineedit" );
  lineedit_->setMaximumWidth( 60 );
  lineedit_->setSizePolicy( policy );

  reset_button_ = new QPushButton( "Reset", this, "button" );
  reset_button_->setSizePolicy( policy );
  reset_button_->setMaximumWidth( 60 );

  setStretchFactor( slider_, 2 );

  QObject::connect( lineedit_, SIGNAL( returnPressed() ),
		    this, SLOT( updateSliderValue() ) );
  QObject::connect( slider_, SIGNAL( valueChanged( int ) ),
		    this, SLOT( updateLineedit( int ) ) );
  QObject::connect( reset_button_, SIGNAL( clicked() ),
		    this, SLOT( reset() ) );
 
  slider_->setMinValue( 0 );
  slider_->setMaxValue( slider_max_ );
  slider_->setPageStep( slider_step_ );
}


void GsTL_slider::set_slider_value( double val ) {
  if( max_value_ == min_value_ ) return;
  
  double p = (val-min_value_)/(max_value_-min_value_);
  int slider_val = int(p*100.0);
  slider_->setValue( slider_val );
}


void GsTL_slider::setMaxValue( double max ) {
  if( max_value_ == max ) return; 
  
  max_value_ = max;
  set_slider_value( value_ );
  emit maxValueChanged( max );
}


void GsTL_slider::setValue( double val ) {
  if( val > max_value_ ) {
    this->setMaxValue( 2*val );
  }

  value_ = val;
  set_slider_value( val );

  QString value_str;
  value_str.setNum( val );
  lineedit_->setText( value_str );
  
  emit valueChanged( val );
}

void GsTL_slider::updateLineedit( int slider_val ) {
  double p = double(slider_val)/double(slider_max_);
  double val = min_value_ + p*(max_value_-min_value_);
  value_ = val;

  QString value_str;
  value_str.setNum( val );
  lineedit_->setText( value_str );
  
  emit valueChanged( val );
}


void GsTL_slider::updateSliderValue() {
  double val = lineedit_->text().toDouble();
  this->setValue( val );

  emit valueChanged( val );
}


void GsTL_slider::reset() {
  max_value_ = default_max_;

  if( value_ > default_max_ )
    this->setValue( default_max_ / 2 );
  else
    this->setValue( value_ );

  emit maxValueChanged(max_value_);
}


/*
GsTL_slider::GsTL_slider( int default_max, const QString& title, 
			  QWidget* parent, const char* name )
  : QHBox( parent, name ) {
  default_max_ = default_max;
  init( title ); 
}

  
GsTL_slider::GsTL_slider( int min_value, int max_value, int page_step,
			  int value, 
			  int default_max, const QString& title, 
			  QWidget* parent, const char* name )
  : QHBox( parent, name ) {
  default_max_ = default_max;
  init( title );
  
  slider_->setMinValue( min_value );
  slider_->setMaxValue( max_value );
  slider_->setPageStep( page_step );
  slider_->setValue( value );

  QString value_str;
  value_str.setNum( value );
  lineedit_->setText( value_str );
}



void GsTL_slider::init( const QString& title ) {
  setSpacing( 4 );

  label_ = new QLabel( title, this, "label" );
  
  slider_ = new QSlider( Qt::Horizontal, this, "slider" );
  QSizePolicy slider_policy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  slider_->setSizePolicy( slider_policy );
  
  QSizePolicy policy( QSizePolicy::Minimum, QSizePolicy::Fixed );

  lineedit_ = new QLineEdit( this, "lineedit" );
  //  QRect rect = lineedit_->geometry();
  //  rect.setWidth( 10 );
  //  lineedit_->setGeometry( rect );
  lineedit_->setMaximumWidth( 60 );
  lineedit_->setSizePolicy( policy );

  reset_button_ = new QPushButton( "Reset", this, "button" );
  reset_button_->setSizePolicy( policy );
  reset_button_->setMaximumWidth( 60 );

  setStretchFactor( slider_, 2 );

  QObject::connect( lineedit_, SIGNAL( returnPressed() ),
		    this, SLOT( setSliderValue() ) );
  QObject::connect( slider_, SIGNAL( valueChanged( int ) ),
		    this, SLOT( setValue( int ) ) );
  QObject::connect( reset_button_, SIGNAL( clicked() ),
		    this, SLOT( reset() ) );
 
}


void GsTL_slider::setMaxValue( int max) {
  if( slider_->maxValue() == max ) return; 

  slider_->setMaxValue( max );

  int new_page_step = float( max - slider_->minValue() ) / 10.0 + 1;
  slider_->setPageStep( new_page_step );

  emit maxValueChanged( max );
}


void GsTL_slider::setValue( int val ) {
  if( val > slider_->maxValue() ) {
    this->setMaxValue( 2*val );
  }

  if( slider_->value() != val )
    slider_->setValue( val );

  QString value_str;
  value_str.setNum( val );
  lineedit_->setText( value_str );
  
  emit valueChanged( val );
}


void GsTL_slider::setValue( const QString& value_str ) {
  if( lineedit_->text() != value_str )
    lineedit_->setText( value_str );
  
  int val = value_str.toInt();
  if( val > slider_->maxValue() ) {
    this->setMaxValue( 2*val );
  }
  slider_->setValue( val );
  
  emit valueChanged( val );
}


void GsTL_slider::setSliderValue() {
  int val = lineedit_->text().toInt();

  if( val == slider_->value() ) return;

  if( val > slider_->maxValue() ) {
    this->setMaxValue( 2*val );
  }
  slider_->setValue( val );
  
  emit valueChanged( val );
}

void GsTL_slider::reset() {
  this->setMaxValue( default_max_ );

  if( slider_->value() > default_max_ )
    this->setValue( default_max_ / 2 );
}
*/

