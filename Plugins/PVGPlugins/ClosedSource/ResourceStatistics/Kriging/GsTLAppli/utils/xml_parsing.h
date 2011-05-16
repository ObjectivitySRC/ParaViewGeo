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

#include <GsTLAppli/utils/common.h>
#include <GsTL/geometry/covariance.h> 
 
#include <GsTLAppli/math/gstlpoint.h> 
 
#include <qdom.h> 
 
#include <strstream> 
#include <string> 
 
 
 
namespace XML_UTIL { 
 
  //=========================================== 
  //     generic version 
  template<class T> 
  bool get_value( T& value_holder,  
		  const QDomDocument& xml_doc,  
		  const QString& tag_name, 
		  const QString& attr ) { 
 
    QDomNodeList nodes = xml_doc.elementsByTagName( tag_name ); 
    QDomNode current_node = nodes.item(0); 
     
    if( !current_node.isElement() ) return false; 
     
    QDomElement elem = current_node.toElement(); 
     
    QString str_value = elem.attribute( attr ); 
     
    std::istrstream convert_str( str_value.latin1() ); 
    convert_str >> value_holder; 
    return true; 
  } 
 
  //=========================================== 
  //     specialized version for QString 
  template<> 
  bool get_value<QString>( QString& value_holder,  
			   const QDomDocument& xml_doc,  
			   const QString& tag_name, 
			   const QString& attr ) { 
 
    QDomNodeList nodes = xml_doc.elementsByTagName( tag_name ); 
    QDomNode current_node = nodes.item(0); 
     
    if( !current_node.isElement() ) return false; 
 
    QDomElement elem = current_node.toElement(); 
     
    value_holder = elem.attribute( attr ); 
    return true; 
  } 
 
  //=========================================== 
  //     specialized version for std::string 
  template<> 
  bool get_value<std::string>( std::string& value_holder,  
			       const QDomDocument& xml_doc,  
			       const QString& tag_name, 
			       const QString& attr ) { 
 
    QDomNodeList nodes = xml_doc.elementsByTagName( tag_name ); 
    QDomNode current_node = nodes.item(0); 
     
    if( !current_node.isElement() ) return false; 
 
    QDomElement elem = current_node.toElement(); 
     
    value_holder = elem.attribute( attr ); 
    return true; 
  } 
 
  /* 
  //=========================================== 
  //     specialized version for Covariance 
  template<> 
  bool get_value<Covariance<GsTLPoint> >(  
	       Covariance<GsTLPoint>& value_holder,  
	       const QDomDocument& xml_doc,  
	       const QString& tag_name, 
	       const QString& attr 
	    ) { 
    return false; 
  } 
  */	     
 
   
} // namespace XML_UTIL 
