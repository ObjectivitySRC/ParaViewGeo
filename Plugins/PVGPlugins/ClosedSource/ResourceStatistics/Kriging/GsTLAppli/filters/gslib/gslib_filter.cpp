/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "filters" module of the Geostatistical Earth
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

#include <GsTLAppli/filters/gslib/gslib_filter.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/filters/gslib/filter_qt_dialogs.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

#include <qdialog.h>
#include <qapplication.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qcursor.h>

#include <string>
#include <cctype>


//temporary
#include <algorithm>


Named_interface* Gslib_infilter::create_new_interface( std::string& ) {
  return new Gslib_infilter();
}

Named_interface* Gslib_grid_infilter::create_new_interface( std::string& ) {
  return new Gslib_grid_infilter();
}

Named_interface* Gslib_mgrid_infilter::create_new_interface( std::string& ) {
  return new Gslib_mgrid_infilter();
}

Named_interface* Gslib_outfilter::create_new_interface( std::string& ) {
  return new Gslib_outfilter();
}

//===================================================================


Gslib_infilter::Gslib_infilter() 
  : wizard_( new Gslib_infilter_dialog( qApp->mainWidget() ) ) {
}

Gslib_infilter::~Gslib_infilter() {
  delete wizard_;
}


bool Gslib_infilter::can_handle( const std::string& filename ) {
  // To figure out if a file is a gslib file we do:
  //   - read the second line: it should be the number of properties in the
  //     data set.
  //   - skip as many lines as indicated on the second line (ie skip the
  //     property names). The next character should be a number.

  QFile file( filename.c_str() );
  if( !file.open( IO_ReadOnly ) ) return false;

  QTextStream stream( &file );
  stream.readLine();
  if( stream.atEnd() ) return false;

  QString str;
  stream >> str;
  stream.readLine();
  bool is_number;
  int properties = str.toInt( &is_number );
  if( !is_number ) return false;
  if( properties <= 0 ) return false;
  
  for( int i= 0; i < properties; i++ )
    stream.readLine();

  // Read the (supposed) first line of property values. Count that there are 
  // as many columns as indicated in the second line, and that each columns 
  // contains a number
  QString line = stream.readLine();
  QStringList fields = QStringList::split( " ", line.simplifyWhiteSpace() );
  if( fields.size() != properties ) return false;
  for( int j= 0; j < properties; j++ ) {
    fields[j].toFloat( &is_number );
    if( !is_number ) return false;
  }

  return true;
}


Geostat_grid* Gslib_infilter::read( const std::string& filename, 
                                    std::string* errors ) {
  std::ifstream infile( filename.c_str() );
  if( !infile ) {
    if( errors ) 
      errors->append( "can't open file: " + filename );
  }

  wizard_->set_file( infile );

  QApplication::setOverrideCursor( QCursor(Qt::ArrowCursor) );
    
  if( wizard_->exec() == QDialog::Rejected ) {
    QApplication::restoreOverrideCursor();
    return 0;
  }

  QApplication::restoreOverrideCursor();

  Gslib_specialized_infilter* filter = wizard_->filter();
  return filter->read( infile ) ;

}






//========================================================


Geostat_grid* Gslib_specialized_infilter::read( const std::string& filename, 
                                                std::string* errors ) {
  std::ifstream in( filename.c_str() );
  if( !in ) {
    if( errors )
      errors->append( "can't open file: " + filename );
    return 0;
  }

  return this->read( in );
}

//========================================================
Gslib_mgrid_infilter::Gslib_mgrid_infilter() : 
	dialog_(new Gslib_input_mgrid_dialog( qApp->mainWidget() ) )
{}

Gslib_mgrid_infilter::~Gslib_mgrid_infilter() {
  dialog_ = 0;
}

QWidget* Gslib_mgrid_infilter::init_dialog( std::ifstream& ) {
  dialog_ = new Gslib_input_mgrid_dialog( qApp->mainWidget() );
  return dialog_;
}

