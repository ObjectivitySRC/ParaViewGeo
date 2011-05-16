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

#include <GsTLAppli/gui/variogram2/print_plots_dialog.h>
#include <GsTLAppli/extra/qtplugins/filechooser.h>
#include <GsTLAppli/extra/qwt/simpleps.h>

#include <qgroupbox.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <fstream>

PrintPlotsDialog::
PrintPlotsDialog( const std::vector<QwtPlot*>& plot_widgets, 
                    QWidget* parent, const char* name, 
                    bool modal ) 
  : PrintPlotsDialogBase( parent, name, modal ), plots_( plot_widgets ) {

  plots_list_->setSelectionMode( QListBox::Extended );
  plots_list_->setHidden( true );

  filechooser_groupbox_->setOrientation( Qt::Horizontal );
  filechooser_groupbox_->setColumns( 1 );
  filechooser_ = new FileChooser( filechooser_groupbox_, "filechooser" );
  filechooser_->setMode( FileChooser::Any );

  format_->insertItem( "PNG format (*.png)" );
  format_->insertItem( "BMP format (*.bmp)" );

  //TL modified
  format_->insertItem( "PS format (*.ps)");
  
  for( unsigned int plot_id = 0 ; plot_id < plots_.size() ; plot_id++ ) {
    plots_list_->insertItem( plots_[plot_id]->name() );
  }

  QObject::connect( ok_button_, SIGNAL( clicked() ),
                    this, SLOT( accept() ) );
  QObject::connect( cancel_button_, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );

  setSizeGripEnabled( true );
}


void PrintPlotsDialog::accept() {
  std::vector<QwtPlot*> plots_to_save;
  if( all_plots_button_->isChecked() ) 
    plots_to_save = plots_;
  else {
    for( unsigned int i=0; i < plots_list_->count() ; i++ ) {
      if( plots_list_->isSelected( i ) )
        plots_to_save.push_back( plots_[i] );
    }
  }

  QString format = format_->currentText().section( ' ', 0, 0 );
  QString base_filename = filechooser_->fileName();
  for( unsigned int plot_id = 0 ; plot_id < plots_to_save.size() ; plot_id++ ) {
    QArray<long> curve_keys = plots_to_save[plot_id]->curveKeys();
	
    QString suffix = plots_to_save[plot_id]->name();

    suffix = suffix.simplifyWhiteSpace();
    suffix.replace( ": ", "_" );
    suffix.replace( ", ", "_" );
    suffix.replace( " ", "_" );
    save_one_plot( plots_to_save[plot_id], 
                   base_filename + suffix + "." + format.lower(),
                   format, show_grid_->isChecked() );
  }

  QDialog::accept();
}



void PrintPlotsDialog::
save_one_plot( QwtPlot* plot, const QString& filename,
               const QString& format, bool show_grid ) {

  // Create a blank image of the correct dimensions
  int extra_width = 15;
  int min_height = 0;

  QSize total_size( plot->size().width() + extra_width, 
                    std::max( min_height, plot->size().height()+10 ) );
  QPixmap pix( total_size );
  pix.fill();
  QPainter painter( &pix );
  
  //TL modified

  // draw the content of the plot

  if( format == "PS" || format == "ps" ) {
  	QwtPlotCurve* c; 
	int style;
	QArray<double>  x; 
	QArray<double>  y;

    SimplePs ps(filename,plot);
	QString lx = plot->axisTitle(QwtPlot::xBottom);
    QArray<long> curve_keys = plot->curveKeys();

	ps.drawAxis(lx.latin1(), "","", false);
	for (int i = 0; i < curve_keys.size(); ++i) {
		c = plot->curve(curve_keys[i]);
		x = c->dataX();
		y = c->dataY();
		if (x.size() == 0) continue;
		style = c->style();
		ps.drawCurve(x, y, style);
	}
	return;
  }


  QwtPlotPrintFilter filter;
  if( show_grid )
    filter.setOptions( QwtPlotPrintFilter::PrintTitle | QwtPlotPrintFilter::PrintGrid );
  else
    filter.setOptions( QwtPlotPrintFilter::PrintTitle );

  QRect rect = plot->rect();
  rect.setY( rect.y() + 10 );
  plot->print( &painter, rect, filter );

  pix.save( filename, format );

  // Finally, save the pixmap in the required format
/*
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
*/
}


