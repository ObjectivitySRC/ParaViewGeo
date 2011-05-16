#include "FDEMPoint.h"
#include "TriangleProperties.h"

#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"

#define ZLEVEL 0.00001

// --------------------------------------  
FDEMPoint::FDEMPoint(char *Name, double NormCoord, double NormForce, 
    double NormVelocity, double NormStress ) : FDEMStorage(Name,NormCoord,NormForce, 
    NormVelocity,NormStress )
  {  
  }     

// --------------------------------------  
void FDEMPoint::Add( double block[100] )
  {
  
  this->HasData = true;
  const vtkIdType size = 3; //points in a triangle
  vtkIdType *pointId = new vtkIdType();
  
  double  x[size], y[size], z=ZLEVEL; //small raise above
  
  for(int i=0; i<size; i++)
    { 
    x[i] = this->UnNormalize( (float)block[3+i], this->NormCoord );
    y[i] = this->UnNormalize( (float)block[6+i], this->NormCoord );    
    }
      
  //in this case we create a seperate point and place 'above' the triangle, so it 
  //is easily seen by the user    
  
  x[0] = (x[0] + x[1] + x[2]) / 3.0;
  y[0] = (y[0] + y[1] + y[2]) / 3.0;   
  
  *pointId = this->Points->InsertNextPoint(x[0],y[0],z);
  this->Cells->InsertNextCell(1,pointId);
                        
  this->AddProperties( block );
          
  delete pointId;         
  }
  
// --------------------------------------             
vtkPolyData* FDEMPoint::GetOutput()
  {
  
  this->Output->SetPoints( this->Points );
  this->Output->SetVerts( this->Cells );
  
  this->Properties->PushToObject( this->Output->GetCellData() );
  return this->Output;
  }
  
#undef ZLEVL  