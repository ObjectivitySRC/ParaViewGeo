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
//#ifdef _DEBUG
//#undef _DEBUG
//#include <Python.h>
//#define _DEBUG
//#else
//#include <Python.h>
//#endif
#include <Python.h>

#include <GsTLAppli/sgems_version.h>
#include <GsTLAppli/gui/utils/qtprogress_notifier.h>
#include <GsTLAppli/gui/utils/multichoice_dialog.h>
#include <GsTLAppli/gui/utils/delete_properties_dialog.h>
#include <GsTLAppli/gui/utils/script_editor.h>
#include <GsTLAppli/gui/utils/qdirdialog.h>
#include <GsTLAppli/gui/appli/qt_sp_application.h>
#include <GsTLAppli/gui/appli/histogram_gui.h>
#include <GsTLAppli/gui/appli/qpplot_gui.h>
#include <GsTLAppli/gui/appli/qt_algo_control_panel.h>
#include <GsTLAppli/gui/appli/new_cartesian_grid_dialog.h>
#include <GsTLAppli/gui/appli/scatterplot_gui.h>
#include <GsTLAppli/gui/appli/oinv_project_view.h>
#include <GsTLAppli/gui/appli/cli_commands_panel.h>
#include <GsTLAppli/gui/variogram2/variogram_modeler_gui.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/filters/filter.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

#include <qmenubar.h>
#include <qlayout.h>
#include <qdockwindow.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qdir.h>
#include <qpushbutton.h>
#include <qdatetime.h>
#include <cstdio>
#include <qsettings.h>
#include <qlistbox.h>
#include <qcheckbox.h>

//--------------------------------------------------


QSP_application::QSP_application( QWidget* parent ) 
  : QMainWindow( parent ) {

  // Use a Qt-based progress notifier to report on progress of long tasks
  Root::instance()->factory( "progress_notifier", 
                             QtProgress_notifier::create_new_interface );
}


QSP_application::~QSP_application() {
  appli_warning( "destructor for QSP_application not implemented yet" );

  /*
  // tell the managers to delete all the objects they manage
  Manager::interface_iterator it = Root::instance()->begin_interfaces();
  for( ; it != Root::instance()->end_interfaces() ; ++it ) {
    std::string name = Root::instance()->name( it->raw_ptr() );
    Root::instance()->delete_interface( "/" + name );
  }

  // temporary
  Root::instance()->list_all( std::cout );
*/
}


void QSP_application::init() {
  
  // create a default project
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "project", projects_manager + "/" + "project" );
  project_ = dynamic_cast<GsTL_project*>( ni.raw_ptr() );
  appli_assert( project_ );
  

  //-----------
  // set up the algorithm panel as a dockable window

  dock_controls_ = new QDockWindow(QDockWindow::InDock, this );
  dock_controls_->setResizeEnabled(true);
  dock_controls_->setCloseMode( QDockWindow::Always );
  dock_controls_->setFixedExtentWidth( 250 );
  algo_panel_ = new Algo_control_panel( project_, dock_controls_, "algo_panel" );
  dock_controls_->setWidget( algo_panel_ );

  moveDockWindow( dock_controls_, Qt::DockLeft ); 
  dock_controls_->setCaption("Algorithms");
  dock_controls_->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  setAppropriate( dock_controls_, true );
  
  QObject::connect( dock_controls_, SIGNAL( visibilityChanged ( bool ) ),
                    this, SLOT( show_algo_panel( bool ) ) );


  //-----------
  // set up the cli panel as a dockable window

  dock_cli_ = new QDockWindow(QDockWindow::InDock, this );
  dock_cli_->setResizeEnabled(true);
  dock_cli_->setCloseMode( QDockWindow::Always );
  cli_panel_ = new CLI_commands_panel( project_, dock_cli_, "cli_panel" );
  dock_cli_->setWidget( cli_panel_ );
  
  moveDockWindow( dock_cli_, Qt::DockBottom ); 
  dock_cli_->setCaption("Commands");
  dock_cli_->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  //dock_cli_->setFixedExtentHeight( 20 );  //seems useless
  setAppropriate( dock_cli_, true );
  dock_cli_->hide();

  QObject::connect( dock_cli_, SIGNAL( visibilityChanged ( bool ) ),
                    this, SLOT( show_commands_panel( bool ) ) );

  
  //-----------
  // create a view of the project

  SmartPtr<Named_interface> view_ni = 
    Root::instance()->new_interface( "oinv_view", projectViews_manager + "/main_view" );
  default_3dview_ = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  appli_assert( default_3dview_ );
  default_3dview_->initialize( project_, this );
  

  // Old code - remove if everything works ok...
  // default_3dview_ = new Oinv_view( project_, this );
  // project_->add_view( default_3dview_ );
  
  setCentralWidget( default_3dview_ );
  

  // drag and drop
  setAcceptDrops( true );

  //----------------------
  // Temporary
  init_menu_bar();
  //----------------

}



void QSP_application::dragEnterEvent( QDragEnterEvent* event ) {
 // event->accept( QIconDrag::canDecode( event ) );
}
void QSP_application::dropEvent ( QDropEvent * ) {

}



