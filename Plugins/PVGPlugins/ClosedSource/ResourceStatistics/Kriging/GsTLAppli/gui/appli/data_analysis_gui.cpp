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

#include <GsTLAppli/gui/appli/data_analysis_gui.h>
#include <GsTLAppli/extra/qwt/qwt_plot.h>
#include <GsTLAppli/extra/qwt/simpleps.h>
#include <GsTLAppli/gui/utils/data_analysis_save_dialog.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qsplitter.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpixmap.h>
#include <qprinter.h>
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
#include <qstatusbar.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qsizegrip.h>
#include <qcheckbox.h>
#include <qscrollview.h>

#include <algorithm>
#include <qmessagebox.h>
#include <math.h>
#include <fstream>


Data_analysis_gui::Data_analysis_gui( QWidget* parent, const char* name )
  : Data_analysis_gui_base( parent, name, 
                            Qt::WType_TopLevel | Qt::WDestructiveClose ) {
  
  plot_ = 0;
  scrollview_ = 0;


//  QLayout* layout = this->layout();
//  QSizeGrip* grip = new QSizeGrip( gripbox_, "grip" );
//  layout->add( grip );
  gripbox_->setOrientation( Qt::Vertical );
  gripbox_->setColumns( 2 );
  gripbox_->addSpace( -1 );
  new QSizeGrip( gripbox_, "grip" );


  /*
  //add a status bar (mainly for its resize grip...)
  QLayout* layout = this->layout();

  QStatusBar* statusbar = new QStatusBar( this, "statusbar" );
  QSizePolicy policy( QSizePolicy::Preferred, QSizePolicy::Fixed );
  statusbar->setSizePolicy( policy );
  statusbar->setSizeGripEnabled( true );

  QPushButton* close_button_ = new QPushButton( "Close", statusbar, "close_button" );
  QPushButton* save_button_ = new QPushButton( "Save As Image", statusbar, "save_button" );

  statusbar->addWidget( save_button_, 0, true );
  QLayout* bar_layout = statusbar->layout();
  bar_layout->addItem( new QSpacerItem( 20, 20, 
                                        QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  statusbar->addWidget( close_button_, 0, true );
  layout->add( statusbar );
*/

  init_display_area();
  init_controls_area();

  QObject::connect( (QObject*) close_button_, SIGNAL( clicked() ),
                    this, SLOT( close_window() ) );
  QObject::connect( (QObject*) save_button_, SIGNAL( clicked() ),
                    this, SLOT( save_as_image() ) );

}


Data_analysis_gui::~Data_analysis_gui() {
  appli_message( "data analysis dialog deleted" );
}


void Data_analysis_gui::init_scrollview() {
  scrollview_ = new QScrollView( controls_box_, "scrollview" );
  scrollview_->setFrameShape( QFrame::Panel );
  scrollview_->setFrameShadow( QFrame::Sunken );
  scrollview_->setLineWidth( 1 );
  scrollview_->setMidLineWidth( 0 );
  scrollview_->setResizePolicy( QScrollView::AutoOneFit );
}

void Data_analysis_gui::init_display_area() {
  display_box_->setOrientation( Qt::Vertical );
  display_box_->setColumns( 3 );

  plot_ = new QwtPlot( display_box_, "plot_" );
  display_box_->addSpace( 12 );
  curve_id_ = plot_->insertCurve( "plot" );

  // Use a light gray for the grid
  plot_->setGridPen( QPen( QColor(150,150,150), 0, Qt::DotLine ) );
  
  // configure the plot axis
  plot_->setAxisOptions( QwtPlot::xBottom, QwtAutoScale::Floating );
  QFont font =  plot_->axisFont( QwtPlot::xBottom );
  font.setPointSize( 7 );
  plot_->setAxisFont( QwtPlot::xBottom, font );
  plot_->setAxisFont( QwtPlot::yLeft, font );

  font.setPointSize( 10 );
  plot_->setAxisTitleFont( QwtPlot::xBottom, font );
  plot_->setAxisTitleFont( QwtPlot::yLeft, font );

}



