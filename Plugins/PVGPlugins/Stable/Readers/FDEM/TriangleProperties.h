// .NAME TriangleProperties
// .SECTION Description
// TriangleProperties is a subclass of vtkObject
// The purpose of this class is a basic data storage

#ifndef __TriangleProperties_h
#define __TriangleProperties_h

class vtkCellData;
class vtkDoubleArray;

class TriangleProperties
{
  public:     
    void AddSXX( float num );
    void AddSXY( float num );
    void AddSYY( float num );
    void AddVEL( float num );
    void AddVEX( float num );
    void AddVEY( float num );
    void AddDamage( float num );
    
    void PushToObject( vtkCellData *data );   
      
    TriangleProperties();
    ~TriangleProperties();     
    
  private:      
    vtkDoubleArray *sxx;
    vtkDoubleArray *sxy;
    vtkDoubleArray *syy;
    vtkDoubleArray *vel;
    vtkDoubleArray *vex;
    vtkDoubleArray *vey;
    vtkDoubleArray *damage;   
        
  };
#endif