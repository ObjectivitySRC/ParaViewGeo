

#ifndef __PDataMineDummyReader_h
#define __PDataMineDummyReader_h

#include "vtkPDataMineReader.h"
#include "dmfile.h"


class VTK_EXPORT vtkPDataMineDummyReader : public vtkPDataMineReader
{
public:
  static vtkPDataMineDummyReader* New();
  vtkTypeRevisionMacro(vtkPDataMineDummyReader,vtkPDataMineReader);
	
	     
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  
  int CanReadFile( const char* fname );    

protected:
  vtkPDataMineDummyReader();
  ~vtkPDataMineDummyReader();
 
  virtual int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *);  
                          
	char *FileName;	                          
};
#endif
