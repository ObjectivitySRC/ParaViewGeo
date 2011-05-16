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

#include <GsTLAppli/math/discrete_function.h>
#include <GsTLAppli/math/Interpolator_1d.h>

#include <algorithm>



Discrete_function::Discrete_function() {
  interpol_ = 0;
  is_valid_=false;
  NaN = -9.9e36;
}
    
Discrete_function::Discrete_function( const std::vector<double>& x_values,
        		                          const std::vector<double>& y_values,
		                                  Interpolator_1d *interpol)
  : interpol_( interpol ), is_valid_( true ) {
  unsigned int min = std::min( x_values.size(), y_values.size() );
  for( unsigned int i= 0 ; i < min ; ++i ) {
    values_.push_back( std::make_pair( x_values[i], y_values[i] ) );
  }
  NaN = -9.9e36;
}
    
Discrete_function::Discrete_function(const std::vector<double> &x_values) {
  interpol_ = 0;
  for( unsigned int i= 0 ; i < x_values.size() ; ++i ) {
    values_.push_back( std::make_pair( x_values[i], 0 ) );
  }
  is_valid_ = false;
  NaN = -9.9e36;
}


std::vector<double> Discrete_function::x_values() const {
  std::vector<double> x_values;
  for( unsigned int i = 0; i < values_.size() ; i++ )
    x_values.push_back( values_[i].first );
  return x_values;
}

void Discrete_function::set_x_values( const std::vector<double>& values ) {
  unsigned int old_size = values_.size();

  values_.resize( values.size() );  
  for( unsigned int i= 0 ; i < values.size() ; ++i ) {
    values_[i].first = values[i];
  }

  if( values_.size() > old_size ) {
    for( int j = old_size; j < values_.size() ; j++ )
      values_[j].second = NaN;
  }
}



std::vector<double> Discrete_function::y_values()const {
  std::vector<double> y_values;
  for( unsigned int i = 0; i < values_.size() ; i++ )
    y_values.push_back( values_[i].second );

  return y_values;
}


void Discrete_function::set_y_values( const std::vector<double>& values ) {
  unsigned int old_size = values_.size();

  values_.resize( values.size() );  
  for( unsigned int i= 0 ; i < values.size() ; ++i ) {
    values_[i].second = values[i];
  }

  if( values_.size() > old_size ) {
    for( int j = old_size; j < values_.size() ; j++ )
      values_[j].first = NaN;
  }
}
  
void Discrete_function::
set_values( const std::vector< std::pair<double,double> >& values ) {
  values_ = values;
}


double Discrete_function:: operator()( double x ) const
{
    if( ! interpol_ ) return NaN;
    
    unsigned int i;
    std::pair<double,double> p1,p2;
    for( i=0 ; i < values_.size() ; i++ )
  	{
	    if ( values_[i].first > x ) break;
	  }
    
    //end points
    if( (i == values_.size()-1) || (i==0) )
    {
      p1 = values_[i];
    	return interpol_->extrapolate(p1,x);
    }
    else
    {
      p1 = values_[i-1];
      p2 = values_[i];
      return interpol_->interpolate(p1,p2,x);
    }
}



struct Functor {
  Functor( double ref ) : ref_(ref) {}
  bool operator() ( std::pair<double,double> pair_val ) {
    return ( pair_val.first == ref_ || pair_val.second == ref_ ); 
  }

  double ref_;
};


int Discrete_function::make_valid() {
  values_.erase( std::remove_if( values_.begin(), values_.end(), Functor( NaN ) ),
                 values_.end() );
  return values_.size();
}
