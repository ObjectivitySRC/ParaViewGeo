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

#ifndef __GSTLAPPLI_GEOSTAT_PARAMETERS_HANDLER_H__ 
#define __GSTLAPPLI_GEOSTAT_PARAMETERS_HANDLER_H__ 
 
 
#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/utils/named_interface.h> 
 
#include <string> 
 
 
/** A Parameters_handler allows to access the values of  
 * different parameters used by geostat algorithms. The complete 
 * set of parameter values is supplied as a formated string.  
 * To support a new string formatting, sub-class Parameters_handler. 
 */ 
class GEOSTAT_DECL Parameters_handler : public Named_interface { 
  
 public:
   Parameters_handler() { is_ready_ = false ; }
  virtual ~Parameters_handler() {} 
   
  /** Sets the parameter string to be handled by the object 
   */ 
  virtual bool handle( const std::string& parameters ) = 0; 
 
  /** Finds the value of a given parameter inside the parameter string. 
   * A parameter has attributes and can also have nested parameters. 
   *   "parameter_name.attribute_name" refers to the attribute "attribute_name" 
   * of parameter "parameter_name". 
   *   "foo/bar/tabac" refers to the sub-sub-parameter "tabac" of sub-parameter 
   * "bar" of parameter "foo".  
   * @param parameter is a formated string.  
   *    "foo/bar.attr" refers to the attribute value "attr" of (sub-)parameter  
   * "bar" which is nested inside parameter "foo" 
   * @return a string containing the value of the requested attribute 
   */ 
  virtual std::string value( const std::string& parameter ) const = 0; 
   
  virtual bool is_ready() const { return is_ready_; }

 protected:
   bool is_ready_;
}; 
 
 
#endif 
