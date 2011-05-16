/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Geostatistical Earth
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

#include <GsTLAppli/extra/gui/library_extragui_init.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/extra/gui/qt_accessors.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qtable_accessor.h>

#include <GsTLAppli/extra/qtplugins/filechooser.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/extra/qtplugins/variogram_input.h>
#include <GsTLAppli/extra/qtplugins/kriging_type_selector.h>
#include <GsTLAppli/extra/qtplugins/gstl_table.h>
#include <GsTLAppli/extra/qtplugins/ellipsoid_input.h>
#include <GsTLAppli/extra/qtplugins/non_param_cdf_input.h>

int library_extragui_init::references_ = 0;


int library_extragui_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "extragui library already registered" << "\n";
    return 2;
  }
     
  GsTLlog << "\n\n registering extragui library" << "\n"; 
  
  //---------------------
  // Create the qt_accessors manager

  GsTLlog << "Getting QT_wrappers manager" << "\n";
  SmartPtr<Named_interface> ni_wrapper = 
    Root::instance()->interface( xmlGeostatParamUtils_manager );
  Manager* dir = dynamic_cast<Manager*>( ni_wrapper.raw_ptr() );

  if( !dir ) {
    GsTLlog << "Directory " << xmlGeostatParamUtils_manager << " does not exist" << "\n";
    return 1;
  }

  bind_qt_wrappers_factories( dir );


  GsTLlog << "Registration done" << "\n\n";

  Root::instance()->list_all( GsTLlog );
  return 0;
}



int library_extragui_init::release_lib() {
  references_--;

  return 0;
}



bool library_extragui_init::bind_qt_wrappers_factories(Manager* dir) {
  // GridSelector
  GridSelector selector;
  std::string classname( selector.className() );
  dir->factory(classname, GridSelector_accessor::create_new_interface);
  
  // PropertySelector
  PropertySelector prop;
  classname = std::string( prop.className() );
  dir->factory(classname, PropertySelector_accessor::create_new_interface);

  // SinglePropertySelector
  SinglePropertySelector single_prop_select;
  classname = std::string( single_prop_select.className() );
  dir->factory(classname, SinglePropertySelector_accessor::create_new_interface);

  // MultiPropertySelector
  MultiPropertySelector multi_prop_select;
  classname = std::string( multi_prop_select.className() );
  dir->factory(classname, MultiPropertySelector_accessor::create_new_interface);

  // MultiPropertySelector
  OrderedPropertySelector order_prop_select;
  classname = std::string( order_prop_select.className() );
  dir->factory(classname, OrderedPropertySelector_accessor::create_new_interface);

  // FileChooser
  FileChooser chooser;
  classname = std::string( chooser.className() );
  dir->factory(classname, FileChooser_accessor::create_new_interface);

  // VariogramInput;
  VariogramInput varg_input;
  classname = std::string( varg_input.className() );
  dir->factory( classname, VariogramInput_accessor::create_new_interface);

  // KrigingTypeSelector
  KrigingTypeSelector ktype_selector;
  classname = std::string( ktype_selector.className() );
  dir->factory( classname, KrigingTypeSelector_accessor::create_new_interface);

  // GsTLTable
  GsTLTable gstl_table;
  classname = std::string( gstl_table.className() );
  dir->factory( classname, QTable_accessor::create_new_interface);

  // KrigingTypeSelector
  TrendComponents trend_components;
  classname = std::string( trend_components.className() );
  dir->factory( classname, TrendComponents_accessor::create_new_interface);

  // GsTLTable
  GsTLTable gstltable;
  classname = gstltable.className();
  dir->factory(classname, QTable_accessor::create_new_interface);

  // GsTLTable
  EllipsoidInput ellipsoid_input;
  classname = ellipsoid_input.className();
  dir->factory(classname, EllipsoidInput_accessor::create_new_interface);

  // LowerTailCdfInput
  LowerTailCdfInput lowerTailCdf_input;
  classname = lowerTailCdf_input.className();
  dir->factory(classname, lowerTailCdfInput_accessor::create_new_interface);

  // UpperTailCdfInput
  UpperTailCdfInput upperTailCdf_input;
  classname = upperTailCdf_input.className();
  dir->factory(classname, upperTailCdfInput_accessor::create_new_interface);

  // NonParamCdfInput
  NonParamCdfInput nonParamCdf_input;
  classname = nonParamCdf_input.className();
  dir->factory(classname, nonParamCdfInput_accessor::create_new_interface);

  return true;

}


extern "C" {
  int libGsTLAppli_extragui_init() {
    return library_extragui_init::init_lib();
  }
  int libGsTLAppli_extragui_release(){
    return library_extragui_init::release_lib();
  }
}
