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

#include <GsTLAppli/gui/appli/oinv_project_view.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/gui/oinv_description/oinv_cgrid.h>
#include <GsTLAppli/gui/appli/display_pref_panel.h>
#include <GsTLAppli/gui/appli/general_display_pref_panel.h>
#include <GsTLAppli/gui/appli/SoQtGsTLViewer.h>
#include <GsTLAppli/gui/appli/snapshot_dialog.h>
#include <GsTLAppli/gui/appli/qt_grid_summary.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/gui/utils/gstl_qlistviewitem.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>

#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/actions/SoWriteAction.h>

//TL modified
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

 
#include <qlistbox.h>
#include <qlistview.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qtabwidget.h>
#include <qscrollview.h>
#include <qsplitter.h>
#include <qfiledialog.h>
#include <qdragobject.h>
#include <qstrlist.h>
#include <qapplication.h>
#include <qmessagebox.h>

#include <vector>
#include <list>
#include <string>
#include <utility>
#include <algorithm>
#include <fstream>


Named_interface* Create_oinv_view( std::string& ) {
  return new Oinv_view();
}

//TL modified
SbBool eventHandler(void * data, SoEvent * e)
{
	return true;
}


//TL modified
void Oinv_view::object_rename_slot(QListViewItem * item, int col, const QString & s)
{
	if (s == _oldName) return;


	switch(item->rtti()) {
		case SingleSel_QListViewItem::RTTI:
			{
				appli_message("property name changed to " << s.latin1());

				// get grid name from parent
				SingleSel_QListViewItem* ss_item = (SingleSel_QListViewItem*) item;
				QListViewItem* up = ss_item->parent();
				QString grid_name = up->text(0);

				std::string obj_name( grid_name.latin1() );

				Oinv_description_map::pair desc_pair = displayed_objects_.description( obj_name );

				SmartPtr<Named_interface> grid_ni =
					Root::instance()->interface( gridModels_manager + "/" + grid_name.latin1() );
				Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
				appli_assert( grid );

				QString newName = s;
				if (!grid->reNameProperty(_oldName, newName)) {
					item->setText(0,_oldName);
					QMessageBox::critical(this, "Cannot perform renaming",
						"Unable to rename property",
						QMessageBox::Ok,
						QMessageBox::NoButton);
					return;
				}
				if( desc_pair.second != 0 ) 
					desc_pair.second->update_desc( _oldName, newName );

				if (project_)
					project_->update();
				break;
		  }
	}
}


const QString Project_view_gui::general_pref_panel_name_( "< General >" );

