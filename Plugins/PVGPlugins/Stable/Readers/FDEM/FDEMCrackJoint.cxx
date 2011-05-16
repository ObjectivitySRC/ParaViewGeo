  
#include "FDEMCrackJoint.h"

#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

#define ZLEVEL 0.00001

#define MODE_CRACK 0
#define MODE_JOINT 1
// --------------------------------------  
FDEMCrackJoint::FDEMCrackJoint( char* Name, int mode, double NormCoord)
  {
  this->HasData = false;
  this->Mode = mode;
  this->Name = Name;
           
  this->NormCoord = NormCoord;  
  
  }  
    
// --------------------------------------  
void FDEMCrackJoint::Add( double block[100] )
  {
  const vtkIdType size = 4; //joints have 4
  vtkIdType *Id = new vtkIdType[size] ;
  
  float x[size], y[size], z=ZLEVEL; //2nd image, dont need more than 1 z
  float damage;
  
  //read in the info for the cracks / joints 
  for(int i=0; i<size; i++)
    { 
    x[i] =(float)block[3+i] * this->NormCoord;
    y[i] =(float)block[7+i] * this->NormCoord;    
    }
    
  //grab the damage entry for this crack or joint
  damage = (float) block[11];  
  if ( damage < 0.0 )
    {
    //change null values to -1
    damage = -1;
    }
  else if ( damage > 0.0)
    {
    //calcualte out the damage, and make sure it falls in the range
    //of -1 to 1
    damage = ( (float)block[11] + (float)block[13] ) * 0.5;
    if (damage > 1.0)
      {
      damage = 1.0;
      }
    else if (damage < -1.0)
      {
      damage = -1.0;
      }
    }
  
  //set the points and cells
  
  if (  ( damage < 0.0 && this->Mode == MODE_CRACK ) || ( damage > 0.0 && this->Mode >= MODE_JOINT  )  )
    {
    this->HasData = true; //only place we really know the block is damaged
    for (int i=0; i < size; i++)
      {
      Id[i] = this->Points->InsertNextPoint(x[i], y[i], z);
      }
    for (int i=0; i < 4; i+=2)
      {
      this->Cells->InsertNextCell(2); //drawing lines
      this->Cells->InsertCellPoint(Id[i]);
      this->Cells->InsertCellPoint(Id[i+1]);        
      this->Properties->AddDamage( damage );
      }    
    } 
  }
// --------------------------------------             
void FDEMCrackJoint::AddProperties( double block[100] )
{
  //not used in this class

}
  
// --------------------------------------             
vtkPolyData* FDEMCrackJoint::GetOutput()
  {
  
  this->Output->SetPoints( this->Points );
  this->Output->SetLines( this->Cells );
  
  this->Properties->PushToObject( this->Output->GetCellData() );
  return this->Output;
  }
  
  
#undef ZLEVL