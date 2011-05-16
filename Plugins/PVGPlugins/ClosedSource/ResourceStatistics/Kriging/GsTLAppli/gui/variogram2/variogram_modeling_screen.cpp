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

#include <GsTLAppli/gui/variogram2/variogram_modeling_screen.h>
#include <GsTLAppli/gui/variogram2/variog_plot.h>
#include <GsTLAppli/gui/variogram2/variog_multiplot.h>
#include <GsTLAppli/gui/variogram2/variogram_controls.h>
#include <GsTLAppli/gui/variogram2/plot_settings_dialog.h>
#include <GsTLAppli/gui/variogram2/print_plots_dialog.h>
#include <GsTLAppli/math/discrete_function.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <GsTL/math/math_functions.h>

#include <qlayout.h>
#include <qfont.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qscrollview.h>
#include <qworkspace.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>

#include <fstream>


Variogram_modeling_screen::
Variogram_modeling_screen( QWidget *parent, const char *name, WFlags f )
  : QWidget( parent, name, f ) {

  this->init();
}


Variogram_modeling_screen::~Variogram_modeling_screen() {
  
}


 
void Variogram_modeling_screen::init() {
  controls_ = 0;
  scroll_ = 0;
  splitter_ = 0;
}


void Variogram_modeling_screen::delete_plots() {
  for( unsigned int i = 0; i < plots_.size() ; i++ ) {
    delete plots_[i];
    plots_[i] = 0;
  } 
}

 

void 
Variogram_modeling_screen::
run( const std::vector<Discrete_function>& df_vec,
     const Variogram_* model_variogram,
     Covariance<GsTLPoint>* model_cov,
     const std::vector<GsTLVector<double> >& angle,
     const std::vector< std::vector<int> >& pairs,
     const QStringList& parameters_list ) {

  delete_plots();
  delete controls_;
  delete scroll_;
  delete splitter_;

  // save some parameters
  experimental_variograms_ = df_vec;
  parameters_list_ = parameters_list;
  directions_ = angle;
  pairs_ = pairs;

  // find out what is the max lag distance
  double max_distance = 10.0;
  for( unsigned int id = 0; id < df_vec.size() ; id++ ) {
    std::vector<double> xvalues = df_vec[id].x_values();
    if( !xvalues.empty() )
      max_distance = std::max( max_distance, xvalues[xvalues.size()-1] );
  }

  QVBoxLayout* main_vertical_layout = new QVBoxLayout( this, 3, 6, "Main_vert_windowLayout");
  splitter_ = new QSplitter( this, "Splitter" );
  splitter_->setOrientation( QSplitter::Horizontal );
  splitter_->setMargin( 1 );

  display_panel_ = new QWorkspace( splitter_, "display_panel_" );    
  
  scroll_ = new QScrollView( splitter_, "Scrollbar", WRepaintNoErase);
  controls_ = 
     new Variogram_controls( model_cov, scroll_->viewport(),
                             "Variogram_controls", max_distance );
  scroll_->addChild( controls_ ); 
  scroll_->setResizePolicy( QScrollView::AutoOneFit );
  scroll_->setMargin( 1 );
  splitter_->setResizeMode( scroll_, QSplitter::KeepSize);  


  //--------------------------------
  // Menu bar set-up
  QMenuBar* menubar = new QMenuBar(this);

  QPopupMenu* file_menu = new QPopupMenu( this );
  file_menu->insertItem( "Load Variogram Model", 
                         controls_, SLOT(load_model()) );
  file_menu->insertSeparator();
  file_menu->insertItem( "Save Variogram Model", 
                         controls_, SLOT(save_model()) );
  file_menu->insertItem( "Save Experimental Variograms", 
                         this, SLOT(save_experimental_variograms()) );
  file_menu->insertSeparator();
  file_menu->insertItem( "Export Plots As Images", 
                         this, SLOT(save_plots_as_images()) );
  menubar->insertItem( "File", file_menu );

  QPopupMenu* edit_menu = new QPopupMenu( this );
  edit_menu->insertItem( "Plot Settings", 
                         this, SLOT(show_plots_settings_dialog()) );
  menubar->insertItem( "Settings", edit_menu );


  QPopupMenu* window_menu = new QPopupMenu( this );
  window_menu->insertItem( "&Tile", display_panel_, SLOT(tile()), CTRL+Key_T );
  window_menu->insertItem( "&Cascade", display_panel_, SLOT(cascade()) );
  menubar->insertItem( "Window", window_menu );


  main_vertical_layout->setMenuBar( menubar );
  main_vertical_layout->addWidget( splitter_ );
//  main_layout->addWidget( splitter_ );

  
  //------------------------------------------
  unsigned int total_plots = df_vec.size();
  if( total_plots > 1 ) 
    total_plots++;  // add space for the multiplot 
  plots_.resize( total_plots );
    
  
  for( unsigned int i=0 ; i < df_vec.size() ; i++ ) {   
    QString base_name( "plot " );
    QString num;
    num.setNum( i+1 );
    base_name.append( num );
    base_name.append( ":  " );
    QString plot_name = base_name + parameters_list[i];
    bool y_starts_at_zero = plot_name.contains( "variog", false );
    plots_[i] = new Variog_plot( df_vec[i], model_variogram, 
                                 angle[i], pairs[i], 
                                 plot_name, y_starts_at_zero,
                                 display_panel_, plot_name.latin1() );

    QObject::connect( controls_, SIGNAL( variogram_changed() ), 
                      plots_[i], SLOT( refresh() ) );
  }

  if( total_plots > 1 ) {
    plots_[total_plots-1] = 
      new Variog_multiplot( df_vec, parameters_list, display_panel_, "all directions" );
  }


  display_panel_->tile();
}   




