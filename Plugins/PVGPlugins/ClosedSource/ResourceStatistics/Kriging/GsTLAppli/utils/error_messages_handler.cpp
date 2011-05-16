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

#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/gstl_messages.h>


Error_messages_handler::Error_messages_handler() {
  errors_ = 0;
}

Error_messages_handler::~Error_messages_handler() {
  // stlport-4.5.3 compiled with intel 7.1 doesn't seem to like when
  // I delete an ostringstream...   To be fixed, 2003/04/23. 
  
  // delete errors_;
}

void Error_messages_handler::report( bool condition,
					 const std::string& field, 
					 const std::string& error_message ) {
  if( !errors_ )
    errors_ = new std::ostringstream;

  if( condition ) {
    *errors_ << "<" << field << "  diagnostic=\"" << error_message 
             << "\" />" << std::endl;
  }
}

void Error_messages_handler::report_if( bool condition,
					 const std::string& error_message ) {
  if( !errors_ )
    errors_ = new std::ostringstream;

  if( condition ) {
    *errors_ << error_message << std::endl;
  }
}


std::string Error_messages_handler::errors() const {
  if( !errors_ )
    return "";

  return errors_->str();
}

bool Error_messages_handler::empty() {
  if( errors_ == 0 )
    return true;

  return errors_->str().empty();
}


void Error_messages_handler::clear() {
  //errors_->str( "" ) ;
}

void Error_messages_handler::output() {
  appli_warning( "output() is obsolete!!" );
}


//==============================================

Error_messages_handler_xml::Error_messages_handler_xml() 
  : Error_messages_handler() {
  errors_ = new std::ostringstream;
}

void Error_messages_handler_xml::report( bool condition,
					 const std::string& field, 
					 const std::string& error_message ) {
  if( condition ) {
    *errors_ << "<" << field << "  diagnostic=\"" << error_message 
             << "\" />" << std::endl;
  }
}

void Error_messages_handler_xml::report( bool condition,
					 const std::string& error_message ) {
  if( condition ) {
    *errors_ << "<generic_error" << "  diagnostic=\"" << error_message 
             << "\" />" << std::endl;
  }
}



