/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
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

#include <GsTLAppli/grid/library_grid_init.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/property_copier.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>


int library_grid_init::references_ = 0;

int library_grid_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "grid library already registered" << "\n";
    return 2;
  }
      
  GsTLlog << "registering grid library" << "\n"; 
    
  /* Set-up directory structure: /GridObject, /GridObject/Model and
   * /GridObject/OinvDescription
   */
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory://grids",
				    gridObject_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << gridObject_manager << "\n";
    return 1;
  }
    
  dir->factory("directory", create_directory);

  SmartPtr<Named_interface> ni_models = 
    Root::instance()->new_interface("directory",
				    gridModels_manager);
    

  /* Bind the factory methods for grid models
   */
  dir = dynamic_cast<Manager*>( ni_models.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory" 
	      << gridModels_manager << "\n";
    return 1;
  }
      
  bind_models_factories( dir );


  /* Bind the factory methods for grid descriptions
   */
  /*
  dir = dynamic_cast<Manager*>( ni_desc.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory" 
	      << oinvDescription_manager << "\n";
    return 1;
  }     
 
  bind_desc_factories( dir );
  */

  init_property_copier_factory();

  GsTLlog << "Registration done" << "\n";
  Root::instance()->list_all( GsTLlog );

  return 0;
}

  
//------------------------


int library_grid_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( gridObject_manager );
    Root::instance()->delete_interface( gridModels_manager );
    Root::instance()->delete_interface( oinvDescription_manager );
  }
  return 0;
}


//------------------------

bool library_grid_init::bind_models_factories(Manager* dir) {
  dir->factory( "cgrid", create_Cgrid );
  dir->factory( "rgrid", create_Rgrid );
  dir->factory( "point_set", Point_set::create_new_interface );

  //TL modified
  dir->factory("reduced_grid", create_reduced_grid);
  return true;
}



//------------------------
bool library_grid_init::init_property_copier_factory() {
  Property_copier_factory::add_method( Point_set().classname(), 
                                       Cartesian_grid().classname(), 
                                       Cgrid_to_pset_copier::create_new_interface );
  Property_copier_factory::add_method( Cartesian_grid().classname(), 
                                       Cartesian_grid().classname(), 
                                       Cgrid_to_cgrid_copier::create_new_interface );
  Property_copier_factory::add_method( Point_set().classname(), 
                                       Point_set().classname(), 
                                       Pset_to_pset_copier::create_new_interface );

  //TL modified
  Property_copier_factory::add_method( Reduced_grid().classname(), 
                                       Reduced_grid().classname(), 
                                       Mask_to_mask_copier::create_new_interface );
  Property_copier_factory::add_method( Point_set().classname(), 
                                       Reduced_grid().classname(), 
                                       Pset_to_mask_copier::create_new_interface );


  return true;
}




extern "C" {
  int libGsTLAppli_grid_init() {
    return library_grid_init::init_lib();
  }
  int libGsTLAppli_grid_release() {
    return library_grid_init::release_lib();
  }
}