//===============================

void QSP_application::init_menu_bar() {

  QPopupMenu* file = new QPopupMenu();
  file->insertItem( "Open Project", this, SLOT( load_project() ), CTRL+Key_O );
  file->insertItem( "Close Project", this, SLOT( close_project() ), CTRL+Key_W );
  file->insertSeparator();
  file->insertItem( "Save Project", this, SLOT( save_project() ), CTRL+Key_S );
  file->insertItem( "Save Project As...", this, SLOT( save_project_as() ) );
  file->insertSeparator();
  file->insertItem( "Quit", this, SLOT(quit_slot()), CTRL+Key_Q );
  menuBar()->insertItem( "&File", file );


  QPopupMenu* objects = new QPopupMenu();
  objects->insertItem( "New Cartesian Grid", this, 
		                   SLOT( new_cartesian_grid() ), CTRL+Key_N );
  objects->insertItem( "Load Object", this, SLOT( load_object() ), CTRL+Key_L );
  objects->insertSeparator();
  objects->insertItem( "Save Object", this, SLOT( save_object() ) );
  objects->insertSeparator();
  objects->insertItem( "Delete Objects", this, SLOT( delete_geostat_objects() ) );
  objects->insertItem( "Delete Properties", this, SLOT( delete_object_properties() ) );
  objects->insertSeparator();
  objects->insertItem( "Copy Property", this, SLOT( copy_property() ) );  
  menuBar()->insertItem( "&Objects", objects );

  
  QPopupMenu* data_analysis = new QPopupMenu();
  data_analysis->insertItem( "Histogram", this, SLOT(show_histogram_dialog() ), CTRL+Key_H );
  data_analysis->insertSeparator();
  data_analysis->insertItem( "QQ/PP -plot", this,
                             SLOT(show_qpplot_dialog() ) );
  data_analysis->insertItem( "Scatter-plot", this,
                             SLOT(show_scatterplot_dialog() ) );
  data_analysis->insertSeparator();
  data_analysis->insertItem( "Variogram", this,
                             SLOT(show_variogram_analyser() ) );

  menuBar()->insertItem( "&Data Analysis", data_analysis);


  view_menu_ = new QPopupMenu();
  view_menu_->setCheckable( true );
  view_menu_->insertItem( "New 3D-Camera", this, SLOT(new_camera() ) );
  view_menu_->insertSeparator();
  ap_ = view_menu_->insertItem( "Algorithms Panel", this, SLOT(show_algo_panel() ), CTRL+Key_P, 1 );
  cli_panel_id_ =
    view_menu_->insertItem( "Commands Panel", this, SLOT(show_commands_panel() ) );
  menuBar()->insertItem( "&View", view_menu_);
  view_menu_->setItemChecked( ap_, true );
  view_menu_->setItemChecked( cli_panel_id_, false );


  QPopupMenu* scripts_menu = new QPopupMenu();
  scripts_menu->insertItem( "Show Scripts Editor", this , SLOT(show_script_editor()) );
  scripts_menu->insertItem( "Run Script...", this, SLOT(run_script() ) );
  menuBar()->insertItem( "&Scripts", scripts_menu );


  QPopupMenu* help_menu = new QPopupMenu();
  help_menu->insertItem( "What's &This", this , SLOT(whatsThis()), SHIFT+Key_F1);
  help_menu->insertItem( "About S-GeMS", this, SLOT(about_slot() ) );
  menuBar()->insertItem( "&Help", help_menu );


#ifndef GSTLAPPLI_NDEBUG
  QPopupMenu* debug_menu = new QPopupMenu();
  debug_menu->insertItem( "Save Scene-Graph", this, SLOT( save_scenegraph() ) );
  debug_menu->insertItem( "Show managers", this, SLOT( list_managers() ), CTRL+Key_M );
  debug_menu->insertItem( "Show Prop Values", this, SLOT(show_prop_val() ) );
  menuBar()->insertItem( "&Debug", debug_menu );
#endif

}





//===============================

bool QSP_application::close_project() {
  if( !project_->has_changed() ) {
    project_->clear();
    return true;
  }

  switch( QMessageBox::information( this, "S-GeMS", 
                "The current project has been modified since last saved\n" 
                "Do you want to save the changes before exiting?",
                "&Save", "&Don't Save", "Cancel", 0, 2 ) ) {
    case 0:
      //save the changes
      save_project();
      break;

    case 1:
      // don't save
      break;

    case 2:
      // cancel
      return false;
  }

  project_->clear();
  return true;
}



