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

#ifndef __GSTLAPPLI_GUI_APPLI_HISTOGRAM_CONTROL_PANEL_H__
#define __GSTLAPPLI_GUI_APPLI_HISTOGRAM_CONTROL_PANEL_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/appli/histogram_control_panel_base.h>

// TL modified
#include <qcheckbox.h>
#include <qcombobox.h>

class GsTL_project;
class GsTLGridProperty;
class GridSelector;
class PropertySelector;
 
//TL modified
class QComboBox;

class GUI_DECL Histogram_control_panel : public Histogram_control_panel_base {

  Q_OBJECT

public:
  Histogram_control_panel( GsTL_project* proj,
                           QWidget* parent = 0, const char* name = 0 );
  ~Histogram_control_panel();

  void init( GsTL_project* project ); 
  virtual QSize sizeHint() const { return QSize( 190, 300 ); }
  int bins_count() const;

  //TL modified
  QString whatPlot() { return _combo->currentText(); }

public slots:
  void set_clipping_values( float low, float high );
  
protected slots:
  void forward_var_changed( const QString& );
  void forward_low_clip_changed();
  void forward_high_clip_changed();
  
signals:
  // TL modified
  void comboChanged(const QString &);

  void var_changed( const GsTLGridProperty* );

  void low_clip_changed( float );
  void high_clip_changed( float );
  void reset_clipping_values_clicked();

  void bins_count_changed( int );

protected:  
  PropertySelector* object_selector_;

  //TL modified
  QComboBox * _combo;

protected:
  GsTLGridProperty* get_property( const PropertySelector* );

};

#endif

