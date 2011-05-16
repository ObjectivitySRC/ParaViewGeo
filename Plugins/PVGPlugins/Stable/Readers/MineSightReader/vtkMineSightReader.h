// .NAME MineSightReader from VTK\IO\vtkSimplePointsReader & vtkLocalConeSource - Example ParaView Plugin Source
// .SECTION Description
// MineSightReader is a subclass of vtkPolyDataAlgorithm to read MineSight Resource Files

#ifndef __vtkMineSightReader_h
#define __vtkMineSightReader_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkInformation.h"
#include "vtkStringList.h"
#include "vtkSetGet.h"
#include "cimsr.h"

#include <string>
using namespace std;

class VTK_EXPORT vtkMineSightReader : public vtkPolyDataAlgorithm {

public:
  static vtkMineSightReader* New();
  vtkTypeRevisionMacro(vtkMineSightReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();

protected:
  vtkMineSightReader();
  ~vtkMineSightReader();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

	int ColorIndex(RGB* ecolor);

  char* FileName;
private:
	vtkStringList* colorlistptr;

};

#endif
