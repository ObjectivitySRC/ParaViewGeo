// .NAME VulcanTrieReader
// VulcanReader is a subclass of vtkUnstructuredGridAlgorithm to read Vulcan Resource Files
// Rmaynard

#ifndef __vtkVulcanBlockModelReader_h
#define __vtkVulcanBlockModelReader_h

#include "vtkUnstructuredGridAlgorithm.h"

#include "blockmodel_api.h"


class VTK_EXPORT vtkVulcanBlockModelReader : public vtkUnstructuredGridAlgorithm {

public:
  static vtkVulcanBlockModelReader* New();
  vtkTypeRevisionMacro(vtkVulcanBlockModelReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);
	vtkGetStringMacro(FileName); 

	// Description:
  // Determine if the file can be readed with this reader.
	//not needed currently
  int CanReadFile( const char* fname );

protected:
  vtkVulcanBlockModelReader();
  ~vtkVulcanBlockModelReader();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

  char* FileName;
private:

};

#endif
