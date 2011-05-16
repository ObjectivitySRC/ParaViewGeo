#ifndef _vtkRuler_h
#define _vtkRuler_h
#include "vtkPolyDataAlgorithm.h"
class VTK_EXPORT vtkRuler : public vtkPolyDataAlgorithm
{
public:
  static vtkRuler *New();
  vtkTypeRevisionMacro(vtkRuler,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

	vtkGetMacro(distance, double);

	vtkGetMacro(xDistance, double);

	vtkGetMacro(yDistance, double);

	vtkGetMacro(zDistance, double);

  vtkSetVector3Macro(Point1, float);
  vtkGetVector3Macro(Point1, float);

  vtkSetVector3Macro(Point2, float);
  vtkGetVector3Macro(Point2, float);

protected:
  vtkRuler();
  ~vtkRuler();
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
	void calculateDistances();
	double distance;
	double xDistance;
	double yDistance;
	double zDistance;

  float Point1[3];
  float Point2[3];

private:
  vtkRuler(const vtkRuler&);
  void operator = (const vtkRuler&);
};
#endif
