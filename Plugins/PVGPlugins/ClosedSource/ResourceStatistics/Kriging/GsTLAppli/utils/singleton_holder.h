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

#ifndef __GSTLAPPLI_UTILS_SINGLETON_HOLDER_H__ 
#define __GSTLAPPLI_UTILS_SINGLETON_HOLDER_H__ 

#include <GsTLAppli/utils/common.h>
 


/** Singleton_holder is a template class to define Singletons
* (see Design Pattern, by Gamma et al). Simply, a singleton
* is a globally accessible object which has a unique instance.
*/
template <class Singleton_> 
class Singleton_holder{ 
public: 
 
  static Singleton_* instance(); 
 
 
private: 
  static Singleton_* instance_; 
 
  Singleton_holder(); 
  Singleton_holder(const Singleton_holder&); 
  Singleton_holder& operator=(const Singleton_holder&); 
 
}; 
 
 
template <class Singleton_> 
Singleton_* Singleton_holder<Singleton_>::instance_ = 0; 
 
 
template<class Singleton_> 
Singleton_* Singleton_holder<Singleton_>::instance() { 
  if(instance_ == 0) { 
    instance_ = new Singleton_; 
  } 
  return instance_; 
} 
 
 
//==================================== 
//   Specialization for managers:  
 
/* In the case of singleton manager, add a factory method for 
 * creating new managers. 
 */ 
// Does not link (multiple definition ...), so commented out 
/* 
template<> 
Manager* Singleton_holder<Manager>::instance() { 
  if(instance_ == 0) { 
    instance_ = new Manager; 
  } 
  return instance_; 
} 
*/ 
 
 
#endif 
