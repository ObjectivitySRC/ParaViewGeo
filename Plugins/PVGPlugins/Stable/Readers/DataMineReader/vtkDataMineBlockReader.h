// .NAME DataMinePointReader from vtkDataMineBlockReader
// .SECTION Description
// vtkDataMineBlockReader is a subclass of vtkPolyDataAlgorithm
// to read DataMine binary Files (point, perimeter, wframe<points/triangle>)

#ifndef __DataMineBlockReader_h
#define __DataMineBlockReader_h

#include "vtkDataMineReader.h"

class VTK_EXPORT vtkDataMineBlockReader : public vtkDataMineReader
{
public:
  static vtkDataMineBlockReader* New();
  vtkTypeRevisionMacro(vtkDataMineBlockReader,vtkDataMineReader);
   
  // Description:
  // Determine if the file can be readed with this reader.
  int CanReadFile( const char* fname );
  
protected:
  vtkDataMineBlockReader();
  ~vtkDataMineBlockReader();

	virtual void Read( vtkPoints* points, vtkCellArray *cells );
	//submethods depending on file type
	void ParsePoints( vtkPoints* points, vtkCellArray* cells, TDMFile *file,const int &XID, const int &YID, const int &ZID );

};
#endif
