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

#include <GsTLAppli/actions/algorithms_actions.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>

#include <fstream>
#include <iostream>


Run_geostat_algo::Run_geostat_algo()
  : proj_( 0 ), algo_( 0 ) {
}


bool Run_geostat_algo::init( std::string& parameters, GsTL_project* proj,
                             Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;

  /* The parameter can have 1 or 3 elements:
   * a parameter file in XML format
   *    OR
   * 1 - the algorithm name
   * 2 - the name of the manager from which to get a parameter handler - ie a device 
   *     that knows how to read the algorithm parameters string -
   * 3 - the algorithm parameters
   */ 

  std::string handler_manager = xmlGeostatParamUtils_manager;
  std::string algo_parameters;
  std::string algo_name;

  std::vector<std::string> params = 
    String_Op::decompose_string( parameters, Actions::separator, Actions::unique );

  if( params.size() == 1 ) {
    std::ifstream infile( params[0].c_str() );
    if( !infile ) {
      GsTLcerr << "Can't open file " << params[0] << "\n" << gstlIO::end;
      return false;
    }

    std::ostringstream file_content;
    char ch;
    while( file_content && infile.get( ch ) )
      file_content.put( ch );

    algo_parameters = file_content.str();
  }
  else {
    if( params.size() != 3 ) {
      GsTLcerr << "Wrong number of parameters supplied to "
	       << "the RunGeostatAlgo command\n"
	       << gstlIO::end;
      return false;
    }

    algo_name = params[0];
    handler_manager = params[1];
    algo_parameters = params[2];
  }

  //-------------
  // Get a Parameter handler (we don't ask the manager to manage the new object)
  // and initialize it with the algorithm parameters
  SmartPtr<Named_interface> ni_handler =
    Root::instance()->new_interface( "Parameters_handler://" + algo_parameters, 
				      handler_manager + "/" );

  if( ni_handler.raw_ptr() == 0 ) {
    appli_warning( "Manager " << handler_manager 
		  << " can't create interface for Parameters_handler" );
     return false;
  }
  algo_param_ = SmartPtr<Parameters_handler>( 
      dynamic_cast<Parameters_handler*>( ni_handler.raw_ptr() )
    ); 

  algo_name = algo_param_->value( "algorithm.name" );


  //-------------
  // Get a Geostat_algo from the algo manager. 

  // don't ask the manager to manage it (which amounts to telling the manager
  // to delete the object once we're done with it)
  SmartPtr<Named_interface> ni =
    Root::instance()->new_interface( algo_name,
				     geostatAlgo_manager + "/" );
  algo_ = SmartPtr<Geostat_algo>( dynamic_cast<Geostat_algo*>( ni.raw_ptr() ) );
  appli_assert( algo_.raw_ptr() );



  return true;
}



bool Run_geostat_algo::exec() {
//  Error_messages_handler_xml error_mesgs;
  bool initialized = algo_->initialize( algo_param_.raw_ptr(), errors_ );
  
  if( !initialized ) {
//    error_mesgs.output();
    return false;
  }
  
  algo_->execute( proj_ );
  proj_->update();

  return true;
}


Named_interface* Run_geostat_algo::create_new_interface(std::string&) {
  return new Run_geostat_algo();
}
