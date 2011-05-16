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

#include <GsTLAppli/math/Interpolator_1d.h>
#include <GsTLAppli/math/Linear_interpolator_1d.h>




double Linear_interpolator_1d:: interpolate(std::pair<double,double>p1,
		   std::pair<double,double>p2,double x)const
{
    double m;
    double c;
    if((p2.first-p1.first)!=0)
    {
	m=(p2.second-p1.second)/(p2.first-p1.first);

	//use the formula y= m*x + c
	 c=p1.second-p1.first*m;
	 return (m*x +c);
    }

    //infinite slope
    else
    {
	//Ask Nicholas about this
	return p1.second;
	
    }

    

    
}

    
double Linear_interpolator_1d:: extrapolate(std::pair<double,double>p1,double x)const
{
    return p1.second;
}

