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

#ifndef __GSTLAPPLI_UTILS_GSTL_MESSAGES_H__
#define __GSTLAPPLI_UTILS_GSTL_MESSAGES_H__


#include <GsTLAppli/utils/common.h>
#include <GsTLAppli/utils/gstl_messages_private.h> 


#if defined(__GNUC__) || (defined(_MSC_VER) && _MSC_VER >= 1310)   // 1310 == VC++ 7.1
  //This works with g++ > 2.96:
  extern "C" UTILS_DECL Out_channel& GsTLcout; 
  extern "C" UTILS_DECL Error_channel& GsTLcerr; 
  extern "C" UTILS_DECL Log_channel& GsTLlog; 
#else
  extern "C" UTILS_DECL Out_channel& GsTLcout = *GsTLAppli_output::instance();
  extern "C" UTILS_DECL Error_channel& GsTLcerr = *GsTLAppli_error::instance();
  extern "C" UTILS_DECL Log_channel& GsTLlog = *GsTLAppli_logging::instance();
#endif


#endif
