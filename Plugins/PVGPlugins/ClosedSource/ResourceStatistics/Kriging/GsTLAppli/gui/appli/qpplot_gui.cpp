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

#include <GsTLAppli/gui/appli/qpplot_gui.h>
#include <GsTLAppli/gui/appli/qpplot_control_panel.h>
#include <GsTLAppli/extra/qwt/qwt_plot.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/math/qpplot.h>

#include <qlabel.h>
#include <qgroupbox.h>
#include <qscrollview.h>
#include <qlayout.h>
#include <qtimer.h>

typedef std::pair<std::string,std::string> Pair;

//===============================================


QPplot_gui::QPplot_gui( GsTL_project* project,
                        QWidget* parent, const char* name ) 
  : Data_analysis_gui( parent, name ){
 
  setCaption( "QQ/PP Plot" );

  qpploter_ = new QPplot();

/*
  QScrollView* scrollview = new QScrollView( controls_box_, "scrollview" );
  scrollview->setFrameShape( QFrame::Panel );
  scrollview->setFrameShadow( QFrame::Sunken );
  scrollview->setLineWidth( 1 );
  scrollview->setMidLineWidth( 0 );
  control_panel_ = 
    new QPplot_control_panel( project, scrollview->viewport(), "control_panel" );
  scrollview->addChild( control_panel_ );
  scrollview->setResizePolicy( QScrollView::AutoOneFit );
*/
  init_scrollview();
  control_panel_ = 
    new QPplot_control_panel( project, scrollview_->viewport(), "control_panel" );
  scrollview_->addChild( control_panel_ );


  // Set up the statitics labels
  QStringList stats_labels;
  stats_labels << "X variable" << " " << "Number of data"
               << "Mean" << "Variance" 
               << "  " << "  " << "  " << "  " << "   "
               << "Y variable" << " " << "Number of data"
               << "Mean" << "Variance";
  set_stats_labels( stats_labels, 5 );


  plot_->setAxisOptions( QwtPlot::yLeft, QwtAutoScale::Floating );

  // set the plotting style (curve style, etc)
  QwtSymbol sym( QwtSymbol::Ellipse, QBrush( Qt::black ), QPen( Qt::black ),
                 QSize( 6,6 ) );
  plot_->setCurveSymbol( curve_id_, sym );
  plot_->setCurveStyle( curve_id_, QwtCurve::Dots );


  // Signal-slot connections
  QObject::connect( control_panel_, 
                    SIGNAL( var1_changed( const GsTLGridProperty* ) ),
                    this,
                    SLOT( get_var1_data_from( const GsTLGridProperty* ) ) );
  
  QObject::connect( control_panel_, 
                    SIGNAL( var2_changed( const GsTLGridProperty* ) ),
                    this,
                    SLOT( get_var2_data_from( const GsTLGridProperty* ) ) );
   
  QObject::connect( control_panel_, 
                    SIGNAL( analysis_type_changed( const QString& ) ),
                    this, SLOT( set_analysis_type( const QString& ) ) );
  
  QObject::connect( control_panel_, SIGNAL( var1_low_clip_changed(float) ),
                    this, SLOT( update_var1_low_clip(float) ) );
  QObject::connect( control_panel_, SIGNAL( var1_high_clip_changed(float) ),
                    this, SLOT( update_var1_high_clip(float) ) );
  QObject::connect( control_panel_,
                    SIGNAL( reset_var1_clipping_values_clicked() ),
                    this, SLOT( reset_var1_clipping_values() ) );

  QObject::connect( control_panel_, SIGNAL( var2_low_clip_changed(float) ),
                    this, SLOT( update_var2_low_clip(float) ) );
  QObject::connect( control_panel_, SIGNAL( var2_high_clip_changed(float) ),
                    this, SLOT( update_var2_high_clip(float) ) );
  QObject::connect( control_panel_,
                    SIGNAL( reset_var2_clipping_values_clicked() ),
                    this, SLOT( reset_var2_clipping_values() ) );

}


QPplot_gui::~QPplot_gui() {
  clean();
}


void QPplot_gui::clean() {
  delete qpploter_;
}


void QPplot_gui::update_all() {
  int size;
  std::pair<double*,double*> curve = qpploter_->plotting_data(size);
  refresh_plot(  curve, size);
  refresh_stats();  
}


void QPplot_gui::update_var1_low_clip( float val ) {
  qpploter_->low_clip( QPplot::Xvar, val );
  update_all();
}

void QPplot_gui::update_var1_high_clip( float val ) {
  qpploter_->high_clip( QPplot::Xvar, val );
  update_all();
}

void QPplot_gui::update_var2_low_clip( float val ) {
  qpploter_->low_clip( QPplot::Yvar, val );
  update_all();  
}

void QPplot_gui::update_var2_high_clip( float val ) {
  qpploter_->high_clip( QPplot::Yvar, val );
  update_all();  
}

