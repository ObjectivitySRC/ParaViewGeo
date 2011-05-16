/*Matthew Livingstone
March 2009

Point File Reader:
- Based on Line Network Reader
- Can read any separated file
- User can specify separator, as well as a no data value for empty values
*/


#ifndef __vtkPointSetReader_h
#define __vtkPointSetReader_h

#include "vtkPolyDataAlgorithm.h"

#include "vtkAppendPolyData.h"
#include <vtkstd/map>


class vtkStdString;
class vtkDoubleArray;

struct LNR_Internals;


//typedef vtkstd::map < vtkStdString, vtkDoubleArray* > doubleArrayMapBase1;

class VTK_EXPORT vtkPointSetReader : public vtkPolyDataAlgorithm
{
public:
  
  static vtkPointSetReader* New();
  vtkTypeRevisionMacro(vtkPointSetReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

	vtkSetStringMacro(Separator);
	vtkGetStringMacro(Separator);

	vtkSetStringMacro(NoDataValue);
	vtkGetStringMacro(NoDataValue);

	vtkSetStringMacro(x1);
	vtkGetStringMacro(x1);

	vtkSetStringMacro(y1);
	vtkGetStringMacro(y1);

	vtkSetStringMacro(z1);
	vtkGetStringMacro(z1);
	
	void SetArrays(const char *a, int s);

	bool isNumeric(vtkStdString value);
	
	bool validateLine(vtkStdString line);
	//checks and see if the file is a valid format
	int CanReadFile( const char* fname);



protected:
  vtkPointSetReader();
  ~vtkPointSetReader();
   

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
	char* Separator;
	char* NoDataValue;

	double Size;
    
private:
  vtkPointSetReader(const vtkPointSetReader&);  // Not implemented.
  void operator=(const vtkPointSetReader&);  // Not implemented.

	LNR_Internals *Internal;

};

#endif  