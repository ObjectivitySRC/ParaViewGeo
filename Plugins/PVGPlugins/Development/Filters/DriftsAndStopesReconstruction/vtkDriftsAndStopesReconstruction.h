

#ifndef __DriftsAndStopesReconstruction_h
#define __DriftsAndStopesReconstruction_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkCleanCells.h"



#include "vtkCellData.h"



class VTK_EXPORT vtkDriftsAndStopesReconstruction : public vtkPolyDataAlgorithm
{
public:
  static vtkDriftsAndStopesReconstruction* New();
  vtkTypeRevisionMacro(vtkDriftsAndStopesReconstruction, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
	void buildDriftsWithTopology();
	void buildDriftsWithGeometry();

  vtkSetMacro(ToleranceIsAbsolute,int);
  vtkGetMacro(ToleranceIsAbsolute,int);

  // Description:
  // Specify tolerance in terms of fraction of bounding box length.
  vtkSetMacro(Tolerance,double);

  // Description:
  // Specify tolerance in absolute terms
  vtkSetMacro(AbsoluteTolerance,double);


protected:
  vtkDriftsAndStopesReconstruction();
  ~vtkDriftsAndStopesReconstruction();

  /// Implementation of the algorithm.
  virtual int RequestData(vtkInformation *, 
                          vtkInformationVector **, 
                          vtkInformationVector *);
	virtual int FillInputPortInformation(int port, vtkInformation *info);

  double Tolerance;
  double AbsoluteTolerance;
  int ToleranceIsAbsolute;


private:
  vtkDriftsAndStopesReconstruction(const vtkDriftsAndStopesReconstruction&); // Not implemented.
  void operator=(const vtkDriftsAndStopesReconstruction&); // Not implemented.

	vtkPolyData *realInput;
	vtkPolyData *output;
	vtkPolyData *input;
	vtkCellArray *outCells;
	vtkCellArray *inCells;
	vtkPoints *inPoints;
	vtkPoints *outPoints;
	int numberOfCells;
	int count;


};

#endif


