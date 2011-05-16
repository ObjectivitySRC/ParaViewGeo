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

#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/appli/display_pref_panel.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/gui/oinv_description/oinv_slice.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/gui/oinv_description/oinv_strati_grid.h>
#include <GsTLAppli/gui/oinv_description/oinv_pointset.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qwidget.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qstring.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qbrush.h>

#include <iterator>
#include <algorithm>


Qcolormap_bitmap::Qcolormap_bitmap( QWidget * parent, const char* name)
  : QWidget( parent, name ), cmap_( 0 ) {
  if( parent )  
    setGeometry( parent->geometry() );

  buffer_.resize( 10,10 );
  setBackgroundMode(NoBackground);
}

void Qcolormap_bitmap::draw_color_scale( const Colormap* cmap ) {
  /* This function paints the colorscale on the widget.
   * It paints a sequence of rectangles, each colored with a color from the
   * colormap. The final colormap should cover the whole length of the widget,
   * so we need to determine the width of each rectangle (as it depends on
   * the number of colors to be painted, and the size of the widget). The 
   * "difficulty" comes from the fact that the widths of the rectangles
   * has to be an integer. 
   */

  cmap_ = cmap;

  QPainter widget_painter;
  QPainter buffer_painter;

  widget_painter.begin( this );
  buffer_painter.begin( &buffer_ );
  widget_painter.setPen( Qt::black );
  buffer_painter.setPen( Qt::black );

  if( !cmap_ ) {
    // paint the whole widget in gray
    widget_painter.fillRect( 0,0,this->width(), this->height(), 
			     Qt::gray );
    buffer_painter.fillRect( 0,0,this->width(), this->height(), 
			     Qt::gray );

    widget_painter.end();
    buffer_painter.end();
    return;
  }

  // We don't want to paint more than 80 colors. Why 80?... why not?
  int nb_colors_to_paint = std::min( 80, cmap->color_scale()->colors_count() );


  // Find the size of the color rectangles (called colorbox_width) 
  int* colorbox_width = new int[nb_colors_to_paint];
  int base_width = this->width() / nb_colors_to_paint;
  int remaining_width = this->width() % nb_colors_to_paint;

  // distribute the remaining_width among the rectangles so that the sum 
  // of all rectangle width is equal to the widget width
  for( int l=0; l < nb_colors_to_paint ; l++ )
    colorbox_width[l]=base_width;

  while(remaining_width != 0) {
    int skip = nb_colors_to_paint / remaining_width;
    for( int l = 0 ; l < nb_colors_to_paint; l += skip ) {
      colorbox_width[l]++;
      remaining_width--;
    }
  }

  // We can now draw the colormap
  float min = cmap->lower_bound();
  float max = cmap->upper_bound();
  float skip = cmap->color_scale()->colors_count() / nb_colors_to_paint;
//  float skip = (max-min) / float(nb_colors_to_paint);
  const int y_cmap_start = 20;

  int widget_height = this->height();
  int already_painted_width = 0;
  for( int k = 0; k < nb_colors_to_paint ; k++ ) {
    float r,g,b;
    cmap->color_scale()->color(int(k*skip), r,g,b);
//    cmap->color(min+float(k)*skip, r,g,b);
    QBrush brush( QColor( int(255*r), int(255*g), int(255*b) ) );
    widget_painter.fillRect( already_painted_width, y_cmap_start,
			     colorbox_width[k], widget_height,
			     brush );
    buffer_painter.fillRect( already_painted_width, y_cmap_start,
			     colorbox_width[k], widget_height,
			     brush );
    already_painted_width += colorbox_width[k];
  }

  // draw tickmarks:
  widget_painter.fillRect( 0,0,this->width(), y_cmap_start, 
			   Qt::gray );
  buffer_painter.fillRect( 0,0,this->width(), y_cmap_start, 
			   Qt::gray );
  const int main_ticks = 4;
  const int tick_length = 5;
  const int intermediate_ticks = 4;
  const int intermediate_tick_length = 2;
  const int tick_y_end = y_cmap_start;
  const int spacing = 4;
  const int text_offset = 1;

  QFont painter_font( widget_painter.font() );
  painter_font.setPointSize( 7 );
  widget_painter.setFont( painter_font );
  buffer_painter.setFont( painter_font );

  int intertick_distance = 
    int( (this->width() -2 ) / ((intermediate_ticks+1)*(main_ticks+1) ) );
  int actual_nb_ticks = this->width() / intertick_distance;

  QString tickmark_value;

  widget_painter.drawLine( 0, tick_y_end,
			   width(), tick_y_end );
  buffer_painter.drawLine( 0, tick_y_end,
			   width(), tick_y_end );

  const int max_text_length = 6;

  for(int i=0; i<= actual_nb_ticks; i++) {
    int tick_x_pos = 1 + i*intertick_distance;

    int tick_y_begin = y_cmap_start - intermediate_tick_length;
    if( (i % (intermediate_ticks+1)) == 0 )
      tick_y_begin = y_cmap_start - tick_length;

    widget_painter.drawLine( tick_x_pos, tick_y_begin,
			     tick_x_pos, tick_y_end );
    buffer_painter.drawLine( tick_x_pos, tick_y_begin,
			     tick_x_pos, tick_y_end );

    if( (i % (intermediate_ticks+1)) == 0 ) {
      double val = min + i*(max - min)/ static_cast<double>(actual_nb_ticks) ;
      tickmark_value.setNum(val, 'g', 2);

      int max_char = std::min( max_text_length, (int) tickmark_value.length() );
      
      int actual_text_offset = text_offset;
      if( i != 0 )
	actual_text_offset = text_offset + 6;
      widget_painter.drawText(tick_x_pos-actual_text_offset, tick_y_begin - spacing,
			      tickmark_value, max_char);
      buffer_painter.drawText(tick_x_pos-actual_text_offset, tick_y_begin - spacing,
			      tickmark_value, max_char);
    }
  }

  widget_painter.drawLine( width() - 2, y_cmap_start - tick_length,
			   width() - 2, tick_y_end );
  buffer_painter.drawLine( width() - 2, y_cmap_start - tick_length,
			   width() - 2, tick_y_end );

  widget_painter.end();
  buffer_painter.end();
  delete [] colorbox_width;
}


