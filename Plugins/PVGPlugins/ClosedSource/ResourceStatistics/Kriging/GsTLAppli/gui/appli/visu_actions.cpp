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

#include <GsTLAppli/gui/appli/visu_actions.h>
#include <GsTLAppli/gui/appli/oinv_project_view.h>
#include <GsTLAppli/gui/appli/SoQtGsTLViewer.h>
#include <GsTLAppli/gui/appli/snapshot_dialog.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/appli/histogram_gui.h>
#include <GsTLAppli/gui/appli/scatterplot_gui.h>
#include <GsTLAppli/gui/appli/qpplot_gui.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/gui/appli/SoQtGsTLViewer.h>

#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/nodes/SoCamera.h>

#include <qfile.h>

bool Load_colorbar::init(std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) 
{
	QString s;
	std::string proj_name = proj->name();
	std::vector< std::string > params = 
		String_Op::decompose_string( parameters, Actions::separator,
		Actions::unique );

	s = params[0].c_str();

	SmartPtr<Named_interface> ni = 
		Root::instance()->interface( projectViews_manager+"/main_view"); 
	Oinv_view * dir1 = dynamic_cast<Oinv_view*>( ni.raw_ptr() );
	appli_assert( dir1 );
	dir1->load_colorbar(s);
	return true;
}

bool Load_colorbar::exec()
{
	return true;
}

Named_interface* Load_colorbar::create_new_interface( std::string& ) {
  return new Load_colorbar();
}
// ===================================

bool Rotate_camera::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 4 ) {
    errors->report( "some parameters are missing" );  
    return false;
  }

  SmartPtr<Named_interface> view_ni =
    Root::instance()->interface( projectViews_manager + "/main_view"  );
  Oinv_view* view = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  if( !view ) {
    return false;
  }
  
  float x = String_Op::to_number<float>( params[0] );
  float y = String_Op::to_number<float>( params[1] );
  float z = String_Op::to_number<float>( params[2] );
  float ang = String_Op::to_number<float>( params[3] );

  view->get_render_area()->rotate_camera( GsTLVector<float>(x,y,z), ang );
  
  return true;
}


bool Rotate_camera::exec() {
  return true;
}


Named_interface* 
Rotate_camera::create_new_interface( std::string& ) {
  return new Rotate_camera; 
}




//=============================================

bool Save_camera_settings_to_file::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 1 ) {
    errors->report( "save_camera_settings filename" );  
    return false;
  }

  SmartPtr<Named_interface> view_ni =
    Root::instance()->interface( projectViews_manager + "/main_view"  );
  Oinv_view* view = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  if( !view ) {
    return false;
  }
  
  SoCamera* camera = view->get_render_area()->getCamera();
  if( !camera ) return false;


  std::ofstream out( params[0].c_str() );
  if( !out ) {
    std::ostringstream message;
    message << "Can't write to file " << params[0]; 
    errors->report( message.str() );  
    return false;
  }

  SbVec3f pos = camera->position.getValue();
  SbRotation rot = camera->orientation.getValue();
  float rot1, rot2, rot3, rot4;
  rot.getValue( rot1, rot2, rot3, rot4 );


  out << pos[0] << " " << pos[1] << " " << pos[2] << "\n"
      << rot1 << " " << rot2 << " " << rot3 << " " << rot4 << "\n"
      << camera->aspectRatio.getValue() << "\n"
      << camera->nearDistance.getValue() << "\n"
      << camera->farDistance.getValue() << "\n"
      << camera->focalDistance.getValue() << std::endl;
  out.close();

  return true;
}


bool Save_camera_settings_to_file::exec() {
  return true;
}


Named_interface* 
Save_camera_settings_to_file::create_new_interface( std::string& ) {
  return new Save_camera_settings_to_file; 
}




//=============================================

