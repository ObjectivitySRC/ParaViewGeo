// .NAME DataMineDummyReader from vtkDataMineDummyReader
// .SECTION Description
// vtkDataMineDummyReader is a subclass of vtkPolyDataAlgorithm
// to read DataMine binary Files that we currently do not support

#ifndef __DataMineDummyReader_h
#define __DataMineDummyReader_h

#include "vtkPolyDataAlgorithm.h"
#include "dmfile.h"


class VTK_EXPORT vtkDataMineDummyReader : public vtkPolyDataAlgorithm
{
public:
  static vtkDataMineDummyReader* New();
  vtkTypeRevisionMacro(vtkDataMineDummyReader,vtkPolyDataAlgorithm);
	
	     
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  
  int CanReadFile( const char* fname );    
protected:
  vtkDataMineDummyReader();
  ~vtkDataMineDummyReader();
 
  virtual int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *);  
                          
	char *FileName;	                          
};
#endif
