/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "math" module of the Geostatistical Earth
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

#include <GsTLAppli/math/random_numbers.h>
 
Rand48_generator* Global_random_number_generator::instance_ = 0;

Rand48_generator* Global_random_number_generator::instance() {
  if(instance_ == 0) {
    instance_ = new Rand48_generator;
  }
  return instance_;
}

 
long int Random_number_generator::calls_ = 0;

Random_number_generator::return_type 
Random_number_generator::operator()() { 
  calls_++;
  return Global_random_number_generator::instance()->operator()(); 
} 
 


long int STL_generator::calls_ = 0;
 
STL_generator::return_type 
STL_generator::operator()( argument_type N ) { 
  calls_++;
  double p = Global_random_number_generator::instance()->operator()(); 
  return static_cast<argument_type>( p * double(N) ); 
} 
 
