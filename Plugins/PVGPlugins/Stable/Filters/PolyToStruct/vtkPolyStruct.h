
// the SetCorners() member.

#ifndef __vtkPolyStruct_h
#define __vtkPolyStruct_h

#include "vtkImageAlgorithm.h"
#include "vtkSetGet.h"

class VTK_EXPORT vtkPolyStruct : public vtkImageAlgorithm
{
public:
  static vtkPolyStruct *New();
  vtkTypeRevisionMacro(vtkPolyStruct,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  //Description
  //Set/Get the value of XCubes
  vtkSetMacro(XCubes, int);
  vtkGetMacro(XCubes, int);

  //Description
  //Set/Get the value of YCubes
  vtkSetMacro(YCubes, int);
  vtkGetMacro(YCubes, int);

  //Description
  //Set/Get the value of ZCubes
  vtkSetMacro(ZCubes,int);
  vtkGetMacro(ZCubes, int);


protected:
  vtkPolyStruct();
  ~vtkPolyStruct() {}
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info);

  int XCubes;
  int YCubes;
  int ZCubes;
    
private:

  vtkPolyStruct(const vtkPolyStruct&);  // Not implemented.
  void operator=(const vtkPolyStruct&);  // Not implemented.
};

#endif
