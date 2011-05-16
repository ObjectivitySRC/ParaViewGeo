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

#include <GsTLAppli/extra/gui/qt_accessors.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <GsTLAppli/extra/qtplugins/filechooser.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/extra/qtplugins/variogram_input.h>
#include <GsTLAppli/extra/qtplugins/kriging_type_selector.h>
#include <GsTLAppli/extra/qtplugins/ellipsoid_input.h>
#include <GsTLAppli/extra/qtplugins/non_param_cdf_input.h>

#include <qstring.h>
#include <qdom.h>

#include <vector>
#include <iterator>
#include <sstream>


Named_interface* GridSelector_accessor::create_new_interface(std::string&) {
  return new GridSelector_accessor;
}

GridSelector_accessor::GridSelector_accessor( QWidget* widget ) 
: selector_( dynamic_cast<GridSelector*>( widget ) ) {
  
}

bool GridSelector_accessor::initialize( QWidget* widget ) {

  selector_ = dynamic_cast<GridSelector*>(widget);
  if( selector_ == 0 )
    return false;
  
  return true;
}


std::string GridSelector_accessor::value() const {
  std::string widget_name = selector_->name();
  std::string val;

  if( selector_->currentText().isEmpty() || 
      selector_->currentText() == GridSelector::no_selection )
     val="";
  else 
    val = selector_->currentText().latin1();

  return "<" + widget_name + "  value=\"" + val + "\"  /> \n";
}


bool GridSelector_accessor::set_value( const std::string& str ) {

  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  QString value = elem.attribute( "value" );
  
  // search if "value" is in the list of possible grids. If yes, select it
  int id = 0; 
  for( ; id < selector_->count() ; id++ ) {
    if( selector_->text( id ) == value ) {
      selector_->setCurrentText( selector_->text(id) );
      break;
    }
  }
  
  return true;
}



//=====================================
Named_interface* PropertySelector_accessor::create_new_interface(std::string&) {
  return new PropertySelector_accessor;
}

PropertySelector_accessor::PropertySelector_accessor( QWidget* widget ) 
: selector_( dynamic_cast<PropertySelector*>( widget ) ) {
  
}

bool PropertySelector_accessor::initialize( QWidget* widget ) {

  selector_ = dynamic_cast<PropertySelector*>(widget);
  if( selector_ == 0 )
    return false;
  
  return true;
}


std::string PropertySelector_accessor::value() const {
  std::string widget_name = selector_->name();
  std::string grid, property;
  
  if( !selector_->selectedGrid().isEmpty() )
    grid = std::string( selector_->selectedGrid().latin1() );
  
  if( !selector_->selectedProperty().isEmpty() )
    property = std::string( selector_->selectedProperty().latin1() ) ;
  
  return "<" + widget_name + "  grid=\"" + grid +
            "\"   property=\"" + property + "\"  /> \n";
}


bool PropertySelector_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  selector_->setSelectedGrid( elem.attribute( "grid" ) );
  selector_->setSelectedProperty( elem.attribute( "property" ) );
  
  return true;
}



//=====================================
Named_interface* SinglePropertySelector_accessor::create_new_interface(std::string&) {
  return new SinglePropertySelector_accessor;
}

SinglePropertySelector_accessor::SinglePropertySelector_accessor( QWidget* widget ) 
: selector_( dynamic_cast<SinglePropertySelector*>( widget ) ) {
  
}

bool SinglePropertySelector_accessor::initialize( QWidget* widget ) {

  selector_ = dynamic_cast<SinglePropertySelector*>(widget);
  if( selector_ == 0 )
    return false;
  
  return true;
}


std::string SinglePropertySelector_accessor::value() const {
  std::string widget_name = selector_->name();
  std::string property;

  if( selector_->currentText().isEmpty() )
     property = "";
  else 
    property = selector_->currentText().latin1();

  return "<" + widget_name + "  value=\"" + property + "\"  /> \n";
}


