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

#include <GsTLAppli/gui/QWidget_value_accessors/qlineedit_accessor.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qdom.h>
#include <qstring.h>

QLineEdit_accessor::QLineEdit_accessor( QWidget* widget ) 
    : line_edit_(dynamic_cast<QLineEdit*>(widget) ) {}

    
bool QLineEdit_accessor::initialize( QWidget* widget ) {

  line_edit_ = dynamic_cast<QLineEdit*>(widget);
  if( line_edit_ == 0 )
    return false;
  
  return true;
}


std::string QLineEdit_accessor::value() const {

  std::string widget_name = line_edit_->name() ;
  std::string val;

  if( line_edit_->text().isEmpty() )
    val="";
  else
    val = line_edit_->text().latin1();

  return "<" + widget_name + "  value=\"" + val + "\" /> \n";
}


bool QLineEdit_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  line_edit_->setText( elem.attribute( "value" ) );

  return true;
}


void QLineEdit_accessor::clear() {
  line_edit_->setText( "" );
}


Named_interface* QLineEdit_accessor::create_new_interface(std::string&) {
  return new QLineEdit_accessor(0);
}

