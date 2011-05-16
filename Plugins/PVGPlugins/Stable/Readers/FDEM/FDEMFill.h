// .NAME FDEMFill
// .SECTION Description
// a datastore for the triangles
//sublcass of FDEMStorage

#ifndef __FDEMFill_h
#define __FDEMFill_h

#include "FDEMStorage.h"

class TriangleProperties;

class FDEMFill : public FDEMStorage
{
  public:    
  FDEMFill(char *Name, double NormCoord, 
      double NormForce, double NormVelocity, double NormStress );          
    //Add a FDEM binary block to the storage
    virtual void Add( double block[100]);
        
    //return a constructed vtkPolyData 
    virtual vtkPolyData* GetOutput(  );        
  };
#endif