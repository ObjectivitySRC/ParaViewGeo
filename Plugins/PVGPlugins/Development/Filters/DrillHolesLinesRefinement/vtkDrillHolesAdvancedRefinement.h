/*=========================================================================

   Program:   Visualization Toolkit
  Module:    $ vtkDrillHolesAdvancedRefinement.h $
  Author:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:      
  Version:   0.1
=========================================================================*/



// .name vtkDrillHolesAdvancedRefinement - a source to test a multiBlock DataSet object.
// .section description
// vtkDrillHolesAdvancedRefinement is a collection of rectilinear grids but can be another structure. 
//each block has a different origin and spacing.




#ifndef __vtkDrillHolesAdvancedRefinement_h
#define __vtkDrillHolesAdvancedRefinement_h

#include "vtkCompositeDataSetAlgorithm.h"
#include "vtkPolyData.h"

class vtkIntArray;
class vtkUniformGrid;
class vtkRectilinearGrid;
class vtkDataSet;
class vtkHierarchicalBoxDataSet;

class VTK_EXPORT vtkDrillHolesAdvancedRefinement : public vtkCompositeDataSetAlgorithm
{
public:
  static vtkDrillHolesAdvancedRefinement *New();

  vtkTypeRevisionMacro(vtkDrillHolesAdvancedRefinement,vtkCompositeDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);


  vtkSetMacro(TypeOfRefinement, int);
  vtkGetMacro(TypeOfRefinement, int);
  // Description:
  // Any blocks touching a predefined line will be subdivided to this level.
  // Other blocks are subdivided so that neighboring blocks only differ
  // by one level.
  vtkSetMacro(MaximumLevel, int);
  vtkGetMacro(MaximumLevel, int);

  vtkSetMacro(MinimumLevel, int);
  vtkGetMacro(MinimumLevel, int);

  // Description:
  // XYZ dimensions of cells.
  vtkSetMacro(Dimensions, int);
  vtkGetMacro(Dimensions, int);

  vtkSetMacro(MakeBounds, int);
  vtkGetMacro(MakeBounds, int);

  //vtkSetVector3Macro(OriginCX, double);
  //vtkGetVector3Macro(OriginCX, double);
  vtkSetMacro(OriginCx, double);
  vtkGetMacro(OriginCx, double);

  vtkSetMacro(OriginCy, double);
  vtkGetMacro(OriginCy, double);

  vtkSetMacro(OriginCz, double);
  vtkGetMacro(OriginCz, double);

  vtkSetMacro(SizeCx, double);
  vtkGetMacro(SizeCx, double);

  vtkSetMacro(SizeCy, double);
  vtkGetMacro(SizeCy, double);

  vtkSetMacro(SizeCz, double);
  vtkGetMacro(SizeCz, double);

 // vtkSetVector3Macro(SizeCX, double);
 // vtkGetVector3Macro(SizeCX, double);

  // Description:
  // Set the number of sides for the tube. At a minimum, number of sides is 3.
  vtkSetClampMacro(NumberOfSides,int,3,VTK_LARGE_INTEGER);
  vtkGetMacro(NumberOfSides,int);

  // Description:
  // Turn on/off whether to cap the ends with polygons.
  vtkSetMacro(Capping,int);
  vtkGetMacro(Capping,int);
  vtkBooleanMacro(Capping,int);

  vtkSetMacro(Distance,double);
  vtkGetMacro(Distance,double);

  vtkSetMacro(TypeOfInterpolation,int);
  vtkGetMacro(TypeOfInterpolation,int);

  // Description:
  // Set the minimum tube radius (minimum because the tube radius may vary).
  vtkSetClampMacro(Radius,double,0.0,VTK_DOUBLE_MAX);
  vtkGetMacro(Radius,double);


  
protected:
  vtkDrillHolesAdvancedRefinement();
  ~vtkDrillHolesAdvancedRefinement();

  int StartBlock;
  int EndBlock;
  int BlockCount;
  //int TimeStep;
 // int TimeStepRange[2];

  // Convenience method.
  unsigned int AppedDataSetToLevel(vtkCompositeDataSet* composite,  unsigned int level, int extents[6], vtkDataSet* ds);

  // Create a vtkMultiBlockDataSet 
  virtual int RequestDataObject(vtkInformation *req,
    vtkInformationVector **inV,
    vtkInformationVector *outV);

  // Description:
  // This is called by the superclass.
  // This is the method you should override.
  virtual int RequestInformation(vtkInformation *request, 
                                 vtkInformationVector **inputVector, 
                                 vtkInformationVector *outputVector);

  // Description:
  // This is called by the superclass.
  // This is the method you should override.
  virtual int RequestData(vtkInformation *, 
                          vtkInformationVector **, 
                          vtkInformationVector *);

  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info); 
  
  virtual float DistancePointToLine(double P[3], double A[3], double B[3]);
  virtual float DistancePointToLine2(double point[3], int HoleId,vtkCellArray* CellsInput1,
						   vtkDataArray* LinesInsideId1,
						   vtkDataArray* PointCoordsInput1);

  virtual int LineIntersectBoxTest(double x0, double y0, double z0,
                             double x1, double y1, double z1, 
                             double bds[6]);
  void RefineBlock(int &blockId, int level, vtkCompositeDataSet* output,
                int x0,int x1, int y0,int y1, int z0,int z1,
                int onFace[6],int id);
 
 void SetRBlockInfo(vtkRectilinearGrid *grid, int level, int* ext,
                     int onFace[6],int HoleId);

 int subdivideTest( double bds[6], int level) ;
  
  void CellExtentToBounds(int level,
                          int ext[6],
                          double bds[6]);
 
  void GetContinuousIncrements(int extent[6],
                               vtkIdType &incX,
                               vtkIdType &incY,
                               vtkIdType &incZ);
  
  // Dimensions:
  // Specify blocks relative to this top level block.
  // For now this has to be set before the blocks are defined.
  vtkSetVector3Macro(TopLevelSpacing, double);
  vtkGetVector3Macro(TopLevelSpacing, double);
  vtkSetVector3Macro(TopLevelOrigin, double);
  vtkGetVector3Macro(TopLevelOrigin, double);
  
  void InternalImageDataCopy(vtkDrillHolesAdvancedRefinement *src);


 
  int MaximumLevel;
  int MinimumLevel;
  int Dimensions;
  vtkIntArray *Levels;
  int MakeBounds;//this property give an option to make a bounding box on the drillHoles or not
  int NumberOfSides;
  int Capping;
  double Radius;
  int TypeOfRefinement;
  int TypeOfInterpolation;
  double Distance;
  double OriginCx;
  double OriginCy;
  double OriginCz;
  double SizeCx;
  double SizeCy;
  double SizeCz;

  vtkCellArray* CellsInput;
  vtkDataArray* PointCoordsInput;
  vtkDataArray* LinesInsideId;



  // New method of specifing blocks.
  double TopLevelSpacing[3];
  double TopLevelOrigin[3];
  

private:
  vtkDrillHolesAdvancedRefinement(const vtkDrillHolesAdvancedRefinement&);  // Not implemented.
  void operator=(const vtkDrillHolesAdvancedRefinement&);  // Not implemented.
};


#endif
