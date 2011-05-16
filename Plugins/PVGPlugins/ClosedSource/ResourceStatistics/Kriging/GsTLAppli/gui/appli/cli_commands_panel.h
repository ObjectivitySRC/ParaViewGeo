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

#ifndef __GSTLAPPLI_GUI_APPLI_CLI_COMMANDS_PANEL_H__
#define __GSTLAPPLI_GUI_APPLI_CLI_COMMANDS_PANEL_H__


#include <GsTLAppli/gui/common.h>

//TL modified
#include <GsTLAppli/gui/appli/historyscribe.h>
#include <GsTLAppli/utils/lineeditkey.h>

#include <qwidget.h>

class GsTL_project;
class QTextedit_scribe;
class QTextEdit;
class QLineEdit;


class GUI_DECL CLI_commands_panel : public QWidget {

  Q_OBJECT

public:
  CLI_commands_panel( QWidget* parent = 0, const char* name = 0 );
  CLI_commands_panel( GsTL_project* project,
                      QWidget* parent = 0, const char* name = 0 );

  ~CLI_commands_panel();

  void set_project( GsTL_project* project );


public slots: 
  void run_command();
  void run_script();

protected:
  void init();

protected:
  GsTL_project* project_;

  QTextEdit* log_window_;

  //TL modified
  //QLineEdit* command_edit_;
  LineEditKey * command_edit_;

  QTextedit_scribe* log_scribe_;
  QTextedit_scribe* messages_scribe_;
  QTextedit_scribe* errors_scribe_;
  QTextedit_scribe* script_messages_scribe_;
  QTextedit_scribe* script_errors_scribe_;

  // TL modified
  HistoryScribe * _hs;

};


#endif
