/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "actions" module of the Geostatistical Earth
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


#include <GsTLAppli/actions/obj_manag_actions.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/filters/filter.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>

// these 3 Qt files are needed by Load_project
#include <qdir.h>
#include <qfileinfo.h>
#include <qstring.h>

//#ifdef _DEBUG
//#undef _DEBUG
//#include <Python.h>
//#define _DEBUG
//#else
//#include <Python.h>
//#endif
#include <Python.h>

#include <stdlib.h>
#include <stdio.h>
//#include <GsTLAppli/filters/eclipse/pprt.h>
#include <memory.h>
#include <list>

// TL modified
#include <GsTLAppli/actions/python_wrapper.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>
#include <qprogressdialog.h>
#include <qapplication.h>
//#include <GsTLAppli/filters/gslib/gslib_filter.h>
//#include <GsTLAppli/gui/appli/oinv_project_view.h>

Named_interface* New_rgrid::create_new_interface( std::string& ) {
  return new New_rgrid();
}

Named_interface* New_cartesian_grid_action::create_new_interface( std::string& ) {
  return new New_cartesian_grid_action();
}

Named_interface* Load_object_from_file::create_new_interface( std::string& ) {
  return new Load_object_from_file();
}

Named_interface* Save_geostat_grid::create_new_interface( std::string& ) {
  return new Save_geostat_grid();
}

Named_interface* Load_project::create_new_interface( std::string& ) {
  return new Load_project();
}

// TL modified
/*
Named_interface* Load_sim::create_new_interface( std::string& ) {
  return new Load_sim();
}
*/
Named_interface* RunScript::create_new_interface( std::string& ) {
  return new RunScript();
}


// name of the python module for parsing Eclipse PRT file
//const std::string Load_sim::parser = "readprt";

//=============================================


bool
New_rgrid::init( std::string& parameters, GsTL_project* proj,
                Error_messages_handler* errors ) {
  proj_ = proj ;
  errors_ = errors;

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() < 4 )
    return false;

  name_ = params[0];
  nx_ = String_Op::to_number<int>( params[1] );
  ny_ = String_Op::to_number<int>( params[2] );
  nz_ = String_Op::to_number<int>( params[3] );
  
  return true;
}


bool
New_rgrid::exec() {
  appli_warning( "New_rgrid::exec() NOT FULLY IMPLEMENTED" );

  std::string full_name( "/GridObject/Model/" + name_ );
  SmartPtr<Named_interface> ni = 
      Root::instance()->new_interface("RGrid", full_name);
  
  if( ni.raw_ptr() == 0 ) {
    errors_->report( "object " + full_name + " already exists. Use a different name." );
//    appli_warning( "object " << full_name << "already exists" );
    return false;
  }
  
  proj_->new_object( name_ );
  return true;
}


//=============================================

New_cartesian_grid_action::New_cartesian_grid_action() {
  xsize_ = 1;
  ysize_ = 1; 
  zsize_ = 1;
  Ox_ = 0;
  Oy_ = 0;
  Oz_ = 0; 
}

bool
New_cartesian_grid_action::init( std::string& parameters, GsTL_project* proj,
                                 Error_messages_handler* errors ) {
  proj_ = proj ;
  errors_ = errors;

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() < 4 )
    return false;

  name_ = params[0];
  nx_ = String_Op::to_number<int>( params[1] );
  ny_ = String_Op::to_number<int>( params[2] );
  nz_ = String_Op::to_number<int>( params[3] );
  
  if( params.size() >= 7 ) {
    xsize_ = String_Op::to_number<float>( params[4] );;
    ysize_ = String_Op::to_number<float>( params[5] );
    zsize_ = String_Op::to_number<float>( params[6] );
  }

  if( params.size() == 10 ) {
    Ox_ = String_Op::to_number<float>( params[7] );;
    Oy_ = String_Op::to_number<float>( params[8] );
    Oz_ = String_Op::to_number<float>( params[9] );
  }

  return true;
}


