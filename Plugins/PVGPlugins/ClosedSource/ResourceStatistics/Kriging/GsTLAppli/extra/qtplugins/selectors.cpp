/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Geostatistical Earth
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

#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <qstring.h>
//#include <qvariant.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qwidgetfactory.h>
#include <qobjectlist.h>
#include <qlabel.h>
#include <qevent.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qvbox.h>

#include <algorithm>


const QString GridSelector::no_selection( "<- None ->" );
 

GridSelector::GridSelector( QWidget* parent, const char* name,
			    GsTL_project* project)
  : QComboBox( parent, name ),
    Project_view( project ) {
/*
  // register the widget with the Project
  if( project ) {
    appli_warning( "project is not 0" );
    Project_view::init( project );
  }
*/
  init( project_ );

  // the combobox is not editable
  setEditable( false );

  insertItem( no_selection, 0 );
}


GridSelector::~GridSelector() {
  // This object might be deleted twice:
  //   - QT automatically deletes all the children of a deleted widget
  //   - the Project keep a smart pointer to "this" and if the number of
  //     references to "this" falls to 0, "this" is deleted
  // To make sure the smart pointer deletion won't happen:
  // (This line might not be needed...)
  //new_ref();
}

void GridSelector::setCurrentText( const QString& text ) {
  QComboBox::setCurrentText( text );
  emit activated( text );
}

void GridSelector::init( GsTL_project* project ) {
  if( project ) {
    Project_view::init( project );

    // if there already is an item selected, save it before initializing 
    // the combobox. If that item is a valid element, it will be selected
//    QString current_text = currentText();
    
    // erase all existing elements
//    clear();

    // Add all the names of all loaded grids to the combobox
    GsTL_project::String_list obj_list = project->objects_list() ;
    GsTL_project::String_list::iterator begin = obj_list.begin();
    GsTL_project::String_list::iterator end = obj_list.end();
    
    for( ; begin != end ; ++begin ) {
      QString name( begin->c_str() );
      insertItem( name );
    }

//    if( std::find( obj_list.begin(), obj_list.end(), 
//      std::string( current_text.latin1() ) ) != obj_list.end() ) {
//      setCurrentText( current_text );
//      emit activated( current_text );
//    }
  }
}


void GridSelector::update( std::string ) {
  // The current grid might have gotten new properties
  // emit the activated signal for the current item so that
  // if someone listens to that signal he gets a chance to know 
  // something has changed
  emit activated( currentItem() );
  emit activated( currentText() );
}

void GridSelector::new_object( std::string obj ) {
  QString new_obj( obj.c_str() );
  insertItem( new_obj );

  emit new_object_available( new_obj );
}

void GridSelector::deleted_object( std::string obj ) {
  QString deleted( obj.c_str() );
  
  int current_item = currentItem();
  // find element "deleted" in the combobox and remove it
  int i = 0;
  while( text( i ) != deleted ) {
    i++;
  }
/* the code in comment is replaced by the following uncommented code.
 * I keep it there until I'm sure the new code works
  removeItem( i );

  if( current_item > i ) current_item--;

  setCurrentItem( current_item );
  emit activated( currentText() );
*/

  //----------------------
  // new code
  removeItem( i );

  if( i == current_item ) {
    current_item--;
    setCurrentItem( current_item );
    emit activated( currentText() );
    return;
  }
  
  if( current_item > i ) current_item--;
  setCurrentItem( current_item );
  //----------------------

}





//===============================================


SinglePropertySelector::SinglePropertySelector( QWidget* parent, const char* name )
  : QComboBox( parent, name ) {
}