void QPplot_gui::reset_var1_clipping_values() {
  float min = qpploter_->min( QPplot::Xvar );
  float max = qpploter_->max( QPplot::Xvar );

  control_panel_->set_var1_clipping_values( min, max );
  qpploter_->low_clip( QPplot::Xvar, min );
  qpploter_->high_clip( QPplot::Xvar, max );

  update_all();
}

void QPplot_gui::reset_var2_clipping_values() {
  float min = qpploter_->min( QPplot::Yvar );
  float max = qpploter_->max( QPplot::Yvar );

  control_panel_->set_var2_clipping_values( min, max );
  qpploter_->low_clip( QPplot::Yvar, min );
  qpploter_->high_clip( QPplot::Yvar, max );

  update_all();
}
 

void QPplot_gui::get_var1_data_from( const GsTLGridProperty* prop ) {
  if( !prop ) return;

  qpploter_->set_x_data( prop->begin(), prop->end() );
  control_panel_->set_var1_clipping_values( qpploter_->low_clip( QPplot::Xvar ),
                                            qpploter_->high_clip( QPplot::Xvar ) );

  plot_->setAxisTitle( QwtPlot::xBottom, prop->name().c_str() );
  update_all();
}


void QPplot_gui::get_var2_data_from( const GsTLGridProperty* prop ) {
  if( !prop ) return;

  qpploter_->set_y_data( prop->begin(), prop->end() );
  control_panel_->set_var2_clipping_values( qpploter_->low_clip( QPplot::Yvar ),
                                            qpploter_->high_clip( QPplot::Yvar ) );
  
  plot_->setAxisTitle( QwtPlot::yLeft, prop->name().c_str() );
  update_all();
}


void QPplot_gui::set_analysis_type( const QString& type ) {
  if( type == "QQ-plot" ) 
    qpploter_->analysis_type( QPplot::QQplot );
  if( type == "PP-plot" ) 
    qpploter_->analysis_type( QPplot::PPplot );

  int size;
  std::pair<double*,double*> curve = qpploter_->plotting_data(size);
  refresh_plot(  curve, size);
}
 

void QPplot_gui::refresh_stats() {
  update_stats_value( 2, qpploter_->data_count( QPplot::Xvar ) ); //var X data count
  update_stats_value( 3, qpploter_->mean( QPplot::Xvar ) ); // var X mean
  update_stats_value( 4, qpploter_->var( QPplot::Xvar ) ); // var X variance

  update_stats_value( 12, qpploter_->data_count( QPplot::Yvar ) );  // var Y data count
  update_stats_value( 13, qpploter_->mean( QPplot::Yvar ) );  // var Y mean
  update_stats_value( 14, qpploter_->var( QPplot::Yvar ) );  // var Y variance
}



void QPplot_gui::build_stats()
{
	QString val;
	_stats.clear();

	_stats.push_back(Pair("Variable X",""));
	val.setNum(qpploter_->data_count( QPplot::Xvar ));
	_stats.push_back(Pair("Data count: ", val.ascii()));

	val.setNum(qpploter_->mean( QPplot::Xvar ));
	_stats.push_back(Pair("Mean: ", val.ascii()));

	val.setNum(qpploter_->var( QPplot::Xvar ));
	_stats.push_back(Pair("Variance: ", val.ascii()));

	_stats.push_back(Pair("Variable Y",""));

	val.setNum(qpploter_->data_count( QPplot::Yvar ));
	_stats.push_back(Pair("Data count: ", val.ascii()));

	val.setNum(qpploter_->mean( QPplot::Yvar ));
	_stats.push_back(Pair("Mean: ", val.ascii()));

	val.setNum(qpploter_->var( QPplot::Yvar ));
	_stats.push_back(Pair("Variance: ", val.ascii()));

}

void QPplot_gui::paint_stats( QPainter& painter ) {
  QString val;

  painter.drawText( 0,0, "Variable X" );
  painter.translate( 0, 20 );

  val.setNum( qpploter_->data_count( QPplot::Xvar ) );
  painter.drawText( 0,0, "Data count: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );

  val.setNum( qpploter_->mean( QPplot::Xvar ) );
  painter.drawText( 0,0, "Mean: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );

  val.setNum( qpploter_->var( QPplot::Xvar ) );
  painter.drawText( 0,0, "Variance: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 25 );

  painter.drawText( 0,0, "Variable Y" );
  painter.translate( 0, 20 );

  val.setNum( qpploter_->data_count( QPplot::Yvar ) );
  painter.drawText( 0,0, "Data count: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );

  val.setNum( qpploter_->mean( QPplot::Yvar ) );
  painter.drawText( 0,0, "Mean: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );

  val.setNum( qpploter_->var( QPplot::Yvar ) );
  painter.drawText( 0,0, "Variance: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 25 );

}