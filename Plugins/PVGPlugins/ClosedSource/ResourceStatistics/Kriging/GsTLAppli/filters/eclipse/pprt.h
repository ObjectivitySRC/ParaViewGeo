/**********************************************************************
** Author: Ting Li
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

/* -*- c++ -*- */


#ifndef __GSTLAPPLI_PPRT_
#define __GSTLAPPLI_PPRT_

#include <stdio.h>



/* !!! WARNING !!! Assumes python in system search path and readprt.py can
   be found in python's search path */

/* will open a pipe to received output from this python script */
const static char * output = "output";

typedef struct _s {
  int X, Y, Z;
  int num_dates;
  int num_properties;

  char ** dates;
  char ** properties;

  float * data;
  
} Prt;

class pprt
{
public:
  pprt();
  ~pprt();

  Prt * fill();
  
private:

  FILE * _pipe;
  Prt * _package;
};

  
#endif