void QSP_application::load_project() {
  if( !project_->is_empty() ) {
    bool ok = close_project();
    if( !ok ) return;
  }

  GsTLcout << "Loading project..." << gstlIO::end;

  QString start_dir = QString::null;
  if( !preferences_.last_load_directory.isEmpty() ) 
    start_dir = preferences_.last_load_directory;

  QDirDialog* fd = new QDirDialog( ".prj", start_dir, this, 
                                     "Load Project" );
  fd->setCaption( "Load Project - select a directory" );

  // show the dialog
  if( fd->exec() == QDialog::Rejected ) return;

  QString dirname = fd->selectedFile();
//  preferences_.project_name = dirname;

  // Save the path to the directory which contained the project. The directory
  // returned by the dialog is one level too low (ie it is the project 
  // directory, instead of the directory that contained the project directory) 
  const QDir* dialog_dir = fd->dir();
  QDir updir( *dialog_dir );
  updir.cdUp();
  preferences_.last_load_directory = updir.absPath();
  delete dialog_dir;


  QApplication::setOverrideCursor( waitCursor );

  Error_messages_handler error_messages;
  std::string param( std::string(dirname.latin1()) );
  std::string command( "LoadProject" );

  bool ok = project_->execute( command, param, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
	  if( !error_messages.empty() )
      GsTLcerr << error_messages.errors() << "\n"; 
    GsTLcerr << gstlIO::end;
    QApplication::restoreOverrideCursor();
    return;
  }
/*
  // Descend into the selected directory
  QDir* dir = new QDir( dirname );
  if( !dir->cd( dirname ) ) {
    GsTLcerr << "Could not load project " << dirname << gstlIO::end;
    return;
  }


  // Load all the files in the current directory using the default filter
  Error_messages_handler error_messages;

  const QFileInfoList* files_info = 
    dir->entryInfoList( QDir::Files | QDir::NoSymLinks );
  QFileInfoListIterator it( *files_info );
  QFileInfo* f_info = 0;

  while( (f_info = it.current()) != 0 ) {
    QString abspath = f_info->absFilePath();
    std::string param( std::string( abspath.latin1() ) + 
                       Actions::separator + "s-gems" );
    bool ok = project_->execute( "LoadObjectFromFile", param, &error_messages );
    if( !ok ) 
      error_messages.report( "... this error occurred while loading \"" + 
                             std::string( f_info->fileName().latin1() ) +"\"" );

    ++it;
  }
  delete dir;  
  

  // output reported errors
  if( !error_messages.empty() ) {
    GsTLcerr << "Some of the objects could not be loaded:\n" 
             << error_messages.errors() << gstlIO::end;
  }
*/

  project_->reset_change_monitor();

  GsTLcout << "Ready" << gstlIO::end;
  QApplication::restoreOverrideCursor();
}



void QSP_application::save_project() {
  if( project_->name() != "" ) {
    //save_project( preferences_.project_name );
    save_project( QString( project_->name().c_str() ) );
  }
  else {
    save_project_as();
  }
}


void QSP_application::save_project_as() {
  QString start_dir = QString::null;
  if( !preferences_.last_save_directory.isEmpty() ) 
    start_dir = preferences_.last_save_directory;

  QDirDialog* fd = new QDirDialog( ".prj", start_dir, this, 
                                  "Save Project", QFileDialog::AnyFile );
  fd->setCaption( "Save Project" );

  // show the dialog
  if( fd->exec() == QDialog::Rejected ) return;

//  QString dirname = fd->selectedFile();
  QString dirname = fd->selectedDirectory();
  save_project( dirname );
}


void QSP_application::save_project( const QString& dirName ) {

  QString dirname = dirName;
  if( !dirname.endsWith( ".prj" ) && !dirname.endsWith( ".prj/" ) ) 
    dirname.append( ".prj" );

//  preferences_.project_name = dirname;
  project_->name( std::string( dirname.latin1() ) ); 
  QDir* dir = new QDir( dirname );

  // If the directory already exists, erase all the files. 
  // Possible improvements: currently all the files are erased sequentially, and
  // if erasing fails, the functions is aborted. But that results in a partially
  // deleted project, and no new project saved. A better solution would be to 
  // check beforehand that we will be able to erase all the files (not difficult
  // with QFileInfo).
  if( dir->exists( dirname ) ) {
    bool ok = dir->cd( dirname );
    QStringList files = dir->entryList( QDir::Files );
    for( QStringList::iterator it = files.begin(); it != files.end(); ++it ) {
      if( !dir->remove( *it, false ) )
        GsTLcerr << "Project " << dirname.ascii() << " already exists and can not be "
                 << "overwritten" << gstlIO::end; 
    }
  }
  else {
    bool created = dir->mkdir( dirname );
    if( !created ) {
      GsTLcerr << "Could not create project " << dirname.ascii() << gstlIO::end;
      return;
    }
  }
  

  if( !dir->cd( dirname ) ) {
    GsTLcerr << "Could not create project " << dirname.ascii() << gstlIO::end;
    return;
  }
  dir->cdUp();
  preferences_.last_save_directory = dir->absPath();
  dir->cd( dirname );


  QApplication::setOverrideCursor( waitCursor );

  // Save all objects in the current directory using the default filter
  Error_messages_handler error_messages;

  const GsTL_project::String_list& grids = project_->objects_list();
  typedef GsTL_project::String_list::const_iterator const_iterator;
  for( const_iterator it = grids.begin(); it != grids.end(); ++it ) {
    QString qabs_file_path = dir->absFilePath( QString( it->c_str() ) );
    std::string abs_file_path( qabs_file_path.latin1() ); 
    std::string param( *it + Actions::separator + 
	              	     abs_file_path + Actions::separator +
		                   "s-gems" );
    bool ok = project_->execute( "SaveGeostatGrid", param, &error_messages );
    if( !ok ) 
      error_messages.report( "... this error occurred while saving " + *it );
  }

  if( !error_messages.empty() ) {
    GsTLcerr << "Some of the objects could not be saved:\n" 
             << error_messages.errors() << gstlIO::end;
  }

  project_->reset_change_monitor();
  delete dir;

  GsTLcout << "Project saved" << gstlIO::end;
  QApplication::restoreOverrideCursor();
}



