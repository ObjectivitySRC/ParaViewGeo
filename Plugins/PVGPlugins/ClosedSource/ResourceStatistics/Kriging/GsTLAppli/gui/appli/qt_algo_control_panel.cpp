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

#include <GsTLAppli/gui/appli/qt_algo_control_panel.h>
#include <GsTLAppli/gui/utils/gstl_widget_factory.h>
#include <GsTLAppli/gui/utils/qwidget_value_accessor.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/gui/utils/qwidget_value_collector.h>
#include <GsTLAppli/actions/defines.h>

#include <qframe.h>
#include <qscrollview.h>
#include <qlayout.h>
#include <qstring.h>
#include <qdom.h>
#include <qfile.h>
#include <qlistview.h>
#include <qfiledialog.h>
#include <qsplitter.h>
#include <qapplication.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qobjectlist.h>
#include <qdragobject.h>

#include <fstream>




GsTL_QListView::GsTL_QListView( QWidget* parent, const char* name )
  : QListView( parent, name ) {

}

QListViewItem* GsTL_QListView::addItem( const QString& item, 
					const QString& sep,
					const QString& text_col2 ) {

  QStringList sequence = QStringList::split( sep, item );
  
  QListViewItem* subitem = this->firstChild();
  QListViewItem* parent = 0;

  QStringList::Iterator it = sequence.begin() ;
  for( ; it != sequence.end() ; ++it ) {
 
    QListViewItem* found = subitem;

    if( !found ) {
	if( !parent )
	  found = new QListViewItem( this, *it );
	else {
	  found = new QListViewItem( parent, *it );
	}
    }

    while( found->text(0) != *it ) {
      found = found->nextSibling();
      if( !found ) {
	if( !subitem->parent() )
	  found = new QListViewItem( this, *it );
	else
	  found = new QListViewItem( subitem->parent(), *it );
      }
    }

    parent = found;
    subitem = found->firstChild();
    
  }

  if( parent )
    parent->setText( 1, text_col2 );

  return parent;
}




//======================================


Controls_panel::Controls_panel( QWidget* parent, 
				const char* name, WFlags f ) 
  : ControlsWidget( parent, name, f ) {

  // Forward pushbutton signals to this
 
  QObject::connect( (QObject*) run_button_,
		    SIGNAL(clicked()),
		    (QObject*) this, 
		    SIGNAL(run_algo_selected()) 
		    );
  QObject::connect( (QObject*) load_button_,
		    SIGNAL(clicked()),
		    (QObject*) this, 
		    SIGNAL(load_selected()) 
		    );
  QObject::connect( (QObject*) save_button_,
		    SIGNAL(clicked()),
		    (QObject*) this, 
		    SIGNAL(save_selected()) 
		    );
  QObject::connect( (QObject*) clear_button_,
		    SIGNAL(clicked()),
		    (QObject*) this, 
		    SIGNAL(clear_selected()) 
		    );

}



//======================================
const std::string Algo_control_panel::utils_manager_( xmlGeostatParamUtils_manager );



Algo_control_panel::Algo_control_panel( GsTL_project* project,
					QWidget* parent, 
					const char* name, WFlags f ) 
  : QWidget( parent, name, f ), project_( project ),
    param_input_(0), selected_algo_("") {

  setAcceptDrops( true );

  //--------
  QSplitter *split = new QSplitter( this, "splitter" );
  split->setOrientation( QSplitter::Vertical );

  algo_selector_ = new GsTL_QListView( split, "Algo_selector" );
  controls_ = new Controls_panel( this, "Controls" );
  param_input_frame_ = new QScrollView( split, "Param_frame", WRepaintNoErase );

  split->setResizeMode( algo_selector_, QSplitter::KeepSize );

  layout_ = new QVBoxLayout( this, 0, -1, "control_panel_layout" );
  layout_->addWidget( split );
  layout_->addWidget( controls_ );
  //---------
  /*
  algo_selector_ = new GsTL_QListView( this, "Algo_selector" );
  controls_ = new Controls_panel( this, "Controls" );
  param_input_frame_ = new QScrollView( this, "Param_frame", WRepaintNoErase );

  layout_ = new QVBoxLayout( this, 0, -1, "control_panel_layout" );
  layout_->addWidget( algo_selector_ );
  layout_->addWidget( param_input_frame_ );
  layout_->addWidget( controls_ );
  */

  setMaximumWidth( 400 );
  param_input_frame_->hide();

  setup_algo_selector();

  QObject::connect( 
        algo_selector_, 
	      SIGNAL(mouseButtonClicked( int, QListViewItem*, const QPoint&, int ) ),
	      this, 
	      SLOT( 
	        prompt_for_parameters( int, QListViewItem*, const QPoint&, int )
        ) 
      );
 
  QObject::connect( controls_, SIGNAL( run_algo_selected() ),
		    this, SLOT( run_geostat_algo() ) );
  QObject::connect( controls_, SIGNAL( save_selected() ),
		    this, SLOT( save_parameters() ) );
  QObject::connect( controls_, SIGNAL( clear_selected() ),
		    this, SLOT( clear_parameters() ) );
  QObject::connect( controls_, SIGNAL( load_selected() ),
		    this, SLOT( load_parameters() ) );

}
  


