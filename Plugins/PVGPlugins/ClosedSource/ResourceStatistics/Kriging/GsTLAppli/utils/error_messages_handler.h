/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "utils" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_UTILS_ERROR_HANDLER_H__ 
#define __GSTLAPPLI_UTILS_ERROR_HANDLER_H__ 
 
#include <GsTLAppli/utils/common.h>

#include <sstream>
#include <string> 
 
/** A Error_messages_handler is an object to which several errors are reported. 
 * When asked, the Error_messages_handler output the errors that were reported 
 * to it. 
 * The error is reported by specifying where it happened (in which field or
 * parameter) and why it happened. It is also possible to report generic errors,
 * ie errors not related to any specific field or parameter.
 */ 
 
class UTILS_DECL Error_messages_handler { 
 public: 

   Error_messages_handler();
   virtual ~Error_messages_handler();

  /** Report an error made in field "field".  
   * @param if "condition" is true, the error is reported 
   * @param field is the name of the field where the error occurred 
   * @param error_message is the explaination of the error 
   */ 
  virtual void report( bool condition, 
		       const std::string& field,  
		       const std::string& error_message ) ; 
 
  /** Report an error made in field "field". 
   */ 
  virtual void report( const std::string& field,  
    const std::string& error_message) {
    report( true, field, error_message );
  }

  /** if \c condition is true, report an error that is not related to any
  * particular parameter
  */
  virtual void report_if( bool condition, const std::string& error_message );

  /** Report an error that is not related to any particular parameter
  */
  virtual void report( const std::string& error_message ) {
    report_if( true, error_message );
  }


  /** "Output" all the errors that were reported - obsolete!! 
   */ 
  virtual void output(); 
 
  /** Return a formated string containing all the reported errors so far.
  */
  virtual std::string errors() const;

  /** Returns whether any errors were reported
  * @return true if no error was reported.
  */
  virtual bool empty(); 

  void clear();

 protected:
   std::ostringstream* errors_;
}; 
 

 
 
class UTILS_DECL Error_messages_handler_xml : public Error_messages_handler { 
 public: 
   Error_messages_handler_xml();
  virtual ~Error_messages_handler_xml() {} 
  virtual void report( bool condition, 
		       const std::string& field,  
		       const std::string& error_message ); 
  
  virtual void report( bool condition, const std::string& error_message );
}; 
 
 
#endif 