Geostat_grid* Gslib_mgrid_infilter::readRegular(std::ifstream& infile,Reduced_grid * grid)
{
	std::vector<std::string> nums;

	int nx = dialog_->nx();
	int ny = dialog_->ny();
	int nz = dialog_->nz();
	float x_size = dialog_->x_size();
	float y_size = dialog_->y_size();
	float z_size = dialog_->z_size();
	float Ox = dialog_->Ox();
	float Oy = dialog_->Oy();
	float Oz = dialog_->Oz();

	grid->set_dimensions( nx, ny, nz,
		x_size, y_size, z_size);
	grid->origin( GsTLPoint( Ox,Oy,Oz) );

	std::string buffer;

	//-------------------------
	//   now, read the file

	// read title
	std::getline( infile, buffer, '\n');

	// read nb of properties
	int property_count;
	infile >> property_count;
	std::getline( infile, buffer, '\n');

	if (dialog_->maskColumnNum() > property_count) {
	    GsTLcerr << "Invalid mask column" << gstlIO::end;
		return NULL;
	}

	// check whether the file contains multiple realizations, and read in mask
	bool has_multi_real = false;
	std::string maskname;
	long int actual = 0;
	for( int pos=0; pos < property_count ; pos++ ) {
		std::getline( infile, buffer, '\n');
		if (pos == dialog_->maskColumnNum()-1) {
			QString prop_name( buffer.c_str() );
			maskname = prop_name.stripWhiteSpace().ascii(); // get mask property name
		}
	}

	grid->setMaskName(maskname);
	
	for( int pos=0; pos < grid->trueSize() ; pos++ ) {
		std::getline( infile, buffer, '\n');
		nums = String_Op::decompose_string(buffer, " ", false);

		if (String_Op::to_number<int>(nums[dialog_->maskColumnNum()-1]) == 0) 
			continue;
		grid->setConversionPair(pos,actual);
		++actual;
	}
	grid->initMaskedGrid(dialog_->isregular());

	float val;
	if( infile >> val ) has_multi_real = true;

	// reposition the stream
	infile.clear();
	infile.seekg( 0 );
	std::getline( infile, buffer, '\n');
	std::getline( infile, buffer, '\n');

	if( has_multi_real ) {
		std::vector<MultiRealization_property*> properties;
		for( int i=0; i<property_count; i++ ) {
			std::getline( infile, buffer, '\n');
			QString prop_name( buffer.c_str() );
			MultiRealization_property* prop;

			if (i != (dialog_->maskColumnNum()-1)) {
				prop = grid->add_multi_realization_property( prop_name.stripWhiteSpace().ascii() );
				if( !prop ) {
					GsTLcerr << "Several properties share the same name " << gstlIO::end;
					return 0;
				}

				properties.push_back( prop );
			}
		}

		while( infile ) {
			if( !infile ) break;
      char c = infile.peek();
      if( !std::isdigit(c) ) break;

			std::vector<GsTLGridProperty*> props;
			for( unsigned int ii=0; ii < properties.size(); ii++) {			
				GsTLGridProperty* prop = properties[ii]->new_realization();
				props.push_back( prop );
			}
			read_one_realization( infile, props, grid );
		}
	}

	else {
		std::vector<GsTLGridProperty*> properties;
		for( int i=0; i<property_count; i++ ) {
			std::getline( infile, buffer, '\n');
			QString prop_name( buffer.c_str() );

			if (i != dialog_->maskColumnNum()-1) {

				GsTLGridProperty* prop = 
					grid->add_property( prop_name.stripWhiteSpace().ascii() );
				properties.push_back( prop );
			}
		}    
		read_one_realization( infile, properties, grid);

	}

	
	return grid;
}


