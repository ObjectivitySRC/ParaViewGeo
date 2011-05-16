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

#ifndef __vtkGocadWriter_h
#define __vtkGocadWriter_h

#include "vtkGocadBase.h"


class vtkPolyData;
class vtkPointData;
class vtkStringArray;
class vtkStdString;
class vtkCellData;

class VTK_EXPORT vtkGocadWriter : public vtkGocadBase
{
public:
	static vtkGocadWriter *New();
	vtkTypeRevisionMacro(vtkGocadWriter,vtkGocadBase);

	// Get the input to this writer.
	vtkPolyData* GetInput();
	vtkPolyData* GetInput(int port);
	
	void SetFileName(const char* filename);

protected:
	vtkGocadWriter();
	~vtkGocadWriter();

  void WriteFile(ostream* fp, vtkPolyData* input); 
  int WriteObject(ostream* fp, vtkPolyData* input);
	void WriteLines(ostream* fp, vtkPolyData* input, char* vrtxLabel, vtkStringArray* propNames);
	void WritePoints(ostream* fp, vtkPolyData* input, char* vrtxLabel, vtkStringArray* propNames);
	void WriteSurfaces(ostream* fp, vtkPolyData* input, char* vrtxLabel, vtkStringArray* propNames);
	void WriteStrip(ostream* fp, vtkPolyData* input, char* vrtxLabel, vtkStringArray* propNames);	
	void WritePointData(ostream* fp, vtkStringArray* propNames, char* vrtxLabel);
	void WriteStripSurfaces(ostream* fp, vtkPolyData* input, char* vrtxLabel,vtkStringArray* propNames);
	
	void WriteData();
	virtual int FillInputPortInformation(int port, vtkInformation* info);


private:
	vtkGocadWriter(const vtkGocadWriter&);  // Not implemented.
	void operator=(const vtkGocadWriter&);  // Not implemented.
	
	vtkCellArray* Cells;
	vtkCellData* CellData;
	vtkDataArray* PointCoords;	
};

#endif


