// .NAME VulcanTrieReader
// VulcanReader is a subclass of vtkPolyDataAlgorithm to read Vulcan Resource Files
// Rmaynard

#ifndef __vtkVulcanTriReader_h
#define __vtkVulcanTriReader_h

#include "vtkPolyDataAlgorithm.h"

#include "tri_api.h"


class VTK_EXPORT vtkVulcanTriReader : public vtkPolyDataAlgorithm {

public:
  static vtkVulcanTriReader* New();
  vtkTypeRevisionMacro(vtkVulcanTriReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);
	vtkGetStringMacro(FileName); 

	// Description:
  // Determine if the file can be readed with this reader.
	//not needed currently
  int CanReadFile( const char* fname );

protected:
  vtkVulcanTriReader();
  ~vtkVulcanTriReader();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

  char* FileName;
private:

};

#endif
