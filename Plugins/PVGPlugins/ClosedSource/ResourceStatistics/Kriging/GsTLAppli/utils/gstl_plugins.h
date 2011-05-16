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

#ifndef __GSTLAPPLI_UTILS_PLUGINS_H__ 
#define __GSTLAPPLI_UTILS_PLUGINS_H__ 
 
#include <GsTLAppli/utils/gstl_messages.h> 
#include <GsTLAppli/appli/manager_repository.h> 




/** To define a new geostatistics algorithm plugin:
* Derive a new class from Geostat_algo, we will call it MyPlugin.
* Redefine the \c initialize and \c execute functions
* Implement a static member function called create_new_interface:
*   static Named_interface* create_new_interface( std::string& )
* That function creates and returns a new instance of MyPlugin,
* e.g. 
* \code
* static Named_interface* create_new_interface( std::string& ) {
*   return new MyPlugin;
* }
* \endcode
* At the end of the class definition (e.g. in the cpp file), add the line:
* GEOSTAT_PLUGIN(MyPlugin)
* Finally, it is very important to call the plugin file the same as
* the class, e.g. MyPlugin.dll (or libMyPlugin.so under Unix).
*/




#define BIND_GEOSTAT_ALGO(CLASSNAME) \
{\
  GsTLlog << "\n\n registering algo " << #CLASSNAME << "\n"; \
  SmartPtr<Named_interface> ni = \
    Root::instance()->interface( geostatAlgo_manager ); \
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() ); \
  if( !dir ) { \
    GsTLlog << "Directory " << geostatAlgo_manager << " does not exist \n"; \
    return 1; \
  } \
  CLASSNAME toto; \
  dir->factory( toto.name(), CLASSNAME##::create_new_interface ); \
} \


#if defined(_WIN32) || defined(WIN32)
#define GEOSTAT_PLUGIN(CLASSNAME)\
extern "C" __declspec(dllexport) int plugin_init() { \
  BIND_GEOSTAT_ALGO(CLASSNAME)\
  return 0; \
}\

#else
#define GEOSTAT_PLUGIN(CLASSNAME)\
extern "C" int plugin_init() { \
  BIND_GEOSTAT_ALGO(CLASSNAME)\
  return 0; \
}\

#endif






//================================================

#define BIND_INPUT_FILTER(CLASSNAME) \
{\
  GsTLlog << "\n\n registering filter #CLASSNAME" << "\n"; \
  SmartPtr<Named_interface> ni = \
    Root::instance()->interface( topLevelInputFilters_manager ); \
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() ); \
  if( !dir ) { \
    GsTLlog << "Directory " << topLevelInputFilters_manager << " does not exist \n"; \
    return 1; \
  } \
  CLASSNAME toto; \
  dir->factory( toto.name(), CLASSNAME##::create_new_interface ); \
} \


#if defined(_WIN32) || defined(WIN32)
#define INPUT_FILTER_PLUGIN(CLASSNAME)\
extern "C" __declspec(dllexport) int plugin_init() { \
  BIND_INPUT_FILTER(CLASSNAME)\
  return 0; \
}\

#else
#define INPUT_FILTER_PLUGIN(CLASSNAME)\
extern "C" int plugin_init() { \
  BIND_INPUT_FILTER(CLASSNAME)\
  return 0; \
}\

#endif

#endif

