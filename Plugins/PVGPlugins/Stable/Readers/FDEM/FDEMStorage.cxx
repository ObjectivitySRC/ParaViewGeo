//Abstract class that is the parent for all the FDEM classes
//defines the simple destructor and GetName methods
#include "FDEMStorage.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"


// --------------------------------------  
FDEMStorage::FDEMStorage( )
  {
  this->HasData = false;  
  this->Name = NULL;  
    
  this->Properties = new TriangleProperties();
  
  this->Points = vtkPoints::New();
  this->Cells = vtkCellArray::New();
  this->Output = vtkPolyData::New(); 
  
  this->NormCoord = 1.0;
  this->NormForce = 1.0;
  this->NormVelocity = 1.0;
  this->NormStress = 1.0;
  
  }      

FDEMStorage::FDEMStorage(char *Name, double NormCoord, double NormForce, double NormVelocity, double NormStress )
  {
  this->HasData = false;  
  this->Name = Name;  
    
  this->Properties = new TriangleProperties();
  
  this->Points = vtkPoints::New();
  this->Cells = vtkCellArray::New();
  this->Output = vtkPolyData::New(); 
  
  this->NormCoord = NormCoord;
  this->NormForce = NormForce;
  this->NormVelocity = NormVelocity;
  this->NormStress = NormStress;  
  } 
        
  
  
// --------------------------------------  
FDEMStorage::~FDEMStorage( )
  {
  delete this->Properties;
  this->Points->Delete();
  this->Cells->Delete();
  this->Output->Delete();
  this->Name=NULL;
  }    
  
// --------------------------------------    
char* FDEMStorage::GetName()
  {
  return this->Name;
  
  }

// --------------------------------------    
bool FDEMStorage::GetHasData()
  {
  return this->HasData;
  
  }  
  
void FDEMStorage::AddProperties( double block[100] )
  {
  
  //set this->Properties for a triangle block      
  const int stressSize = 3;
  double v[2], stress[stressSize], vel, damage;

  v[0] = this->UnNormalize( (float) block[9], this->NormVelocity );
  v[1] = this->UnNormalize( (float) block[10], this->NormVelocity );
  for (int i=0; i < stressSize ; i++)
    {    
    stress[i]= this->UnNormalize( (float) block[11+i] , this->NormStress );            
    }      
  
  damage= this->UnNormalize( (float) block[14] ,this->NormForce ); 
  
  vel = sqrt( v[0]*v[0]+v[1]*v[1] );
    
  //add elements to the 7 property arrays 
  this->Properties->AddSXX( stress[0] );
	this->Properties->AddSYY( stress[1] );
  this->Properties->AddSXY( stress[2] );
  
  
  this->Properties->AddVEL( vel );
  this->Properties->AddVEX( v[0] );
  this->Properties->AddVEY( v[1] );
  
  this->Properties->AddDamage( damage );
  
  }


// --------------------------------------  
double FDEMStorage::UnNormalize( float value, double norm )
  {
  //cast the float to a double, then multiple by the normalization factor
  return ( ( double ) value ) * norm;    
  }