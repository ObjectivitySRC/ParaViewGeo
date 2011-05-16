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

#include <GsTLAppli/gui/appli/scatterplot_gui.h>
#include <GsTLAppli/gui/appli/scatterplot_control_panel.h>
#include <GsTLAppli/extra/qwt/qwt_plot.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/math/scatterplot.h>
#include <GsTLAppli/extra/qwt/simpleps.h>

#include <qlabel.h>
#include <qgroupbox.h>
#include <qscrollview.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <math.h>
#include <fstream>

typedef std::pair<std::string,std::string> Pair;

void Scatterplot_gui::savePostScript(SimplePs & ps)
{
	Data_analysis_gui::savePostScript(ps);
	if (!control_panel_->ls_fit_shown())
		return;
	QwtPlotCurve* c1 = plot_->curve(regression_line_curve_id_);
	int style1 = c1->style();
	QArray<double>  & x1 = c1->dataX();
	QArray<double>  & y1 = c1->dataY();
	ps.drawCurve(x1,y1,style1);
}

Scatterplot_gui::Scatterplot_gui( GsTL_project* project,
                                  QWidget* parent, const char* name ) 
  : Data_analysis_gui( parent, name ),
    regression_line_curve_id_( -1 ) {

  setCaption( "Scatterplot" );

  plotter_ = new Scatter_plot();

  init_scrollview();
  control_panel_ = 
    new Scatterplot_control_panel( project, scrollview_->viewport(),
                                   "control_panel" );
  scrollview_->addChild( control_panel_ );


  // Set up the statitics labels
  QStringList stats_labels;
  stats_labels << "plotted data " << "  " << "Coef. correl " << "  " << "  ";
  stats_labels << "    " << "    " << "    " << "    " << "    ";
  stats_labels << "X variable" << " " << "Number of data "
               << "Mean " << "Variance: " 
               << "  " << "  " << "  " << "  " << "   "
               << "Y variable" << " " << "Number of data "
               << "Mean " << "Variance ";
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

  QObject::connect( control_panel_,
                    SIGNAL( show_ls_fit( bool ) ),
                    this, SLOT( draw_regression_line( bool ) ) );

}


Scatterplot_gui::~Scatterplot_gui() {
  clean();
}


void Scatterplot_gui::clean() {
  delete plotter_;
}


void Scatterplot_gui::update_all() {
//  plot_->removeCurve( regression_line_curve_id_ );
  
  int size;
  std::pair<double*,double*> curve = plotter_->plotting_data(size);
  refresh_plot(  curve, size);
  draw_regression_line( control_panel_->ls_fit_shown() );
  refresh_stats();  
}


void Scatterplot_gui::update_var1_low_clip( float val ) {
  plotter_->low_clip( Scatter_plot::Xvar, val );
  update_all();
}

void Scatterplot_gui::update_var1_high_clip( float val ) {
  plotter_->high_clip( Scatter_plot::Xvar, val );
  update_all();
}

void Scatterplot_gui::update_var2_low_clip( float val ) {
  plotter_->low_clip( Scatter_plot::Yvar, val );
  update_all();  
}

void Scatterplot_gui::update_var2_high_clip( float val ) {
  plotter_->high_clip( Scatter_plot::Yvar, val );
  update_all();  
}

void Scatterplot_gui::reset_var1_clipping_values() {
  float min = plotter_->min( Scatter_plot::Xvar );
  float max = plotter_->max( Scatter_plot::Xvar );

  control_panel_->set_var1_clipping_values( min, max );
  plotter_->low_clip( Scatter_plot::Xvar, min );
  plotter_->high_clip( Scatter_plot::Xvar, max );

  update_all();
}

void Scatterplot_gui::reset_var2_clipping_values() {
  float min = plotter_->min( Scatter_plot::Yvar );
  float max = plotter_->max( Scatter_plot::Yvar );

  control_panel_->set_var2_clipping_values( min, max );
  plotter_->low_clip( Scatter_plot::Yvar, min );
  plotter_->high_clip( Scatter_plot::Yvar, max );

  update_all();
}


