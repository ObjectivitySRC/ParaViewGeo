// .NAME DataMinePointReader from vtkPDataMinePerimeterReader
// .SECTION Description
// vtkPDataMinePerimeterReader is a subclass of vtkPolyDataAlgorithm
// to read DataMine binary Files (point, perimeter, wframe<points/triangle>)

#ifndef __PDataMinePerimeterReader_h
#define __PDataMinePerimeterReader_h

#include "vtkPDataMineReader.h"

class VTK_EXPORT vtkPDataMinePerimeterReader : public vtkPDataMineReader
{
public:
  static vtkPDataMinePerimeterReader* New();
  vtkTypeRevisionMacro(vtkPDataMinePerimeterReader,vtkPDataMineReader);
   
  // Description:
  // Determine if the file can be readed with this reader.
  int CanReadFile( const char* fname );
  
protected:
  vtkPDataMinePerimeterReader();
  ~vtkPDataMinePerimeterReader();

	virtual int ReadShrunks( int start, int end, vtkPolyData* out);

};
#endif
