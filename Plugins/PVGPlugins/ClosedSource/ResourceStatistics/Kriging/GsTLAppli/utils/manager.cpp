/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "utils" module of the Geostatistical Earth
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

  #include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qlibrary.h>
#include <qdir.h>
#include <qstring.h>

#include <cstdlib>
//#include <dlfcn.h>




/** \class Manager
* A Manager is responsible for creating new \c Named_interfaces and managing
* them, i.e. being able to retrieve a \c Named_interface or delete it.
* A Manager is a Named_interface itself, hence Managers can manage other
* Managers. 
* By default, a Manager can not create any Named_interface. To teach the 
* Manager how to create a specific type of Named_interface, use function 
* \c factory. New interfaces can then be created with function \c new_interface.
* To retrieve an existing interface, use \c interface.
* When an interface is deleted by \c delete_interface, it is not managed by
* the Manager anymore. If that interface is not used anywhere else, it is
* automatically \c deleted (i.e. the memory is freed).
* When a Manager is asked to create a Named_interface that it doesn't know
* of, it searches for a way to create the interface into plug-ins. Plug-ins  
* must be placed in a pre-defined directory. Each manager can define a 
* sub-directory of a path common to all managers where plug-ins are searched.
* The main path is set by static function \c set_main_plugin_path, the sub-
* directory is set in the constructor of the manager.
*
* Manager defines iterators to iterate through:
* - the types the Manager can create (\c type_iterator)
* - the interfaces currently managed by the Manager (\c interface_iterator)
* - the names of the interfaces currently managed by the Manager 
*   (\c names_iterator)
* Functions list_all outputs a list of all managed interfaces.
*/





Named_interface* create_directory(std::string& dir) {
  return new Manager(dir);
}


// TL modified
// build a tree structure on top of sgems commands to support auto finish
std::string Manager::finish(std::string & s)
{
	int begin,j,k;
	CallBackMap::const_iterator it_f;
	std::string temp;
	bool found = false;

	if (s.size() == 0) return "";

	if (_buf.size() == 0)
		for( it_f = factory_.begin(); it_f != factory_.end(); ++it_f ) 
			_buf.push_back(it_f->first);

	for (begin = 0; begin < _buf.size(); ++begin) {
		QString temp = _buf[begin].c_str();
    if (temp.startsWith(s.c_str(),Qt::CaseInsensitive))
			break;
	}

	if (begin == _buf.size() || _buf[begin] == s)  
		return s;
 
	j = s.size();
	while (1) {
		temp = _buf[begin].substr(0, j);
		int count = 0;
		if ((begin+1) >= _buf.size()) // begin is last element in list
			return s;
		for (k = begin+1; k < _buf.size(); ++k) {
			std::string kk = _buf[k];
			if (_buf[k].compare(0,j,temp) == 0){ // equal
				++count;
				found = true;
			}
			break;
		}
		if (count == 0) {
			if (found)
				return temp.substr(0,j-1);
			return s;
		}
		++j;
		if (j >= _buf[begin].size()) {
			return temp;
		}
	}

	return s;  // should never reach here
}



typedef std::pair<std::string, std::string> Prefixed_name;


/** Ex: /usr/bin -> "/usr" and "/bin"
 *      /bin -> "bin" and ""
 */
static Prefixed_name split_name(const std::string& name) {
  Prefixed_name s;

  if( name.empty() ) {
    s.first="";
    s.second="";
    return s;
  }

  std::string::size_type i = name.find('/', 1);
  if( ( name[0] == '/' ) && ( i != std::string::npos ) ) {
    s.first = name.substr(0, i);
    s.second = name.substr(i);
  }
  else {
    s.first=name.substr(1);
    s.second="";
  }
  return s;
}



std::string Manager::init_plugin_path() { 
  char* env = getenv("GSTLAPPLIHOME"); 
  if( env ) { 
    //std::string base_path( env ); 
    //return base_path + "/plugins/";
    QString envpath( env );
    QDir homedir( envpath );
    homedir.cd( "plugins" );
    return homedir.absolutePath().toAscii();
  } 
  else {
    QDir current_dir;
    //std::string path( current_dir.absPath().latin1() );
    //path += "/plugins/";
    //return ( path + "/plugins/" );
    current_dir.cd( "plugins" );
    return current_dir.absolutePath().toAscii();
  }
} 
 


SmartPtr<Directory> Manager::get_subdir(const std::string& name) {
  SmartPtr<Named_interface> sptr = interface(name);
  if( sptr.raw_ptr() == 0)
    appli_warning( "no directory " << name << std::endl );
  Directory* dir = dynamic_cast<Directory*>( sptr.raw_ptr() );  
  
  return dir;
}