Project_view_gui::Project_view_gui( QWidget* parent )
  : Project_view_form( parent ),
    current_pref_panel_( 0 ),
    current_info_panel_( 0 )
{


  //--------------
  // Set up the Open Inventor components
  SoSeparator* root = new SoSeparator;
  
//  scene_root_ = new SoSelection;
  scene_root_ = new SoSeparator;
  root->addChild( scene_root_ );
  
  oinv_viewer_ = new SoQtGsTLViewer( (QWidget*) ViewerFrame, "camera" );
  oinv_viewer_->setSceneGraph( root );

  colormap_root_node_ = new SoSeparator;
  root->addChild( colormap_root_node_ );
  
  // Initialize the map of displayed objects
  displayed_objects_.scene_graph( scene_root_ );

  //--------------
  // Set up the frame for the preference panels
  GroupBox7->setOrientation( Qt::Horizontal );
  GroupBox7->setColumns( 1 );
  pref_object_selector_ = new GridSelector( GroupBox7, "pref_object_selector" );
  QFont cur_font = pref_object_selector_->font();
  cur_font.setBold( false );
  pref_object_selector_->setFont( cur_font );

  QScrollView* scroll_view = new QScrollView( ControlPanel->page( 1 ), 
						"scrollview" );
  pref_panel_frame_ = new QVBox( scroll_view->viewport(), "pref_panel_frame" );
  scroll_view->addChild( pref_panel_frame_ );
  scroll_view->setResizePolicy( QScrollView::AutoOneFit );
  pref_panel_frame_->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
  pref_panel_frame_->setSpacing( 8 );
  pref_panel_frame_->setMargin( 4 );
    
  ControlPanel->page( 1 )->layout()->add( scroll_view );
  ControlPanel->page( 1 )->layout()->addItem( 
       new QSpacerItem( 10, 16, QSizePolicy::Minimum, QSizePolicy::Minimum )
       );
  scroll_view->show();

  
  //--------------
  // Set up the frame for the info panel
  InfoGroupBox->setOrientation( Qt::Horizontal );
  InfoGroupBox->setColumns( 1 );
  info_object_selector_ = new GridSelector( InfoGroupBox, "info_object_selector" );
  cur_font = info_object_selector_->font();
  cur_font.setBold( false );
  info_object_selector_->setFont( cur_font );

  QScrollView* info_scroll_view = new QScrollView( ControlPanel->page( 2 ), 
						"infoscrollview" );
  info_panel_frame_ = new QVBox( info_scroll_view->viewport(), "info_panel_frame" );
  info_scroll_view->addChild( info_panel_frame_ );
  info_scroll_view->setResizePolicy( QScrollView::AutoOneFit );
  info_panel_frame_->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
  info_panel_frame_->setSpacing( 8 );
  info_panel_frame_->setMargin( 4 );
    
  ControlPanel->page( 2 )->layout()->add( info_scroll_view );
  ControlPanel->page( 2 )->layout()->addItem( 
       new QSpacerItem( 10, 16, QSizePolicy::Minimum, QSizePolicy::Minimum )
       );
  

  //--------------
  // set up the "general" preference panel
  
  // first remove the "<- none ->" option from the list:
  pref_object_selector_->clear();

  general_pref_panel_ = 
    new General_display_pref_panel( oinv_viewer_, colormap_root_node_,
				    &displayed_objects_,
				    pref_panel_frame_, 0 );
  current_pref_panel_ = general_pref_panel_;
  current_pref_panel_->setGeometry( 0,0, 250, 
				    pref_panel_frame_->height() );
  current_pref_panel_->show();



  //--------------
  // initialize the other elements of the interface

  main_splitter_->setResizeMode( ControlPanel, QSplitter::KeepSize );

  init_objects_selector();
  Object_tree->setSelectionMode( QListView::NoSelection );

  pref_object_selector_->insertItem( general_pref_panel_name_ );


  //--------------
  // Set up the connections between the different widgets


  QObject::connect( Object_tree, 
		    SIGNAL(mouseButtonClicked( int, QListViewItem*, const QPoint&, int ) ),
		    this, 
		    SLOT( object_clicked_slot( int, QListViewItem*, const QPoint&, int ) ) );
  
  QObject::connect( pref_object_selector_, 
		    SIGNAL( activated( const QString& ) ),
		    (QObject*) this,
		    SLOT( show_preference_panel( const QString& ) ) ); 

  QObject::connect( info_object_selector_, 
		    SIGNAL( activated( const QString& ) ),
		    (QObject*) this,
		    SLOT( show_info_panel( const QString& ) ) ); 

  QObject::connect( (QObject*) view_all_button_, SIGNAL( clicked() ),
		    (QObject*) this, SLOT( view_all() ) ); 
  QObject::connect( (QObject*) set_home_view_button_, SIGNAL( clicked() ),
		    (QObject*) this, SLOT( set_home_view() ) ); 
  QObject::connect( (QObject*) home_view_button_, SIGNAL( clicked() ),
		    (QObject*) this, SLOT( home_view() ) ); 
  QObject::connect( (QObject*) face_view_button_, SIGNAL( clicked() ),
		    (QObject*) this, SLOT( face_view() ) ); 
  QObject::connect( (QObject*) side_view_button_, SIGNAL( clicked() ),
		    (QObject*) this, SLOT( side_view() ) ); 
  QObject::connect( (QObject*) top_view_button_, SIGNAL( clicked() ),
		    (QObject*) this, SLOT( top_view() ) ); 
  QObject::connect( (QObject*) snapshot_button_, SIGNAL( clicked() ),
		    (QObject*) this, SLOT( snapshot() ) ); 


  // accept drag and drop
  setAcceptDrops( true );

}
/*
void Project_view_gui::dragEnterEvent( QDragEnterEvent* event ) {
//  event->accept( QUriDrag::canDecode(event) );
  std::cout << "enter" << std::endl;
  event->accept( true );
}

void Project_view_gui::dropEvent( QDropEvent* event ) {
  std::cout << "dropped" << std::endl;
  QUriDrag uri;
  QStrList files;
  if ( QUriDrag::decode( event, files ) ) {
    char* str;
    for( str = files.first(); str; str = files.next() )
      std::cout << str << std::endl;
  }
}
*/







void Project_view_gui::set_parent( QWidget* parent ) {
  QPoint p(0,0);
  reparent( parent, p, false ); 
}


Project_view_gui::~Project_view_gui() {
  if( oinv_viewer_ ) {
    delete oinv_viewer_;
    //scene_root_->unref();
    //oinv_viewer_ = 0;
  }
  
  /* a QT widget is supposed to destroy all its children when it is destroyed.
   * Each display pref panel is a widget child, so I expect them to be destroyed
   * automatically. If that is true, the following piece of code should not be
   * necessary
   */
  /*
  for( Pref_Panel_Map::iterator it = display_pref_panels_.begin(); 
       it != display_pref_panels_.end() ; ++it ) {
    delete it->second;
  }
  */
}



