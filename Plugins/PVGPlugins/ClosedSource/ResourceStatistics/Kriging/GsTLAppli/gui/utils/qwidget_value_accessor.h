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

#ifndef __GSTLAPPLI_QWIDGET_VALUE_ACCESSOR_H__ 
#define __GSTLAPPLI_QWIDGET_VALUE_ACCESSOR_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/utils/named_interface.h> 
 
#include <qwidget.h> 
 
/** 
 * A QWidget_value_accessor is an object that can read and modify 
 * the value of a QWidget. What we call "value of a QWidget" is the 
 * value that the user can interactively set for a QWidget.  
 * Not all QWidgets have a "value": QGroupButton, QFrame, etc don't 
 * have values that can be set by the user.  
 * 
 * Examples of QWidgets that hold a value: 
 *   - a QLineEdit (stores a string) 
 *   - a QSpinBox  (stores an int) 
 *   - a QRadioButton (stores a boolean) 
 */ 
 
class GUI_DECL QWidget_value_accessor : public Named_interface { 
public: 
	virtual ~QWidget_value_accessor();
  virtual bool initialize( QWidget* widget = 0 ) { return false; } 
  virtual bool is_valid() const { return false; } 
  virtual std::string value() const { return ""; } 
  virtual bool set_value(const std::string& ) { return false; } 
  virtual void clear() {}; 

  // TODO: don't forget to check these: should these be private?
  QWidget_value_accessor();
  QWidget_value_accessor( const QWidget_value_accessor& rhs );
  QWidget_value_accessor& operator = ( const QWidget_value_accessor& rhs );
}; 
 
#endif 