void SinglePropertySelector::show_properties( const QString& grid_name ) {
  QString current_text;
  if( grid_name == current_grid_ )
    current_text = currentText();  
  
  current_grid_ = grid_name;
  QComboBox::clear();

  if( grid_name.isEmpty() || grid_name == GridSelector::no_selection ) return;
  

  std::string name( grid_name.latin1() );

  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + name );
  Geostat_grid* grid = (Geostat_grid*) ni.raw_ptr() ;
  // no dynamic_cast is used on the previous line because it makes 
  // designer and uic segfault when they try to load the plugin...
  // It used to work before, don't know what changed.  

  if( !grid ) return;

  int selected_item = 0;
  int i=0;
  std::list<std::string> properties = grid->property_list();
  std::list<std::string>::iterator begin = properties.begin();
  for( ; begin != properties.end() ; ++begin, i++ ) {
    QString prop_name( begin->c_str() );
    insertItem( prop_name );

    if( current_text == prop_name ) selected_item = i;
  }

  // a dirty hack to have the combobox resize itself to the size of 
  // its longest element
  setFont( font() );
  updateGeometry();
  
  setCurrentItem( selected_item );


  emit activated( currentText() );
  // Don't know why "0". Should be "currentItem". remove if everything works fine
    emit activated( 0 );
//  emit activated( currentItem() );
}





//===============================================

MultiPropertySelector::MultiPropertySelector( QWidget* parent, const char* name )
  : QListBox( parent, name ) {
  setSelectionMode( QListBox::Extended );

  QObject::connect( this, SIGNAL( selectionChanged() ),
		    this, SLOT( selection_size() ) );
}

void MultiPropertySelector::show_properties( const QString& grid_name ) {
  QListBox::clear();

  if( grid_name.isEmpty() || grid_name == GridSelector::no_selection ) return;
  

  std::string name( grid_name.latin1() );

  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + name );
  Geostat_grid* grid = (Geostat_grid*) ni.raw_ptr() ;
  // no dynamic_cast is used on the previous line because it makes 
  // designer and uic segfault when they try to load the plugin...
  // It used to work before, don't know what changed.  

  if( !grid ) return;

  std::list<std::string> properties = grid->property_list();
  std::list<std::string>::iterator begin = properties.begin();
  for( ; begin != properties.end() ; ++begin ) {
    QString prop_name( begin->c_str() );
    insertItem( prop_name );
  }
  
}


void MultiPropertySelector::selection_size() {
  int size=0;
  for( unsigned int i = 0; i < count() ; i++ ) {
    if( isSelected( i ) && !text(i).isEmpty() ) {
      size++;
    }
  }
  emit( selected_count(size) );
}


//===============================================

PropertySelector::PropertySelector( QWidget* parent,
				    const char* name,
				    GsTL_project* project )
  : QWidget( parent, name ) {
   
  //--------------------
  // set up the widget
  resize( 217, 104 ); 
  setCaption( trUtf8( "GridProperty_selector" ) );
  Selector_uiLayout = new QHBoxLayout( this, 4, 2, "Selector_uiLayout"); 

  Layout2 = new QVBoxLayout( 0, 0, 6, "Layout2"); 

  GroupBox4 = new QGroupBox( this, "GroupBox4" );
  GroupBox4->setFrameShape( QGroupBox::NoFrame );
  GroupBox4->setFrameShadow( QGroupBox::Plain );
  GroupBox4->setTitle( trUtf8( "Object" ) );
  GroupBox4->setColumnLayout(0, Qt::Vertical );
  GroupBox4->layout()->setSpacing( 2 );
  GroupBox4->layout()->setMargin( 4 );
  GroupBox4Layout = new QHBoxLayout( GroupBox4->layout() );
  GroupBox4Layout->setAlignment( Qt::AlignTop );

  object_selector_ = new GridSelector( GroupBox4, "object_selector_" );
  GroupBox4Layout->addWidget( object_selector_ );
  Layout2->addWidget( GroupBox4 );

  GroupBox5 = new QGroupBox( this, "GroupBox5" );
  GroupBox5->setFrameShape( QGroupBox::NoFrame );
  GroupBox5->setFrameShadow( QGroupBox::Plain );
  GroupBox5->setTitle( trUtf8( "Property" ) );
  GroupBox5->setColumnLayout(0, Qt::Vertical );
  GroupBox5->layout()->setSpacing( 2 );
  GroupBox5->layout()->setMargin( 4 );
  GroupBox5Layout = new QHBoxLayout( GroupBox5->layout() );
  GroupBox5Layout->setAlignment( Qt::AlignTop );

//  property_selector_ = new QComboBox( FALSE, GroupBox5, "property_selector_" );
  property_selector_ = 
    new SinglePropertySelector( GroupBox5, "property_selector_" );
  GroupBox5Layout->addWidget( property_selector_ );
  Layout2->addWidget( GroupBox5 );
  Selector_uiLayout->addLayout( Layout2 );
  //--------------------


  object_selector_->init( project );
/*
  QObject::connect( 
		   object_selector_, 
		   SIGNAL( activated( const QString& ) ),
		   this, 
		   SLOT( update_properties( const QString& ) )
		   );
*/
  QObject::connect( 
		   object_selector_, 
		   SIGNAL( activated( const QString& ) ),
		   property_selector_, 
		   SLOT( show_properties( const QString& ) )
		   );


  QObject::connect( this, 
		   SIGNAL( object_changed( const QString& ) ),
		   this, 
		   SLOT( forward_property_changed() )
		   );

  // Forward some signals
  QObject::connect( object_selector_, 
		    SIGNAL( activated( const QString& ) ),
		    SIGNAL( object_changed( const QString& ) ) );
  QObject::connect( property_selector_,
		    SIGNAL( activated( const QString& ) ),
		    SIGNAL( property_changed( const QString&  ) ) );
  QObject::connect( object_selector_, 
		    SIGNAL( new_object_available( const QString&  ) ),
		    SIGNAL( new_object_available( const QString&  ) ) );
  
}


