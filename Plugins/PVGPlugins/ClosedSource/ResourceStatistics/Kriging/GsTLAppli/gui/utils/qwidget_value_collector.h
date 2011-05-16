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

#ifndef __GSTLAPPLI_QWIDGET_VALUE_COLLECTOR_H__ 
#define __GSTLAPPLI_QWIDGET_VALUE_COLLECTOR_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/utils/named_interface.h> 
 
#include <qwidget.h> 
 
#include <string> 
 
/**  
 * The aim of a QWidgets_values_collector is to collect the values of 
 * all Valued_QWidgets rooted at "parent". 
 */ 
 
class GUI_DECL QWidgets_values_collector : public Named_interface { 
 public: 
  virtual ~QWidgets_values_collector() {} 
 
  /** Collects the values of all widgets whose parent is "parent". 
   * The function returns a string containing all the values. 
   * An optional title can be provided. 
   */ 
  virtual std::string widgets_values( QWidget* parent, 
				      const std::string& title ) = 0; 
   
  virtual void set_widgets_values( const std::string& values, 
				   QWidget* parent ) = 0; 
 
  virtual void clear_widgets_values( QWidget* parent ) = 0; 
 
  /** This function uses the Accessors provided by accessors_manager to 
   * read the widgets' values. The values are stored in string "values". 
   * This is a recursive function 
   */ 
  static void widgets_values( QWidget* parent, 
			      std::string& values, 
			      const std::string& accessors_manager ); 
 
  /** This function uses the Accessors provided by accessors_manager to 
   * clear the widgets' values. This is a recursive function 
   */ 
  static void clear_widgets_values( QWidget* parent, 
				    const std::string& accessors_manager ); 
}; 
 
 
/** This class collects the values of all widgets rooted at "parent" 
 * into an xml-formated string 
 */ 
class GUI_DECL QWidgets_values_collector_xml : public QWidgets_values_collector { 
 public: 
  static Named_interface* create_new_interface(std::string&); 
 
 public: 
  virtual ~QWidgets_values_collector_xml() {} 
  virtual std::string widgets_values( QWidget* parent, 
				      const std::string& title ); 
  virtual void set_widgets_values( const std::string& values, 
				   QWidget* parent ); 
  virtual void clear_widgets_values( QWidget* parent ); 
}; 
 
 
#endif 