bool SinglePropertySelector_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  QString name = elem.attribute( "value" );
  
  int id = 0; 
  for( ; id < selector_->count() ; id++ ) {
    if( selector_->text( id ) == name ) {
      selector_->setCurrentText( name );
      break;
    }
  } 

  return true;
}



//=====================================
Named_interface* MultiPropertySelector_accessor::create_new_interface(std::string&) {
  return new MultiPropertySelector_accessor;
}

MultiPropertySelector_accessor::MultiPropertySelector_accessor( QWidget* widget ) 
: selector_( dynamic_cast<MultiPropertySelector*>( widget ) ) {
  
}

bool MultiPropertySelector_accessor::initialize( QWidget* widget ) {

  selector_ = dynamic_cast<MultiPropertySelector*>(widget);
  if( selector_ == 0 )
    return false;
  
  return true;
}


std::string MultiPropertySelector_accessor::value() const {
  std::string widget_name = selector_->name();
  std::string property;

  // make a list of all the selected properties. Each property is separated
  // by a semi-column
  int prop_count=0;
  for( unsigned int i = 0; i < selector_->count() ; i++ ) {
    if( selector_->isSelected( i ) && !selector_->text(i).isEmpty() ) {
      property += std::string( selector_->text( i ).latin1() ) + ";";
      prop_count++;
    }
  } 
  
  std::ostringstream so;
  so << "<" << widget_name 
     << " count=\"" << prop_count << "\" "
     << "  value=\"" <<  property <<  "\"  /> \n";

  return so.str();
}


bool MultiPropertySelector_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();

  QString count_str = elem.attribute( "count" );
  int count = count_str.toInt();
  QString prop_string = elem.attribute( "value" );
  QStringList prop_list = QStringList::split( ";", prop_string );

  for( int i = 0 ; i < prop_list.size() ; i++ ) {
    QString name = prop_list[i];
    int id = 0; 
    for( ; id < selector_->count() ; id++ ) {
      if( selector_->text( id ) == name ) {
        selector_->setCurrentItem( id );
        break;
      }
    }
  }
  return true;
}




//=====================================
Named_interface* OrderedPropertySelector_accessor::create_new_interface(std::string&) {
  return new OrderedPropertySelector_accessor;
}

OrderedPropertySelector_accessor::OrderedPropertySelector_accessor( QWidget* widget ) 
: selector_( dynamic_cast<OrderedPropertySelector*>( widget ) ) {
  
}

bool OrderedPropertySelector_accessor::initialize( QWidget* widget ) {

  selector_ = dynamic_cast<OrderedPropertySelector*>(widget);
  if( selector_ == 0 )
    return false;
  
  return true;
}


std::string OrderedPropertySelector_accessor::value() const {
  std::string widget_name = selector_->name();

  // make a list of all the selected properties. Each property is separated
  // by a semi-column
  QStringList list = selector_->selected_properties();
  std::string property;
  if( !list.isEmpty() )
    property = list.join( ";" ).ascii();

  std::ostringstream so;
  so << "<" << widget_name 
     << " count=\"" << list.size() << "\" "
     << "  value=\"" << property <<  "\"  /> \n";

  return so.str();
}


bool OrderedPropertySelector_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();

  QString count_str = elem.attribute( "count" );
  int count = count_str.toInt();
  QString prop_string = elem.attribute( "value" );
  QStringList prop_list = QStringList::split( ";", prop_string );

  selector_->set_selected_properties( prop_list );

  return true;
}




//=====================================
FileChooser_accessor::FileChooser_accessor( QWidget* widget ) 
    : file_chooser_(dynamic_cast<FileChooser*>(widget) ) {}

    
bool FileChooser_accessor::initialize( QWidget* widget ) {

  file_chooser_ = dynamic_cast<FileChooser*>(widget);
  if( file_chooser_ == 0 )
    return false;
  
  return true;
}