void Qcolormap_bitmap::paintEvent(QPaintEvent *) { 
  bitBlt(this, 0,0, &buffer_);
}

void Qcolormap_bitmap::resizeEvent( QResizeEvent* event ) {
  buffer_.resize( event->size() );
  //  buffer_.fill( Qt::gray );
  draw_color_scale( cmap_ );
}







QTransparency_colormap_bitmap::
QTransparency_colormap_bitmap( QWidget * parent, const char * name ) 
: Qcolormap_bitmap( parent, name ) {

}


void QTransparency_colormap_bitmap::
draw_color_scale( const Colormap* cmap ) {
  cmap_ = cmap;
  if( !transparency_string_.isEmpty() )
    paint_transparent_intervals( transparency_string_ );
  else
    Qcolormap_bitmap::draw_color_scale( cmap );
}


void QTransparency_colormap_bitmap::
paint_transparent_intervals( const QString& str ) {

  QPainter widget_painter;
  QPainter buffer_painter;

  widget_painter.begin( this );
  buffer_painter.begin( &buffer_ );
  widget_painter.setPen( Qt::black );
  buffer_painter.setPen( Qt::black );

  if( !cmap_ ) {
    // paint the whole widget in gray
    widget_painter.fillRect( 0,0,this->width(), this->height(), 
			     Qt::gray );
    buffer_painter.fillRect( 0,0,this->width(), this->height(), 
			     Qt::gray );

    widget_painter.end();
    buffer_painter.end();
    return;
  }


  Qcolormap_bitmap::draw_color_scale( cmap_ );
  transparency_string_ = str;

  QBrush brush( Qt::lightGray );

  // We can now draw the colormap
  float min = cmap_->lower_bound();
  float max = cmap_->upper_bound();
  const int y_cmap_start = 20;

  int widget_height = this->height();
  int widget_width = this->width();

  QStringList intervals_str = QStringList::split( ";", str);
  QStringList::Iterator it = intervals_str.begin(); 
  for ( ; it != intervals_str.end(); ++it ) {
    QStringList interval_str = QStringList::split( ",", *it );
    if( interval_str.size() != 2 ) continue; 

    float low = std::max( interval_str[0].stripWhiteSpace().toFloat(), min );
    float high= std::min( interval_str[1].stripWhiteSpace().toFloat(), max );
    
    int a= (low  - min) / ( max - min ) * widget_width;
    int b= (high - min) / ( max - min ) * widget_width;

    widget_painter.fillRect( a, y_cmap_start,
		                  	     b-a, widget_height,
			                       brush );
    buffer_painter.fillRect( a, y_cmap_start,
		                  	     b-a, widget_height,
			                       brush );
  }

  widget_painter.end();
  buffer_painter.end();
}






