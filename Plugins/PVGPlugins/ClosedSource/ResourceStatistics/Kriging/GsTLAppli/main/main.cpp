/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "main" module of the Geostatistical Earth
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

//#ifdef _DEBUG
//#undef _DEBUG
//#include <Python.h>
//#define _DEBUG
//#else
//#include <Python.h>
//#endif
#include <Python.h>

#include <GsTLAppli/main/lib_initializer.h>
//#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
//#include <GsTLAppli/gui/oinv_description/gstl_SoClipPlaneManip.h>
#include <GsTLAppli/gui/appli/qt_sp_application.h>
#include <GsTLAppli/gui/appli/splash_screen.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/progress_notifier.h>
#include <GsTLAppli/actions/python_wrapper.h>

#include <GsTLAppli/actions/library_actions_init.h>


#include <qapplication.h>
#include <qstatusbar.h>
#include <qwidgetfactory.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qsettings.h>

#include <Inventor/Qt/SoQt.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>

#include <iostream>
#include <fstream>
#include <cstdio>




// Std_scribe is a scribe that writes to standard output or standard error
class Std_scribe : public Scribe {
public:
  Std_scribe( std::ostream& os ) : os_(os) {}
  virtual void write( const std::string& str, const Channel* ) {
    os_ << str ;
  }

private:
  std::ostream& os_;
};


class File_logger : public Scribe {
public:
  File_logger( const std::string& filename ) 
    :of_( 0 ), append_newline_( false ) 
  {
	  of_ = new std::ofstream(filename.c_str()) ;
    if( of_->bad() ) {
      std::cerr << "unable to open filename" << std::endl;
      of_->open( "log.txt" );
      appli_assert( of_ );
    }
  }

  bool append_newline() const { return append_newline_; }
  void append_newline( bool on ) { append_newline_ = on; }

  virtual void write( const std::string& str, const Channel* ) {
    *of_ << str;
    if( append_newline_ ) 
      *of_ << std::endl;
    else
      *of_ << std::flush;
  }

private:
  std::ofstream* of_;
  bool append_newline_;
};



class QTscribe : public Scribe {
public:
  virtual void write( const std::string& str, const Channel* ) {
    QMessageBox::critical( qApp->mainWidget(), "An Error Occurred...", 
			   QString( str.c_str() ), 
			   QMessageBox::Ok, QMessageBox::NoButton );
    
  }
};

class Status_bar_scribe : public Scribe {
public:
  Status_bar_scribe( QStatusBar* bar ) : bar_( bar ) {}
  virtual void write( const std::string& str, const Channel* ) {
    QString msg( str.c_str() );
    //msg.prepend( "Error: " );
    bar_->message( msg );
  }
private:
  QStatusBar* bar_;
};





//====================================================


void print_help() {
  std::cerr << "Usage: sgems [-s] file\n"
            << "If the -s option is specified, the file is a Python script\n"
            << "file. Otherwise, the file contains a list of S-GeMS commands\n"
            << "that will be executed sequentially."
            << std::endl;  
}

int nogui_main( int argc, char** argv ) {

  if( argc < 2 ) {
    print_help();
    return 1;
  }

  QString arg( argv[1] );
  if( argc == 2 && (arg.contains("-h") || arg.contains("--help")) ) {
    print_help();
    return 0;
  }

  Std_scribe* error_scribe = new Std_scribe( std::cerr );
  error_scribe->subscribe( GsTLcerr );

  QApplication app( argc, argv );
  Lib_initializer::minimal_init();
  Lib_initializer::load_geostat_algos();
  Lib_initializer::load_filters_plugins();

  //-------------------------------
  // create a default project
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "project", projects_manager + "/" + "project" );
  GsTL_project* project = dynamic_cast<GsTL_project*>( ni.raw_ptr() );
  appli_assert( project );
  Python_project_wrapper::set_project( project );


  //-------------------------------
  // Use a text-based progress notifier to report on progress of long tasks
  Root::instance()->factory( "progress_notifier", 
                             Text_progress_notifier::create_new_interface );


 

  if( argc == 2 ) {
    // The file contains S-GeMS commands
    const char* script_filename = argv[1];
    QFile file( script_filename );
    if( !file.open( IO_ReadOnly ) ) {
      GsTLcerr << "Can not open " << script_filename
               << ": no such file" << gstlIO::end; 
      return 1;
    }
    std::cerr << "Executing commands...\n" << std::endl;

    QTextStream stream( &file );
    QString line;
    while ( !stream.atEnd() ) {
      line = stream.readLine(); 
      line.stripWhiteSpace();
      if( line.startsWith( "#" ) ) continue;

      Error_messages_handler error_messages;
      std::string command = line.ascii();
      String_Op::string_pair args = 
        String_Op::split_string( command, " ", false );

      GsTLcerr << "Running: " << args.first << "\n" << gstlIO::end;
  
      bool ok = project->execute( args.first, args.second, &error_messages );
      if( !ok ) {
        GsTLcerr << "Command " << args.first<< " could not be performed: \n";
	      if( !error_messages.empty() )
          GsTLcerr << error_messages.errors() << "\n"; 
      }
    }

    file.close();
  }
  else {
    QString switch_arg( argv[1] );
    if( !switch_arg.contains("-s") ) {
      print_help();
      return 1;
    }

    // The file contains a Python script

    const char* script_filename = argv[2];
    QFile file( script_filename );
    if( !file.open( IO_ReadOnly ) ) {
      GsTLcerr << "Can not open " << script_filename
               << ": no such file" << gstlIO::end; 
      return 1;
    }
    file.close();

      
    std::cerr << "Executing script...\n" << std::endl;

    FILE* fp = fopen( argv[2], "r" );

    if( !fp ) {

      std::cerr << "can't open file " << argv[2] << std::endl;

      return 1;

    }
    PyRun_SimpleFile( fp, argv[2] );

    fclose( fp );
  }


  GsTLcerr << gstlIO::end;
  Root::instance()->delete_interface( gridObject_manager );

  libGsTLAppli_actions_release();
  return 0;
}