void Scatterplot_gui::get_var1_data_from( const GsTLGridProperty* prop ) {
  if( !prop ) return;

  plotter_->set_data( Scatter_plot::Xvar, 
                      prop->begin(false), prop->end(), GsTLGridProperty::no_data_value );
  control_panel_->set_var1_clipping_values( plotter_->low_clip( Scatter_plot::Xvar ),
                                            plotter_->high_clip( Scatter_plot::Xvar ) );

  plot_->setAxisTitle( QwtPlot::xBottom, prop->name().c_str() );
  update_all();
}

 
void Scatterplot_gui::get_var2_data_from( const GsTLGridProperty* prop ) {
  if( !prop ) return;

  plotter_->set_data( Scatter_plot::Yvar, 
                      prop->begin(false), prop->end(), GsTLGridProperty::no_data_value );
  control_panel_->set_var2_clipping_values( plotter_->low_clip( Scatter_plot::Yvar ),
                                            plotter_->high_clip( Scatter_plot::Yvar ) );

  plot_->setAxisTitle( QwtPlot::yLeft, prop->name().c_str() );
  update_all();
}

 

void Scatterplot_gui::refresh_stats() {
  QwtCurve* curve = plot_->curve( curve_id_ );
  int data_size = 0;
  if( curve )
    data_size = curve->dataSize();
 
  update_stats_value( 0, data_size ); //plotted data
  update_stats_value( 2, plotter_->correlation() ); //var X data count
  update_stats_value( 12, plotter_->data_count( Scatter_plot::Xvar ) ); //var X data count
  update_stats_value( 13, plotter_->mean( Scatter_plot::Xvar ) ); // var X mean
  update_stats_value( 14, plotter_->var( Scatter_plot::Xvar ) ); // var X variance

  update_stats_value( 22, plotter_->data_count( Scatter_plot::Yvar ) );  // var Y data count
  update_stats_value( 23, plotter_->mean( Scatter_plot::Yvar ) );  // var Y mean
  update_stats_value( 24, plotter_->var( Scatter_plot::Yvar ) );  // var Y variance

  std::pair<float,float> lsfit = plotter_->least_sq_fit();
  control_panel_->show_lsfit_coeffs( lsfit.first, lsfit.second );
}


//TL modified
void Scatterplot_gui::build_stats()
{
	QString val;
	_stats.clear();
	QwtCurve* curve = plot_->curve( curve_id_ );
	if( curve )
		val.setNum( curve->dataSize() );
	else
		val.setNum( 0 );

	_stats.push_back(Pair("plotted data: ",val.ascii()));

	val.setNum(plotter_->correlation());
	_stats.push_back(Pair("Correlation: ",val.ascii()));

	_stats.push_back(Pair("Variable X",""));

	val.setNum(plotter_->mean(Scatter_plot::Xvar));
	_stats.push_back(Pair("Mean: ",val.ascii()));

	val.setNum(plotter_->var(Scatter_plot::Xvar));
	_stats.push_back(Pair("Variance: ",val.ascii()));

	_stats.push_back(Pair("Variable Y",""));

	val.setNum(plotter_->mean(Scatter_plot::Yvar));
	_stats.push_back(Pair("Mean: ",val.ascii()));

	val.setNum(plotter_->var(Scatter_plot::Yvar));
	_stats.push_back(Pair("Variance: ",val.ascii()));

    if( control_panel_->ls_fit_shown() ) {
		_stats.push_back(Pair("Linear Regression Line",""));

		std::pair<float,float> lsfit = plotter_->least_sq_fit();
		val.setNum( lsfit.first );
		_stats.push_back(Pair("Slope: ",val.ascii()));
		val.setNum( lsfit.second );
		_stats.push_back(Pair("Intercept: ",val.ascii()));

	}

}

