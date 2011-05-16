/*
   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*=========================================================================
MIRARCO MINING INNOVATION
Author: Nehme Bilal (nehmebilal@gmail.com)
===========================================================================*/

#ifndef __vtkCleanCells_h
#define __vtkCleanCells_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"


class VTK_EXPORT vtkCleanCells : public vtkPolyDataAlgorithm
{
public:
  static vtkCleanCells *New();
  vtkTypeRevisionMacro(vtkCleanCells,vtkPolyDataAlgorithm);
	//vtkPoints getInPoints();
	vtkCellArray *Cells;
	vtkCellArray *outCells;
	vtkPoints *inPoints;
	vtkPoints *outPoints;
// Description:
  // By default ToleranceIsAbsolute is false and Tolerance is
  // a fraction of Bounding box diagonal, if true, AbsoluteTolerance is
  // used when adding points to locator (merging)
  vtkSetMacro(ToleranceIsAbsolute,int);
  vtkGetMacro(ToleranceIsAbsolute,int);

  // Description:
  // Specify tolerance in terms of fraction of bounding box length.
  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);

  // Description:
  // Specify tolerance in absolute terms
  vtkSetMacro(AbsoluteTolerance,double);
  vtkGetMacro(AbsoluteTolerance,double);

  // Description:
  // Turn on/off conversion of degenerate lines to points
  vtkSetMacro(ConvertLinesToPoints,int);
  vtkGetMacro(ConvertLinesToPoints,int);

  // Description:
  // Turn on/off conversion of degenerate polys to lines
  vtkSetMacro(ConvertPolysToLines,int);
  vtkGetMacro(ConvertPolysToLines,int);

  // Description:
  // Turn on/off conversion of degenerate strips to polys
  vtkSetMacro(ConvertStripsToPolys,int);
  vtkGetMacro(ConvertStripsToPolys,int);

  // Description:
  // Set/Get a boolean value that controls whether point merging is
  // performed. If on, a locator will be used, and points laying within
  // the appropriate tolerance may be merged. If off, points are never
  // merged. By default, merging is on.
  vtkSetMacro(PointMerging,int);
  vtkGetMacro(PointMerging,int);
 
  // This filter is difficult to stream.
  // To get invariant results, the whole input must be processed at once.
  // This flag allows the user to select whether strict piece invariance
  // is required.  By default it is on.  When off, the filter can stream,
  // but results may change.
  vtkSetMacro(PieceInvariant, int);
  vtkGetMacro(PieceInvariant, int);
  

protected:

  int    PointMerging;
  double Tolerance;
  double AbsoluteTolerance;
  int ConvertLinesToPoints;
  int ConvertPolysToLines;
  int ConvertStripsToPolys;
  int ToleranceIsAbsolute;  
  int PieceInvariant;


  vtkCleanCells();
  ~vtkCleanCells();
	void CleanCellGroup(vtkCellArray* groupInput, vtkCellArray* groupOutput, vtkCellData* inputCD, vtkCellData* outputCD);
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
  vtkCleanCells(const vtkCleanCells&);  // Not implemented.
  void operator=(const vtkCleanCells&);  // Not implemented.



};

#endif