void Project_view_gui::object_clicked_slot( int button, QListViewItem* item, 
                                           const QPoint&, int ) {
  if( button == 1 ) {
    update_display( item );
  }
  else {
	  if (item) {
		  if (item == Object_tree->firstChild()) {
			  appli_message("Cannot rename the root");
			  return;
		  }
		  else if (item->rtti() == MultiSel_QListViewItem::RTTI) {
			  appli_message("Renaming grid is not currently supported");
			  return ;
		  }
		  _oldName = item->text(0); 
		  item->setRenameEnabled(0,true);
		  item->startRename(0);
	  }
  }
}


void Project_view_gui::init_objects_selector() {
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( gridModels_manager );
  Manager* mng = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( mng );

  QListViewItem* root = Object_tree->firstChild();
  appli_assert( root != 0);
  root->setSelectable( false );


  Manager::interface_iterator begin = mng->begin_interfaces();
  Manager::interface_iterator end = mng->end_interfaces();

  for( ; begin != end ; ++begin ) {

    // Add the grid-object to the list

    std::string name = mng->name( begin->raw_ptr() );
    MultiSel_QListViewItem* entry =
        new MultiSel_QListViewItem( root, QString( name.c_str() ) );
    

    // Add the properties of the grid-object to the list

    Geostat_grid* grid = dynamic_cast<Geostat_grid*>(  begin->raw_ptr() );
    appli_assert( grid != 0 );
    std::list<std::string> property_names = grid->property_list();
    typedef std::list<std::string>::const_iterator iterator;
    
    iterator prop_end = property_names.end();
    for( iterator it = property_names.begin(); it !=  prop_end ; ++it ) {
      new SingleSel_QListViewItem( entry, QString( it->c_str() ) ); 
    }
  }
}



void Project_view_gui::add_object( std::string obj_name ) {
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( gridModels_manager + "/" + obj_name );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( ni.raw_ptr() );
  if( !grid ) {
    appli_warning( "invalid object name (object does not exist): " << obj_name );
    return;
  }
  

  QListViewItem* root = Object_tree->firstChild();
  appli_assert( root != 0);

  // Add the grid-object to the list
  MultiSel_QListViewItem* entry =
        new MultiSel_QListViewItem( root, QString( obj_name.c_str() ) );

  // Add the properties of the grid-object to the list
  std::list<std::string> property_names = grid->property_list();
  typedef std::list<std::string>::const_iterator iterator;
    
  iterator prop_end = property_names.end();
  for( iterator it = property_names.begin(); it !=  prop_end ; ++it ) {
    new SingleSel_QListViewItem( entry, QString( it->c_str() ) ); 
  }

} 



//------------------
void Project_view_gui::set_object_displayed( const QString& obj ) {
  QListViewItem* grid_item = get_grid_listitem( obj );
  if( !grid_item ) return;

  grid_item->setSelected( true );
  display_object( obj );
}

void Project_view_gui::set_object_undisplayed( const QString& obj ) {
  QListViewItem* grid_item = get_grid_listitem( obj );
  if( !grid_item ) return;

  grid_item->setSelected( false );
  undisplay_object( obj );
}


void Project_view_gui::
set_property_displayed( const QString& grid, const QString& prop ) {
  QListViewItem* prop_item = get_property_listitem( grid, prop );
  if( !prop_item ) return;

  prop_item->setSelected( true );
  display_property( grid, prop );
}


void Project_view_gui::
set_property_undisplayed( const QString& grid, const QString& prop ) {
  QListViewItem* prop_item = get_property_listitem( grid, prop );
  if( !prop_item ) return;

  prop_item->setSelected( false );
  undisplay_property( grid, prop );
}


void Project_view_gui::
set_colormap( const QString& cmap_name,  
              const QString& grid, const QString& prop,
              const QString& min_str, const QString& max_str ) {

  show_preference_panel( grid );

  Pref_Panel_Map::iterator it = display_pref_panels_.find( grid.ascii() );
  if( it == display_pref_panels_.end() ) return;
  
  Display_pref_panel* pref_panel = it->second;
  pref_panel->set_colormap( cmap_name );
  if( !min_str.isNull() ) pref_panel->set_colormap_min_value( min_str.toFloat() );
  if( !max_str.isNull() ) pref_panel->set_colormap_max_value( max_str.toFloat() );
}




//------------------



void Project_view_gui::update_display( QListViewItem* item ) {
  if( !item ) return;

  QString obj_name = item->text(0);

  switch( item->rtti() ) {

    case MultiSel_QListViewItem::RTTI :
      {
	// The user clicked on an object name
  

	MultiSel_QListViewItem* ms_item = (MultiSel_QListViewItem*) item;
	ms_item->switch_state();
	if( ms_item->is_visible() ) {
	  appli_message( "displaying " << obj_name );
	  display_object( obj_name );
	}
	else {
	  appli_message( "un-displaying " << obj_name );
	  undisplay_object( obj_name );
	}
	break;
      }

    case SingleSel_QListViewItem::RTTI :
      {
	// The user clicked on a property name
	SingleSel_QListViewItem* ss_item = (SingleSel_QListViewItem*) item;
	ss_item->switch_state();
	
	// tell the grid to change the displayed property
	QListViewItem* up = ss_item->parent();
	QString grid_name = up->text(0);

	if( ss_item->is_visible() ) {
	  display_property( grid_name, obj_name );
	}
	else {
	  undisplay_property( grid_name, obj_name );
	}

	// find if there is a display preference panel for grid_name and
	// notify it of the changes
	Pref_Panel_Map::const_iterator panel_it = 
	  display_pref_panels_.find( std::string( grid_name.latin1() ) );
	if( panel_it != display_pref_panels_.end() ) {
	  panel_it->second->change_selected_property( obj_name );
	  panel_it->second->toggle_paint_property( ss_item->is_visible() );
	}

	break;
      }
  }
}




