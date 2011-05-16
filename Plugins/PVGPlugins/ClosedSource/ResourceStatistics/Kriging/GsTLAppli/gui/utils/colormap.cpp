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

#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>

Colormap::Colormap(Color_scale* colors ) :
  colors_( colors ) {
  max_ = -99999;
  min_ = -99999;
  step_ = 0;
}

Colormap::Colormap( Color_scale* colors,
		    float min, float max ) :
  colors_( colors ) {
  set_bounds( min , max );
//  step_ = (max_ - min_) / double( colors_->colors_count() - 2 );
}


Colormap::Colormap(const Colormap& rhs ) 
  : max_( rhs.max_ ), min_( rhs.min_ ),
    step_( rhs.step_ ),
    colors_( rhs.colors_ ) {
}

Colormap& Colormap::operator = ( const Colormap& rhs ) {
  if( &rhs != this )
  max_ = rhs.max_;
  min_ = rhs.min_;
  step_ = rhs.step_;
  colors_ = rhs.colors_;
 
  return *this;
}

bool Colormap::operator == ( const Colormap& rhs ) {
  return max_==rhs.max_ && min_ == rhs.min_ &&
    step_ == rhs.step_ && colors_ == rhs.colors_;
}

void Colormap::color_scale( Color_scale* colors ) {
  colors_ = SmartPtr<Color_scale>( colors );
  step_ = (max_ - min_) / double( colors_->colors_count() - 2 );
}



/** Compute the color corresponding to "value"
 *  Parameters: 
 *    value - value whose color is to be looked up
 *    r, g, b - component colors for the value in question
 */
void Colormap::color( float value, 
		      float& r, float& g, float& b ) const{
  
  int nb_colors = colors_->colors_count();

  if( value <= min_ ) {
    colors_->color( 0, r, g, b );
    return;
  }
  if( value >= max_ ) {
    colors_->color( nb_colors-1, r, g, b );
    return;
  }

  int color_class = static_cast<int>( (value-min_)/step_ ) + 1;
  
  appli_assert( color_class > 0 && color_class < nb_colors-1 );

  colors_->color( color_class, r, g, b );

}




RGB_color Colormap::color( float value ) const {
  RGB_color c;
  color( value, c.red(), c.green(), c.blue() );
  return c;
}



//--------------------------------------

Oinv_colormap::Oinv_colormap( Color_scale* colors ) 
  : Colormap( colors ) {

}