void PropertySelector::forward_property_changed() {
  emit property_changed( selectedProperty() );
}
/*
void PropertySelector::update_properties( const QString& obj ) {

  property_selector_->clear();

  if( obj.isEmpty() || obj == GridSelector::no_selection ) return;
  

  std::string obj_name( obj.latin1() );

  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + obj_name );
  Geostat_grid* grid = (Geostat_grid*) ni.raw_ptr() ;
  // no dynamic_cast is used on the previous line because it makes 
  // designer and uic segfault when they try to load the plugin...
  // It used to work before, don't know what changed.  

  if( !grid ) return;

  std::list<std::string> properties = grid->property_list();
  std::list<std::string>::iterator begin = properties.begin();
  for( ; begin != properties.end() ; ++begin ) {
    QString prop_name( begin->c_str() );
    property_selector_->insertItem( prop_name );
  }
  
  // a dirty hack to have property_selector_ resize itself to the size of 
  // its longest element
  property_selector_->setFont( property_selector_->font() );
  property_selector_->updateGeometry();
}
*/

QString PropertySelector::selectedGrid() const {
  if( object_selector_->count() == 0 ||
      object_selector_->currentText() == GridSelector::no_selection )
    return "";
  else {
    return object_selector_->currentText();
  }
}

QString PropertySelector::selectedProperty() const {
  if( property_selector_->count() == 0 )
    return "";
  else {
    return property_selector_->currentText();
  }

}

void PropertySelector::setSelectedGrid( const QString& name ) {
  // make sure grid "name" exists
  int id = 0; 
  for( ; id < object_selector_->count() ; id++ ) {
    if( object_selector_->text( id ) == name ) {
      object_selector_->setCurrentText( name );
      break;
    }
  }
}

void PropertySelector::setSelectedProperty( const QString& name ) {
  int id = 0; 
  for( ; id < property_selector_->count() ; id++ ) {
    if( property_selector_->text( id ) == name ) {
      property_selector_->setCurrentText( name );
      break;
    }
  } 
}
  