//========================================

Slice_display_control::Slice_display_control( const QString& label, 
					      Oinv_slice* slice, int slice_id,
					      QWidget* parent, const char* name ) 
  : QWidget( parent, name ) {
  
  slice_ = slice;
  slice_id_ = slice_id;

  slice_display_toggle_ = new QCheckBox( label, this, "slice_display_toggle" );

  slice_pos_slider_ = new QSlider( Qt::Horizontal, this, "slice_pos_slider" );
  slice_pos_slider_->setMinimumWidth( 40 );
  slice_pos_slider_->setMaxValue( slice_->max_position() );
  if( slice_ )
    slice_pos_slider_->setMaxValue( slice_->max_position() );
  else 
    slice_pos_slider_->setMaxValue( 0 );

  slice_pos_spinbox_ = new QSpinBox( this, "slice_pos_spin" );
  slice_pos_spinbox_->setMaxValue( slice_->max_position() );
  if( slice_ )
    slice_pos_spinbox_->setMaxValue( slice_->max_position() );
  else 
    slice_pos_spinbox_->setMaxValue( 0 );
  
  remove_slice_button_ = new QPushButton( "Remove", this, "remove_button" );
  remove_slice_button_->setMaximumWidth( 50 );

  hlayout_ = new QHBoxLayout( this );
  hlayout_->addWidget( slice_display_toggle_ );
  hlayout_->addSpacing( 8 );
  hlayout_->addWidget( slice_pos_slider_ );
  hlayout_->addSpacing( 4 );
  hlayout_->addWidget( slice_pos_spinbox_ );
   hlayout_->addSpacing( 8 );
  hlayout_->addWidget( remove_slice_button_ );

  setMaximumWidth( 200 );

  QObject::connect( slice_pos_spinbox_, SIGNAL( valueChanged( int ) ),
		    slice_pos_slider_, SLOT( setValue( int ) ) );
  QObject::connect( slice_pos_slider_, SIGNAL( valueChanged( int ) ),
		    slice_pos_spinbox_, SLOT( setValue( int ) ) );
  

  QObject::connect( slice_display_toggle_, SIGNAL( toggled( bool ) ),
		    this, SLOT( show_slice( bool ) ) );
  QObject::connect( slice_pos_slider_, SIGNAL( valueChanged( int ) ),
		    this, SLOT( slice_position( int ) ) );
  QObject::connect( remove_slice_button_, SIGNAL( clicked() ),
		    this, SLOT( delete_slice() ) );
  
}



int Slice_display_control::slice_position() const {
  return slice_->position();
}

void Slice_display_control::show_slice( bool on ) {
  slice_->oinv_node()->visible = on;
}


void Slice_display_control::slice_position( int pos ) {
  slice_->position( pos );
}


void Slice_display_control::delete_slice() {
  emit removed( slice_id_ );
}






//================================================================

