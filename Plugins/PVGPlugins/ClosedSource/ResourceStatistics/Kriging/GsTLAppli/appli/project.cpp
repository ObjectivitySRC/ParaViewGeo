/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "appli" module of the Geostatistical Earth
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

#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/clock.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/actions/action.h>

#include <algorithm>



Project_view::Project_view() 
  : project_(0) {
}

Project_view::Project_view( GsTL_project* project ) 
  : project_(project) {
}

Project_view::~Project_view() {
  appli_warning( "destroying Project_view...    still " << this->references()
		<< " references" );

  if( project_ ) {
    project_->remove_view( this );

    appli_warning( "size after removing view: " << project_->size() << std::endl
		  << "nb of references on project_view: "
		  << this->references() );
  }
}

void Project_view::init( GsTL_project* project ) { 
  project_ = project;
  if( project_ )
    project_->add_view( this );
} 




//===========================================


Named_interface* Create_gstl_project( std::string& ) {
  return new GsTL_project;
}
Named_interface* GsTL_project::create_new_interface( std::string& ) {
  return new GsTL_project;
}


/** A project, or document. It is a collection of different 
 * (grid-) objects, e.g. one stratigraphic grid and two surfaces,
 * or a single cartesian grid.
 * Example of interaction between the project and project_views:
 *  - we load 2 surfaces and build one cartesian grid.
 *  - The project now has a record of these 3 objects.
 *  - we ask for a new view of the project. In each view, we can
 *  display different elements of the project (one view could currently
 *  display all the elements, while the other one could be used to view
 *  only the grid. 
 *  - when a modification is made to the project (e.g. a new element is
 *  added, or one element is modified), the project notifies all its
 *  views, and the views update themselves.
 */

GsTL_project::GsTL_project() {
  project_name_ = "";
  project_modified_ = false;
}

GsTL_project::GsTL_project( const std::list< std::string>& objects )
  : grid_objects_list_( objects ) {
  project_name_ = "";
  project_modified_ = false;
}


GsTL_project::~GsTL_project() {

}


bool GsTL_project::execute( std::string action, std::string param,
                            Error_messages_handler* errors ) {
  if( action.empty() ) return false;

  SmartPtr<Named_interface> ni = 
       Root::instance()->new_interface( action, actions_manager + "/" );

  Action* act = dynamic_cast<Action*>( ni.raw_ptr() );
  if( !act ) {
    appli_warning( action << ":  no such action " );
    return false;
  }

  // before logging the action, we replace all newline characters
  // by a "\". This is because a script is interpreted line by line,
  // and to make it easy to copy from the logs, we write to the logs with the 
  // correct syntax (ie use "\" to say that the line is not over)
  String_Op::replace( param, "\n", "  " );
  GsTLlog << action << "  " << param << gstlIO::end;

  // if no error handler was provided, just create a temporary one that
  // we will discard later.
  bool tmp_handler = false;
  if( !errors ) {
    tmp_handler = true;
    errors = new Error_messages_handler;
  }


  Qt_clock clock;
  clock.start();



  // initialize the action. Check if not successful, return
  if( ! act->init( param, this, errors ) )
    return false;
  appli_message( action << " took " << clock.elapsed() << "ms to initialize" );

  // run
  clock.restart();
  bool ok = act->exec();
  appli_message( action << " took " << clock.elapsed() << "ms to run" );
  GsTLcout << "completing " << action << " took " << clock.elapsed() 
           << "ms" << gstlIO::end;

  if( tmp_handler ) delete errors;

  return ok;
}



void GsTL_project::update( std::string obj ) {
  project_modified_ = true;

  iterator it = views_list_.begin();
  
  for( ; it != views_list_.end() ; ++it )
    (*it)->update( obj );
}

void GsTL_project::new_object( std::string obj ) {
  project_modified_ = true;

  grid_objects_list_.push_back( obj );

  iterator it = views_list_.begin();
  
  for( ; it != views_list_.end() ; ++it )
    (*it)->new_object( obj );
}


void GsTL_project::deleted_object( std::string obj ) {

  // check if the object actually existed 
  std::list<std::string>::iterator found =
    std::find( grid_objects_list_.begin(), grid_objects_list_.end(), obj );
  if( found == grid_objects_list_.end() ) 
    return;


  project_modified_ = true;

  //grid_objects_list_.remove( obj );
  grid_objects_list_.erase( found );

  iterator it = views_list_.begin();  
  for( ; it != views_list_.end() ; ++it )
    (*it)->deleted_object( obj );
}


void GsTL_project::add_view( const std::string& view_type ) {
  // go to manager of project-views and get a new view

  // get project name
  SmartPtr<Named_interface> dir_ni =
      Root::instance()->interface( projects_manager );
  Manager* dir = dynamic_cast<Manager*>( dir_ni.raw_ptr() );
  appli_assert( dir );
  std::string proj_name = dir->name( (Named_interface*) this );

  // name of the new view: proj_view_2 where "proj" is the name of the project,
  // and suffix 2 is deduced from the number of views currently registered
  std::string view_name( projectViews_manager + "/" + proj_name + "_view_" );
  view_name += String_Op::to_string( views_list_.size() );

  SmartPtr<Named_interface> ni = 
      Root::instance()->new_interface( view_type, view_name );

  Project_view* view = dynamic_cast<Project_view*>( ni.raw_ptr() ); 
  appli_assert( view );
  view->init( this );

  views_list_.push_back( view );
}


void GsTL_project::add_view( Project_view* view ) {
  // views_list_.push_back( SmartPtr<Project_view>( view ) );
  views_list_.push_back( view );
}


void GsTL_project::remove_view( Project_view* view ) {
  appli_warning( "removing view...    the view has " << view->references()
		<< " references" );

  iterator pos = std::find( views_list_.begin(), views_list_.end(), view );
  if( pos == views_list_.end() ) return;

  views_list_.erase( pos );

  appli_warning( "the view has been erased. nb of references is now " 
		<< view->references() );
}


// The following version of remove_view assumes the view is managed by a 
// Manager.
/*
void GsTL_project::remove_view( Project_view* view ) {
  // go to manager of project-views and remove view
  iterator pos = std::find( views_list_.begin(), views_list_.end(), view );
  if( pos == views_list_.end() ) return;
  
  SmartPtr<Named_interface> ni = 
      Root::instance()->interface( projectViews_manager ); 
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( dir );
  dir->delete_interface( dir->name( (Named_interface*) view ) );

  views_list_.erase( pos );
}
*/


void GsTL_project::clear() {
  SmartPtr<Named_interface> ni_dir =
    Root::instance()->interface( gridModels_manager );
  Manager* mng = dynamic_cast<Manager*>( ni_dir.raw_ptr() );
  appli_assert( mng );
 
  std::vector< std::string > tmp_copy( grid_objects_list_.size() );
  std::copy( grid_objects_list_.begin(), grid_objects_list_.end(),
             tmp_copy.begin() );
  for( std::vector< std::string >::iterator it = tmp_copy.begin();
       it != tmp_copy.end(); ++it ) { 
    mng->delete_interface( "/" + (*it) );
    this->deleted_object( *it );
  }

  project_modified_ = true;
  project_name_ = "";

}


void GsTL_project::name( const std::string& name ) {
  project_name_ = name;
}