void Data_analysis_gui::init_controls_area() {
  controls_box_->setOrientation( Qt::Vertical );
  controls_box_->setColumns( 1 );

  QRect controls_tabwidget_geom = controls_tabwidget_->geometry();
  controls_tabwidget_geom.setWidth( 220 );
  controls_tabwidget_->setGeometry( controls_tabwidget_geom );
  splitter_->setResizeMode( controls_tabwidget_, QSplitter::KeepSize );

  // signal-slot connections for the axis preferences
  QObject::connect( minx_edit_, SIGNAL( returnPressed() ),
                    this, SLOT( set_x_axis_min() ) );
  QObject::connect( maxx_edit_, SIGNAL( returnPressed() ),
                    this, SLOT( set_x_axis_max() ) );
  QObject::connect( miny_edit_, SIGNAL( returnPressed() ),
                    this, SLOT( set_y_axis_min() ) );
  QObject::connect( maxy_edit_, SIGNAL( returnPressed() ),
                    this, SLOT( set_y_axis_max() ) );

  QObject::connect( reset_axis_x_button_, SIGNAL( clicked() ),
                    this, SLOT( reset_x_axis() ) );
  QObject::connect( reset_axis_y_button_, SIGNAL( clicked() ),
                    this, SLOT( reset_y_axis() ) );

  QObject::connect( x_logscale_checkbox_, SIGNAL( toggled( bool ) ),
                    this, SLOT( set_x_axis_logscale( bool ) ) );
  QObject::connect( y_logscale_checkbox_, SIGNAL( toggled( bool ) ),
                    this, SLOT( set_y_axis_logscale( bool ) ) );

}


void Data_analysis_gui::set_stats_labels( const QStringList& list, int columns ) {

  // set-up the space where statistics will be displayed
  QGroupBox* stat_summary_box = 
    new QGroupBox( 2, Qt::Horizontal, "Summary Statistics", display_box_, "stat_summary_box" );
  stat_summary_box->setAlignment( Qt::AlignHCenter );
  QGrid* stat_summary = new QGrid( columns, Qt::Vertical,
                                   stat_summary_box, "stat_summary" );
  stat_summary_box->addSpace(-1);
  stat_summary->setSpacing(4 );
  
  // add the labels
  int done = 0;
  for( int i=1; i <= list.size() ; i++ ) {
    new QLabel( list[i-1], stat_summary, list[i-1] );

    if( i % columns == 0 ) {
      for( ; done < i ; done++ ) {
        stats_labels_.push_back( 
          new QLabel( "", stat_summary, QString( list[done] + "_val") ) 
          );
      }
    }
  }
  
}


void Data_analysis_gui::close_window() {
  QWidget::close( true );
}


void Data_analysis_gui::closeEvent( QCloseEvent* e ) {
  e->accept();
}

void Data_analysis_gui::refresh_plot(  std::pair<double*, double*> curve,
                                      int size ) {
//  plot_->clear();
  if( !curve.first || !curve.second ) return;

  plot_->setCurveData( curve_id_, curve.first, curve.second, size );
  plot_->replot();

  // update the axis limits
  double x_min = plot_->axisScale( QwtPlot::xBottom )->lBound();
  double x_max = plot_->axisScale( QwtPlot::xBottom )->hBound();
  double y_min = plot_->axisScale( QwtPlot::yLeft )->lBound();
  double y_max = plot_->axisScale( QwtPlot::yLeft )->hBound();
  QString val;
  val.setNum( x_min );
  minx_edit_->setText( val );
  val.setNum( x_max );
  maxx_edit_->setText( val );
  val.setNum( y_min );
  miny_edit_->setText( val );
  val.setNum( y_max );
  maxy_edit_->setText( val );
}

//TL modified
void Data_analysis_gui::refresh_plot( long curveid, std::pair<double*, double*> curve,
                                      int size ) {
//  plot_->clear();
  if( !curve.first || !curve.second ) return;

  plot_->setCurveData( curveid, curve.first, curve.second, size );
  plot_->replot();

  // update the axis limits
  double x_min = plot_->axisScale( QwtPlot::xBottom )->lBound();
  double x_max = plot_->axisScale( QwtPlot::xBottom )->hBound();
  double y_min = plot_->axisScale( QwtPlot::yLeft )->lBound();
  double y_max = plot_->axisScale( QwtPlot::yLeft )->hBound();
  QString val;
  val.setNum( x_min );
  minx_edit_->setText( val );
  val.setNum( x_max );
  maxx_edit_->setText( val );
  val.setNum( y_min );
  miny_edit_->setText( val );
  val.setNum( y_max );
  maxy_edit_->setText( val );
}


void Data_analysis_gui::save_as_image() {
  // get a file name and the name of the filter to use to save the image.
  // 3 filters are available: PNG, BMP, and Postscript. Postscript is not
  // available on Windows (Qt limitation).

  Data_analysis_save_dialog* dialog = 
    new Data_analysis_save_dialog( QString::null, QString::null, 
                                   this, "Save Histogram as Image" );
  
  if( dialog->exec() == QDialog::Rejected ) return;

  QString filename = dialog->selectedFile();
  QString selected_filter = dialog->selectedFilter();
  QString format = selected_filter.section( ' ', 0, 0 );

  if (format == "PNG" && (!filename.endsWith(".png",false)))
	  filename += ".png";
  if (format == "BMP" && (!filename.endsWith(".bmp",false)))
	  filename += ".bmp";
  if (format == "Postscript" && (!filename.endsWith(".ps",false)))
	  filename += ".ps";

  save_as_image( filename, format, dialog->write_stats_required(),
                 dialog->paint_grid_required() );
}