void Project_view_gui::display_object( const QString& obj ) {
  std::string obj_name( obj.latin1() );
  //Desc_Map::iterator it = displayed_objects_.find( obj_name );
  Oinv_description_map::pair desc_pair = displayed_objects_.description( obj_name );
  if( !desc_pair.second ) return;

/* This is already done by Oinv_description_map::description( QString ), hence 
 * is commented out. I did not delete it because I am not sure 
 * Oinv_description_map should add the description to the scene...
  if( desc_pair.first == false ) {
    scene_root_->addChild( (SoNode*) desc_pair.second->oinv_node() );
  }
*/

  desc_pair.second->oinv_node()->visible = true;
  

  // If there is only 1 object displayed, do a viewAll.
  if( scene_root_->getNumChildren() == 1 )
    oinv_viewer_->viewAll();

   
#ifdef OINV_DEBUG
  // Write the scene to "scene.iv"
  FILE* file = fopen("scene.iv", "w");
  SoWriteAction writer;

  writer.getOutput()->setFilePointer(file);
  writer.apply( oinv_viewer_->getSceneGraph() );
  fclose( file );
#endif

}


void Project_view_gui::undisplay_object( const QString& obj ) {
  appli_message( "un-displaying " << obj );

  std::string obj_name( obj.latin1() );
  //Desc_Map::iterator it = displayed_objects_.find( obj_name );
  //appli_assert( it != displayed_objects_.end() );
  Oinv_description_map::pair desc_pair = displayed_objects_.description( obj_name );

  if( !desc_pair.second ) return;

  desc_pair.second->oinv_node()->visible = false;
  
}


void Project_view_gui::display_property( const QString& grid, const QString& prop ) {
  appli_message( "displaying property "<< prop << " of object " << grid );
  
  std::string obj_name( grid.latin1() );

  Oinv_description_map::pair desc_pair = displayed_objects_.description( obj_name );
  if( desc_pair.second == 0 ) return;

  //TL modified
  SmartPtr<Named_interface> grid_ni =
	  Root::instance()->interface( gridModels_manager + "/" + obj_name );
  Geostat_grid* gr = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  appli_assert( gr );
  Reduced_grid temp;

  if( desc_pair.first == false )
    scene_root_->addChild( (SoNode*) desc_pair.second->oinv_node() );

  if (gr->classname() == temp.classname()) {
	  Oinv_strati_grid * g = dynamic_cast<Oinv_strati_grid*>(desc_pair.second);
	  g->display_mode(Oinv_strati_grid::RENDERING);
  }

  QApplication::setOverrideCursor( Qt::waitCursor );

  desc_pair.second->set_property( std::string( prop.latin1() ) );
  desc_pair.second->property_display_mode( Oinv::PAINTED );
  QApplication::restoreOverrideCursor();
 
}