//=======================================================
/*
#include "icons/selectors_icons.h"
#include <qpixmap.h>

QByteArray find_icon_data( const char* name ) {
  return qembed_findData( name );
}


Ordered_property_selector_dialog::
Ordered_property_selector_dialog( QWidget* parent, const char* name ) 
: QDialog( parent, name ) {

  QVBoxLayout* main_layout = new QVBoxLayout( this );
  main_layout->setMargin( 12 );
  main_layout->setSpacing( 6 );

  
  property_selector_ = 
    new MultiPropertySelector( this, "prop_select" );

  QSizePolicy button_size_policy( QSizePolicy::Minimum, QSizePolicy::Fixed );

  QPushButton* left2right_button_ = new QPushButton( this, "left2right" );
  QPushButton* right2left_button_ = new QPushButton( this, "right2left" );


  selected_properties_ = new QListBox( this, "selected_prop" );
  selected_properties_->setSelectionMode( QListBox::Single );

  QPushButton* up_button_ = new QPushButton( this, "up" ) ;
  QPushButton* down_button_ = new QPushButton( this, "down" ) ;

  QHBoxLayout* top_layout = new QHBoxLayout( main_layout );
  top_layout->addWidget( property_selector_ );

  QVBoxLayout* arrow_layout = new QVBoxLayout( top_layout );
  arrow_layout->addStretch();
  arrow_layout->addWidget( left2right_button_ );
  arrow_layout->addWidget( right2left_button_ );
  arrow_layout->addStretch();

  top_layout->addWidget( selected_properties_ );

  QVBoxLayout* sort_arrow_layout = new QVBoxLayout( top_layout );
  sort_arrow_layout->addWidget( up_button_ );
  sort_arrow_layout->addWidget( down_button_ );
  sort_arrow_layout->addStretch();

  
  QHBoxLayout* bottom_layout = new QHBoxLayout( main_layout);
  QPushButton* ok_button = new QPushButton( "Ok", this, "ok" );
  QPushButton* cancel_button = new QPushButton( "Cancel", this, "cancel" );

  bottom_layout->addStretch();
  bottom_layout->addWidget( ok_button );
  bottom_layout->addSpacing(8);
  bottom_layout->addWidget( cancel_button );
  bottom_layout->addSpacing(8);


//  left2right_button_->setSizePolicy( button_size_policy );
//  right2left_button_->setSizePolicy( button_size_policy );
  left2right_button_->setIconSet( QPixmap( find_icon_data( "right.png" ) ) );
  right2left_button_->setIconSet( QPixmap( find_icon_data( "left.png" ) ) );

//  up_button_->setSizePolicy( button_size_policy );
//  down_button_->setSizePolicy( button_size_policy );
  up_button_->setIconSet( QPixmap( find_icon_data( "up.png" ) ) );
  down_button_->setIconSet( QPixmap( find_icon_data( "down.png" ) ) );


  QObject::connect( left2right_button_, SIGNAL( clicked() ),
                    this, SLOT( left2right() ) );
  QObject::connect( right2left_button_, SIGNAL( clicked() ),
                    this, SLOT( right2left() ) );

  QObject::connect( up_button_, SIGNAL( clicked() ),
                    this, SLOT( move_selected_item_up() ) );
  QObject::connect( down_button_, SIGNAL( clicked() ),
                    this, SLOT( move_selected_item_down() ) );

  QObject::connect( ok_button, SIGNAL( clicked() ),
                    this, SLOT( accept() ) );
  QObject::connect( cancel_button, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );
}
*/

Ordered_property_selector_dialog::
Ordered_property_selector_dialog( QWidget* parent, const char* name ) 
: Ordered_property_selector_dialog_base( parent, name ) {

  selector_box_->setColumnLayout( 1, Qt::Vertical );
  property_selector_ = 
    new MultiPropertySelector( selector_box_, "prop_select" );


  QObject::connect( left2right_button_, SIGNAL( clicked() ),
                    this, SLOT( left2right() ) );
  QObject::connect( right2left_button_, SIGNAL( clicked() ),
                    this, SLOT( right2left() ) );

  QObject::connect( up_button_, SIGNAL( clicked() ),
                    this, SLOT( move_selected_item_up() ) );
  QObject::connect( down_button_, SIGNAL( clicked() ),
                    this, SLOT( move_selected_item_down() ) );

  selected_properties_->setSelectionMode(QListBox::Extended);
}




