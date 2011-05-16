/*Robert Maynard
MIRARCO 
Aug 287 2008

vtkGocadTSurf handles reading triangle based gocad objects
*/


#ifndef __vtkGocadTSurf_h
#define __vtkGocadTSurf_h

#include "vtkGocadAtomic.h"
#include "vtkIdTypeArray.h"

//BTX
class vtkInternalMap;
class vtkInternalProps;
//ETX
//class vtkIdTypeArray.h

class VTK_EXPORT vtkGocadTSurf : public vtkGocadAtomic
{
public:
  static vtkGocadTSurf* New();
  vtkTypeRevisionMacro(vtkGocadTSurf,vtkGocadAtomic);
  
protected:
  vtkGocadTSurf();
  ~vtkGocadTSurf();
    
  //the request data will call Read that does the real work
  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
 
  
private:
  vtkGocadTSurf(const vtkGocadTSurf&);  // Not implemented.
  void operator=(const vtkGocadTSurf&);  // Not implemented.
};
#endif