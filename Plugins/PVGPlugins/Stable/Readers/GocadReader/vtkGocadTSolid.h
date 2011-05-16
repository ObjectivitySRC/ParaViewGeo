/*Robert Maynard
MIRARCO 
Aug 287 2008

vtkGocadTSolid handles reading tetra  based gocad objects
*/


#ifndef __vtkGocadTSolid_h
#define __vtkGocadTSolid_h

#include "vtkGocadAtomic.h"

//BTX
class vtkInternalMap;
class vtkInternalProps;
//ETX

class VTK_EXPORT vtkGocadTSolid : public vtkGocadAtomic
{
public:
  static vtkGocadTSolid* New();
  vtkTypeRevisionMacro(vtkGocadTSolid,vtkGocadAtomic);
  
protected:
  vtkGocadTSolid();
  ~vtkGocadTSolid();
    
  //the request data will call Read that does the real work
  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector); 
  
private:
  vtkGocadTSolid(const vtkGocadTSolid&);  // Not implemented.
  void operator=(const vtkGocadTSolid&);  // Not implemented.
};
#endif