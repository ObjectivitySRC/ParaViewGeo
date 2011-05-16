
#ifndef __vtkEqualAreaPropFilter_h
#define __vtkEqualAreaPropFilter_h

//#include "vtkPolyDataAlgorithm.h"
#include "vtkPointSetAlgorithm.h"
#include "vtkSetGet.h"


//class VTK_EXPORT vtkEqualAreaPropFilter : public vtkPolyDataAlgorithm
class VTK_EXPORT vtkEqualAreaPropFilter : public vtkPointSetAlgorithm
{
public:
  static vtkEqualAreaPropFilter *New();
//  vtkTypeRevisionMacro(vtkEqualAreaPropFilter,vtkPolyDataAlgorithm);
  vtkTypeRevisionMacro(vtkEqualAreaPropFilter,vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  //Description
  //Set/Get the value of Bins
  vtkSetMacro(Bins,int);
  vtkGetMacro(Bins,int);

protected:
  vtkEqualAreaPropFilter();
  ~vtkEqualAreaPropFilter() {}
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info);

  int Bins;

private:

  vtkEqualAreaPropFilter(const vtkEqualAreaPropFilter&);  // Not implemented.
  void operator=(const vtkEqualAreaPropFilter&);  // Not implemented.
};

#endif
