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

#ifndef __GSTLAPPLI_GEOSTAT_PARAMETERS_HANDLER_IMPL_H__ 
#define __GSTLAPPLI_GEOSTAT_PARAMETERS_HANDLER_IMPL_H__ 
 
#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/geostat/parameters_handler.h> 
 

//#include <qdom.h>
#include <QDomDocument>
#include <QDomElement>
 
/** This parameters handler understands xml-formated parameter strings. 
 */  
class GEOSTAT_DECL Parameters_handler_xml : public Parameters_handler { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Parameters_handler_xml( const std::string& parameters ); 
  virtual ~Parameters_handler_xml() {} 
 
  virtual bool handle( const std::string& parameters ); 
  virtual std::string value( const std::string& parameter ) const; 
 
  //TL modified
  const QDomDocument & getDoc() const { return xml_params_; }

 protected: 
  QDomDocument xml_params_; 
  QDomElement root_element_;

 private: 
  std::string get_value( QDomElement start, const std::string& param ) const; 
}; 
 
 
#endif 
