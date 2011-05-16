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

#include <GsTLAppli/gui/utils/qwidget_value_accessor.h>
#include <GsTLAppli/gui/utils/qwidget_value_collector.h>
#include <GsTLAppli/gui/QWidget_value_accessors/default_accessor.h>
#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qobjectlist.h>
#include <qobject.h>
#include <qdom.h>

void 
QWidgets_values_collector::widgets_values( QWidget* parent, 
					   std::string& values,
					   const std::string& accessors_manager ) {

  // Get all the children widgets of the current widget (no recursive 
  // search)
  QObjectList* children = parent->queryList("QWidget", 0, TRUE, FALSE);
  
  // Loop over all the children QObjects
  for( QObject* obj = children->first() ; obj ; obj=children->next() ) {
    const char* name= obj->name() ;
    QWidget* current_widget = (QWidget*) obj;
    appli_assert( current_widget != 0 );
    
    if( !current_widget->isEnabled() /*%|| !current_widget->isShown()*/ ) continue;
    
    // Get an accessor for the current widget
    std::string classname( current_widget->className() );
    
    SmartPtr<Named_interface> ni = 
      Root::instance()->interface( accessors_manager + "/" + classname );
    if( ni.raw_ptr() == 0 ) {
      // The interface does not already exist: create a new one
      ni = Root::instance()->new_interface( classname, 
					    accessors_manager + "/" + classname );
    }
    QWidget_value_accessor* accessor = 
      dynamic_cast<QWidget_value_accessor*>( ni.raw_ptr() );

    
    if( !accessor ) {
      // There is no accessor for that widget (and a new accessor could not be
      // created). Associate it with the default accessor so that next time 
      // such a widget is encountered, we don't need to look for a
      // accessor (which does not exist). 
      // Then recursively call widgets_value(...) on this widget

      SmartPtr<Named_interface> mng_ni = 
      	Root::instance()->interface( accessors_manager );
      Manager* mng = dynamic_cast<Manager*>( mng_ni.raw_ptr() );
      appli_assert( mng );
      mng->factory( classname, Default_accessor::create_new_interface );
      widgets_values( current_widget, values, accessors_manager );
    }
    else {
      accessor->initialize( current_widget );
      if( accessor->is_valid() ) {
      	// We have a valid accessor:
      	// append to "values" a string with the widget's name and its value
      	values += "  " + accessor->value() ;
      }
      else {
      	// The accessor is not valid. Recursively call widgets_value(...)
      	// on this widget
      	widgets_values( current_widget, values, accessors_manager );
      }
      
    }
  }
}



void 
QWidgets_values_collector::clear_widgets_values( QWidget* parent, 
						 const std::string& accessors_manager ) {

  // Get all the children widgets of the current widget (no recursive 
  // search)
  QObjectList* children = parent->queryList("QWidget", 0, TRUE, FALSE);
  
  // Loop over all the children QObjects
  for( QObject* obj = children->first() ; obj ; obj=children->next() ) {
    
    QWidget* current_widget = (QWidget*) obj;
    appli_assert( current_widget != 0 );
    
    if( !current_widget->isEnabled() /*%|| !current_widget->isShown()*/ ) continue;
    
    // Get an accessor for the current widget
    std::string classname( current_widget->className() );
    
    SmartPtr<Named_interface> ni = 
      Root::instance()->interface( accessors_manager + "/" + classname );
    if( ni.raw_ptr() == 0 ) {
      // The interface does not already exist: create a new one
      ni = Root::instance()->new_interface( classname, 
					    accessors_manager + "/" + classname );
    }
    QWidget_value_accessor* accessor = 
      dynamic_cast<QWidget_value_accessor*>( ni.raw_ptr() );

    
    if( !accessor ) {
      // There is no accessor for that widget (and a new accessor could not be
      // created). Associate it with the default accessor so that next time 
      // such a widget is encountered, we don't need to look for a
      // accessor (which does not exist). 
      // Then recursively call widgets_value(...) on this widget

      SmartPtr<Named_interface> mng_ni = 
	Root::instance()->interface( accessors_manager );
      Manager* mng = dynamic_cast<Manager*>( mng_ni.raw_ptr() );
      appli_assert( mng );
      mng->factory( classname, Default_accessor::create_new_interface );
      clear_widgets_values( current_widget, accessors_manager );
    }
    else {
      accessor->initialize( current_widget );
      if( accessor->is_valid() ) {
	// We have a valid accessor: clear the widget value
	accessor->clear() ;
      }
      else {
	// The accessor is not valid. Recursively call widgets_value(...)
	// on this widget
	clear_widgets_values( current_widget, accessors_manager );
      }
      
    }
  }
}





