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

#ifndef __GSTLAPPLI_GUI_COLOR_SCALE_H__ 
#define __GSTLAPPLI_GUI_COLOR_SCALE_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/colors.h> 
#include <GsTLAppli/utils/named_interface.h> 
#include <GsTLAppli/utils/gstl_messages.h> 
 
#include <vector> 
 
 
class GUI_DECL Color_scale : public Named_interface { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Color_scale( const std::string& filename ); 
  virtual ~Color_scale() {}; 
   
  int colors_count() const; 
 
  RGB_color color( int i ) const; 
  void color( int i, RGB_color& color ) const; 
  void color( int i, float& r, float& g, float& b ) const; 
 
 
 private: 
  std::vector< RGB_color > colors_; 
 
}; 
 
 
inline RGB_color Color_scale::color( int i ) const { 
  appli_assert( i >=0 && i < int( colors_.size() ) ); 
  return colors_[i]; 
} 
 
inline void Color_scale::color( int i, RGB_color& color ) const { 
  appli_assert( i >=0 && i < int( colors_.size() ) ); 
  color = colors_[i]; 
} 
 
inline void  
Color_scale::color( int i, float& r, float& g, float& b ) const { 
  appli_assert( i >=0 && i < int( colors_.size() ) ); 
  r = colors_[i].red(); 
  g = colors_[i].green(); 
  b = colors_[i].blue(); 
} 
 
inline int Color_scale::colors_count() const { 
  return colors_.size(); 
} 
 
#endif 
