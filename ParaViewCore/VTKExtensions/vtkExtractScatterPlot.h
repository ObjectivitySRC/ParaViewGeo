/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractScatterPlot.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkExtractScatterPlot_h
#define __vtkExtractScatterPlot_h

#include "vtkPolyDataAlgorithm.h"

// .NAME vtkExtractScatterPlot - Extract a scatter plot (two-dimensional histogram) from any dataset
// .SECTION Description

// vtkExtractScatterPlot accepts any vtkDataSet as input and produces a
// vtkPolyData containing two-dimensional histogram data as output.  The
// output vtkPolyData will contain two vtkDoubleArray instances named
// "x_bin_extents" and "y_bin_extents", which contain the boundaries
// between bins along each dimension.  It will also contain a
// vtkUnsignedLongArray named "bin_values" which contains the value for
// each bin.

class VTK_EXPORT vtkExtractScatterPlot :
  public vtkPolyDataAlgorithm
{
public:
  static vtkExtractScatterPlot* New();
  vtkTypeMacro(vtkExtractScatterPlot, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Controls which input data component should be binned, for input arrays with more-than-one component
  vtkSetClampMacro(XComponent, int, 0, VTK_LARGE_INTEGER);
  vtkGetMacro(XComponent, int);
  
  // Description:
  // Controls which input data component should be binned, for input arrays
  // with more-than-one component
  vtkSetClampMacro(YComponent, int, 0, VTK_LARGE_INTEGER);
  vtkGetMacro(YComponent, int);
  
  // Description:
  // Controls the number of bins along the X axis in the output histogram data
  vtkSetClampMacro(XBinCount, int, 1, VTK_LARGE_INTEGER);
  vtkGetMacro(XBinCount, int);
  
  // Description:
  // Controls the number of bins along the Y axis in the output histogram data
  vtkSetClampMacro(YBinCount, int, 1, VTK_LARGE_INTEGER);
  vtkGetMacro(YBinCount, int);
  
private:
  vtkExtractScatterPlot();
  vtkExtractScatterPlot(const vtkExtractScatterPlot&); // Not implemented
  void operator=(const vtkExtractScatterPlot&); // Not implemented
  ~vtkExtractScatterPlot();

  virtual int FillInputPortInformation (int port, vtkInformation *info);
  virtual int RequestData(vtkInformation *request, 
                          vtkInformationVector **inputVector, 
                          vtkInformationVector *outputVector);

  int XComponent;
  int YComponent;
  int XBinCount;
  int YBinCount;
};

#endif