void Ordered_property_selector_dialog::
show_properties( const QString& grid_name ) {
  property_selector_->show_properties( grid_name );
}


void Ordered_property_selector_dialog::move_selected_item_up() {
  // if the first item is selected, we can't move the selection up
  if( selected_properties_->isSelected(0) ) return;

  for( int i=1; i < selected_properties_->count() ; i++ ) {
    if( !selected_properties_->isSelected(i) ) continue;
      
    QString item_above = selected_properties_->text( i-1 );
    QString current_item = selected_properties_->text( i );

    selected_properties_->changeItem( current_item, i-1 );
    selected_properties_->changeItem( item_above, i );
    selected_properties_->setSelected( i-1, true );
    selected_properties_->setSelected( i, false );
  }
}


void Ordered_property_selector_dialog::move_selected_item_down() {
/*
int id = selected_properties_->currentItem();
  if( id == selected_properties_->count()-1 ) return;

  QString item_below = selected_properties_->text( id + 1 );
  QString current_item = selected_properties_->text( id );

  selected_properties_->changeItem( current_item, id + 1 );
  selected_properties_->changeItem( item_below, id );
  selected_properties_->setSelected( id+1, true );
  selected_properties_->setCurrentItem( id+1 );
*/

  // if the last item is selected, we can't move the selection down
  int last = selected_properties_->count()-1;
  if( selected_properties_->isSelected(last) ) return;

  for( int i=last-1; i >= 0 ; i-- ) {
    if( !selected_properties_->isSelected(i) ) continue;
    
    QString item_below = selected_properties_->text( i+1 );
    QString current_item = selected_properties_->text( i );

    selected_properties_->changeItem( current_item, i+1 );
    selected_properties_->changeItem( item_below, i );
    selected_properties_->setSelected( i+1, true );
    selected_properties_->setSelected( i, false );
    
  }
}


void Ordered_property_selector_dialog::remove_selected_item() {
  int id = selected_properties_->currentItem();
  if( id < 0 ) return;

  selected_properties_->removeItem( id );
  
  id = selected_properties_->currentItem();
  if( id < 0 ) return;

  selected_properties_->setSelected( id, true );
}


void Ordered_property_selector_dialog::left2right() {
  for( int i = 0; i < property_selector_->count() ; i++ ) {
    if( !property_selector_->isSelected( i ) ) continue;

    selected_properties_->insertItem( property_selector_->text( i ) );
    property_selector_->removeItem( i );
    i--;
  }
}


void Ordered_property_selector_dialog::right2left() {
/*
  int i = selected_properties_->currentItem();
  property_selector_->insertItem( selected_properties_->text( i ) );
  selected_properties_->removeItem( i );
*/
  for( int i = 0; i < selected_properties_->count() ; i++ ) {
    if( !selected_properties_->isSelected( i ) ) continue;

    property_selector_->insertItem( selected_properties_->text( i ) );
    selected_properties_->removeItem( i );
    i--;
  }
}


void Ordered_property_selector_dialog::
set_current_selection( const QStringList& list ) {
  for( int i = 0; i < list.size() ; i++ ) {
    QListBoxItem* item = property_selector_->findItem( list[i], Qt::ExactMatch );
    if( !item ) continue;

    selected_properties_->insertItem( item->text() );
    delete item;
  }

//  selected_properties_->insertStringList( list );
}

QStringList Ordered_property_selector_dialog::selection() const {
  QStringList current_selection;
  for( int i = 0; i < selected_properties_->count() ; i++ )
    current_selection.push_back( selected_properties_->text( i ) );

  return current_selection;
}


//=======================================================

OrderedPropertySelector::
OrderedPropertySelector( QWidget* parent, const char* name ) 
: GsTLGroupBox( parent, name ) {
  setColumnLayout( 2, Qt::Vertical );
  selected_properties_ = new MultiPropertySelector( this, "listbox" );
  QPushButton* choose_button = 
    new QPushButton( "Choose Properties...", this, "button" );

  selected_properties_->setSelectionMode( QListBox::NoSelection );

  QObject::connect( choose_button, SIGNAL( clicked() ),
                    this, SLOT( show_selection_dialog() ) );
}


