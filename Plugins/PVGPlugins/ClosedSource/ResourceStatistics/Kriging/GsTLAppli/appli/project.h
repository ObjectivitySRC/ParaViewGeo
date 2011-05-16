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

#ifndef __GSTLAPPLI_GUI_APP_PROJECT_H__ 
#define __GSTLAPPLI_GUI_APP_PROJECT_H__ 
 
#include <GsTLAppli/appli/common.h>
#include <GsTLAppli/utils/named_interface.h> 
 
#include <list> 
#include <string> 
 
class Error_messages_handler; 
class GsTL_project; 
  
 
/** Base class for all views of a "project". 
 * A project view is an object whose aspect depends on the containts of a 
 * "project". It can be a 3D visualisation of the project, or a widget that 
 * shows a list of all objects in the project and should be updated when the 
 * project changes (e.g. an object is added or deleted). 
 *  
 * A project view is notified when the project changes. 
 */ 
 
class APPLI_DECL Project_view : public Named_interface { 
 
 public: 
 
  Project_view() ; 
  Project_view( GsTL_project* project ); 
  virtual ~Project_view(); 
 
  virtual void init( GsTL_project* project ); 
 
  virtual void update( std::string obj ) = 0; 
  virtual void new_object( std::string obj ) = 0; 
  virtual void deleted_object( std::string obj ) = 0; 
 
 protected: 
  GsTL_project* project_; 
}; 
 
 
 
 
/** A project, or document. It is a collection of different  
 * (grid-) objects, e.g. one stratigraphic grid and two surfaces, 
 * or a single cartesian grid. 
 */ 
 
class APPLI_DECL GsTL_project : public Named_interface { 
 public:
  static Named_interface* create_new_interface( std::string& );
  
 public: 
  typedef std::list< std::string > String_list; 
 
  GsTL_project(); 
  GsTL_project( const std::list< std::string>& objects ); 
  ~GsTL_project(); 
     
  const String_list& objects_list() const { return grid_objects_list_; } 
  // String_list objects_list() const { return grid_objects_list_; } 
 
  void add_view( const std::string& view_type ); 
  void add_view( Project_view* view ); 
  void remove_view( Project_view* view ); 
 
  bool execute( std::string action, std::string param, 
                Error_messages_handler* errors = 0 ); 
  void update( std::string obj = "" ); 
  void new_object( std::string obj ); 
  void deleted_object( std::string obj ); 
  int size() const { return views_list_.size(); } 
 
  bool has_changed() const { return project_modified_; }
  void reset_change_monitor() { project_modified_ = false; }

  bool is_empty() const { return grid_objects_list_.empty(); }
  void clear();

  void name( const std::string& name );
  std::string name() const { return project_name_; }


 private: 
//  typedef std::list< SmartPtr<Project_view> >::iterator iterator; 
//  std::list< SmartPtr<Project_view> > views_list_; 
  typedef std::list< Project_view* >::iterator iterator; 
  std::list< Project_view* > views_list_; 
  std::list< std::string > grid_objects_list_; 
 
  bool project_modified_;

  std::string project_name_;
}; 
 
 
Named_interface* Create_gstl_project( std::string& ); 
 
 
#endif 