void Variogram_modeling_screen::
save_experimental_variograms() {
  QString filename =
    QFileDialog::getSaveFileName( QString::null, QString::null, this, 
                                  "Save Experimental Variograms", 
                                  "Choose a file to save the experimental variograms");
  if( filename.isNull() ) return;

  std::ofstream OUT( filename.ascii() );
  if( !OUT ) {
    GsTLcerr << "Can't open file " << filename.ascii() << gstlIO::end;
    return;
  }

  OUT << "<experimental_variograms>\n";
  for( unsigned int df = 0; df < experimental_variograms_.size() ; df++ ) {
    std::vector<double> x = experimental_variograms_[df].x_values();
    std::vector<double> y = experimental_variograms_[df].y_values();

    OUT << "  <variogram> \n"
        << "    <title>" << parameters_list_[df] << "</title>\n"
        << "    <direction>" << directions_[df].x() << " " 
        << directions_[df].y() << " "
        << directions_[df].z() << " "
        << "</direction>\n" 
        << "    <x>";
    std::copy( x.begin(), x.end(), std::ostream_iterator<double>( OUT, " " ) );
    OUT << "</x>\n"
        << "    <y>";
    std::copy( y.begin(), y.end(), std::ostream_iterator<double>( OUT, " " ) );
    OUT << "</y>\n";
    OUT << "    <pairs>";
    std::copy( pairs_[df].begin(), pairs_[df].end(), 
               std::ostream_iterator<int>( OUT, " " ) );
    OUT << "    </pairs>\n"
        << "  </variogram>\n";
  }

  OUT << "</experimental_variograms>\n";
}


void Variogram_modeling_screen::
show_plots_settings_dialog() {
  QWidgetList list = display_panel_->windowList( QWorkspace::CreationOrder );
  std::vector<QwtPlot*> plots;
  QWidget* widget=0;
  for( widget = list.first() ; widget ; widget = list.next() ) {
    plots.push_back( (QwtPlot*) widget );
  }

  PlotSettingsDialog* dialog = 
    new PlotSettingsDialog( plots, this, "Plots Settings", true );
  dialog->exec();
}


void Variogram_modeling_screen::
save_plots_as_images() {
  QWidgetList list = display_panel_->windowList( QWorkspace::CreationOrder );
  std::vector<QwtPlot*> plots;
  QWidget* widget=0;
  for( widget = list.first() ; widget ; widget = list.next() ) {
    plots.push_back( (QwtPlot*) widget );
  }

  PrintPlotsDialog* dialog = 
    new PrintPlotsDialog( plots, this, "Save Plots", true );
  dialog->exec();
}