//TL modified
void Data_analysis_gui::savePostScript(SimplePs & ps)
{
	QString lx = plot_->axisTitle(QwtPlot::xBottom);
	QString lyl = plot_->axisTitle(QwtPlot::yLeft);
	QwtPlotCurve* c = plot_->curve( curve_id_ );
	int style = c->style();
	QArray<double>  & x = c->dataX();
	QArray<double>  & y = c->dataY();

	if (lx.isEmpty() || lyl.isEmpty()) {
		QMessageBox::warning(this,"Invalid plot",
		"This is an invalid plot",QMessageBox::Ok,
		QMessageBox::NoButton);
		return;
	}

	/*
	if (x_logscale_checkbox_->isChecked())
		for (int i = 0; i < x.size(); ++i)
			if (x[i] > 0)
				x[i] = log10(x[i]);
	if (y_logscale_checkbox_->isChecked())
		for (int i = 0; i < x.size(); ++i)
			if (y[i] > 0)
				y[i] = log10(y[i]);
	*/
	ps.drawAxis( lx.latin1(), lyl.latin1(),"", false);
	ps.drawCurve(x, y, style);
}

void Data_analysis_gui::save_as_image( const QString& filename, 
                                       const QString& format,
                                       bool show_stats, bool show_grid ) {
  // get a file name and the name of the filter to use to save the image.
  // 3 filters are available: PNG, BMP, and Postscript. Postscript is not
  // available on Windows (Qt limitation).


  // Create a blank image of the correct dimensions
  int extra_width = 15;
  int min_height = 0;
  if( show_stats ) {
    extra_width = 200;
    min_height = 250;
  }
  QSize total_size( plot_->size().width() + extra_width, 
                    std::max( min_height, plot_->size().height()+10 ) );
  QPixmap pix( total_size );
  pix.fill();
  QPainter painter( &pix );
  

  // draw the content of the plot

  QwtPlotPrintFilter filter;
  if( show_grid )
    filter.setOptions( QwtPlotPrintFilter::PrintTitle | QwtPlotPrintFilter::PrintGrid );
  else
    filter.setOptions( QwtPlotPrintFilter::PrintTitle );

  QRect rect = plot_->rect();
  rect.setY( rect.y() + 10 );
  plot_->print( &painter, rect, filter );


  // Add the summary statistics to the image if requested

  if( show_stats ) {
    QFont font = plot_->axisFont( QwtPlot::xBottom );
    painter.setFont( font );
    int text_y_start = std::max( 40, total_size.height()/2 - 100 );
    painter.translate( plot_->size().width()+15 , text_y_start );
    paint_stats( painter );
  }


  // Finally, save the pixmap in the required format

  if( format == "Postscript" || format == "PS" ) {
#if defined(WIN32) || defined(_WIN32)
    if (show_stats)
		build_stats();
    SimplePs ps(filename,plot_, _stats,show_stats);
	if (!ps.isopen()){
		QMessageBox::warning(this,"Unable to save file",
		"Failed to save ps file",QMessageBox::Ok,
		QMessageBox::NoButton);
		return;
	}
	savePostScript(ps);

#else
    QPrinter printer;
    printer.setOutputToFile( true );
    printer.setOutputFileName( filename );
    printer.setPageSize( QPrinter::A6 );
    printer.setFullPage( true );
    printer.setOrientation( QPrinter::Landscape );
    plot_->print(printer, filter);
    bitBlt( &printer, QPoint(0,0), &pix ); 
#endif
  }
  else
    pix.save( filename, format );

}

/*
void Data_analysis_gui::save_as_image( const QString& filename, 
                                       const QString& format,
                                       bool show_stats, bool show_grid ) {
  // get a file name and the name of the filter to use to save the image.
  // 3 filters are available: PNG, BMP, and Postscript. Postscript is not
  // available on Windows (Qt limitation).


  // Create a blank image of the correct dimensions
  int extra_width = 15;
  int min_height = 0;
  if( show_stats ) {
    extra_width = 200;
    min_height = 250;
  }

  int plot_width = plot_->size().width();
  int plot_height = plot_->size().height();
  QRect plot_rect = plot_->rect();

  if( plot_width == 0 || plot_height == 0 ) {
    plot_width = 400;
    plot_height = 500;
    plot_rect.setWidth( plot_width );
    plot_rect.setHeight( plot_height );
  }


  QSize total_size( plot_width + extra_width, 
                    std::max( min_height, plot_height +10 ) );
  QPixmap pix( total_size );
  pix.fill();
  QPainter painter( &pix );
  

  // draw the content of the plot

  QwtPlotPrintFilter filter;
  if( show_grid )
    filter.setOptions( QwtPlotPrintFilter::PrintTitle | QwtPlotPrintFilter::PrintGrid );
  else
    filter.setOptions( QwtPlotPrintFilter::PrintTitle );

  plot_rect.setY( plot_rect.y() + 10 );
  plot_->print( &painter, plot_rect, filter );


  // Add the summary statistics to the image if requested

  if( show_stats ) {
    QFont font = plot_->axisFont( QwtPlot::xBottom );
    painter.setFont( font );
    int text_y_start = std::max( 40, total_size.height()/2 - 100 );
    painter.translate( plot_width+15 , text_y_start );
    paint_stats( painter );
  }


  // Finally, save the pixmap in the required format

  if( format == "Postscript" || format == "PS" ) {
    QPrinter printer;
    printer.setOutputToFile( true );
    printer.setOutputFileName( filename );
    printer.setPageSize( QPrinter::A6 );
    printer.setFullPage( true );
    printer.setOrientation( QPrinter::Landscape );
    bitBlt( &printer, QPoint(0,0), &pix ); 
  }
  else
    pix.save( filename, format );

}
*/




