// .NAME VulcanGrideReader
// VulcanReader is a subclass of vtkImageAlgorithm to read Vulcan Resource Files
// Rmaynard

#ifndef __vtkVulcanGridReader_h
#define __vtkVulcanGridReader_h

#include "vtkImageAlgorithm.h"

#include "tri_api.h"


class VTK_EXPORT vtkVulcanGridReader : public vtkImageAlgorithm {

public:
  static vtkVulcanGridReader* New();
  vtkTypeRevisionMacro(vtkVulcanGridReader,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);
	vtkGetStringMacro(FileName); 

	// Description:
  // Determine if the file can be readed with this reader.
	//not needed currently
  int CanReadFile( const char* fname );

protected:
  vtkVulcanGridReader();
  ~vtkVulcanGridReader();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

  char* FileName;
private:

};

#endif