void Project_view_gui::undisplay_property( const QString& grid,
					   const QString& ) {
  Oinv_description_map::pair desc_pair = 
    displayed_objects_.description( std::string( grid.latin1() ) );

  appli_assert( desc_pair.first == true );
  desc_pair.second->property_display_mode( Oinv::NOT_PAINTED );

}

  
void Project_view_gui::show_preference_panel( const QString& obj ) {

  // set up the frame where the preference panel will be displayed, 
  // if that has not been done yet
  if( !pref_panel_frame_ ) {
    QScrollView* scroll_view = new QScrollView( ControlPanel->page( 1 ), 
						"scrollview" );
    pref_panel_frame_ = new QVBox( scroll_view->viewport(), "pref_panel_frame" );
    scroll_view->addChild( pref_panel_frame_ );
    scroll_view->setResizePolicy( QScrollView::AutoOneFit );
    pref_panel_frame_->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
    pref_panel_frame_->setSpacing( 8 );
    pref_panel_frame_->setMargin( 4 );
    
    ControlPanel->page( 1 )->layout()->add( scroll_view );
    ControlPanel->page( 1 )->layout()->addItem( 
	    new QSpacerItem( 10, 16, QSizePolicy::Minimum, QSizePolicy::Minimum )
	  );
    scroll_view->show();
  }
  

  // If the user wants to see the "general preferences panel"
  if( obj == general_pref_panel_name_ ) {
    if( !general_pref_panel_ ) {
      general_pref_panel_ = 
	      new General_display_pref_panel( oinv_viewer_, colormap_root_node_,
					                              &displayed_objects_,
					                              pref_panel_frame_, 0 );
    }
    
    if( current_pref_panel_ )
      current_pref_panel_->hide();
    
    current_pref_panel_ = general_pref_panel_;
    current_pref_panel_->setGeometry( 0,0, 250, 
				      pref_panel_frame_->height() );
    current_pref_panel_->show();
    return;
  }



  // The user wants to see the preference panel of an object:
  //  - get the panel from the panel map
  //  - initialize the panel and set it as the current one

  std::string obj_name( obj.latin1() );
  Pref_Panel_Map::iterator it =
    display_pref_panels_.find( obj_name );



  if( it == display_pref_panels_.end() ) {
    Oinv_description_map::pair desc_pair = displayed_objects_.description( obj_name );
    if( desc_pair.second == 0 ) return;

    if( desc_pair.first == false )
      scene_root_->addChild( (SoNode*) desc_pair.second->oinv_node() );

    Oinv_description* description = desc_pair.second;

    
    std::pair<Pref_Panel_Map::iterator, bool> inserted =
      display_pref_panels_.insert( 
	  std::make_pair( obj_name, 
			  new Display_pref_panel( description, 
						  pref_panel_frame_, 0 )
			  )
	  );
    appli_assert( inserted.second == true );
    it = inserted.first;
    
    Display_pref_panel* pref_panel = it->second;
    QObject::connect( pref_panel, 
		      SIGNAL( displayed_property_changed( const QString&, 
							  const QString& ) ),
		      this,
		      SLOT( toggle_grid_property( const QString&, 
						  const QString& ) )
		      );

    QObject::connect( pref_panel, 
		      SIGNAL( property_painted_toggled( const QString&, 
							const QString& ) ),
		      this,
		      SLOT( toggle_grid_property( const QString&, 
						  const QString& ) )
		      );

    if( general_pref_panel_ ) {
      QObject::connect( pref_panel, SIGNAL( colormap_changed( const Colormap* ) ),
			general_pref_panel_, SLOT( update_colorbar() ) );
    }

  }

  // set it->second as the currently displayed panel, and show() it 
  if( current_pref_panel_ )
    current_pref_panel_->hide();

  current_pref_panel_ = it->second;

  SmartPtr<Named_interface> grid_ni =
	  Root::instance()->interface( gridModels_manager + "/" + obj_name);
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  appli_assert( grid );

  
  Display_pref_panel* p = dynamic_cast<Display_pref_panel*>(current_pref_panel_);
  
  /*
  if (rgrid) {
	  p->volume_explorer_checkbox_->setDisabled(true);
  }
  else
	  p->volume_explorer_checkbox_->setDisabled(false);
  */

  //current_pref_panel_->setMaximumWidth( 250 );
  current_pref_panel_->setGeometry( 0,0, 250, 
				    pref_panel_frame_->height() );
  current_pref_panel_->show();

}




void Project_view_gui::show_info_panel( const QString& obj ) {

  appli_assert( info_panel_frame_ );
  std::cout << "showing " << obj.ascii() << std::endl;

  SmartPtr<Named_interface> model = 
      Root::instance()->interface( gridModels_manager + "/" + obj.ascii() );
    
  Geostat_grid* grid_obj = dynamic_cast<Geostat_grid*>( model.raw_ptr() );
  if( !grid_obj ) {
    if( current_info_panel_ ) 
      current_info_panel_->close();
    return;
  }

  SmartPtr<Named_interface> desc_ni =
      Root::instance()->new_interface( grid_obj->classname(), 
				       QtSummaryDescription_manager + "/" );
    
  QtGrid_summary* desc = dynamic_cast<QtGrid_summary*>( desc_ni.raw_ptr() );
  appli_assert( desc != 0 );

  desc->init( grid_obj );
  if( current_info_panel_ ) 
    current_info_panel_->close();

  current_info_panel_ = desc->description();
  current_info_panel_->reparent( info_panel_frame_, QPoint(0,0), true  );

}


void 
Project_view_gui::toggle_grid_property( const QString& grid, 
					const QString& prop ) {
/*
  QListViewItem* root = Object_tree->firstChild();
  appli_assert( root != 0);

  // search for the listview item corresponding to grid
  QListViewItem* grid_item = root->firstChild();
  while( grid_item ) {
    if( grid_item->text( 0 ) == grid )
      break;
    grid_item = grid_item->nextSibling();
  }
  appli_assert( grid_item );
  
  // search for the child listview item of grid_item corresponding to prop
  QListViewItem* prop_item = grid_item->firstChild();
  while( prop_item ) {
    if( prop_item->text( 0 ) == prop )
      break;
    prop_item = prop_item->nextSibling();
  }
  appli_assert( prop_item );
*/
  QListViewItem* prop_item = get_property_listitem( grid, prop );
  appli_assert( prop_item );
  
  // Select that item as the new visible property
  SingleSel_QListViewItem* ss_item = (SingleSel_QListViewItem*) prop_item;
  ss_item->switch_state();
  
}




