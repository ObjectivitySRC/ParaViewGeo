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

#include <GsTLAppli/extra/qtplugins/ellipsoid_input.h>
#include <GsTLAppli/extra/qtplugins/gstl_table.h>

#include <qlayout.h>


EllipsoidInput::EllipsoidInput( QWidget *parent, const char *name ) 
  : QWidget( parent, name ) {
  
  QVBoxLayout* main_layout = new QVBoxLayout( this );

  ranges_table_ = new GsTLTable( this, "ranges" );
  angles_table_ = new GsTLTable( this, "angles" );
  main_layout->addWidget( ranges_table_ );
  main_layout->addSpacing( 4 );
  main_layout->addWidget( angles_table_ );

  ranges_table_->setMinimumWidth( 150 );
  angles_table_->setMinimumWidth( 150 );

  ranges_table_->setNumRows( 1 );
  ranges_table_->setNumCols( 3 );
  angles_table_->setNumRows( 1 );
  angles_table_->setNumCols( 3 );

  ranges_table_->verticalHeader()->setLabel( 0, "Ranges" );
  ranges_table_->horizontalHeader()->setLabel( 0, "Max" );
  ranges_table_->horizontalHeader()->setLabel( 1, "Med" );
  ranges_table_->horizontalHeader()->setLabel( 2, "Min" );
  
  angles_table_->verticalHeader()->setLabel( 0, "Angles" );
  angles_table_->horizontalHeader()->setLabel( 0, "Azimuth" );
  angles_table_->horizontalHeader()->setLabel( 1, "Dip" );
  angles_table_->horizontalHeader()->setLabel( 2, "Rake" );

  for( int i = 0; i < 3; i++ ) {
    ranges_table_->setColumnWidth( i, 60 );
    angles_table_->setColumnWidth( i, 60 );
  }

  ranges_table_->setMaximumHeight( 40 );
  ranges_table_->setFrameShape( QFrame::NoFrame );
  ranges_table_->setFrameShadow( QFrame::Plain );

  angles_table_->setMaximumHeight( 40 );
  angles_table_->setFrameShape( QFrame::NoFrame );
  angles_table_->setFrameShadow( QFrame::Plain );
  
  ranges_table_->setVScrollBarMode( QScrollView::AlwaysOff );
  ranges_table_->setHScrollBarMode( QScrollView::AlwaysOff );
  angles_table_->setVScrollBarMode( QScrollView::AlwaysOff );
  angles_table_->setHScrollBarMode( QScrollView::AlwaysOff );

}



EllipsoidInput::~EllipsoidInput() {}

void EllipsoidInput::ranges( double& r1, double& r2, double& r3 ) {
  r1 = ranges_table_->text( 0,0 ).toDouble();  
  r2 = ranges_table_->text( 0,1 ).toDouble();  
  r3 = ranges_table_->text( 0,2 ).toDouble();  
}
 

double EllipsoidInput::max_range() const {
  return ranges_table_->text( 0,0 ).toDouble();  
}
double EllipsoidInput::med_range() const {
  return ranges_table_->text( 0,1 ).toDouble();  
}
double EllipsoidInput::min_range() const{
  return ranges_table_->text( 0,2 ).toDouble();  
}

void EllipsoidInput::angles( double& a1, double& a2, double& a3 ) {
  a1 = angles_table_->text( 0,0 ).toDouble();  
  a2 = angles_table_->text( 0,1 ).toDouble();  
  a3 = angles_table_->text( 0,2 ).toDouble();  
}

double EllipsoidInput::azimuth() const {
  return angles_table_->text( 0,0 ).toDouble();  
}
double EllipsoidInput::dip() const {
  return angles_table_->text( 0,1 ).toDouble();  
}
double EllipsoidInput::rake() const {
  return angles_table_->text( 0,2 ).toDouble();  
}

void EllipsoidInput::set_ranges( double r1, double r2, double r3 ) {
  QString val;
  val.setNum( r1 );
  ranges_table_->setText( 0,0, val );
  val.setNum( r2 );
  ranges_table_->setText( 0,1, val );
  val.setNum( r3 );
  ranges_table_->setText( 0,2, val );
}
void EllipsoidInput::set_angles( double r1, double r2, double r3 ) {
  QString val;
  val.setNum( r1 );
  angles_table_->setText( 0,0, val );
  val.setNum( r2 );
  angles_table_->setText( 0,1, val );
  val.setNum( r3 );
  angles_table_->setText( 0,2, val );
}


void EllipsoidInput::set_max_range( double a ) {
  QString val;
  val.setNum( a );
  ranges_table_->setText( 0,0, val ); 
}

void EllipsoidInput::set_med_range( double a ) {
  QString val;
  val.setNum( a );
  ranges_table_->setText( 0,1, val ); 
}
void EllipsoidInput::set_min_range( double a ) {
  QString val;
  val.setNum( a );
  ranges_table_->setText( 0,2, val ); 
}


void EllipsoidInput::set_azimuth( double a ) {
  QString val;
  val.setNum( a );
  angles_table_->setText( 0,0, val ); 
}
void EllipsoidInput::set_dip( double a ) {
  QString val;
  val.setNum( a );
  angles_table_->setText( 0,1, val ); 
}
void EllipsoidInput::set_rake( double a ) {
  QString val;
  val.setNum( a );
  angles_table_->setText( 0,2, val ); 
}



void EllipsoidInput::resizeEvent( QResizeEvent * e ) {
  const int label_width = 70;
  int cell_width = ( ranges_table_->size().width() - label_width ) / 3 - 2;

  for( int i = 0; i < 3; i++ ) {
    ranges_table_->setColumnWidth( i, cell_width );
    angles_table_->setColumnWidth( i, cell_width );
  }
  QWidget::resizeEvent( e );
}

void EllipsoidInput::set_max_width( int w ) {
  ranges_table_->setMaximumWidth( w );
  angles_table_->setMaximumWidth( w ); 

  const int label_width = 70;
  int cell_width = ( ranges_table_->size().width() - label_width ) / 3 - 2;

  for( int i = 0; i < 3; i++ ) {
    ranges_table_->setColumnWidth( i, cell_width );
    angles_table_->setColumnWidth( i, cell_width );
  }

}
