// .NAME vtkSGeMSReader from vtkSGeMSReader
// .SECTION Description
// vtkSGeMSReader is a subclass of vtkPolyDataAlgorithm
// to read SGeMS binary Files (point, perimeter, wframe<points/triangle>)

#ifndef __vtkSGeMSReader_h
#define __vtkSGeMSReader_h

#include "vtkPolyDataAlgorithm.h"
// #include "vtkSGeMSBlkModelReader.h"

#include <string>
using namespace std;

class vtkCallbackCommand;

class VTK_EXPORT vtkSGeMSReader : public vtkPolyDataAlgorithm
{
public:
  static vtkSGeMSReader* New();
  vtkTypeRevisionMacro(vtkSGeMSReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
/*
  vtkGetObjectMacro(CellDataArraySelection, vtkDataArraySelection);
  int GetCellArrayStatus(const char* name);
  void SetCellArrayStatus(const char* name, int status);  
	int GetNumberOfCellArrays();
	const char* GetCellArrayName(int index);
*/
  vtkSetStringMacro(FileName); //SetFileName()
  vtkGetStringMacro(FileName); //GetFileName()

protected:
  vtkSGeMSReader();
  ~vtkSGeMSReader();

  virtual int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *);
  virtual int FillOutputPortInformation(int,  vtkInformation* info);

	int getInt(FILE* fp);
	float getFloat(FILE* fp);
  
  char *FileName;
	char fileType;  // P(points) C(Cgrid) R(ReducedGrid)

private:
};
#endif