std::string FileChooser_accessor::value() const {

  std::string widget_name = file_chooser_->name() ;
  std::string val;

  if( file_chooser_->fileName().isEmpty() )
    val="";
  else
    val = file_chooser_->fileName().latin1();

  return "<" + widget_name + "  value=\"" + val + "\" /> \n";
}


bool FileChooser_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  file_chooser_->setFileName( elem.attribute( "value" ) );

  return true;
}


void FileChooser_accessor::clear() {
  file_chooser_->setFileName( "" );
}


Named_interface* FileChooser_accessor::create_new_interface(std::string&) {
  return new FileChooser_accessor(0);
}








//=====================================
VariogramInput_accessor::VariogramInput_accessor( QWidget* widget ) 
    : varg_input_(dynamic_cast<VariogramInput*>(widget) ) {}

    
bool VariogramInput_accessor::initialize( QWidget* widget ) {
  varg_input_ = dynamic_cast<VariogramInput*>(widget);
  if( varg_input_ == 0 )
    return false;
  
  return true;
}


std::string VariogramInput_accessor::value() const {

  std::string widget_name = varg_input_->name() ;
  std::ostringstream result;
 
  // Write the nugget effect and the number of structures
  result << "<" << widget_name << "  "
	 << "nugget=\"" <<  varg_input_->nugget() << "\" "
	 << "structures_count=\"" << varg_input_->structures_count()  << "\"  >"
	 << std::endl;
  
  // Write the info about each structure
  for( int i=0; i < varg_input_->structures_count() ; i++ ) {
    const Variogram_structure_input* structure = varg_input_->structure( i );
    appli_assert( structure );
    
    result << "  <structure_" << i+1 << "  "
	   << "contribution=\"" << structure->contribution() << "\"  " 
	   << "type=\"" << structure->variogram_type() << "\"   >" 
	   << std::endl
	   << "    <ranges max=\"" << structure->max_range() << "\"  "
	   << "medium=\"" << structure->medium_range() << "\"  "
	   << "min=\"" << structure->min_range() << "\"   />"
	   << std::endl
	   << "    <angles x=\"" << structure->x_angle() << "\"  "
	   << "y=\"" << structure->y_angle() << "\"  "
	   << "z=\"" << structure->z_angle() << "\"   />"
	   << std::endl
	   << "  </structure_" << i+1 << ">"
	   << std::endl; 
  }
  
  result << "</" << widget_name << ">" << std::endl;
  return result.str();
}


bool VariogramInput_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  // Get the nugget value and the number of structures
  QString val = elem.attribute( "nugget" );
  varg_input_->set_nugget( val.toFloat() );
  val = elem.attribute( "structures_count" );
  varg_input_->update_structures_count( val.toInt() );

  // work on each structure
  for( int i=0; i < varg_input_->structures_count() ; i++ ) {
    Variogram_structure_input* structure = varg_input_->structure( i );
    appli_assert( structure );
    
    // Get the node describing the structure and initialize the contribution
    // and variogram type values
    QString id;
    id.setNum( i+1 );
    QString structure_tagname = "structure_" + id;
    QDomNodeList nodes = elem.elementsByTagName( structure_tagname );
    if( nodes.count() == 0 ) {
      appli_warning( "No element called \"" <<  structure_tagname << "\"" << std::endl
		    << "Aborting" );
      return false;
    }
    QDomNode structure_node = nodes.item(0);
    appli_assert( structure_node.isElement() );
    QDomElement structure_elem = structure_node.toElement();

    val = structure_elem.attribute( "contribution" );
    structure->set_contribution( val.toFloat() );
    structure->set_variogram_type( structure_elem.attribute( "type" ) );


    // Get the ranges
    QDomNodeList ranges_node_list = structure_elem.elementsByTagName( "ranges" );
    if( ranges_node_list.count() == 0 ) {
      appli_warning( "No element called \"ranges\"" << std::endl
		    << "Aborting" );
      return false;
    }
    QDomNode ranges_node = ranges_node_list.item(0);
    appli_assert( ranges_node.isElement() );
    QDomElement ranges_elem = ranges_node.toElement();
    
    val = ranges_elem.attribute( "max" );
    structure->max_range( val.toFloat() );
    val = ranges_elem.attribute( "medium" );
    structure->medium_range( val.toFloat() );
    val = ranges_elem.attribute( "min" );
    structure->min_range( val.toFloat() );

    
    // Get the angles
    QDomNodeList angles_node_list = structure_elem.elementsByTagName( "angles" );
    if( angles_node_list.count() == 0 ) {
      appli_warning( "No element called \"angles\"" << std::endl
		    << "Aborting" );
      return false;
    }
    QDomNode angles_node = angles_node_list.item(0);
    appli_assert( angles_node.isElement() );
    QDomElement angles_elem = angles_node.toElement();
    
    val = angles_elem.attribute( "x" );
    structure->x_angle( val.toFloat() );
    val = angles_elem.attribute( "y" );
    structure->y_angle( val.toFloat() );
    val = angles_elem.attribute( "z" );
    structure->z_angle( val.toFloat() );

  }

  return true;
}