bool
New_cartesian_grid_action::exec() {

  std::string full_name( "/GridObject/Model/" + name_ );
  SmartPtr<Named_interface> ni = 
      Root::instance()->new_interface("cgrid", full_name);
  
  if( ni.raw_ptr() == 0 ) {
    errors_->report( "Object " + full_name + " already exists. Use a different name." );
//    appli_warning( "object " << full_name << "already exists" );
    return false;
  }
  
  Cartesian_grid* grid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
  grid->set_dimensions( nx_, ny_, nz_,
			xsize_, ysize_, zsize_ );
  grid->origin( GsTLPoint( Ox_, Oy_, Oz_ ) );

  proj_->new_object( name_ );
  return true;
}




//=========================================
Load_object_from_file::~Load_object_from_file() {
}

bool Load_object_from_file::init( std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() != 2 )
    return false;


  file_name_ = params[0];

  if( params[1] == "All" ) {
    if( !find_filter( file_name_ ) ) {
      errors->report( "Cannot find appropriate filter to read " + file_name_ );
      return false;
    }
    return true;
  }

  SmartPtr<Named_interface> ni =
    Root::instance()->new_interface( params[1], topLevelInputFilters_manager + "/" );

  if( ni.raw_ptr() == 0 )
    return false;

  Input_filter* filter = dynamic_cast<Input_filter*>( ni.raw_ptr() );
  appli_assert( filter );

  filter_ = SmartPtr<Input_filter>(filter);

  return true;
}


bool Load_object_from_file::exec() {
  std::string filter_errors;

  Geostat_grid* grid = filter_->read( file_name_, &filter_errors );

  // Notify the project that a new object was added. To do so, we need to 
  // get the name of the new grid from the grid manager.
  if( grid )  {
	  SmartPtr<Named_interface> dir = Root::instance()->interface( gridModels_manager );
	  Manager* mng = dynamic_cast<Manager*>( dir.raw_ptr() );
	  appli_assert( mng );

	  std::string grid_name = mng->name( dynamic_cast<Named_interface*>(grid) ); 

	  proj_->new_object(grid_name);
  }
  
  if( filter_errors.empty() ) return true;

  errors_->report( filter_errors );
  return false;
}



bool Load_object_from_file::find_filter( const std::string& filename ) {
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( topLevelInputFilters_manager );
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( dir );

  Manager::type_iterator begin = dir->begin();
  Manager::type_iterator end = dir->end();

  for( ; begin != end ; ++begin ) {
    SmartPtr<Named_interface> filter_ni = 
      Root::instance()->new_interface( *begin, topLevelInputFilters_manager + "/" );
    Input_filter* filter = dynamic_cast<Input_filter*>( filter_ni.raw_ptr() );  
    appli_assert( filter );
    if( filter->can_handle( filename ) ) {
      filter_ = SmartPtr<Input_filter>( filter );
      return true;
    }
  }

  return false;
}




//=========================================

bool Save_geostat_grid::init( std::string& parameters, 
			      GsTL_project* proj, Error_messages_handler* errors ) {
 proj_ = proj;
  errors_ = errors;

  //TL modified
  std::list<std::string> write_list;
  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

 
  // Get the grid from the manager

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  appli_assert( grid );
  grid_ = SmartPtr<Geostat_grid>( grid );

  // Get the filter from the manager
  SmartPtr<Named_interface> filter_ni =
	  Root::instance()->new_interface( params[2], 
	  outfilters_manager + "/" );

  if( filter_ni.raw_ptr() == 0 )
	  return false;

  Output_filter* filter = dynamic_cast<Output_filter*>( filter_ni.raw_ptr() );
  appli_assert( filter );

  //TL modified
  if (params.size() == 3)
    filter->setWriteMode(false);
  else {
    if (params[3] == "0")
		filter->setWriteMode(false);
	else 
		filter->setWriteMode(true);
	for (int i = 4; i < params.size(); ++i)
	  write_list.push_back(params[i]);
  }

  //if properties not specified, take entire grid
  if (write_list.empty())
	  write_list = grid->property_list();

  // Initialize the file name
  file_name_ = params[1];
  std::string filter_errors = "";

  filter->setWriteList(write_list);
  filter_ = SmartPtr<Output_filter>(filter);
  return filter_->write( file_name_, grid_.raw_ptr(), &filter_errors);
}




bool Save_geostat_grid::exec() {
	return true;
}


