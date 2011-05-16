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

#ifndef __GSTLAPPLI_OBJ_MANAG_ACTIONS_H__ 
#define __GSTLAPPLI_OBJ_MANAG_ACTIONS_H__ 
 
#include <GsTLAppli/actions/common.h>
#include <GsTLAppli/actions/action.h> 
#include <GsTLAppli/filters/filter.h> 
#include <GsTLAppli/grid/grid_model/geostat_grid.h> 
#include <GsTLAppli/grid/grid_model/property_copier.h>

#include <qstring.h>
 
#include <string> 
 

class Input_filter; 
class GsTL_project; 
class Error_messages_handler;
 
 
class ACTIONS_DECL New_rgrid : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  New_rgrid() {} 
  virtual ~New_rgrid() {} 
   
  /** The parameters for this function are: 
   * - the name of the new grid 
   * - its dimensions nx, ny, nz 
   */ 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
 
 private: 
  int nx_, ny_, nz_; 
  std::string name_; 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 
 
 
class ACTIONS_DECL New_cartesian_grid_action : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  New_cartesian_grid_action(); 
  virtual ~New_cartesian_grid_action() {} 
   
  /** The parameters for this function are: 
   * - the name of the new grid 
   * - its dimensions nx, ny, nz 
   */ 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
 
 private: 
  int nx_, ny_, nz_; 
  float xsize_, ysize_, zsize_; 
  float Ox_, Oy_, Oz_;  
  std::string name_; 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 
 
 
class ACTIONS_DECL Load_object_from_file : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Load_object_from_file() {} 
  virtual ~Load_object_from_file(); 
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
 
 protected:
   bool find_filter( const std::string& filename );

 protected: 
  std::string file_name_; 
  SmartPtr<Input_filter> filter_; 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 
 
 
class ACTIONS_DECL Save_geostat_grid : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Save_geostat_grid() {}  // TL modified
  virtual ~Save_geostat_grid() {}; 
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
 
 protected: 
  std::string file_name_; 
  SmartPtr<Geostat_grid> grid_; 
  SmartPtr<Output_filter> filter_; 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 
 





class ACTIONS_DECL RunScript : public Action 
{
public:
	static Named_interface* create_new_interface( std::string& );

public:
	RunScript() { }
	virtual ~RunScript() {}

	virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
	virtual bool exec();
	
protected:
	std::string _scriptfile;
	GsTL_project* _proj; 
	Error_messages_handler* _errors;

};

//TL modified
/*
class ACTIONS_DECL Load_sim : public Action {
public: 
  static Named_interface* create_new_interface( std::string& );
  
public:
  Load_sim() ;
  virtual ~Load_sim() ;

  
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();
  
protected: 

  typedef GsTLGridProperty::property_type Type ;

  QString dirname_; 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
  SmartPtr<Input_filter> filter_; 
  static const std::string parser ;

  void transform(const Type *, int, int, int, GsTLGridProperty *) ;

};

*/


class ACTIONS_DECL Load_project : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Load_project() {} 
  virtual ~Load_project(); 
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
 
 protected: 
  QString dirname_; 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
};  

 

/** Transfer a property from a grid to another grid.
* 4 parameters are required (in that order):
* - the server grid
* - the server property
* - the client grid
* - the client porperty
*/
class ACTIONS_DECL Copy_property: public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Copy_property() {} 
  virtual ~Copy_property() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
 
 protected: 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
  SmartPtr<Property_copier> copier_;

  Geostat_grid* server_;
  GsTLGridProperty* server_prop_;

  Geostat_grid* client_;
  GsTLGridProperty* client_prop_;

};  


/** Transfer a property from RAM to disk.
* parameters: grid name and property names
*/
class ACTIONS_DECL Swap_property_to_disk : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Swap_property_to_disk() {} 
  virtual ~Swap_property_to_disk() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 

 protected: 
  std::vector<GsTLGridProperty*> properties_to_swap_;
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 



/** Transfer a property from disk to RAM.
* parameters: grid name and property names
*/
class ACTIONS_DECL Swap_property_to_ram : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Swap_property_to_ram() {} 
  virtual ~Swap_property_to_ram() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 

 protected: 
  std::vector<GsTLGridProperty*> properties_to_swap_;
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 




class ACTIONS_DECL Delete_objects : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Delete_objects() {} 
  virtual ~Delete_objects() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 

 protected: 
  std::vector<GsTLGridProperty*> properties_to_delete_;
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 



class ACTIONS_DECL Delete_properties : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Delete_properties() {} 
  virtual ~Delete_properties() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 

 protected: 
  std::vector<GsTLGridProperty*> properties_to_delete_;
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 



class ACTIONS_DECL Clear_property_value_if: public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Clear_property_value_if() {} 
  virtual ~Clear_property_value_if() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
}; 




#endif 