void VariogramInput_accessor::clear() {
  varg_input_->set_nugget( 0 );
  varg_input_->update_structures_count( 1 );
  Variogram_structure_input* structure = varg_input_->structure( 0 );
  structure->set_contribution( 0 );
  structure->set_ranges( 0,0,0 );
  structure->set_angles( 0,0,0 );
  
}


Named_interface* VariogramInput_accessor::create_new_interface(std::string&) {
  return new VariogramInput_accessor( 0 );
}





//=====================================
KrigingTypeSelector_accessor::KrigingTypeSelector_accessor( QWidget* widget ) 
    : selector_( dynamic_cast<KrigingTypeSelector*>(widget) ) {}

    
bool KrigingTypeSelector_accessor::initialize( QWidget* widget ) {

  selector_ = dynamic_cast<KrigingTypeSelector*>(widget);
  if( selector_ == 0 )
    return false;
  
  return true;
}


std::string KrigingTypeSelector_accessor::value() const {

  std::string widget_name = selector_->name() ;
  std::ostringstream result;
  result << "<" << widget_name << "  "
	 << "type=\"" << selector_->krigingType() << "\" >" 
	 << std::endl
	 << "  <parameters ";

  if( selector_->krigingType().contains( "(SK)" ) ) {
    result << "mean=\"" << selector_->skMean() << "\" />" << std::endl
	   << "</" << widget_name << ">" << std::endl;
    return result.str();
  }
  
  if( selector_->krigingType().contains( "(KT)" ) ) {
    result << "trend=\"";
    std::vector<bool> flags = selector_->trendComponents();
    std::copy( flags.begin(), flags.end(),
	       std::ostream_iterator<bool>( result, " " ) );
    result << "\" />" << std::endl
	   << "</" << widget_name << ">" << std::endl;
    return result.str();
  }

  if( selector_->krigingType().contains( "(LVM)" ) ) {
    result << "grid=\"" << selector_->LocalMeanProperty().first << "\"  "
	   << "property=\"" << selector_->LocalMeanProperty().second << "\" "
	   << "/>" << std::endl
	   << "</" << widget_name << ">" << std::endl;
    return result.str();
  }

  result << "/>" << std::endl
	 << "</" << widget_name << ">" << std::endl;
  return result.str();
}



