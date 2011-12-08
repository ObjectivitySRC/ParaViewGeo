/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAMRDualContour.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAMRDualContour - Extract particles and analyse them.
// .SECTION Description
// This filter takes a cell data volume fraction and generates a polydata
// surface.  It also performs connectivity on the particles and generates
// a particle index as part of the cell data of the output.  It computes
// the volume of each particle from the volume fraction.

// This will turn on validation and debug i/o of the filter.
//#define vtkAMRDualContourDEBUG
#define vtkAMRDualContourPROFILE

#ifndef __vtkAMRDualContour_h
#define __vtkAMRDualContour_h

#include "vtkMultiBlockDataSetAlgorithm.h"
#include <vtkstd/vector>
#include <vtkstd/string>

class vtkDataSet;
class vtkImageData;
class vtkPolyData;
class vtkHierarchicalBoxDataSet;
class vtkPoints;
class vtkDoubleArray;
class vtkCellArray;
class vtkCellData;
class vtkIntArray;
class vtkFloatArray;
class vtkMultiProcessController;
class vtkDataArraySelection;
class vtkCallbackCommand;

class vtkAMRDualGridHelper;
class vtkAMRDualGridHelperBlock;
class vtkAMRDualGridHelperFace;
class vtkAMRDualContourEdgeLocator;


class VTK_EXPORT vtkAMRDualContour : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkAMRDualContour *New();
  vtkTypeMacro(vtkAMRDualContour,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(IsoValue, double);
  vtkGetMacro(IsoValue, double);

  // Description:
  // These are to evaluate performances. You can turn off capping, degenerate cells
  // and multiprocess comunication to see how they affect speed of execution.
  // Degenerate cells is the meshing between levels in the grid.
  vtkSetMacro(EnableCapping,int);
  vtkGetMacro(EnableCapping,int);
  vtkBooleanMacro(EnableCapping,int);
  vtkSetMacro(EnableDegenerateCells,int);
  vtkGetMacro(EnableDegenerateCells,int);
  vtkBooleanMacro(EnableDegenerateCells,int);
  vtkSetMacro(EnableMultiProcessCommunication,int);
  vtkGetMacro(EnableMultiProcessCommunication,int);
  vtkBooleanMacro(EnableMultiProcessCommunication,int);

  // Description:
  // This flag causes blocks to share locators so there are no
  // boundary edges between blocks. It does not eliminate
  // boundary edges between processes.
  vtkSetMacro(EnableMergePoints,int);
  vtkGetMacro(EnableMergePoints,int);
  vtkBooleanMacro(EnableMergePoints,int);

  // Description:
  // A flag that causes the polygons on the capping surfaces to be triagulated.
  vtkSetMacro(TriangulateCap,int);
  vtkGetMacro(TriangulateCap,int);
  vtkBooleanMacro(TriangulateCap,int);

  // Description:
  // An option to turn off copying ghost values across process boundaries.
  // If the ghost values are already correct, then the extra communication is
  // not necessary.  If this assumption is wrong, this option will produce
  // cracks / seams.
  vtkSetMacro(SkipGhostCopy,int);
  vtkGetMacro(SkipGhostCopy,int);
  vtkBooleanMacro(SkipGhostCopy,int);

  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  virtual void SetController(vtkMultiProcessController *);

protected:
  vtkAMRDualContour();
  ~vtkAMRDualContour();

  double IsoValue;

  // Algorithm options that may improve performance.
  int EnableDegenerateCells;
  int EnableCapping;
  int EnableMultiProcessCommunication;
  int EnableMergePoints;
  int TriangulateCap;
  int SkipGhostCopy;

  //BTX
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  // Description:
  // Not a pipeline function. This is a helper function that
  // allows creating a new data set given a input and a cell array name.
  vtkMultiBlockDataSet* DoRequestData(vtkHierarchicalBoxDataSet* input,
                                          const char* arrayNameToProcess);

  virtual int FillInputPortInformation(int port, vtkInformation *info);
  virtual int FillOutputPortInformation(int port, vtkInformation *info);

  void ShareBlockLocatorWithNeighbors(
    vtkAMRDualGridHelperBlock* block);

  void ProcessBlock(vtkAMRDualGridHelperBlock* block, int blockId,
                    const char* arrayName);


  void ProcessDualCell(
    vtkAMRDualGridHelperBlock* block, int blockId,
    int x, int y, int z,
    vtkIdType cornerOffsets[8],
    vtkDataArray *volumeFractionArray);

  void AddCapPolygon(int ptCount, vtkIdType* pointIds, int blockId);

  // This method is getting too many arguements!
  // Capping was an after thought...
  void CapCell(
    int cellX, int cellY, int cellZ,  // block coordinates
    // Which cell faces need to be capped.
    unsigned char cubeBoundaryBits,
    // Marching cubes case for this cell
    int cubeCase,
    // Ids of the point created on edges for the internal surface
    vtkIdType edgePtIds[12],
    // Locations of 8 corners. (xyz4xyz4...) 4th value is not used.
    double cornerPoints[32],
    // The id order is VTK from marching cube cases.  Different than axis orded "cornerPoints".
    vtkIdType cornerOffsets[8],
    // For block id array (for debugging).  I should just make this an ivar.
    int blockId,
    // For passing attirbutes to output mesh
    vtkDataSet* inData);

  // Stuff exclusively for debugging.
  vtkIntArray* BlockIdCellArray;
  vtkFloatArray* TemperatureArray;

  // Ivars used to reduce method parrameters.
  vtkAMRDualGridHelper* Helper;
  vtkPolyData* Mesh;
  vtkPoints* Points;
  vtkCellArray* Faces;

  vtkMultiProcessController *Controller;

  // I made these ivars to avoid allocating multiple times.
  // The buffer is not used too many times, but ...
  int* MessageBuffer;
  int* MessageBufferLength;

  vtkAMRDualContourEdgeLocator* BlockLocator;

  // Stuff for passing cell attributes to point attributes.
  void InitializeCopyAttributes(
    vtkHierarchicalBoxDataSet *hbdsInput,
    vtkDataSet* mesh);
  void InterpolateAttributes(
    vtkDataSet* uGrid, vtkIdType offset0, vtkIdType offset1, double k,
    vtkDataSet* mesh, vtkIdType outId);
  void CopyAttributes(
    vtkDataSet* uGrid, vtkIdType inId,
    vtkDataSet* mesh, vtkIdType outId);
  void FinalizeCopyAttributes(vtkDataSet* mesh);

private:
  vtkAMRDualContour(const vtkAMRDualContour&);  // Not implemented.
  void operator=(const vtkAMRDualContour&);  // Not implemented.
  //ETX
};

#endif
