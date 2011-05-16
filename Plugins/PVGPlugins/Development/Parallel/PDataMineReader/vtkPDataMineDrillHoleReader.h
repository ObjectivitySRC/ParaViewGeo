
#ifndef __PDataMineDrillHoleReader_h
#define __PDataMineDrillHoleReader_h

#include "vtkPDataMineReader.h"

class VTK_EXPORT vtkPDataMineDrillHoleReader : public vtkPDataMineReader
{
public:
  static vtkPDataMineDrillHoleReader* New();
  vtkTypeRevisionMacro(vtkPDataMineDrillHoleReader,vtkPDataMineReader);
   
  // Description:
  // Determine if the file can be readed with this reader.
  int CanReadFile( const char* fname );
  
protected:
  vtkPDataMineDrillHoleReader();
  ~vtkPDataMineDrillHoleReader();

virtual int ReadShrunks( int start, int end, vtkPolyData* out);

};
#endif
