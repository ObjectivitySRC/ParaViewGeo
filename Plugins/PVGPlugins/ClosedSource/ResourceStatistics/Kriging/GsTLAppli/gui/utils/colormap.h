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

#ifndef __GSTLAPPLI_GUI_COLOR_MAP_H__
 
#define __GSTLAPPLI_GUI_COLOR_MAP_H__
 

 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/colorscale.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <GsTL/utils/smartptr.h>
 
#include <Inventor/SbColor.h>
 
#include <string>
#include <vector>
 

 

 
/** A colormap converts a value into a color.
 * Colors are RGB with each R,G,B component represented by a floating point 
 * value between 0 and 1. To get back to the usual integer RGB notation, just
 * multiply each component by 255.
 */
 
class GUI_DECL Colormap {
 

 
 public:
 
  Colormap( Color_scale* colors = 0 );
  Colormap( Color_scale* colors, float min, float max );
  Colormap( const Colormap& rhs );
 
  virtual ~Colormap() {};
 
  Colormap& operator = ( const Colormap& rhs );
  bool operator == ( const Colormap& rhs );
 
  void color_scale( Color_scale* colors );
  Color_scale* color_scale() const { return colors_.raw_ptr(); }
 
  void set_bounds( float min, float max );
  void upper_bound( float max );
  void lower_bound( float min );
  float upper_bound() const;
  float lower_bound() const;
 
  RGB_color color( float value ) const ;
 
  void color( float value, float& r, float& g, float& b) const;
  
 protected:
  // double min_ / max_:  range of values expected
  float max_, min_;
  float step_;
  // lookup table of available colors 
 
  SmartPtr<Color_scale> colors_;
 
};
 

 

 

 
/** This colormap adds the ability to compute a SbColor, directly usable by 
 
 * Open Inventor.
 
 */
 
class GUI_DECL Oinv_colormap : public Colormap {
 
 public:
 
  Oinv_colormap( Color_scale* colors );
 
  virtual ~Oinv_colormap() {}
 
  inline SbColor sbcolor( float value );
 
};
 

 

 

 
//---------------------------------------
 
//  Colormap - inline functions
 
//------
 

 
inline void Colormap::set_bounds( float min, float max ) {
 
  appli_assert( min <= max );
 
  min_ = min;
 
  max_ = max;
 
  
 
  // we keep 2 colors for out-of-bound values
 
  step_ = (max_ - min_) / double( colors_->colors_count() - 2 );
 
}
 
inline void Colormap::upper_bound( float max ) {
 
  appli_assert( min_ <= max );
 
  max_ = max;
 
  // we keep 2 colors for out-of-bound values
 
  float nb_classes = float( colors_->colors_count() ) - 2;
 
  step_ = (max_ - min_) / nb_classes;
 
}
 
inline void Colormap::lower_bound( float min ) {
 
  appli_assert( min <= max_ );
 
  min_ = min;
 
  // we keep 2 colors for out-of-bound values
 
  step_ = (max_ - min_) / double( colors_->colors_count() - 2 );
 
}
 

 
inline float Colormap::upper_bound() const {
 
  return max_;
 
}
 
inline float Colormap::lower_bound() const {
 
  return min_;
 
}
 

 

 

 

 

 
//---------------------------------------
 
//  Oinv_colormap - inline functions
 
//------
 

 
inline SbColor Oinv_colormap::sbcolor( float value ) {
 
  float r,g,b;
 
  Colormap::color( value, r,g,b );
 
  return SbColor( r,g,b );
 
}
 

 

 
#endif
 