void Algo_control_panel::setup_algo_selector() {
  appli_warning( "setting-up the algo selector" << std::endl << std::endl );

  algo_selector_->addColumn( "Algorithm", 120 );
  algo_selector_->addColumn( "Description" );
  algo_selector_->setRootIsDecorated( true );

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( geostatAlgo_manager );
  Manager* mng = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( mng );


  Manager::type_iterator it = mng->begin();
  Manager::type_iterator end = mng->end();

  for( ; it != end ; ++it ) {

    // create a new interface for each known algorithm, and add the algorithm
    // to the selection list. We don't need to keep the interface in the manager 

    //SmartPtr<Named_interface> algo_interface = mng->new_interface( *it, "/"+(*it) );
    SmartPtr<Named_interface> algo_interface = mng->new_interface( *it, "/" );
    Geostat_algo* algo = dynamic_cast<Geostat_algo*>( algo_interface.raw_ptr() );
    appli_assert( algo != 0 );
  
    add_algorithm( algo, *it );

  }
  
}



void Algo_control_panel::add_algorithm( Geostat_algo* algo,
					const std::string& entry_name ) {

  // Look for the .ui file describing the algorithm. 
  SmartPtr<Named_interface> ni = 
      Root::instance()->interface( geostatAlgo_manager );
  Manager* mng = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( mng );

  std::string name( algo->name() );
  
  GsTLlog << "Adding algorithm " << name << " to GUI selector\n";

  std::string ui_filename( mng->plugin_path() + "/" + name + ".ui" );
  QFile ui_file( ui_filename.c_str() );
  if( !ui_file.open( IO_ReadOnly ) ) {
    GsTLlog << "Can't find file " << ui_filename << "\n" 
	    << "Aborting registration of algorithm " << name 
	    << "\n\n";
    return;
  }

 
  //Parse the .ui file and look for the "algorithm" tag

  QDomDocument xml_doc;
  bool parsed = xml_doc.setContent( &ui_file, true );
  if( !parsed ) {
    GsTLlog << "file " << ui_filename << " is not correctly formated. \n"
	    << "Error while parsing " << ui_filename  << "\nAborting\n\n";
    return;
  }
  QDomNodeList node_list = xml_doc.elementsByTagName( "algorithm" );
  

  // retrieve the "name" and "description" attributes

  QString category( "" );
  QString description( "" );

  switch( node_list.count() ) {
  case 0:
    {
      GsTLlog << "Tag 'algorithm' not defined.\n ";
      break;
    }
  case 1:
    {
      QDomNode node = node_list.item(0);
      QDomElement elem = node.toElement();
      category = elem.attribute("category").simplifyWhiteSpace();
      description = elem.attribute( "description" ).simplifyWhiteSpace();
      break;
    }
  default:
     GsTLlog << "Multiple definitions of tag 'algorithm'.\n" ;
    break;
  }

  // make the algorithm available in the algorithm list, 
  // in the correct category.
  algo_selector_->addItem( category + "/" + QString( entry_name.c_str() ),
			   "/",
			   description );
  
}



/** Generate the widget that prompts for the parameters specific to the 
 * chosen algorithm. The widget is generated from a .ui file (XML file)
 * supposed to be located in:
 *   geostat_plugin_path/algo_name.ui
 * where algo_name is the name of the chosen algorithm and geostat_plugin_path 
 * is the directory where geostat algo are stored.
 */