bool Manager::load_library( const std::string& type ) {

  // If no plugin sub-directory is defined, no need to look for
  // a plugin.
  if( plugin_subdir_.empty() ) return false ;


  typedef int (*InitFnPtr)( );

  std::string libname = plugin_dir_ + plugin_subdir_ + 
                        "/lib" + type + dll_extension_;
  
  /*
  void * lib = dlopen( libname.c_str(), RTLD_LAZY );
  if ( !lib ){
    appli_warning( "Dynamic link failure for " << libname << ": "
		  << dlerror() << std::endl );
    return false;
  }

  std::string init_function_name( libname + "init" );
  InitFnPtr init_lib = (InitFnPtr) dlsym( lib, init_function_name.c_str() );
  if ( !init_lib ){
    appli_warning( "Can't find function " << init_function_name 
    << "in library " << libname << std::endl );
    return false;
  }
  */
 
  std::string init_function_name( libname + "init" );
  QLibrary myplugin( libname.c_str() );
  //myplugin.setAutoUnload( false );
  InitFnPtr init_lib = (InitFnPtr) myplugin.resolve( init_function_name.c_str() );
  if ( !init_lib ) {
    if( myplugin.isLoaded() ) {
      appli_warning( "Unable to load " << libname );
    }
    else {
      appli_warning( "Can't find function " << init_function_name << "in library " << libname );
    }
    return false;
  }

  init_lib();

  return true;
}


//=========================================
//   Static members

std::string Manager::plugin_dir_( Manager::init_plugin_path() ); 

 
#if defined(WIN32)
  const std::string Manager::dll_extension_( ".dll" );
#else
  const std::string Manager::dll_extension_( ".so" );
#endif

void Manager::set_main_plugin_path( const std::string& path ) {
  plugin_dir_ = path;

#if defined(__GNUC__) && __GNUC__ < 3
  if( plugin_dir_.compare( "/", plugin_dir_.size() - 1, 1 ) )
    plugin_dir_ += "/";
#else
  if( plugin_dir_.compare( plugin_dir_.size() - 1, 1, "/" ) )
    plugin_dir_ += "/";
#endif
}


//=========================================
//   Public functions

Manager::Manager() 
  : plugin_subdir_("") {
}

Manager::Manager( std::string& plugin_subdir ) 
  : plugin_subdir_(plugin_subdir) {
}



Manager::~Manager() {
  // delete all the managed interfaces
  if( interfaces_.empty() ) return;
/*
  InterfaceMap::iterator it = interfaces_.begin();
  for( ; it != interfaces_.end() ; ++it ) {
	  std::cerr << "deleting interface " << std::string("/") + it->first << std::endl;
	  delete_interface( std::string("/") + it->first );
  }
  */

  while( !interfaces_.empty() ) {
	InterfaceMap::iterator it = interfaces_.begin();
	delete_interface( "/" + it->first );
  }
}




SmartPtr<Named_interface>
Manager::new_interface(const std::string& type_param,
                       const std::string& name, std::string* final_name ) {

  // Look for the required manager and create the new interface.
  // If no factory method can be found to create the interface, look
  // for a plugin that would provide a factory method.

  // It would be better to use the functionalities in namespace String_Op
  // rather than the split function defined above... I'll change that later
  Prefixed_name split = split_name(name);

  if( split.second == "" ) {
    // we're at the end of the path

    //----------
    // Extract the type of the object to be created 
    // and the parameters for the creation. These information are
    // contained in string "type_param"
    String_Op::string_pair split_params = 
      String_Op::split_string( type_param, "://", true );
    std::string type = split_params.first;
    std::string param = split_params.second;
    
    CallBackMap::iterator it = factory_.find(type);
    
    if( it == factory_.end() ) {
      // No factory method found => try loading a plugin
      bool loaded = load_library( type );
      if( loaded ) 
	      it = factory_.find(type);
    }

    if( it == factory_.end() ) {
      // No factory method could be found, and no plugin provides a
      // suitable one
      appli_warning( "No factory method to create type " << type << std::endl
		    <<  "Interface ..." << name << " could not be created" << std::endl );
      return 0;
    }

    // If we've reached this point, it means we have a factory method
    // to create the requested interface. 
    std::pair<InterfaceMap::iterator, bool> pos;
    SmartPtr<Named_interface> interf = (it->second)( param );
    
    // if the interface has no name, don't manage it
    if( !split.first.empty() ) {
      pos = interfaces_.insert( std::make_pair( split.first, interf) );
      if( pos.second == false ) {
        // If the name is already used, modify it if final_name is not null
        if( !final_name ) {
          appli_warning( "Interface " << split.first << " already exists" );
          return 0;
        }
        else {
          int count = 1;
          std::string initial_name = split.first;
          do {
            final_name->erase();
            std::ostringstream ostr;
            ostr << initial_name << "_" << count++;
            final_name->append( ostr.str() );
          } while ( !interfaces_.insert( 
                       std::make_pair( *final_name, interf) 
                     ).second );
        }
      }
    }
    return interf;
    
    
  }
  
  else {
    // we're not at the end of the path.
    // go down one level of the tree, if it exists, and try again
    SmartPtr<Directory> subdir = get_subdir( split.first );
    if( subdir.raw_ptr() == 0 ) {
       appli_warning( "Could not descend into directory " << split.first << std::endl );
      return 0;
    }
    else
      return subdir->new_interface( type_param, split.second, final_name );
  }
}
 