bool KrigingTypeSelector_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );

  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  // Get the type of kriging
  QString val = elem.attribute( "type" );
  selector_->setKrigingType( val );

  QDomNodeList nodes = elem.elementsByTagName( "parameters" );


  if( selector_->krigingType().contains( "(SK)" ) ) {
    if( nodes.count() == 0 ) {
      appli_warning( "No element called \"parameters\"" << std::endl
		    << "Aborting" );
      return false;
    }
    QDomNode parameters_node = nodes.item(0);
    appli_assert( parameters_node.isElement() );
    QDomElement parameters_elem = parameters_node.toElement();
    val = parameters_elem.attribute( "mean" );
    selector_->setSkMean( val.toFloat() );
    return true;
  }

  
  if( selector_->krigingType().contains( "(KT)" ) ) {
    if( nodes.count() == 0 ) {
      appli_warning( "No element called \"parameters\"" << std::endl
		    << "Aborting" );
      return false;
    }
    QDomNode parameters_node = nodes.item(0);
    appli_assert( parameters_node.isElement() );
    QDomElement parameters_elem = parameters_node.toElement();
    val = parameters_elem.attribute( "trend" );
    
    // convert the string of 0 and 1 into a vector<bool>
    std::vector<bool> flags;
    std::istringstream input( val.latin1() );
    std::copy( std::istream_iterator<int>( input ), std::istream_iterator<int>(),
	       std::back_inserter( flags ) );
    selector_->setTrendComponents( flags );
    return true;
  }


  if( selector_->krigingType().contains( "(LVM)" ) ) {
    if( nodes.count() == 0 ) {
      appli_warning( "No element called \"parameters\"" << std::endl
		    << "Aborting" );
      return false;
    }
    QDomNode parameters_node = nodes.item(0);
    appli_assert( parameters_node.isElement() );
    QDomElement parameters_elem = parameters_node.toElement();
    QString grid = parameters_elem.attribute( "grid" );
    QString property = parameters_elem.attribute( "property" );
    selector_->setLocalMeanProperty( std::make_pair( grid, property ) );
    return true;
  }

  return true;
}


void KrigingTypeSelector_accessor::clear() {
  std::vector<bool> components( 9, false );
  selector_->setTrendComponents( components );
  selector_->setSkMean( 0.0 );
}


Named_interface* KrigingTypeSelector_accessor::create_new_interface(std::string&) {
  return new KrigingTypeSelector_accessor(0);
}




//===========================================

TrendComponents_accessor::TrendComponents_accessor( QWidget* widget ) 
    : selector_( dynamic_cast<TrendComponents*>(widget) ) {}

    
bool TrendComponents_accessor::initialize( QWidget* widget ) {

  selector_ = dynamic_cast<TrendComponents*>(widget);
  if( selector_ == 0 )
    return false;
  
  return true;
}


std::string TrendComponents_accessor::value() const {
  std::string widget_name = selector_->name() ;
  std::ostringstream result;
  result << "<" << widget_name << "  value=\"";

  std::vector<bool> flags = selector_->trendComponents();
  std::copy( flags.begin(), flags.end(),
             std::ostream_iterator<bool>( result, " " ) );

  result << "\" />" << std::endl;
  return result.str();
}



bool TrendComponents_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );

  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  QString val = elem.attribute( "value" );
  
  // convert the string of 0 and 1 into a vector<bool>
  std::vector<bool> flags;
  std::istringstream input( val.latin1() );
  std::copy( std::istream_iterator<int>( input ), std::istream_iterator<int>(),
             std::back_inserter( flags ) );
  selector_->setTrendComponents( flags );
  return true;
}



void TrendComponents_accessor::clear() {
  std::vector<bool> components( 9, false );
  selector_->setTrendComponents( components );
}


Named_interface* TrendComponents_accessor::create_new_interface(std::string&) {
  return new TrendComponents_accessor(0);
}




//===========================================

EllipsoidInput_accessor::EllipsoidInput_accessor( QWidget* widget ) 
    : input_( dynamic_cast<EllipsoidInput*>(widget) ) {}

    
