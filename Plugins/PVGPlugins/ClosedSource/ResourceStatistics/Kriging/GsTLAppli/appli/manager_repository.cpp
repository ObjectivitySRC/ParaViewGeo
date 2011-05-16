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

#include <GsTLAppli/appli/manager_repository.h>

/* Implementation note:
 * This class is different from Singleton_holder in that it uses
 * a different "constructor". When instance_ is first constructed,
 * a factory method for creating managers is registered.
 * It could have been possible to use a specialization of template
 * class Singleton_holder for the case of class Manager. 
 */

Manager* Root::instance_ = 0;


Manager* Root::instance() {
  if(instance_ == 0) {
    instance_ = new Manager;
    instance_->factory("directory", create_directory);
  }
  return instance_;
}