bool Gslib_mgrid_infilter::
read_one_realization( std::ifstream& infile, 
                      const std::vector<GsTLGridProperty*>& properties,
                      Reduced_grid * grid) 
{

  // read the property values
  int property_count = properties.size()+1;  //with mask
  long int count = 0, actual = 0;
  std::string buffer;
  float val;
  std::vector< std::string > nums;
  float no_data_value;

  if( dialog_->use_no_data_value() ) 
    no_data_value = dialog_->no_data_value();

  while( infile && count < grid->trueSize() ) {
	  std::getline( infile, buffer, '\n');
	  nums = String_Op::decompose_string( buffer, " ", false );

	  // if inactive
	  if (!grid->isActive(count)) {
		  ++count;
		  continue;
	  }

	  int prop_index = 0;
	  for (int i = 0; i < property_count; ++i){
		  if (i == dialog_->maskColumnNum()-1) // mask column
			  continue;
		  val = String_Op::to_number<float>(nums[i]);
		  if (dialog_->use_no_data_value() && val == no_data_value) {
			  ++prop_index;
  			  continue;
		  }
		  properties[prop_index]->set_value(val,actual);
		  ++prop_index;
	  }
  	  ++count;
  	  ++actual;
  }  
  return true;
}


Geostat_grid* Gslib_mgrid_infilter::readPointset(std::ifstream& infile, Reduced_grid * grid)
{
	int nx = dialog_->nx();
	int ny = dialog_->ny();
	int nz = dialog_->nz();
	float x_size = dialog_->x_size();
	float y_size = dialog_->y_size();
	float z_size = dialog_->z_size();
	float Ox = dialog_->Ox();
	float Oy = dialog_->Oy();
	float Oz = dialog_->Oz();
	int X_col_id = dialog_->X_column()-1;
	int Y_col_id = dialog_->Y_column()-1;
	int Z_col_id = dialog_->Z_column()-1;

	if( X_col_id == Y_col_id || X_col_id == Z_col_id || Y_col_id == Z_col_id ) {
		GsTLcerr << "The same column number was selected for multiple coordinates \n" << gstlIO::end;
		return 0;
	}

	grid->set_dimensions( nx,ny,nz, x_size, y_size, z_size );
	grid->origin( Cartesian_grid::location_type( Ox,Oy,Oz) );

	std::string buffer;

	// read title
	std::getline( infile, buffer, '\n');
	
	// read nb of columns
	int columns_count;
	infile >> columns_count;
	std::getline( infile, buffer, '\n');

	// read property names 
	std::vector<std::string> property_names;
	for( int i=0; i<columns_count; i++ ) {
		std::getline( infile, buffer, '\n');
		QString prop_name( buffer.c_str() );
		if( i != X_col_id && i != Y_col_id && i != Z_col_id ) {
			property_names.push_back( prop_name.stripWhiteSpace().ascii() );
		}
	}

	std::vector< std::vector<float> > property_values( property_names.size() );

	// read the property values
	float val;
	int count = 0;
	int step_xy = nx*ny; // # of pixels on xy plane
	bool use_no_data_value = dialog_->use_no_data_value();
	float no_data_value = dialog_->no_data_value();
	std::vector<std::string> buf;

	while( infile ) {
		int property_index=0;
		Point_set::location_type loc;
		std::getline( infile, buffer, '\n');
		buf = String_Op::decompose_string( buffer, " ", false );
		if (buf.empty()) break;

		loc[0] = String_Op::to_number<float>(buf[X_col_id]);
		loc[1] = String_Op::to_number<float>(buf[Y_col_id]);
		loc[2] = String_Op::to_number<float>(buf[Z_col_id]);

		for( int j=0; j< columns_count; j++ ) {

			if (j != X_col_id && j != Y_col_id && j != Z_col_id){	
				val = String_Op::to_number<float>(buf[j]);
				if( use_no_data_value ) {
					if( val == no_data_value )
						val = GsTLGridProperty::no_data_value;          
				}
				property_values[ property_index ].push_back( val );
				property_index++;
			}

		}
		if (infile){
			GsTLGridNode ijk;
			ijk[0] = (loc[0]-Ox)/x_size;
			ijk[1] = (loc[1]-Oy)/y_size;
			ijk[2] = (loc[2]-Oz)/z_size;
			int idFull = ijk[2]*step_xy+ijk[1]*nx+ijk[0];
			grid->setConversionPair(idFull,count);
			++count;
			grid->insertLocation(ijk);
		}

	}
	
	grid->initMaskedGrid(dialog_->isregular());
	appli_message("filter is " << count);

	// pass values from buffer to properties
	for( unsigned int k= 0; k < property_names.size(); k++ ) {
		GsTLGridProperty* prop = grid->add_property( property_names[k] );
		for( int l=0; l < grid->size(); l++ ) {
			prop->set_value( property_values[k][l], l );
		}
	}

	return grid;
}

