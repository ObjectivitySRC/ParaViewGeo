// .NAME vtkEPSReader 
// .SECTION Description
// vtkEPSReader is a subclass of vtkPolyDataAlgorithm to read raw triangles
#ifndef __EPSReader_h
#define __EPSReader_h

class vtkStdString;
class vtkStringArray;
class vtkDoubleArray;
#include "vtkPythonProgrammableFilter.h"

class VTK_EXPORT vtkEPSReader : public vtkPythonProgrammableFilter
{
public:
  static vtkEPSReader* New();
  vtkTypeRevisionMacro(vtkEPSReader,vtkPythonProgrammableFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  //set get for FileName
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  
  //set get for Date
  vtkSetStringMacro(Date);
  vtkGetStringMacro(Date);


protected:
  vtkEPSReader();
  ~vtkEPSReader();
  
  virtual int RequestInformation(vtkInformation* request, 
                                 vtkInformationVector** inputVector,                                  
                                 vtkInformationVector* outputVector);
  virtual  vtkStdString CreateReference(const char* funcname);
  
  char* FileName;  
  char* Date;  
  
  //hack to hold 3 pieces of information that requestInfo should have set
  vtkStringArray* PyPointFile;
  vtkStringArray* PyDate;  
  vtkDoubleArray* PyOffset;
    
private:
   
};

#endif