/*
void Algo_control_panel::prompt_for_parameters( int button, QListViewItem* item,
						 const QPoint& , int c ) {
  
  if( !item || button != 1 ) {
    return;
  }
  

  // if the item is not a leaf, or the user didn't click in the 1st column, return
  //if( item->firstChild() || c != 0 ) return;
  if( item->firstChild() ) return;

  selected_algo_ = std::string( item->text(0).latin1() );

  if( param_input_ )
    param_input_->hide();

  param_input_ = widget_manager_.widget( selected_algo_ );
  flagged_widgets_.clear();

  if( !param_input_ ) {
    SmartPtr<Named_interface> ni = 
        Root::instance()->interface( geostatAlgo_manager );
    Manager* mng = dynamic_cast<Manager*>( ni.raw_ptr() );
    appli_assert( mng );

    std::string ui_filename( mng->plugin_path() + "/" + selected_algo_ + ".ui" );
    QFile ui_file( ui_filename.c_str() );
    if( !ui_file.open( IO_ReadOnly ) ) {
      appli_warning( "Can't find file " << ui_filename << std::endl 
		    << "unable to generate input widget for algorithm " << selected_algo_
		    << std::endl );
      return;
    }

    appli_message( "generating interface for algo " << item->text( 0 ) );
  
    param_input_ = 
      GsTL_widget_factory::create( project_, &ui_file, 
		  		 param_input_frame_->viewport(), "input_parameters" );

    widget_manager_.add_widget( selected_algo_, param_input_ );
  }

  param_input_->setBaseSize( 200, this->height() / 2 );

  param_input_frame_->setMaximumHeight( param_input_->maximumHeight() );
  param_input_frame_->addChild( param_input_ );
  param_input_frame_->setResizePolicy( QScrollView::AutoOneFit );
  if( !param_input_frame_->isVisible() )
    param_input_frame_->show();

  if( !param_input_->isVisible() )
    param_input_->show();
}
*/


void Algo_control_panel::prompt_for_parameters( int button, QListViewItem* item,
						 const QPoint& , int c ) {
  
  if( !item || button != 1 ) {
    return;
  }
  

  // if the item is not a leaf, or the user didn't click in the 1st column, return
  //if( item->firstChild() || c != 0 ) return;
  if( item->firstChild() ) return;
  if( selected_algo_ == std::string( item->text(0).latin1() ) ) return;

  //-------------
  // save the current parameters 
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "Value_collector", utils_manager_ + "/"  );
  QWidgets_values_collector* collector =
    dynamic_cast<QWidgets_values_collector*>( ni.raw_ptr() );
  appli_assert( collector );

  if( !selected_algo_.empty() ) {

    std::string algo_parameters =
      collector->widgets_values(param_input_, selected_algo_);

    parameters_manager_.add_parameters( selected_algo_, algo_parameters );
  }

  selected_algo_ = std::string( item->text(0).latin1() );

  SmartPtr<Named_interface> algo_ni = 
      Root::instance()->interface( geostatAlgo_manager );
  Manager* mng = dynamic_cast<Manager*>( algo_ni.raw_ptr() );
  appli_assert( mng );

  std::string ui_filename( mng->plugin_path() + "/" + selected_algo_ + ".ui" );
  QFile ui_file( ui_filename.c_str() );
  if( !ui_file.open( IO_ReadOnly ) ) {
    appli_warning( "Can't find file " << ui_filename << std::endl 
		  << "unable to generate input widget for algorithm " << selected_algo_
		   << std::endl );
    return;
  }

  appli_message( "generating interface for algo " << item->text( 0 ) );
  
  if( param_input_ ) {
    flagged_widgets_.clear();
    delete param_input_;
  }  

  param_input_ = 
    GsTL_widget_factory::create( project_, &ui_file, 
				 param_input_frame_->viewport(), "input_parameters" );

  //------
  // check if we have parameters for this algorithm:
  std::pair<std::string*,bool> found = 
    parameters_manager_.parameters( selected_algo_ );
  if( found.second ) 
    collector->set_widgets_values( *found.first, param_input_ );


  //  param_input->setGeometry( 0,0, 200, 400 );
  param_input_->setBaseSize( 200, this->height() / 2 );
  //  param_input_frame_->setMaximumWidth( param_input->maximumWidth() );

  param_input_frame_->setMaximumHeight( param_input_->maximumHeight() );
  param_input_frame_->addChild( param_input_ );
  param_input_frame_->setResizePolicy( QScrollView::AutoOneFit );
  if( !param_input_frame_->isVisible() )
    param_input_frame_->show();

  if( !param_input_->isVisible() )
    param_input_->show();
}


