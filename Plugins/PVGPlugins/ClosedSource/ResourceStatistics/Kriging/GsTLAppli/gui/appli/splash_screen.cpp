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

#include <GsTLAppli/gui/appli/splash_screen.h>
#include <GsTLAppli/sgems_version.h>

#include <qapplication.h>
#include <qpainter.h>

/*
SplashScreen::SplashScreen( const QPixmap& pixmap )
: QWidget( 0, 0, WStyle_Customize | WStyle_Splash ),
  pixmap_( pixmap ) {

  if( pixmap_.isNull() ) return;

  setErasePixmap( pixmap_ );
  resize( pixmap_.size() );
  QRect scr = QApplication::desktop()->screenGeometry();
  move( scr.center() - rect().center() );
  show();
  repaint();
}
*/
 
SplashScreen::SplashScreen( const QPixmap& pixmap )
: QWidget( 0, 0, WStyle_Customize | WStyle_Splash ),
  pixmap_( pixmap ) {

  if( pixmap_.isNull() ) return;

  QString version( GEMS_VERSION_STR );
 
  setFont( QFont("Times", 12, QFont::Bold) );
  QPainter painter( &pixmap_, this );
  painter.setPen( black );
  QRect r = rect();
  r.setRect( r.x() + 450, r.y() +40, r.width() - 20, r.height() - 20 );
  painter.drawText( r, Qt::AlignLeft, QString( "v" ) + version );

  setErasePixmap( pixmap_ );
  resize( pixmap_.size() );
  QRect scr = QApplication::desktop()->screenGeometry();
  move( scr.center() - rect().center() );
  show();
  repaint();
}


void SplashScreen::repaint() {
  QWidget::repaint();
  QApplication::flush();
}


#if defined(Q_WS_X11)
void qt_wait_for_window_manager( QWidget* widget );
#endif

void SplashScreen::finish( QWidget* main_win ) {
#if defined(Q_WS_X11)
  qt_wait_for_window_manager( main_win );
#endif
  close();
}


void SplashScreen::setStatus( const QString& message, int alignment, 
                             const QColor& color ) {
  if( pixmap_.isNull() ) return;

  QPixmap textPix =  pixmap_;
  QPainter painter( &textPix, this );
  painter.setPen( color );
  QRect r = rect();
//  r.setRect( r.x() + 10, r.y() +10, r.width() - 20, r.height() - 20 );
  r.setRect( r.x() + 30, r.y() +120, r.width() - 20, r.height() - 20 );
  painter.drawText( r, alignment, message );
  setErasePixmap( textPix );
  repaint();
}


void SplashScreen::mousePressEvent( QMouseEvent* ) {
  hide();
}
