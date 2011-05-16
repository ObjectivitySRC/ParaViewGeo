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

#ifndef __GSTLAPPLI_GUI_UTILS_SCRIPT_EDITOR_H__
#define __GSTLAPPLI_GUI_UTILS_SCRIPT_EDITOR_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/qtbased_scribes.h>

#include <qwidget.h>
#include <qmainwindow.h>

class QTextEdit;
class QLineEdit;
class GsTL_project;
class QCloseEvent;



class GUI_DECL Script_editor: public QMainWindow {

  Q_OBJECT

public:
  Script_editor( GsTL_project* project = 0, 
                 QWidget* parent = 0, const char* name = 0 );

  virtual ~Script_editor();

  virtual void closeEvent( QCloseEvent* e );


public slots:
  void new_script();
  void load_script();
  void save_script();
  void save_script_as();
  void go_to_line();
  void run_script();
  void set_point_size( int s );
  
  void close_slot();


private:
  void menu_setup();
  QString find_tmp_filename( const QString& filename );

protected:
  QTextEdit* script_in_;
  QTextEdit* script_out_;
  
  QString filename_;
  bool accept_close_;

  SmartPtr<QTextedit_scribe> script_messages_scribe_;
  SmartPtr<QTextedit_scribe> script_errors_scribe_;

  GsTL_project* project_;
};

#endif