void Project_view_gui::view_all() {
  oinv_viewer_->viewAll();
}

void Project_view_gui::set_home_view() {
  oinv_viewer_->saveHomePosition();
}

void Project_view_gui::home_view() {
  oinv_viewer_->resetToHomePosition();
}


void Project_view_gui::face_view() {
  SbVec3f newNormal( 1,0,0 );
  SbVec3f newRight( 0,1,0 );

  set_view_plane( newNormal, newRight );
}

void Project_view_gui::side_view(){
  SbVec3f newNormal( 0,1,0 );
  SbVec3f newRight( -1,0,0 );

  set_view_plane( newNormal, newRight );
}


void Project_view_gui::top_view(){
  SbVec3f newNormal( 0,0,1 );
  SbVec3f newRight( 1,0,0 );

  set_view_plane( newNormal, newRight );
}



void Project_view_gui::snapshot(){
  Snapshot_dialog* dialog = new Snapshot_dialog( oinv_viewer_, this );
  dialog->show();
}



void Project_view_gui::set_view_plane( const SbVec3f& newNormal, 
				       const SbVec3f& newRight ) {

  SoCamera* camera = oinv_viewer_->getCamera();
  
  // get center of rotation
  SbRotation camRot = camera->orientation.getValue();
  float radius = camera->focalDistance.getValue();
  SbMatrix mx;
  mx = camRot;
  SbVec3f forward( -mx[2][0], -mx[2][1], -mx[2][2]);
  SbVec3f center = camera->position.getValue()
    + radius * forward;
    
  // rotate the camera to be aligned with the new plane normal
  SbRotation rot( -forward, newNormal);
  camRot = camRot * rot;
  
  // rotate the camera to be aligned with new right direction
  mx = camRot;
  SbVec3f right(mx[0][0], mx[0][1], mx[0][2]);
  rot.setValue(right, newRight);
  camRot = camRot * rot;
  camera->orientation = camRot;
    
  // reposition camera to look at pt of interest
  mx = camRot;
  forward.setValue( -mx[2][0], -mx[2][1], -mx[2][2]);
  camera->position = center - radius * forward;

}



void Project_view_gui::save_scenegraph_to_file( const QString& filename ) {
  FILE* file = fopen( filename.latin1(), "w" );
  if( !file ) return;

  SoWriteAction writer;
  writer.getOutput()->setFilePointer(file);
  writer.apply( scene_root_ );

  fclose( file );
}



QListViewItem* Project_view_gui::get_grid_listitem( const QString& grid ) {
  QListViewItem* root = Object_tree->firstChild();
  appli_assert( root != 0);

  // search for the listview item corresponding to grid
  QListViewItem* grid_item = root->firstChild();
  while( grid_item ) {
    if( grid_item->text( 0 ) == grid )
      break;
    grid_item = grid_item->nextSibling();
  }

  return grid_item;
}


QListViewItem* Project_view_gui::
get_property_listitem( const QString& grid, const QString& prop ) {
  QListViewItem* root = Object_tree->firstChild();
  appli_assert( root != 0);

  // search for the listview item corresponding to grid
  QListViewItem* grid_item = root->firstChild();
  while( grid_item ) {
    if( grid_item->text( 0 ) == grid )
      break;
    grid_item = grid_item->nextSibling();
  }
  if( !grid_item ) return 0;
  
  // search for the child listview item of grid_item corresponding to prop
  QListViewItem* prop_item = grid_item->firstChild();
  while( prop_item ) {
    if( prop_item->text( 0 ) == prop )
      break;
    prop_item = prop_item->nextSibling();
  }
  return prop_item;

}




//=======================================================


Oinv_view::Oinv_view( GsTL_project* project, QWidget* parent )
  : Project_view_gui( parent ), 
    Project_view( project ) {

  pref_object_selector_->init( project_ );
  info_object_selector_->init( project_ );
  general_pref_panel_->init( project_ );

    //TL modified
  QObject::connect (Object_tree, 
			SIGNAL(itemRenamed ( QListViewItem * , int , const QString & )),
			this,
			SLOT(object_rename_slot(QListViewItem * , int , const QString &)));

}


void Oinv_view::initialize( GsTL_project* project, QWidget* parent ) {
  Project_view::init( project );
  Project_view_gui::set_parent( parent );

  pref_object_selector_->init( project_ );
  info_object_selector_->init( project_ );
  general_pref_panel_->init( project_ );
} 