void OrderedPropertySelector::show_properties( const QString& grid_name ) {
  if( grid_name_ == grid_name ) return;

  selected_properties_->clear();
  grid_name_ = grid_name;
}

void OrderedPropertySelector::show_selection_dialog() {
  Ordered_property_selector_dialog* dialog =
    new Ordered_property_selector_dialog( this, "Choose" );
  dialog->setCaption( "Select Properties" );
  dialog->show_properties( grid_name_ );

  if( selected_properties_->count() != 0 ) {
    QStringList current_selection;
    for( int i = 0; i < selected_properties_->count() ; i++ )
      current_selection.push_back( selected_properties_->text( i ) );
    dialog->set_current_selection( current_selection );
  }

  if( dialog->exec() == QDialog::Rejected ) return;
  
  QStringList selection = dialog->selection();
  selected_properties_->clear();
  selected_properties_->insertStringList( selection );

  emit selected_count( selected_properties_->count() );
}


QStringList OrderedPropertySelector::selected_properties() const {
  QStringList list;
  for( int i = 0 ; i < selected_properties_->count() ; i++ ) 
    list << selected_properties_->text( i );

  return list;
}


void OrderedPropertySelector::set_selected_properties( QStringList name_list ) {
/*
  selected_properties_->show_properties( grid_name_ );
  
  QStringList current;
  for( int i=0; i < selected_properties_->count() ; i++ ) {
    QString text = selected_properties_->text( i );

    bool found = false;
    int j=0;
    while( !found && j < list.size() ) {
      if( text == list[j++] ) found = true;
    }

    if( !found ) {
      selected_properties_->removeItem( i );
      i--;
    }
  }
*/

  if( grid_name_.isEmpty() || grid_name_ == GridSelector::no_selection ) return;
  

  std::string name( grid_name_.latin1() );

  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + name );
  Geostat_grid* grid = (Geostat_grid*) ni.raw_ptr() ;
  // no dynamic_cast is used on the previous line because it makes 
  // designer and uic segfault when they try to load the plugin...
  // It used to work before, don't know what changed.  

  if( !grid ) return;
    
  selected_properties_->clear();

  std::list<std::string> properties = grid->property_list();
  properties.sort();

  for( unsigned int i=0; i < name_list.size() ; i++ ) {
    if( name_list[i] == QString::null ) continue;

    bool found = std::binary_search( properties.begin(), properties.end(), 
                                     std::string( name_list[i].latin1() ) );
    if( found )
      selected_properties_->insertItem( name_list[i] );
  }

}




//=======================================================

GsTLGroupBox::GsTLGroupBox( QWidget* parent, const char* name,
			    const QString& keyword,
			    bool displayed, bool activated )
  : QGroupBox( parent, name ), 
    keyword_( keyword ),
    displayed_( displayed ), activated_( activated ) {

}

GsTLGroupBox::GsTLGroupBox( int strips, Orientation orientation,
			    QWidget* parent, const char* name,
			    const QString& keyword,
			    bool displayed, bool activated )
  : QGroupBox( strips, orientation, parent, name ),
    keyword_( keyword ),
    displayed_( displayed ), activated_( activated ) {

}
 

QString GsTLGroupBox::keyword() const {
  return keyword_;
}

void GsTLGroupBox::set_keyword( const QString& keyword ) {
  keyword_ = keyword ;
  std::string str = std::string( keyword.ascii() );
  std::vector<std::string> key =  String_Op::decompose_string( str, ";" );
  keywords_map_.clear();
  for(std::vector<std::string>::iterator it = key.begin(); it != key.end() ; ++it )
    keywords_map_.insert( QString(it->c_str()) );
}


bool GsTLGroupBox::is_displayed() const {
  return displayed_;
}

void GsTLGroupBox::set_displayed( bool on ) {
  displayed_ = on;
}