Display_pref_panel::Display_pref_panel( Oinv_description* oinv_desc,
					QWidget* parent, const char* name )
  : DisplayPrefPanelBase( parent, name ),
    oinv_desc_( oinv_desc ),
    oinv_strati_desc_( dynamic_cast<Oinv_strati_grid*>( oinv_desc_ ) ),
    slices_frame_( 0 ) {
  

  // If the current object can not be "sliced", do not show the slice 
  // preference panel
  if( !oinv_strati_desc_ ) {
    volume_explorer_prefs_->hide();
  }
  
  //!!!!!!!!!!!!!!
  // This control is currently not implemented, so we're hiding it...
  show_mesh_checkbox_->hide();
  //!!!!!!!!

  // If the object is not a point set, don't show some of the options
  if( dynamic_cast<Oinv_pointset*>( oinv_desc_ ) ) {
    show_mesh_checkbox_->hide();
  }
  else {
    point_size_label_->hide();
    point_size_spinbox_->hide();
  }

  // set up the property combobox
  const Geostat_grid* grid = oinv_desc_->described_grid();
  std::list<std::string> properties = grid->property_list();
  for( std::list<std::string>::const_iterator it = properties.begin();
       it != properties.end() ; ++it ) {
    property_selector_->insertItem( QString(it->c_str()) );
  }
  
  if( properties.empty() ) { 
    // disable the property options if no properties available 
    property_prefs_->setEnabled( false );
  }

  // set up the colormap combobox 
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( colormap_manager );
  Manager* cmap_manager = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( cmap_manager );
  
  for( Manager::interface_iterator cmap_it = cmap_manager->begin_interfaces();
       cmap_it != cmap_manager->end_interfaces(); ++cmap_it ) {
    QString cmap_name( cmap_manager->name( cmap_it->raw_ptr() ).c_str() );
    colormap_selector_->insertItem( cmap_name );
  }
  
  Qcolormap_bitmap* cmap_bitmap = new Qcolormap_bitmap( colormap_pix_,
							"cmap_bitmap");
  QObject::connect( this, SIGNAL( colormap_changed( const Colormap* ) ),
		    cmap_bitmap, SLOT( draw_color_scale( const Colormap* ) ) );


  QTransparency_colormap_bitmap* transparency_cmap_bitmap = 
    new QTransparency_colormap_bitmap( transparency_colormap_pix_,
                                       "transparency_cmap_bitmap");
  QObject::connect( this, SIGNAL( colormap_changed( const Colormap* ) ),
		    transparency_cmap_bitmap, SLOT( draw_color_scale( const Colormap* ) ) );
  QObject::connect( this, SIGNAL( transparency_changed( const QString& ) ),
            		    transparency_cmap_bitmap, 
                    SLOT( paint_transparent_intervals( const QString& ) ) );



  // find the name of the grid
  SmartPtr<Named_interface> grid_ni = 
    Root::instance()->interface( gridModels_manager );
  Manager* grid_manager = dynamic_cast<Manager*>( grid_ni.raw_ptr() );
  appli_assert( grid_manager );
  grid_name_ = grid_manager->name( (Named_interface*) grid ).c_str();



  // sync the widgets with the current state of the oinv description
  
  if( !properties.empty() ) {
    QString current_property = oinv_desc_->current_property().c_str();
    //    if( current_property.isEmpty() && !properties.empty() ) {
    if( current_property.isEmpty() ) {
      property_selector_->setCurrentItem( 0 );
      set_property( property_selector_->currentText() );
      cmap_bitmap->draw_color_scale( oinv_desc_->colormap() );
      transparency_cmap_bitmap->draw_color_scale( oinv_desc_->colormap() );
    }
    else {
      change_selected_property( current_property );
      cmap_bitmap->draw_color_scale( oinv_desc_->colormap() );
      transparency_cmap_bitmap->draw_color_scale( oinv_desc_->colormap() );
    }
    bool is_painted = oinv_desc_->property_display_mode();
    prop_painted_checkbox_->setChecked( is_painted );
  }
  
  else {
    // there are no properties in the grid. 
    cmap_bitmap->draw_color_scale( 0 );
    transparency_cmap_bitmap->draw_color_scale( 0 );
  }


 
  // set up widgets connections
  QObject::connect( BBox_checkbox_, SIGNAL( toggled( bool ) ),
		    this, SLOT( show_bbox( bool ) ) );
  QObject::connect( show_mesh_checkbox_, SIGNAL( toggled( bool ) ),
		    this, SLOT( show_mesh( bool ) ) );
  QObject::connect( point_size_spinbox_, SIGNAL( valueChanged( int ) ),
		    this, SLOT( change_point_size( int ) ) );
  QObject::connect( prop_painted_checkbox_, SIGNAL( clicked() ),
  		    this, SLOT( paint_property() ) );
  QObject::connect( property_selector_, SIGNAL( activated( const QString& ) ),
		    this, SLOT( set_property( const QString&  ) ) );
  QObject::connect( colormap_selector_, SIGNAL( activated( const QString& ) ),
		    this, SLOT( set_colormap( const QString&  ) ) );
  QObject::connect( min_cmap_value_edit_, SIGNAL( returnPressed() ),
		    this, SLOT( set_colormap_min_value() ) );
  QObject::connect( max_cmap_value_edit_, SIGNAL( returnPressed() ),
		    this, SLOT( set_colormap_max_value() ) );
  QObject::connect( reset_cmap_bounds_button_, SIGNAL( clicked() ),
		    this, SLOT( recompute_cmap_bounds() ) );
  QObject::connect( reset_cmap_bounds_button_, SIGNAL( clicked() ),
		    this, SLOT( recompute_cmap_bounds() ) );
  QObject::connect( volume_explorer_checkbox_, SIGNAL( toggled( bool ) ),
		    this, SLOT( enable_volume_explorer( bool ) ) );
  QObject::connect( transparency_lineedit_, SIGNAL( returnPressed() ),
		    this, SLOT( set_transparency() ) );
  QObject::connect( hide_volume_checkbox_, SIGNAL( toggled( bool ) ),
		    this, SLOT( hide_volume( bool ) ) );
  QObject::connect( add_slice_button_, SIGNAL( clicked() ),
		    this, SLOT( add_slice() ) );
  

  // If the object is a strati grid, show enable 3 slices by default
  if( oinv_strati_desc_ ) {
    add_slice( Oinv::X_AXIS );
    add_slice( Oinv::Y_AXIS );
    add_slice( Oinv::Z_AXIS );
  }

}



