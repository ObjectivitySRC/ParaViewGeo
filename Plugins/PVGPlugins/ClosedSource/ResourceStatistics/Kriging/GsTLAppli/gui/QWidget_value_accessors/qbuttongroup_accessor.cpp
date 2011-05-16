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

#include <GsTLAppli/gui/QWidget_value_accessors/qbuttongroup_accessor.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qstring.h>
#include <qdom.h>



QButtonGroup_accessor::QButtonGroup_accessor( QWidget* widget ) 
    : group_(dynamic_cast<QButtonGroup*>(widget) ) {}

    
bool QButtonGroup_accessor::initialize( QWidget* widget ) {

  group_ = dynamic_cast<QButtonGroup*>(widget);
  if( group_ == 0 )
    return false;
  
  return true;
}


std::string QButtonGroup_accessor::value() const {
  std::string widget_name = group_->name();
  // Not implemented yet.

  return "<" + widget_name  + "\"  /> \n";
}


bool QButtonGroup_accessor::set_value( const std::string& str ) {
  // Not implemented yet.
  return false;
}


void QButtonGroup_accessor::clear() {
   // Not implemented yet.
}

Named_interface* QButtonGroup_accessor::create_new_interface(std::string&) {
  return new QButtonGroup_accessor(0);
}
