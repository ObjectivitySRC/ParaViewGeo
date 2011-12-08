/*=========================================================================

  Program:   ParaView
  Module:    vtkHierarchicalFractal.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkHierarchicalFractal.h"

#include "vtkAMRBox.h"
#include "vtkCellData.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkDoubleArray.h"
#include "vtkExtractCTHPart.h" // for the BOUNDS key
#include "vtkHierarchicalBoxDataSet.h"
#include "vtkImageMandelbrotSource.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkSmartPointer.h"
#include "vtkUniformGrid.h"
#include "vtkUnsignedCharArray.h"

#include <assert.h>

vtkStandardNewMacro(vtkHierarchicalFractal);

//----------------------------------------------------------------------------
vtkHierarchicalFractal::vtkHierarchicalFractal()
{
  this->SetNumberOfInputPorts(0);
  
  this->Dimensions = 10;
  this->FractalValue = 9.5;
  this->MaximumLevel = 6;
  this->GhostLevels = 0;

  this->Levels = vtkIntArray::New();
  this->TwoDimensional = 1;
  this->Asymetric = 1;
  this->Overlap = 1;
  
  this->TopLevelSpacing[0] = 1.0;
  this->TopLevelSpacing[1] = 1.0;
  this->TopLevelSpacing[2] = 1.0;

  this->TopLevelOrigin[0] = 0.0;
  this->TopLevelOrigin[1] = 0.0;
  this->TopLevelOrigin[2] = 0.0;
  
  this->GenerateRectilinearGrids=0;

  this->TimeStep = 0;
  this->TimeStepRange[0] = 0;
  this->TimeStepRange[1] = 10;
}

//----------------------------------------------------------------------------
vtkHierarchicalFractal::~vtkHierarchicalFractal()
{
  this->Levels->Delete();
  this->Levels = NULL;
}

//----------------------------------------------------------------------------
int vtkHierarchicalFractal::RequestDataObject(vtkInformation *,
                                              vtkInformationVector **,
                                              vtkInformationVector *outV)
{
  vtkInformation *outInfo = outV->GetInformationObject(0);
  vtkCompositeDataSet* outData = NULL;

  if (this->GenerateRectilinearGrids)
    {
    outData = vtkMultiBlockDataSet::New();
    }
  else
    {
    outData = vtkHierarchicalBoxDataSet::New();
    }

  outData->SetPipelineInformation(outInfo);
  outInfo->Set(vtkDataObject::DATA_EXTENT_TYPE(), outData->GetExtentType());
  outInfo->Set(vtkDataObject::DATA_OBJECT(), outData);
  outData->Delete();
  return 1;
}

//----------------------------------------------------------------------------
// This handles any alterations necessary for ghost levels.
void vtkHierarchicalFractal::SetBlockInfo(vtkUniformGrid *grid,
                                          int level, 
                                          int *ext,
                                          int onFace[6])
{
  // onFace[0]:xmin
  // onFace[1]:xmax
  // onFace[2]:ymin
  // onFace[3]:ymax
  // onFace[4]:zmin
  // onFace[5]:zmax
  
  if (this->GhostLevels)
    {
    if(!onFace[0])
      {
      ext[0] -= 1;
      }
    if(!onFace[2])
      {
      ext[2] -= 1;
      }
    if(!onFace[4])
      {
      ext[4] -= 1;
      }
    if(!onFace[1])
      {
      ext[1] += 1;
      }
    if(!onFace[3])
      {
      ext[3] += 1;
      }
    if(!onFace[5])
      {
      ext[5] += 1;
      }
    }
  if (this->TwoDimensional)
    {
    ext[4] = ext[5] = 0;
    }

  double bds[6];
  double origin[3];
  double spacing[3];
  
  this->CellExtentToBounds(level, ext, bds);
  origin[0] = bds[0];
  origin[1] = bds[2];
  origin[2] = bds[4];
  spacing[0] = (bds[1]-bds[0])/(double)(ext[1]-ext[0]+1);
  spacing[1] = (bds[3]-bds[2])/(double)(ext[3]-ext[2]+1);
  spacing[2] = (bds[5]-bds[4])/(double)(ext[5]-ext[4]+1);

  // Handle Lower dimensions.  Assume that cell dimension of 1 is a collapsed
  // dimension.  Point dim equal 1 also.
  int dim[3];
  
  dim[0] = dim[1] = dim[2] = 1;
  if (ext[1] > ext[0])
    {
    dim[0] = ext[1]-ext[0]+2;
    }
  if (ext[3] > ext[2])
    {
    dim[1] = ext[3]-ext[2]+2;
    }
  if (ext[5] > ext[4])
    {
    dim[2] = ext[5]-ext[4]+2;
    }
  
  grid->SetDimensions(dim);
  grid->SetSpacing(spacing);
  grid->SetOrigin(origin);
  
  if(this->GhostLevels>0)
    {
    this->AddGhostLevelArray(grid,dim,onFace);
    }
}

//----------------------------------------------------------------------------
// This handles any alterations necessary for ghost levels.
void vtkHierarchicalFractal::SetRBlockInfo(vtkRectilinearGrid *grid,
                                           int level, 
                                           int *ext,
                                           int onFace[6])
{
  if (this->GhostLevels)
    {
    if(!onFace[0])
      {
      ext[0] -= 1;
      }
    if(!onFace[2])
      {
      ext[2] -= 1;
      }
    if(!onFace[4])
      {
      ext[4] -= 1;
      }
    if(!onFace[1])
      {
      ext[1] += 1;
      }
    if(!onFace[3])
      {
      ext[3] += 1;
      }
    if(!onFace[5])
      {
      ext[5] += 1;
      }
    }
  if (this->TwoDimensional)
    {
    ext[4] = ext[5] = 0;
    }

  double bds[6];
  double origin[3];
  double spacing[3];
  
  this->CellExtentToBounds(level, ext, bds);
  origin[0] = bds[0];
  origin[1] = bds[2];
  origin[2] = bds[4];
  spacing[0] = (bds[1]-bds[0])/(double)(ext[1]-ext[0]+1);
  spacing[1] = (bds[3]-bds[2])/(double)(ext[3]-ext[2]+1);
  spacing[2] = (bds[5]-bds[4])/(double)(ext[5]-ext[4]+1);

  // Handle Lower dimensions.  Assume that cell dimension of 1 is a collapsed
  // dimension.  Point dim equal 1 also.
  int dim[3];
  
  dim[0] = dim[1] = dim[2] = 1;
  if (ext[1] > ext[0])
    {
    dim[0] = ext[1]-ext[0]+2;
    }
  if (ext[3] > ext[2])
    {
    dim[1] = ext[3]-ext[2]+2;
    }
  if (ext[5] > ext[4])
    {
    dim[2] = ext[5]-ext[4]+2;
    }
  
  grid->SetDimensions(dim);
  
  vtkDoubleArray *coords[3];
  
  vtkMath::RandomSeed(1234);
  int coord=0;
  while(coord<3)
    {
    coords[coord]=vtkDoubleArray::New();
    //grid->SetOrigin(origin);
    
    // first point
    coords[coord]->InsertNextValue(origin[coord]);
    
    double uniformCoordinate;
    
    int i=1;
    int c;
    if(this->GhostLevels && !onFace[coord*2+1])
      {
      c=dim[coord]-2;
      }
    else
      {
      c=dim[coord]-1;
      }
    uniformCoordinate=origin[coord];
    
    if(this->GhostLevels && !onFace[coord*2])
      {
      uniformCoordinate+=spacing[coord];
      coords[coord]->InsertNextValue(uniformCoordinate);
      ++i;
      }
    
    while(i<c)
      {
      uniformCoordinate+=spacing[coord];
      // get a random number about 1/5 of the uniform spacing.
      double epsilon=(vtkMath::Random()-0.5)*spacing[coord]*0.4;
      coords[coord]->InsertNextValue(uniformCoordinate+epsilon);
      ++i;
      }
    
    if(this->GhostLevels && !onFace[coord*2+1])
      {
      uniformCoordinate+=spacing[coord];
      coords[coord]->InsertNextValue(uniformCoordinate);
      ++i;
      }
    
    // last point 
    uniformCoordinate+=spacing[coord];
    coords[coord]->InsertNextValue(uniformCoordinate);
    
    ++coord;
    }
  
//  grid->SetSpacing(spacing);
  grid->SetXCoordinates(coords[0]);
  grid->SetYCoordinates(coords[1]);
  grid->SetZCoordinates(coords[2]);
  coord=0;
  while(coord<3)
    {
    coords[coord]->Delete();
    ++coord;
    }
  
  if(this->GhostLevels>0)
    {
    this->AddGhostLevelArray(grid,dim,onFace);
    }
}

//----------------------------------------------------------------------------
int vtkHierarchicalFractal::TwoDTest(double bds[6], int level, int target) 
{
  // Test the 4 corners.  Refine if the blocks cross the border.
  int v0, v1, v2, v3;
  
  if (level == target)
    {
    return 0;
    }
  
  if (level < 2)
    {
    return 1;
    }
  
  v0 = this->MandelbrotTest(bds[0], bds[2]);
  v1 = this->MandelbrotTest(bds[1], bds[2]);
  v2 = this->MandelbrotTest(bds[0], bds[3]);
  v3 = this->MandelbrotTest(bds[1], bds[3]);
  if (v0 && v1 && v2 && v3)
    {
    return 0;
    }
  if (!v0 && !v1 && !v2 && !v3)
    {
    return 0;
    }
  return 1;
}

int vtkHierarchicalFractal::MandelbrotTest(double x, double y)
{
  unsigned short count = 0;
  double v1;
  double cReal, cImag, zReal, zImag;
  double zReal2, zImag2;

  cReal = x;
  cImag = y;
  zReal = 0.0;
  zImag = this->TimeStep / 10.0;

  zReal2 = zReal * zReal;
  zImag2 = zImag * zImag;
  v1 = (zReal2 + zImag2);
  while ( v1 < 4.0 && count < 100)
    {
    zImag = 2.0 * zReal * zImag + cImag;
    zReal = zReal2 - zImag2 + cReal;
    zReal2 = zReal * zReal;
    zImag2 = zImag * zImag;
    ++count;
    v1 = (zReal2 + zImag2);
    }

  if (count == 100)
    {
    return 1;
    }

  return 0;
}


//----------------------------------------------------------------------------
// Description:
// This is called by the superclass.
// This is the method you should override.
int vtkHierarchicalFractal::RequestInformation(
  vtkInformation *request, 
  vtkInformationVector **inputVector, 
  vtkInformationVector *outputVector)
{
  if(!this->Superclass::RequestInformation(request,inputVector,outputVector))
    {
    return 0;
    }
  
  vtkInformation *info=outputVector->GetInformationObject(0);
  info->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
  return 1;
}


//----------------------------------------------------------------------------
// Description:
// This is called by the superclass.
// This is the method you should override.
int vtkHierarchicalFractal::RequestData(
  vtkInformation *vtkNotUsed(request), 
  vtkInformationVector **vtkNotUsed(inputVector), 
  vtkInformationVector *outputVector)
{
  vtkInformation *info=outputVector->GetInformationObject(0);
  vtkDataObject *doOutput=info->Get(vtkDataObject::DATA_OBJECT());
  vtkCompositeDataSet *output;
  output=vtkCompositeDataSet::SafeDownCast(doOutput);
  
  if(output==0)
    {
    vtkErrorMacro("The output is not a HierarchicalDataSet");
    return 0;
    }
  if (!info->Has(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()) ||
    !info->Has(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES()))
    {
    vtkErrorMacro("Expected information not found. "
                  "Cannot provide update extent.");
    return 0;
    }

  output->Initialize(); // remove all previous blocks
  
  
  // By setting SetMaximumNumberOfPieces(-1) 
  // then GetUpdateNumberOfPieces() should always return the number
  // of processors in the parallel job and GetUpdatePiece() should
  // return the specific process number
  int piece=info->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
  int numPieces =info->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());

  float ox = -1.75;
  float oy = -1.25;
  float oz = 0.0;
  float xSize = 2.5;
  float ySize = 2.5;
  float zSize = 2.0;
  int blockId = 0;

  // This is 10x10x10 in cells.
  this->SetTopLevelOrigin(ox, oy, oz);
  this->SetTopLevelSpacing(xSize/this->Dimensions,
                           ySize/this->Dimensions,
                           zSize/this->Dimensions);
  
  
  int ext[6];
  ext[0] = ext[2] = ext[4] = 0;
  ext[1] = ext[3] = ext[5] = this->Dimensions - 1;
  if (this->Asymetric)
    { // The changes to an extra 2 in the next level.
    ext[1] += 1;
    }

  // Get a global (across all processes) count of the blocks.
  // Do not create the blocks.
  this->StartBlock = 0;
  this->EndBlock = -1;
  this->BlockCount = 0;
  int onFace[6];
  onFace[0]=1;
  onFace[1]=1;
  onFace[2]=1;
  onFace[3]=1;
  onFace[4]=1;
  onFace[5]=1;
  this->Traverse(blockId, 0, output, ext[0], ext[1], ext[2], ext[3], ext[4],
                 ext[5],onFace);

  // Generate our share of the blocks.
  this->StartBlock = (int)((float)(piece*this->BlockCount)/(float)(numPieces));
  this->EndBlock = (int)((float)((piece+1)*this->BlockCount)/(float)(numPieces)) - 1;
  this->BlockCount = 0;

  this->Levels->Initialize();
  this->Traverse(blockId, 0, output, ext[0], ext[1], ext[2], ext[3], ext[4],
                 ext[5],onFace);
  
  
  double bounds[6];

  bounds[0]=ox;
  bounds[1]=ox+xSize;
  bounds[2]=oy;
  bounds[3]=oy+ySize;
  bounds[4]=oz;
  if(this->TwoDimensional)
    {
    bounds[5]=oz;
    }
  else
    {
    bounds[5]=oz+zSize;
    }
  
  info->Set(vtkExtractCTHPart::BOUNDS(),bounds,6);
  
  if(!this->GenerateRectilinearGrids)
    {
    this->AddVectorArray(output);
    this->AddTestArray(output);
    this->AddBlockIdArray(output);
    this->AddDepthArray(
      vtkHierarchicalBoxDataSet::SafeDownCast(output));
    }
  this->AddFractalArray(output);
  
  return 1;
}
  

//----------------------------------------------------------------------------
int vtkHierarchicalFractal::LineTest2(float x0, float y0, float z0,
                             float x1, float y1, float z1, 
                             double bds[6]) 
{
  // intersect line with plane.
  float x, y, z;
  float k;

  // Special case ane point is inside box.
  if (x0>bds[0] && x0<bds[1] && y0>bds[2] && y0<bds[3] && z0>bds[4]
      && z0<bds[5])
    {
    return 1;
    }
  if (x1>bds[0] && x1<bds[1] && y1>bds[2] && y1<bds[3] && z1>bds[4]
      && z1<bds[5])
    {
    return 1;
    }

  // Do not worry about divide by zero.
  // min x
  x = bds[0];
  k = (x- x0) / (x1-x0);
  if (k >=0.0 && k <= 1.0)
    {
    y = y0 + k*(y1-y0);
    z = z0 + k*(z1-z0);
    if (y >= bds[2] && y <= bds[3] && z >= bds[4] && z <= bds[5])
      {
      return 1;
      }
    } 
  // max x
  x = bds[1];
  k = (x- x0) / (x1-x0);
  if (k >=0.0 && k <= 1.0)
    {
    y = y0 + k*(y1-y0);
    z = z0 + k*(z1-z0);
    if (y >= bds[2] && y <= bds[3] && z >= bds[4] && z <= bds[5])
      {
      return 1;
      }
    } 
  // min y
  y = bds[2];
  k = (y- y0) / (y1-y0);
  if (k >=0.0 && k <= 1.0)
    {
    x = x0 + k*(x1-x0);
    z = z0 + k*(z1-z0);
    if (x >= bds[0] && x <= bds[1] && z >= bds[4] && z <= bds[5])
      {
      return 1;
      }
    } 
  // max y
  y = bds[3];
  k = (y- y0) / (y1-y0);
  if (k >=0.0 && k <= 1.0)
    {
    x = x0 + k*(x1-x0);
    z = z0 + k*(z1-z0);
    if (x >= bds[0] && x <= bds[1] && z >= bds[4] && z <= bds[5])
      {
      return 1;
      }
    } 
  // min z
  z = bds[4];
  k = (z- z0) / (z1-z0);
  if (k >=0.0 && k <= 1.0)
    {
    x = x0 + k*(x1-x0);
    y = y0 + k*(y1-y0);
    if (y >= bds[2] && y <= bds[3] && x >= bds[0] && x <= bds[1])
      {
      return 1;
      }
    } 

  return 0;
}

//----------------------------------------------------------------------------
int vtkHierarchicalFractal::LineTest(float x0, float y0, float z0, 
                            float x1, float y1, float z1,
                            double bds[6], int level, int target) 
{
  if (level >= target)
    {
    return 0;
    }
  // First check to see if the line intersects this block.
  if (this->LineTest2(x0, y0, z0, x1, y1, z1, bds))
    {
    return 1;
    }

  // If the line intersects our neighbor, then our levels cannot differ by
  // more than one.
  // Assume that our neighbor is half our size.
  double bds2[6];
  memcpy(bds2, bds, 6*sizeof(double));
  target = target - 1;
  float size;

  size = 0.5*(bds[1]-bds[0]);
  bds2[0] = bds[0] - size;
  bds2[1] = bds[1] + size;
  if (this->LineTest(x0, y0, z0, x1, y1, z1, bds2, level, target))
    {
    return 1;
    }
  bds2[0] = bds[0];
  bds2[1] = bds[1];

  size = 0.5*(bds[3]-bds[2]);
  bds2[2] = bds[2] - size;
  bds2[3] = bds[3] + size;
  if (this->LineTest(x0, y0, z0, x1, y1, z1, bds2, level, target))
    {
    return 1;
    }
  bds2[2] = bds[2];
  bds2[3] = bds[3];

  size = 0.5*(bds[5]-bds[4]);
  bds2[4] = bds[4] - size;
  bds2[5] = bds[5] + size;
  if (this->LineTest(x0, y0, z0, x1, y1, z1, bds2, level, target))
    {
    return 1;
    }

  return 0;
}

//----------------------------------------------------------------------------
void vtkHierarchicalFractal::Traverse(int &blockId,
                                      int level,
                                      vtkCompositeDataSet* output, 
                                      int x0,
                                      int x3,
                                      int y0,
                                      int y3,
                                      int z0,
                                      int z3,
                                      int onFace[6])
{
  double bds[6];
  int x1, x2, y1, y2, z1, z2;
  
  if (this->TwoDimensional)
    {
    z0 = z3 = 0;
    }  
  
  // Get the bounds of the proposed block.
  int ext[6];
  ext[0]=x0; ext[1]=x3; ext[2]=y0; ext[3]=y3, ext[4]=z0; ext[5]=z3;
  this->CellExtentToBounds(level, ext, bds);

  x0 = x0*2;
  x3 = (x3+1)*2 - 1;
  y0 = y0*2;
  y3 = (y3+1)*2 - 1;
  z0 = z0*2;
  z3 = (z3+1)*2 - 1;

  x2 = x0+this->Dimensions;
  x1 = x2-1;
  y2 = y0+this->Dimensions;
  y1 = y2-1;
  z2 = z0+this->Dimensions;
  z1 = z2-1;

  if (x3-x2-x1+x0 > 2)
    { // balance asymetric blocks.
    x2 += 2;
    x1 += 2;
    }
    
  int subOnFace[6];
  int generateBlock  = 1;
  if (this->TwoDimensional)
    {
    if (this->TwoDTest(bds, level, this->MaximumLevel))
      {
      if ( ! this->Overlap)
        {
        generateBlock = 0;
        }
      ++level;
      // Traverse the 4 new blocks.
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=onFace[2];
      subOnFace[3]=0;
      subOnFace[4]=1;
      subOnFace[5]=1;
      this->Traverse(blockId, level, output, x0,x1,y0,y1,z0,z0,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnFace[2]=onFace[2];
//      subOnFace[3]=0;
      this->Traverse(blockId, level, output, x2,x3,y0,y1,z0,z0,subOnFace);
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=0;
      subOnFace[3]=onFace[3];
      this->Traverse(blockId, level, output, x0,x1,y2,y3,z0,z0,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnFace[2]=0;
//      subOnFace[3]=onFace[3];
      this->Traverse(blockId, level, output, x2,x3,y2,y3,z0,z0,subOnFace);
      }
    if (generateBlock)
      {
      if (this->BlockCount >= this->StartBlock
          && this->BlockCount <= this->EndBlock)
        {
        if(this->GenerateRectilinearGrids)
          {
          vtkRectilinearGrid *grid=vtkRectilinearGrid::New();
          this->AppedDataSetToLevel(output, level, ext, grid);
          grid->Delete();
          this->SetRBlockInfo(grid, level, ext,onFace);
          }
        else
          {
          vtkUniformGrid *grid=vtkUniformGrid::New();
          this->AppedDataSetToLevel(output, level, ext, grid);
          grid->Delete();
          this->SetBlockInfo(grid, level, ext,onFace);
          }
        this->Levels->InsertValue(blockId, level);
        ++blockId;
        }
      else if (this->EndBlock != -1)
        {
        this->AppedDataSetToLevel(output, level, ext, NULL);
        }
      ++this->BlockCount;
      }
    }
  else
    { // 3D
    if (this->LineTest(-1.64662,0.56383,1.16369, -1.05088,0.85595,0.87104, bds,
                       level, this->MaximumLevel) ||
        this->LineTest(-1.05088,0.85595,0.87104, -0.61430,1.00347,0.59553, bds,
                       level, this->MaximumLevel) )
      { // break block into eight.
      if ( ! this->Overlap)
        {
        generateBlock = 0;
        }
      ++level;
      // Traverse the 8 new blocks.
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=onFace[2];
      subOnFace[3]=0;
      subOnFace[4]=onFace[4];
      subOnFace[5]=0;
      this->Traverse(blockId, level, output, x0,x1,y0,y1,z0,z1,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnFace[2]=onFace[2];
//      subOnFace[3]=0;
      this->Traverse(blockId, level, output, x2,x3,y0,y1,z0,z1,subOnFace);
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=0;
      subOnFace[3]=onFace[3];
      this->Traverse(blockId, level, output, x0,x1,y2,y3,z0,z1,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnface[2]=0;
//      subOnFace[3]=onFace[3];
      this->Traverse(blockId, level, output, x2,x3,y2,y3,z0,z1,subOnFace);
      
      
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=onFace[2];
      subOnFace[3]=0;
      subOnFace[4]=0;
      subOnFace[5]=onFace[5];
      this->Traverse(blockId, level, output, x0,x1,y0,y1,z2,z3,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnFace[2]=onFace[2];
//      subOnFace[3]=0;
      this->Traverse(blockId, level, output, x2,x3,y0,y1,z2,z3,subOnFace);
      subOnFace[0]=onFace[0];
      subOnFace[1]=0;
      subOnFace[2]=0;
      subOnFace[3]=onFace[3];
      this->Traverse(blockId, level, output, x0,x1,y2,y3,z2,z3,subOnFace);
      subOnFace[0]=0;
      subOnFace[1]=onFace[1];
//      subOnFace[2]=0;
//      subOnFace[3]=onFace[3];
      this->Traverse(blockId, level, output, x2,x3,y2,y3,z2,z3,subOnFace);
      }
    if (generateBlock)
      {
      if (this->BlockCount >= this->StartBlock
          && this->BlockCount <= this->EndBlock)
        {
        if(this->GenerateRectilinearGrids)
          {
          vtkRectilinearGrid *grid=vtkRectilinearGrid::New();
          this->AppedDataSetToLevel(output, level, ext, grid);
          grid->Delete();
          this->SetRBlockInfo(grid, level, ext,onFace);
          }
        else
          {
          vtkUniformGrid *grid=vtkUniformGrid::New();
          this->AppedDataSetToLevel(output, level, ext, grid);
          grid->Delete();
          this->SetBlockInfo(grid, level, ext,onFace);
          }
        this->Levels->InsertValue(blockId, level);
        ++blockId;
        }
      ++this->BlockCount;
      }
    }
}

//----------------------------------------------------------------------------
void vtkHierarchicalFractal::AddTestArray(vtkCompositeDataSet *output)
{
  double *origin = this->GetTopLevelOrigin();
 
  vtkSmartPointer<vtkCompositeDataIterator> iter;
  iter.TakeReference(output->NewIterator());
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
    vtkUniformGrid *grid;
    grid = vtkUniformGrid::SafeDownCast(iter->GetCurrentDataObject());
    assert("check: grid_exists" && grid!=0);

    vtkDoubleArray* array = vtkDoubleArray::New();
    int numCells=grid->GetNumberOfCells();
    array->Allocate(numCells);
    array->SetNumberOfTuples(numCells);
    double *arrayPtr = static_cast<double*>(array->GetPointer(0));
    double  spacing[3];
    grid->GetSpacing(spacing);
    int x,y,z;
    int ext[6];
    grid->GetExtent(ext);
    // we need cell extents bu we just get point extents
    if(ext[5]>0)
      {
      --ext[5];
      }
    if(ext[3]>0)
      {
      --ext[3];
      }
    if(ext[1]>0)
      {
      --ext[1];
      }
    int debugcounter=0;
    for (z = ext[4]; z <= ext[5]; ++z)
      {
      for (y = ext[2]; y <= ext[3]; ++y)
        {
        for (x = ext[0]; x <= ext[1]; ++x)
          {
          *arrayPtr++ = origin[0] + spacing[0]*(static_cast<double>(x) + 0.5)
            + origin[1] + spacing[1]*(static_cast<double>(y) + 0.5);
          ++debugcounter;
          }
        }
      }
    assert("check: valid_debugcounter" && debugcounter==numCells);
    array->SetName("TestX");
    grid->GetCellData()->AddArray(array);
    array->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkHierarchicalFractal::AddVectorArray(vtkCompositeDataSet *output)
{
  double *origin = this->GetTopLevelOrigin();

  vtkSmartPointer<vtkCompositeDataIterator> iter;
  iter.TakeReference(output->NewIterator());
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
    vtkUniformGrid *grid;
    grid=vtkUniformGrid::SafeDownCast(iter->GetCurrentDataObject());
    assert("check: grid_exists" && grid!=0);

    vtkDoubleArray* array = vtkDoubleArray::New();
    array->SetNumberOfComponents(3);
    int numCells=grid->GetNumberOfCells();
    array->Allocate(numCells);
    array->SetNumberOfTuples(numCells);
    double *arrayPtr = static_cast<double*>(array->GetPointer(0));
    double  spacing[3];
    grid->GetSpacing(spacing);
    int x,y,z;
    int ext[6];
    grid->GetExtent(ext);
    // we need cell extents bu we just get point extents
    if(ext[5]>0)
      {
      --ext[5];
      }
    if(ext[3]>0)
      {
      --ext[3];
      }
    if(ext[1]>0)
      {
      --ext[1];
      }

    for (z = ext[4]; z <= ext[5]; ++z)
      {
      for (y = ext[2]; y <= ext[3]; ++y)
        {
        for (x = ext[0]; x <= ext[1]; ++x)
          {
          *arrayPtr++ = origin[0] + spacing[0]*(static_cast<double>(x)+ 0.5);
          *arrayPtr++ = origin[1] + spacing[1]*(static_cast<double>(y)+ 0.5);
          *arrayPtr++ = origin[2] + spacing[2]*(static_cast<double>(z)+ 0.5);
          }
        }
      }
    array->SetName("VectorXYZ");
    grid->GetCellData()->AddArray(array);
    array->Delete();
    }
}


//----------------------------------------------------------------------------
void vtkHierarchicalFractal::AddFractalArray(vtkCompositeDataSet *output)
{
  vtkImageMandelbrotSource* fractalSource = vtkImageMandelbrotSource::New();
  int dims[3];

  vtkSmartPointer<vtkCompositeDataIterator> iter;
  iter.TakeReference(output->NewIterator());
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
    if(!this->GenerateRectilinearGrids)
      {
      vtkUniformGrid *grid;
      grid=vtkUniformGrid::SafeDownCast(iter->GetCurrentDataObject());
      assert("check: grid_exists" && grid!=0);


      vtkDoubleArray* array = vtkDoubleArray::New();
      int numCells=grid->GetNumberOfCells();
      array->Allocate(numCells);
      array->SetNumberOfTuples(numCells);
      double *arrayPtr = static_cast<double*>(array->GetPointer(0));
      double  spacing[3];
      double origin[3];
      grid->GetSpacing(spacing);
      grid->GetOrigin(origin);
      grid->GetDimensions(dims);
      // we get the dimensions according to the points
      // we need the dimensions according to the cells

      if(dims[0]>1)
        {
        --dims[0];
        }
      if(dims[1]>1)
        {
        --dims[1];
        }
      if(dims[2]>1)
        {
        --dims[2];
        }

      // Shift point to center of voxel.
      fractalSource->SetWholeExtent(0,dims[0]-1, 0,dims[1]-1, 0,dims[2]-1);
      fractalSource->SetOriginCX(origin[0]+(spacing[0]*0.5), 
        origin[1]+(spacing[1]*0.5), 
        origin[2]+(spacing[2]*0.5),
        this->TimeStep/10.0);
      fractalSource->SetSampleCX(spacing[0], spacing[1], spacing[2], 0.1);
      fractalSource->Update();
      vtkDataArray *fractal;
      fractal=fractalSource->GetOutput()->GetPointData()->GetScalars();
      float *fractalPtr = static_cast<float *>(fractal->GetVoidPointer(0));

      for (int i = 0; i < fractal->GetNumberOfTuples(); ++i)
        {
        // Change fractal into volume fraction (iso surface at 0.5).
        *arrayPtr++ = *fractalPtr++ / (2.0 * this->FractalValue);
        }

      array->SetName("Fractal Volume Fraction");
      grid->GetCellData()->AddArray(array);
      array->Delete();
      }
    else // rectilinear grid
      {
      vtkRectilinearGrid *grid;
      grid=vtkRectilinearGrid::SafeDownCast(iter->GetCurrentDataObject());
      assert("check: grid_exists" && grid!=0);

      vtkDoubleArray* array = vtkDoubleArray::New();
      int numCells=grid->GetNumberOfCells();
      array->Allocate(numCells);
      array->SetNumberOfTuples(numCells);
      double *arrayPtr = static_cast<double*>(array->GetPointer(0));

      this->ExecuteRectilinearMandelbrot(grid,arrayPtr);
      array->SetName("Fractal Volume Fraction");
      grid->GetCellData()->AddArray(array);
      array->Delete();
      }
    }
  fractalSource->Delete();
}


//----------------------------------------------------------------------------
void vtkHierarchicalFractal::AddBlockIdArray(vtkCompositeDataSet *output)
{
  int blockId = 0;
  vtkSmartPointer<vtkCompositeDataIterator> iter;
  iter.TakeReference(output->NewIterator());
  iter->SkipEmptyNodesOff();
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); 
    iter->GoToNextItem(), blockId++)
    {
    vtkUniformGrid *grid;
    grid=vtkUniformGrid::SafeDownCast(iter->GetCurrentDataObject());
    if (!grid)
      {
      continue;
      }

    vtkIntArray* array = vtkIntArray::New();
    int numCells=grid->GetNumberOfCells();
    array->Allocate(numCells);
    int cell=0;
    while(cell<numCells)
      {
      array->InsertNextValue(blockId);
      ++cell;
      }
    array->SetName("BlockId");
    grid->GetCellData()->AddArray(array);
    array->Delete();
    }
}


//----------------------------------------------------------------------------
void vtkHierarchicalFractal::AddDepthArray(vtkHierarchicalBoxDataSet* output)
{
  int levels=output->GetNumberOfLevels();
  int level=0;
  while(level<levels)
    {
    int blocks=output->GetNumberOfDataSets(level);
    int block=0;
    while(block<blocks)
      {
      vtkAMRBox temp;
      vtkUniformGrid *grid;
      grid=vtkUniformGrid::SafeDownCast(output->GetDataSet(level,block, temp));
      if (grid)
        {
        vtkIntArray* array = vtkIntArray::New();
        int numCells=grid->GetNumberOfCells();
        array->Allocate(numCells);
        int cell=0;
        while(cell<numCells)
          {
          array->InsertNextValue(level);
          ++cell;
          }
        array->SetName("Depth");
        grid->GetCellData()->AddArray(array);
        array->Delete();
        }
      ++block;
      }
    ++level;
    }
}

void vtkHierarchicalFractal::AddGhostLevelArray(vtkDataSet *grid,
                                                int dim[3],
                                                int onFace[6])
{
  vtkUnsignedCharArray* array = vtkUnsignedCharArray::New();
  // we just get the dimensions according to points
  // we need the dimensions according to cells
  
  int dims[3];
  dims[0]=dim[0];
  dims[1]=dim[1];
  dims[2]=dim[2];
  
  if(dims[0]>1)
    {
    --dims[0];
    }
  if(dims[1]>1)
    {
    --dims[1];
      }
  if(dims[2]>1)
    {
    --dims[2];
    }
  
  int numCells=grid->GetNumberOfCells();
  array->SetNumberOfTuples(numCells);
  
  int i, j, k;
  int iLevel, jLevel, kLevel, tmp;
  unsigned char* ptr;
  
  ptr = (unsigned char*)(array->GetVoidPointer(0));
  
  
  for (k = 0; k < dims[2]; ++k)
    {
    // ghost level at the beginning
    if(onFace[4])
      {
      kLevel=this->GhostLevels-1 - k; // or =-k
      }
    else
      {
      kLevel = this->GhostLevels - k;
      }
    // ghost level at the end.
    if(onFace[5])
      {
      tmp = k - dims[2] + 1 + this->GhostLevels-1;
      }
    else
      {
      tmp = k - dims[2] + 1 + this->GhostLevels;
      }
    if (tmp > kLevel) { kLevel = tmp;}
    if (this->TwoDimensional)
      {
      kLevel = 0;
        }
    for (j = 0; j < dims[1]; ++j)
      {
      jLevel = kLevel;
      if(onFace[2])
        {
        tmp = this->GhostLevels-1 - j;
        }
      else
        {
        tmp = this->GhostLevels - j;
        }
      if (tmp > jLevel) { jLevel = tmp;}
      if(onFace[3])
          {
          tmp = j - dims[1] + 1 + this->GhostLevels-1;
          }
      else
        {
        tmp = j - dims[1] + 1 + this->GhostLevels;
        }
      if (tmp > jLevel) { jLevel = tmp;}
      for (i = 0; i < dims[0]; ++i)
        {
        iLevel = jLevel;
        if(onFace[0])
          {
          tmp = this->GhostLevels-1 - i;
          }
        else
          {
          tmp = this->GhostLevels - i;
          }
        if (tmp > iLevel) { iLevel = tmp;}
        if(onFace[1])
          {
          tmp = i - dims[0] + 1 + this->GhostLevels-1;
          }
        else
          {
          tmp = i - dims[0] + 1 + this->GhostLevels;
          }
        if (tmp > iLevel) { iLevel = tmp;}
        
        if (iLevel <= 0)
          {
          *ptr = 0;
          }
        else
          {
          *ptr = iLevel;
          }
        ++ptr;
        }
      }
    }
  array->SetName("vtkGhostLevels");
//      array->SetName("vtkNotGhostLevels");
  grid->GetCellData()->AddArray(array);
  array->Delete();
}

//----------------------------------------------------------------------------
void vtkHierarchicalFractal::CellExtentToBounds(int level,
                                                int ext[6],
                                                double bds[6])
{
  int spacingFactor = 1;
  double spacing[3];
  spacingFactor = spacingFactor << level;
  
  spacing[0] = this->TopLevelSpacing[0] / (double)(spacingFactor);
  spacing[1] = this->TopLevelSpacing[1] / (double)(spacingFactor);
  spacing[2] = this->TopLevelSpacing[2] / (double)(spacingFactor);

  bds[0] = this->TopLevelOrigin[0] + (double)(ext[0]) * spacing[0];
  bds[1] = this->TopLevelOrigin[0] + (double)(ext[1]+1) * spacing[0];
  bds[2] = this->TopLevelOrigin[1] + (double)(ext[2]) * spacing[1];
  bds[3] = this->TopLevelOrigin[1] + (double)(ext[3]+1) * spacing[1];
  bds[4] = this->TopLevelOrigin[2] + (double)(ext[4]) * spacing[2];
  bds[5] = this->TopLevelOrigin[2] + (double)(ext[5]+1) * spacing[2];
}

//----------------------------------------------------------------------------
void vtkHierarchicalFractal::ExecuteRectilinearMandelbrot(
  vtkRectilinearGrid *grid,
  double *ptr)
{
  int a0=0;
  int a1=1;
  int a2=2;
  
  int dims[3];
  
  grid->GetDimensions(dims);
  // we get the dimensions according to the points
  // we need the dimensions according to the cells
  
  if(dims[0]>1)
    {
    --dims[0];
    }
  if(dims[1]>1)
    {
    --dims[1];
    }
  if(dims[2]>1)
    {
    --dims[2];
    }
  
  int ext[6];
  double p[4];
  
  // Shift point to center of voxel.
//  fractalSource->SetWholeExtent(0,dims[0]-1, 0,dims[1]-1, 0,dims[2]-1);
  ext[0]=0;
  ext[1]=dims[0]-1;
  ext[2]=0;
  ext[3]=dims[1]-1;
  ext[4]=0;
  ext[5]=dims[2]-1;
  
  int min0, max0;
  int idx0, idx1, idx2;
  vtkIdType inc0, inc1, inc2;
  
//  fractalSource->SetOriginCX(origin[0]+(spacing[0]*0.5), 
//                             origin[1]+(spacing[1]*0.5), 
//                             origin[2]+(spacing[2]*0.5), 0.0);
  
  double origin[4];
  vtkDataArray *coords[3];
  coords[0]=grid->GetXCoordinates();
  coords[1]=grid->GetYCoordinates();
  coords[2]=grid->GetZCoordinates();
  
  origin[0]=coords[0]->GetTuple1(0)+0.5*(coords[0]->GetTuple1(1)-coords[0]->GetTuple1(0));
  origin[1]=coords[0]->GetTuple1(0)+0.5*(coords[1]->GetTuple1(1)-coords[1]->GetTuple1(0));
  origin[2]=coords[0]->GetTuple1(0)+0.5*(coords[2]->GetTuple1(1)-coords[2]->GetTuple1(0));
  origin[3]=this->TimeStep/10.0;
  
  // Copy origin into pixel
  for (idx0 = 0; idx0 < 4; ++idx0)
    {
    p[idx0] = origin[idx0];
    }
  
  min0 = ext[0];
  max0 = ext[1];
  
  this->GetContinuousIncrements(ext, inc0, inc1, inc2);
  
  
//  fractalSource->SetSampleCX(spacing[0], spacing[1], spacing[2], 0.1);
  
  for (idx2 = ext[4]; idx2 <= ext[5]; ++idx2)
    {
//    p[a2] = (double)(origin[a2]) + (double)(idx2)*(sample[a2]);
    p[a2] = coords[a2]->GetTuple1(idx2)+0.5*(coords[a2]->GetTuple1(idx2+1)-coords[a2]->GetTuple1(idx2));
    for (idx1 = ext[2]; idx1 <= ext[3]; ++idx1)
      {
//      p[a1] = (double)(origin[a1]) + (double)(idx1)*(sample[a1]);
      p[a1] = coords[a1]->GetTuple1(idx1)+0.5*(coords[a1]->GetTuple1(idx1+1)-coords[a1]->GetTuple1(idx1));
      for (idx0 = min0; idx0 <= max0; ++idx0)
        {
        //      p[a0] = (double)(origin[a0]) + (double)(idx0)*(sample[a0]);
        p[a0] = coords[a0]->GetTuple1(idx0)+0.5*(coords[a0]->GetTuple1(idx0+1)-coords[a0]->GetTuple1(idx0));
        
        // Change fractal into volume fraction (iso surface at 0.5).
        *ptr = this->EvaluateSet(p)/ (2.0 * this->FractalValue);
        
        ++ptr;
        // inc0 is 0
        }
      ptr += inc1;
      }
    ptr += inc2;
    }
}

//----------------------------------------------------------------------------
double vtkHierarchicalFractal::EvaluateSet(double p[4])
{
  unsigned short count = 0;
  double v0, v1;
  double cReal, cImag, zReal, zImag;
  double zReal2, zImag2;

  const int maximumNumberOfIterations=100;
  
  cReal = p[0];
  cImag = p[1];
  zReal = p[2];
  zImag = p[3];

  zReal2 = zReal * zReal;
  zImag2 = zImag * zImag;
  v0 = 0.0;
  v1 = (zReal2 + zImag2);
  while ( v1 < 4.0 && count < maximumNumberOfIterations)
    {
    zImag = 2.0 * zReal * zImag + cImag;
    zReal = zReal2 - zImag2 + cReal;
    zReal2 = zReal * zReal;
    zImag2 = zImag * zImag;
    ++count;
    v0 = v1;
    v1 = (zReal2 + zImag2);
    }

  if (count == maximumNumberOfIterations)
    {
    return (double)count;
    }

  return (double)count + (4.0 - v0)/(v1 - v0);
}

//----------------------------------------------------------------------------
void vtkHierarchicalFractal::GetContinuousIncrements(int extent[6],
                                                     vtkIdType &incX,
                                                     vtkIdType &incY,
                                                     vtkIdType &incZ)
{
  int e0, e1, e2, e3;
  
  incX = 0;

  e0 = extent[0];
  e1 = extent[1];
  e2 = extent[2];
  e3 = extent[3];

  // Make sure the increments are up to date
  
  int idx;
  
// vtkIdType inc = this->GetPointData()->GetScalars()->GetNumberOfComponents();
  vtkIdType inc =1;
  
  int increments[3];
  
  for (idx = 0; idx < 3; ++idx)
    {
    increments[idx] = inc;
    inc *= (extent[idx*2+1] - extent[idx*2] + 1);
    }
  
  incY = increments[1] - (e1 - e0 + 1)*increments[0];
  incZ = increments[2] - (e3 - e2 + 1)*increments[1];
}

//----------------------------------------------------------------------------
unsigned int vtkHierarchicalFractal::AppedDataSetToLevel(
  vtkCompositeDataSet* composite,
  unsigned int level,
  int extents[6],
  vtkDataSet* dataset)
{
  unsigned int index = 0;
  vtkMultiBlockDataSet* mbDS = vtkMultiBlockDataSet::SafeDownCast(composite);
  vtkHierarchicalBoxDataSet* hbDS = 
    vtkHierarchicalBoxDataSet::SafeDownCast(composite);
  if (mbDS)
    {
    vtkMultiBlockDataSet* block = vtkMultiBlockDataSet::SafeDownCast(
      mbDS->GetBlock(level));
    if (!block)
      {
      block = vtkMultiBlockDataSet::New();
      mbDS->SetBlock(level, block);
      block->Delete();
      }
    index = block->GetNumberOfBlocks();
    block->SetBlock(index, dataset);
    }
  else if (hbDS)
    {
    int dim=this->TwoDimensional? 2:3;
    vtkAMRBox box(dim,extents);
    index = hbDS->GetNumberOfDataSets(level);
    hbDS->SetDataSet(level, index, box, vtkUniformGrid::SafeDownCast(dataset));
    }

  return index;
}

//----------------------------------------------------------------------------
void vtkHierarchicalFractal::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Dimensions: " << this->Dimensions << endl;
  os << indent << "TwoDimensional: " << this->TwoDimensional << endl;
  os << indent << "FractalValue: " << this->FractalValue << endl;
  os << indent << "MaximumLevel: " << this->MaximumLevel << endl;
  os << indent << "GhostLevels: " << this->GhostLevels << endl;
  os << indent << "Asymetric: " << this->Asymetric << endl;
  os << indent << "GenerateRectilinearGrids: ";
  if(this->GenerateRectilinearGrids)
    {
    os << "True";
    }
  else
    {
    os << "False";
    }
  os<< endl;
  os << indent << "TimeStep: " << this->TimeStep << endl;
  os << indent << "TimeStepRange: " 
     << this->TimeStepRange[0] << " " << this->TimeStepRange[1]
     << endl;
}

