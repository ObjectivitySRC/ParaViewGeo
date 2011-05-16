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

#ifndef __GSTLAPPLI_LINEAR_INTERPOLATOR_1D_H__
#define __GSTLAPPLI_LINEAR_INTERPOLATOR_1D_H__

#include <GsTLAppli/math/common.h>

#include <vector>
class Interpolator_1d;

class MATH_DECL Linear_interpolator_1d : public Interpolator_1d
{
    virtual double interpolate(std::pair<double,double>p1,
			       std::pair<double,double>p2,double x)const;
    //return first or last y value
    virtual double extrapolate(std::pair<double,double>p1,double x)const; 
};
#endif
