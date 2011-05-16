/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Geostatistical Earth
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

#include <GsTLAppli/extra/qtplugins/gstl_widgets_plugin.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/extra/qtplugins/filechooser.h>
#include <GsTLAppli/extra/qtplugins/variogram_input.h>
#include <GsTLAppli/extra/qtplugins/kriging_type_selector.h>
#include <GsTLAppli/extra/qtplugins/gstl_table.h>
#include <GsTLAppli/extra/qtplugins/ellipsoid_input.h>
#include <GsTLAppli/extra/qtplugins/non_param_cdf_input.h>


static const char* variogram_pixmap[] = {
"32 32 3 1",
". c None",
"a c #c00000",
"# c #ff0000",
"................................",
"................................",
"................................",
"........................#.......",
".....................####.......",
".##................#####........",
".####.............#####.........",
"..#####..........####...........",
"....####........####............",
".....####......###..............",
"......####....###...............",
".......####..###................",
".........######......a.......a..",
".........#####......aa.......aa.",
".........####.......a.........a.",
"........######.....aa..aa.....a.",
".......###..###....a...aa.....aa",
"......###....###...a...aa......a",
".....###.....###...a...aa......a",
".....###......###.aa...aa......a",
".....##.......###.a....aaaaaa..a",
"....###.......###.a....aaaaaa..a",
"....###.......###.a....aa..aa..a",
"....###......###..a....aa..aa..a",
"....###.....####...a...aa..aa..a",
"....###.....###....aa..aa..aa.aa",
".....#########......a.........a.",
".....#######........aa.......aa.",
"......#####..........aa......a..",
"......................a......a..",
"................................",
"................................"};

static const char *GridSelector_pixmap[] = {
    "22 22 8 1",
    "  c Gray100",
    ". c Gray97",
    "X c #4f504f",
    "o c #00007f",
    "O c Gray0",
    "+ c none",
    "@ c Gray0",
    "# c Gray0",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "+OOOOOOOOOOOOOOOOOOOO+",
    "OOXXXXXXXXXXXXXXXXXXOO",
    "OXX.          OO OO  O",
    "OX.      oo     O    O",
    "OX.      oo     O   .O",
    "OX  ooo  oooo   O    O",
    "OX    oo oo oo  O    O",
    "OX  oooo oo oo  O    O",
    "OX oo oo oo oo  O    O",
    "OX oo oo oo oo  O    O",
    "OX  oooo oooo   O    O",
    "OX            OO OO  O",
    "OO..................OO",
    "+OOOOOOOOOOOOOOOOOOOO+",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++"
};


GsTL_Widgets_plugin::GsTL_Widgets_plugin() { 
}

QStringList GsTL_Widgets_plugin::keys() const {
  QStringList list;
  list << "GridSelector";
  list << "PropertySelector";
  list << "SinglePropertySelector";
  list << "MultiPropertySelector";
  list << "FileChooser";
  list << "VariogramInput";
  list << "KrigingTypeSelector";
  list << "GsTLButtonGroup";
  list << "GsTLGroupBox";
  list << "CloneGroupBox";
  list << "GsTLTable";
  list << "OrderedPropertySelector";
  list << "TrendComponents";
  list << "EllipsoidInput";
  list << "LowerTailCdfInput";
  list << "UpperTailCdfInput";
  list << "NonParamCdfInput";

  return list;
}

QWidget* GsTL_Widgets_plugin::create( const QString& key,
				                              QWidget* parent, const char* name ) {
  if( key == "GridSelector" )
    return new GridSelector( parent, name, 0 );

  if( key == "PropertySelector" )
    return new PropertySelector( parent, name, 0 ); 

  if( key == "SinglePropertySelector" )
    return new SinglePropertySelector( parent, name ); 

  if( key == "MultiPropertySelector" )
    return new MultiPropertySelector( parent, name ); 

  if( key == "OrderedPropertySelector" )
    return new OrderedPropertySelector( parent, name ); 

  if( key == "FileChooser" )
    return new FileChooser( parent, name );

  if( key == "VariogramInput" )
    return new VariogramInput( parent, name );

  if( key == "KrigingTypeSelector" )
    return new KrigingTypeSelector( parent, name );

  if( key == "GsTLButtonGroup" )
    return new GsTLButtonGroup( parent, name );

  if( key == "GsTLGroupBox" )
    return new GsTLGroupBox( parent, name );

  if( key == "CloneGroupBox" )
    return new CloneGroupBox( parent, name );

  if( key == "GsTLTable" )
    return new GsTLTable( parent, name );

  if( key == "TrendComponents" )
    return new TrendComponents( parent, name );

  if( key == "EllipsoidInput" )
    return new EllipsoidInput( parent, name );

  if( key == "UpperTailCdfInput" )
    return new UpperTailCdfInput( parent, name );

  if( key == "LowerTailCdfInput" )
    return new LowerTailCdfInput( parent, name );

  if( key == "NonParamCdfInput" )
    return new NonParamCdfInput( parent, name );

  return 0;
}


