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

#ifndef __GSTLAPPLI_UTILS_NAMED_INTERFACE_H__ 
#define __GSTLAPPLI_UTILS_NAMED_INTERFACE_H__ 
 
#include <GsTLAppli/utils/common.h>
#include <GsTL/utils/smartptr.h> 
 
#include <string>

typedef long RefCount; 
 
 
/** A Named_interface is an object that can be instanciated and 
* managed by a Manager. Named_interfaces are reference counted,
* hence can be used with smart pointers (SmartPtr). Remember that
* when using smart pointers, memory must be allocated with \c new.
* Example: \code SmartPtr<Named_interface> ni = new Cartesian_grid; \endcode 
*/
class UTILS_DECL Named_interface { 
  public: 
    Named_interface();

    const Named_interface* new_ref() const;
    void delete_ref() const;
    void unref_no_delete() const;
 
    RefCount references() const { return references_; } 
 
  protected: 
    virtual ~Named_interface() {} 
 
  private: 
    RefCount references_; 
 
    virtual void on_zero_references();
}; 
 
 
 
#endif 