void Display_pref_panel::update() {
  if( !oinv_desc_ ) return ;

  //for( int i=0; i < property_selector_->count(); i++ )
  //  property_selector_->removeItem( i );
  property_selector_->clear();

  const Geostat_grid* grid = oinv_desc_->described_grid();
  std::list<std::string> properties = grid->property_list();

  if( properties.empty() ) {
    property_prefs_->setEnabled( false );
  }
  else {
    property_prefs_->setEnabled( true );
    for( std::list<std::string>::const_iterator it = properties.begin();
	 it != properties.end() ; ++it ) {
      property_selector_->insertItem( QString(it->c_str()) );
    }

    // sync the widgets with the current state of the oinv description
    QString current_property = oinv_desc_->current_property().c_str();
    if( current_property.isEmpty() ) {
      property_selector_->setCurrentItem( 0 );
      set_property( property_selector_->currentText() );
      emit colormap_changed( oinv_desc_->colormap() );
    }
    else {
      change_selected_property( current_property );
      emit colormap_changed( oinv_desc_->colormap() );
    }
    bool is_painted = oinv_desc_->property_display_mode();
    prop_painted_checkbox_->setChecked( is_painted );
  
  }
}


QSize Display_pref_panel::sizeHint() const {
  return QSize( 230, 500 );
}


void Display_pref_panel::show_bbox( bool on ) {
  appli_assert( oinv_strati_desc_ );
  oinv_strati_desc_->show_bounding_box( on );
}

void Display_pref_panel::show_mesh( bool  ) {
  appli_warning( "show_mesh not yet implemented" );
}

/*
void Display_pref_panel::paint_property( bool on ) {
  if( on )
    oinv_desc_->property_display_mode( Oinv::PAINTED );
  else
    oinv_desc_->property_display_mode( Oinv::NOT_PAINTED );

  // emit the "property_painted_toggled" signal
  emit property_painted_toggled( grid_name_, property_selector_->currentText() );

}
*/

void Display_pref_panel::paint_property() {
  bool on = prop_painted_checkbox_->isChecked();
  if( on )
    oinv_desc_->property_display_mode( Oinv::PAINTED );
  else
    oinv_desc_->property_display_mode( Oinv::NOT_PAINTED );

  // emit the "property_painted_toggled" signal
  emit property_painted_toggled( grid_name_, property_selector_->currentText() );

}

