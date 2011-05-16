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


#include <GsTLAppli/gui/appli/qt_grid_summary.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

#include <qwidget.h>
#include <qgroupbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qstring.h>

#include <sstream>

QtGrid_summary::QtGrid_summary() {
	geostat_grid_ = 0;
	desc_widget_ = 0;
}

QtGrid_summary::~QtGrid_summary() {}



//TL modified
Named_interface* 
Qt_masked_grid_summary::create_new_interface( std::string& ) {
  return new Qt_masked_grid_summary;
}
Qt_masked_grid_summary::Qt_masked_grid_summary() : QtGrid_summary() {}

void Qt_masked_grid_summary::init( const Geostat_grid* grid ) {
  //TL modified
  const Reduced_grid * rgrid = dynamic_cast<const Reduced_grid*>(grid);
  if( !rgrid ) return;

  //geostat_grid_ = cgrid;
  GsTLCoordVector cell_dims = rgrid->cell_dimensions();
  GsTLPoint origin = rgrid->origin();

  desc_widget_ = new QGroupBox( 1, Qt::Horizontal );
  QGroupBox* groupbox = (QGroupBox*) desc_widget_;
  groupbox->setInsideSpacing( 4 );

  QString count;
  QFont font;
  font.setBold(true);


  QLabel * title;
  title = new QLabel(QString("Grid with inactive cells"),groupbox);
  title->setFont(font);


  // number of cells
  QLabel* dimslabel;
  dimslabel = new QLabel ("Bounding box", groupbox);
  dimslabel->setFont( font );
  count.setNum( rgrid->nx() );
  new QLabel( QString("X: ")+count, groupbox );
  count.setNum( rgrid->ny() );
  new QLabel( QString("Y: ")+count, groupbox );
  count.setNum( rgrid->nz() );
  new QLabel( QString("Z: ")+count, groupbox );

  //TL modified
  QLabel * num;
  num = new QLabel(QString("# of active cells: ")+count.setNum(rgrid->numActive()), groupbox);
  num->setFont(font);


  // cell dimensions
  groupbox->addSpace( 8 );
  QLabel* sizelabel = new QLabel( "Cells dimensions", groupbox );
  sizelabel->setFont( font );
  count.setNum( cell_dims.x() );
  new QLabel( QString("X: ")+count, groupbox );
  count.setNum( cell_dims.y() );
  new QLabel( QString("Y: ")+count, groupbox );
  count.setNum( cell_dims.z() );
  new QLabel( QString("Z: ")+count, groupbox );

  // Origin
  groupbox->addSpace( 8 );
  QLabel* originlabel = new QLabel( "Origin (center of origin cell)", groupbox );
  originlabel->setFont( font );
  count.setNum( origin.x() );
  new QLabel( QString("X: ")+count, groupbox );
  count.setNum( origin.y() );
  new QLabel( QString("Y: ")+count, groupbox );
  count.setNum( origin.z() );
  new QLabel( QString("Z: ")+count, groupbox );

}

Named_interface* 
Qt_cartesian_grid_summary::create_new_interface( std::string& ) {
  return new Qt_cartesian_grid_summary;
}

Qt_cartesian_grid_summary::Qt_cartesian_grid_summary() : QtGrid_summary() {}
  
void Qt_cartesian_grid_summary::init( const Geostat_grid* grid ) {
  //TL modified
  const Cartesian_grid* cgrid = dynamic_cast<const Cartesian_grid*>( grid );
  if( !cgrid ) return;

  geostat_grid_ = cgrid;
  GsTLCoordVector cell_dims = cgrid->cell_dimensions();
  GsTLPoint origin = cgrid->origin();

  desc_widget_ = new QGroupBox( 1, Qt::Horizontal );
  QGroupBox* groupbox = (QGroupBox*) desc_widget_;
  groupbox->setInsideSpacing( 4 );

  QString count;
  QFont font;
  font.setBold(true);

  // number of cells
  QLabel* dimslabel;
  dimslabel = new QLabel( "Number of cells", groupbox );
  dimslabel->setFont( font );
  count.setNum( cgrid->nx() );
  new QLabel( QString("X: ")+count, groupbox );
  count.setNum( cgrid->ny() );
  new QLabel( QString("Y: ")+count, groupbox );
  count.setNum( cgrid->nz() );
  new QLabel( QString("Z: ")+count, groupbox );

  QLabel * num;
  

  // cell dimensions
  groupbox->addSpace( 8 );
  QLabel* sizelabel = new QLabel( "Cells dimensions", groupbox );
  sizelabel->setFont( font );
  count.setNum( cell_dims.x() );
  new QLabel( QString("X: ")+count, groupbox );
  count.setNum( cell_dims.y() );
  new QLabel( QString("Y: ")+count, groupbox );
  count.setNum( cell_dims.z() );
  new QLabel( QString("Z: ")+count, groupbox );

  // Origin
  groupbox->addSpace( 8 );
  QLabel* originlabel = new QLabel( "Origin (center of origin cell)", groupbox );
  originlabel->setFont( font );
  count.setNum( origin.x() );
  new QLabel( QString("X: ")+count, groupbox );
  count.setNum( origin.y() );
  new QLabel( QString("Y: ")+count, groupbox );
  count.setNum( origin.z() );
  new QLabel( QString("Z: ")+count, groupbox );

}; 



 void bounding_box( GsTLPoint& min, GsTLPoint& max, const Point_set* pset ) {
  const std::vector<GsTLPoint>& point_coords = pset->point_locations();
  if( point_coords.empty() ) return;

  min = point_coords[0];
  max = point_coords[0];

  std::vector<GsTLPoint>::const_iterator it = point_coords.begin();
  for( ; it != point_coords.end() ; ++it ) {
    for( int dim = 0; dim < 3; dim++ ) {
      min[dim] = std::min( (*it)[dim], min[dim] );
      max[dim] = std::max( (*it)[dim], max[dim] );
    }
  }
}


Qt_pointset_summary::Qt_pointset_summary() : QtGrid_summary() {}

Named_interface* 
Qt_pointset_summary::create_new_interface( std::string& ) {
  return new Qt_pointset_summary;
}
void Qt_pointset_summary::init( const Geostat_grid* grid ) {
  const Point_set* pset = dynamic_cast<const Point_set*>( grid );
  if( !pset ) return;

  geostat_grid_ = pset;
  int points_count = pset->size();
  QString count;
  count.setNum( points_count );
 
  desc_widget_ = new QGroupBox( 1, Qt::Horizontal );
  QGroupBox* groupbox = (QGroupBox*) desc_widget_;
  groupbox->setInsideSpacing( 4 );

  new QLabel( QString("# of points: ")+count, groupbox );


  GsTLPoint min, max;
  bounding_box( min, max, pset );

  QFont font;
  font.setBold(true);

  groupbox->addSpace( 8 );
  QLabel* bboxlabel = new QLabel( "Bounding Box", groupbox );
  bboxlabel->setFont( font );
  std::ostringstream minlabel;

  minlabel << "Min: ( " << min.x() << " , " << min.y() << " , " << min.z() << " )";
  new QLabel( QString( minlabel.str() ), groupbox );

  std::ostringstream maxlabel;
  maxlabel << "Max: ( " << max.x() << " , " << max.y() << " , " << max.z() << " )";
  new QLabel( QString( maxlabel.str() ), groupbox );

}
 

