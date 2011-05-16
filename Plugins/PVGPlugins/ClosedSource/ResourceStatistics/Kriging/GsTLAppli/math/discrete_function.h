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

#ifndef __GSTLAPPLI_DISCRETE_FUNCTION_H__
#define __GSTLAPPLI_DISCRETE_FUNCTION_H__

#include <GsTLAppli/math/common.h>

#include <vector>
#include <utility>


class Interpolator_1d;


class MATH_DECL Discrete_function{
public:
   
  Discrete_function();    
  Discrete_function( const std::vector<double>& x_values,
            		     const std::vector<double>& y_values,
                     Interpolator_1d *interpol);
  Discrete_function(const std::vector<double> &x_values);

  double no_data_value() const { return NaN; }
  void set_no_data_value( double noval ) { NaN = noval ; }

  std::vector<double> x_values() const;
  /// The x values must be sorted in increasing order
  void set_x_values( const std::vector<double>& values );

  std::vector<double> y_values()const;
  void set_y_values( const std::vector<double>& values );
  
  void set_values( const std::vector< std::pair<double,double> >& values );

  double operator()(double x) const;

  const Interpolator_1d * interpolator() const;
  void interpolator(const Interpolator_1d *interpol);

  /// removes any data pair that contains a no data value
  int make_valid();


private:
  std::vector< std::pair<double,double> > values_;
  const Interpolator_1d *interpol_;
  bool is_valid_;
    
  double NaN;
};




inline void Discrete_function::interpolator(const Interpolator_1d *interpol)
         {interpol_=interpol;}

inline  const Interpolator_1d * Discrete_function:: interpolator() const
         {return interpol_;}



#endif


    
