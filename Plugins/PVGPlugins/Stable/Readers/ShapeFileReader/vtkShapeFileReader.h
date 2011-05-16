// .NAME vtkShapeFileReader from VTK\IO\vtkSimplePointsReader & vtkLocalConeSource - Example ParaView Plugin Source
// .SECTION Description
// vtkShapeFileReader is a subclass of vtkPolyDataAlgorithm to read Data Files Extracted from DXF Files.

#ifndef __vtkShapeFileReader_h
#define __vtkShapeFileReader_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkInformation.h"
#include "vtkSetGet.h"

#include <string>
using namespace std;

class VTK_EXPORT vtkShapeFileReader : public vtkPolyDataAlgorithm
{
public:
  static vtkShapeFileReader* New();
  vtkTypeRevisionMacro(vtkShapeFileReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();

protected:
  vtkShapeFileReader();
  ~vtkShapeFileReader();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

  int my_getline(ifstream* finptr, string* sbuff);  // std::string Version
  char* FileName;
private:

};

#endif
