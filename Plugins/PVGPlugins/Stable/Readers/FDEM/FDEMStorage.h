// .NAME FDEMStorage
// .SECTION Description
// a datastore for the point FDEM polydata
//the points are the triangle center points 

#ifndef __FDEMStroage_h
#define __FDEMStroage_h

#include "TriangleProperties.h"

class vtkPolyData;
class vtkPoints;
class vtkCellArray;


class FDEMStorage
{
public:     
    FDEMStorage();
    FDEMStorage(char *Name, double NormCoord, 
      double NormForce, double NormVelocity, double NormStress );
    virtual ~FDEMStorage();
    
    //return the class name
    virtual char* GetName();
    
    //returns if the Object has any data to show to the screen
    virtual bool GetHasData();
    
    //Add a FDEM binary block to the storage
    virtual void Add( double block[100] )=0;
    
    //add Properties
    virtual void AddProperties( double block[100] );
    
    virtual double UnNormalize( float value, double norm );
    
    //return a constructed vtkPolyData 
    virtual vtkPolyData* GetOutput()=0;
    
protected:
    double NormCoord;
    double NormForce;
    double NormVelocity;
    double NormStress;

    TriangleProperties *Properties;    
    
    bool HasData;
    char* Name;
    vtkPoints *Points;
    vtkCellArray *Cells;
    vtkPolyData *Output;            
};
#endif