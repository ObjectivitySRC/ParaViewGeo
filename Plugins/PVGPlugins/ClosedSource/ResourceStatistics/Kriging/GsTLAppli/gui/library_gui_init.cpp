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
  
#include <GsTLAppli/gui/library_gui_init.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/appli/oinv_project_view.h>
#include <GsTLAppli/gui/oinv_description/oinv_cgrid.h>
#include <GsTLAppli/gui/oinv_description/oinv_pointset.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qcombobox_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qlineedit_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qradiobutton_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qcheckbox_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qspin_box_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qtable_accessor.h>
#include <GsTLAppli/gui/utils/qwidget_value_collector.h>
#include <GsTLAppli/gui/utils/colorscale.h>
#include <GsTLAppli/gui/appli/visu_actions.h>
#include <GsTLAppli/gui/appli/qt_grid_summary.h>

//TL modified
#include <GsTLAppli/gui/oinv_description/oinv_mgrid.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

int library_gui_init::references_ = 0;


int library_gui_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "gui library already registered" << "\n";
    return 2;
  }
     
  GsTLlog << "\n\n registering gui library" << "\n"; 
  
  //---------------------
  // Create the Project_views manager
  
  GsTLlog << "Creating Project_view manager" << "\n";
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory",
				    projectViews_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << projectViews_manager << "\n";
    return 1;
  }
  
  bind_project_views_factories( dir );
  

  //---------------------
  // Create the qt_accessors manager

  GsTLlog << "Creating QWidget_value_accessors manager" << "\n";
  
  SmartPtr<Named_interface> ni_utils = 
    Root::instance()->interface( geostatParamUtils_manager );

  // If directory "geostatParamUtils_manager" does not already exist, 
  // create it and set it up.
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

  //----------------------
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
  
  dir->factory("Value_collector", 
	       QWidgets_values_collector_xml::create_new_interface);
  bind_qt_accessors_factories( dir );


  //----------------------
  // Create the manager for the colormaps ("color scales" actually)
  GsTLlog << "Creating Colormaps manager" << "\n";
  SmartPtr<Named_interface> ni_cmaps = 
    Root::instance()->new_interface("directory://colormaps",
				    colormap_manager );
      
  dir = dynamic_cast<Manager*>( ni_cmaps.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << colormap_manager << "\n";
    return 1;
  }
  dir->factory( "colormap", Color_scale::create_new_interface );


  //----------------------
  // Create the manager for Open Inventor descriptions
  // We assume that directory /GridObject already exists
  GsTLlog << "Creating Open Inventor descriptions manager" << "\n";
  SmartPtr<Named_interface> ni_desc =
    Root::instance()->new_interface("directory",
                                    oinvDescription_manager);
  
  dir = dynamic_cast<Manager*>( ni_desc.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory"
              << oinvDescription_manager << "\n";
    return 1;
  }
  
  bind_oinv_descriptions( dir );
  

  //----------------------
  // Create the manager for Qt summary descriptions
  // We assume that directory /GridObject already exists
  GsTLlog << "Creating Qt summary descriptions manager" << "\n";
  SmartPtr<Named_interface> ni_qtdesc =
    Root::instance()->new_interface("directory",
                                    QtSummaryDescription_manager);
  
  dir = dynamic_cast<Manager*>( ni_qtdesc.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory"
              << QtSummaryDescription_manager << "\n";
    return 1;
  }
  
  bind_qtsummary_descriptions( dir );
  


  //----------------------
  // Bind the GUI-related actions
  SmartPtr<Named_interface> actions_ni = 
    Root::instance()->interface( actions_manager );
      
  Manager* actions_manager = dynamic_cast<Manager*>( actions_ni.raw_ptr() );
    
  if( !actions_manager ) {
    GsTLlog << "could not find directory " 
	      << actions_manager << "\n";
    return 1;
  }
  
  bind_gui_actions( actions_manager );


  //----------------------
  GsTLlog << "Registration done" << "\n\n";

  Root::instance()->list_all( GsTLlog );
  return 0;
}



int library_gui_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( projectViews_manager );
  }
  return 0;
}



bool library_gui_init::bind_project_views_factories(Manager* dir) {
  dir->factory( "oinv_view", Create_oinv_view );
  return true;
}


bool library_gui_init::bind_qt_accessors_factories(Manager* dir) {
  // ComboBox
  QComboBox combobox;
  std::string classname( combobox.className() );
  dir->factory(classname, QComboBox_accessor::create_new_interface);
      
  // SpinBox
  QSpinBox spinbox;
  classname = spinbox.className();
  dir->factory(classname, QSpinBox_accessor::create_new_interface);

  // RadioButton
  QRadioButton radiobutton(0);
  classname = radiobutton.className();
  dir->factory(classname, QRadioButton_accessor::create_new_interface);

  // CheckBox
  QCheckBox checkbox(0);
  classname = checkbox.className();
  dir->factory(classname, QCheckBox_accessor::create_new_interface);

  // LineEdit
  QLineEdit lineedit(0);
  classname = lineedit.className();
  dir->factory(classname, QLineEdit_accessor::create_new_interface);
  
  // QTable
  QTable table;
  classname = table.className();
  dir->factory(classname, QTable_accessor::create_new_interface);
    
  return true;
}


bool library_gui_init::bind_oinv_descriptions(Manager* dir) {
  Cartesian_grid cg;
  dir->factory( cg.classname(), Oinv_cgrid::create_new_interface );
  Point_set pset;
  dir->factory( pset.classname(), Oinv_pointset::create_new_interface );

  //TL modified
  Reduced_grid rg;
  dir->factory( rg.classname(), Oinv_mgrid::create_new_interface );

  return true;
}

bool library_gui_init::bind_qtsummary_descriptions(Manager* dir) {
  Cartesian_grid cg;
  dir->factory( cg.classname(), Qt_cartesian_grid_summary::create_new_interface );
  Point_set pset;
  dir->factory( pset.classname(), Qt_pointset_summary::create_new_interface );

  //TL modified
  Reduced_grid rg;
  dir->factory( rg.classname(), Qt_masked_grid_summary::create_new_interface );
  return true;
}


bool library_gui_init::bind_gui_actions(Manager* dir) {
  dir->factory( "RotateCamera", Rotate_camera::create_new_interface );
  dir->factory( "ShowHistogram", Show_histogram::create_new_interface );
  dir->factory( "SaveHistogram", Save_histogram::create_new_interface );
  dir->factory( "SaveQQplot", Save_qqplot::create_new_interface );
  dir->factory( "SaveScatterplot", Save_scatterplot::create_new_interface );
  dir->factory( "DisplayObject", Display_object::create_new_interface );
  dir->factory( "HideObject", Hide_object::create_new_interface );
  dir->factory( "TakeSnapshot", Snapshot_action::create_new_interface );
  dir->factory( "ResizeCameraWindow", Resize_camera_window::create_new_interface );
  dir->factory( "SaveCameraSettings", 
    Save_camera_settings_to_file::create_new_interface );
  dir->factory( "LoadCameraSettings", 
    Load_camera_settings::create_new_interface );
  dir->factory( "SetColormap", 
    Choose_colormap::create_new_interface );

  //TL modified
  dir->factory( "LoadColorBar", Load_colorbar::create_new_interface );

  return true;
}

extern "C" {
  int libGsTLAppli_gui_init() {
    return library_gui_init::init_lib();
  }
  int libGsTLAppli_gui_release(){
    return library_gui_init::release_lib();
  }
}
