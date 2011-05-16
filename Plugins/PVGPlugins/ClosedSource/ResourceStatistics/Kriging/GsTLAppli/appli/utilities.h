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

#ifndef __GSTLAPPLI_APPLI_UTILITIES_H__
#define __GSTLAPPLI_APPLI_UTILITIES_H__

#include <GsTLAppli/appli/common.h>
#include <GsTLAppli/utils/progress_notifier.h>

#include <GsTL/utils/smartptr.h>

#include <string>


namespace utils {

/** This function creates a new instance of a progress notifier. The progress
* notifier can either be a pop-up dialog, with title \a title, or a plain text
* progress bar. \a total_steps is the total number of steps to execute to 
* complete the task. \a frequency is how often (expressed in number of steps)
* to report progress is made. 
*/
APPLI_DECL SmartPtr<Progress_notifier> 
create_notifier( const std::string& title,
                 int total_steps,
                 int frequency ) ;

} // end of utils namespace


#endif

