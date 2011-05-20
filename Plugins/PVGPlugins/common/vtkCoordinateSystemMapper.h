/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkCoordinateSystemMapper.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


//===================================================================================//
//				Mirarco Mining Innovation
// Author:   Nehme Bilal
// Date:     october 2009
// contact: nehmebilal@gmail.com 
//====================================================================================//

#include <vtkMatrix4x4.h>
#include "vtkObject.h"


#ifndef _VTK_COORDINATE_SYSTEM_MAPPER
#define _VTK_COORDINATE_SYSTEM_MAPPER


class vtkCoordinateSystemMapper : public vtkObject
{

public:
  static vtkCoordinateSystemMapper *New();
  vtkTypeRevisionMacro(vtkCoordinateSystemMapper,vtkObject);
	
//BTX
	// the coordinates systems axes must be normalized
	// before passing it
	// you must call computeMappingMatrices() after this
	// to update the transformation matrices.
	void setCoordSystemA(
								const double c[3],
								const double x[3], 
								const double y[3], 
								const double z[3]);


	// the coordinates systems axes must be normalized
	// before passing it
	// you must call computeMappingMatrices() after this
	// to update the transformation matrices.
	void setCoordSystemB(
								const double c[3],
								const double x[3], 
								const double y[3], 
								const double z[3]);

	// the input point in must contain the coordinates 
	// of the input point in the coordinate system A.
	// the output point out will contain the new coordinates
	// in the coordinate system B
	inline void mapPointA_B(const double in[3], double out[3]);


	// the input point in must contain the coordinates 
	// of the input point in the coordinate system B.
	// the output point out will contain the new coordinates
	// in the coordinate system A
	inline void mapPointB_A(const double in[3], double out[3]);


	void computeMappingMatrices();

protected:
	vtkCoordinateSystemMapper();
	~vtkCoordinateSystemMapper();

private:
	


	double cA[3];
	double xA[3];
	double yA[3];
	double zA[3];

	double cB[3];
	double xB[3];
	double yB[3];
	double zB[3];

	double p[4]; // used for calculations

	vtkMatrix4x4 *TA_B;
	vtkMatrix4x4 *TB_A;

//ETX
};



//----------------------------------------------------------------
inline void vtkCoordinateSystemMapper::mapPointA_B(const double in[3], double out[3])
	{
		p[0] = in[0];
		p[1] = in[1];
		p[2] = in[2];
		p[3] = 1;
		this->TA_B->MultiplyPoint(p, p);
		out[0] = p[0];
		out[1] = p[1];
		out[2] = p[2];
	}
//----------------------------------------------------------------
inline void vtkCoordinateSystemMapper::mapPointB_A(const double in[3], double out[3])
	{
		p[0] = in[0];
		p[1] = in[1];
		p[2] = in[2];
		p[3] = 1;
		this->TB_A->MultiplyPoint(p, p);
		out[0] = p[0];
		out[1] = p[1];
		out[2] = p[2];
	}

#endif