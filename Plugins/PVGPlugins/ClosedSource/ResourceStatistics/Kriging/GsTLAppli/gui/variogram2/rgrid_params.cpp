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

#include <GsTLAppli/gui/variogram2/rgrid_params.h>
#include <GsTLAppli/gui/utils/qwidget_value_collector.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/extra/qtplugins/gstl_table.h>
#include <GsTLAppli/math/correlation_measure.h>

#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qgroupbox.h>

#include <fstream>



RGrid_params::
RGrid_params( QWidget* parent, const char* name )
  : RGrid_base( parent, name )
{
  // disable the standardize option because it doesn't work as expected 
  // in the case of indicator variograms
  standardize_options_box_->hide();

  table_frame_->setOrientation( Qt::Vertical );
  table_frame_->setColumns(1);

  vector_table_ = new GsTLTable( table_frame_, "vector_table_" );
  vector_table_->setNumCols( 6 );
  vector_table_->setNumRows( 1 );
  QStringList headers;
  headers += "x";
  headers += "y";
  headers += "z";
  headers += "measure type";
  headers += " head indic. cutoff";
  headers += " tail indic. cutoff";
  vector_table_->setColumnLabels( headers );
//  for( int i=0; i < 4; i++ ) {
//    vector_table_->setColumnWidth( i, 70 );
//  }

  //--------------------
  // signal forwarding
  QObject::connect(num_lags_,SIGNAL(valueChanged(int)),
                   this,SIGNAL(parameterChanged()));
  QObject::connect(num_lags_,SIGNAL(valueChanged(const QString &)),
                   this,SIGNAL(parameterChanged()));
  QObject::connect(vector_table_,SIGNAL(valueChanged(int,int)),
                   this,SIGNAL(parameterChanged()));
  QObject::connect(variance_scale_,SIGNAL(toggled(bool)),
                   this,SIGNAL(parameterChanged()));

  //--------------------
  // signal-slot connections
  QObject::connect(load_button_, SIGNAL( clicked() ),
                   this, SLOT( load_parameters() ));
  QObject::connect(save_button_, SIGNAL( clicked() ),
                   this, SLOT( save_parameters() ));
  QObject::connect(directions_count_, SIGNAL(valueChanged(int)),
          		     this, SLOT( set_directions_count(int) ));


  directions_count_->setValue( 1 );

  std::vector<std::string> correl_methods =
    Correlation_measure_factory::available_methods();
  
  for( unsigned int i=0; i < correl_methods.size() ; i++ ) {
    methods_list_.append( correl_methods[i].c_str() );
  }

  for ( int row = 0; row < vector_table_->numRows(); row++ ) {
    QComboTableItem* item = 
      new QComboTableItem( vector_table_, methods_list_, FALSE );
    vector_table_->setItem( row, 3, item );
  }
 

  QWidget::setTabOrder( num_lags_, directions_count_ );
  QWidget::setTabOrder( directions_count_, vector_table_ );
  QWidget::setTabOrder( vector_table_, variance_scale_ );
  QWidget::setTabOrder( variance_scale_, load_button_ );
  QWidget::setTabOrder( load_button_, save_button_ );
}


void RGrid_params::set_directions_count( int n ) {
  int old_count = vector_table_->numRows();
  vector_table_->setNumRows( n );
  for( int row = old_count ; row < n ; row++ ) {
    QComboTableItem* item = 
      new QComboTableItem( vector_table_, methods_list_, FALSE );
    vector_table_->setItem( row, 3, item );
  }
}


const QTable * RGrid_params::vector_table()
{
    return static_cast<const QTable*>(vector_table_);
}



int RGrid_params::num_lags()
{
    return num_lags_->value();
}

bool RGrid_params::scale_with_variance()
{
    return variance_scale_->isChecked();
}

void RGrid_params::
all_parameters( std::vector<GsTLVector<double> > & d,
                std::vector<std::string > & mod_type,
                std::vector< std::pair<double, double> > &mod_param )
{
  for( int i=0 ; i < vector_table_->numRows() ; i++ ) {
	  if( !vector_table_->text(i,0).isEmpty() ) {
      GsTLVector<int> v( vector_table_->text(i,0).toInt(),
                         vector_table_->text(i,1).toInt(),
                         vector_table_->text(i,2).toInt() );

      d.push_back(v);
      QComboTableItem* combo_item =
        (QComboTableItem*) vector_table_->item( i, 3 );
	    mod_type.push_back( combo_item->currentText().ascii() );
	     
      double thresh1, thresh2;
      bool ok;
      thresh1 = vector_table_->text(i,4).toDouble( &ok );
      if( !ok ) thresh1 = 0;
      thresh2 = vector_table_->text(i,5).toDouble( &ok );
      if( !ok ) thresh2 = 0;
      mod_param.push_back( std::make_pair( thresh1, thresh2 ) );
     
	  }
  }
}
		    



void RGrid_params::load_parameters() {
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
 


void RGrid_params::save_parameters() {
  QString filename = 
    QFileDialog::getSaveFileName( QString::null, QString::null, this, 
                                  "SaveParameters", "Save Parameters" );
  
  if( filename == QString::null ) return;

  QWidgets_values_collector_xml collector;
  std::string params = collector.widgets_values( this, "regular_grid_variogram" );

  std::ofstream outfile( filename.latin1() );
  if( !outfile ) {
    GsTLcerr << "Can't create file " << filename.ascii() << "\n" << gstlIO::end;
    return;
  }

  outfile << params << std::endl;
}
