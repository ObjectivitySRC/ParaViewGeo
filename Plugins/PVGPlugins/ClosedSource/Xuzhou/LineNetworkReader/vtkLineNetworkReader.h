/*Nehme Bilal
MIRARCO 
Jan 2009

Line Network Reader:
can read file wich contains a set of lines. Each line is represented by 2 points coordinates and some properties:
x1,y1,z1,x2,y2,z2,prop1,prop2,...propn
The output of this reader is a poly data. The properties will be mapped on the cells
*/


#ifndef __vtkLineNetworkReader_h
#define __vtkLineNetworkReader_h

#include "vtkPolyDataAlgorithm.h"

#include "vtkAppendPolyData.h"
#include <vtkstd/map>

class vtkStdString;
class vtkDoubleArray;

struct LNR_Internals;


//typedef vtkstd::map < vtkStdString, vtkDoubleArray* > doubleArrayMapBase1;

class VTK_EXPORT vtkLineNetworkReader : public vtkPolyDataAlgorithm
{
public:
  
  static vtkLineNetworkReader* New();
  vtkTypeRevisionMacro(vtkLineNetworkReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

	vtkSetStringMacro(ArrowProp);
	vtkGetStringMacro(ArrowProp);

	vtkSetStringMacro(x1);
	vtkGetStringMacro(x1);

	vtkSetStringMacro(y1);
	vtkGetStringMacro(y1);

	vtkSetStringMacro(z1);
	vtkGetStringMacro(z1);

	vtkSetStringMacro(x2);
	vtkGetStringMacro(x2);

	vtkSetStringMacro(y2);
	vtkGetStringMacro(y2);

	vtkSetStringMacro(z2);
	vtkGetStringMacro(z2);

	vtkSetMacro(withTunnels, int);
	vtkGetMacro(withTunnels, int);

	vtkSetMacro(ScaleArrowSize, double);

	void SetArrays(const char *a, int s);

	int AddArrow( vtkAppendPolyData* &append, double* point1, 
									double* point2, int coneResolution);

	int AddTunnel( vtkAppendPolyData* &append, 
								 double* point1, double* point2, double resolution);

	//void addItem(const char* text, double* point1,
		//double* point2, vtkAppendPolyData* append);

	void CreateMatrix( 
		double rotation[16], double *direction, double *center );

	bool isNumeric(vtkStdString value);





protected:
  vtkLineNetworkReader();
  ~vtkLineNetworkReader();
   

  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
		vtkInformationVector* outputVector);

int RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);
  
  char* FileName;
	char* Properties;
	char* ArrowProp;

	char* x1;
	char* y1;
	char* z1;
	char* x2;
	char* y2;
	char* z2;


	double Size;
	int ScaleArrowSize;
	int withTunnels;
			
    
private:
  vtkLineNetworkReader(const vtkLineNetworkReader&);  // Not implemented.
  void operator=(const vtkLineNetworkReader&);  // Not implemented.

	LNR_Internals *Internal;

};

#endif  