Geostat_grid* Gslib_mgrid_infilter::read( std::ifstream& infile ) 
{
  std::string name( (dialog_->name().stripWhiteSpace()).latin1() );

  // ask manager to get a new grid and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return 0;
  }

  ni = Root::instance()->new_interface( "reduced_grid", 
                                        gridModels_manager + "/" + name );
  Reduced_grid* grid = dynamic_cast<Reduced_grid*>( ni.raw_ptr() );
  appli_assert( grid != 0 );

  if (dialog_->isregular())
	  return readRegular(infile, grid);
  else
	  return readPointset(infile, grid);
}

bool Gslib_mgrid_infilter::has_valid_parameters() const {
  if (dialog_->nx() == 0 || dialog_->ny() == 0 || dialog_->nz() == 0) {
    GsTLcerr << "Enter bounding box information" << gstlIO::end;
    return false;
  }
  return !dialog_->name().isEmpty();
}

//========================================================


Gslib_grid_infilter::Gslib_grid_infilter() 
: dialog_( new Gslib_input_grid_dialog( qApp->mainWidget() ) ) {
}

Gslib_grid_infilter::~Gslib_grid_infilter() {
  // deleting dialog_ causes a segfault. See why.
  //  delete dialog_;
  dialog_ = 0;
}

QWidget* Gslib_grid_infilter::init_dialog( std::ifstream& ) {
  dialog_ = new Gslib_input_grid_dialog( qApp->mainWidget() );
  return dialog_;
}