std::string
QWidgets_values_collector_xml::widgets_values( QWidget* parent,
					       const std::string& geostat_algo_name ) {
  std::string values;
  QWidgets_values_collector::widgets_values( parent, values, 
					     xmlGeostatParamUtils_manager );
  values = "<parameters>  <algorithm name=\"" + geostat_algo_name + "\" /> \n"
    + values + "</parameters> \n";

  return values;
}



void 
QWidgets_values_collector_xml::set_widgets_values( const std::string& values,
						   QWidget* parent ) {

  appli_message( "setting widgets values " );

  QDomDocument xml_doc;
  QString parsing_errors;
  bool parsed = xml_doc.setContent( QString( values.c_str() ), &parsing_errors );
  if( !parsed ) {
    appli_warning( "Error while parsing xml string: " << std::endl
		  << values << std::endl
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
    appli_message( "dealing with node \"" << childname << "\"" );

    QObjectList* objects = 
      parent->queryList( "QWidget", childname.latin1(), false );
    
    QObject* obj = objects->first();
    if( !obj ) {
      appli_warning( "No widget called \"" << childname << "\" could be found..."
		    << std::endl << "... skipping" ); 
      continue;
    }
    
    QWidget* current_widget = (QWidget*) obj;
 
    if( !current_widget->isEnabled() ) 
      current_widget->setEnabled( true );
    
    if( !current_widget->isShown() ) { 
      current_widget->show();
      current_widget->setEnabled( true );
    }
    
    // Get an accessor for the current widget
    std::string classname( current_widget->className() );
    SmartPtr<Named_interface> ni = 
      Root::instance()->interface( xmlGeostatParamUtils_manager + "/" + classname );
    if( ni.raw_ptr() == 0 ) {
      // The interface does not already exist: create a new one
      ni = Root::instance()->new_interface( 
				 classname, 
				 xmlGeostatParamUtils_manager + "/" + classname
			       );
    }
    QWidget_value_accessor* accessor = 
      dynamic_cast<QWidget_value_accessor*>( ni.raw_ptr() );

    if( !accessor ) {
      // There is no accessor for that widget (and a new accessor could not be
      // created). Associate it with the default accessor so that next time 
      // such a widget is encountered, we don't need to look for a
      // accessor (which does not exist). 
      appli_warning( "no accessor found for widget " << classname );
      SmartPtr<Named_interface> mng_ni = 
	Root::instance()->interface( xmlGeostatParamUtils_manager );
      Manager* mng = dynamic_cast<Manager*>( mng_ni.raw_ptr() );
      appli_assert( mng );
      mng->factory( classname, Default_accessor::create_new_interface );
      continue;
    }
    else {
      appli_message( "accessor for widget " << classname << " found" );
      accessor->initialize( current_widget );
      if( accessor->is_valid() ) {
	// We have a valid accessor:
	// convert the current DomNode to a string and pass it to the
	// accessor 
	QString node_string;
	QTextStream stream( &node_string, IO_WriteOnly );
	node.save( stream, 2 );
	appli_message( "setting value: \"" << node_string << "\"" );
	accessor->set_value( node_string.latin1() );
      }
    }
  }
}




void QWidgets_values_collector_xml::clear_widgets_values( QWidget* parent ) {

  QWidgets_values_collector::clear_widgets_values( parent, 
						   xmlGeostatParamUtils_manager );
}


Named_interface* 
QWidgets_values_collector_xml::create_new_interface(std::string&) {
  return new QWidgets_values_collector_xml;
}
