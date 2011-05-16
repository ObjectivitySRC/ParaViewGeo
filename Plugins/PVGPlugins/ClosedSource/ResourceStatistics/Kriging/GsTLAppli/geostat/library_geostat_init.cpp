/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
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

#include <GsTLAppli/geostat/library_geostat_init.h>
#include <GsTLAppli/geostat/parameters_handler_impl.h>

#include <GsTLAppli/geostat/kriging.h>
#include <GsTLAppli/geostat/indicator_kriging.h>
#include <GsTLAppli/geostat/cosisim.h>
#include <GsTLAppli/geostat/cokriging.h>
#include <GsTLAppli/geostat/sgsim.h>
#include <GsTLAppli/geostat/sisim.h>
#include <GsTLAppli/geostat/cosgsim.h>
#include <GsTLAppli/geostat/snesim_std/snesim_std.h>
#include <GsTLAppli/geostat/PostKriging.h>
#include <GsTLAppli/geostat/Postsim.h>
#include <GsTLAppli/geostat/trans.h>
#include <GsTLAppli/geostat/dssim.h>
#include <GsTLAppli/geostat/LU_sim.h>
#include <GsTLAppli/geostat/hmatch.h>
#include <GsTLAppli/geostat/filtersim_std/filtersim_cate.h>
#include <GsTLAppli/geostat/filtersim_std/filtersim_cont.h>
#include <GsTLAppli/geostat/moving_window.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/geostat/transcat.h>
#include <GsTLAppli/geostat/ImageProcess.h>
#include <GsTLAppli/geostat/nuTauModel.h>

int library_geostat_init::references_ = 0;
 

int library_geostat_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "geostat library already registered" << "\n";
    return 2;
  }
     
  GsTLlog << "\n\n registering geostat library" << "\n"; 
  
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory://Geostat",
				     geostatAlgo_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << geostatAlgo_manager << "\n";
    return 1;
  }
  
  bind_geostat_factories( dir );

  // Get or create directory for geostat utilities
  SmartPtr<Named_interface> ni_utils = 
    Root::instance()->interface( geostatParamUtils_manager );
  
  if( ni_utils.raw_ptr() == 0 ) {
    ni_utils = 
      Root::instance()->new_interface("directory",
				      geostatParamUtils_manager);

    dir = dynamic_cast<Manager*>( ni_utils.raw_ptr() );
    if( !dir ) {
      GsTLlog << "could not create directory " 
	      << geostatParamUtils_manager << "\n";
      return 1;
    }

    dir->factory("directory", create_directory);
  }
    
  // Now create subdirectory "xmlGeostatParamUtils_manager" of directory
  // "geostatParamUtils_manager" if it does not already exist
  SmartPtr<Named_interface> ni_xml_utils = 
    Root::instance()->interface( xmlGeostatParamUtils_manager );

  if( ni_xml_utils.raw_ptr() == 0 )
    ni_xml_utils = 
      Root::instance()->new_interface("directory://QTaccessors/xml",
				      xmlGeostatParamUtils_manager );

  dir = dynamic_cast<Manager*>( ni_xml_utils.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not get nor create directory " 
	      << xmlGeostatParamUtils_manager << "\n";
    return 1;
  }
  bind_geostat_utilities( dir );


  GsTLlog << "Registration done" << "\n\n";

  Root::instance()->list_all( GsTLlog );
  return 0;
}


int library_geostat_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( geostatAlgo_manager );
  }
  return 0;
}


bool library_geostat_init::bind_geostat_utilities( Manager* dir ) {
  dir->factory( "Parameters_handler", Parameters_handler_xml::create_new_interface );
  return true;
}
 
bool library_geostat_init::bind_geostat_factories( Manager* dir ) {
  dir->factory( Kriging().name(), Kriging::create_new_interface );
  dir->factory( Sgsim().name(), Sgsim::create_new_interface );
  dir->factory( Indicator_kriging().name(), 
		Indicator_kriging::create_new_interface );
  dir->factory( Cokriging().name(), Cokriging::create_new_interface );
  dir->factory( Sisim().name(), Sisim::create_new_interface );
  dir->factory( Cosgsim().name(), Cosgsim::create_new_interface );
  dir->factory( Cosisim().name(), Cosisim::create_new_interface );
  dir->factory( Snesim_Std().name(), Snesim_Std::create_new_interface );
  dir->factory( Postsim().name(), Postsim::create_new_interface );
  dir->factory( PostKriging().name(), PostKriging::create_new_interface );
  dir->factory( trans().name(), trans::create_new_interface );
  dir->factory( transcat().name(), transcat::create_new_interface );
  dir->factory( ImageProcess().name(), ImageProcess::create_new_interface );
  dir->factory( dssim().name(), dssim::create_new_interface );
  dir->factory( LU_sim().name(), LU_sim::create_new_interface );
  dir->factory( Filtersim_Cate().name(), Filtersim_Cate::create_new_interface );
  dir->factory( Filtersim_Cont().name(), Filtersim_Cont::create_new_interface );
  dir->factory( NuTauModel().name(), NuTauModel::create_new_interface );
  dir->factory( Moving_window().name(), Moving_window::create_new_interface );

  return true;
}


extern "C" {
  int libGsTLAppli_geostat_init() {
    return library_geostat_init::init_lib();
  }
  int libGsTLAppli_geostat_release(){
    return library_geostat_init::release_lib();
  }
}