Geostat_grid* Gslib_grid_infilter::read( std::ifstream& infile ) {

  int nx = dialog_->nx();
  int ny = dialog_->ny();
  int nz = dialog_->nz();
  float x_size = dialog_->x_size();
  float y_size = dialog_->y_size();
  float z_size = dialog_->z_size();
  float Ox = dialog_->Ox();
  float Oy = dialog_->Oy();
  float Oz = dialog_->Oz();

  std::string name( (dialog_->name().stripWhiteSpace()).latin1() );

  // ask manager to get a new grid and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return 0;
  }

  appli_message( "creating new grid '" << name << "'" 
		             << " of dimensions: " << nx << "x" << ny << "x" << nz);

  ni = Root::instance()->new_interface( "cgrid", 
                                        gridModels_manager + "/" + name );
  Cartesian_grid* grid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
  appli_assert( grid != 0 );

  grid->set_dimensions( nx, ny, nz,
			x_size, y_size, z_size);
  grid->origin( GsTLPoint( Ox,Oy,Oz) );
  appli_message( "grid resized to " << nx << "x" << ny << "x" << nz
		<< "  total=: " << grid->size() );

  std::string buffer;
  
  //-------------------------
  //   now, read the file

  // read title
  std::getline( infile, buffer, '\n');

  // read nb of properties
  int property_count;
  infile >> property_count;
  std::getline( infile, buffer, '\n');

  
  // check whether the file contains multiple realizations
  bool has_multi_real = false;

  int lines_to_skip = grid->size() + property_count;
  for( int pos=0; pos < lines_to_skip ; pos++ )
    std::getline( infile, buffer, '\n');
  float val;
  if( infile >> val ) has_multi_real = true;
  
  // reposition the stream
  infile.clear();
  infile.seekg( 0 );
  std::getline( infile, buffer, '\n');
  std::getline( infile, buffer, '\n');

  if( has_multi_real ) {
    std::vector<MultiRealization_property*> properties;
    for( int i=0; i<property_count; i++ ) {
	  std::getline( infile, buffer, '\n');
      QString prop_name( buffer.c_str() );
      MultiRealization_property* prop;

	  prop = grid->add_multi_realization_property( prop_name.stripWhiteSpace().ascii() );
	  if( !prop ) {
		  GsTLcerr << "Several properties share the same name " << gstlIO::end;
		  return 0;
	  }

	  properties.push_back( prop );

	}


    while( infile ) {
      if( !infile ) break;
      char c = infile.peek();
      if( !std::isdigit(c) ) break;

      std::vector<GsTLGridProperty*> props;
	  int index = 0;
      for( unsigned int ii=0; ii < property_count; ii++) {
		 
		  GsTLGridProperty* prop = properties[index]->new_realization();
		  props.push_back( prop );
		  ++index;

      }
      read_one_realization( infile, props, grid->size() );
    }
  }

  else {
    std::vector<GsTLGridProperty*> properties;
    for( int i=0; i<property_count; i++ ) {
	  std::getline( infile, buffer, '\n');
      QString prop_name( buffer.c_str() );

      GsTLGridProperty* prop = 
        grid->add_property( prop_name.stripWhiteSpace().ascii() );
      properties.push_back( prop );
	}    
    read_one_realization( infile, properties, grid->size() );

  }
  
  return grid;
}


bool Gslib_grid_infilter::
read_one_realization( std::ifstream& infile, 
                      const std::vector<GsTLGridProperty*>& properties,
                      long int grid_size) {
  // read the property values
  int property_count = properties.size();
  long int count = 0;
  long int max_read = properties.size() * grid_size;
  float val;

  if( dialog_->use_no_data_value() ) {
    float no_data_value = dialog_->no_data_value();

    while( infile && count < max_read ) {
      infile >> val;
      int prop_id = count % property_count;
      int node_id = count / property_count;
      if( val != no_data_value )
        properties[prop_id]->set_value( val, node_id );
  
      count ++;
    }
    
  }
  else {
    // there is no "no-data-value"
    while( infile && count < max_read ) {
      infile >> val;
      int prop_id = count % property_count;
      int node_id = count / property_count;
      properties[prop_id]->set_value( val, node_id );
      count ++;
    }
  }

  std::string buffer;
  std::getline( infile, buffer, '\n' );
  return true;
}



bool Gslib_grid_infilter::has_valid_parameters() const {
  return !dialog_->name().isEmpty();
}





//==============================================================
Named_interface* Gslib_poinset_infilter::create_new_interface( std::string& ) {
  return new Gslib_poinset_infilter();
}

Gslib_poinset_infilter::Gslib_poinset_infilter() 
  : dialog_( new Gslib_input_pointset_dialog( qApp->mainWidget() ) ) {
}

Gslib_poinset_infilter::~Gslib_poinset_infilter() {
//  delete dialog_;
  dialog_ = 0;
}

QWidget* Gslib_poinset_infilter::init_dialog( std::ifstream& infile ) {
  dialog_ = new Gslib_input_pointset_dialog( infile, qApp->mainWidget() );
  return dialog_;
}

bool Gslib_poinset_infilter::has_valid_parameters() const {
  return !dialog_->name().isEmpty();
}