bool Load_camera_settings::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 1 ) {
    errors->report( "load_camera_settings filename" );  
    return false;
  }

  SmartPtr<Named_interface> view_ni =
    Root::instance()->interface( projectViews_manager + "/main_view"  );
  Oinv_view* view = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  if( !view ) {
    return false;
  }
  
  SoCamera* camera = view->get_render_area()->getCamera();
  if( !camera ) return false;


  std::ifstream in( params[0].c_str() );
  if( !in ) {
    std::ostringstream message;
    message << "Can't open file " << params[0]; 
    errors->report( message.str() );  
    return false;
  }

  float x,y,z;
  in >> x >> y >> z;
  camera->position.setValue( SbVec3f(x,y,z) );

  float r1,r2,r3,r4;
  in >> r1 >> r2 >> r3 >> r4;
  camera->orientation.setValue( SbRotation(r1,r2,r3,r4) );

  float val;
  in >> val;
  camera->aspectRatio = val;
  in >> val;
  camera->nearDistance = val;
  in >> val;
  camera->farDistance = val;
  in >> val;
  camera->focalDistance = val;

  in.close();

  return true;
}


bool Load_camera_settings::exec() {
  return true;
}


Named_interface* 
Load_camera_settings::create_new_interface( std::string& ) {
  return new Load_camera_settings; 
}




//=============================================

bool Resize_camera_window::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 2 ) {
    errors->report( "resize_camera_window width height" );  
    return false;
  }

  SmartPtr<Named_interface> view_ni =
    Root::instance()->interface( projectViews_manager + "/main_view"  );
  Oinv_view* view = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  if( !view ) {
    return false;
  }
  
  int width = String_Op::to_number<int>( params[0] );
  int height = String_Op::to_number<int>( params[1] );

  view->get_render_area()->getParentWidget()->resize( width, height );
  return true;
}


bool Resize_camera_window::exec() {
  return true;
}


Named_interface* 
Resize_camera_window::create_new_interface( std::string& ) {
  return new Resize_camera_window; 
}




//=============================================

Named_interface* Show_histogram::create_new_interface( std::string& ) {
  return new Show_histogram;
}


bool Show_histogram::init( std::string& parameters, GsTL_project* proj,
                           Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) {
	  // TL modified
    errors->report( "Usage: ShowHistogram grid prop [#bins " 
                    "logscaling_flag cdf_flag]" );  
    return false;
  }


  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  GsTLGridProperty* prop = grid->property( params[1] );
  if( !prop ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has no property called \"" 
            << params[1] << "\"";
    errors->report( message.str() ); 
    return false;
  }

  //TL modified
  bool cdf_flag = true;
  if (params.size() == 5) {
	  if (params[4] == "0") {
		  appli_message("Turning off cdf");
		  cdf_flag = false;
	  }
  }

  Histogram_gui* histog = new Histogram_gui( proj, 0 );
  if (cdf_flag)
	  histog->changeCurve("pdf+cdf");
  else
	  histog->changeCurve("pdf");
  histog->get_data_from( prop );


  // check if other options were input
  if( params.size() >= 3 ) {
    int bins = String_Op::to_number<int>( params[2] );
    histog->update_bins( bins );

    if( params.size() >= 4 ) {
      bool logscale = String_Op::to_number<bool>( params[3] );
      histog->set_x_axis_logscale( logscale );
    }
  }

  histog->show();
  return true;
}

bool Show_histogram::exec() {
  return true;
}




//=============================================

Named_interface* Save_histogram::create_new_interface( std::string& ) {
  return new Save_histogram;
}