void Display_pref_panel::change_point_size( int size ) {
  Oinv_pointset* oinv_pset = dynamic_cast<Oinv_pointset*>( oinv_desc_ );
  if( oinv_pset )
    oinv_pset->point_size( size );
}



void Display_pref_panel::set_property( const QString& prop ) {
  oinv_desc_->set_property( std::string( prop.latin1() ) );
  
  std::string cmap_name;

  // We now look for the name of the colorscale used to paint prop:
  if( oinv_desc_->colormap() ) {
    SmartPtr<Named_interface> ni = 
      Root::instance()->interface( colormap_manager );
    Manager* cmap_manager = dynamic_cast<Manager*>( ni.raw_ptr() );
    appli_assert( cmap_manager );
    cmap_name = cmap_manager->name( oinv_desc_->colormap()->color_scale() );

    QString value;
    value.setNum( oinv_desc_->colormap()->lower_bound() );
    min_cmap_value_edit_->setText( value );
    value.setNum( oinv_desc_->colormap()->upper_bound() );
    max_cmap_value_edit_->setText( value );
    
  }

  //appli_assert( !cmap_name.empty() );
  QString qcmap_name( cmap_name.c_str() );
  int i=0;
  for( ; i < colormap_selector_->count() ; i++ ) {
    if( colormap_selector_->text(i) == qcmap_name ) {
      break;
    }
  }
  colormap_selector_->setCurrentItem( i );
  
  // Emit the "displayed property changed" signal
  if( prop_painted_checkbox_->isChecked() )
    emit displayed_property_changed( grid_name_, prop );

  // Emit the colormap_changed signal
  appli_assert( oinv_desc_->colormap() );
  emit colormap_changed( oinv_desc_->colormap() );


  // update the transparency lineedit
  if( oinv_strati_desc_ ) {
    QString intervals = 
      oinv_strati_desc_->transparency_intervals( std::string(prop.ascii()) ); 
    transparency_lineedit_->setText( intervals );
    set_transparency();
  }
}


void Display_pref_panel::set_colormap( const QString& map_name ) {
  oinv_desc_->set_colormap( std::string( map_name.latin1() ) );
  const Colormap* cmap = oinv_desc_->colormap();
  if( !cmap ) return;

  QString min_str, max_str;
  min_str.setNum( cmap->lower_bound() );
  max_str.setNum( cmap->upper_bound() );
  min_cmap_value_edit_->setText( min_str );
  max_cmap_value_edit_->setText( max_str );
  
  emit colormap_changed( oinv_desc_->colormap() );
}


void Display_pref_panel::set_colormap_min_value() {
  QString min_str = min_cmap_value_edit_->text();
  float min = min_str.toFloat();
  set_colormap_min_value( min );
/*
  Colormap new_map( *(oinv_desc_->colormap()) );
  if( min > new_map.upper_bound() ) {
    min = new_map.upper_bound();
    min_cmap_value_edit_->setText( min_str.setNum( min ) );
  }

  new_map.lower_bound( min );
  oinv_desc_->set_colormap( new_map );

  emit colormap_changed( oinv_desc_->colormap() );
*/
}

void Display_pref_panel::set_colormap_max_value() {
  QString max_str = max_cmap_value_edit_->text();
  float max = max_str.toFloat();
  set_colormap_max_value( max );
/*
  Colormap new_map( *(oinv_desc_->colormap()) );
  if( max < new_map.lower_bound() ) {
    max = new_map.lower_bound();
    max_cmap_value_edit_->setText( max_str.setNum( max ) );
  }
  
  new_map.upper_bound( max );
  oinv_desc_->set_colormap( new_map );

  emit colormap_changed( oinv_desc_->colormap() );
*/
}


void Display_pref_panel::set_colormap_min_value( float min ) {
  Colormap new_map( *(oinv_desc_->colormap()) );
  if( min > new_map.upper_bound() ) {
    min = new_map.upper_bound();
    QString min_str;
    min_cmap_value_edit_->setText( min_str.setNum( min ) );
  }

  new_map.lower_bound( min );
  oinv_desc_->set_colormap( new_map );

  emit colormap_changed( oinv_desc_->colormap() );
}

