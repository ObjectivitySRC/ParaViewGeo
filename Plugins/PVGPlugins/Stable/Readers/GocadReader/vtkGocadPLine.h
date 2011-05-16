/*Robert Maynard
MIRARCO 
Aug 287 2008

vtkGocadPLine handles reading triangle based gocad objects
*/


#ifndef __vtkGocadPLine_h
#define __vtkGocadPLine_h

#include "vtkGocadAtomic.h"


class VTK_EXPORT vtkGocadPLine : public vtkGocadAtomic
{
public:
  static vtkGocadPLine* New();
  vtkTypeRevisionMacro(vtkGocadPLine,vtkGocadAtomic);
  
protected:
  vtkGocadPLine();
  ~vtkGocadPLine();
    
  //the request data will call Read that does the real work
  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
 
  
private:
  vtkGocadPLine(const vtkGocadPLine&);  // Not implemented.
  void operator=(const vtkGocadPLine&);  // Not implemented.
};
#endif