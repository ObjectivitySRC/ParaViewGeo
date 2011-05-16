// .NAME vtkSGeMSCgridReader from vtkSGeMSCgridReader
// .SECTION Description
// vtkSGeMSReader is a subclass of vtkImageAlgorithm
// to read SGeMS binary Files (point, perimeter, wframe<points/triangle>)

#ifndef __vtkSGeMSCgridReader_h
#define __vtkSGeMSCgridReader_h

#include "vtkImageAlgorithm.h"

#include <string>
using namespace std;

class VTK_EXPORT vtkSGeMSCgridReader: public vtkImageAlgorithm
{
public:
  static vtkSGeMSCgridReader* New();
  vtkTypeRevisionMacro(vtkSGeMSCgridReader,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkSetStringMacro(FileName); //SetFileName()
  vtkGetStringMacro(FileName); //GetFileName()

protected:
  vtkSGeMSCgridReader();
  ~vtkSGeMSCgridReader();

  virtual int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *);

	int getInt(FILE* fp);
	float getFloat(FILE* fp);
  
  char *FileName;

private:
};
#endif