// TL modified
bool RunScript::init(std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) 
{
	_proj = proj;
	_errors = errors;
	_scriptfile = parameters;

	if (_scriptfile == "")
		return false;
	return true;
}

bool RunScript::exec()
{
  std::string buf = "";
  char ch;

  // redirect stdout and stderr to sgems output zones
  
  PyRun_SimpleString(""
   "import redirect\n"
   "class CoutLogger:\n"
   "    def __init__(self):\n"
   "        self.buf = []\n"
   "    def write(self, data):\n"
   "        self.buf.append(data)\n"
   "        if data.endswith('\\n'):\n"
   "            redirect.sgems_cout(''.join(self.buf))\n"
   "            self.buf = []\n"
   "\n"
   "class CerrLogger:\n"
   "    def __init__(self):\n"
   "        self.buf = []\n"
   "    def write(self, data):\n"
   "        self.buf.append(data)\n"
   "        if data.endswith('\\n'):\n"
   "            redirect.sgems_cerr(''.join(self.buf))\n"
   "            self.buf = []\n"
   "\n"
   "import sys\n"
   "sys.stdout = CoutLogger()\n"
   "sys.stderr = CerrLogger()\n"
   "");
  
  FILE* fp = fopen( _scriptfile.c_str(), "r" );
  if( !fp ) {
    GsTLcerr << "can't open file " << _scriptfile.c_str() << gstlIO::end;
    return false;
  }

  while ((ch = fgetc(fp)) != EOF)
	  buf += ch;
  buf += "\n";

  PyRun_SimpleString( (char*) buf.c_str() );

  fclose( fp );	
  if( Python_project_wrapper::is_project_modified() ) {
    Python_project_wrapper::project()->update();
  }

  return true;
}

// ===================================


// TL modified
/*
Load_sim::Load_sim() {

}
Load_sim::~Load_sim() {
}

bool Load_sim::init(std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;

  dirname_ =  QString( parameters.c_str() );

  // a very crude validity check
  if( !dirname_.endsWith( ".prt", false ) ) {
    errors->report("wrong extension");
    return false;
  }
  
  return true;
}

bool Load_sim::exec() {
	Prt * p = NULL;
	int i,j,index = 0, totalsize = 0;
	PyObject * mod, *func;

	Py_Initialize();
	PyRun_SimpleString(""
		"import redirect\n"
		"class CoutLogger:\n"
		"    def __init__(self):\n"
		"        self.buf = []\n"
		"    def write(self, data):\n"
		"        self.buf.append(data)\n"
		"        if data.endswith('\\n'):\n"
		"            redirect.sgems_cout(''.join(self.buf))\n"
		"            self.buf = []\n"
		"\n"
		"class CerrLogger:\n"
		"    def __init__(self):\n"
		"        self.buf = []\n"
		"    def write(self, data):\n"
		"        self.buf.append(data)\n"
		"        if data.endswith('\\n'):\n"
		"            redirect.sgems_cerr(''.join(self.buf))\n"
		"            self.buf = []\n"
		"\n"
		"import sys\n"
		"sys.stdout = CoutLogger()\n"
		"sys.stderr = CerrLogger()\n"
		"");

	// !!! Main program will wait for this to finish !!!

	PyObject * fromList = Py_BuildValue("[s]", "readprt");

	if (!(mod = PyImport_ImportModuleEx((char *)parser.c_str(), NULL, NULL, fromList))){
		PyRun_SimpleString("print 'unable to get module'");
		PyErr_Print();
		return false;
	}

	if (!(func = PyDict_GetItemString(PyModule_GetDict(mod), parser.c_str()))) {
		PyRun_SimpleString("print 'unable to get function'");
		PyErr_Print();
		return false;
	}

	if (!PyObject_CallObject(func, Py_BuildValue("(s)",dirname_.ascii()))) {
		appli_warning("Unable to run script " << parser);
		PyErr_Print();
		return false;
	}


	
	pprt PRT;
	if (!(p = PRT.fill()))
		return false;

	totalsize = (p->X) * (p->Y) * (p->Z);

    appli_message( "creating new grid '" << dirname_.latin1() << "'" 
	  	           << " of dimensions: " << p->X << "x" << p->Y << "x" << p->Z); 
	
	// create a new grid
	SmartPtr<Named_interface> ni = Root::instance()->new_interface( "cgrid", 
		gridModels_manager + "/" + "eclipse" );
	Cartesian_grid* grid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
	appli_assert( grid != 0 );
	grid->set_dimensions( p->X, p->Y, p->Z);

	
	for (i = 0; i < p->num_dates; ++i) {
		for (j = 0; j < p->num_properties; ++j, ++index) {
			string temp = p->dates[i];
			temp+="_";
			temp+=p->properties[j];
			GsTLGridProperty* prop = grid->add_property( temp );
			transform((p->data)+index*totalsize, p->X, p->Y, p->Z, prop);
		}
	}

	proj_->new_object("eclipse");

	return true;
}

// performs axis conversion between sgems and eclipse format
void Load_sim::transform(const Type * src, int x, int y,
			      int z, GsTLGridProperty * dest)
{
  int i,j,k, cur, index = 0;

 
  for (i = z-1; i >= 0; --i) {
	  for (j = y-1; j >= 0; --j) { 
		cur = j*x+i*x*y;
		for (k = 0; k < x; ++k,++index){
			dest->set_value(*(src+cur+k), index);
		}
	  }
  }
}
*/

