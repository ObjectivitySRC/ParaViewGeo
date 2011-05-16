/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkTopOfCell.h $
  Author:    Arolde VIDJINNAGNI& Nehme BILAL
  MIRARCO, Laurentian University
  Date:      May  22 2008
  Version:   0.1

=========================================================================*/

// .NAME vtkTopOfCell - gives a top
// .SECTION Description
// vtkTopOfCell is a source object that gives the top point of cell

#ifndef __vtkTopOfCell_h
#define __vtkTopOfCell_h

#include "vtkPolyDataAlgorithm.h"

class VTK_EXPORT vtkTopOfCell : public vtkPolyDataAlgorithm 
{
public:
  static vtkTopOfCell *New();
  vtkTypeRevisionMacro(vtkTopOfCell,vtkPolyDataAlgorithm);//?
  void PrintSelf(ostream& os, vtkIndent indent);//?

  vtkSetMacro(TopOfCellType, int); //SetTopOfCellType(TopOfCellType)
  
protected:
  vtkTopOfCell();
  ~vtkTopOfCell() {};
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

   int TopOfCellType;
private:
	vtkCellArray* Cells;
	vtkCellArray* Surfaces;
	vtkCellArray* StripSurfaces;
	vtkCellArray* Lines;
	vtkCellArray* Points;
	vtkPointData* PointData;
	vtkCellData* CellData;
	vtkDataArray* PointCoords;
	double**  TopPoints;
	int numberOfCells;
	

	vtkTopOfCell(const vtkTopOfCell&);  // Not implemented.
  void operator=(const vtkTopOfCell&);  // Not implemented.
};

#endif
