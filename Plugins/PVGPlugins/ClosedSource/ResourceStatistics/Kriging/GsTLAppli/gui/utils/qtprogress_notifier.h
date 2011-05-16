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

#ifndef __GSTLAPPLI_GUI_UTILS_QTPROGRESS_NOTIFIER_H__ 
#define __GSTLAPPLI_GUI_UTILS_QTPROGRESS_NOTIFIER_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/utils/progress_notifier.h> 
 
#include <qapplication.h> 
#include <qlabel.h> 
#include <qstring.h> 
 
#include <string> 
 
class QProgressDialog; 
class Notifier_label; 
 

/** QtProgress_notifier displays a Qt progress bar to show the progress
* of a given task. It also changes the cursor to the busy state. The cursor
* is turned back to its default state when the progress notifier is destroyed.
*/ 
class GUI_DECL QtProgress_notifier : public Progress_notifier { 
public: 
  static Named_interface* create_new_interface( std::string& );

  QtProgress_notifier( int total_steps, const std::string& title = "" ); 
  virtual ~QtProgress_notifier(); 
  virtual void title( const std::string& str );
  virtual void total_steps( int count );

  virtual bool notify(); 
  virtual void write( const std::string& str, const Channel* ); 

 
protected: 
  QProgressDialog* dialog_; 
  Notifier_label* label_; 
 
  long int requested_; 
  int steps_done_; 
 
}; 
 

/** Notifier_label is just a helper class used by QtProgress_notifier to 
* display messages about the progress of a task.
*/ 
class GUI_DECL Notifier_label : public QLabel { 
 public: 
  Notifier_label( QWidget* parent, const char* name,  
		  const QString& header ); 
  virtual void setText( const QString& text ); 
  virtual void setHeader( const QString& text );

 private: 
  QString header_; 
}; 
 
  
 
#endif 