void GsTLGroupBox::setHidden( bool on ) {
  if( on ) { 
    displayed_ = false;
    if( isShown() )
      hide();
  }
  else {
    displayed_ = true;
    if( !isShown() )
      show();
  }
}

void GsTLGroupBox::setHidden( const QString& keyword ) {
  //setHidden( keyword == keyword_ );
  setHidden( keywords_map_.find( keyword) != keywords_map_.end() );
}



void GsTLGroupBox::setShown( bool on ) {
  if( on ) { 
    displayed_ = true;
    if( !isShown() )
      show();
  }
  else {
    displayed_ = false;
    if( isShown() )
      hide();
  }
}

void GsTLGroupBox::setShown( const QString& keyword ) {
  //setShown( keyword == keyword_ );
  setShown( keywords_map_.find( keyword) != keywords_map_.end() );
}


void GsTLGroupBox::setDeActivated( bool on ) {
  if( on ) {
    activated_ = false;
    hide(); 
    setEnabled( false );
  }
  else {
    activated_ = true;
    show(); 
    setEnabled( true );
  } 
}

void GsTLGroupBox::setDeActivated( const QString& keyword ) {
  //setDeActivated( keyword == keyword_ );
  setDeActivated( keywords_map_.find( keyword) != keywords_map_.end() );
}

void GsTLGroupBox::setActivated( bool on ) {
  if( on ) {
    activated_ = true;
    show(); 
    setEnabled( true );
  }
  else {
    activated_ = false;
    hide(); 
    setEnabled( false );
  } 
}

void GsTLGroupBox::setActivated( const QString& keyword ) {
  //setActivated( keyword == keyword_ );
  setActivated( keywords_map_.find( keyword) != keywords_map_.end() );
}

void GsTLGroupBox::setEnabled( bool on ) {
	QGroupBox::setEnabled( on );
}

void GsTLGroupBox::setDisabled( bool on ) {
	QGroupBox::setDisabled( on );
}

void GsTLGroupBox::setEnabled( const QString& keyword ) {
  //setEnabled( keyword == keyword_ );
  setEnabled( keywords_map_.find( keyword) != keywords_map_.end() );
}

void GsTLGroupBox::setDisabled( const QString& keyword ) {
  //setDisabled( keyword == keyword_ );
  setDisabled(keywords_map_.find( keyword) != keywords_map_.end() );
}

//=======================================================

GsTLButtonGroup::GsTLButtonGroup( QWidget* parent, const char* name,
				  const QString& keyword,
          bool displayed, bool activated )
  : QButtonGroup( parent, name ), 
    keyword_( keyword ),
    displayed_( displayed ), activated_( activated ) {

}

QString GsTLButtonGroup::keyword() const {
  return keyword_;
}

void GsTLButtonGroup::set_keyword( const QString& keyword ) {
  keyword_ = keyword ;
}

void GsTLButtonGroup::setHidden( bool on ) {
  if( on ) { 
    if( isShown() )
      hide();
  }
  else {
    if( !isShown() )
      show();
  }
}

void GsTLButtonGroup::setHidden( const QString& keyword ) {
  setHidden( keyword == keyword_ );
}


void GsTLButtonGroup::setShown( bool on ) {
  if( on ) { 
    if( !isShown() )
      show();
  }
  else {
    if( isShown() )
      hide();
  }
}

void GsTLButtonGroup::setShown( const QString& keyword ) {
  setShown( keyword == keyword_ );
}


void GsTLButtonGroup::setDeActivated( bool on ) {
  if( on ) {
    activated_ = false;
    hide(); 
    setEnabled( false );
  }
  else {
    activated_ = true;
    show(); 
    setEnabled( true );
  } 
}

void GsTLButtonGroup::setDeActivated( const QString& keyword ) {
  setDeActivated( keyword == keyword_ );
}

void GsTLButtonGroup::setActivated( bool on ) {
  if( on ) {
    activated_ = true;
    show(); 
    setEnabled( true );
  }
  else {
    activated_ = false;
    hide(); 
    setEnabled( false );
  } 
}

