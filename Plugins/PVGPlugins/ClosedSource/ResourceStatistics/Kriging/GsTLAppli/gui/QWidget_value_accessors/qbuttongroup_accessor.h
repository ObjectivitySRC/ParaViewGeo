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

#ifndef __GSTLAPPLI_QBUTTONGROUP_WRAPPER_H__ 
#define __GSTLAPPLI_QBUTTONGROUP_WRAPPER_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/qwidget_value_accessor.h> 
 
#include <qbuttongroup.h> 
 
#include <string> 
 
 
class GUI_DECL QButtonGroup_accessor : public QWidget_value_accessor { 
 public: 
  static Named_interface* create_new_interface(std::string&); 
 
 public: 
  QButtonGroup_accessor( QWidget* widget = 0 ); 
  virtual ~QButtonGroup_accessor() {} 
 
  virtual bool initialize( QWidget* widget = 0 ); 
  virtual bool is_valid() const { return group_ != 0; } 
  virtual std::string value() const; 
  virtual bool set_value( const std::string& str ); 
  virtual void clear(); 
     
 private: 
  QButtonGroup* group_; 
 
}; 
 
 
 
#endif  
