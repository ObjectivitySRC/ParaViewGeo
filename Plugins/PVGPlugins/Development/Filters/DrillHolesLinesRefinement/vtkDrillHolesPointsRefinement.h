/*=========================================================================

   Program:   Visualization Toolkit
  Module:    $ vtkDrillHolesPointsRefinement.h $
  Author:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:      
  Version:   0.1
=========================================================================*/



// .name vtkDrillHolesPointsRefinement - a source to test a multiBlock DataSet object.
// .section description
// vtkDrillHolesPointsRefinement is a collection of rectilinear grids but can be another structure. 
//each block has a different origin and spacing.




#ifndef __vtkDrillHolesPointsRefinement_h
#define __vtkDrillHolesPointsRefinement_h

#include "vtkCompositeDataSetAlgorithm.h"
#include "vtkPolyData.h"

class vtkIntArray;
class vtkUniformGrid;
class vtkRectilinearGrid;
class vtkDataSet;
class vtkHierarchicalBoxDataSet;

class VTK_EXPORT vtkDrillHolesPointsRefinement : public vtkCompositeDataSetAlgorithm
{
public:
  static vtkDrillHolesPointsRefinement *New();

  vtkTypeRevisionMacro(vtkDrillHolesPointsRefinement,vtkCompositeDataSetAlgorithm);
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

   
  // Description:
  // Controls when a leaf gets subdivided. If the number of points inside the block is greater or equal than this value, then we decide to refine.
  //another option is to test a number of different drillHoles inside the block.
  // defaults to 2.
  vtkSetMacro(SpanThreshold, double);
  vtkGetMacro(SpanThreshold, double);

protected:
  vtkDrillHolesPointsRefinement();
  ~vtkDrillHolesPointsRefinement();

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
 
  
  // Dimensions:
  // Specify blocks relative to this top level block.
  // For now this has to be set before the blocks are defined.
  vtkSetVector3Macro(TopLevelSpacing, double);
  vtkGetVector3Macro(TopLevelSpacing, double);
  vtkSetVector3Macro(TopLevelOrigin, double);
  vtkGetVector3Macro(TopLevelOrigin, double);
  

  int MaximumLevel;
  int MinimumLevel;
  int Dimensions;
  vtkIntArray *Levels;
  double SpanThreshold;
  int MakeBounds;//this property give an option to make a bounding box on the drillHoles or not
  double OriginCx;
  double OriginCy;
  double OriginCz;
  double SizeCx;
  double SizeCy;
  double SizeCz;


  // New method of specifing blocks.
  double TopLevelSpacing[3];
  double TopLevelOrigin[3];
  

private:
  vtkDrillHolesPointsRefinement(const vtkDrillHolesPointsRefinement&);  // Not implemented.
  void operator=(const vtkDrillHolesPointsRefinement&);  // Not implemented.
};


#endif
