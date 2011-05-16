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

#ifndef __GSTLAPPLI_GSTL_QLISTVIEWITEM_H__ 
#define __GSTLAPPLI_GSTL_QLISTVIEWITEM_H__ 
 
 
#include <GsTLAppli/gui/common.h>
#include <qlistview.h> 
 
 
 
 
class GUI_DECL MultiSel_QListViewItem : public QListViewItem { 
 
 public: 
  MultiSel_QListViewItem( QListView* parent ); 
  MultiSel_QListViewItem( QListView* parent, QString label ); 
  MultiSel_QListViewItem( QListViewItem* parent ); 
  MultiSel_QListViewItem( QListViewItem* parent, QString label ); 
  virtual ~MultiSel_QListViewItem() {} 
 
  bool is_visible() const { return is_visible_;} 
  void switch_state(); 
  
  virtual void setSelected( bool ok );

  virtual int rtti() const { return MultiSel_QListViewItem::RTTI; } 
  static const int RTTI=211175; 
 
 protected: 
  bool is_visible_; 
}; 
 
 
 
 
class GUI_DECL SingleSel_QListViewItem : public QListViewItem { 
 
 public: 
  SingleSel_QListViewItem( QListView* parent ); 
  SingleSel_QListViewItem( QListView* parent, QString label ); 
  SingleSel_QListViewItem( QListViewItem* parent ); 
  SingleSel_QListViewItem( QListViewItem* parent, QString label ); 
  virtual ~SingleSel_QListViewItem() {} 
   
  bool is_visible() const { return is_visible_;} 
  virtual void switch_state(); 
  virtual void setSelected( bool ok );
 
  virtual int rtti() const { return SingleSel_QListViewItem::RTTI; } 
  static const int RTTI=211176; 
 
 protected: 
  bool is_visible_; 
   
  virtual void set_visible(); 
  virtual void set_unvisible(); 
}; 
 
 
 
 
#endif 
 
