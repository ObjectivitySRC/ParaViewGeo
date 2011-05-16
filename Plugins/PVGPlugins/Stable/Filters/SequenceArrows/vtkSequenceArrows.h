/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSequenceArrows.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkSequenceArrows_h
#define __vtkSequenceArrows_h

#include "vtkPolyDataAlgorithm.h"

#include "vtkCell.h" // Needed for VTK_CELL_SIZE

class vtkCellArray;
class vtkIntArray;
class vtkAppendPolyData;

class VTK_EXPORT vtkSequenceArrows : public vtkPolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkSequenceArrows,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct with default resolution 6, height 1.0, radius 0.5, and
  // capping on. The cone is centered at the origin and points down
  // the x-axis.
  static vtkSequenceArrows *New();

  // Description:
  // Set the height of the cone. This is the height along the cone in
  // its specified direction.
  vtkSetClampMacro(Size,double,0.0,VTK_DOUBLE_MAX)
  vtkGetMacro(Size,double);

  // Description:
  // When the component mode is UseSelected, this ivar indicated the selected
  // component. The default value is 0.
  vtkSetClampMacro(SelectedComponent,int,0,VTK_INT_MAX);
  vtkGetMacro(SelectedComponent,int);

protected:
  vtkSequenceArrows();
  ~vtkSequenceArrows();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

  double Size;  
  int    SelectedComponent;
  
  void SortOrder(vtkCellArray* verts, vtkInformationVector **inputVector);


private:
  void CreateMatrix( double rotation[16], double *direction, double *center);
  int AddArrow( vtkAppendPolyData* &append, double* point1, double* point2);
  vtkSequenceArrows(const vtkSequenceArrows&);  // Not implemented.
  void operator=(const vtkSequenceArrows&);  // Not implemented.
  vtkIntArray *pointOrder;
};

#endif


