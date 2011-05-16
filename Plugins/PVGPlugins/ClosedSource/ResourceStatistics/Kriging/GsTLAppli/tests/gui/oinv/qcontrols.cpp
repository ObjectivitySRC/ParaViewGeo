#include "qcontrols.h"
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/gui/oinv_description/oinv_slice.h>
#include <GsTLAppli/utils/gstl_messages.h>

Controls::Controls( SoGroup* root, Oinv_description* oinv_grid ) : QWidget( 0, 0 ) {
  oinv_grid_ = dynamic_cast<Oinv_strati_grid*>( oinv_grid ) ;
  appli_assert( oinv_grid );
  root_ = root;
  added_ = false;

  paint_= new QCheckBox(this);
  paint_->setText( "Paint property" );
  paint_->move( QPoint(10, 10) );
  QObject::connect( paint_, SIGNAL( toggled(bool) ),
		    this, SLOT( paint_property(bool) ) );
		    
  show_ = new QCheckBox(this);
  show_->setText( "Display" );
  show_->move( QPoint(10, 40) );
  QObject::connect( show_, SIGNAL( toggled(bool) ),
		    this, SLOT( show_grid(bool) ) );

  cmaps_ = new QComboBox( this );
  cmaps_->move( 10, 80 );
  cmaps_->insertItem( "rainbow" );
  cmaps_->insertItem( "cyan_red" );
  QObject::connect( cmaps_, SIGNAL( activated(int) ),
		    this, SLOT( change_cmap(int) ) );


  add_button_ = new QPushButton( this );
  add_button_->setText( "Add to Node" );
  add_button_->move( 10, 120 );
  QObject::connect( add_button_, SIGNAL( released() ),
		    this, SLOT( add_to_display() ) );

  new_Z_slice_ = new QPushButton( this );
  new_Z_slice_->setText( "New Z Slice" );
  new_Z_slice_->move( 10, 160 );
  QObject::connect( new_Z_slice_, SIGNAL( released() ),
		    this, SLOT( new_Z_slice() ) );

  slice_view_ = new QCheckBox(this);
  slice_view_->setText( "Slice View" );
  slice_view_->move( 10, 200 );
  QObject::connect( slice_view_, SIGNAL( toggled(bool) ),
		    this, SLOT( toggle_view(bool) ) );

  slice_pos_ = new QSlider( this );
  slice_pos_->setGeometry( 10, 230, 50, 20 );
  slice_pos_->setMinValue(0);
  slice_pos_->setMaxValue(0);
  slice_pos_->setOrientation( Qt::Horizontal );
  QObject::connect( slice_pos_, SIGNAL( valueChanged( int ) ),
		    this, SLOT( move_slice_to( int ) ) );

  slice_pos_label_ = new QLCDNumber(this);
  slice_pos_label_->setGeometry( 80, 230, 40, 40 );
  slice_pos_label_->display( 0 );
  QObject::connect( slice_pos_, SIGNAL( valueChanged( int ) ),
		    slice_pos_label_, SLOT( display( int ) ) );
    
  setGeometry( 300, 600, 300, 300 );

}

void Controls::paint_property( bool on ) {
  if( on )
    oinv_grid_->property_display_mode( Oinv::PAINTED );
  else
    oinv_grid_->property_display_mode( Oinv::NOT_PAINTED );
}

void Controls::show_grid( bool on ) {
  oinv_grid_->oinv_node()->visible = on;
}

void Controls::change_cmap( int map ) {
  std::string map_name( cmaps_->text(map).latin1() );
  oinv_grid_->set_colormap( map_name );
}

void Controls::add_to_display() {
  if( !added_ ) {
    root_->addChild( oinv_grid_->oinv_node() );
    added_ = true;
    add_button_->setEnabled( false );
  }
}


void Controls::new_Z_slice() {
  oinv_grid_->add_slice( Oinv::Z_AXIS , 0, true );
  Oinv_slice* slice = oinv_grid_->slice( 0 );
  slice->oinv_node()->visible = true;
  slice_pos_->setMaxValue( slice->max_position() );
}


void Controls::toggle_view( bool state ) {
  if( state ) 
    oinv_grid_->display_mode( Oinv_strati_grid::SLICES );
  else
    oinv_grid_->display_mode( Oinv_strati_grid::FULL );
}


void Controls::move_slice_to( int new_pos ) {
  Oinv_slice* slice = oinv_grid_->slice( 0 );
  int current_pos = slice->position();
  slice->translate( new_pos - current_pos );
  //slice->position( new_pos );
}