void QSP_application::load_object() {
  
  QFileDialog* fd = new QFileDialog( this, "Load Object", true );
  fd->setCaption( "Load object - select a file" );

  // Set the filters for the file dialog
  QStringList filters_list;
  filters_list.push_back( "All (*)" );

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( topLevelInputFilters_manager );
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( dir );

  Manager::type_iterator begin = dir->begin();
  Manager::type_iterator end = dir->end();
  for( ; begin != end ; ++begin ) {
    SmartPtr<Named_interface> filter_ni = 
      Root::instance()->new_interface( *begin, topLevelInputFilters_manager + "/" );
    Input_filter* filter = 
      dynamic_cast<Input_filter*>( filter_ni.raw_ptr() );  
    appli_assert( filter );
    
    std::string filt_stl = *begin + " (" + filter->file_extensions() + ")";
    QString filter_name( filt_stl.c_str() );
    filters_list.push_back( filter_name ) ;
  }

  // Use the previous value entered by the user to initialize the dialog
  fd->setFilters( filters_list );
  if( !preferences_.last_input_filter.isEmpty() ) 
    fd->setSelectedFilter( preferences_.last_input_filter );

  if( !preferences_.last_load_directory.isEmpty() ) 
    fd->setDir( preferences_.last_load_directory );

  // show the dialog
  if( fd->exec() == QDialog::Rejected ) return;


  // gather the info input by the user
  const QDir* current_dir = fd->dir();
  preferences_.last_load_directory = current_dir->absPath();
  delete current_dir;

  QString file_name, qfilter;
  file_name = fd->selectedFile();
  qfilter = fd->selectedFilter();
  preferences_.last_input_filter = qfilter;
  std::string full_name( qfilter.latin1() );
  String_Op::string_pair filter_name = 
      String_Op::split_string( full_name, " (" );
  
  QApplication::setOverrideCursor( waitCursor );
  appli_message( "Loading file " << file_name 
		<< " with filter \'" << filter_name.first << "\'" );

  Error_messages_handler error_messages;

  std::string param( std::string(file_name.latin1()) + Actions::separator
		     + filter_name.first );
  std::string command( "LoadObjectFromFile" );

  bool ok = project_->execute( command, param, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
	  if( !error_messages.empty() )
      GsTLcerr << error_messages.errors() << "\n"; 
    GsTLcerr << gstlIO::end;
  }

  QApplication::restoreOverrideCursor();
}




void QSP_application::save_object() {

  Save_grid_dialog* fd = new Save_grid_dialog( project_, this, "Save Grid" );
  fd->setCaption( "Save object" );

  if( !preferences_.last_save_directory.isEmpty() ) 
    fd->setDir( preferences_.last_save_directory );

  if( fd->exec() == QDialog::Rejected ) return;
  
  const QDir* dir = fd->dir();
  preferences_.last_save_directory = dir->absPath();
  delete dir;
  preferences_.last_output_filter = fd->selectedFilter();

  //TL modified
  const std::vector<QString> & p_list = fd->selected_p();

  std::string file_name, cluttered_filter_name, grid_name;
  file_name = fd->selectedFile().latin1();
  cluttered_filter_name = fd->selectedFilter().latin1();
  grid_name = fd->selected_grid().latin1();

  String_Op::string_pair filter_name = 
      String_Op::split_string( cluttered_filter_name, " (" );
  
  appli_message( "Saving grid " << grid_name << " to file " << file_name  
		<< " with filter \'" << filter_name.first << "\'" );

  QApplication::setOverrideCursor( waitCursor );

  Error_messages_handler error_messages;

  std::string param( grid_name + Actions::separator + 
		     file_name + Actions::separator +
		     filter_name.first );

  //TL modified
  if (fd->maskToRegular())
	  param += Actions::separator+"1";
  else
	  param += Actions::separator+"0";
  for (std::vector<QString>::const_iterator it = p_list.begin(); it != p_list.end(); ++it)
	  param += Actions::separator+(*it).ascii();

  std::string command( "SaveGeostatGrid" );
  bool ok = project_->execute( command, param, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n" ;
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
  }

  QApplication::restoreOverrideCursor();
}



