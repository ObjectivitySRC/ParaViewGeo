/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkElliptic.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkElliptic_h
#define __vtkElliptic_h

#include "vtkPolyDataAlgorithm.h"

#include "vtkCell.h" // Needed for VTK_CELL_SIZE

class VTK_EXPORT vtkElliptic : public vtkPolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkElliptic,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct with default resolution 100, x =1, y=1, z = .5
  static vtkElliptic *New();
  
   
  // Description:
  // Set the center of the ellipsoid
  vtkSetVector3Macro(Translate,double);
  vtkGetVectorMacro(Translate,double,3);
  
  vtkSetVector3Macro(Rotate,double);
  vtkGetVectorMacro(Rotate,double,3);
  
  vtkSetVector2Macro(Scale,double);
  vtkGetVectorMacro(Scale,double,2);
  	
  vtkSetMacro(RadialResolution,double);
  vtkGetMacro(RadialResolution,double);

	vtkSetMacro(CircumferentialResolution,double);
  vtkGetMacro(CircumferentialResolution,double);

  virtual void SetMatrix(double Matrix[16]);


protected:
  vtkElliptic();
  ~vtkElliptic() {}

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

  double Translate[3];
  double Rotate[3];
  double Scale[2];
  
  double RadialResolution;  
	double CircumferentialResolution;  

  //matrix
  double Matrix[16];
  bool MatrixFlag;

private:
  vtkElliptic(const vtkElliptic&);  // Not implemented.
  void operator=(const vtkElliptic&);  // Not implemented.
};

#endif


