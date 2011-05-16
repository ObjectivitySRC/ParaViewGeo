// .NAME FDEMCrackJoint
// .SECTION Description
// a datastore for the cracks, which 
//are type of represenation of the data, usually the outline
//sublcass of FDEMStorage

#ifndef __FDEMCrackJoint_h
#define __FDEMCrackJoint_h

#include "FDEMStorage.h"

class vtkFloatArray;

class FDEMCrackJoint : public FDEMStorage
{
  public:     
    FDEMCrackJoint(char *Name, int mode, double NormCoord);
      
    //Add a FDEM binary block to the storage
    virtual void Add( double block[100]);
    
    //add Properties
    virtual void AddProperties( double block[100] );
        
    //return a constructed vtkPolyData 
    virtual vtkPolyData* GetOutput(  );
  private:    
    int Mode;    
          
             
    
    
        
  };
#endif