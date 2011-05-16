// .NAME vtkTriReader 
// .SECTION Description
// vtkTriReader is a subclass of vtkPolyDataAlgorithm to read raw triangles
#ifndef __vtkTriReader_h
#define __vtkTriReader_h

//#include "vtkPolyDataAlgorithm.h"
#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkInformation.h"
#include "vtkSetGet.h"
#include "vtkStdString.h"

class vtkSBuffer;

class VTK_EXPORT vtkTriReader : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkTriReader* New();
  vtkTypeRevisionMacro(vtkTriReader,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
  vtkSetMacro(HeaderExists, bool);

protected:
  vtkTriReader();
  ~vtkTriReader();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);
  
  char* FileName;
  bool HeaderExists;
private:
   void fromString(const vtkStdString &s, double &result);
   double* parseString(vtkStdString line);
};

#endif