//===========================================



//===========================================
Load_project::~Load_project() {
}

bool Load_project::init( std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;

  dirname_ =  QString( parameters.c_str() );
  if( !dirname_.endsWith( ".prj" ) && !dirname_.endsWith( ".prj/") ) return false;

  QFileInfo info( dirname_ );
  if( !info.isDir() ) return false;

  return true;
}


bool Load_project::exec() {

  QDir* dir = new QDir( dirname_ );
  if( !dir->cd( dirname_ ) ) {
    //GsTLcerr << "Could not load project " << dirname_.toAscii() << gstlIO::end;
    return false;
  }

  // if there is already a project, close it
  if( !proj_->is_empty() )
    proj_->clear();
  
  proj_->name( std::string( dirname_.toLatin1() ) );

  QFileInfoList files_info = dir->entryInfoList( QDir::Files | QDir::NoSymLinks );
  //files_info::iterator it = files_info->begin();
  //QFileInfoListIterator it( *files_info );
  //QFileInfo* f_info = 0;
  for(int i = 0; i < files_info.size(); i++) {
  //while( (f_info = it.current()) != 0 ) {
    //files_info[i];
    QString abspath = files_info[i].absoluteFilePath();
    std::string param( std::string( abspath.toLatin1() ) + Actions::separator + "s-gems" );
    bool ok = proj_->execute( "LoadObjectFromFile", param, errors_ );
    if( !ok ) 
      errors_->report( "... this error occurred while loading \"" + 
                             std::string( files_info[i].fileName().toLatin1() ) +"\"" );

    //++it;
  }

  proj_->reset_change_monitor();
  return true;
}




//==========================================

bool Copy_property::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;
  bool overwrite = false;
  bool mark_as_hard = false;

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() < 4 )
    return false;

  if( params.size() >= 5 ) {
    overwrite = String_Op::to_number<bool>( params[4] );
    if( params.size() >=6 )
      mark_as_hard = String_Op::to_number<bool>( params[5] );
  }


  // Get the 2 grids from the manager

  SmartPtr<Named_interface> serv_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  server_ = dynamic_cast<Geostat_grid*>( serv_grid_ni.raw_ptr() );

  SmartPtr<Named_interface> client_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[2] );
  client_ = dynamic_cast<Geostat_grid*>( client_grid_ni.raw_ptr() );

  if( !client_ || !server_ ) return false;

  server_prop_ = server_->property( params[1] );
  client_prop_ = client_->property( params[3] );
  if( !client_prop_ ) 
    client_prop_ = client_->add_property( params[3] );

  if( !server_prop_ || !client_prop_ ) return false;

  copier_ = Property_copier_factory::get_copier( server_, client_ );
  if( !copier_ ) return false;

  copier_->does_overwrite( overwrite );
  copier_->marks_as_hard( mark_as_hard );

  return true;
}


bool Copy_property::exec() {
  bool ok = copier_->copy( server_, server_prop_, client_, client_prop_ );
  if( !ok ) return false;

  proj_->update();
  return ok;
}


