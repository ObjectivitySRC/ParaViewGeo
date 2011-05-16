// .NAME DataMinePointReader from vtkDataMinePerimeterReader
// .SECTION Description
// vtkDataMinePerimeterReader is a subclass of vtkPolyDataAlgorithm
// to read DataMine binary Files (point, perimeter, wframe<points/triangle>)

#ifndef __DataMinePerimeterReader_h
#define __DataMinePerimeterReader_h

#include "vtkDataMineReader.h"

class VTK_EXPORT vtkDataMinePerimeterReader : public vtkDataMineReader
{
public:
  static vtkDataMinePerimeterReader* New();
  vtkTypeRevisionMacro(vtkDataMinePerimeterReader,vtkDataMineReader);
  
  // Description:
  // Determine if the file can be readed with this reader.
  int CanReadFile( const char* fname );
  
protected:
  vtkDataMinePerimeterReader();
  ~vtkDataMinePerimeterReader();
		
	virtual void Read( vtkPoints* points, vtkCellArray* cells );
	//submethods depending on file type
	void ParsePoints(vtkPoints* points, vtkCellArray* cells, TDMFile* file,const int &XID,const int &YID,const int &ZID,const int &PTN,const int &PV );		
	
	
};
#endif
