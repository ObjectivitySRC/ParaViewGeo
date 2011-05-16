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

#ifndef __vtkGocadCompositeWriter_h
#define __vtkGocadCompositeWriter_h

#include "vtkGocadBase.h"
class vtkStdString;
class vtkCompositeDataSet;
class vtkGocadCompositeWriterInternals;
class vtkDataWriter;

class VTK_EXPORT vtkGocadCompositeWriter : public vtkGocadBase
{
public:
	static vtkGocadCompositeWriter *New();
	vtkTypeRevisionMacro(vtkGocadCompositeWriter,vtkGocadBase);
  
protected:
	vtkGocadCompositeWriter();
	~vtkGocadCompositeWriter();
		
  virtual int RequestData(vtkInformation*  , vtkInformationVector** , vtkInformationVector*);
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  
  // Internal method called recursively
  virtual int WriteComposite(vtkCompositeDataSet* compositeData, int &writerIdx);  
  virtual int WriteNonCompositeData(vtkDataObject* dObj, vtkStdString name,  int &writerIdx);
  virtual void CreateWriters(vtkCompositeDataSet* hdInput);
  virtual void FillDataTypes(vtkCompositeDataSet* hdInput);
  virtual vtkGocadBase* GetWriter(int index);
  
  //for writing the gocad group file
  virtual void WriteGroupHeader( int num );
  virtual void WriteGroupHeader( vtkStdString name, int num );
  virtual void WriteGroupEnd();
  
private:  
	vtkGocadCompositeWriterInternals* Internal;  
	
private:
	vtkGocadCompositeWriter(const vtkGocadCompositeWriter&);  // Not implemented.
	void operator=(const vtkGocadCompositeWriter&);  // Not implemented.
};

#endif