void Data_analysis_gui::show_x_axis_options( bool on ) {
  if( on ) 
    xaxis_options_->show();
  else
    xaxis_options_->hide();
}

void Data_analysis_gui::show_y_axis_options( bool on ) {
  if( on ) 
    yaxis_options_->show();
  else
    yaxis_options_->hide();
}


void Data_analysis_gui::set_x_axis_min() {
  QString valstr = minx_edit_->text();
  double min = valstr.toDouble();
  double max = plot_->axisScale( QwtPlot::xBottom )->hBound();
  plot_->setAxisScale( QwtPlot::xBottom, min, max );
  plot_->replot();
}

void Data_analysis_gui::set_x_axis_max() {
  QString valstr = maxx_edit_->text();
  double max = valstr.toDouble();
  double min = plot_->axisScale( QwtPlot::xBottom )->lBound();
  plot_->setAxisScale( QwtPlot::xBottom, min, max );
  plot_->replot();
}

void Data_analysis_gui::set_y_axis_min() {
  QString valstr = miny_edit_->text();
  double min = valstr.toDouble();
  double max = plot_->axisScale( QwtPlot::yLeft )->hBound();
  plot_->setAxisScale( QwtPlot::yLeft, min, max );
  plot_->replot();
}

void Data_analysis_gui::set_y_axis_max() {
  QString valstr = maxy_edit_->text();
  double max = valstr.toDouble();
  double min = plot_->axisScale( QwtPlot::yLeft )->lBound();
  plot_->setAxisScale( QwtPlot::yLeft, min, max );
  plot_->replot();
}

void Data_analysis_gui::reset_x_axis() {
  plot_->setAxisAutoScale( QwtPlot::xBottom );
  double min = plot_->axisScale( QwtPlot::xBottom )->lBound();
  double max = plot_->axisScale( QwtPlot::xBottom )->hBound();
  QString val;
  val.setNum( min );
  minx_edit_->setText( val );
  val.setNum( max );
  maxx_edit_->setText( val );
  plot_->replot();
}

void Data_analysis_gui::reset_y_axis() {
  plot_->setAxisAutoScale( QwtPlot::yLeft );
  double min = plot_->axisScale( QwtPlot::yLeft )->lBound();
  double max = plot_->axisScale( QwtPlot::yLeft )->hBound();
  QString val;
  val.setNum( min );
  miny_edit_->setText( val );
  val.setNum( max );
  maxy_edit_->setText( val );
  plot_->replot();
}


void Data_analysis_gui::enable_x_log_scaling( bool on ) {
  x_logscale_checkbox_->setEnabled( on );
}

void Data_analysis_gui::enable_y_log_scaling( bool on ) {
  y_logscale_checkbox_->setEnabled( on );
}


void Data_analysis_gui::set_x_axis_logscale( bool on ) {

  if( plot_->axisScale( QwtPlot::xBottom )->logScale() == on ) return;

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

  plot_->changeAxisOptions( QwtPlot::xBottom, QwtAutoScale::Logarithmic, on );
  plot_->replot();
}


void Data_analysis_gui::set_y_axis_logscale( bool on ) {
  if( plot_->axisScale( QwtPlot::yLeft )->logScale() == on ) return;

  if( on ) {
    // Before allowing to go to log scaling, make sure all values are > 0:
    // if some are < 0, return without doing anything
    QArray<long> curve_keys = plot_->curveKeys();
    for( int i = 0; i < curve_keys.size() ; i++ ) {
      QwtPlotCurve* curve = plot_->curve( curve_keys[i] );
      if( curve->minYValue() <= 0 )
        return;
    }
  }

  plot_->changeAxisOptions( QwtPlot::yLeft, QwtAutoScale::Logarithmic, on );
  plot_->replot();
}
