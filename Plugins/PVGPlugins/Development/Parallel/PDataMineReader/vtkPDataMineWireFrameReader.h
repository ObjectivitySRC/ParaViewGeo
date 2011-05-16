/*=========================================================================*/
// .NAME vtkPDataSetReader - Manages reading pieces of a data set.
// .SECTION Description
// vtkPDataSetReader will read a piece of a file, it takes as input 
// a metadata file that lists all of the files in a data set.

#ifndef __vtkPDataMineWireFrameReader_h
#define __vtkPDataMineWireFrameReader_h


#include "vtkPDataMineReader.h"

class VTK_EXPORT vtkPDataMineWireFrameReader : public vtkPDataMineReader
{
public:
  static vtkPDataMineWireFrameReader* New();
  vtkTypeRevisionMacro(vtkPDataMineWireFrameReader,vtkPDataMineReader);
  
	vtkSetStringMacro(PointFileName); 
  vtkGetStringMacro(PointFileName); 
  
	vtkSetStringMacro(TopoFileName); 
  vtkGetStringMacro(TopoFileName);  

	vtkSetStringMacro(StopeSummaryFileName); 
  vtkGetStringMacro(StopeSummaryFileName); 

	vtkSetMacro(UseStopeSummary,int);
	vtkGetMacro(UseStopeSummary,int);

  // Description:
  // Determine if the file can be readed with this reader.
  int CanReadFile( const char* fname );
  
protected:
  vtkPDataMineWireFrameReader();
  ~vtkPDataMineWireFrameReader();

	
	virtual int ReadShrunks( int start, int end, vtkPolyData* out);

	//user defined filenames
  char* PointFileName;
  char* TopoFileName;	

	//special extra file info for Stope Summary Support
	char *StopeSummaryFileName;
	int UseStopeSummary;

};
#endif
