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

#ifndef __GSTLAPPLI_ACTIONS_ACTION_H__ 
#define __GSTLAPPLI_ACTIONS_ACTION_H__ 
 
#include <GsTLAppli/actions/common.h>
#include <GsTLAppli/utils/named_interface.h> 
 
#include <string> 
 
 
class GsTL_project; 
class Error_messages_handler; 
 
/** The base class for all actions. 
 */ 
class ACTIONS_DECL Action : public Named_interface { 
 
 public: 
  virtual ~Action() {} 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ) = 0; 
  virtual bool exec() = 0; 
 
 
}; // end of class Action 
 
#endif 