/*
void Algo_control_panel::prompt_for_parameters( int button, QListViewItem* item,
						 const QPoint& , int c ) {
  
  if( !item || button != 1 ) {
//    param_input_frame_->hide();
//    if( param_input_ )
//      param_input_->hide();
    return;
  }
  

  // if the item is not a leaf, or the user didn't click in the 1st column, return
  //if( item->firstChild() || c != 0 ) return;
  if( item->firstChild() ) return;

  selected_algo_ = std::string( item->text(0).latin1() );

  SmartPtr<Named_interface> ni = 
      Root::instance()->interface( geostatAlgo_manager );
  Manager* mng = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( mng );

  std::string ui_filename( mng->plugin_path() + "/" + selected_algo_ + ".ui" );
  QFile ui_file( ui_filename.c_str() );
  if( !ui_file.open( IO_ReadOnly ) ) {
    appli_warning( "Can't find file " << ui_filename << std::endl 
		  << "unable to generate input widget for algorithm " << selected_algo_
		   << std::endl );
    return;
  }

  appli_message( "generating interface for algo " << item->text( 0 ) );
  
  if( param_input_ ) {
    flagged_widgets_.clear();
    delete param_input_;
  }  

  param_input_ = 
    GsTL_widget_factory::create( project_, &ui_file, 
				 param_input_frame_->viewport(), "input_parameters" );
 
  //  param_input->setGeometry( 0,0, 200, 400 );
  param_input_->setBaseSize( 200, this->height() / 2 );
  //  param_input_frame_->setMaximumWidth( param_input->maximumWidth() );

  param_input_frame_->setMaximumHeight( param_input_->maximumHeight() );
  param_input_frame_->addChild( param_input_ );
  param_input_frame_->setResizePolicy( QScrollView::AutoOneFit );
  if( !param_input_frame_->isVisible() )
    param_input_frame_->show();

  if( !param_input_->isVisible() )
    param_input_->show();
}
*/


void Algo_control_panel::run_geostat_algo() {
  if( !param_input_ ) return;

  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "Value_collector", 
				     utils_manager_ + "/" );
  QWidgets_values_collector* collector =
    dynamic_cast<QWidgets_values_collector*>( ni.raw_ptr() );
  appli_assert( collector );

  std::string algo_parameters = 
    collector->widgets_values( param_input_, selected_algo_ );
  
  appli_message( algo_parameters << std::endl );

  
  QApplication::setOverrideCursor( waitCursor );

  reset_param_input_widget();

  Error_messages_handler_xml error_messages;
  project_->execute( "RunGeostatAlgorithm",
		     selected_algo_ + Actions::separator + utils_manager_ + 
		     Actions::separator + algo_parameters,
         (Error_messages_handler*) &error_messages );
  
  QApplication::restoreOverrideCursor();


  if( !error_messages.empty() ) {
    // inform the user of the reported errors
    report_errors( &error_messages, param_input_ );
    GsTLcerr << "Some of the input parameters were missing or not correct.\n"
             << "Please correct those errors and run the algorithm again\n\n"
             << "The fields containing errors have been highlighted in red.\n"
             << "A message detailing the error will appear if you leave the\n"
             << "mouse pointer on an highlighted field for a few seconds"
             << gstlIO::end;  
  }
}



