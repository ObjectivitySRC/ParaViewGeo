/**********************************************************************
** Author: Ting Li
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "filters" module of the Geostatistical Earth
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

/* -*- c++ -*- */

#ifndef __GSTLAPPLI_FILTERS_ECLOUTDLG_QT_DIALOGS_H__ 
#define __GSTLAPPLI_FILTERS_ECLOUTDLG_QT_DIALOGS_H__ 
 
#include <GsTLAppli/filters/common.h>

// "qt-designer" generated file 
#include <GsTLAppli/filters/eclipse/EclOutDlgBase.h> 

 
#include <qtextedit.h> 
#include <qcombobox.h> 
#include <qstring.h> 
#include <qwidget.h> 
#include <qlistbox.h>

#include <fstream> 
#include <map> 


class FILTERS_DECL EclOutDlg : public EclOutDlgBase { 
   
  Q_OBJECT 
 
 public: 
  EclOutDlg( QWidget* parent , const char* name = 0 ); 
  virtual ~EclOutDlg(); 

  void insertSrcItem (const std::string & i) { 
	  QString temp(i.c_str());
	  listSrc->insertStringList(temp); 
  }
  std::map<std::string,std::string> & getPairs() { return _pairs; }
    
 public slots: 

 protected slots:
   virtual void accept();
   void associate();

 protected: 
   std::map<std::string,std::string> _pairs;
}; 

 
 
#endif 
