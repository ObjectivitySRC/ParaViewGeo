/**********************************************************************
** Author: Jianbing Wu
**
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Stanford Geostatistical 
** Earth Modeling Software (SGEMS)
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


#ifndef __TI_manipulation_h__
#define __TI_manipulation_h__

#include <GsTL/matrix_library/tnt_lib.h>
#include <GsTL/matrix_library/tnt/cmat.h>

#include <vector>

#include <GsTLAppli/geostat/filtersim_std/is_categorical.h>

using namespace std;
using namespace TNT;

typedef Matrix<float> Matrix_2D;

/*
 * Core class to perform rotation
 */
class GEOSTAT_DECL Rotation
{
public:
    Rotation( vector<float> source_center, vector<float> target_center, vector<float> angle  );
    ~Rotation(){}

    void operator()(float si, float sj, float sk, int& ti, int& tj, int& tk);

protected:
    Matrix_2D rot_;
    vector<float> source_center_;
    vector<float> target_center_;
};


/*
 * Core class to perform scaling
 */
class GEOSTAT_DECL Scaling
{
public:
    Scaling(vector<float> source_center, vector<float> target_center, vector<float> factor );
    ~Scaling(){}

    void operator()(float si, float sj, float sk, int& ti, int& tj, int& tk);

private:
    vector<float> source_center_;
    vector<float> target_center_;
    vector<float> factor_;
};


/*
 * Core class to perform rotation followed by scaling
 */
class GEOSTAT_DECL SRotation: public Rotation
{
public:
    SRotation( vector<float> source_center, vector<float> target_center, vector<float> angle, vector<float> factor  ):
      Rotation( source_center, target_center, angle ), factor_(factor) {}
    ~SRotation(){}

    void operator()(float si, float sj, float sk, int& ti, int& tj, int& tk);

private:
    vector<float> factor_;
};


/*
 * Core class to perform scaling followed by rotation
 */
class GEOSTAT_DECL RScaling: public Rotation
{
public:
    RScaling( vector<float> source_center, vector<float> target_center, vector<float> angle, vector<float> factor  ):
      Rotation( source_center, target_center, angle ), factor_(factor) {}
    ~RScaling(){}

    void operator()(float si, float sj, float sk, int& ti, int& tj, int& tk);

private:
    vector<float> factor_;
};


#endif  // __TI_manipulation_h__
