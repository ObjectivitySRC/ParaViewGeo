/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkTransformFilter.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkTransformFilter - 

#ifndef __vtkVectorTransform_h
#define __vtkVectorTransform_h

#include "vtkTransformFilter.h"

class VTK_EXPORT vtkVectorTransform : public vtkTransformFilter
{
public:
  static vtkVectorTransform *New();
  vtkTypeRevisionMacro(vtkVectorTransform,vtkTransformFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetVector3Macro(Point1, float);
  vtkGetVector3Macro(Point1, float);

  vtkSetVector3Macro(Point2, float);
  vtkGetVector3Macro(Point2, float);
protected:
  vtkVectorTransform();
  ~vtkVectorTransform();

  float Point1[3];
  float Point2[3];

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
private:
  vtkVectorTransform(const vtkVectorTransform&);  // Not implemented.
  void operator=(const vtkVectorTransform&);  // Not implemented.
};

#endif
