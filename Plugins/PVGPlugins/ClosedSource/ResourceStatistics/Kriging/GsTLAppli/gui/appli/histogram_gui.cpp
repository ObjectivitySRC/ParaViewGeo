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

#include <GsTLAppli/math/histogram.h>
#include <GsTLAppli/gui/appli/histogram_gui.h>
#include <GsTLAppli/gui/appli/histogram_control_panel.h>
#include <GsTLAppli/gui/utils/data_analysis_save_dialog.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/extra/qwt/qwt_plot.h>
#include <GsTLAppli/extra/qwt/qwt_legend.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/extra/qwt/simpleps.h>

#include <qspinbox.h>
#include <qlineedit.h>
#include <qsplitter.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qmessagebox.h>

#include <sstream>
#include <GsTLAppli/extra/qwt/qwt_math.h>

typedef std::pair<std::string,std::string> Pair;
//=======================================


void Histogram_gui::changeCurve(const QString & s)
{
	std::map<float,int>::iterator itr;

	int total = histogram_->rawDataSize();
	int i = 0;


	QPen pen(blue,1);
	std::pair<double*,double*> curve = histogram_->plotting_data();
	if (!curve.first) {
		appli_message("No data present");
		return;
	}

	if (s == "cdf") {
	    std::map<float, int> freqCount = histogram_->frequency();

		plot_->removeCurve(_cdfCurve);
  	    _cdfCurve = plot_->insertCurve("Cdf",QwtPlot::xBottom, QwtPlot::yLeft);
		plot_->setAxisTitle(QwtPlot::yLeft, "Probability");
		plot_->enableAxis(QwtPlot::yRight,false);
		plot_->setGridYAxis(QwtPlot::yLeft);
		plot_->enableAxis(QwtPlot::yLeft);
		plot_->setCurveStyle( curve_id_, QwtCurve::NoCurve );
		plot_->setCurveStyle( _cdfCurve, QwtCurve::Lines );
		//plot_->setAxisScale( QwtPlot::yLeft, 0, 1 );

		double * x = new double[freqCount.size()+1];
		double * y = new double[freqCount.size()+1];

		y[0] = 0;
		for (itr = freqCount.begin(); itr != freqCount.end(); ++itr,++i) {
			x[i+1] = itr->first;
			y[i+1] = y[i]+(double)(itr->second)/(double)(total);
		}
		x[0] = x[1];

		plot_->setCurvePen(_cdfCurve, pen);
		plot_->setCurveData( _cdfCurve, x, y, freqCount.size()+1 );
		refresh_plot( _cdfCurve, std::make_pair( x,y ), freqCount.size()+1 );
		refresh_stats();


		delete []x;
		delete []y;
	}
	else if (s == "pdf") {
		plot_->removeCurve(_cdfCurve);
		plot_->enableAxis(QwtPlot::yRight, false);
		plot_->setAxisTitle(QwtPlot::yLeft, "frequency");
		plot_->setGridYAxis(QwtPlot::yLeft);
		plot_->enableAxis(QwtPlot::yLeft);
		plot_->setCurveStyle( _cdfCurve, QwtCurve::NoCurve );
		plot_->setCurveStyle( curve_id_, QwtCurve::Histogram );

		int size = histogram_->bins()+1;
		double* x = new double[size+1];
		double* y = new double[size+1];
		for( int i =0 ; i < size ; i++ ) {
			x[i+1] = curve.first[i];
			y[i+1] = curve.second[i];
		}
		x[0] = x[1];
		y[0] = 0.0;

		refresh_plot(  std::make_pair( x,y ), size+1 );
		refresh_stats();
		delete []x;
		delete []y;
	}
	else {

	    std::map<float, int> freqCount = histogram_->frequency();

		plot_->removeCurve(_cdfCurve);
		plot_->setAxisTitleFont(QwtPlot::yRight, plot_->axisTitleFont(QwtPlot::yLeft));
  	    _cdfCurve = plot_->insertCurve("Cdf",QwtPlot::xBottom, QwtPlot::yRight);
		plot_->enableAxis(QwtPlot::yRight, true);
		plot_->enableAxis(QwtPlot::yLeft, true);
		plot_->setGridYAxis(QwtPlot::yLeft);
		plot_->setAxisTitle(QwtPlot::yLeft, "frequency");
		plot_->setAxisTitle(QwtPlot::yRight, "probability");
		plot_->setCurveStyle( _cdfCurve, QwtCurve::Lines );
		plot_->setCurveStyle( curve_id_, QwtCurve::Histogram );

		double * x = new double[freqCount.size()+1];
		double * y = new double[freqCount.size()+1];

		y[0] = 0;
		for (itr = freqCount.begin(); itr != freqCount.end(); ++itr,++i) {
			x[i+1] = itr->first;
			y[i+1] = y[i]+(double)(itr->second)/(double)(total);
		}
		x[0] = x[1];

		plot_->setCurvePen(_cdfCurve, pen);
		plot_->setCurveData( _cdfCurve, x, y, freqCount.size()+1 );
		refresh_plot( _cdfCurve, std::make_pair( x,y ), freqCount.size()+1 );
		//refresh_stats();


		int size = histogram_->bins()+1;
		double* x1 = new double[size+1];
		double* y1 = new double[size+1];
		for( int i =0 ; i < size ; i++ ) {
			x1[i+1] = curve.first[i];
			y1[i+1] = curve.second[i];
		}
		x1[0] = x1[1];
		y1[0] = 0.0;

		refresh_plot(  std::make_pair( x1,y1 ), size+1 );
		refresh_stats();

		delete []x1;
		delete []y1;
		delete []x;
		delete []y;

	}
	//plot_->replot();

}