Geostat_grid* Gslib_poinset_infilter::read( std::ifstream& infile ) {


  std::string name( dialog_->name().stripWhiteSpace().latin1() );

  const int X_col_id = dialog_->X_column()-1;
  const int Y_col_id = dialog_->Y_column()-1;
  const int Z_col_id = dialog_->Z_column()-1;
  
  if( X_col_id == Y_col_id || X_col_id == Z_col_id || Y_col_id == Z_col_id ) {
    GsTLcerr << "The same column number was selected for multiple coordinates \n" << gstlIO::end;
    return 0;
  }

  std::string buffer;
  
  //-------------------------
  //   now, read the file

  // read title
  std::getline( infile, buffer, '\n');

  // read nb of columns
  int columns_count;
  infile >> columns_count;
  std::getline( infile, buffer, '\n');


  // read property names 
  std::vector<std::string> property_names;
  for( int i=0; i<columns_count; i++ ) {
    std::getline( infile, buffer, '\n');
    QString prop_name( buffer.c_str() );

    if( i != X_col_id && i != Y_col_id && i != Z_col_id ) {
      property_names.push_back( prop_name.stripWhiteSpace().ascii() );
    }
  }

  std::vector< std::vector<float> > property_values( property_names.size() );
  std::vector< Point_set::location_type > point_locations;

  // read the property values
  float val;
  bool use_no_data_value = dialog_->use_no_data_value();

  while( infile ) {
    int property_index=0;
    Point_set::location_type loc;
    for( int j=0; j< columns_count; j++ ) {
      if( ! (infile >> val) ) {
	      break;
      }
      bool is_property_value = true;

      if( j == X_col_id ) {
	      loc[0] = val;
	      is_property_value = false;
      }
      if( j == Y_col_id ) {
	      loc[1] = val;
	      is_property_value = false;
      }
      if( j == Z_col_id ) {
	      loc[2] = val;
	      is_property_value = false;
      }
      if( is_property_value ) {
        if( use_no_data_value ) {
          float no_data_value = dialog_->no_data_value();
          if( val == no_data_value )
            val = GsTLGridProperty::no_data_value;          
        }
  	    property_values[ property_index ].push_back( val );
	      property_index++;
      }
	
    }
    if( infile )
      point_locations.push_back( loc );
  }
  //   done reading file
  //----------------------------

  int point_set_size = point_locations.size();
  appli_message( "read " << point_set_size << " points" );

  // We now have a vector containing all the locations and another one with
  // all the property values.
  // Create a pointset, initialize it with the data we collected, and we're done
 
  // ask manager to get a new pointset and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return 0;
  }

  std::string size_str = String_Op::to_string( point_set_size );
  ni = Root::instance()->new_interface( "point_set://" + size_str, 
					gridModels_manager + "/" + name );
  Point_set* pset = dynamic_cast<Point_set*>( ni.raw_ptr() );
  appli_assert( pset != 0 );

  pset->point_locations( point_locations );

  for( unsigned int k= 0; k < property_names.size(); k++ ) {
    GsTLGridProperty* prop = pset->add_property( property_names[k] );
    for( int l=0; l < point_set_size; l++ ) {
      prop->set_value( property_values[k][l], l );
    }
  }

  return pset;
}





//===========================================================
Gslib_outfilter::Gslib_outfilter()  {

}

Gslib_outfilter::~Gslib_outfilter() {

}

bool Gslib_outfilter::write( std::string outfile, const Geostat_grid* grid, 
                            std::string* errors ) 
{
  std::ofstream out( outfile.c_str() );
  if( !out ) {
    if( errors )
      errors->append( "can't write to file: " + outfile );
    return false;
  }

  return this->write( out, grid );
}

