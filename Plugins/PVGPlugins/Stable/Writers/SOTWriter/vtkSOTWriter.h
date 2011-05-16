/*=================================================
==  vtkSOTWriter.h
==  By: Patrick Ryan & Julie Zuo
==  Date: May 21, 2009
==  Description: Writes SOT Schedule IDs of selected
==    pointsto a .sqf (SOT Queue File) file. To be 
==    used in conjunction with vtkSOTResult.
================================================*/
#ifndef __vtkSOTWriter_h
#define __vtkSOTWriter_h

#include "vtkDataWriter.h"
#include "vtkUnstructuredGrid.h"

class vtkStringArray;

class VTK_EXPORT vtkSOTWriter : public vtkDataWriter
{
public:
	static vtkSOTWriter *New();
	vtkTypeRevisionMacro(vtkSOTWriter,vtkDataWriter);

protected:
	vtkSOTWriter();
	~vtkSOTWriter();
	void WriteData();
  virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
	vtkSOTWriter(const vtkSOTWriter&);  // Not implemented.
	void operator=(const vtkSOTWriter&);  // Not implemented.
};
#endif


