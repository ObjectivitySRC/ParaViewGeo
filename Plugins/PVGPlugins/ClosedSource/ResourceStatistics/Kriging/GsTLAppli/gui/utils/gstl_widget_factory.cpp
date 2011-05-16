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

#include <GsTLAppli/gui/utils/gstl_widget_factory.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>

#include <qobjectlist.h>
#include <qcombobox.h>

QWidget*
GsTL_widget_factory::create(  GsTL_project* project,
			      QIODevice* dev, 
			      QWidget* parent,
			      const char* name ) {

  QWidget* new_form = QWidgetFactory::create( dev, 0, parent, name );
     

  // Look for widgets inheriting Project_view and initialize them
  // Get the list of all qwidgets and check if they're also of type
  // Project_view
  QObjectList* project_views = 
    new_form->queryList( "QWidget" ); 
  QObjectListIt it( *project_views );
  
  while( it.current()  != 0 ) {
    Project_view* view = dynamic_cast< Project_view* >( it.current() );
//	Project_view* view = (Project_view*) it.current() ;
    if( view ) {
      view->init( project );
    }
    
    ++it;
  }
  delete project_views;


  // Look for widgets inheriting GsTLGroupBox and hide them if they 
  // should be

  QObjectList* GsTLGroups = new_form->queryList( "GsTLGroupBox" ); 
  QObjectListIt it2( *GsTLGroups );
  
  while( it2.current()  != 0 ) {
    GsTLGroupBox* group = dynamic_cast< GsTLGroupBox* >( it2.current() );
    if( group ) {
      if( !group->is_displayed() )
	      group->hide();
      if( !group->is_activated() )
	      group->hide();
    }
    
    ++it2;
  }
  delete GsTLGroups;


  return new_form;
}