void QSP_application::new_cartesian_grid() {
  NewCartesianGridDialog* dialog = 
    new NewCartesianGridDialog( this, "New Cartesian Grid" );
  dialog->setCaption( "Create new cartesian grid" );

  if( dialog->exec() == QDialog::Rejected ) return;

  std::string parameters = dialog->grid_parameters();
  Error_messages_handler error_messages;

  std::string command( "NewCartesianGrid" );
  bool ok = project_->execute( command, parameters, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
  }
}



void QSP_application::show_algo_panel() {
  if( !dock_controls_->isVisible() ) {
    dock_controls_->show();
    view_menu_->setItemChecked( ap_, true );
  }
  else {
    dock_controls_->hide();
    view_menu_->setItemChecked( ap_, false );
  }
}

void QSP_application::show_commands_panel() {
  if( !dock_cli_->isVisible() ) {
    dock_cli_->show();
    view_menu_->setItemChecked( cli_panel_id_, true );
  }
  else {
    dock_cli_->hide();
    view_menu_->setItemChecked( cli_panel_id_, false );
  }
}


void QSP_application::show_algo_panel( bool on ) {
  if( on ) {
    dock_controls_->show();
    view_menu_->setItemChecked( ap_, true );
  }
  else {
    dock_controls_->hide();
    view_menu_->setItemChecked( ap_, false );
  }
}

void QSP_application::show_commands_panel( bool on ) {
  if( on ) {
    dock_cli_->show();
    view_menu_->setItemChecked( cli_panel_id_, true );
  }
  else {
    dock_cli_->hide();
    view_menu_->setItemChecked( cli_panel_id_, false );
  }
}



void QSP_application::list_managers() {
  std::cerr << std::endl << std::endl 
	    << "-----------------------------------" << std::endl;
  Root::instance()->list_all( std::cerr );
  std::cerr << std::endl;
}



void QSP_application::show_prop_val() {
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( gridModels_manager );
  Manager* mng = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( mng );

  Manager::interface_iterator begin = mng->begin_interfaces();
  Manager::interface_iterator end = mng->end_interfaces();
  for( ; begin != end ; ++begin ) {
    Geostat_grid* grid = dynamic_cast<Geostat_grid*>(  begin->raw_ptr() );
    appli_assert( grid != 0 );
    Geostat_grid::iterator it = grid->begin();
    Geostat_grid::iterator grid_end = grid->end();
    for( ; it != grid_end ; ++it ){
      std::cerr << it->property_value() << std::endl;
    }
  }
  
}


void QSP_application::save_scenegraph() {
  default_3dview_->save_scenegraph_to_file( "scene.iv" );
}



void QSP_application::show_histogram_dialog() {
  Histogram_gui* histogram_dialog = new Histogram_gui( project_,
                                                       0, "histogram_gui" );
  histogram_dialog->show();
}   

void QSP_application::show_qpplot_dialog() {
  QPplot_gui* dialog = 
      new QPplot_gui( project_,
                      0, "Bivariate_analysis_gui" );
  dialog->show();
}  

void QSP_application::show_scatterplot_dialog() {
  Scatterplot_gui* dialog = 
      new Scatterplot_gui( project_,
                           0, "Bivariate_analysis_gui" );
  dialog->show();
} 



void QSP_application::delete_geostat_objects() {
  Multichoice_dialog* dialog = new Multichoice_dialog( this, "Delete Dialog" );
  dialog->setCaption( "Delete Objects" );
  dialog->set_okbutton_caption( "&Delete" );  
  
  const GsTL_project::String_list& grids = project_->objects_list();
  typedef GsTL_project::String_list::const_iterator const_iterator;
  for( const_iterator it = grids.begin(); it != grids.end(); ++it ) {
    dialog->add_choice_item( QString( it->c_str() ) );
  }
  if( dialog->exec() == QDialog::Rejected ) return;

  QStringList names = dialog->selected_items();
  delete_geostat_objects( names );
}


void QSP_application::delete_geostat_objects( const QStringList& names ) {
/*  SmartPtr<Named_interface> ni_dir =
    Root::instance()->interface( gridModels_manager );
  Manager* mng = dynamic_cast<Manager*>( ni_dir.raw_ptr() );
  appli_assert( mng );
  
  for( QStringList::const_iterator it = names.begin(); it != names.end(); ++it ) {
    std::string obj_name( (*it).latin1() );

    mng->delete_interface( "/" + obj_name );
    project_->deleted_object( obj_name );
  }
*/

  if( names.isEmpty() ) return;

  QApplication::setOverrideCursor( waitCursor );

  QString sep = Actions::separator.c_str();
  std::string parameters = std::string( names.join( sep ).latin1() );
  if( parameters.empty() ) return;

  // call the DeleteObjectProperties action
  Error_messages_handler error_messages;

  std::string command( "DeleteObjects" );
  bool ok = project_->execute( command, parameters, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
  }
  
  QApplication::restoreOverrideCursor();
}



