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

#include <GsTL/math/math_functions.h>
#include <cmath>
#include "TI_manipulation.h"

// --- class Rotation ---
Rotation::Rotation(vector<float> source_center, vector<float> target_center, vector<float> angle )
{
    source_center_ = source_center;
    target_center_ = target_center;
    rot_ = Matrix_2D(3, 3);

    // the operator (i,j) is 1-based indexing
    rot_(1,1) = 1.0;
    rot_(2,2) = 1.0;
    rot_(3,3) = 1.0;

    // first, rotate about Z axis
    if ( !GsTL::equals( angle[2], float(0.) ) )
    {
        Matrix_2D Rz(3,3);
        Rz(1,1) = std::cos( angle[2] );
        Rz(1,2) = std::sin( angle[2] );
        Rz(2,1) = -Rz(1,2);
        Rz(2,2) = Rz(1,1);
        Rz(3,3) = 1.;

        rot_ = matmult(Rz, rot_);
    }

    // then, rotate about X axis
    if ( !GsTL::equals( angle[0], float(0.) ) )
    {
        Matrix_2D Rx(3,3);
        Rx(2,2) = std::cos( angle[0] );
        Rx(2,3) = std::sin( angle[0] );
        Rx(3,2) = -Rx(2,3);
        Rx(3,3) = Rx(2,2);
        Rx(1,1) = 1.;

        rot_ = matmult(Rx, rot_);
    }

    // finally, rotate about Y axis
    if ( !GsTL::equals( angle[1], float(0.) ) )
    {
        Matrix_2D Ry(3,3);
        Ry(1,1) = std::cos( angle[1] );
        Ry(3,1) = std::sin( angle[1] );
        Ry(1,3) = -Ry(3,1);
        Ry(3,3) = Ry(1,1);
        Ry(2,2) = 1.;

        rot_ = matmult(Ry, rot_);
    }
}


void Rotation::operator()(float si, float sj, float sk, int& ti, int& tj, int& tk)
{
    si -= source_center_[0];
    sj -= source_center_[1];
    sk -= source_center_[2];

    ti = round( rot_(1,1)*si + rot_(1,2)*sj + rot_(1,3)*sk + target_center_[0] );
    tj = round( rot_(2,1)*si + rot_(2,2)*sj + rot_(2,3)*sk + target_center_[1] );
    tk = round( rot_(3,1)*si + rot_(3,2)*sj + rot_(3,3)*sk + target_center_[2] );
}


// --- class Scaling ---
Scaling::Scaling(vector<float> source_center, vector<float> target_center, vector<float> factor )
{
    source_center_ = source_center;
    target_center_ = target_center;
    factor_ = factor;
}


void Scaling::operator()(float si, float sj, float sk, int& ti, int& tj, int& tk)
{
    ti = round( (si-source_center_[0])*factor_[0] + target_center_[0] );
    tj = round( (sj-source_center_[1])*factor_[1] + target_center_[1] );
    tk = round( (sk-source_center_[2])*factor_[2] + target_center_[2] );
}


// --- class SRotation ---
void SRotation::operator()(float si, float sj, float sk, int& ti, int& tj, int& tk)
{
    si -= source_center_[0];
    sj -= source_center_[1];
    sk -= source_center_[2];

    si *= factor_[0];
    sj *= factor_[1];
    sk *= factor_[2];

    ti = round( rot_(1,1)*si + rot_(1,2)*sj + rot_(1,3)*sk + target_center_[0] );
    tj = round( rot_(2,1)*si + rot_(2,2)*sj + rot_(2,3)*sk + target_center_[1] );
    tk = round( rot_(3,1)*si + rot_(3,2)*sj + rot_(3,3)*sk + target_center_[2] );
}


// --- class RScaling ---
void RScaling::operator()(float si, float sj, float sk, int& ti, int& tj, int& tk)
{
    si -= source_center_[0];
    sj -= source_center_[1];
    sk -= source_center_[2];

    ti = round( ( rot_(1,1)*si + rot_(1,2)*sj + rot_(1,3)*sk  )*factor_[0]  + target_center_[0]);
    tj = round( ( rot_(2,1)*si + rot_(2,2)*sj + rot_(2,3)*sk  )*factor_[1]  + target_center_[1]);
    tk = round( ( rot_(3,1)*si + rot_(3,2)*sj + rot_(3,3)*sk  )*factor_[2]  + target_center_[2]);
}
