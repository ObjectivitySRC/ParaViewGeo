// .NAME vtkMap3dBlockReader
// .SECTION Description
// vtkMap3dBlockReader is a subclass of vtkPolyDataAlgorithm to read raw triangles
#ifndef __vtkMap3dBlockReader_h
#define __vtkMap3dBlockReader_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkInformation.h"
#include "vtkSetGet.h"
#include <string>

using namespace std;

class VTK_EXPORT vtkMap3dBlockReader : public vtkPolyDataAlgorithm
{
public:
  static vtkMap3dBlockReader* New();
  vtkTypeRevisionMacro(vtkMap3dBlockReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();

protected:
  vtkMap3dBlockReader();
  ~vtkMap3dBlockReader();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

  char* FileName;
private:

};

#endif
