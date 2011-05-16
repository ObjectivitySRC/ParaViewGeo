/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "filters" module of the Geostatistical Earth
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

#include <GsTLAppli/filters/gslib/filter_qt_dialogs.h>
#include <GsTLAppli/filters/gslib/gslib_filter.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qtextbrowser.h>
#include <qcheckbox.h>
#include <qmessagebox.h>

#include <iostream>

Gslib_infilter_dialog::Gslib_infilter_dialog( QWidget* parent, const char* name )
: GslibImportWizard(parent, name ),
  filter_( 0 ), 
  file_stream_( 0 ) {
  
  SmartPtr<Named_interface> ni = 
      Root::instance()->interface( gslibInputFilters_manager );
  Manager* mng = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( mng );

  Manager::type_iterator begin = mng->begin();
  Manager::type_iterator end = mng->end();
  for( ; begin != end ; ++begin ) {
    ObjectType->insertItem( QString( begin->c_str() ) );
  }

  create_specialized_filter( ObjectType->currentText() );
  
  ObjectType->setFocus();

  QObject::connect( ObjectType, SIGNAL(activated( const QString& )), 
		    this, SLOT( create_specialized_filter( const QString& ) ) );

}


Gslib_infilter_dialog::~Gslib_infilter_dialog(){
}



void Gslib_infilter_dialog::set_file( std::ifstream& infile ) {
  file_stream_ = &infile;

  std::streampos mark = infile.tellg();

  QString text;
  std::string line;

  int count = 0;
  const int limit = 50;
 
  // Read up to "limit" lines
  while( std::getline( infile, line ) && count < limit ) {
    line += "\n";
    text.append( line.c_str() );
    count ++;
  } 
    
  TextPreview->setText( text );
  
  // set the stream back to where it was
  infile.clear();
  infile.seekg( mark );
}


void 
Gslib_infilter_dialog::create_specialized_filter( const QString& obj_type ) {
  if( obj_type.isEmpty() ) return;

  std::string type( obj_type.latin1() );

  if( filter_ ) {
    if( filter_->object_filtered() == type ) return;
  }


  nextButton()->setFocus();

  SmartPtr<Named_interface> ni = 
      Root::instance()->interface( gslibInputFilters_manager + "/" + type );
  
  if( ni.raw_ptr() == 0 ) {
    ni = Root::instance()->new_interface( type,
					  gslibInputFilters_manager + "/" + type );
  }

  filter_ = dynamic_cast<Gslib_specialized_infilter*>( ni.raw_ptr() );
  appli_assert( filter_ );

  // If a filter previously added a new page to the wizard, remove that page
  if( pageCount() > 1 )
    removePage( page(1) );

  // ask the specialized filter to give its page widget. If widget is 0, 
  // enable Finish (ie there is no need for additional information).
  // if != 0, enable Next. 

  QWidget* page1_ = filter_->init_dialog( *file_stream_ );
  if( page1_ == 0 ) {
    setFinishEnabled( currentPage(), true );
  }
  else {
    insertPage( page1_, "Additional Parameters", 1 );
    setFinishEnabled( page(1), true );
    finishButton()->setFocus();
  }
}


void Gslib_infilter_dialog::accept() {
  if( !filter_->has_valid_parameters() ) {
    QMessageBox::warning( this, "Missing parameters", 
                          "A name for the new object must be provided",
                          QMessageBox::Ok, QMessageBox::NoButton,
                          QMessageBox::NoButton );
  }
  else {
    QDialog::accept();
  }

}



//==============================================================
Gslib_input_mgrid_dialog::Gslib_input_mgrid_dialog( QWidget* parent, 
												   const char* name ) 
 : GslibMaskedImport( parent, name )
{
	name_->setFocus();
}
int Gslib_input_mgrid_dialog::nx() const {
	return nx_->text().toInt();
}

int Gslib_input_mgrid_dialog::ny() const {
  return ny_->text().toInt();
}

int Gslib_input_mgrid_dialog::nz() const {
	return nz_->text().toInt();
}