void Display_pref_panel::set_colormap_max_value( float max ) {
  Colormap new_map( *(oinv_desc_->colormap()) );
  if( max < new_map.lower_bound() ) {
    max = new_map.lower_bound();
    QString max_str;
    max_cmap_value_edit_->setText( max_str.setNum( max ) );
  }
  
  new_map.upper_bound( max );
  oinv_desc_->set_colormap( new_map );

  emit colormap_changed( oinv_desc_->colormap() );
}


void Display_pref_panel::recompute_cmap_bounds() {
  oinv_desc_->reset_colormap_bounds();
  const Colormap* cmap = oinv_desc_->colormap();
  
  if( !cmap ) return;

  QString min_str, max_str;
  min_str.setNum( cmap->lower_bound() );
  max_str.setNum( cmap->upper_bound() );
  min_cmap_value_edit_->setText( min_str );
  max_cmap_value_edit_->setText( max_str );

  emit colormap_changed( oinv_desc_->colormap() );
}



void Display_pref_panel::hide_volume( bool on ) {
  appli_assert( oinv_strati_desc_ );
  oinv_strati_desc_->hide_volume( on );
}


void Display_pref_panel::set_transparency( ) {
  appli_assert( oinv_strati_desc_ );
  oinv_strati_desc_->set_transparency( transparency_lineedit_->text() );

  emit transparency_changed( transparency_lineedit_->text() );
}



void Display_pref_panel::enable_volume_explorer( bool on ) {
  if( on )
    oinv_strati_desc_->display_mode( Oinv_strati_grid::RENDERING );
  else
    oinv_strati_desc_->display_mode( Oinv_strati_grid::FULL );      
}



void Display_pref_panel::show_slices( bool on ) {
  if( on )
    oinv_strati_desc_->display_mode( Oinv_strati_grid::SLICES );
  else
    oinv_strati_desc_->display_mode( Oinv_strati_grid::FULL );
      
}

/*
void Display_pref_panel::add_slice() {
  if( !slices_frame_ ) {
    slices_frame_ = new QVBox( volume_explorer_prefs_, "slices_layout" );
    slices_frame_->setFrameStyle( QFrame::Box | QFrame::Sunken );
    slices_frame_->setSpacing( 8 );
    slices_frame_->setMargin( 4 );
    
    QHBoxLayout* slices_frame_layout_ =
      new QHBoxLayout( volume_explorer_prefs_, 0, -1, "slices_frame_layout" );
    slices_frame_layout_->addStretch( 0 );
    slices_frame_layout_->addWidget( slices_frame_ );
    slices_frame_layout_->addStretch( 0 );
    
    //    slices_prefs_->layout()->add( slices_frame_ );
    volume_explorer_prefs_->layout()->addItem( slices_frame_layout_ );
  }
  int axis_int = slice_axis_selector_->currentItem();
  Oinv::Axis axis;
  QString slice_label;

  switch( axis_int ) {
  case 0:
    axis = Oinv::X_AXIS;
    slice_label = "X";
    break;
  case 1:
    axis = Oinv::Y_AXIS;
    slice_label = "Y";
    break;
  case 2:
    axis = Oinv::Z_AXIS;
    slice_label = "Z";
    break;
  }

  int slice_id = oinv_strati_desc_->add_slice( axis, 0, false );
  QString slice_name;
  slice_name = slice_name.setNum( slice_id );

  Slice_display_control* new_control = 
    new Slice_display_control( slice_label, 
			       oinv_strati_desc_->slice( slice_id ), slice_id,
			       slices_frame_, slice_name.latin1() );

  slice_controls_list_.push_back( new_control );

  if( !slices_frame_->isVisible() )
    slices_frame_->show();

  QObject::connect( new_control, SIGNAL( removed( int ) ),
		    this, SLOT( remove_slice( int ) ) );
  new_control->show();
}
*/