bool Save_histogram::init( std::string& parameters, GsTL_project* proj,
                           Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  // TL modified
  if( params.size() < 3 ) {
    errors->report( "Usage: SaveHistogram grid prop file [format #bins " 
                    "logscaling_flag show_stats_flag show_grid_flag cdf_flag]" );  
    return false;
  }


  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  GsTLGridProperty* prop = grid->property( params[1] );
  if( !prop ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has no property called \"" 
            << params[1] << "\"";
    errors->report( message.str() ); 
    return false;
  }

  // check if we can write to the requested file
  QFile file( params[2].c_str() );
  if( !file.open( IO_WriteOnly ) ) {
    std::ostringstream message;
    message << "Can't open file " << params[2];
    errors->report( message.str() ); 
    return false;
  }
  file.close();


    //TL modified
  bool cdf_flag = true;
  if (params.size() == 9) {
	  if (params[8] == "0") {
		  appli_message("Turning off cdf");
		  cdf_flag = false;
	  }
  }

  Histogram_gui* histog = new Histogram_gui( proj, 0 );
  //TL modified
  if (cdf_flag)
	histog->changeCurve("pdf+cdf");
  else
	histog->changeCurve("pdf");

  histog->get_data_from( prop );

  std::string format = "PNG";
  bool show_stats = true;
  bool show_grid = false;

  // check if other options were input
  if( params.size() >= 4 ) {
    format = params[3] ;
  }
  if( params.size() >= 5 ) {
    int bins = String_Op::to_number<int>( params[4] );
    histog->update_bins( bins );
  }
  if( params.size() >= 6 ) {
    bool logscale = String_Op::to_number<bool>( params[5] );
    histog->set_x_axis_logscale( logscale );
  }
  if( params.size() >= 7 ) {
    show_stats = String_Op::to_number<bool>( params[6] );
  }
  if( params.size() >= 8 ) {
    show_grid = String_Op::to_number<bool>( params[7] );
  }
  histog->show();
  histog->save_as_image( params[2].c_str(), format.c_str(), 
                         show_stats, show_grid );
  histog->hide();
  delete histog;
  return true;
}

bool Save_histogram::exec() {
  return true;
}



//=============================================

Named_interface* Save_qqplot::create_new_interface( std::string& ) {
  return new Save_qqplot;
}


bool Save_qqplot::init( std::string& parameters, GsTL_project* proj,
                           Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 4 ) {
    errors->report( "Usage: SaveQQplot grid prop1 prop2 file [format "
                    "stats_flag grid_flag]" );
    return false;
  }


  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  GsTLGridProperty* prop1 = grid->property( params[1] );
  GsTLGridProperty* prop2 = grid->property( params[2] );
  if( !prop1 || !prop2 ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has no property called \"" ;
    if (!prop1 ) message << params[1] << "\" ";
    if (!prop2 ) message << params[2] << "\" ";
    errors->report( message.str() ); 
    return false;
  }

  // check if we can write to the requested file
  QFile file( params[3].c_str() );
  if( !file.open( IO_WriteOnly ) ) {
    std::ostringstream message;
    message << "Can't open file " << params[3];
    errors->report( message.str() ); 
    return false;
  }
  file.close();


  QPplot_gui* qqplot = new QPplot_gui( proj, 0 );
  qqplot->get_var1_data_from( prop1 );
  qqplot->get_var2_data_from( prop2 );

  std::string format = "PNG";
  bool show_stats = true;
  bool show_grid = false;

  // check if other options were input
  if( params.size() >= 5 ) {
    format = params[4] ;
  }
  if( params.size() >= 6 ) {
    show_stats = String_Op::to_number<bool>( params[5] );
  }
  if( params.size() >= 7 ) {
    show_grid = String_Op::to_number<bool>( params[6] );
  }

  qqplot->show();
  qqplot->save_as_image( params[3].c_str(), format.c_str(), 
                         show_stats, show_grid );
  qqplot->hide();
  delete qqplot;
  return true;
}

bool Save_qqplot::exec() {
  return true;
}




//=============================================

Named_interface* Save_scatterplot::create_new_interface( std::string& ) {
  return new Save_scatterplot;
}


bool Save_scatterplot::init( std::string& parameters, GsTL_project* proj,
                             Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 4 ) {
    errors->report( "Usage: SaveScatterplot grid prop1 prop2 file [format "
                    "stats_flag grid_flag logY logX]" );
    return false;
  }


  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  GsTLGridProperty* prop1 = grid->property( params[1] );
  GsTLGridProperty* prop2 = grid->property( params[2] );
  if( !prop1 || !prop2 ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has no property called \"" ;
    if (!prop1 ) message << params[1] << "\" ";
    if (!prop2 ) message << params[2] << "\" ";
    errors->report( message.str() ); 
    return false;
  }

  // check if we can write to the requested file
  QFile file( params[3].c_str() );
  if( !file.open( IO_WriteOnly ) ) {
    std::ostringstream message;
    message << "Can't open file " << params[3];
    errors->report( message.str() ); 
    return false;
  }
  file.close();


  Scatterplot_gui* scplot = new Scatterplot_gui( proj, 0 );
  scplot->get_var1_data_from( prop1 );
  scplot->get_var2_data_from( prop2 );

  std::string format = "PNG";
  bool show_stats = true;
  bool show_grid = false;

  // check if other options were input
  if( params.size() >= 5 ) {
    format = params[4] ;
  }
  if( params.size() >= 6 ) {
    show_stats = String_Op::to_number<bool>( params[5] );
  }
  if( params.size() >= 7 ) {
    show_grid = String_Op::to_number<bool>( params[6] );
  }
  if( params.size() >= 8 ) {
    bool ylog = String_Op::to_number<bool>( params[7] );
    scplot->set_y_axis_logscale( ylog );
  }
  if( params.size() >= 8 ) {
    bool xlog = String_Op::to_number<bool>( params[7] );
    scplot->set_x_axis_logscale( xlog );
  }

  scplot->show();
  scplot->save_as_image( params[3].c_str(), format.c_str(), 
                         show_stats, show_grid );
  scplot->hide();
  delete scplot;
  return true;
}