float Gslib_input_mgrid_dialog::x_size() const {
  QString val = x_size_->text();
  return val.toFloat();
}
float Gslib_input_mgrid_dialog::y_size() const{
  QString val = y_size_->text();
  return val.toFloat();
}
float Gslib_input_mgrid_dialog::z_size() const{
  QString val = z_size_->text();
  return val.toFloat();
}
float Gslib_input_mgrid_dialog::Ox() const{
  QString val = xmn_->text();
  return val.toFloat();
}
float Gslib_input_mgrid_dialog::Oy() const{
  QString val = ymn_->text();
  return val.toFloat();
}
float Gslib_input_mgrid_dialog::Oz() const{
  QString val = zmn_->text();
  return val.toFloat();
}

QString Gslib_input_mgrid_dialog::name() const {
  return name_->text();
}



int Gslib_input_mgrid_dialog::X_column() const {
  return X_col_->value();
}
int Gslib_input_mgrid_dialog::Y_column() const {
  return Y_col_->value();
}
int Gslib_input_mgrid_dialog::Z_column() const {
  return Z_col_->value();
}

bool Gslib_input_mgrid_dialog::use_no_data_value() const {
  return use_no_data_value_->isChecked();
}

float Gslib_input_mgrid_dialog::no_data_value() const {
  QString val = no_data_value_edit_->text();
  return val.toFloat();
}

//==============================================================

Gslib_input_grid_dialog::Gslib_input_grid_dialog( QWidget* parent, 
						  const char* name )
  : GslibGridImport( parent, name ) {
  name_->setFocus();
}

int Gslib_input_grid_dialog::nx() const {
  return nx_->value();
}

int Gslib_input_grid_dialog::ny() const {
  return ny_->value();
}

int Gslib_input_grid_dialog::nz() const {
  return nz_->value();
}


float Gslib_input_grid_dialog::x_size() const {
  QString val = x_size_->text();
  return val.toFloat();
}
float Gslib_input_grid_dialog::y_size() const{
  QString val = y_size_->text();
  return val.toFloat();
}
float Gslib_input_grid_dialog::z_size() const{
  QString val = z_size_->text();
  return val.toFloat();
}
float Gslib_input_grid_dialog::Ox() const{
  QString val = xmn_->text();
  return val.toFloat();
}
float Gslib_input_grid_dialog::Oy() const{
  QString val = ymn_->text();
  return val.toFloat();
}
float Gslib_input_grid_dialog::Oz() const{
  QString val = zmn_->text();
  return val.toFloat();
}

QString Gslib_input_grid_dialog::name() const {
  return name_->text();
}


bool Gslib_input_grid_dialog::use_no_data_value() const {
  return use_no_data_value_->isChecked();
}

float Gslib_input_grid_dialog::no_data_value() const {
  QString val = no_data_value_edit_->text();
  return val.toFloat();
}


//==========================================================

Gslib_input_pointset_dialog::Gslib_input_pointset_dialog( QWidget* parent , 
							  const char* name ) 
  : GslibPointsetImport( parent, name ) {
  name_->setFocus();
}
  
Gslib_input_pointset_dialog::Gslib_input_pointset_dialog( std::ifstream& infile,
							  QWidget* parent , 
							  const char* name ) 
  : GslibPointsetImport( parent, name ) {
  
  std::streampos mark = infile.tellg();

  // read how many properties are defined in the file (indicated at the
  // second line of the file)
  std::string line;

  std::getline( infile, line );
  int nb_properties;
  infile >> nb_properties;
  infile.clear();
  infile.seekg( mark );

  X_col_->setMaxValue( nb_properties );
  Y_col_->setMaxValue( nb_properties );
  Z_col_->setMaxValue( nb_properties );

  // make a preview of the file

  QString text;

  int count = 0;
  const int limit = 50;
 
  // Read up to "limit" lines
  while( std::getline( infile, line ) && count < limit ) {
    line += "\n";
    text.append( line.c_str() );
    count ++;
  } 
    
  text_preview_->setText( text );
  
  // set the stream back to where it was
  infile.clear();
  infile.seekg( mark );

  name_->setFocus();
}
  

QString Gslib_input_pointset_dialog::name() const {
  return name_->text();
}
int Gslib_input_pointset_dialog::X_column() const {
  return X_col_->value();
}
int Gslib_input_pointset_dialog::Y_column() const {
  return Y_col_->value();
}
int Gslib_input_pointset_dialog::Z_column() const {
  return Z_col_->value();
}
  

bool Gslib_input_pointset_dialog::use_no_data_value() const {
  return use_no_data_value_->isChecked();
}

float Gslib_input_pointset_dialog::no_data_value() const {
  QString val = no_data_value_edit_->text();
  return val.toFloat();
}


