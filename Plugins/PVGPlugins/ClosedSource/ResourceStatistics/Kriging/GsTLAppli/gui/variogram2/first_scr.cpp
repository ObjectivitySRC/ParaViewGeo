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

#include <GsTLAppli/gui/variogram2/first_scr.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/extra/qtplugins/filechooser.h>

#include <qstring.h>
#include <qdom.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qradiobutton.h>


First_scr::First_scr(GsTL_project *project, QWidget* parent, const char* name )
: First_scr_base( parent, name ) {

    grid_name_->setOrientation(Qt::Vertical);
    grid_name_->setColumns(1);

    head_groupbox_->setOrientation(Qt::Vertical);
    head_groupbox_->setColumns(1);
    tail_groupbox_->setOrientation(Qt::Vertical);
    tail_groupbox_->setColumns(1);

    grid_=new GridSelector(grid_name_,"grid",project);
    head_prop_ = new SinglePropertySelector(head_groupbox_,"head_property");
    tail_prop_ = new SinglePropertySelector(tail_groupbox_,"tail_property");

    variog_file_box_->setOrientation(Qt::Vertical);
    variog_file_box_->setColumns(1);
    filechooser_ = new FileChooser( variog_file_box_, "filechooser" );
    variog_file_box_->setHidden( true );

    QObject::connect(grid_, SIGNAL(activated(const QString&)),
		                 head_prop_, SLOT(show_properties(const QString&)));
    QObject::connect(grid_,SIGNAL(activated(const QString&)),
		                 tail_prop_, SLOT(show_properties(const QString&)) );

    QObject::connect( compute_variograms_button_, SIGNAL( toggled(bool) ),
                      compute_variog_box_, SLOT( setShown(bool) ) );
    QObject::connect( compute_variograms_button_, SIGNAL( toggled(bool) ),
                      variog_file_box_, SLOT( setHidden( bool ) ) );


    //---------------------------
    // signal forwarding

    QObject::connect(grid_, SIGNAL( activated(const QString&) ),
		                 this, SIGNAL( parameterChanged() ));
    QObject::connect(head_prop_, SIGNAL( activated(const QString&) ),
		                 this, SIGNAL( parameterChanged() ));
    QObject::connect(tail_prop_, SIGNAL( activated(const QString&) ),
		                 this, SIGNAL( parameterChanged() ));
    QObject::connect(filechooser_, SIGNAL( fileNameChanged(const QString&) ),
		                 this, SIGNAL( parameterChanged() ));
    QObject::connect(compute_variograms_button_, SIGNAL( toggled(bool) ),
		                 this, SIGNAL( parameterChanged() ));

}



bool First_scr::skip_variogram_computation() {
  return load_variograms_button_->isChecked();
}



bool First_scr::load_experimental_variograms() {
  QString filename = filechooser_->fileName();

  QFile file( filename );
  if ( !file.open( IO_ReadOnly ) ) {
    GsTLcerr << "Can't open file " << filename.ascii() << "\n" << gstlIO::end;
    return false;
  }

  QTextStream stream( &file );
  QString qstr = stream.read();
  
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  if( !parsed ) {
    GsTLcerr << "File " << filename.ascii() << " is not a valid XML file\n" << gstlIO::end;
    return false;
  }

  experimental_variograms_.clear();
  parameters_list_.clear();
  directions_.clear();
  pairs_.clear();

  QDomElement root_element = doc.documentElement();
  QDomNodeList variogram_nodes = root_element.elementsByTagName( "variogram" );
  for( int id = 0; id < variogram_nodes.count() ; id++ ) {
    QDomNode varg_node = variogram_nodes.item( id );
    appli_assert( varg_node.isElement() );
    QDomElement varg_element = varg_node.toElement();

    // get the title
    parameters_list_ += extract_title( varg_element, id );

    // get the direction
    GsTLVector<double> dir;
    if( !extract_direction( dir, varg_element, id ) ) return false;
    
    directions_.push_back( dir );


    // get the variogram values
    Discrete_function experim_variog;
    std::vector<double> x;
    std::vector<double> y;
    if( !extract_values( x, "x", varg_element, id ) ) return false;
    if( !extract_values( y, "y", varg_element, id ) ) return false;
    if( x.size() != y.size() ) {
      GsTLcerr << "Error reading variogram " << id << ":\n"
               << "x and y must have the same length" << gstlIO::end;
      return false;
    }
    experim_variog.set_x_values( x );
    experim_variog.set_y_values( y );
    experimental_variograms_.push_back( experim_variog );

    // get the pairs
    std::vector<double> pairs_d;
    if( !extract_values( pairs_d, "pairs", varg_element, id ) ) return false;
    if( x.size() != pairs_d.size() ) {
      GsTLcerr << "Error reading variogram " << id << ":\n"
               << "pair count is missing for some lags" << gstlIO::end;
      return false;
    }

    // cast into int's
    std::vector<int> pairs;
    std::vector<double>::iterator it  = pairs_d.begin();
    for( ; it != pairs_d.end(); ++it ) {
     pairs.push_back( static_cast<int>(*it) );
    }
    pairs_.push_back( pairs );
  }

  return true;
}


QString First_scr::extract_title( QDomElement& elem, int id ) {
  QDomNodeList title_node = elem.elementsByTagName( "title" );
  if( title_node.count() == 0 ) {
    GsTLcerr << "Title missing for variogram " << id << gstlIO::end;
    return " ";
  }

  QDomElement title_elem = title_node.item(0).toElement();
  if( !title_elem.isNull() )
    return title_elem.text();
  else
    return " ";
}


bool First_scr::extract_direction( GsTLVector<double>& dir,
                                   QDomElement& elem, int id ) {
  QDomNodeList node = elem.elementsByTagName( "direction" );
  if( node.count() == 0 ) {
    GsTLcerr << "direction missing for variogram " << id << gstlIO::end;
    return false;
  }
  QDomElement dir_elem = node.item(0).toElement();
  if( dir_elem.isNull() ) {
    GsTLcerr << "Direction missing for variogram " << id << gstlIO::end;
    return false;
  }

  QString dir_str = dir_elem.text();
  QStringList coords = QStringList::split( " ", dir_str, false );
  if( coords.size() != 3 ) {
    GsTLcerr << "Direction for variogram " << id << " is not correctly formated" << gstlIO::end;
    return false;
  }
  dir[0] = coords[0].toDouble();
  dir[1] = coords[1].toDouble();
  dir[2] = coords[2].toDouble();

  return true;
}


bool First_scr::
extract_values( std::vector<double>& values, const QString& tagname,
                QDomElement& elem, int id ) {
  QDomNodeList node = elem.elementsByTagName( tagname );
  if( node.count() == 0 ) {
    GsTLcerr << "missing tag \"" << tagname.ascii() << "\" for variogram " 
             << id << gstlIO::end;
    return false;
  }
  QDomElement dir_elem = node.item(0).toElement();
  if( dir_elem.isNull() ) {
    GsTLcerr << "missing tag \"" << tagname.ascii() << "\" for variogram " 
             << id << gstlIO::end;
    return false;
  }

  QString dir_str = dir_elem.text();
  QStringList coords = QStringList::split( " ", dir_str, false );
  values.clear();
  for( int i=0 ; i < coords.size() ; i++ ) {
    values.push_back( coords[i].toDouble() );
  }

  return true;
}

