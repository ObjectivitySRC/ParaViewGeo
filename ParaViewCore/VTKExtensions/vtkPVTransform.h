/*=========================================================================

  Program:   ParaView
  Module:    vtkPVTransform

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPVTransform - baseclass for all ParaView vtkTransform class.
// .SECTION Description
// vtkPVTransform extend vtkTransform in the sens that it extend the vtkTransform
// API with absolute position/rotation/scale change and not incremental one like
// the vtkTransform does.

#ifndef __vtkPVTransform_h
#define __vtkPVTransform_h

#include "vtkTransform.h"
class vtkTransform;

class VTK_EXPORT vtkPVTransform : public vtkTransform
{
public:
  static vtkPVTransform* New();
  vtkTypeMacro(vtkPVTransform, vtkTransform);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set Position of the transform.
  void SetAbsolutePosition(double xyz[3]);
  void SetAbsolutePosition(double x, double y, double z);
  vtkGetVector3Macro(AbsolutePosition, double);

  // Description:
  // Get/Set Rotation for the transform.
  void SetAbsoluteRotation(double xyz[3]);
  void SetAbsoluteRotation(double x, double y, double z);
  vtkGetVector3Macro(AbsoluteRotation, double);

  // Description:
  // Get/Set Scale for the transform.
  void SetAbsoluteScale(double xyz[3]);
  void SetAbsoluteScale(double x, double y, double z);
  vtkGetVector3Macro(AbsoluteScale, double);

//BTX
protected:
  vtkPVTransform();
  ~vtkPVTransform();

  void UpdateMatrix();

  double AbsolutePosition[3];
  double AbsoluteRotation[3];
  double AbsoluteScale[3];
  vtkTransform *AbsoluteTransform;

private:
  vtkPVTransform(const vtkPVTransform&); // Not implemented
  void operator=(const vtkPVTransform&); // Not implemented

//ETX
};

#endif
