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

#include <GsTLAppli/gui/utils/colorscale.h>
#include <fstream>


Named_interface* 
Color_scale::create_new_interface( std::string& filename ) {
  return new Color_scale( filename );
}


Color_scale::Color_scale( const std::string& filename ) {
  std::ifstream infile( filename.c_str() );
  if( !infile ) {
    appli_warning( "Can not open file " << filename );
    return;
  }

  std::vector<float> tmp;

  float v;
  while( infile >> v ) { 
    tmp.push_back( v );
  }
  
  
  if( (tmp.size() % 3) != 0 ) {
    appli_warning( "some r,g,b components missing" );
  }

  int nb_colors = tmp.size() / 3;
  if( nb_colors > 256 ) tmp.resize( 256*3 );

  int base_replic = 256 / nb_colors;
  std::vector<int> replic(256, base_replic );
  int remainder =  256 - base_replic*nb_colors;
  for( int i=0; i < remainder ; i++ ) {
    replic[i]++;
  }

  for( int j=0; j< nb_colors ; j++ ) {
    float r = tmp[3*j];
    float g = tmp[3*j+1];
    float b = tmp[3*j+2];

    for( int k=0; k < replic[j] ; k++ ) {
      colors_.push_back( RGB_color(r,g,b) );
    }

  }


  /*
  float r,g,b;
  while( infile >> r ) {
    infile >> g >> b ;
    colors_.push_back( RGB_color(r,g,b) );
  }
*/
}
