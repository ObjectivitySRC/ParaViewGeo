//===================================================================================//
//				Mirarco Mining Innovation
// Author:   Nehme Bilal
// Date:     Augaust 2009
// contact: nehmebilal@gmail.com
//====================================================================================//

#ifndef __vtkFitDataset_h
#define __vtkFitDataset_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"


class vtkPoints;
class vtkAppendPolyData;

class VTK_EXPORT vtkFitDataset : public vtkPolyDataAlgorithm
{
public:
  static vtkFitDataset *New();
  vtkTypeRevisionMacro(vtkFitDataset,vtkPolyDataAlgorithm);

	void computeODRLine(double* V);
	void computeODRPolygon(double* N);
	void computeODRRectangle(double* N, double* V);
	void computeODRBox(double* N, double* V);
	void compute3dConvexHull();

  vtkSetMacro(Type, int);
	vtkGetMacro(Type, int);

protected:

  vtkFitDataset();
  ~vtkFitDataset();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
  vtkFitDataset(const vtkFitDataset&);  // Not implemented.
  void operator=(const vtkFitDataset&);  // Not implemented.

	int Type;

	double bounds[6];
	double centroid[3];
	double **points;

	vtkPolyData *output;
	vtkPolyData *input;
	vtkPoints* inPoints;

	int numberOfPoints;

	vtkAppendPolyData* append;

};

#endif
