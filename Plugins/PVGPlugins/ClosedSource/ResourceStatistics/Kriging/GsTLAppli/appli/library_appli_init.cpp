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

#include <GsTLAppli/appli/library_appli_init.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/utils/gstl_messages.h>


int library_appli_init::references_ = 0;


int library_appli_init::init_lib() {

//  std::cout << "number of subscriber to GsTLlog: "  
//    << GsTLlog.subscribers() << "  second opinion: " 
//    << GsTLAppli_logging::instance()->subscribers() << std::endl;
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "appli library already registered \n";
    return 2;
  }
     
  GsTLlog << "\n\n registering appli library \n"; 
  
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory",
				    projects_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << projects_manager << "\n";
    return 1;
  }
  
  bind_appli_factories( dir );
  GsTLlog << "Registration done\n\n" ;

  Root::instance()->list_all( GsTLlog );
  return 0;
}


int library_appli_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( projects_manager );
  }
  return 0;
}


bool library_appli_init::bind_appli_factories(Manager* dir) {
  dir->factory( "project", Create_gstl_project );
  return true;
}



extern "C" {
  int libGsTLAppli_appli_init() {
    return library_appli_init::init_lib();
  }
  int libGsTLAppli_appli_release(){
    return library_appli_init::release_lib();
  }
}