void Display_pref_panel::add_slice() {
  int axis_int = slice_axis_selector_->currentItem();
  Oinv::Axis axis;

  switch( axis_int ) {
  case 0:
    axis = Oinv::X_AXIS;
    break;
  case 1:
    axis = Oinv::Y_AXIS;
    break;
  case 2:
    axis = Oinv::Z_AXIS;
    break;
  }

  add_slice( axis );
}


void Display_pref_panel::add_slice( Oinv::Axis axis ) {
  if( !slices_frame_ ) {
    slices_frame_ = new QVBox( volume_explorer_prefs_, "slices_layout" );
    slices_frame_->setFrameStyle( QFrame::Box | QFrame::Sunken );
    slices_frame_->setSpacing( 8 );
    slices_frame_->setMargin( 4 );
    
    QHBoxLayout* slices_frame_layout_ =
      new QHBoxLayout( volume_explorer_prefs_, 0, -1, "slices_frame_layout" );
    slices_frame_layout_->addStretch( 0 );
    slices_frame_layout_->addWidget( slices_frame_ );
    slices_frame_layout_->addStretch( 0 );
    
    //    slices_prefs_->layout()->add( slices_frame_ );
    volume_explorer_prefs_->layout()->addItem( slices_frame_layout_ );
  }
  QString slice_label;

  switch( axis ) {
  case 0:
    slice_label = "X";
    break;
  case 1:
    slice_label = "Y";
    break;
  case 2:
    slice_label = "Z";
    break;
  }

  int slice_id = oinv_strati_desc_->add_slice( axis, 0, false );
  QString slice_name;
  slice_name = slice_name.setNum( slice_id );

  Slice_display_control* new_control = 
    new Slice_display_control( slice_label, 
			       oinv_strati_desc_->slice( slice_id ), slice_id,
			       slices_frame_, slice_name.latin1() );

  slice_controls_list_.push_back( new_control );

  if( !slices_frame_->isVisible() )
    slices_frame_->show();

  QObject::connect( new_control, SIGNAL( removed( int ) ),
		    this, SLOT( remove_slice( int ) ) );
  new_control->show();
}


void Display_pref_panel::remove_slice( int slice_id ) {
  appli_assert( slice_id >=0 && slice_id < int( slice_controls_list_.size() ) );
  std::list<Slice_display_control*>::iterator it = slice_controls_list_.begin();
  std::advance( it, slice_id );

  std::list<Slice_display_control*>::iterator next = it;
  next++;

  delete *it;
  slice_controls_list_.erase( it );
  

  oinv_strati_desc_->remove_slice( slice_id );

  // update the slice ids
  for( ; next != slice_controls_list_.end(); ++next )
    (*next)->slice_id( slice_id++ );

  if( slice_controls_list_.empty() )
    slices_frame_->hide();
}


void Display_pref_panel::toggle_paint_property( bool on ) {
  prop_painted_checkbox_->setChecked( on );

}



void Display_pref_panel::change_selected_property( const QString& prop ) {
  int i=0;
  for( ; i < property_selector_->count() ; i++ ) {
    if( property_selector_->text( i ) == prop )
      break;
  }
  property_selector_->setCurrentItem( i );


  std::string cmap_name;

  // We now look for the name of the colorscale used to paint prop:
  if( oinv_desc_->colormap() ) {
    SmartPtr<Named_interface> ni = 
      Root::instance()->interface( colormap_manager );
    Manager* cmap_manager = dynamic_cast<Manager*>( ni.raw_ptr() );
    appli_assert( cmap_manager );
    cmap_name = cmap_manager->name( oinv_desc_->colormap()->color_scale() );

    QString value;
    value.setNum( oinv_desc_->colormap()->lower_bound() );
    min_cmap_value_edit_->setText( value );
    value.setNum( oinv_desc_->colormap()->upper_bound() );
    max_cmap_value_edit_->setText( value );
    
  }

  appli_assert( !cmap_name.empty() );
  QString qcmap_name( cmap_name.c_str() );
  i=0;
  for( ; i < colormap_selector_->count() ; i++ ) {
    if( colormap_selector_->text(i) == qcmap_name ) {
      break;
    }
  }
  colormap_selector_->setCurrentItem( i );
  
}


