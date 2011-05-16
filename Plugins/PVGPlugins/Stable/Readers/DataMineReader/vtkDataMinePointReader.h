// .NAME DataMinePointReader from vtkDataMinePointReader
// .SECTION Description
// vtkDataMinePointReader is a subclass of vtkPolyDataAlgorithm
// to read DataMine binary Files (point, perimeter, wframe<points/triangle>)

#ifndef __DataMinePointReader_h
#define __DataMinePointReader_h

#include "vtkDataMineReader.h"

class VTK_EXPORT vtkDataMinePointReader : public vtkDataMineReader
{
public:
  static vtkDataMinePointReader* New();
  vtkTypeRevisionMacro(vtkDataMinePointReader,vtkDataMineReader);
   
  // Description:
  // Determine if the file can be readed with this reader.
  int CanReadFile( const char* fname );
  
protected:
  vtkDataMinePointReader();
  ~vtkDataMinePointReader();

	virtual void Read( vtkPoints* points, vtkCellArray *cells );
	//submethods depending on file type
	void ParsePoints( vtkPoints* points, vtkCellArray* cells, TDMFile *file, const int &XID, const int &YID, const int &ZID );

};
#endif