bool EllipsoidInput_accessor::initialize( QWidget* widget ) {

  input_ = dynamic_cast<EllipsoidInput*>(widget);
  if( input_ == 0 )
    return false;
  
  return true;
}


std::string EllipsoidInput_accessor::value() const {
  std::string widget_name = input_->name() ;
  std::ostringstream result;
  result << "<" << widget_name << "  value=\"";

  double r1, r2, r3;
  input_->ranges( r1, r2, r3 );
  result << r1 << " " << r2 << " " << r3 << "\n";

  input_->angles( r1, r2, r3 );
  result << r1 << " " << r2 << " " << r3;

  result << "\" />" << std::endl;
  return result.str();
}



bool EllipsoidInput_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );

  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  QString val = elem.attribute( "value" );
  String_Op::string_pair rows = 
    String_Op::split_string( val.ascii(), "\n", false );
  std::vector<double> ranges = String_Op::to_numbers<double>( rows.first );
  std::vector<double> angles = String_Op::to_numbers<double>( rows.second );

  for( int i=ranges.size(); i < 3; i++ )
    ranges.push_back( 0 );

  for( int i=angles.size(); i < 3; i++ )
    angles.push_back( 0 );

  input_->set_ranges( ranges[0], ranges[1], ranges[2] );
  input_->set_angles( angles[0], angles[1], angles[2] );

  return true;
}



void EllipsoidInput_accessor::clear() {
  input_->set_ranges( 0, 0, 0 );
  input_->set_angles( 0, 0, 0 );
}


Named_interface* EllipsoidInput_accessor::create_new_interface(std::string&) {
  return new EllipsoidInput_accessor(0);
}

//===========================================

tailCdfInput_accessor::tailCdfInput_accessor( QWidget* widget ) 
    : input_( dynamic_cast<TailCdfInput*>(widget) ) {}

bool tailCdfInput_accessor::initialize(QWidget *widget ) {

  input_ = dynamic_cast<TailCdfInput*>(widget);
  if( input_ == 0 )
    return false;
  
  return true;
}

std::string tailCdfInput_accessor::value() const {
  std::string widget_name = input_->name() ;
  std::ostringstream result;
  result << "<" << widget_name<<"  ";
  result <<"function =\""<<input_->getFunctionType()<<"\"  ";
  result <<"extreme =\""<<input_->getExtremeValue()<<"\"  ";
  result <<"omega =\""<<input_->getOmega()<<"\"";
  result << " />" << std::endl;
  return result.str();
}

bool tailCdfInput_accessor::set_value( const std::string& str ) {
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  QString qstr( str.c_str() );
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();

  QString val = elem.attribute( "function" );
  input_->setFunctionType( val );
  val = elem.attribute("extreme");
  input_->setExtremeValue( val );
  val = elem.attribute("omega");
  input_->setOmega( val );

  return true;
}

void tailCdfInput_accessor::clear() {
  input_->setFunctionType("Power");
  input_->setOmega("1");
  input_->setExtremeValue("");
}

Named_interface* tailCdfInput_accessor::create_new_interface(std::string&) {
  return new tailCdfInput_accessor(0);
}

//===========================================

lowerTailCdfInput_accessor::lowerTailCdfInput_accessor( QWidget* widget ) 
    : tailCdfInput_accessor(widget) {};

upperTailCdfInput_accessor::upperTailCdfInput_accessor( QWidget* widget ) 
    : tailCdfInput_accessor(widget) {};

Named_interface* lowerTailCdfInput_accessor::create_new_interface(std::string&) {
  return new lowerTailCdfInput_accessor(0);
}

Named_interface* upperTailCdfInput_accessor::create_new_interface(std::string&) {
  return new upperTailCdfInput_accessor(0);
}

//lowerTailCdfInput_accessor::lowerTailCdfInput_accessor( QWidget* widget ) 
//    : input_( dynamic_cast<TailCdfInput*>(widget) ) {}