void Algo_control_panel::report_errors( Error_messages_handler_xml* errors_handler,
                                        QWidget* parent ) {
  QString errors( errors_handler->errors().c_str() );
  errors = "<root>" + errors + "</root>";

  QDomDocument xml_doc;
  QString parsing_errors;
  bool parsed = xml_doc.setContent( errors, &parsing_errors );
  
  if( !parsed ) {
    appli_warning( "Error while parsing xml string: " << std::endl
		  << errors << std::endl
		  << "Reported errors: " << std::endl
		  << parsing_errors );
    return;
  }

  // Get the list of all children. For each child:
  //  - get the child name: "childname",
  //  - find the widget rooted at "parent" called "childname",
  //  - get an accessor for that widget
  //  - set the value of the widget
  QDomElement root_element = xml_doc.documentElement();

  for( QDomNode node = root_element.firstChild(); 
       !node.isNull();
       node=node.nextSibling() ) {
    
    appli_assert( node.isElement() );
    QString childname = node.nodeName();
    
    if( childname == "generic_error" ) {
      GsTLcerr << node.toElement().attribute( "diagnostic" ).ascii() << "\n";
      continue;
    }

    QObjectList* objects = parent->queryList( "QWidget", childname.latin1() );
    
    QObject* obj = objects->first();
    if( !obj ) {
      appli_warning( "No widget called \"" << childname << "\" could be found..."
		    << std::endl << "... skipping" ); 
      continue;
    }
    
    QWidget* current_widget = (QWidget*) obj;
    
    // Get the explanation of the error
    QDomElement elem = node.toElement();
    QString diagnostic = elem.attribute( "diagnostic" );

    // Change the background color of the widget to red and add a tooltip
    // with the explanation of the error
    set_widget_color( current_widget, Qt::red );    
    QToolTip::add( current_widget, diagnostic );
    QWhatsThis::add( current_widget, diagnostic );
    flagged_widgets_.push_back( current_widget );

  }

  
  GsTLcerr << gstlIO::end;  
}



void Algo_control_panel::reset_param_input_widget() {
  if( flagged_widgets_.empty() ) return;
  
  for( unsigned int i= 0; i < flagged_widgets_.size(); i++ ) {
    set_widget_color( flagged_widgets_[i], Qt::white );
    QToolTip::remove( flagged_widgets_[i] );
    QWhatsThis::remove( flagged_widgets_[i] );
  }

  flagged_widgets_.clear();
}




void Algo_control_panel::save_parameters() {
  if( !param_input_ ) return;
  
  QString filename = QFileDialog::getSaveFileName( QString::null, QString::null,
						   this, "Save Parameters",
						   "Choose a file" );

  if( filename == QString::null ) return;

  std::ofstream outfile( filename.latin1() );
  if( !outfile ) {
    GsTLcerr << "Can't create file " << filename.ascii() << "\n" << gstlIO::end;
    return;
  }

  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "Value_collector", utils_manager_ + "/"  );
  QWidgets_values_collector* collector =
    dynamic_cast<QWidgets_values_collector*>( ni.raw_ptr() );
  appli_assert( collector );

  std::string algo_parameters =
    collector->widgets_values(param_input_, selected_algo_);

  outfile << algo_parameters;
  
}



