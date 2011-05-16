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

#include <GsTLAppli/actions/python_wrapper.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/utils/gstl_messages.h>




GsTL_project* Python_project_wrapper::project_ = 0;
bool Python_project_wrapper::project_modified_ = false;



//=================================

Python_cout_channel* GsTLAppli_Python_cout::instance_ = 0;

Python_cout_channel* GsTLAppli_Python_cout::instance() {
  if(instance_ == 0) {
    instance_ = new Python_cout_channel;
  }
  return instance_;
}




Python_cerr_channel* GsTLAppli_Python_cerr::instance_ = 0;

Python_cerr_channel* GsTLAppli_Python_cerr::instance() {
  if(instance_ == 0) {
    instance_ = new Python_cerr_channel;
  }
  return instance_;
}