//TL modified
void Histogram_gui::savePostScript(SimplePs & ps)
{
	QString lx = plot_->axisTitle(QwtPlot::xBottom);
	QString lyl = plot_->axisTitle(QwtPlot::yLeft);
	QString lyr;
	QwtPlotCurve* c;
	QString whatplot = control_panel_->whatPlot();

	if (whatplot == "cdf")
		c = plot_->curve( _cdfCurve );
	else
		c = plot_->curve( curve_id_ );
	int style = c->style();
	QArray<double>  & x = c->dataX();
	QArray<double>  & y = c->dataY();

	if (lx.isEmpty() || lyl.isEmpty()) {
		QMessageBox::warning(this,"Invalid plot",
			"This is an invalid plot",QMessageBox::Ok,
			QMessageBox::NoButton);
		return;
	}

	ps.drawCurve(x, y, style);
	ps.drawAxis(lx.latin1(), lyl.latin1(),"", false);

	if (whatplot == "pdf+cdf") {
		QwtPlotCurve* c1 = plot_->curve(_cdfCurve);
		int style1 = c1->style();
		QArray<double>  xb = c1->dataX();
		QArray<double>  yb = c1->dataY();
		lyr = plot_->axisTitle(QwtPlot::yRight);
		ps.drawYRightAxis(lyr.latin1());
		ps.drawCurve(xb,yb,style1,true);
	}
}

Histogram_gui::Histogram_gui( GsTL_project* project, 
                              QWidget* parent, const char* name )
: Data_analysis_gui( parent, name ),
  histogram_( 0 ) {

  //TL modified
  _cdfCurve = plot_->insertCurve("Cdf",QwtPlot::xBottom, QwtPlot::yRight);
  //plot_->setAxisTitle(QwtPlot::yLeft, "Histogram scale");
  //plot_->setAxisTitle(QwtPlot::yRight, "Cdf scale");
  plot_->enableAxis(QwtPlot::yRight);
  plot_->setAxisAutoScale(QwtPlot::yRight, true);
  plot_->enableGridX();
  plot_->enableGridY();

  setCaption( "Histogram" );
 
  histogram_ = new Histogram;

  enable_y_log_scaling( false );

  control_panel_ = 
    new Histogram_control_panel( project, controls_box_, "control_panel" );

  QStringList stats_labels;
  stats_labels << "Number of Data" << "  " << "Mean" << "Variance" << "  ";
  stats_labels << "   " << "  " << "   " << "   " << "  ";
  stats_labels << "Maximum" << "Upper Quartile" << "Median" 
               << "Lower Quartile" << "Minimum";
  set_stats_labels( stats_labels, 5 );


  // set the plotting style (curve style, etc)
  plot_->setCurveStyle( curve_id_, QwtCurve::Histogram );


  // Signal-slot connections
  QObject::connect( control_panel_, 
                    SIGNAL( var_changed( const GsTLGridProperty* ) ),
                    this,
                    SLOT( get_data_from( const GsTLGridProperty* ) ) );
       
  QObject::connect( control_panel_, SIGNAL( low_clip_changed(float) ),
                    this, SLOT( update_low_clip(float) ) );
  QObject::connect( control_panel_, SIGNAL( high_clip_changed(float) ),
                    this, SLOT( update_high_clip(float) ) );
  QObject::connect( control_panel_,
                    SIGNAL( reset_clipping_values_clicked() ),
                    this, SLOT( reset_clipping_values() ) );

  QObject::connect( control_panel_,
                    SIGNAL( bins_count_changed( int ) ),
                    this, SLOT( update_bins( int ) ) );

  //TL modified
  QObject::connect(control_panel_, SIGNAL(comboChanged(const QString &)), this, 
				   SLOT(changeCurve(const QString &)));


}


Histogram_gui::~Histogram_gui() {
  clean();
}

void Histogram_gui::clean() {
  delete histogram_;
}


void Histogram_gui::get_data_from( const GsTLGridProperty* prop ) {
  if( !prop ) return;
  
  histogram_->set_data( prop->begin(), prop->end() );
  histogram_->bins( control_panel_->bins_count() );

  control_panel_->set_clipping_values( histogram_->low_clip(),
                                       histogram_->high_clip() );

  plot_->setAxisTitle( QwtPlot::xBottom, prop->name().c_str() );

  update_all();
}



void Histogram_gui::update_low_clip( float val ) {
  histogram_->low_clip( val );
  update_all();
}

