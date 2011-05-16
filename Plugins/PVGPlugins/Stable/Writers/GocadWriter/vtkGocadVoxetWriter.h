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

#ifndef __vtkGocadVoxetWriter_h
#define __vtkGocadVoxetWriter_h

#include "vtkGocadBase.h"

#define X_MIN 0
#define X_MAX 1
#define Y_MIN 2
#define Y_MAX 3
#define Z_MIN 4
#define Z_MAX 5

class vtkStringArray;
class vtkStdString;
class vtkImageData;
class vtkDataArray;
class vtkPointData;
class vtkCellData;

class VTK_EXPORT vtkGocadVoxetWriter : public vtkGocadBase
{
public:
	static vtkGocadVoxetWriter *New();
	vtkTypeRevisionMacro(vtkGocadVoxetWriter,vtkGocadBase);
	
	// Description:
	// Get the input to this writer.
	vtkImageData* GetInput();
	vtkImageData* GetInput(int port);
		
protected:
	vtkGocadVoxetWriter();
	~vtkGocadVoxetWriter();

	
	void WritePoints(ostream* file1, vtkImageData* input, vtkStringArray* propNames);
	void WritePointData(vtkImageData* input, ostream* fp, vtkStringArray* propNames);
	
	void WriteData();

	virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
	vtkGocadVoxetWriter(const vtkGocadVoxetWriter&);  // Not implemented.
	void operator=(const vtkGocadVoxetWriter&);  // Not implemented.
	vtkCellData* CellData;

	//number of points on each axis
	int I_Dimension;
	int J_Dimension;
	int K_Dimension;
};

#endif


