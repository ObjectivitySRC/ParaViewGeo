/*=========================================================================
  Program:   ParaviewGeo
  Module:    $ vtkPDataMineReader.cxx $
  Author:    Arolde VIDJINNAGNI	    
  Date:      MIRARCO June 25, 2009 
  Version:   0.1
 =========================================================================*/

#ifndef __PDataMinePointReader_h
#define __PDataMinePointReader_h

#include "vtkPDataMineReader.h"

class VTK_EXPORT vtkPDataMineBlockReader : public vtkPDataMineReader
{
public:
  static vtkPDataMineBlockReader* New();
  vtkTypeRevisionMacro(vtkPDataMineBlockReader,vtkPDataMineReader);
   
  // Description:
  // Determine if the file can be readed with this reader.
  int CanReadFile( const char* fname );
  
protected:
  vtkPDataMineBlockReader();
  ~vtkPDataMineBlockReader();

	virtual int ReadShrunks( int start, int end, vtkPolyData* out);
	
};
#endif
