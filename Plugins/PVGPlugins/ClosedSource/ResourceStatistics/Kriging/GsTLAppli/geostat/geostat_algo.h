/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_GEOSTAT_ALGO_H__ 
#define __GSTLAPPLI_GEOSTAT_ALGO_H__ 
 
 
#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/utils/named_interface.h> 
 
class Error_messages_handler; 
class Parameters_handler; 
class GsTL_project; 
 
/** 
 * This is the interface common to all geostat algorithms. 
 * It is a Named_interface, so it can be created by a Manager. 
 */ 
 
class GEOSTAT_DECL Geostat_algo : public Named_interface { 
  public: 
    virtual ~Geostat_algo() {}; 
 
    /** Initializes the parameters of the algorithm 
     * @return false if errors were encountered. If error_mesg 
     * is not 0, the encountered errors are placed into error_mesg. 
     * @param error_mesg is a string containing the name of 
     * the parameters that contained errors. 
     * If the string is empty, the initialization 
     * was successful. The string has the following format: 
     * field1://explanation1;;field2://explanation;; 
     */  
    virtual bool initialize( const Parameters_handler* parameters, 
			     Error_messages_handler* errors ) = 0; 
 
    /** Runs the algorithm.  
     * @return 0 if the run was successful 
     */ 
    virtual int execute( GsTL_project* proj=0 ) = 0; 
 
    /** Tells the name of the algorithm 
     */ 
    virtual std::string name() const { return ""; } 
     
}; 
 
 
 
 
#endif 