void GsTLButtonGroup::setActivated( const QString& keyword ) {
  setActivated( keyword == keyword_ );
}



//=======================================================
/*
CloneGroupBox::CloneGroupBox( QWidget* parent, const char* name )
  : GsTLGroupBox( 2, Qt::Vertical, parent, name ),
    count_( 1 ) {

}
*/

CloneGroupBox::CloneGroupBox( QWidget* parent, const char* name )
  : GsTLGroupBox( parent, name ),
    count_( 1 ) {
  main_widget_ = 0;
  main_label_ = 0;

  adjust_ = 0;
}

void CloneGroupBox::init_cloning() {
  QObjectList* children = queryList( "QWidget", 0, TRUE, FALSE );
  QObject* obj = children->first() ;
  QWidget* current_widget = (QWidget*) obj;
  if( !current_widget ) return;
  
  if( current_widget->isA( "QLabel" ) ) {
    QObject* sec_obj = children->next();
    if( sec_obj ) {
      main_label_ = (QLabel*) current_widget;
      main_widget_ = (QWidget*) sec_obj;
    }
    else {
      main_widget_ = current_widget;
      adjust_ = 1;
    }
  }
  else {
    main_widget_ = current_widget;
    adjust_ = 1;
  }
}


void CloneGroupBox::set_count( int n ) {
  n=std::max(n,1);

  if( n == count_ ) return;

  if( !main_widget_ )
    init_cloning();
  

  if( n > count_ ) {      
    setColumns( 2*n - adjust_ );
    
    int to_be_added = n - count_;

    for( int i = 0; i < to_be_added; i++ ) {
      count_++;
      QString id; 
      id.setNum( count_ );
      QString new_name( main_widget_->name() );
      new_name += "_" + id ;
      
      QLabel* label = new QLabel( this, new_name + "_label" );
      if( main_label_ ) {
        label->setText( main_label_->text() + " " + id );
        label->setFont( main_label_->font() );
      } 

      QWidget* clone = clone_widget( main_widget_, this, new_name );
      appli_assert( clone != 0 );

      label->show();
      clone->show();
      
      cloned_widgets_.push_back( clone );
      labels_.push_back( label );
    }
  }
  else {
    for( int i=cloned_widgets_.size()-1; i>n-2; i-- ) {
      delete cloned_widgets_[i];
      delete labels_[i];
    }
    count_ = n;
    cloned_widgets_.resize( n-1 );
    labels_.resize( n-1 );
    setColumns( 2*count_ - adjust_ );
  }

}
 
 
QWidget* CloneGroupBox::clone_widget( QWidget* widget, QWidget* parent, 
                                      const char* name ) {
  QWidgetFactory factory;
  QWidget* cloned = factory.createWidget( widget->className(), parent, name );
  return cloned;
}


/*void CloneGroupBox::set_count( int n ) {
  n=std::max(n,1);

  if( n == count_ ) return;

  if( n > count_ ) {

    // only duplicate the first widget in the group box
    QObjectList* children = queryList( "QWidget", 0, TRUE, FALSE );
    QObject* obj = children->first() ;
    QWidget* current_widget = (QWidget*) obj;
    if( !current_widget ) return;
    
    setColumns( n );
    QWidgetFactory factory;
    int to_be_added = n - count_;

    for( int i = 0; i < to_be_added; i++ ) {
      count_++;
      QString id; 
      id.setNum( count_ );
      QString new_name( current_widget->name() );
      new_name += "_" + id ;
      QWidget* clone = factory.createWidget( current_widget->className(), 
					     this, new_name );
      appli_assert( clone != 0 );
      clone->show();
      cloned_widgets_.push_back( clone );
    }
  }
  else {
    for( int i=cloned_widgets_.size()-1; i>n-2; i-- ) {
      delete cloned_widgets_[i];
    }
    count_ = n;
    cloned_widgets_.resize( n-1 );
    setColumns( count_ );
  }

}
*/