/*
void Algo_control_panel::load_parameters() {

  QString filename = 
    QFileDialog::getOpenFileName( QString::null, 
                                  "Parameter files (*.par);;All files (*.*)",
						                      this, "Load Parameters",
						                      "Choose a file"
                                  );
  
  if( filename == QString::null ) return;

  
  // Open the file and put the content into a string (using a stringstream)

  std::ifstream infile( filename.latin1() );
  if( !infile ) {
    GsTLcerr << "Can't open file " << filename << "\n" << gstlIO::end;
    return;
  }

  std::ostringstream file_content;
  char ch;
  while( file_content && infile.get( ch ) )
    file_content.put( ch );


  // Find the name of the algorithm in the parameter file. To extract it, we need a 
  // "parameters handler".
  SmartPtr<Named_interface> ni_handler = 
    Root::instance()->new_interface( "Parameters_handler://" + file_content.str(),
				     utils_manager_ + "/"  );
  Parameters_handler* handler =
    dynamic_cast<Parameters_handler*>( ni_handler.raw_ptr() );
  appli_assert( handler );
  
  if( !handler->is_ready() ) {
    GsTLcerr << filename << " is not a valid parameter file" << gstlIO::end;
    return;
  }

  std::string algo_name = handler->value( "algorithm.name" );
  if( algo_name.empty() ) {
    GsTLcerr << "Your parameter file should contain a \"algorithm\" field" << gstlIO::end;
    return;
  }

  // If we currently have a different algorithm interface loaded, we need to 
  // generate the requested algorithm interface
  
  if( selected_algo_ != algo_name ) {

    // Select the new algorithm in the algorithm selection tree:
    QListViewItem* item = algo_selector_->findItem( algo_name.c_str(), 0 );
    if( item ) {
      algo_selector_->setSelected( item, true );
      algo_selector_->ensureItemVisible( item );
    }

    if( param_input_ )
      delete param_input_ ;

    SmartPtr<Named_interface> ni_mng = 
      Root::instance()->interface( geostatAlgo_manager );
    Manager* mng = dynamic_cast<Manager*>( ni_mng.raw_ptr() );
    appli_assert( mng );

    // Get the description file of the interface
    std::string ui_filename( mng->plugin_path() + "/" + algo_name + ".ui" );
    QFile ui_file( ui_filename.c_str() );
    if( !ui_file.open( IO_ReadOnly ) ) {
      appli_warning( "Can't find file " << ui_filename << std::endl 
		    << "unable to generate input widget for algorithm " << selected_algo_
		    << std::endl << std::endl );
      return;
    }

    appli_message( "generating interface for algo " << algo_name );
  
    // generate the interface and do some widget geometry set up
    param_input_ = 
      GsTL_widget_factory::create( project_, &ui_file, 
        param_input_frame_->viewport(), "input_parameters" );
    
    param_input_->setBaseSize( 200, this->height() / 2 );
    param_input_frame_->setMaximumHeight( param_input_->maximumHeight() );
    param_input_frame_->addChild( param_input_ );

    param_input_frame_->setResizePolicy( QScrollView::AutoOneFit );
    if( !param_input_frame_->isVisible() )
      param_input_frame_->show();

    if( !param_input_->isVisible() )
      param_input_->show();

    selected_algo_ = algo_name;
  }


  // Get a value-collector that will parse the read parameter string and set 
  // the values of the widgets rooted at param_input_

  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "Value_collector", utils_manager_ + "/"  );
  QWidgets_values_collector* collector =
    dynamic_cast<QWidgets_values_collector*>( ni.raw_ptr() );
  appli_assert( collector );
  
  collector->set_widgets_values( file_content.str(), param_input_ );


  if( !param_input_frame_->isVisible() )
    param_input_frame_->show();

  if( !param_input_->isVisible() )
    param_input_->show();

}
*/

void Algo_control_panel::load_parameters() {

  QString filename = 
    QFileDialog::getOpenFileName( QString::null, 
                                  "Parameter files (*.par);;All files (*.*)",
						                      this, "Load Parameters",
						                      "Choose a file"
                                  );
  
  Algo_control_panel::load_parameters( filename );

}



