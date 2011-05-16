  
#include "FDEMFill.h"
#include "TriangleProperties.h"

#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"

#define ZLEVEL 0.00001


// --------------------------------------  
FDEMFill::FDEMFill(char *Name, double NormCoord, double NormForce, 
    double NormVelocity, double NormStress ) : FDEMStorage(Name,NormCoord,NormForce, 
    NormVelocity,NormStress )
  {  
  }     


// --------------------------------------  
void FDEMFill::Add( double block[100] )
  {
  this->HasData = true;
  const vtkIdType size = 3; //points in a triangle
  vtkIdType *Id = new vtkIdType[size] ;
  
  double x[size], y[size], z=0; //2nd image, dont need more than 1 z
  
  for(int i=0; i<size; i++)
    { 
    x[i] = this->UnNormalize( (float)block[3+i], this->NormCoord );
    y[i] = this->UnNormalize( (float)block[6+i], this->NormCoord );    
    Id[i] = this->Points->InsertNextPoint(x[i],y[i],z);   
    }
  this->Cells->InsertNextCell( size , Id);
  
  this->AddProperties( block );
            
  delete[] Id;        
  }
  
// --------------------------------------             
vtkPolyData* FDEMFill::GetOutput()
  {
  
  this->Output->SetPoints( this->Points );
  this->Output->SetPolys( this->Cells );
  
  this->Properties->PushToObject( this->Output->GetCellData() );
  return this->Output;
  }
  
  
#undef ZLEVL