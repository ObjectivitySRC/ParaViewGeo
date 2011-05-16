// .NAME vtkArcMapReader from VTK\IO\vtkSimplePointsReader & vtkLocalConeSource - Example ParaView Plugin Source
// .SECTION Description
// vtkArcMapReader is a subclass of vtkMultiBlockDataSetAlgorithm to read Gocad Voxet Files

#ifndef __vtkArcMapReader_h
#define __vtkArcMapReader_h

#include <vtksys/ios/sstream>
#include "vtkUnstructuredGridAlgorithm.h"

class vtkStdString;
class vtkFloatArray;
class vtkCollection;
class vtkInformationVector;
class vtkStringList;
class vtkIdTypeArray;
class vtkUnsignedCharArray;

class VTK_EXPORT vtkArcMapReader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkArcMapReader* New();
  vtkTypeRevisionMacro(vtkArcMapReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();

protected:
  vtkArcMapReader();
  ~vtkArcMapReader();

  virtual int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);
                  
  int RequestInformation(vtkInformation *,vtkInformationVector **,vtkInformationVector *outputVector);

  bool Read(ifstream* file, double &x, double &y, vtkFloatArray *z, vtkIdTypeArray *prop, int linesToRead);
  bool SkipXRow(ifstream* file, double &x, double &y, vtkFloatArray *z, vtkIdTypeArray *prop, int linesToRead, int currentX);
  //BTX
  std::stringstream textStream; //the stream to be re-utilzied by the class
  //ETX
private:
	vtkStringList* parseString(vtkStdString line);

	const char* FileName; //name of file (full path), useful for obtaining object names

	double xAxisJumpValue; //distance between 2 points on X axis
	double yAxisJumpValue; //distance between 2 points on Y axis
	void Sort(vtkFloatArray *list, vtkIdTypeArray *props);
	
};

#endif
