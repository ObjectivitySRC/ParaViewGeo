/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkWriter.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkWriter - write vtk polygonal data
// .SECTION Description
// vtkWriter is a source object that writes ASCII or binary 
// polygonal data files in vtk format. See text for format details.
// .SECTION Caveats
// Binary files written on one system may not be readable on other systems.

#ifndef __vtkSGeMSWriter_h
#define __vtkSGeMSWriter_h

#include "vtkDataWriter.h"
#include "vtkImageData.h"

class vtkPolyData;
class vtkPointData;
class vtkStringArray;
class vtkStdString;
class vtkCellData;

class VTK_EXPORT vtkSGeMSWriter : public vtkDataWriter
{
public:
	static vtkSGeMSWriter *New();
	vtkTypeRevisionMacro(vtkSGeMSWriter,vtkDataWriter);
	void PrintSelf(ostream& os, vtkIndent indent);

protected:
	vtkSGeMSWriter();
	~vtkSGeMSWriter();

	vtkStdString FindFileName();

	void WriteData();

	virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
	vtkSGeMSWriter(const vtkSGeMSWriter&);  // Not implemented.
	void operator=(const vtkSGeMSWriter&);  // Not implemented.

	void WriteInt(FILE* fp, int number);
	void WriteFloat(FILE* fp, float number);
	void WritePointSet(vtkPolyData* inputPS);
	void WriteCgrid(vtkImageData* inputCG);
	void WriteVector(vtkDataArray* currentPropArray, int numComponents, int numUnits, FILE* fp);
	void WriteVectorApproximation(vtkDataArray* currentPropArray, int numComponents, int numUnits, FILE* fp);
	//void WriteApproxColor(vtkDataArray* currentPropArray, int numCells, FILE* fp);

//	void WriteVectorProp(vtkDataArray* currentPropArray, int currentProp, int numComponents, FILE* fp);

	vtkCellArray* Cells;
	vtkPointData* PointData;
	vtkCellData* CellData;
	vtkDataArray* PointCoords;
	vtkPolyData *inputPS;
	vtkImageData* inputCG;

	int numPoints;
	int numCells;
	int numProps;

	float cell_xdim;
	float cell_ydim;
	float cell_zdim;

	int grid_xdim;
	int grid_ydim;
	int grid_zdim;
};

#endif


