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

#ifndef __GSTLAPPLI_GUI_APPLI_QPPLOT_CONTROL_PANEL_H__
#define __GSTLAPPLI_GUI_APPLI_QPPLOT_CONTROL_PANEL_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/appli/qpplot_control_panel_base.h>

#include <qstring.h>

class GsTL_project;
class GsTLGridProperty;
class PropertySelector;
class QPplot;

class GUI_DECL QPplot_control_panel : public QPplot_control_panel_base {

  Q_OBJECT

public:
  QPplot_control_panel( GsTL_project* proj,
                        QWidget* parent = 0, const char* name = 0 );
  ~QPplot_control_panel();
  void init( GsTL_project* project ); 
  virtual QSize sizeHint() const { return QSize( 200, 650 ); }

public slots:
  void set_var1_clipping_values( float low, float high );
  void set_var2_clipping_values( float low, float high );
  
protected slots:
  void forward_var1_changed( const QString& );
  void forward_var2_changed( const QString& );
  void forward_var1_low_clip_changed();
  void forward_var1_high_clip_changed();
  void forward_var2_low_clip_changed();
  void forward_var2_high_clip_changed();

signals:
  void var1_changed( const GsTLGridProperty* );
  void var2_changed( const GsTLGridProperty* );

  void var1_low_clip_changed( float );
  void var2_low_clip_changed( float );
  void var1_high_clip_changed( float );
  void var2_high_clip_changed( float );

  void reset_var1_clipping_values_clicked();
  void reset_var2_clipping_values_clicked();

  void analysis_type_changed( const QString& );


protected:
  PropertySelector* object_selector_var1_;
  PropertySelector* object_selector_var2_;

protected:
  GsTLGridProperty* get_property( const PropertySelector* selector );

};


#endif

