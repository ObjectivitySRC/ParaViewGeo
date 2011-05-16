/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_GUI_COLOR_H__ 
#define __GSTLAPPLI_GUI_COLOR_H__ 

#include <GsTLAppli/gui/common.h>
 
 
class GUI_DECL RGB_color { 
 public: 
  RGB_color( float r=0, float g=0, float b=0 ); 
  float& red()               { return r_; } 
  const float& red() const   { return r_; } 
  float& green()             { return g_; } 
  const float& green() const { return g_; } 
  float& blue()              { return b_; } 
  const float& blue() const  { return b_; } 
  void rgb( float r, float g, float b ); 
   
 
 private: 
  float r_, g_, b_; 
}; 
 
#endif 
