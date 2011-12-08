/*=========================================================================

  Program:   ParaView
  Module:    vtkBalancedRedistributePolyData.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*----------------------------------------------------------------------------
 Copyright (c) Los Alamos National Laboratory
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.
----------------------------------------------------------------------------*/

// .NAME vtkBalancedRedistributePolyData - do balance of cells on processors

#ifndef __vtkBalancedRedistributePolyData_h
#define __vtkBalancedRedistributePolyData_h

#include "vtkWeightedRedistributePolyData.h"
class vtkMultiProcessController;


//*******************************************************************

class VTK_EXPORT vtkBalancedRedistributePolyData : public vtkWeightedRedistributePolyData
{
public:
  vtkTypeMacro(vtkBalancedRedistributePolyData, vtkWeightedRedistributePolyData);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  static vtkBalancedRedistributePolyData *New();

protected:
  vtkBalancedRedistributePolyData();
  ~vtkBalancedRedistributePolyData();
  void MakeSchedule (vtkCommSched*);

private:
  vtkBalancedRedistributePolyData(const vtkBalancedRedistributePolyData&); // Not implemented
  void operator=(const vtkBalancedRedistributePolyData&); // Not implemented
};

//****************************************************************

#endif