QString path_to_splash_image() {
  QFileInfo finfo;
  QString image_file( "new_splash2.png" );

  char* env = getenv("GSTLAPPLIHOME"); 
  if( env ) { 
    QString envpath( env );
    QDir homedir( envpath );
    homedir.cd( "bin" );
    finfo.setFile( homedir, image_file );
  } 
  else {
    QDir current_dir;
    finfo.setFile( current_dir, image_file );
  }
  return finfo.absFilePath();
}

QString path_to_plugins() {
  QDir pluginsdir; //points to current directory
  
  char* env = getenv("GSTLAPPLIHOME"); 
  if( env ) { 
    QString envpath( env );
    pluginsdir.setPath( envpath );
  } 
  
  pluginsdir.cd( "plugins" );
  return pluginsdir.absPath();
}


int gui_main( int argc, char **argv ) {

  File_logger* initialization_log = new File_logger( "sgems_status.log" );
  initialization_log->subscribe( GsTLlog );

  QTscribe* qt_error_scribe = new QTscribe;
  qt_error_scribe->subscribe( GsTLcerr );

  GsTLlog << gstlIO::no_wait_end;
  //-----

  //---------------------
  // Initialize the libraries, manage the splash-screen, and set-up the
  // main application window

  QApplication app( argc, argv );
  QApplication::addLibraryPath( path_to_plugins() );

  
  QPixmap pixmap( path_to_splash_image() );

  SplashScreen* splash = new SplashScreen( pixmap );

  splash->setFont( QFont("Times", 8, QFont::Bold) );
  splash->setStatus( "Initializing..." );
  app.processEvents();

  Lib_initializer::init();
  
  splash->setStatus( "Loading colormaps..." );
  app.processEvents();
  Lib_initializer::load_colormaps();

  splash->setStatus( "Loading geostat plugins..." );
  app.processEvents();
  Lib_initializer::load_geostat_algos();
  Lib_initializer::load_filters_plugins();
  SoDB::init();
//  QSP_application* appli = new QSP_application( 0 );
//  SoQt::init( appli );
  //SoClipPlaneManip::initClass();
//  GsTL_SoClipPlaneManip::initClass();
  //GsTL_SoNode::initClass();
  SoVolumeRendering::init();

  QSP_application* appli = new QSP_application( 0 );
  SoQt::init( appli );
  
  app.setMainWidget( appli );
  
  Status_bar_scribe* statusbar_scribe = new Status_bar_scribe( appli->statusBar() );
  statusbar_scribe->subscribe( GsTLcerr );
  statusbar_scribe->subscribe( GsTLcout );


  appli->init();
  appli->setCaption( "S-GeMS" );

  // restore preferences
  QSettings settings;
  settings.setPath( "scrf.stanford.edu", "sgems", QSettings::User );
  int height = settings.readNumEntry( "/geometry/height", 480 );
  int width = settings.readNumEntry( "/geometry/width", 640 );
  appli->resize( QSize( width, height ) );

  bool show_algo_panel = settings.readBoolEntry( "/panels/algo", true );
  bool show_cli_panel = settings.readBoolEntry( "/panels/cli", false );

  appli->setIcon( QPixmap( "new_icon6_32x32.bmp" ) );
  appli->show();
  appli->show_algo_panel( show_algo_panel );
  appli->show_commands_panel( show_cli_panel );
  

  //------------------
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( projects_manager + "/" + "project" );
  GsTL_project* project = dynamic_cast<GsTL_project*>( ni.raw_ptr() );
  appli_assert( project );
  Python_project_wrapper::set_project( project );
  //------------------


  splash->finish( appli );
  delete splash;


  //------------------------
  // Everything is now ready. We set up one more logging device, and it's done

  GsTLlog << gstlIO::wait_end;
  
  initialization_log->unsubscribe( GsTLlog );

  File_logger* main_log = new File_logger( "sgems_history.log" );
  main_log->subscribe( GsTLlog );
  main_log->append_newline( true );

  /*
//-------------
  QSP_application appli( 0 );
  SoQt::init( &appli );
  
  app.setMainWidget( &appli );
  
  Status_bar_scribe* statusbar_scribe = new Status_bar_scribe( appli.statusBar() );
  statusbar_scribe->subscribe( GsTLcerr );

  appli.init();
  appli.show();
  appli.setCaption( "S-GeMS" );
//--------------
  */
  SoQt::mainLoop();
  
  //delete appli;
  Lib_initializer::release();

  Root::instance()->list_all( std::cout );

  return 0;
}




int main( int argc, char **argv ) {
  if( argc == 1 )
    return gui_main( argc, argv );
  else {
    // use no gui version
    return nogui_main( argc, argv );
  }
}