void QSP_application::delete_object_properties() {
  Delete_properties_dialog* dialog = 
    new Delete_properties_dialog( project_, this, "Delete Dialog" );
  dialog->setCaption( "Delete Object Properties" );
  dialog->set_okbutton_caption( "&Delete" );  
  if( dialog->exec() == QDialog::Rejected ) return;

  QStringList names = dialog->selected_items();
  QString grid_name = dialog->selected_grid();
  delete_object_properties( grid_name, names );
}



void QSP_application::delete_object_properties( const QString& qgrid_name,
                                                const QStringList& prop_names ) {
  if( qgrid_name.isEmpty() || prop_names.empty() ) return;
/*
  std::string grid_name( qgrid_name.latin1() );
  
  // get a pointer to the grid
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + grid_name );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  appli_assert( grid );
  
  QStringList::const_iterator it = prop_names.begin(); 
  for( ; it != prop_names.end(); ++it ) {
    std::string prop_name( (*it).latin1() );
    grid->remove_property( prop_name );
  }

  project_->update();
*/

  QApplication::setOverrideCursor( waitCursor );

  QString sep = Actions::separator.c_str();
  QStringList list( prop_names );
  list.prepend( qgrid_name );
  std::string parameters = std::string( list.join( sep ).latin1() );
  if( parameters.empty() ) return;

  // call the DeleteObjectProperties action
  Error_messages_handler error_messages;

  std::string command( "DeleteObjectProperties" );
  bool ok = project_->execute( command, parameters, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
  }
  
  QApplication::restoreOverrideCursor();
}


void QSP_application::copy_property() {
  Copy_property_dialog* dialog = 
    new Copy_property_dialog( project_, this, "Copy Properties" );
  dialog->setCaption( "Copy property" );
/*
  if( dialog->exec() == QDialog::Rejected ) return;

  std::string parameters = dialog->parameters();
  Error_messages_handler error_messages;

  std::string command( "CopyProperty" );
  bool ok = project_->execute( command, parameters, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
  }
*/  
  dialog->show();
}





void QSP_application::show_script_editor() {
  Script_editor* editor = new Script_editor( project_ );
  editor->show();
}

void QSP_application::run_script() {
  QString filename = 
    QFileDialog::getOpenFileName( QString::null, "Python scripts(*.py);;All (*)", 
                                  this, "script dialog",
                                  "Select script file" );
  if( filename.isNull() ) return;

  // redirect stdout and stderr to sgems output zones
  PyRun_SimpleString(""
   "import redirect\n"
   "class CoutLogger:\n"
   "    def __init__(self):\n"
   "        self.buf = []\n"
   "    def write(self, data):\n"
   "        self.buf.append(data)\n"
   "        if data.endswith('\\n'):\n"
   "            redirect.sgems_cout(''.join(self.buf))\n"
   "            self.buf = []\n"
   "\n"
   "class CerrLogger:\n"
   "    def __init__(self):\n"
   "        self.buf = []\n"
   "    def write(self, data):\n"
   "        self.buf.append(data)\n"
   "        if data.endswith('\\n'):\n"
   "            redirect.sgems_cerr(''.join(self.buf))\n"
   "            self.buf = []\n"
   "\n"
   "import sys\n"
   "sys.stdout = CoutLogger()\n"
   "sys.stderr = CerrLogger()\n"
   "");
  
  FILE* fp = fopen( filename.ascii(), "r" );
  if( !fp ) {
    GsTLcerr << "can't open file " << filename.ascii() << gstlIO::end;
    return;
  }

  PyRun_SimpleFile( fp, (char*) filename.ascii() );
  fclose( fp );
}






QString generate_build_number() {
//  QString date_str = QString( "Sat " ) + QString( __DATE__ );
//  QDate date = QDate::fromString( date_str );
//  return date.toString( "yyyyMMdd" );
  return QString( __DATE__ );
}

void QSP_application::about_slot() {
  QPixmap pixmap( "new_splash2.png" );
  QDialog* about_screen = new QDialog( this, 0, true );
       
  std::ostringstream version;
  version << "v" << GEMS_VERSION_STR << "\nBuild " 
          << generate_build_number().latin1();

  about_screen->setFont( QFont("Times", 10, QFont::Bold) );
  QPainter painter( &pixmap, about_screen );
  painter.setPen( black );
  QRect r = rect();
  r.setRect( r.x() + 350, r.y() +20, r.width() - 20, r.height() - 20 );
//  painter.drawText( r, Qt::AlignLeft, "V 1.0\nBuild: 20040410");
  painter.drawText( r, Qt::AlignLeft, version.str().c_str() );

  about_screen->setErasePixmap( pixmap );
  about_screen->resize( pixmap.size() );
  about_screen->setMinimumSize( about_screen->size() );
  about_screen->setMaximumSize( about_screen->size() );

  QVBoxLayout* vlayout = new QVBoxLayout( about_screen );
  vlayout->addStretch();

  QHBoxLayout* button_layout = new QHBoxLayout( about_screen );
  QPushButton* close = new QPushButton( "&Close", about_screen, "close" );
  close->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  button_layout->addStretch();
  button_layout->addWidget( close );
  button_layout->addStretch();
 
  vlayout->addLayout( button_layout );
  vlayout->addSpacing( 12 );
  
  QObject::connect( close, SIGNAL( clicked() ),
                    about_screen, SLOT( reject() ) );
  about_screen->exec();
}


