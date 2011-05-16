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

#include <GsTLAppli/gui/utils/data_analysis_save_dialog.h>

#include <qcheckbox.h>
#include <qlabel.h>


Data_analysis_save_dialog::
Data_analysis_save_dialog( const QString& dirName, 
                       const QString& filter,
                       QWidget* parent, const char* name ) 
  : QFileDialog( dirName, filter, parent, name, true ) {

  addWidgets( new QLabel("Extra Options ", this, "options"), 0, 0 );
  
  stats_checkbox_ = new QCheckBox( "Write summary statistics", this, "box" );
  stats_checkbox_->setChecked( true );
  addWidgets( 0, stats_checkbox_, 0 );

  grid_checkbox_ = new QCheckBox( "Paint grid", this, "box" );
  grid_checkbox_->setChecked( true );
  addWidgets( 0, grid_checkbox_, 0 );
  
  // No postscript mode in Windows because of some limitations of QPrinter on
  // that plateform
//#if defined(WIN32) || defined(_WIN32)
  //setFilters( QString("PNG format (*.png);;BMP format (*.bmp)") );
//#else
  setFilters( QString("PNG format (*.png);;BMP format (*.bmp);;Postscript (*.ps)") );
//#endif
  
  setMode( QFileDialog::AnyFile );
}


bool Data_analysis_save_dialog::write_stats_required() const {
  return stats_checkbox_->isChecked();
}


bool Data_analysis_save_dialog::paint_grid_required() const {
  return grid_checkbox_->isChecked();
}