QString GsTL_Widgets_plugin::includeFile( const QString& key ) const {
/*
  if ( key == "GridSelector" )
    return "gstl_selectors.h";
  if ( key == "PropertySelector" )
    return "gstl_selectors.h";
  if ( key == "SinglePropertySelector" )
    return "gstl_selectors.h";
  if ( key == "MultiPropertySelector" )
    return "gstl_selectors.h";
  if ( key == "OrderedPropertySelector" )
    return "gstl_selectors.h";
  if ( key == "FileChooser" )
    return "gstl_filechooser.h";
  if ( key == "VariogramInput" )
    return "gstl_variogram_input.h";
  if ( key == "KrigingTypeSelector" )
    return "gstl_kriging_type_selector.h";
  if ( key == "GsTLButtonGroup" )
    return "gstl_selectors.h";
  if ( key == "GsTLGroupBox" )
    return "gstl_selectors.h";
  if ( key == "CloneGroupBox" )
    return "gstl_selectors.h";
  if ( key == "GsTLTable" )
    return "gstl_table.h";
  */  
  if ( key == "GsTLTable" )
    return "gstl_table.h";
  return QString::null;
}


QString GsTL_Widgets_plugin::group( const QString& ) const {
  return "GsTLWidgets";
}


QIconSet GsTL_Widgets_plugin::iconSet( const QString& key ) const {
  if( key == "VariogramInput" )
    return QIconSet( QPixmap( variogram_pixmap ) );
  else
    return QIconSet( QPixmap( GridSelector_pixmap ) );
    
}


QString GsTL_Widgets_plugin::toolTip( const QString& key ) const {
  if ( key == "GridSelector" )
    return "GridSelector widget";
  if ( key == "PropertySelector" )
    return "PropertySelector widget";
  if ( key == "SinglePropertySelector" )
    return "SinglePropertySelector widget";
  if ( key == "MultiPropertySelector" )
    return "MultiPropertySelector widget";
  if ( key == "OrderedPropertySelector" )
    return "OrderedPropertySelector widget";
  if ( key == "FileChooser" )
    return "FileChooser widget";
  if ( key == "VariogramInput" )
    return "Variogram Input widget";
  if ( key == "KrigingTypeSelector" )
    return "Kriging type selection widget";
  if ( key == "GsTLButtonGroup" )
    return "GsTLButtonGroup widget";
  if ( key == "GsTLGroupBox" )
    return "GsTLGroupBox widget";
  if ( key == "CloneGroupBox" )
    return "CloneGroupBox widget";
  if ( key == "GsTLTable" )
    return "Table widget with copy/paste support";
  if ( key == "TrendComponents" )
    return "Trend components for KT";
  if ( key == "EllipsoidInput" )
    return "3D Ellipsoid input widget";
  if( key == "UpperTailCdfInput" )
    return "Upper tail extrapolation parameters";
  if( key == "LowerTailCdfInput" )
    return "Lower tail extrapolation parameters";
  if( key == "NonParamCdfInput" )
    return "non Parametric Cdf parametrization";

  return QString::null;
}
 
QString GsTL_Widgets_plugin::WhatsThis( const QString& key ) const {
  if ( key == "GridSelector" )
    return "Widget for selecting a grid object among all the objects currently loaded";
  if ( key == "PropertySelector" )
    return "Widget for selecting a property of a grid object";
  if ( key == "SinglePropertySelector" )
    return "Widget for selecting a single property of a grid object";
  if ( key == "MultiPropertySelector" )
    return "Widget for selecting multiple properties of a grid object";
  if ( key == "OrderedPropertySelector" )
    return "Widget for selecting multiple properties of a grid object and ordering them";
  if ( key == "FileChooser" )
    return "Widget for selecting a file";
  if ( key == "VariogramInput" )
    return "Widget for inputing a variogram";
  if ( key == "KrigingTypeSelector" )
    return "Widget for choose a kriging type and input the corresponding parameters";
  if ( key == "GsTLButtonGroup" )
    return "Widget to group other widgets";
  if ( key == "GsTLGroupBox" )
    return "Widget to group other widgets";
  if ( key == "TrendComponents" )
    return "Trend components";
  if ( key == "EllipsoidInput" )
    return "widget for defining a 3D Ellipsoid";
  if( key == "UpperTailCdfInput" )
    return "Widget for upper tail extrapolation";
  if( key == "LowerTailCdfInput" )
    return "Widget for lower tail extrapolation";
  if ( key == "NonParamCdfInput" )
    return "Widgets for defining a non parametric distribution";
 
  return QString::null;
}

bool GsTL_Widgets_plugin::isContainer( const QString& key ) const {
  if ( key == "GsTLButtonGroup" )
    return true;
  if ( key == "GsTLGroupBox" )
    return true;
  if ( key == "CloneGroupBox" )
    return true;
  
return false;
}


 
Q_EXPORT_PLUGIN( GsTL_Widgets_plugin )