bool Gslib_outfilter::writeReduced2Cartesian( std::ofstream& outfile, const Geostat_grid* grid ) 
{
	typedef std::list<std::string>::const_iterator string_iterator; 
	std::vector< const GsTLGridProperty* > properties;
    std::list<std::string> & property_names = _list_to_write;
	const Reduced_grid * rgrid = dynamic_cast<const Reduced_grid *>(grid);

	if (!rgrid) return false;

	int nb_properties = property_names.size()+1; // for the mask column

	outfile << nb_properties << std::endl;

	for( string_iterator it = property_names.begin(); it != property_names.end();
		++ it ) 
	{
		outfile << *it << std::endl;
		properties.push_back( grid->property( *it ) );
	}
	outfile << rgrid->maskColumn() << std::endl; //use saved name for the mask column

	int grid_size = rgrid->trueSize();

	// Write the property values
	for( int i=0; i < grid_size ; i++ ) {
		for( unsigned int j=0; j < property_names.size(); ++j ) {
			if (rgrid->isActive(i)) {
				appli_assert(rgrid->full2reduced(i) != -1);
				if (!properties[j]->is_informed(rgrid->full2reduced(i))) 
					cout << "-9999 ";
				
				outfile << properties[j]->get_value( rgrid->full2reduced(i) ) << " ";
			}
			else 
				outfile << "-9999 ";
		}

		if (rgrid->isActive(i)) {
			outfile << "1 ";
		}
		else
			outfile << "0 ";
		outfile << std::endl;
	}
	return true;
}

bool Gslib_outfilter::write( std::ofstream& outfile, const Geostat_grid* grid ) {
  /* Not all objects are saved the same way: if we are dealing with a 
   * stratigraphic grid, we don't need to save the point coordinates.
   */

  // write the name of the object
  std::string grid_name;
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager );
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( dir );
  grid_name = dir->name( (Named_interface* ) grid );

  const RGrid * rgrid;
  if (rgrid = dynamic_cast<const RGrid*>(grid))
	outfile << grid_name << " " << "(" << rgrid->nx() << "x" << rgrid->ny() 
			<< "x" << rgrid->nz() << ")" << std::endl;
  else
	  outfile << grid_name << std::endl;
  
  typedef std::list<std::string>::const_iterator string_iterator; 
  std::list<std::string> & property_names = _list_to_write;
  //std::list<std::string> property_names = grid->property_list();
  std::vector< const GsTLGridProperty* > properties;

  if (_maskToRegular)
	  return writeReduced2Cartesian(outfile, grid);

  bool output_locations = false;
  int nb_properties = property_names.size();

  //TL modified
  if( dynamic_cast<const Point_set*>( grid ) || dynamic_cast<const Reduced_grid*>(grid)) {
    output_locations = true;
    nb_properties += 3;
  }


  // write number of properties and property names 
  outfile << nb_properties << std::endl;
  if( output_locations )
    outfile << "X" << std::endl 
	    << "Y" << std::endl
	    << "Z" << std::endl;
  
  for( string_iterator it = property_names.begin(); it != property_names.end();
       ++ it ) {
    outfile << *it << std::endl;
    properties.push_back( grid->property( *it ) );
  }
  
  int grid_size;
  if( properties.empty() )
    grid_size = 0;
  else
    grid_size = properties[0]->size();

  // Write the property values
  for( int i=0; i < grid_size ; i++ ) {
    if( output_locations ) {
		/*
	  if (dynamic_cast<const Reduced_grid*>(grid)) {
		  Geostat_grid::location_type loc = (dynamic_cast<const Reduced_grid*>(grid))->location( i );
          outfile << (int)(loc.x()) << " " << (int)(loc.y()) << " " << (int)(loc.z()) << " ";
	  }
	  else {
          Geostat_grid::location_type loc = grid->location( i );
		  outfile << loc.x() << " " << loc.y() << " " << loc.z() << " ";
	  }
	  */
      Geostat_grid::location_type loc = grid->location( i );
	  outfile << loc.x() << " " << loc.y() << " " << loc.z() << " ";

	}
    for( unsigned int j=0; j < property_names.size(); ++j ) {
      if( properties[j]->is_informed( i ) )
        outfile << properties[j]->get_value( i ) << " ";
      else
        outfile << "-9999 ";
	
    }
	
    outfile << std::endl;
  }
  return true;
}