bool 
Manager::delete_interface(const std::string& name) {
  Prefixed_name split = split_name(name);
  if( split.second == "" ) {
    // if we're at the end of the path
    int n = interfaces_.erase(split.first);
    if( n == 0 ) {
      appli_warning( split.first << " does not exist" << std::endl );
      return false;
    }
    return true;
  }
  else {
    // go down one level of the tree, if it exists
    SmartPtr<Directory> subdir = get_subdir( split.first );
    if( subdir.raw_ptr() == 0 )
      return false;
    else
      return subdir->delete_interface( split.second );
  }

}
 
   
SmartPtr<Named_interface>
Manager::interface(const std::string& name) {
  Prefixed_name split = split_name(name);

  if( split.second == "" ) {
    // if we're at the end of the path
    InterfaceMap::iterator it = interfaces_.find(split.first);

    if( it != interfaces_.end() ) {
      // if we found an object called "name"
      return it->second;
    }
    else
      return 0;
  }
  else {
    // go down one level of the tree, if it exists
    SmartPtr<Directory> subdir = get_subdir( split.first );
    if( subdir.raw_ptr() == 0 )
      return 0;
    else
      return subdir->interface( split.second );
  }
}



std::string 
Manager::name( const Named_interface* interface ) {

  InterfaceMap::const_iterator it = interfaces_.begin();

  for( ; it != interfaces_.end() ; ++it ) {
    if( it->second.raw_ptr() == interface )
      return it->first;
  }

  return "";
}



bool Manager::factory(const std::string& type, const CallBackType& callback) {
	return (factory_.insert( std::make_pair( type, callback ) ) ).second;

  /*
 Prefixed_name split = split_name(type);

  if( split.second == "" ) {
    // if we're at the end of the path
    std::cerr << "creating factory for " << split.first << std::endl;
    return ( factory_.insert( std::make_pair( split.first, callback ) ) ).second;
  }
  else {
    // go down one level of the tree, if it exists
    SmartPtr<Directory> subdir = get_subdir( split.first.substr(1) );
    Manager* sub_manager = dynamic_cast<Manager*>( subdir.raw_ptr() );
    if( sub_manager == 0 ) {
      std::cerr << "subdirectory " << split.first.substr(1) << " not found" << std::endl;
      return false;
    }
    else
      return sub_manager->factory( split.second, callback );
  }
  */

}


SmartPtr<Directory> Manager::directory() const {
  return 0;
}

void Manager::directory(Directory*) {
  
}
    


void Manager::list_all(std::ostream& os, int level) const {

  for( CallBackMap::const_iterator it_f = factory_.begin();
       it_f != factory_.end(); 
       ++it_f ) {
    for(int i=0; i<level ; i++)  os << "   ";

    os << "|- (" << it_f->first << ")" <<  std::endl;
  }

  for(InterfaceMap::const_iterator it = interfaces_.begin();
      it != interfaces_.end() ; 
      ++it) {
    //    os << "|" ;
    for(int i=0; i<level ; i++)  os << "   ";

    os << "|- " << it->first << std::endl;
    
    //recursively display the content of the sub-directories
    Manager* subdir = dynamic_cast<Manager*>( it->second.raw_ptr() );
    if( subdir != 0 ) {
      subdir->list_all(os, level+1);
    }
  }
}



void Manager::list_all(Channel& os, int level) const {

  for( CallBackMap::const_iterator it_f = factory_.begin();
       it_f != factory_.end(); 
       ++it_f ) {
    for(int i=0; i<level ; i++)
	os << "   ";
    os << "|- (" << it_f->first << ")\n" ;
  }

  for(InterfaceMap::const_iterator it = interfaces_.begin();
      it != interfaces_.end() ; 
      ++it) {
    //    os << "|" ;
    for(int i=0; i<level ; i++)
      os << "   ";
    os << "|- " << it->first << "\n";
    
    //recursively display the content of the sub-directories
    Manager* subdir = dynamic_cast<Manager*>( it->second.raw_ptr() );
    if( subdir != 0 ) {
      subdir->list_all(os, level+1);
    }
  }
}


void Manager::list_factory_methods( Channel& os ) const {

  for( CallBackMap::const_iterator it_f = factory_.begin();
       it_f != factory_.end(); 
       ++it_f ) {
    os << it_f->first << "\n" ;
  }
  os << gstlIO::end;
}
