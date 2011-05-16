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

#include <GsTLAppli/gui/appli/cli_commands_panel.h>
#include <GsTLAppli/gui/utils/qtbased_scribes.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/actions/python_wrapper.h>

#include <qtextedit.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <qtabwidget.h>

CLI_commands_panel::CLI_commands_panel( QWidget* parent, const char* name )
: QWidget( parent, name ) {
  init();
}


CLI_commands_panel::
CLI_commands_panel( GsTL_project* project,
                    QWidget* parent, const char* name )
: QWidget( parent, name ) {
  project_ = project;
  init();
}
/*
void CLI_commands_panel::init() {
  QVBoxLayout* layout = new QVBoxLayout( this );
  layout->setMargin( 7 );
  //layout->setAutoAdd( true );

  QTabWidget* tab_widget = new QTabWidget( this, "tabs" );
    
  log_window_ = new QTextEdit( this, "message_box" );
  log_window_->setWordWrap( QTextEdit::NoWrap );
  //log_window_->setTextFormat( Qt::LogText );
  log_window_->setReadOnly( true );

  QTextEdit* script_output = new QTextEdit( this, "script_box" );
  script_output->setWordWrap( QTextEdit::NoWrap );
  script_output->setReadOnly( true );

  tab_widget->addTab( log_window_, "GEMS Commands History" );
  tab_widget->addTab( script_output, "Scripts Output" );


  QLabel* label2 = new QLabel( "Run Command:", this, "label2" );
  command_edit_ = new QLineEdit( this, "command_edit");
  QPushButton* clear_line_button = new QPushButton( "Clear", this, "button1" );
  QPushButton* run_script_button = 
    new QPushButton( "Execute Commands File...", this, "button2" );

  QHBoxLayout* line_layout = new QHBoxLayout( 0 );
  QHBoxLayout* run_script_layout = new QHBoxLayout( 0 );
  
  line_layout->addWidget( command_edit_ );
  line_layout->addSpacing( 8 );
  line_layout->addWidget( clear_line_button );

  run_script_layout->addWidget( run_script_button );
  run_script_layout->addStretch();

  layout->addWidget( tab_widget );
  layout->addSpacing( 16 );
  layout->addWidget( label2 );
  layout->addLayout( line_layout );
  layout->addSpacing( 5 );
  layout->addLayout( run_script_layout );


  log_scribe_ = new QTextedit_scribe( log_window_ );
  log_scribe_->subscribe( GsTLlog );
  messages_scribe_ = new QTextedit_scribe( log_window_, "blue" );
  messages_scribe_->subscribe( GsTLcout );
  errors_scribe_ = new QTextedit_scribe( log_window_, "red" );
  errors_scribe_->subscribe( GsTLcerr );

  script_messages_scribe_ = new QTextedit_scribe( script_output );
  script_messages_scribe_->subscribe( *GsTLAppli_Python_cout::instance() );
  script_errors_scribe_ = new QTextedit_scribe( script_output, "red" );
  script_errors_scribe_->subscribe( *GsTLAppli_Python_cerr::instance() );



  QObject::connect( command_edit_, SIGNAL( returnPressed() ),
                    this, SLOT( run_command() ) );
  QObject::connect( run_script_button, SIGNAL( clicked() ),
                    this, SLOT( run_script() ) );
  QObject::connect( clear_line_button, SIGNAL( clicked() ),
                    command_edit_, SLOT( clear() ) );

}
*/

