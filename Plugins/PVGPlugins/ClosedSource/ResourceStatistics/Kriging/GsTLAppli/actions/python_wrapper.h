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

#ifndef __GSTLAPPLI_ACTIONS_PYHTON_WRAPPER_H__ 
#define __GSTLAPPLI_ACTIONS_PYHTON_WRAPPER_H__ 
 
#include <GsTLAppli/actions/common.h>
#include <GsTLAppli/utils/gstl_messages.h>

class GsTL_project; 
class Error_messages_handler; 


 
//---------------------------------------------- 

class ACTIONS_DECL Python_project_wrapper {
public:
  static GsTL_project* project() { return project_; }
  static void set_project( GsTL_project* proj ) { project_ = proj; }

  static void set_project_modified() { project_modified_ = true; }
  static void reset_project_modified() { project_modified_ = false; }
  static bool is_project_modified() { return project_modified_ ; }

private:
  static GsTL_project* project_;
  static bool project_modified_;
};


//===========================================

/** Python_channel is a globally accessible channel dedicated
* to messages regarding Python scripts
*/

class ACTIONS_DECL Python_cout_channel : public Channel { 
public: 
  Python_cout_channel() {} 
  virtual ~Python_cout_channel() {} 
}; 

class ACTIONS_DECL Python_cerr_channel : public Channel { 
public: 
  Python_cerr_channel() {} 
  virtual ~Python_cerr_channel() {} 
}; 


class ACTIONS_DECL GsTLAppli_Python_cout { 
public:
  static Python_cout_channel* instance(); 
 
private: 
  static Python_cout_channel* instance_; 
 
  GsTLAppli_Python_cout(); 
  GsTLAppli_Python_cout(const GsTLAppli_Python_cout&); 
  GsTLAppli_Python_cout& operator=(const GsTLAppli_Python_cout&); 
}; 


class ACTIONS_DECL GsTLAppli_Python_cerr { 
public:
  static Python_cerr_channel* instance(); 
 
private: 
  static Python_cerr_channel* instance_; 
 
  GsTLAppli_Python_cerr(); 
  GsTLAppli_Python_cerr(const GsTLAppli_Python_cerr&); 
  GsTLAppli_Python_cerr& operator=(const GsTLAppli_Python_cerr&); 
}; 



#endif