Oinv_view::~Oinv_view() {
  appli_message( "destroying Oinv_view" );

  /* This is a dirty trick...
   * Oinv_view is a QWidget, and as such is destroyed by its parent widget
   * However, I would like the project to maintain smart-pointers to its views.
   * If smart-pointers are used, they take care of the deletion.
   * Finally the view gets deleted twice, once by the smart pointer, and another
   * time by QT. 
   * The (dirty) trick is to artificially increment the number of references on
   * the view so that the smart pointer does not trigger the deletion.
   * Should find a better solution...
   */
//  Named_interface::new_ref();
}









void Oinv_view::update( std::string obj ) {
  
  // Some of the existing objects have been updated (ie a property
  // has been added, removed, etc, or a region has been added, etc)
  
  /* TODO:
   *  - The current implementation ignores the hint "obj".
   */

  QListViewItem* root = Object_tree->firstChild();
  appli_assert( root != 0);

  QListViewItem* grid_item = root->firstChild();

  // Visit each grid object entry. For each, check that the property
  // list is up-to-date. If not, add/remove the properties that should be
  // added/removed
  while( grid_item ) {
    std::string grid_name( grid_item->text(0).latin1() );
    appli_message( "updating grid_name: \"" << grid_name << "\"" );

    // get the list of properties of the current grid
    SmartPtr<Named_interface> grid_ni = 
      Root::instance()->interface( gridModels_manager + "/" + grid_name );
    if( !grid_ni.raw_ptr() ) {
      appli_warning( "no grid called \"" << grid_name << "\"" );
      return;
    }
     
    Geostat_grid* grid = dynamic_cast<Geostat_grid*>(  grid_ni.raw_ptr() );
    appli_assert( grid != 0 );
    std::list<std::string> property_names = grid->property_list();
    typedef std::list<std::string>::const_iterator iterator;
    

    // get the list of properties currently displayed
    std::list<std::string> displayed_properties;
    QListViewItem* prop_item = grid_item->firstChild();
    while( prop_item ) {
      displayed_properties.push_back( prop_item->text(0).latin1() );
      prop_item = prop_item->nextSibling();
    }


    property_names.sort();
    displayed_properties.sort();

    typedef std::vector<std::string>::iterator String_iterator;
    std::vector<std::string> to_be_added( property_names.size() );
    std::vector<std::string> to_be_removed( displayed_properties.size() );

    String_iterator added_end = 
      std::set_difference( property_names.begin(), property_names.end(),
			   displayed_properties.begin(), displayed_properties.end(),
			   to_be_added.begin() );
    String_iterator removed_end =
      std::set_difference( displayed_properties.begin(), displayed_properties.end(),
			   property_names.begin(), property_names.end(),
			   to_be_removed.begin() );


    // Add the property names that should be added
    for( String_iterator it = to_be_added.begin(); it!= added_end; ++it ) {
      new SingleSel_QListViewItem( grid_item, QString( it->c_str() ) ); 
    }


    for( String_iterator it2 = to_be_removed.begin(); it2!= removed_end; ++it2 ) {
      QListViewItem* prop_item2 = grid_item->firstChild();
      while( prop_item2 ) {
    	  if( std::string( prop_item2->text(0).latin1() ) == *it2 ) {
	        delete prop_item2;
	        prop_item2 = 0;
	      }
        else {
	        prop_item2 = prop_item2->nextSibling();
        }
      }
    }
    
    // If properties were removed, tell the oinv description
    // Remove the property names that should be removed
    Oinv_description_map::iterator desc_found = displayed_objects_.find( grid_name );
    if( desc_found != displayed_objects_.end() ) {
      Oinv_description* desc = desc_found->second.raw_ptr();
      desc->update( Oinv::PROPERTY_DELETED, &to_be_removed );
//      for( String_iterator it3 = to_be_removed.begin(); it3!= removed_end; ++it3 ) {
//        desc->update( Oinv::PROPERTY_DELETED, *it3 );
//      }
    }


    grid_item = grid_item->nextSibling();
  }

  
  // Update the oinv descriptions, in case an existing property was modified
  for( Oinv_description_map::iterator it = displayed_objects_.begin(); 
       it != displayed_objects_.end(); ++it ) {
    it->second->update( Oinv::PROPERTY_CHANGED );
  }


  // Update the display preference panels 
  for( Pref_Panel_Map::iterator it2 = display_pref_panels_.begin(); 
       it2 != display_pref_panels_.end(); ++it2 ) {
    it2->second->update();
  }

  appli_warning( "Oinv_view::update() NOT YET IMPLEMENTED" );
  // check if objet obj is displayed. If yes, redraw
  // otherwise, ignore
}




void Oinv_view::new_object( std::string obj ) {
  add_object( obj );
}

