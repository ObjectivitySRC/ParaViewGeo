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

#include <GsTLAppli/gui/variogram2/pset_params.h>
#include <GsTLAppli/math/correlation_measure.h>
#include <GsTLAppli/gui/utils/qwidget_value_collector.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/extra/qtplugins/gstl_table.h>

#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qgroupbox.h>

#include <fstream>




Pset_params::Pset_params(QWidget* parent,
					               const char* name )
  : Point_set_base( parent, name )
{
  // disable the standardize option because it doesn't work as expected 
  // in the case of indicator variograms
  standardize_options_box_->hide();

  table_frame_->setOrientation( Qt::Vertical );
  table_frame_->setColumns(1);

  angle_table_ = new GsTLTable( table_frame_, "angle_table_" );
  angle_table_->setNumCols( 7 );
  angle_table_->setNumRows( 1 );
  QStringList headers;
  headers += "azimuth";
  headers += "dip";
  headers += "tolerance";
  headers += "bandwidth";
  headers += "measure type";
  headers += " head indic. cutoff";
  headers += " tail indic. cutoff";
  angle_table_->setColumnLabels( headers );
  for( int i=0; i < 4; i++ ) 
    angle_table_->setColumnWidth( i, 70 );

  //--------------------
  // signal forwarding
  QObject::connect(num_lags_,SIGNAL(valueChanged(int)),
		     this,SIGNAL(parameterChanged()));
  QObject::connect(num_lags_,SIGNAL(valueChanged(const QString &)),
		     this,SIGNAL(parameterChanged()));

  QObject::connect(lag_tol_,SIGNAL(textChanged(const QString &)),
		     this,SIGNAL(parameterChanged()));
    
  QObject::connect(lag_sep_,SIGNAL(textChanged(const QString &)),
		     this,SIGNAL(parameterChanged()));

  QObject::connect(angle_table_,SIGNAL(valueChanged(int,int)),
		     this,SIGNAL(parameterChanged()));

  QObject::connect(standardize_checkbox_,SIGNAL(toggled(bool)),
		     this,SIGNAL(parameterChanged()));

  //--------------------
  // signal-slot connections
  QObject::connect(load_button_, SIGNAL( clicked() ),
                   this, SLOT( load_parameters() ));
  QObject::connect(save_button_, SIGNAL( clicked() ),
                   this, SLOT( save_parameters() ));
  QObject::connect(directions_count_, SIGNAL(valueChanged(int)),
          		     this, SLOT( set_directions_count(int) ));

  const int default_directions_count = 1; 
  directions_count_->setValue( default_directions_count );

  std::vector<std::string> correl_methods =
    Correlation_measure_factory::available_methods();
  
  for( unsigned int i=0; i < correl_methods.size() ; i++ ) {
    methods_list_.append( correl_methods[i].c_str() );
  }

  for ( int row = 0; row < angle_table_->numRows(); row++ ) {
    QComboTableItem* item = 
      new QComboTableItem( angle_table_, methods_list_, FALSE );
    angle_table_->setItem( row, 4, item );
  }

  QWidget::setTabOrder( num_lags_, lag_sep_ );
  QWidget::setTabOrder( lag_sep_, lag_tol_ );
  QWidget::setTabOrder( lag_tol_, directions_count_ );
  QWidget::setTabOrder( directions_count_, angle_table_ );
  QWidget::setTabOrder( angle_table_, standardize_checkbox_ );
  QWidget::setTabOrder( standardize_checkbox_, load_button_ );
  QWidget::setTabOrder( load_button_, save_button_ );


}
    

void Pset_params::set_directions_count( int n ) {
  int old_count = angle_table_->numRows();
  angle_table_->setNumRows( n );
  for( int row = old_count ; row < n ; row++ ) {
    QComboTableItem* item = 
      new QComboTableItem( angle_table_, methods_list_, FALSE );
    angle_table_->setItem( row, 4, item );
  }
}

const QTable * Pset_params::angle_table()
{
    return static_cast<const QTable*>(angle_table_);
}



int Pset_params::num_lags()
{
    return num_lags_->value();
}


double Pset_params::lag_sep() {
  bool is_a_number;
  double val = lag_sep_->text().toDouble( &is_a_number );
  if( is_a_number )
    return val;
  else
    return -1;
}



double Pset_params::lag_tol() {
  bool is_a_number;
  double val = lag_tol_->text().toDouble( &is_a_number );
  if( is_a_number )
    return val;
  else
    return -1;
}




void Pset_params::load_parameters() {
  QString filename = 
    QFileDialog::getOpenFileName( QString::null, QString::null, this, 
                                  "LoadParameters", "Load Parameters" );
  
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


  QWidgets_values_collector_xml collector;
  collector.set_widgets_values( file_content.str(), this );  
}
 


void Pset_params::save_parameters() {
  QString filename = 
    QFileDialog::getSaveFileName( QString::null, QString::null, this, 
                                  "SaveParameters", "Save Parameters" );
  
  if( filename == QString::null ) return;

  QWidgets_values_collector_xml collector;
  std::string params = collector.widgets_values( this, "point_set_variogram" );

  std::ofstream outfile( filename.ascii() );
  if( !outfile ) {
    GsTLcerr << "Can't create file " << filename.ascii() << "\n" << gstlIO::end;
    return;
  }

  outfile << params << std::endl;
}

