/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkEllipsoid.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkEllipsoid_h
#define __vtkEllipsoid_h

#include "vtkPolyDataAlgorithm.h"

#include "vtkCell.h" // Needed for VTK_CELL_SIZE

class VTK_EXPORT vtkEllipsoid : public vtkPolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkEllipsoid,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct with default resolution 100, x =1, y=1, z = .5
  static vtkEllipsoid *New();
  
  // Description:
  // Set the center of the ellipsoid
  vtkSetVector3Macro(Translate,double);
  vtkGetVectorMacro(Translate,double,3);
  
  vtkSetVector3Macro(Rotate,double);
  vtkGetVectorMacro(Rotate,double,3);
  
  vtkSetVector3Macro(Scale,double);
  vtkGetVectorMacro(Scale,double,3);

	vtkSetMacro(PhiResolution,double);
  vtkGetMacro(PhiResolution,double);

	vtkSetMacro(ThetaResolution,double);
  vtkGetMacro(ThetaResolution,double);
    
  virtual void SetWAxis(double WAxis[3]);
  virtual void SetWAngle(double WAngle);
  
  virtual void SetMatrix(double Matrix[16]);
 


protected:
  vtkEllipsoid();
  ~vtkEllipsoid() {}

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

  double Translate[3];
  double Rotate[3];
  double Scale[3];
  
  //axis angle rotation
  double WAngle;
  bool WFlags[2];
  
  //matrix
  double Matrix[16];
  bool MatrixFlag;
  
  
  double PhiResolution;  
	double ThetaResolution;  


private:
  vtkEllipsoid(const vtkEllipsoid&);  // Not implemented.
  void operator=(const vtkEllipsoid&);  // Not implemented.
};

#endif


