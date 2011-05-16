#include "TriangleProperties.h"

#include "vtkDoubleArray.h"
#include "vtkCellData.h"


// --------------------------------------  
TriangleProperties::TriangleProperties()
  {  
  
  //create and name each property
  this->sxx = vtkDoubleArray::New();
  this->sxx->SetName("SXX");
  
  this->sxy = vtkDoubleArray::New();
  this->sxy->SetName("SXY");
  
  this->syy = vtkDoubleArray::New();
  this->syy->SetName("SYY");
  
  this->vel = vtkDoubleArray::New();
  this->vel->SetName("VEL");
  
  this->vex = vtkDoubleArray::New();
  this->vex->SetName("VEX");
  
  this->vey= vtkDoubleArray::New();
  this->vey->SetName("VEY");
  
  this->damage = vtkDoubleArray::New();
  this->damage->SetName("Damage");
  
  }
// --------------------------------------  
TriangleProperties::~TriangleProperties()
  {
  this->sxx->Delete();    
  this->sxy->Delete();  
  this->syy->Delete();  
  this->vel->Delete();  
  this->vex->Delete();  
  this->vey->Delete();  
  this->damage->Delete();  
  }

// -------------------------------------- 
void TriangleProperties::PushToObject( vtkCellData *data ) 
  {    
  if (this->sxx->GetNumberOfTuples() > 0)
    data->AddArray(this->sxx); 
       
  if (this->sxy->GetNumberOfTuples() > 0)
    data->AddArray(this->sxy);
    
  if (this->syy->GetNumberOfTuples() > 0)     
    data->AddArray(this->syy);
    
  if (this->vel->GetNumberOfTuples() > 0)
    data->SetScalars(this->vel);  
    
  if (this->vex->GetNumberOfTuples() > 0)
    data->AddArray(this->vex);
    
  if (this->vey->GetNumberOfTuples() > 0)
    data->AddArray(this->vey);
    
  if (this->damage->GetNumberOfTuples() > 0)        
    data->AddArray(this->damage);    
  }
// --------------------------------------  
void TriangleProperties::AddSXX( float num )
  {
  this->sxx->InsertNextValue(num);
  }

// --------------------------------------
void TriangleProperties::AddSXY( float num )
  {
  this->sxy->InsertNextValue(num);
  }
  
// --------------------------------------  
void TriangleProperties::AddSYY( float num )
  {
  this->syy->InsertNextValue(num);
  }
  
// --------------------------------------  
void TriangleProperties::AddVEL( float num )
  {
  this->vel->InsertNextValue(num);
  }
  
// --------------------------------------  
void TriangleProperties::AddVEX( float num )
  {
  this->vex->InsertNextValue(num);
  }
  
// --------------------------------------  
void TriangleProperties::AddVEY( float num )
  {
  this->vey->InsertNextValue(num);
  }  

// --------------------------------------  
void TriangleProperties::AddDamage( float num )
  {
  this->damage->InsertNextValue(num);
  }
