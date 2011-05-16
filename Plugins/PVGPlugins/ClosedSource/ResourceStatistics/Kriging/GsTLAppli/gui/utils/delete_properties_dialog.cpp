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

#include <GsTLAppli/gui/utils/delete_properties_dialog.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qpushbutton.h>
#include <qstringlist.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qlabel.h>


Delete_properties_dialog::
Delete_properties_dialog( GsTL_project* proj, QWidget* parent, const char* name )
  : Multichoice_dialog( parent, name ) {

  ok_button_->setText( "&Delete" );

  set_label( "Select properties" );

  appli_assert( layout()->inherits( "QBoxLayout" ) );  
  QBoxLayout* dialog_layout = (QBoxLayout*) layout();
  
  grid_selector_ = new GridSelector( this, "grid_selector", proj );
  dialog_layout->insertWidget( 0, new QLabel( "Delete Properties from: ",
                                              this, "grid_selector_label" ) );
  dialog_layout->insertWidget( 1, grid_selector_ );
  dialog_layout->insertSpacing( 2, 12 );

  QObject::connect( grid_selector_, SIGNAL( activated( const QString& ) ),
                    choice_list_, SLOT( show_properties( const QString& ) ) );

}


QString Delete_properties_dialog::selected_grid() const {
  return grid_selector_->currentText();
}
