
#ifndef __vtkVentilationReader_h
#define __vtkVentilationReader_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkInformation.h"
#include "vtkSetGet.h"
#include "vtkAppendPolyData.h"
#include <string>

//using namespace std;

class VTK_EXPORT vtkVentilationReader : public vtkPolyDataAlgorithm
{
public:
  static vtkVentilationReader* New();
  vtkTypeRevisionMacro(vtkVentilationReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
	vtkSetMacro(ShowArrowOn, int);
	vtkSetMacro(ScaleArrowSize, double);

	
	int CanReadFile( const char* fname );

protected:
  vtkVentilationReader();
  ~vtkVentilationReader();



  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

	void addItem(const char* text, double* point1, double* point2, vtkAppendPolyData* append);

	int AddArrow( vtkAppendPolyData* &append,
		double* point1, double* point2, double height, int coneResolution);

	void CreateMatrix( double rotation[16], double *direction, double *center);

	unsigned int loadHeaders(ifstream &myFile);

  char* FileName;
	double Size;
	int ShowArrowOn;
	double ScaleArrowSize;
private:

	int Node;
	int x1;
	int y1;
	int z1;
	int x2;
	int y2;
	int z2;
	int msdate;
	int velocity;
	int area;
	int volum;
	int gazeousType;
};

#endif
