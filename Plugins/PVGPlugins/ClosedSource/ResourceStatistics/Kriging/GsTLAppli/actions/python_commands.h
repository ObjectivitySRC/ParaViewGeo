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

#ifndef __GSTLAPPLI_ACTIONS_PYTHON_COMMANDS_H__ 
#define __GSTLAPPLI_ACTIONS_PYTHON_COMMANDS_H__ 
 
//#ifdef _DEBUG
//#undef _DEBUG
//#include <Python.h>
//#define _DEBUG
//#else
//#include <Python.h>
//#endif
#include <Python.h>

#include <GsTLAppli/actions/common.h>
#include <GsTLAppli/actions/python_wrapper.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>

// TL added this
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

#include <string> 
#include <algorithm>
#include <fstream>


static PyObject* sgems_execute(PyObject *self, PyObject *args)
{
  char* command_str;
  if( !PyArg_ParseTuple(args, "s:execute", &command_str) )
    return NULL;
  
  std::string command( command_str );
  String_Op::string_pair sgems_command =
    String_Op::split_string( command, " ", false );

  Error_messages_handler error_messages;
  bool ok = 
    Python_project_wrapper::project()->execute( sgems_command.first, sgems_command.second,
				       &error_messages );
  if( !ok ) {
    std::ostringstream message;
    message << "Error executing S-GeMS command \"" << sgems_command.first 
            << "\": " ;
    if( !error_messages.empty() )
      message << error_messages.errors() ; 
    
    *GsTLAppli_Python_cerr::instance() << message.str() << gstlIO::end;
  }
  Py_INCREF(Py_None);
  return Py_None;
}


static GsTLGridProperty* get_coordinates( Geostat_grid* grid, int coord ) {
  GsTLGridProperty* prop = new GsTLGridProperty(grid->size(), "coord" );
  for( int i=0; i < grid->size() ; i++ ) {
    Geostat_grid::location_type loc = grid->location( i );
    prop->set_value( loc[coord], i );
  }
  return prop;
}


static PyObject* sgems_get_property( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;
  Reduced_grid * redgrid;
  long int trueSize=0;
 
  if( !PyArg_ParseTuple(args, "ss", &obj_str, &prop_str) )
    return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );
  
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  bool delete_prop = false;
  GsTLGridProperty* prop = 0;
  if( prop_name == "_X_" ) {
    prop = get_coordinates( grid, 0 );
    delete_prop = true;
  } else if (prop_name == "_Y_" ) {
    prop = get_coordinates( grid, 1 );
    delete_prop = true;
  } else if (prop_name == "_Z_" ) {
    prop = get_coordinates( grid, 2 );
    delete_prop = true;
  } else {
    prop = grid->property( prop_name );
  }


  if( !prop ) {
    *GsTLAppli_Python_cerr::instance() << "Grid \"" << object 
                << "\" does not have a property "
                << "called \"" << prop_name << "\"" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  redgrid = dynamic_cast<Reduced_grid *>(grid_ni.raw_ptr());

  // useless: Geostat_grid::size() should be used instead
  if (!redgrid) 
	trueSize = prop->size();
  else 
	trueSize = redgrid->trueSize();

  PyObject *list = PyList_New(trueSize);
  
  //TL modified (special handling of grid w/ inactive cells)
  if (!redgrid) {
	  for ( int i = 0; i < prop->size(); i++) {
		  float val = -9e30;
		  if( prop->is_informed( i ) )
			  val = prop->get_value( i );
		  PyObject* item = Py_BuildValue("f", val);
		  if (!item) {
			  Py_DECREF(list);
			  list = NULL;
			  break;
		  }
		  PyList_SetItem(list, i, item);
	  }
  }
  else {
	  for (int i = 0; i < trueSize; ++i) {
		  float val = -9e30;
		  if (redgrid->isActive(i))
			  val = prop->get_value(redgrid->full2reduced(i));
		  PyObject* item = Py_BuildValue("f", val);
		  if (!item) {
			  Py_DECREF(list);
			  list = NULL;
			  break;
		  }
		  PyList_SetItem(list, i, item);
	  }
  }

  if( delete_prop ) delete prop;
  return list;  
}



