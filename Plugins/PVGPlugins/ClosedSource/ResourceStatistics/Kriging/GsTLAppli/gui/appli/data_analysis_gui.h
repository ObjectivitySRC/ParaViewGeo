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

#ifndef __GSTLAPPLI_GUI_APPLI_DATA_ANALYSIS_GUI_H__
#define __GSTLAPPLI_GUI_APPLI_DATA_ANALYSIS_GUI_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/appli/data_analysis_gui_base.h>

#include <qstring.h>
#include <qlabel.h>
#include <qpainter.h>

#include <vector>


class QwtPlot;
class QLabel;
class QScrollView;
class QCloseEvent;
class SimplePs;

typedef std::vector< std::pair<std::string,std::string> > Stat;

class GUI_DECL Data_analysis_gui : public Data_analysis_gui_base {

  Q_OBJECT

public:
  void show_x_axis_options( bool on );
  void show_y_axis_options( bool on );
  void enable_x_log_scaling( bool on );
  void enable_y_log_scaling( bool on );

  void save_as_image( const QString& filename, const QString& format,
                      bool show_stats = true, bool show_grid = false );

public slots:
  void set_x_axis_min();
  void set_x_axis_max();
  void set_y_axis_min();
  void set_y_axis_max();
  void reset_x_axis();
  void reset_y_axis();

  virtual void set_x_axis_logscale( bool on );
  virtual void set_y_axis_logscale( bool on );

  virtual void close_window();
  virtual void save_as_image();

  //TL modified
  virtual void refresh_plot( long pcurve, std::pair<double*,double*>, int );
  virtual void refresh_plot(  std::pair<double*,double*>, int );

protected:
  Data_analysis_gui( QWidget* parent = 0, const char* name = 0 );

  virtual ~Data_analysis_gui();
  virtual void closeEvent( QCloseEvent* e );

  virtual void init_display_area();
  virtual void init_controls_area();
  virtual void init_scrollview();
  virtual void set_stats_labels( const QStringList& list, int columns );
  virtual void update_stats_value( unsigned int id, float val );
  virtual void paint_stats( QPainter& ) {}
  
  //TL modified
  virtual void savePostScript(SimplePs & );
  virtual void build_stats() {}

  /** Delete any dynamically allocated memory that has to be freed.
  */
  virtual void clean() = 0;

protected:
  QwtPlot* plot_;
  long curve_id_;

  //TL modified
  Stat _stats;

  std::vector<QLabel*> stats_labels_;

  QScrollView* scrollview_;
};


inline void 
Data_analysis_gui::update_stats_value( unsigned int id, float val ) {
  if( id >= stats_labels_.size() ) return;
  stats_labels_[id]->setNum( val );
}


#endif

