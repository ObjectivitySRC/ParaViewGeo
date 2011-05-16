/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "appli" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_TREE_FACTORY_H__ 
#define __GSTLAPPLI_TREE_FACTORY_H__ 
 
#include <GsTLAppli/appli/common.h>
#include <GsTLAppli/utils/manager.h> 
#include <GsTLAppli/utils/singleton_holder.h> 
 
 
/** Root is a manager singleton (see Design Patterns by Gamma et al. for a 
 * precise definition of singleton). It is a globally accessible Manager
 * that manages all other managers in S-GeMS.
 * Using Root, all managers are globally accessible. For example, to
 * access a Manager called TopLevel which is itself managed by Manager 
 * InpuFilter, itself managed by Root, use:
 * \code
 * SmartPrt<Named_interface> ni = 
 *   Root::instance()->interface( "/InputFilter/TopLevel" );
 * Manager* mng = dynamic_cast<Manager*>( ni.raw_ptr() );
 * \endcode
 * \c ni is a smart pointer to Manager TopLevel. Root returns a Named_interface
 * which is then casted into a required type, i.e. a Manager.
 * If TopLevel can create interfaces of type "gslib_filter", the following code
 * creates a new instance of type "gslib_filter" called "example":
 * \code
 * SmartPtr<Named_interface> ni =
 *   Root::instance()->new_interface( "gslib_filter", 
 *                                    "/InputFilter/TopLevel/example" );
 * \endcode
 * \c ni can then be casted into a \c gslib_filter. 
 */ 
class APPLI_DECL Root { 
   
 public: 
   
  static Manager* instance(); 
 
 
 private: 
  static Manager* instance_; 
 
  Root(); 
  Root(const Root&); 
  Root& operator=(const Root&); 
   
}; 
 
 
 
/* Aliases for manager names. Having all the names centralized  
 * makes maintanance easier. 
 */ 
 
// namespace MngRep { 
 
static const std::string actions_manager = "/Actions"; 
 
static const std::string infilters_manager = "/InputFilters"; 
static const std::string outfilters_manager = "/OutputFilters"; 
static const std::string topLevelInputFilters_manager = "/InputFilters/TopLevel"; 
static const std::string gslibInputFilters_manager = "/InputFilters/Gslib"; 
 
static const std::string gridObject_manager = "/GridObject"; 
static const std::string gridModels_manager = "/GridObject/Model"; 
static const std::string oinvDescription_manager = "/GridObject/OinvDescription"; 
static const std::string QtSummaryDescription_manager = "/GridObject/QtSummaryDescription";

static const std::string geostatAlgo_manager = "/GeostatAlgo"; 
static const std::string qtWrapper_manager = "/QTWrapper"; 
 
static const std::string projectViews_manager = "/ProjectView"; 
static const std::string projects_manager = "/Project"; 
 
static const std::string geostatParamUtils_manager = "/GeostatParamUtils"; 
static const std::string xmlGeostatParamUtils_manager =  
  "/GeostatParamUtils/XML"; 
 
static const std::string colormap_manager = "/Colormaps"; 
 
// } 
 
#endif 
