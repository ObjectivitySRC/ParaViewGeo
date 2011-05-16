
#ifndef __vtkDrainageReader_h
#define __vtkDrainageReader_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkInformation.h"
#include "vtkSetGet.h"
#include "vtkAppendPolyData.h"
#include <string>

//using namespace std;

class VTK_EXPORT vtkDrainageReader : public vtkPolyDataAlgorithm
{
public:
  static vtkDrainageReader* New();
  vtkTypeRevisionMacro(vtkDrainageReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
	vtkSetMacro(ShowArrowOn, int);
	vtkSetMacro(ScaleArrowSize, double);

	
	int CanReadFile( const char* fname );

protected:
  vtkDrainageReader();
  ~vtkDrainageReader();



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
	int tunnel_name; 
	int x1;
	int y1;
	int z1;
	int x2;
	int y2;
	int z2;
	int model;
	int motor;
	int power;
	int quantity;
	int flow;
	int pump;
	int drainpipe_diameter;
	int dibhole_capacity;

};

#endif