void Algo_control_panel::load_parameters( const QString& filename ) {

  if( filename == QString::null ) return;

  // Open the file and put the content into a string (using a stringstream)

  std::ifstream infile( filename.latin1() );
  if( !infile ) {
    GsTLcerr << "Can't open file " << filename.ascii() << "\n" << gstlIO::end;
    return;
  }

  std::ostringstream file_content;
  char ch;
  while( file_content && infile.get( ch ) )
    file_content.put( ch );


  // Find the name of the algorithm in the parameter file. To extract it, we need a 
  // "parameters handler".
  SmartPtr<Named_interface> ni_handler = 
    Root::instance()->new_interface( "Parameters_handler://" + file_content.str(),
				     utils_manager_ + "/"  );
  Parameters_handler* handler =
    dynamic_cast<Parameters_handler*>( ni_handler.raw_ptr() );
  appli_assert( handler );
  
  if( !handler->is_ready() ) {
    GsTLcerr << filename.ascii() << " is not a valid parameter file" << gstlIO::end;
    return;
  }

  std::string algo_name = handler->value( "algorithm.name" );
  if( algo_name.empty() ) {
    GsTLcerr << "Your parameter file should contain a \"algorithm\" field" << gstlIO::end;
    return;
  }

  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "Value_collector", utils_manager_ + "/"  );
  QWidgets_values_collector* collector =
    dynamic_cast<QWidgets_values_collector*>( ni.raw_ptr() );
  appli_assert( collector );

  // If we currently have a different algorithm interface loaded, we need to 
  // generate the requested algorithm interface
  
  if( selected_algo_ != algo_name ) {

    // Select the new algorithm in the algorithm selection tree:
    QListViewItem* item = algo_selector_->findItem( algo_name.c_str(), 0 );

    if( !item ) {
      GsTLcerr << "parameter file " << filename.latin1() << " is not valid"
               << gstlIO::end; 
      return;
    }

    algo_selector_->setSelected( item, true );
    algo_selector_->ensureItemVisible( item );

    // save the the current parameters 
    if( !selected_algo_.empty() ) {
      std::string algo_parameters =
        collector->widgets_values( param_input_, selected_algo_ );
      parameters_manager_.add_parameters( selected_algo_, algo_parameters );
    }


    SmartPtr<Named_interface> ni_mng = 
      Root::instance()->interface( geostatAlgo_manager );
    Manager* mng = dynamic_cast<Manager*>( ni_mng.raw_ptr() );
    appli_assert( mng );

    // Get the description file of the interface
    std::string ui_filename( mng->plugin_path() + "/" + algo_name + ".ui" );
    QFile ui_file( ui_filename.c_str() );
    if( !ui_file.open( IO_ReadOnly ) ) {
      appli_warning( "Can't find file " << ui_filename << std::endl 
		    << "unable to generate input widget for algorithm " << selected_algo_
		    << std::endl << std::endl );
      GsTLcerr << "parameter file " << filename.latin1() << " is not valid"
               << gstlIO::end; 
      return;
    }

    if( param_input_ ) {
      delete param_input_ ;
      param_input_ = 0;
    }


    appli_message( "generating interface for algo " << algo_name );
  
    // generate the interface and do some widget geometry set up
    param_input_ = 
      GsTL_widget_factory::create( project_, &ui_file, 
        param_input_frame_->viewport(), "input_parameters" );
    
    param_input_->setBaseSize( 200, this->height() / 2 );
    param_input_frame_->setMaximumHeight( param_input_->maximumHeight() );
    param_input_frame_->addChild( param_input_ );

    param_input_frame_->setResizePolicy( QScrollView::AutoOneFit );
    if( !param_input_frame_->isVisible() )
      param_input_frame_->show();

    if( !param_input_->isVisible() )
      param_input_->show();

    selected_algo_ = algo_name;
  }


  // Get a value-collector that will parse the read parameter string and set 
  // the values of the widgets rooted at param_input_
/*
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "Value_collector", utils_manager_ + "/"  );
  QWidgets_values_collector* collector =
    dynamic_cast<QWidgets_values_collector*>( ni.raw_ptr() );
  appli_assert( collector );
  */
  collector->set_widgets_values( file_content.str(), param_input_ );

  // (just added)
  parameters_manager_.add_parameters( selected_algo_, file_content.str() );

  if( !param_input_frame_->isVisible() )
    param_input_frame_->show();

  if( !param_input_->isVisible() )
    param_input_->show();

}




void Algo_control_panel::clear_parameters() {
  if( !param_input_ ) return;
  
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface( "Value_collector", utils_manager_ + "/"  );
  QWidgets_values_collector* collector =
    dynamic_cast<QWidgets_values_collector*>( ni.raw_ptr() );
  appli_assert( collector );

  collector->clear_widgets_values( param_input_ );

}




void Algo_control_panel::set_widget_color( QWidget* widget, QColor color ) {
  QPalette p;
  p = widget->palette();
  p.setColor( QPalette::Normal, QColorGroup::Base, color );
  widget->setPalette( p );
}




void Algo_control_panel::dragMoveEvent( QDragMoveEvent * ) {
}

void Algo_control_panel::dragEnterEvent( QDragEnterEvent * e ) {
  if( QUriDrag::canDecode( e ) ) {
    e->accept();
  }
}

void Algo_control_panel::dragLeaveEvent( QDragLeaveEvent * ) {
}

void Algo_control_panel::dropEvent( QDropEvent * e ) {
  QStrList strings;
  QUriDrag::decode( e, strings );
  QString m;
  QStringList files;

  if ( QUriDrag::decodeLocalFiles( e, files ) ) {
    QString file= *( files.begin() );
    appli_message( "Loading file " << file );
    Algo_control_panel::load_parameters( file );
  }
}







//=========================================

std::pair<std::string*,bool> 
Parameters_manager::parameters( const std::string& algo_name ) {
  ParamMap::iterator found = map_.find( algo_name );
  if( found == map_.end() ) return std::make_pair( &empty_, false );


  return std::make_pair( &found->second, true );
}


bool Parameters_manager::add_parameters( const std::string& algo_name, 
                                        const std::string& parameters) {
  map_[algo_name] = parameters;
  return true;
}