static PyObject* sgems_set_property( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;
  PyObject* tuple;
  Reduced_grid * redgrid;
  int trueSize;

  if( !PyArg_ParseTuple(args, "ssO", &obj_str, &prop_str, &tuple) )
    return NULL;

  if( !PyList_Check( tuple ) ) return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );
  
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  redgrid = dynamic_cast<Reduced_grid *>(grid);


  GsTLGridProperty* prop = grid->property( prop_name );
  if( !prop ) {
    prop = grid->add_property( prop_name );
  }

  if (redgrid)
	  trueSize = redgrid->trueSize();
  else
	  trueSize = prop->size();

  int size = std::min( trueSize, PyList_Size( tuple ) );

  for( int i=0 ; i < size ; i++ ) {
	float val;

	if (!redgrid) {
		PyArg_Parse( PyList_GET_ITEM( tuple, i ), "f", &val );
		prop->set_value( val, i );
	}
	else {
		if (redgrid->isActive(i)){
			PyArg_Parse(PyList_GET_ITEM(tuple, i),"f", &val);
			prop->set_value(val, redgrid->full2reduced(i));
		}
	}
  }

  Python_project_wrapper::set_project_modified();

  Py_INCREF(Py_None);
  return Py_None;
}

//TL  modified
static PyObject* sgems_get_dims( PyObject *self, PyObject *args)
{
	RGrid * grid;
	char * obj_str;

	if( !PyArg_ParseTuple(args, "s", &obj_str) )
		return NULL;

	std::string object(obj_str);

	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + object );
	grid = dynamic_cast<RGrid*>( grid_ni.raw_ptr() );
	if( !grid ) {
		*GsTLAppli_Python_cerr::instance() << object << " is not a valid regular grid" << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("[]");
	}

    PyObject *list = PyList_New(3);
	PyList_SetItem(list, 0, PyInt_FromLong(grid->nx()));
	PyList_SetItem(list, 1, PyInt_FromLong(grid->ny()));
	PyList_SetItem(list, 2, PyInt_FromLong(grid->nz()));

	return list;
}

static PyMethodDef SGemsMethods[] = {
    {"execute", sgems_execute, METH_VARARGS,
     "Return the number of arguments received by the process."},
    {"get_property", sgems_get_property, METH_VARARGS,
     "Return a vector."},
    {"set_property", sgems_set_property, METH_VARARGS,
     "Change a property of a grid."},
	{"get_dims", sgems_get_dims, METH_VARARGS, "Get dims of a regular grid"},
    {NULL, NULL, 0, NULL}
};






//==============================================================

static PyObject *sgems_cout(PyObject *self, PyObject *args) {
  char *s_line;
  if (!PyArg_ParseTuple(args, "s:sgemslog", &s_line))
    return NULL;
  
  Py_BEGIN_ALLOW_THREADS
    *GsTLAppli_Python_cout::instance() << s_line << gstlIO::end;
  Py_END_ALLOW_THREADS
    
  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *sgems_cerr(PyObject *self, PyObject *args) {
  char *s_line;
  if (!PyArg_ParseTuple(args, "s:sgemslog", &s_line))
    return NULL;
  
  Py_BEGIN_ALLOW_THREADS
    *GsTLAppli_Python_cerr::instance() << s_line << gstlIO::end;
  Py_END_ALLOW_THREADS
    
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef RedirectMethods[] = {
 {"sgems_cout", (PyCFunction)sgems_cout, METH_VARARGS,
  "sgems_cout(line) writes a message to GsTLAppli_Python_cout"},
 {"sgems_cerr", (PyCFunction)sgems_cerr, METH_VARARGS,
  "sgems_cerr(line) writes a message to GsTLAppli_Python_cerr"},
 {NULL, NULL, 0, NULL}
};



#endif 
