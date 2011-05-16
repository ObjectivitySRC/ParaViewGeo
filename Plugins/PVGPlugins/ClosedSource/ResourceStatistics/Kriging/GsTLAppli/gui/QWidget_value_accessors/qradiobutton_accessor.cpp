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

#include <GsTLAppli/gui/QWidget_value_accessors/qradiobutton_accessor.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qstring.h>
#include <qdom.h>



QRadioButton_accessor::QRadioButton_accessor( QWidget* widget ) 
    : radio_button_(dynamic_cast<QRadioButton*>(widget) ) {}

    
bool QRadioButton_accessor::initialize( QWidget* widget ) {

  radio_button_ = dynamic_cast<QRadioButton*>(widget);
  if( radio_button_ == 0 )
    return false;
  
  return true;
}


std::string QRadioButton_accessor::value() const {
  std::string widget_name = radio_button_->name();
  std::string val;

  if( radio_button_->isChecked() )
    val = "1";
  else
    val = "0";

  return "<" + widget_name + "  value=\"" + val + "\"  /> \n";
}


bool QRadioButton_accessor::set_value( const std::string& str ) {

  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  QString value = elem.attribute( "value" );

  if( value == "1")
    radio_button_->setChecked(true);
  else
    radio_button_->setChecked(false);

  return true;
}


void QRadioButton_accessor::clear() {
  radio_button_->setChecked(false);
}

Named_interface* QRadioButton_accessor::create_new_interface(std::string&) {
  return new QRadioButton_accessor(0);
}
