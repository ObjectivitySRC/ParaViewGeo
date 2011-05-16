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

#ifndef __GSTLAPPLI_GUI_APPLI_VISU_ACTIONS_H__ 
#define __GSTLAPPLI_GUI_APPLI_VISU_ACTIONS_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/actions/action.h>

//TL modified
class GUI_DECL Load_colorbar : public Action
{
public:
	static Named_interface* create_new_interface( std::string& );
public:
	Load_colorbar() {}
	virtual ~Load_colorbar() {}

	virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
	virtual bool exec();

};

class GUI_DECL Rotate_camera: public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Rotate_camera() {} 
  virtual ~Rotate_camera() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 
 

/** Saves the state of the camera (orientation, position,...) to a file
*/
class GUI_DECL Save_camera_settings_to_file : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Save_camera_settings_to_file() {} 
  virtual ~Save_camera_settings_to_file() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 
 

/** Saves the state of the camera (orientation, position,...) to a file
*/
class GUI_DECL Load_camera_settings : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Load_camera_settings() {} 
  virtual ~Load_camera_settings() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 



/** Saves the state of the camera (orientation, position,...) to a file
*/
class GUI_DECL Resize_camera_window : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Resize_camera_window() {} 
  virtual ~Resize_camera_window() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 

 

/** Shows the histogram of a given property. 
* Usage: ShowHistogram grid prop [#bins logscaling_flag y_min y_max x_min x_max] 
*/
class GUI_DECL Show_histogram : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Show_histogram() {} 
  virtual ~Show_histogram() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 
 


/** Saves the histogram of a given property to a file. 
* Usage: SaveHistogram grid prop file [format #bins logscaling_flag stats_flag grid_flag]
*/
class GUI_DECL Save_histogram : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Save_histogram() {} 
  virtual ~Save_histogram() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 
 


/** Saves the qqplot of two properties to a file. 
* Usage: SaveQQplot grid prop1 prop2 file [format stats_flag grid_flag]
*/
class GUI_DECL Save_qqplot : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Save_qqplot() {} 
  virtual ~Save_qqplot() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 


/** Saves the histogram of a given property to a file. 
* Usage: SaveHistogram grid prop file [format #bins logscaling_flag stats_flag grid_flag]
*/
class GUI_DECL Save_scatterplot : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Save_scatterplot() {} 
  virtual ~Save_scatterplot() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 





/** Displays a grid object in the default 3d camera
* Usage: display grid prop
*/
class GUI_DECL Display_object : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Display_object() {} 
  virtual ~Display_object() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 



/** Hides a grid object in the default 3d camera
* Usage: hide grid
*/
class GUI_DECL Hide_object : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Hide_object() {} 
  virtual ~Hide_object() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 

/** Sets the colormap for a given grid property.
* Usage: choose_colormap  grid::property::colormap[::min][::max]
*/
class GUI_DECL Choose_colormap : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Choose_colormap() {} 
  virtual ~Choose_colormap() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 



/** Takes a snapshot of the default 3d camera
* Usage: snapshot format
*/
class GUI_DECL Snapshot_action : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Snapshot_action() {} 
  virtual ~Snapshot_action() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 




#endif 