bool Save_scatterplot::exec() {
  return true;
}





//=============================================

Named_interface* Display_object::create_new_interface( std::string& ) {
  return new Display_object;
}


bool Display_object::init( std::string& parameters, GsTL_project* proj,
                           Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() == 0 ) {
    errors->report( "Usage: display grid prop" );
    return false;
  }

  SmartPtr<Named_interface> view_ni =
    Root::instance()->interface( projectViews_manager + "/main_view"  );
  Oinv_view* view = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  if( !view ) {
    return false;
  }

  if( params.size() == 1 )
    view->set_object_displayed( params[0].c_str() );
  else {
    view->set_object_displayed( params[0].c_str() );
    view->set_property_displayed( params[0].c_str(), params[1].c_str() );
  }
  return true;
}

bool Display_object::exec() {
  return true;
}




//=============================================

Named_interface* Hide_object::create_new_interface( std::string& ) {
  return new Hide_object;
}


bool Hide_object::init( std::string& parameters, GsTL_project* proj,
                           Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 1 ) {
    errors->report( "Usage: hide grid" );
    return false;
  }

  SmartPtr<Named_interface> view_ni =
    Root::instance()->interface( projectViews_manager + "/main_view"  );
  Oinv_view* view = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  if( !view ) {
    return false;
  }

  view->set_object_undisplayed( params[0].c_str() );
  return true;
}

bool Hide_object::exec() {
  return true;
}

//=============================================

Named_interface* Choose_colormap::create_new_interface( std::string& ) {
  return new Choose_colormap;
}


bool Choose_colormap::init( std::string& parameters, GsTL_project* proj,
                           Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 3 ) {
    errors->report( "Usage: choose_colormap  grid::property::colormap[::min][::max]" );
    return false;
  }

  SmartPtr<Named_interface> view_ni =
    Root::instance()->interface( projectViews_manager + "/main_view"  );
  Oinv_view* view = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  if( !view ) {
    return false;
  }

  QString min_str, max_str;
  if( params.size() > 3 ) min_str = QString( params[3].c_str() );
  if( params.size() > 4 ) max_str = QString( params[4].c_str() );
  view->set_colormap( params[2].c_str(), params[0].c_str(), params[1].c_str(),
                         min_str, max_str );
  return true;
}

bool Choose_colormap::exec() {
  return true;
}




//=============================================

Named_interface* Snapshot_action::create_new_interface( std::string& ) {
  return new Snapshot_action;
}



bool Snapshot_action::init( std::string& parameters, GsTL_project* proj,
                           Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );


  if( params.size() < 1 ) {
    errors->report( "Usage: snapshot filename format" );
    return false;
  }

  SmartPtr<Named_interface> view_ni =
    Root::instance()->interface( projectViews_manager + "/main_view"  );
  Oinv_view* view = dynamic_cast<Oinv_view*>( view_ni.raw_ptr() );
  if( !view ) {
    return false;
  }

  QString format = "PNG";
  if( params.size() == 2 )
    format = params[1].c_str();

 

  Snapshot_dialog snap( view->get_render_area() );
  snap.take_snapshot( params[0].c_str(), format );
  return true;
}

bool Snapshot_action::exec() {
  return true;
}

