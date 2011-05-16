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

#include <GsTLAppli/gui/QWidget_value_accessors/qtable_accessor.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <qstring.h>
#include <qdom.h>


std::string QTable_accessor::row_sep = "\n";
std::string QTable_accessor::col_sep = " ";

QTable_accessor::QTable_accessor( QWidget* widget ) 
    : table_(dynamic_cast<QTable*>(widget) ) {}

    
bool QTable_accessor::initialize( QWidget* widget ) {

  table_ = dynamic_cast<QTable*>(widget);
  if( table_ == 0 )
    return false;
  
  return true;
}


std::string QTable_accessor::value() const {
  appli_assert( table_ );

  /* Scan the table line by line. Stop scanning a line
   * as soon as an empty cell is encountered.
   */
  std::string widget_name = table_->name() ;
  std::string val;

  for( int row = 0 ; row < table_->numRows(); row++) {
    int col=0;
    while( col < table_->numCols() && !table_->text( row, col ).isEmpty() ) {

      QString text( table_->text( row, col ) );
      std::string std_text;
      std_text = text.latin1() ;
	
      val += std_text + col_sep ;
      col++;
    }
    if( row != table_->numRows() - 1 )
      val += row_sep;
  }

  return "<" + widget_name + "  value=\"" + val + "\" /> \n";
}


bool QTable_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  //  QDomNode node = doc.firstChild();
  //  QDomElement elem = node.toElement();
  
  // In string "value", each row is separated by a new line and each
  // element of a row is separated by a tab 
  appli_message( "value= " << elem.attribute( "value" ) );
  std::string value( elem.attribute( "value" ).latin1() ) ;
  String_Op::string_vector rows = 
    String_Op::decompose_string( value, row_sep, false );


  for( int i=0; i < rows.size() ; i++ ) {
    String_Op::string_vector row_elements = 
      String_Op::decompose_string( rows[i], col_sep, false );
    for( int j=0; j < row_elements.size(); j++ ) {
      QComboTableItem* item = dynamic_cast<QComboTableItem*>( table_->item(i,j) );
      if( item ) 
        item->setCurrentItem( row_elements[j].c_str() );
      else 
        table_->setText( i,j, row_elements[j].c_str() );
    }
  }
  return true;
}


void QTable_accessor::clear() {
  for( int i = 0; i < table_->numRows() ; i++ )
    for( int j = 0; j < table_->numCols() ; j++ )
      table_->clearCell( i, j );
}

Named_interface* QTable_accessor::create_new_interface(std::string&) {
  return new QTable_accessor(0);
}


