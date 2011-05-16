#ifndef _vtkDrillholeGA_h
#define _vtkDrillholeGA_h
#include "vtkPolyDataAlgorithm.h"



class VTK_EXPORT vtkDrillholeGA : public vtkPolyDataAlgorithm
{
public:
  static vtkDrillholeGA *New();
  vtkTypeRevisionMacro(vtkDrillholeGA,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkDrillholeGA();
  ~vtkDrillholeGA();
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkDrillholeGA(const vtkDrillholeGA&);
  void operator = (const vtkDrillholeGA&);

  int FakeMain(int argc, char **argv);

};
#endif
