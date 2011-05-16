
#ifndef __vtkTransportationReader_h
#define __vtkTransportationReader_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkInformation.h"
#include "vtkSetGet.h"
#include "vtkAppendPolyData.h"
#include <string>

//using namespace std;

class VTK_EXPORT vtkTransportationReader : public vtkPolyDataAlgorithm
{
public:
  static vtkTransportationReader* New();
  vtkTypeRevisionMacro(vtkTransportationReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
	vtkSetMacro(ShowArrowOn, int);
	vtkSetMacro(ScaleArrowSize, double);
	
	int CanReadFile( const char* fname );

protected:
  vtkTransportationReader();
  ~vtkTransportationReader();



  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);


	int AddArrow( vtkAppendPolyData* &append,
		double* point1, double* point2, double height);

	void CreateMatrix( double rotation[16], double *direction, double *center);

	unsigned int loadHeaders(ifstream &myFile);

  char* FileName;
	double Size;
	int ShowArrowOn;
	double ScaleArrowSize;
private:

	int id;
	int x1;
	int y1;
	int z1;
	int x2;
	int y2;
	int z2;
	int model;
	int length;
	int capacity;
	int velocity;
	int width;
	int dip;

};

#endif










