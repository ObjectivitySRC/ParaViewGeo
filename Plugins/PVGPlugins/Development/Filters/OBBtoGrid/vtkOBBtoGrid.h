/*create a structured grid inside an oriented bounding box*/


#ifndef _vtkOBBtoGrid_h
#define _vtkOBBtoGrid_h
#include "vtkStructuredGridAlgorithm.h"




class VTK_EXPORT vtkOBBtoGrid : public vtkStructuredGridAlgorithm
{
public:
  static vtkOBBtoGrid *New();
  vtkTypeRevisionMacro(vtkOBBtoGrid,vtkStructuredGridAlgorithm);

	vtkSetVector3Macro(GridSize, int);
	vtkGetVector3Macro(GridSize, int);

protected:
  vtkOBBtoGrid();
  ~vtkOBBtoGrid();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
	virtual int FillInputPortInformation(int port, vtkInformation *info);


private:

	int GridSize[3];

  vtkOBBtoGrid(const vtkOBBtoGrid&);
  void operator = (const vtkOBBtoGrid&);
};
#endif