void CLI_commands_panel::init() {
  QVBoxLayout* main_layout = new QVBoxLayout( this );
  main_layout->setMargin( 0 );
  //layout->setAutoAdd( true );

  QTabWidget* tab_widget = new QTabWidget( this, "tabs" );
  

  QWidget* commands_tab = new QWidget( this, "commands_tab" );
  QVBoxLayout* layout = new QVBoxLayout( commands_tab );
  layout->setMargin( 7 );
  
  log_window_ = new QTextEdit( commands_tab, "message_box" );
  log_window_->setWordWrap( QTextEdit::NoWrap );
  //log_window_->setTextFormat( Qt::LogText );
  log_window_->setReadOnly( true );

  QLabel* label2 = new QLabel( "Run Command:", commands_tab, "label2" );

  //TL modified
  //command_edit_ = new QLineEdit( commands_tab, "command_edit");
  command_edit_ = new LineEditKey( commands_tab, "command_edit");
  _hs = new HistoryScribe(command_edit_);

  QPushButton* clear_line_button = 
    new QPushButton( "Clear", commands_tab, "button1" );
  QPushButton* run_script_button = 
    new QPushButton( "Execute Commands File...", commands_tab, "button2" );

  QHBoxLayout* line_layout = new QHBoxLayout( 0 );
  QHBoxLayout* run_script_layout = new QHBoxLayout( 0 );
  
  line_layout->addWidget( command_edit_ );
  line_layout->addSpacing( 8 );
  line_layout->addWidget( clear_line_button );

  run_script_layout->addWidget( run_script_button );
  run_script_layout->addStretch();

  layout->addWidget( log_window_ );
  layout->addSpacing( 16 );
  layout->addWidget( label2 );
  layout->addLayout( line_layout );
  layout->addSpacing( 5 );
  layout->addLayout( run_script_layout );


  QTextEdit* script_output = new QTextEdit( this, "script_box" );
  script_output->setWordWrap( QTextEdit::NoWrap );
  script_output->setReadOnly( true );

  tab_widget->addTab( commands_tab, "S-GeMS Commands History" );
  tab_widget->addTab( script_output, "Scripts Output" );

  main_layout->addWidget( tab_widget );


  // TL modified
  _hs->subscribe(GsTLlog);
  const std::vector<std::string> & tp = _hs->history();
  command_edit_->setHistory(&tp);

  log_scribe_ = new QTextedit_scribe( log_window_ );
  log_scribe_->subscribe( GsTLlog );
  messages_scribe_ = new QTextedit_scribe( log_window_, "blue" );
  messages_scribe_->subscribe( GsTLcout );
  errors_scribe_ = new QTextedit_scribe( log_window_, "red" );
  errors_scribe_->subscribe( GsTLcerr );

  script_messages_scribe_ = new QTextedit_scribe( script_output );
  script_messages_scribe_->subscribe( *GsTLAppli_Python_cout::instance() );
  script_errors_scribe_ = new QTextedit_scribe( script_output, "red" );
  script_errors_scribe_->subscribe( *GsTLAppli_Python_cerr::instance() );



  QObject::connect( command_edit_, SIGNAL( returnPressed() ),
                    this, SLOT( run_command() ) );
  QObject::connect( run_script_button, SIGNAL( clicked() ),
                    this, SLOT( run_script() ) );
  QObject::connect( clear_line_button, SIGNAL( clicked() ),
                    command_edit_, SLOT( clear() ) );

}



CLI_commands_panel::~CLI_commands_panel() {
  delete log_scribe_;
  delete messages_scribe_;
  delete errors_scribe_;
  delete script_messages_scribe_;
  delete script_errors_scribe_;
}



void CLI_commands_panel::run_command() {
  if( !project_ ) return ;

  // TL modified
  command_edit_->reset();

  std::string command_line( command_edit_->text().stripWhiteSpace().latin1() );
  
  String_Op::string_pair args = String_Op::split_string( command_line, " ", false );
  if( args.first.empty() ) return;   // no command supplied

  Error_messages_handler error_messages;


  //TL modified
  // We do not want commands in the script to go into command history
  if (args.first == "RunScript") {
	  // need to save the "runscript ..." line
	  GsTLlog << command_line << gstlIO::end;
	  /*
	  if (!_hs->unsubscribe(GsTLlog)) { 
	  	appli_message("Unable to detach");
	  }
	  */
  }

  
  bool ok = project_->execute( args.first, args.second, &error_messages );
  /*
  if (args.first == "RunScript")
	_hs->subscribe(GsTLlog);
  */

  if( !ok ) {
    GsTLcerr << "Command " << args.first<< " could not be performed: \n";
	  if( !error_messages.empty() )
      GsTLcerr << error_messages.errors() << "\n"; 
    GsTLcerr << gstlIO::end;
    return;
  }
  
  command_edit_->clear();
}
 



void CLI_commands_panel::run_script() {
  QString filename = 
    QFileDialog::getOpenFileName( QString::null, QString::null, 
                                  this, "script dialog",
                                  "Select script file" );
  if( filename.isNull() ) return;


  QFile file( filename );
  if( !file.open( IO_ReadOnly ) ) {
    GsTLcerr << "Can not open " << filename.latin1() 
             << ": no such file" << gstlIO::end; 
    return;
  }
      
  QTextStream stream( &file );      
  QString line;

  while ( !stream.atEnd() ) {
    line = stream.readLine(); 
    line.stripWhiteSpace();
    if( line.startsWith( "#" ) || line.isEmpty() ) continue;

    Error_messages_handler error_messages;
    std::string command( line.ascii() );
    String_Op::string_pair args = 
      String_Op::split_string( command, " ", false );
  
    bool ok = project_->execute( args.first, args.second, &error_messages );
    if( !ok ) {
      GsTLcerr << "Command " << args.first<< " could not be performed: \n";
	    if( !error_messages.empty() )
        GsTLcerr << error_messages.errors() << "\n"; 
    }
  }

  GsTLcerr << gstlIO::end;
  
  file.close();

}
