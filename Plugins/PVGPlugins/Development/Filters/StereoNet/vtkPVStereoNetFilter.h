#ifndef __vtkPVStereoNetFilter_h
#define __vtkPVStereoNetFilter_h

#include "vtkPolyDataAlgorithm.h"

class vtkCellArray;
class vtkPoints;

class VTK_EXPORT vtkPVStereoNetFilter : public vtkPolyDataAlgorithm
{
public:
	static vtkPVStereoNetFilter *New();
	vtkTypeRevisionMacro(vtkPVStereoNetFilter,vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkSetMacro(Mode,int);
	vtkGetMacro(Mode,int);

	virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
	virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	virtual int FillInputPortInformation(int port, vtkInformation *info);
	virtual int FillOutputPortInformation(int port, vtkInformation *info);

protected:
	vtkPVStereoNetFilter();
	~vtkPVStereoNetFilter();
	void unitCircle(vtkCellArray *outCells, vtkPoints *outPoints);
	void TransformPlanes( vtkCellArray *inCells, vtkPoints *inPoints, vtkCellArray *outCells, vtkPoints *outPoints );
	void TransformLines( vtkCellArray *inCells, vtkPoints *inPoints, vtkCellArray *outCells, vtkPoints *outPoints );
	bool TransformCoordinate( double *xyz);
	void LineIntersect( double *xyz );
	
	int Mode;	

private:
	vtkPVStereoNetFilter(const vtkPVStereoNetFilter&);
	void operator = (const vtkPVStereoNetFilter&);

};
#endif