Named_interface* Copy_property::create_new_interface( std::string& ) {
  return new Copy_property;
}



//================================================

bool Swap_property_to_disk::init( std::string& parameters, GsTL_project* proj,
                                 Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return false;

  // get the grid from the grid manager
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );

  if( !grid ) {
    std::ostringstream message;
    message << "grid \"" << params[0] << "\" does not exist"; 
    errors->report_if( true, message.str() );
    return false;
  }

  for( unsigned int i=1; i < params.size() ; i++ ) {
    GsTLGridProperty* prop = grid->property( params[i] );
    if( prop )
      properties_to_swap_.push_back( prop );
  }

  return true;
}

bool Swap_property_to_disk::exec() {
  for( unsigned int i=0; i < properties_to_swap_.size() ; i++ ) {
    properties_to_swap_[i]->swap_to_disk();
  }
  return true;
}


Named_interface* Swap_property_to_disk::create_new_interface( std::string& ) {
  return new Swap_property_to_disk; 
}



//================================================

bool Swap_property_to_ram::init( std::string& parameters, GsTL_project* proj,
                                 Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return false;

  // get the grid from the grid manager
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );

  if( !grid ) {
    std::ostringstream message;
    message << "grid \"" << params[0] << "\" does not exist"; 
    errors->report( false, message.str() );
    return false;
  }

  for( unsigned int i=1; i < params.size() ; i++ ) {
    GsTLGridProperty* prop = grid->property( params[i] );
    if( prop )
      properties_to_swap_.push_back( prop );
  }

  return true;
}

bool Swap_property_to_ram::exec() {
  for( unsigned int i=0; i < properties_to_swap_.size() ; i++ ) {
    properties_to_swap_[i]->swap_to_memory();
  }
  return true;
}


Named_interface* Swap_property_to_ram::create_new_interface( std::string& ) {
  return new Swap_property_to_ram; 
}





//================================================

bool Delete_objects::init( std::string& parameters, GsTL_project* project,
                           Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.empty() ) return true;

  SmartPtr<Named_interface> ni_dir =
    Root::instance()->interface( gridModels_manager );
  Manager* grid_manager = dynamic_cast<Manager*>( ni_dir.raw_ptr() );
  appli_assert( grid_manager );
  if( !grid_manager ) return false;
  
  for( std::vector<std::string>::const_iterator it = params.begin() ;
        it != params.end(); ++it ) {
    bool ok = grid_manager->delete_interface( "/" + (*it) );
    if( ok )
      project->deleted_object( *it );
  }

  project->update();
  return true;
}


bool Delete_objects::exec() {
  return true;
}


Named_interface* Delete_objects::create_new_interface( std::string& ) {
  return new Delete_objects; 
}




//================================================
/* DeleteProperties grid_name::prop1::prop2...
*/
bool Delete_properties::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return true;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  
  for( unsigned int i = 1 ; i < params.size() ; i++ ) {
    grid->remove_property( params[i] );
  }

  proj->update( params[0] );
  return true;
}


bool Delete_properties::exec() {
  return true;
}


Named_interface* Delete_properties::create_new_interface( std::string& ) {
  return new Delete_properties; 
}




//================================================
/* Clear_property_value_if grid_name::prop::min::max
* will set to un-informed all value of property "prop" in grid "grid_name"
* that are in range [min,max]
*/
bool Clear_property_value_if::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 4 ) {
    errors->report( "some parameters are missing" );  
    return false;
  }

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  
  GsTLGridProperty* prop = grid->property( params[1] );
  if( !prop ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has no property called \"" << params[1] << "\"";
    errors->report( message.str() ); 
    return false;
  }

  float min = String_Op::to_number<float>( params[2] );
  float max = String_Op::to_number<float>( params[3] );

  for( int i = 0 ; i < prop->size() ; i++ ) {
    if( prop->get_value( i ) >= min && prop->get_value(i) <= max )
      prop->set_not_informed( i );
  }

  proj->update( params[0] );
  return true;
}


bool Clear_property_value_if::exec() {
  return true;
}


Named_interface* 
Clear_property_value_if::create_new_interface( std::string& ) {
  return new Clear_property_value_if; 
}








