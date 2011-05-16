/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "actions" module of the Geostatistical Earth
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

#include <GsTLAppli/actions/library_actions_init.h>
#include <GsTLAppli/actions/python_commands.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/actions/obj_manag_actions.h>
#include <GsTLAppli/actions/algorithms_actions.h>
#include <GsTLAppli/actions/misc_actions.h>
#include <GsTLAppli/utils/gstl_messages.h>


void init_python_interpreter();



int library_actions_init::references_ = 0;

int library_actions_init::init_lib() {

  // initialize Python
  init_python_interpreter();


  references_++;
  if( references_ != 1 ) {
    GsTLlog << "actions library already registered \n" ;
    return 2;
  }
     
  GsTLlog << "\n\n registering actions library \n" ; 
  
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory",
				    actions_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << actions_manager << "\n";
    return 1;
  }
  
  bind_action_factories( dir );

  GsTLlog << "Registration done \n\n" ;

  Root::instance()->list_all( GsTLlog );
  return 0;
}


int library_actions_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( actions_manager );
    Py_Finalize();
  }
  return 0;
}


bool library_actions_init::bind_action_factories(Manager* dir) {
  // Grid management actions
  dir->factory( "Help", Help_action::create_new_interface );

  dir->factory( "NewRGrid", New_rgrid::create_new_interface );

  // TL modified
  /*
  dir->factory( "LoadSimFromFile", 
		Load_sim::create_new_interface );
		*/
  dir->factory( "RunScript", RunScript::create_new_interface );

  dir->factory( "LoadObjectFromFile", 
		Load_object_from_file::create_new_interface );
  dir->factory( "SaveGeostatGrid", Save_geostat_grid::create_new_interface );
  dir->factory( "NewCartesianGrid", 
		New_cartesian_grid_action::create_new_interface );
  dir->factory( "LoadProject", 
		Load_project::create_new_interface );
  dir->factory( "CopyProperty", 
		Copy_property::create_new_interface );
  dir->factory( "SwapPropertyToDisk", 
		Swap_property_to_disk::create_new_interface );
  dir->factory( "SwapPropertyToRAM", 
		Swap_property_to_ram::create_new_interface );
  dir->factory( "DeleteObjects", 
    Delete_objects::create_new_interface );
  dir->factory( "DeleteObjectProperties", 
    Delete_properties::create_new_interface );
  dir->factory( "ClearPropertyValueIf", 
    Clear_property_value_if::create_new_interface );

  
  
  // algorithm related actions
  dir->factory( "RunGeostatAlgorithm", Run_geostat_algo::create_new_interface );

  return true;
}



extern "C" {
  int libGsTLAppli_actions_init() {
    return library_actions_init::init_lib();
  }
  int libGsTLAppli_actions_release(){
    return library_actions_init::release_lib();
  }
}



//--------------------------------------

void init_python_interpreter() {
  Py_Initialize();
  Py_InitModule("sgems", SGemsMethods);
  Py_InitModule("redirect", RedirectMethods); 
}
