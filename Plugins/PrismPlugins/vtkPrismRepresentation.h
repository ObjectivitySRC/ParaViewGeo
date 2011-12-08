/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPrismRepresentation
// .SECTION Description
// vtkPrismRepresentation extends vtkGeometryRepresentationWithFaces to add
// support auto scaling for Prism filters.

#ifndef __vtkPrismRepresentation_h
#define __vtkPrismRepresentation_h

#include "vtkGeometryRepresentationWithFaces.h"

class VTK_EXPORT vtkPrismRepresentation : public vtkGeometryRepresentationWithFaces
{
public:
  static vtkPrismRepresentation* New();
  vtkTypeMacro(vtkPrismRepresentation, vtkGeometryRepresentationWithFaces);
  void PrintSelf(ostream& os, vtkIndent indent);

//BTX
protected:
  vtkPrismRepresentation();
  ~vtkPrismRepresentation();

  virtual bool GenerateMetaData(vtkInformation*, vtkInformation* outInfo);
private:
  vtkPrismRepresentation(const vtkPrismRepresentation&); // Not implemented
  void operator=(const vtkPrismRepresentation&); // Not implemented
//ETX
};

#endif
