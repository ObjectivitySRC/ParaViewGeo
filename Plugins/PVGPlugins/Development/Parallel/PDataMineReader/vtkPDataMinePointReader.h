
#ifndef __PDataMinePointReader_h
#define __PDataMinePointReader_h

#include "vtkPDataMineReader.h"

class VTK_EXPORT vtkPDataMinePointReader : public vtkPDataMineReader
{
public:
  static vtkPDataMinePointReader* New();
  vtkTypeRevisionMacro(vtkPDataMinePointReader,vtkPDataMineReader);
   
  // Description:
  // Determine if the file can be readed with this reader.
  int CanReadFile( const char* fname );
  
protected:
  vtkPDataMinePointReader();
  ~vtkPDataMinePointReader();

	virtual int ReadShrunks( int start, int end, vtkPolyData* out);

};
#endif
