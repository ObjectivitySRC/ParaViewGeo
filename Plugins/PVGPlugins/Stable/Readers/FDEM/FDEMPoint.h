// .NAME FDEMPoint
// .SECTION Description
// a datastore for the point FDEM polydata
//the points are the triangle center points 
//sublcass of FDEMStorage

#ifndef __FDEMPoint_h
#define __FDEMPoint_h

#include "FDEMStorage.h"

class TriangleProperties;

class FDEMPoint : public FDEMStorage
{
  public:     
    FDEMPoint(char *Name, double NormCoord, 
      double NormForce, double NormVelocity, double NormStress );
      
    //Add a FDEM binary block to the storage
    virtual void Add( double block[100] );
        
    //return a constructed vtkPolyData 
    virtual vtkPolyData* GetOutput();      
  };
#endif