void QSP_application::quit_slot() {
  if( project_->has_changed() ) {
    switch( QMessageBox::information( this, "S-GeMS", 
                          "The current project has been modified since last saved\n" 
                          "Do you want to save the changes before exiting?",
                          "&Save", "&Don't Save", "Cancel", 0, 2 ) ) {
    case 0:
      //save the changes
      save_project();
      break;

    case 1:
      // don't save, and exit
      break;

    case 2:
      // don't exit
      return;
    }
  }

  save_app_preferences();
  qApp->quit();
}


void QSP_application::save_app_preferences() {
  QSettings settings;
  settings.setPath( "scrf.stanford.edu", "sgems" );

  // geometry
  int h = this->height();
  int w = this->width();
  settings.writeEntry( "/geometry/height", h );
  settings.writeEntry( "/geometry/width", w );

  // which panels are on?
  bool show_algo = dock_controls_->isVisible();
  bool show_cli = dock_cli_->isVisible();
  settings.writeEntry("/panels/algo", show_algo );
  settings.writeEntry("/panels/cli", show_cli );
}


void QSP_application::show_variogram_analyser() {
  Variogram_modeler_gui* p = new Variogram_modeler_gui( project_, 0 );
  p->show();
}




void QSP_application::new_camera() {
  // create a new view of the project 
//  std::string view_name( "camera" );
//  view_name.append( String_Op::to_string( additional_views_.size() + 1 ) );
  SmartPtr<Named_interface> view_ni = 
    Root::instance()->new_interface( "oinv_view", 
                                     projectViews_manager + "/"  );
  Oinv_view* new_view = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  appli_assert( new_view );
  new_view->initialize( project_, 0 );
  new_view->unref_no_delete();
//  additional_views_.push_back( new_view );

  
  const int offset = 30;
  int x = this->x() + offset;
  int y = this->y() + offset;
  new_view->setGeometry( x, y, 
                         default_3dview_->width(), default_3dview_->height() );

  new_view->show();
}




void QSP_application::closeEvent( QCloseEvent* e ) {
  quit_slot();
  e->ignore();
}




//==========================================================

Save_grid_dialog::Save_grid_dialog( const GsTL_project* project,
				    QWidget * parent, const char * name) 
  : QFileDialog( parent, name, true ) {
  
  setMode( QFileDialog::AnyFile );

  grid_selector_ = new QComboBox( this, "grid_selector" );
  QLabel* label = new QLabel( "Grid to save", this );
  addWidgets( label, grid_selector_, 0 );

  //TL modified
  _propList = new QListBox( this, "prop_selector" );
  _propList->setSelectionMode(QListBox::Extended);
  QLabel* plabel = new QLabel( "Properties to save", this );
  addWidgets( plabel, _propList, 0 );

  //TL modified
  _saveRegular = new QCheckBox(this, "save_regular");
  _saveRegular->setDisabled(true);
  QLabel* slabel = new QLabel( "Save masked grid as Cartesian", this );
  addWidgets( slabel, _saveRegular, 0 );

  // search for available output filters

  QStringList filters;
  SmartPtr<Named_interface> ni_filter = 
    Root::instance()->interface( outfilters_manager );
  Manager* dir = dynamic_cast<Manager*>( ni_filter.raw_ptr() );
  appli_assert( dir );

  Manager::type_iterator begin = dir->begin();
  Manager::type_iterator end = dir->end();
  for( ; begin != end ; ++begin ) {
    QString filt( begin->c_str() );
    filt += " (*.*)";
    filters.push_back( filt ) ;
  }

  setFilters( filters );

  
  // search for available grids

  const GsTL_project::String_list& grids = project->objects_list();
  typedef GsTL_project::String_list::const_iterator const_iterator;
  for( const_iterator it = grids.begin(); it != grids.end(); ++it ) {
    grid_selector_->insertItem( it->c_str() );
  }

  QObject::connect( grid_selector_, SIGNAL( activated(const QString &) ),
	  this, SLOT( gridChanged(const QString &) ) );

  if (!grid_selector_->currentText().isEmpty())
	gridChanged(grid_selector_->currentText());
}


void Save_grid_dialog::gridChanged(const QString & s)
{
	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + s.ascii() );
	Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
	appli_assert( grid );

    std::list<std::string> pn = grid->property_list();
	_propList->clear();
	for (std::list<std::string>::iterator it = pn.begin(); it != pn.end(); ++it){
		_propList->insertItem(*it);
		_propList->setSelected(_propList->findItem(*it),true);
	}

	if (dynamic_cast<Reduced_grid*>(grid))
		_saveRegular->setDisabled(false);
	else
		_saveRegular->setDisabled(true);
}

QString Save_grid_dialog::selected_grid() const {
  return grid_selector_->currentText();
}


