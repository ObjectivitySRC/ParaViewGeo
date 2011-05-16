// .NAME vtkBlockModelReader 
// .SECTION Description
// vtkBlockModelReader is a subclass of vtkStructuredPointsReader to read raw triangles
#ifndef __vtkBlockModelReader_h
#define __vtkBlockModelReader_h

#include "vtkStructuredPointsReader.h"
#include "vtkInformation.h"
#include "vtkSetGet.h"
#include "vtkStdString.h"

class VTK_EXPORT vtkBlockModelReader : public vtkStructuredPointsReader
{
public:
  static vtkBlockModelReader* New();
  vtkTypeRevisionMacro(vtkBlockModelReader,vtkStructuredPointsReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
  vtkSetMacro(HeaderExists, bool);
  
  int CanReadFile( const char* fname );
protected:
  vtkBlockModelReader();
  ~vtkBlockModelReader();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

  virtual int RequestInformation(vtkInformation *, vtkInformationVector **,
                                 vtkInformationVector *);
  
  char* FileName;
  bool HeaderExists;
    
private:
   void fromString(const vtkStdString &s, double &result);
   double* parseCoords(vtkStdString line);
   int numberOfValuesOnLine(vtkStdString line);
   double* parseProperties(vtkStdString line, int numValues);
   vtkStdString* headerNames(vtkStdString line, int num);
   double max(double one, double two);
   double min(double one, double two);
};

#endif
