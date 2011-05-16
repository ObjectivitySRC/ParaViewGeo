/*Robert Maynard
MIRARCO 
Aug 287 2008

vtkGocadVSet handles reading triangle based gocad objects
*/


#ifndef __vtkGocadVSet_h
#define __vtkGocadVSet_h

#include "vtkGocadAtomic.h"

//BTX
class vtkInternalMap;
class vtkInternalProps;
//ETX

class VTK_EXPORT vtkGocadVSet : public vtkGocadAtomic
{
public:
  static vtkGocadVSet* New();
  vtkTypeRevisionMacro(vtkGocadVSet,vtkGocadAtomic);
  
protected:
  vtkGocadVSet();
  ~vtkGocadVSet();
    
  //the request data will call Read that does the real work
  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  
  virtual bool ReadCells( vtkCellArray *cells, int numberOfPoints );
  
private:
  vtkGocadVSet(const vtkGocadVSet&);  // Not implemented.
  void operator=(const vtkGocadVSet&);  // Not implemented.
};
#endif