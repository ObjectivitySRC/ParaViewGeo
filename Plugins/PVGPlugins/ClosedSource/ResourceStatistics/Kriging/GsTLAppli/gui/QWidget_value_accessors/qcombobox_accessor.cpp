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

#include <GsTLAppli/gui/QWidget_value_accessors/qcombobox_accessor.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qstring.h>
#include <qdom.h>
#include <qtimer.h>



QComboBox_accessor::QComboBox_accessor( QWidget* widget ) 
    : combo_box_(dynamic_cast<QComboBox*>(widget) ) {
  helper_ = new ComboBoxHelper(0);
}

    
bool QComboBox_accessor::initialize( QWidget* widget ) {

  combo_box_ = dynamic_cast<QComboBox*>(widget);
  if( combo_box_ == 0 )
    return false;
  
  return true;
}


std::string QComboBox_accessor::value() const {
  std::string widget_name = combo_box_->name();
  std::string val;

  if( combo_box_->currentText().isEmpty() )
     val="";
  else 
    val = combo_box_->currentText().latin1();

  return "<" + widget_name + "  value=\"" + val + "\"  /> \n";
}


bool QComboBox_accessor::set_value( const std::string& str ) {
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
  
  // select entry "value" in the combobox. If there is no such entry,
  // either add it if the combobox is editable or return false
  int entry_id = 0;
  while( combo_box_->text( entry_id ) != value ) {
    entry_id++;
    if( entry_id >= combo_box_->count() ) break;
  }

  if( entry_id >= combo_box_->count() ) {
    // the entry wasn't found
    if( combo_box_->editable() ) {
      combo_box_->insertItem( elem.attribute( "value" ) );
      return true;
    }
    else
      return false;
  }

//  combo_box_->setCurrentItem( entry_id );
  helper_->activateComboBox( combo_box_, entry_id );
  return true;
}

 

void QComboBox_accessor::clear() {
  if( combo_box_->editable() ) 
    combo_box_->setCurrentText( "" );
}



Named_interface* QComboBox_accessor::create_new_interface(std::string&) {
  return new QComboBox_accessor;
}




//===================================

ComboBoxHelper::ComboBoxHelper( QObject* parent ) : QObject( parent ) {}

void ComboBoxHelper::activateComboBox( QComboBox* box, int  i ) {
  QObject::connect( this, SIGNAL( doInternalActivate( int ) ),
                    box, SLOT( internalActivate(int) ) );
  emit doInternalActivate(i);
  QObject::disconnect( this, SIGNAL( doInternalActivate( int ) ), 0, 0 );
}
