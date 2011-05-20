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


#include "vtkCoordinateSystemMapper.h"

#include "vtkObjectFactory.h"
#include <vtkMath.h>


vtkCxxRevisionMacro(vtkCoordinateSystemMapper, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkCoordinateSystemMapper);


//----------------------------------------------------------------
vtkCoordinateSystemMapper::vtkCoordinateSystemMapper()
{
	this->TA_B = vtkMatrix4x4::New();
	this->TB_A = vtkMatrix4x4::New();
}


//----------------------------------------------------------------
vtkCoordinateSystemMapper::~vtkCoordinateSystemMapper()
{
	this->TA_B->Delete();
	this->TB_A->Delete();
}

//----------------------------------------------------------------
void vtkCoordinateSystemMapper::setCoordSystemA(
										const double c[3],
										const double x[3], 
										const double y[3], 
										const double z[3])
{
	for (int i=0; i<3; i++)
	{
		this->cA[i] = c[i];
		this->xA[i] = x[i];
		this->yA[i] = y[i];
		this->zA[i] = z[i];
	}
	//this->computeMappingMatrices();
}


//----------------------------------------------------------------
void vtkCoordinateSystemMapper::setCoordSystemB(
								const double c[3],
								const double x[3], 
								const double y[3], 
								const double z[3])
{
	for (int i=0; i<3; i++)
	{
		this->cB[i] = c[i];
		this->xB[i] = x[i];
		this->yB[i] = y[i];
		this->zB[i] = z[i];
	}
	//this->computeMappingMatrices();
}

//----------------------------------------------------------------
void vtkCoordinateSystemMapper::computeMappingMatrices()
{
	double nx, ny, nz, ox, oy, oz, ax, ay, az, px, py, pz;

	// n, o, a are the x,y and z components of the 
	// system B in A coordinate system
	nx = vtkMath::Dot(xB, xA);
	ny = vtkMath::Dot(xB, yA);
	nz = vtkMath::Dot(xB, zA);

	ox = vtkMath::Dot(yB, xA);
	oy = vtkMath::Dot(yB, yA);
	oz = vtkMath::Dot(yB, zA);

	ax = vtkMath::Dot(zB, xA);
	ay = vtkMath::Dot(zB, yA);
	az = vtkMath::Dot(zB, zA);

	double cB_A[3];
	cB_A[0] = cB[0] - cA[0];
	cB_A[1] = cB[1] - cA[1];
	cB_A[2] = cB[2] - cA[2];

	px = vtkMath::Dot(cB_A, xA);
	py = vtkMath::Dot(cB_A, yA);
	pz = vtkMath::Dot(cB_A, zA);

	TB_A->SetElement(0,0, nx);
	TB_A->SetElement(0,1, ox);
	TB_A->SetElement(0,2, ax);
	TB_A->SetElement(0,3, px);

	TB_A->SetElement(1,0, ny);
	TB_A->SetElement(1,1, oy);
	TB_A->SetElement(1,2, ay);
	TB_A->SetElement(1,3, py);

	TB_A->SetElement(2,0, nz);
	TB_A->SetElement(2,1, oz);
	TB_A->SetElement(2,2, az);
	TB_A->SetElement(2,3, pz);

	TB_A->SetElement(3,0, 0.0);
	TB_A->SetElement(3,1, 0.0);
	TB_A->SetElement(3,2, 0.0);
	TB_A->SetElement(3,3, 1.0);


	// n, o, a are the x,y and z components of the 
	// system A in B coordinate system
	nx = vtkMath::Dot(xA, xB);
	ny = vtkMath::Dot(xA, yB);
	nz = vtkMath::Dot(xA, zB);

	ox = vtkMath::Dot(yA, xB);
	oy = vtkMath::Dot(yA, yB);
	oz = vtkMath::Dot(yA, zB);

	ax = vtkMath::Dot(zA, xB);
	ay = vtkMath::Dot(zA, yB);
	az = vtkMath::Dot(zA, zB);

	double cA_B[3];
	cA_B[0] = cA[0] - cB[0];
	cA_B[1] = cA[1] - cB[1];
	cA_B[2] = cA[2] - cB[2];

	px = vtkMath::Dot(cA_B, xB);
	py = vtkMath::Dot(cA_B, yB);
	pz = vtkMath::Dot(cA_B, zB);

	TA_B->SetElement(0,0, nx);
	TA_B->SetElement(0,1, ox);
	TA_B->SetElement(0,2, ax);
	TA_B->SetElement(0,3, px);

	TA_B->SetElement(1,0, ny);
	TA_B->SetElement(1,1, oy);
	TA_B->SetElement(1,2, ay);
	TA_B->SetElement(1,3, py);

	TA_B->SetElement(2,0, nz);
	TA_B->SetElement(2,1, oz);
	TA_B->SetElement(2,2, az);
	TA_B->SetElement(2,3, pz);

	TA_B->SetElement(3,0, 0.0);
	TA_B->SetElement(3,1, 0.0);
	TA_B->SetElement(3,2, 0.0);
	TA_B->SetElement(3,3, 1.0);
}