void Save_grid_dialog::accept() { 

  _selected.clear();
  for (int i = 0; i < _propList->count(); ++i)
	  if (_propList->isSelected(i))
		  _selected.push_back(_propList->text(i));

  if( grid_selector_->currentText().isEmpty() ) {
    QMessageBox::warning( this, "No Grid Selected",
			  " You must specify a grid to save" );
	done(Rejected);
  }
  if (_selected.empty())
    QMessageBox::warning( this, "No Properties Selected",
			  " WARNING! Saving empty grid" );
  
  done(Accepted);
}



//=========================================


Copy_property_dialog::
Copy_property_dialog( GsTL_project* proj,
                     QWidget* parent, const char* name ) 
                     : QDialog( parent, name ) {
  project_ = proj;

  QVBoxLayout* main_layout = new QVBoxLayout( this, 9, -1, "mainlayout" );
  QGroupBox* source_box = new QGroupBox( this, "source_box" );
  QGroupBox* dest_box = new QGroupBox( this, "dest_box" );
  QGroupBox* options_box = new QGroupBox( this, "option_box" );

  source_box->setColumnLayout( 4, Qt::Vertical );
  dest_box->setColumnLayout( 4, Qt::Vertical );
  options_box->setColumnLayout( 2, Qt::Vertical );

  new QLabel( "Copy property from object:", source_box );
  source_ = new GridSelector( source_box, "source", proj );
  new QLabel( "Select property to copy:", source_box );
  source_property_ = new SinglePropertySelector( source_box, "source_prop" );

  new QLabel( "Copy to object:", dest_box );
  destination_ = new GridSelector( dest_box, "dest", proj );
  new QLabel( "Select or type-in a destination\nproperty name", dest_box );
  destination_property_ = new SinglePropertySelector( dest_box, "dest_prop" );
//  destination_property_ = new QComboBox( dest_box, "dest_prop" );
  destination_property_->setEditable( true );

  overwrite_ = new QCheckBox( "Overwrite", options_box );
  mark_as_hard_ = new QCheckBox( "Mark as Hard Data", options_box );
  
  QHBoxLayout* bottom_layout = new QHBoxLayout( this, 9 );
  QPushButton* ok = new QPushButton( "Copy", this, "ok" );  
  QPushButton* cancel = new QPushButton( "Close", this, "ok" );
  bottom_layout->addStretch();
  bottom_layout->addWidget( ok );
  bottom_layout->addWidget( cancel );

  main_layout->addWidget( source_box );
  main_layout->addStretch();
  main_layout->addWidget( dest_box );
  main_layout->addStretch();
  main_layout->addWidget( options_box );
  main_layout->addStretch();
  main_layout->addLayout( bottom_layout );

  
  QObject::connect( source_, SIGNAL( activated( const QString& ) ),
                    source_property_, SLOT( show_properties( const QString& ) ) );
  QObject::connect( destination_, SIGNAL( activated( const QString& ) ),
                    destination_property_, SLOT( show_properties( const QString& ) ) );
/*
  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( accept() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );
*/
  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( copy_property() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( accept() ) );

  setSizeGripEnabled( true );
}


std::string Copy_property_dialog::parameters() const {
  QString overwrite_str;
  overwrite_str.setNum( overwrite_->isChecked() );
  QString mark_hard_str;
  mark_hard_str.setNum( mark_as_hard_->isChecked() );

  QString sep = Actions::separator.c_str();
  QStringList list;
  list << source_->currentText()
       << source_property_->currentText() 
       << destination_->currentText() 
       << destination_property_->currentText() 
       << overwrite_str << mark_hard_str;
  
  QString res = list.join( sep );
  if( res.isEmpty() ) return std::string( "" );

  return std::string( res.latin1() );
}

void Copy_property_dialog::copy_property() {
  if( !project_ ) return;

  QString overwrite_str;
  overwrite_str.setNum( overwrite_->isChecked() );
  QString mark_hard_str;
  mark_hard_str.setNum( mark_as_hard_->isChecked() );

  QString sep = Actions::separator.c_str();
  QStringList list;
  list << source_->currentText()
       << source_property_->currentText() 
       << destination_->currentText() 
       << destination_property_->currentText() 
       << overwrite_str << mark_hard_str;
  
  std::string parameters = std::string( list.join( sep ).ascii() );
  if( parameters.empty() ) return;


  QApplication::setOverrideCursor( waitCursor );
 
  // call the CopyProperty action
  Error_messages_handler error_messages;

  std::string command( "CopyProperty" );
  bool ok = project_->execute( command, parameters, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
  }

  QApplication::restoreOverrideCursor();
}


QString Copy_property_dialog::source() const { 
  return source_->currentText(); 
}
QString Copy_property_dialog::source_property() const { 
  return source_property_->currentText();
}
QString Copy_property_dialog::destination() const { 
  return destination_->currentText(); 
}
QString Copy_property_dialog::destination_property() const { 
  return destination_property_->currentText(); 
}
bool Copy_property_dialog::overwrite() const { 
  return overwrite_->isChecked(); 
}
bool Copy_property_dialog::mark_as_hard() const { 
  return mark_as_hard_->isChecked(); 
}
