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

#ifndef __GSTLAPPLI_GUI_DISPLAY_PREFERENCES_PANEL_H__ 
#define __GSTLAPPLI_GUI_DISPLAY_PREFERENCES_PANEL_H__ 
 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/oinv_description/oinv_common.h>
#include <GsTLAppli/gui/appli/display_pref_panel_base.h> 
 
#include <qwidget.h> 
#include <qpixmap.h> 
 
#include <list> 
 
class Oinv_description; 
class Oinv_strati_grid; 
class Oinv_slice; 
class Colormap; 
 
class QString; 
class QCheckBox; 
class QSlider; 
class QSpinBox; 
class QPushButton; 
class QHBoxLayout; 
class QVBox; 
 
 
/** Qcolormap_bitmap is a simple widget which shows the colors of a colormap. 
 */ 
class GUI_DECL Qcolormap_bitmap : public QWidget { 
 
  Q_OBJECT 
 
 public: 
  Qcolormap_bitmap( QWidget * parent = 0, const char * name = 0 ); 
   
 public slots: 
   /** This function paints a colorscale (specified by colormap \c cmap) on the widget. 
   * It paints a sequence of rectangles, each colored with a color from the 
   * colormap.  
   */ 
  virtual void draw_color_scale( const Colormap* cmap ); 
 
 protected: 
  virtual void paintEvent( QPaintEvent * ); 
  virtual void resizeEvent( QResizeEvent * ); 
 
 protected: 
  QPixmap buffer_; 
  const Colormap* cmap_; 
}; 
 
 
/** QTransparency_colormap_bitmap is a simple widget which shows the colors of 
 * a colormap and transparent intervals. 
 */ 
class GUI_DECL QTransparency_colormap_bitmap : public Qcolormap_bitmap { 
 
  Q_OBJECT 
 
 public: 
  virtual void draw_color_scale( const Colormap* cmap ); 
  QTransparency_colormap_bitmap( QWidget * parent = 0, const char * name = 0 ); 
   
 public slots: 
  void paint_transparent_intervals( const QString& str );
    
 protected:
   QString transparency_string_;
}; 
 
 
 
 
 
class GUI_DECL Slice_display_control : public QWidget { 
 
  Q_OBJECT 
 
 public: 
  Slice_display_control( const QString& label, 
			 Oinv_slice* slice, int slice_id, 
			 QWidget* parent = 0, const char* name = 0 ); 
 
  int slice_position() const; 
  void slice_id( int id ) { slice_id_ = id; } 
 
 public slots: 
  void show_slice( bool on ); 
  void slice_position( int pos ); 
  void delete_slice(); 
 
 signals: 
  void removed( int ); 
   
 
 private: 
  Oinv_slice* slice_; 
  int slice_id_; 
   
 private: 
  QCheckBox* slice_display_toggle_; 
  QSlider* slice_pos_slider_; 
  QSpinBox* slice_pos_spinbox_; 
  QPushButton* remove_slice_button_; 
  
  QHBoxLayout* hlayout_; 
 
}; 
 
 
 
//============================================= 
 
class GUI_DECL Display_pref_panel : public DisplayPrefPanelBase { 
 
  Q_OBJECT 
 
 public: 
  Display_pref_panel( Oinv_description* oinv_desc, 
		      QWidget* parent, const char* name ); 
   
  ~Display_pref_panel() {}
 
  /** Updates the panel if a property, a region, etc, 
   * has been added (or removed) to the grid 
   */ 
  void update(); 
 
  virtual QSize sizeHint() const; 
 

 public slots: 
  void show_bbox( bool on ); 
  void show_mesh( bool on ); 
  //  void paint_property( bool on ); 
  void paint_property(); 
  void set_property( const QString& prop ); 
  void set_colormap( const QString& prop ); 
  void set_colormap_min_value(); 
  void set_colormap_max_value(); 
  void set_colormap_min_value( float min ); 
  void set_colormap_max_value( float max ); 
  void recompute_cmap_bounds(); 
  void enable_volume_explorer( bool on );
  void hide_volume( bool on );
  void set_transparency(); 
  void show_slices( bool on ); 
  void add_slice(); 
  void add_slice( Oinv::Axis axis ); 
  void remove_slice( int slice_id ); 
  void toggle_paint_property( bool on ); 
  void change_selected_property( const QString& prop ); 
  void change_point_size( int size ); 
 
 
 signals: 
  void displayed_property_changed( const QString& grid,  
				   const QString& prop ); 
  void property_painted_toggled( const QString& grid, 
				 const QString& prop ); 
  void colormap_changed( const Colormap* ); 
  void transparency_changed( const QString& ); 
 
 private: 
  Oinv_description* oinv_desc_; 
  Oinv_strati_grid* oinv_strati_desc_; 
  QString grid_name_; 
 
 private: 
  QVBox* slices_frame_; 
  std::list<Slice_display_control*> slice_controls_list_; 
 
}; 
 
 
 
#endif 