void Oinv_view::deleted_object( std::string obj ) {
  /* 3 things have to be updated:
   *   - remove "obj" from tree view of objects (Object_tree)
   *   - undisplay "obj" if it was displayed
   *   - remove the preference panel for "obj"
   */
  
  QString gridname( obj.c_str() );

  //--------------
  // Remove obj from the tree view
/*
  // search for the listview item corresponding to obj
  QListViewItem* root = Object_tree->firstChild();
  QListViewItem* grid_item = root->firstChild();
  while( grid_item ) {
    if( grid_item->text( 0 ) == gridname )
      break;
    grid_item = grid_item->nextSibling();
  }     
*/
  QListViewItem* grid_item = get_grid_listitem( obj.c_str() );

  if( grid_item ) delete grid_item;


  //--------------
  // undisplay obj
  
  displayed_objects_.delete_description( obj );

  //--------------
  // remove the preference panel
  Pref_Panel_Map::iterator pref_it = display_pref_panels_.find( obj );
  if( pref_it != display_pref_panels_.end() ) {
    if( current_pref_panel_ == pref_it->second ) {
      pref_object_selector_->setCurrentText( general_pref_panel_name_ );
      show_preference_panel( pref_object_selector_->currentText() );
    }

    delete pref_it->second;
    display_pref_panels_.erase( obj );
  }

  //----------------
  // remove the info panel 

  if( info_object_selector_->currentText() == obj.c_str() ) {
    current_info_panel_->close();
  }
  
}




void Oinv_view::dragMoveEvent( QDragMoveEvent * ) {
}

void Oinv_view::dragEnterEvent( QDragEnterEvent * e ) {
  if( QUriDrag::canDecode( e ) ) {
    e->accept();
  }
}

void Oinv_view::dragLeaveEvent( QDragLeaveEvent * ) {
}

void Oinv_view::dropEvent( QDropEvent * e ) {
  QStrList strings;
  QUriDrag::decode( e, strings );
  QString m;
  QStringList files;

  if ( QUriDrag::decodeLocalFiles( e, files ) ) {
    Error_messages_handler error_messages;
    bool ok = true;

    for (QStringList::Iterator i=files.begin(); i!=files.end(); ++i) {
      appli_message( "Loading file " << *i );
      QFileInfo f_info( *i );
      if( f_info.isDir() && 
          ( (*i).endsWith( ".prj" ) || (*i).endsWith( ".prj/" ) ) ) {
        ok *= project_->execute( "LoadProject", std::string( (*i).latin1() ),
                                 &error_messages );
      }
      else {
		if ((*i).endsWith(".prt",false)) {
	        ok *= project_->execute( "LoadSimFromFile", (*i).latin1(), &error_messages );
		}
		else {
			std::string param( std::string((*i).latin1()) + Actions::separator
	  	                     + "All" );
			ok *= project_->execute( "LoadObjectFromFile", param, &error_messages );
		}
      }
    }
    if( !ok ) {
      GsTLcerr << "All the selected files could not be loaded. \n";
      if( !error_messages.empty() )
          GsTLcerr << error_messages.errors() << "\n"; 
      GsTLcerr << gstlIO::end;
    }
  }
}



 

//=========================================================

Oinv_description_map::Oinv_description_map( SoGroup* scene_root )
  : scene_root_( scene_root ) {

}



void Oinv_description_map::scene_graph( SoGroup* scene_root ) {
  scene_root_ = scene_root;
}



std::pair< bool, Oinv_description* >
Oinv_description_map::description( const std::string& obj_name ) {
  bool already_there = true;
  Desc_Map::iterator it = available_descriptions_.find( obj_name );

  if( it == available_descriptions_.end() ) {
    already_there = false;

    SmartPtr<Named_interface> model = 
      Root::instance()->interface( gridModels_manager + "/" + obj_name );
    
    Geostat_grid* grid_obj = dynamic_cast<Geostat_grid*>( model.raw_ptr() );
    if( !grid_obj ) return std::make_pair( false, (Oinv_description*) 0 );
    
    SmartPtr<Named_interface> desc_ni =
      Root::instance()->new_interface( grid_obj->classname(), 
				       oinvDescription_manager + "/" );
    
    Oinv_description* desc = dynamic_cast<Oinv_description*>( desc_ni.raw_ptr() );
    appli_assert( desc != 0 );
    
    
    desc->init( grid_obj );

    appli_assert( scene_root_ );
    if( scene_root_ )
      scene_root_->addChild( desc->oinv_node() );

    std::pair<Desc_Map::iterator, bool> ins = 
      available_descriptions_.insert( 
	  std::make_pair( obj_name, SmartPtr<Oinv_description>(desc) )
	);

    appli_assert( ins.second == true );  
    it = ins.first;
  }

  return std::make_pair( already_there, it->second.raw_ptr() );
}




bool Oinv_description_map::delete_description( const std::string& obj_name ) {
  Desc_Map::iterator it = available_descriptions_.find( obj_name );

  if( it == available_descriptions_.end() ) 
    return false;

  GsTL_SoNode* node = it->second->oinv_node();
  scene_root_->removeChild( node );
  available_descriptions_.erase( it );

  return true;
}


Oinv_description_map::iterator 
Oinv_description_map::find( const std::string& obj_name ) { 
  return available_descriptions_.find( obj_name ); 
}