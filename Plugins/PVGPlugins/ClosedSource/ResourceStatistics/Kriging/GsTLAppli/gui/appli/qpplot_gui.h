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

#ifndef __GSTLAPPLI_GUI_APPLI_QPPLOT_GUI_H__
#define __GSTLAPPLI_GUI_APPLI_QPPLOT_GUI_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/appli/data_analysis_gui.h>

#include <qscrollview.h>

class GsTL_project;
class GsTLGridProperty;
class QPplot_control_panel;
class QLabel;
class QPplot;


class GUI_DECL QPplot_gui : public Data_analysis_gui {

  Q_OBJECT

public:
  QPplot_gui( GsTL_project* project,
              QWidget* parent = 0, const char* name = 0 );
  ~QPplot_gui();

public slots:
  void update_var1_low_clip( float );
  void update_var1_high_clip( float );
  void update_var2_low_clip( float );
  void update_var2_high_clip( float );
  void reset_var1_clipping_values();
  void reset_var2_clipping_values();
  void get_var1_data_from( const GsTLGridProperty* );
  void get_var2_data_from( const GsTLGridProperty* );
  void set_analysis_type( const QString& type );

protected:
  virtual void clean();
  void update_all();
  void refresh_stats();
  virtual void paint_stats( QPainter& );

  //TL modified
  void build_stats();
//  GsTLGridProperty* get_property( const PropertySelector* );

protected:
  QPplot_control_panel* control_panel_;

  QPplot* qpploter_; 

};




/*
class GUI_DECL GsTLScrollView : public QScrollView {

  Q_OBJECT

public:
  GsTLScrollView( GsTL_project* project,
                  QWidget* parent = 0, const char* name = 0 );

  QPplot_control_panel* panel() { return panel_; }

private slots:
  void resz() {
    resize( 200, 500 );
  }

private:
  QPplot_control_panel* panel_;
};
*/

#endif