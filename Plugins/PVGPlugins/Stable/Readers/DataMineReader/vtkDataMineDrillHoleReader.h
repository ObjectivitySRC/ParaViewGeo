// .NAME vtkDataMineDrillHoleReader from vtkDataMineDrillHoleReader
// .SECTION Description
// vtkDataMineDrillHoleReader is a subclass of vtkPolyDataAlgorithm
// to read DataMine binary Files (point, perimeter, wframe<points/triangle>)

#ifndef __vtkDataMineDrillHoleReader_h
#define __vtkDataMineDrillHoleReader_h

#include "vtkDataMineReader.h"

class VTK_EXPORT vtkDataMineDrillHoleReader : public vtkDataMineReader
{
public:
  static vtkDataMineDrillHoleReader* New();
  vtkTypeRevisionMacro(vtkDataMineDrillHoleReader,vtkDataMineReader);

  // Description:
  // Determine if the file can be readed with this reader.
  int CanReadFile( const char* fname );
  
protected:
  vtkDataMineDrillHoleReader();
  ~vtkDataMineDrillHoleReader();

	void Read( vtkPoints* points, vtkCellArray *cells );
	//submethods depending on file type
	void ParsePoints( vtkPoints* points, vtkCellArray *cells, TDMFile *file,const int &XID,const int &YID,const int &ZID,const int &BHID,const int &BHIDSize );

};
#endif