void Scatterplot_gui::paint_stats( QPainter& painter ) {
  QString val;
  const int hspace = 90;

  QwtCurve* curve = plot_->curve( curve_id_ );
  if( curve )
    val.setNum( curve->dataSize() );
  else
    val.setNum( 0 );

  painter.drawText( 0,0, "plotted data: " );
  painter.drawText( hspace,0, val );
  painter.translate( 0, 25 );

  val.setNum( plotter_->correlation() );
  painter.drawText( 0,0, "Correlation: " );
  painter.drawText( hspace,0, val );
  painter.translate( 0, 25 );

  painter.drawText( 0,0, "Variable X" );
  painter.translate( 0, 20 );

  val.setNum( plotter_->mean( Scatter_plot::Xvar ) );
  painter.drawText( 0,0, "Mean: " );
  painter.drawText( hspace,0, val );
  painter.translate( 0, 15 );

  val.setNum( plotter_->var( Scatter_plot::Xvar ) );
  painter.drawText( 0,0, "Variance: " );
  painter.drawText( hspace,0, val );
  painter.translate( 0, 25 );

  painter.drawText( 0,0, "Variable Y" );
  painter.translate( 0, 20 );

  val.setNum( plotter_->mean( Scatter_plot::Yvar ) );
  painter.drawText( 0,0, "Mean: " );
  painter.drawText( hspace,0, val );
  painter.translate( 0, 15 );

  val.setNum( plotter_->var( Scatter_plot::Yvar ) );
  painter.drawText( 0,0, "Variance: " );
  painter.drawText( hspace,0, val );
  painter.translate( 0, 25 );

  if( control_panel_->ls_fit_shown() ) {
    painter.drawText( 0,0, "Linear Regression Line" );
    painter.translate( 0, 20 );

    std::pair<float,float> lsfit = plotter_->least_sq_fit();
    val.setNum( lsfit.first );
    painter.drawText( 0,0, "Slope: " );
    painter.drawText( hspace,0, val );
    painter.translate( 0, 15 );

    val.setNum( lsfit.second );
    painter.drawText( 0,0, "Intercept: " );
    painter.drawText( hspace,0, val );
    painter.translate( 0, 25 );
  }
}


void Scatterplot_gui::draw_regression_line( bool ok ) {
  plot_->removeCurve( regression_line_curve_id_ );
  
  if( !ok ) {
    plot_->replot();
    return;
  }

  std::pair<float, float> coeffs = plotter_->least_sq_fit();

  const int size = 2;
  double* x_curve = new double[size];
  double* y_curve = new double[size];

  float xmin = plot_->axisScale( QwtPlot::xBottom )->lBound();
  float xmax = plot_->axisScale( QwtPlot::xBottom )->hBound();
  float ymin = plot_->axisScale( QwtPlot::yLeft )->lBound();
  float ymax = plot_->axisScale( QwtPlot::yLeft )->hBound();

  find_regression_line_end_points( x_curve, y_curve,
				   xmin, ymin,xmax, ymax,
				   coeffs.first, coeffs.second );

  regression_line_curve_id_ = plot_->insertCurve( "regression" );

  plot_->setCurveData( regression_line_curve_id_, x_curve, y_curve, size );
  plot_->setCurvePen( regression_line_curve_id_, QPen( Qt::red ) );
  plot_->replot();

}



void Scatterplot_gui::
find_regression_line_end_points( double*& x, double*& y,
				 float xmin, float ymin,
				 float xmax, float ymax,
				 float slope, float intercept ) {
  int found = 0;
  float p;

  // intersection with x = xmin
  p = slope * xmin + intercept;
  if( p >= ymin && p <= ymax ) {
    x[found] = xmin;
    y[found] = p;
    found++;
  }

  // intersection with x = xmax
  p = slope * xmax + intercept;
  if( p >= ymin && p <= ymax ) {
    x[found] = xmax;
    y[found] = p;
    found++;
  }
  if( found == 2 ) return;

  // intersection with y = ymin
  p = 1./slope * ( ymin - intercept );
  if( p >= xmin && p <= xmax ) {
    x[found] = p;
    y[found] = ymin;
    found++;
  }
  if( found == 2 ) return;

  // intersection with y = ymax
  p = 1./slope * ( ymax - intercept );
  if( p >= xmin && p <= xmax ) {
    x[found] = p;
    y[found] = ymax;
    found++;
  }
}



void Scatterplot_gui::
set_x_axis_logscale( bool on ) {
  // disable regression line 
  if( on ) {
    draw_regression_line( false );
    control_panel_->enable_lsfit( false );
  }
  else {
    if( !y_logscale_checkbox_->isChecked() )
      control_panel_->enable_lsfit( true );
  }
  Data_analysis_gui::set_x_axis_logscale( on );
}


void Scatterplot_gui::
set_y_axis_logscale( bool on ) {
  // disable regression line 
  if( on ) {
    draw_regression_line( false );
    control_panel_->enable_lsfit( false );
  }
  else {
    if( !x_logscale_checkbox_->isChecked() )
      control_panel_->enable_lsfit( true );
  }
  
  Data_analysis_gui::set_y_axis_logscale( on );
}
