// .NAME vtkPolyLineAddPoints - Add points to an existing PolyLine
// .SECTION Description
// Author: Matthew Livingstone -- February 2009

#ifndef __vtkPolyLineAddPoints_h
#define __vtkPolyLineAddPoints_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkInformationVector.h"


class VTK_EXPORT vtkPolyLineAddPoints : public vtkPolyDataAlgorithm
{
public:
	static vtkPolyLineAddPoints* New();
	vtkTypeRevisionMacro(vtkPolyLineAddPoints,vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetClampMacro(AddFactor,int,2,10);
  vtkGetMacro(AddFactor,int);

protected:
  vtkPolyLineAddPoints();
  ~vtkPolyLineAddPoints();

	// Point multiplier
	int AddFactor;

	virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	virtual int FillInputPortInformation(int port, vtkInformation* info);

private:
  vtkPolyLineAddPoints(const vtkPolyLineAddPoints&);  // Not implemented.
  void operator=(const vtkPolyLineAddPoints&);  // Not implemented.
};

#endif