void Histogram_gui::update_high_clip( float val ) {
  histogram_->high_clip( val );
  update_all();
}


void Histogram_gui::reset_clipping_values() {
  float min = histogram_->min();
  float max = histogram_->max();

  control_panel_->set_clipping_values( min, max );
  histogram_->low_clip( min );
  histogram_->high_clip( max );

  update_all();
}


void Histogram_gui::update_bins( int count ) {
  histogram_->bins( count );
  update_all();
}


void Histogram_gui::update_all() {
/*
  std::vector<float> frequencies = histogram_->histogram_frequencies();
  unsigned int size = frequencies.size();
  std::pair<float,float> min_max = histogram_->clipping_values();

  double* x = new double[size+1];
  double* y = new double[size+1];
  for( unsigned int i= 0 ; i < size ; i++ ) {
    x[i] = min_max.first + 
            double(i) * (min_max.second - min_max.first) / double( size );
    y[i] = frequencies[i];
  }
  x[size] = min_max.second;
  y[size] = y[size-1];

  refresh_plot( std::make_pair( x,y ), size+1);
  refresh_stats();
  
  delete [] x;
  delete [] y;
*/	

    changeCurve(control_panel_->whatPlot());

	if (plot_->curve(curve_id_)->minXValue() <= 0) {
		x_logscale_checkbox_->setText("Remove zeros from data first!");
		x_logscale_checkbox_->setEnabled( false );
	}
	else{
		x_logscale_checkbox_->setText("Logarithmic Scale");
		x_logscale_checkbox_->setEnabled( true );
	}
}


void Histogram_gui::refresh_stats() {
  update_stats_value( 0, histogram_->data_count() ); // data count
  update_stats_value( 2, histogram_->mean() );       // mean
  update_stats_value( 3, histogram_->var() );        // variance

  update_stats_value( 10, histogram_->percentile( 1.0 ) );        // max
  update_stats_value( 11, histogram_->percentile( 0.75 ) ); //up. quartile
  update_stats_value( 12, histogram_->percentile( 0.5 ) );  // median
  update_stats_value( 13, histogram_->percentile( 0.25 ) ); //low quartile
  update_stats_value( 14, histogram_->percentile( 0.0 ) );         // min
}


void Histogram_gui::build_stats()
{
	_stats.clear();
	QString val;
    val.setNum( histogram_->data_count() );
	_stats.push_back(Pair("Data count: ",val.ascii()));

    val.setNum( histogram_->mean() );
	_stats.push_back(Pair("Mean: ",val.ascii()));

    val.setNum( histogram_->var() );
	_stats.push_back(Pair("Variance: ",val.ascii()));

    val.setNum( histogram_->max() );
	_stats.push_back(Pair("Maximum: ",val.ascii()));

    val.setNum( histogram_->percentile(0.75) );
	_stats.push_back(Pair("Upper quartile: ",val.ascii()));

    val.setNum( histogram_->percentile(0.5) );
	_stats.push_back(Pair("Median: ",val.ascii()));

    val.setNum( histogram_->percentile(0.25) );
	_stats.push_back(Pair("lower quartile: ",val.ascii()));

    val.setNum( histogram_->min() );
	_stats.push_back(Pair("Minimum: ",val.ascii()));

}

void Histogram_gui::paint_stats( QPainter& painter ) {
  QString val;
  val.setNum( histogram_->data_count() );
  painter.drawText( 0,0, "Data count: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 25 );

  val.setNum( histogram_->mean() );
  painter.drawText( 0,0, "Mean: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );

  val.setNum( histogram_->var() );
  painter.drawText( 0,0, "Variance: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 25 );

  val.setNum( histogram_->max() );
  painter.drawText( 0,0, "Maximum: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );

  val.setNum( histogram_->percentile( 0.75 ) );
  painter.drawText( 0,0, "Upper quartile: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );

  val.setNum( histogram_->percentile( 0.5 ) );
  painter.drawText( 0,0, "Median: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );

  val.setNum( histogram_->percentile( 0.25 ) );
  painter.drawText( 0,0, "Lower quartile: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );

  val.setNum( histogram_->min() );
  painter.drawText( 0,0, "Minimum: " );
  painter.drawText( 90,0, val );
  painter.translate( 0, 15 );
}

 
void Histogram_gui::set_x_axis_logscale( bool on ) {
  if( plot_->axisScale( QwtPlot::xBottom )->logScale() == on ) return;
/*
  // Nico: the following check may be unnecessary
  if( on ) {
    // Before allowing to go to log scaling, make sure all values are > 0:
    // if some are < 0, return without doing anything
    QArray<long> curve_keys = plot_->curveKeys();
    for( int i = 0; i < curve_keys.size() ; i++ ) {
      QwtPlotCurve* curve = plot_->curve( curve_keys[i] );
      if( curve->minXValue() <= 0 )
        return;
    }
  }
*/
  histogram_->set_use_logscale( on );
  plot_->changeAxisOptions( QwtPlot::xBottom, QwtAutoScale::Logarithmic, on );
  update_all();
}