//nonParamCdfInput_accessor::nonParamCdfInput_accessor( QWidget* widget ) 
//    : input_( dynamic_cast<NonParamCdfInput*>(widget) ) {}

//===========================================

nonParamCdfInput_accessor::nonParamCdfInput_accessor(QWidget *widget )
: input_(dynamic_cast<NonParamCdfInput*>(widget)){}

bool nonParamCdfInput_accessor::initialize(QWidget *widget ) {

  input_ = dynamic_cast<NonParamCdfInput*>(widget);
  if( input_ == 0 )
    return false;
  
  return true;
}

std::string nonParamCdfInput_accessor::value() const {
  std::string widget_name = input_->name() ;
  std::ostringstream result;
  result << "<" << widget_name<<"  ";
  result << "ref_on_file =\"" << input_->isRefOnFile()<<"\"  ";
  result << "ref_on_grid =\"" << input_->isRefOnGrid()<<"\""<<"\n";
  result << "break_ties =\"" << input_->isTieBreaking()<<"\" ";
//  if( input_->isRefOnFile() ) {
	result <<"filename =\"" << input_->getRefFileName() <<"\"  ";
//  } else if ( input_->isRefOnGrid() ) {
	result <<" grid =\"" << input_->getRefGridName();
  result <<"\"  property =\"" << input_->getRefPropName() <<"\">\n";
//  }
  tailCdfInput_accessor LTI_accessor( input_->LTI );
  tailCdfInput_accessor UTI_accessor( input_->UTI );
  result<< LTI_accessor.value();
  result<< UTI_accessor.value();;
  result << "</"<<widget_name<<">" << std::endl;
  return result.str();
}

bool nonParamCdfInput_accessor::set_value( const std::string& str ) {
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  QString qstr( str.c_str() );
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();

  QString val = elem.attribute( "ref_on_file" );
  input_->setRefOnFile(val == "1" );
  val = elem.attribute( "ref_on_grid" );
  input_->setRefOnGrid(val == "1" );

  val = elem.attribute( "break_ties" );
  input_->setTieBreaking(val == "1" );

  val = elem.attribute( "filename" );
  input_->setRefFileName(val);
  val = elem.attribute( "grid" );
  input_->setRefGridName(val);
  val = elem.attribute( "property" );
  input_->setRefPropName(val);

  QDomNodeList nodes = elem.elementsByTagName( "LTI_type" );
  if( nodes.count() == 0 ) {
    appli_warning( "No element called \"LTI_type\"" << std::endl
		  << "Aborting" );
    return false;
  }
  QDomNode elemLower_node = nodes.item(0);
  appli_assert( elemLower_node.isElement() );
  QDomElement elemLower = elemLower_node.toElement();
  input_->LTI->setFunctionType( elemLower.attribute("function") );
  input_->LTI->setExtremeValue( elemLower.attribute("extreme") );
  input_->LTI->setOmega( elemLower.attribute("omega") );

  nodes = elem.elementsByTagName( "UTI_type" );
  if( nodes.count() == 0 ) {
    appli_warning( "No element called \"UTI_type\"" << std::endl
		  << "Aborting" );
    return false;
  }
  QDomNode elemUpper_node = nodes.item(0);;
  appli_assert( elemUpper_node.isElement() );
  QDomElement elemUpper = elemUpper_node.toElement();
  input_->UTI->setFunctionType( elemUpper.attribute("function") );
  input_->UTI->setExtremeValue( elemUpper.attribute("extreme") );
  input_->UTI->setOmega( elemUpper.attribute("omega") );

  return true;

}


void nonParamCdfInput_accessor::clear() {
  input_->setRefOnGrid( true );
  input_->setRefOnFile( false );
  input_->setRefFileName("");
  input_->setRefGridName("<None>");
}

Named_interface* nonParamCdfInput_accessor::create_new_interface(std::string&) {
  return new nonParamCdfInput_accessor(0);
}
