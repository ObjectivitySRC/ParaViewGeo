/*=========================================================================

   Program:   Visualization Toolkit
  Module:    $ vtkDrillHolesBoundingBoxRefinement.h $
  Author:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:      
  Version:   0.1
=========================================================================*/



// .name vtkDrillHolesBoundingBoxRefinement - a source to test a multiBlock DataSet object.
// .section description
// vtkDrillHolesBoundingBoxRefinement is a collection of rectilinear grids but can be another structure. 
//each block has a different origin and spacing.




#ifndef __vtkDrillHolesBoundingBoxRefinement_h
#define __vtkDrillHolesBoundingBoxRefinement_h

#include "vtkCompositeDataSetAlgorithm.h"
#include "vtkPolyData.h"

class vtkIntArray;
class vtkUniformGrid;
class vtkRectilinearGrid;
class vtkDataSet;
class vtkHierarchicalBoxDataSet;

class VTK_EXPORT vtkDrillHolesBoundingBoxRefinement : public vtkCompositeDataSetAlgorithm
{
public:
  static vtkDrillHolesBoundingBoxRefinement *New();

  vtkTypeRevisionMacro(vtkDrillHolesBoundingBoxRefinement,vtkCompositeDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

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

  vtkSetVector3Macro(OriginCX, double);
  vtkGetVector3Macro(OriginCX, double);

  vtkSetVector3Macro(SizeCX, double);
  vtkGetVector3Macro(SizeCX, double);

  // Description:
  // Set the number of sides for the tube. At a minimum, number of sides is 3.
  vtkSetClampMacro(NumberOfSides,int,3,VTK_LARGE_INTEGER);
  vtkGetMacro(NumberOfSides,int);

  // Description:
  // Turn on/off whether to cap the ends with polygons.
  vtkSetMacro(Capping,int);
  vtkGetMacro(Capping,int);
  vtkBooleanMacro(Capping,int);


  // Description:
  // Set the minimum tube radius (minimum because the tube radius may vary).
  vtkSetClampMacro(Radius,double,0.0,VTK_DOUBLE_MAX);
  vtkGetMacro(Radius,double);


  
protected:
  vtkDrillHolesBoundingBoxRefinement();
  ~vtkDrillHolesBoundingBoxRefinement();

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
  
  void RefineBlock(int &blockId, int level, vtkCompositeDataSet* output, vtkPolyData* input,
                int x0,int x1, int y0,int y1, int z0,int z1,
                int onFace[6]);
 
 void SetRBlockInfo(vtkRectilinearGrid *grid, int level, int* ext,
                     int onFace[6]);

 int subdivideTest(vtkPolyData* input, double bds[6], int level) ;
  
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
  
  void InternalImageDataCopy(vtkDrillHolesBoundingBoxRefinement *src);


  int Asymetric;
  int MaximumLevel;
  int MinimumLevel;
  int Dimensions;
  vtkIntArray *Levels;
  int MakeBounds;//this property give an option to make a bounding box on the drillHoles or not
  int NumberOfSides;
  int Capping;
  double Radius;


  //constant/initial-value at origin.
  double OriginCX[3];

  // A temporary vector that is computed as needed.
  // It is used to return a vector.
  double SizeCX[3];


  // New method of specifing blocks.
  double TopLevelSpacing[3];
  double TopLevelOrigin[3];
  

private:
  vtkDrillHolesBoundingBoxRefinement(const vtkDrillHolesBoundingBoxRefinement&);  // Not implemented.
  void operator=(const vtkDrillHolesBoundingBoxRefinement&);  // Not implemented.
};


#endif
