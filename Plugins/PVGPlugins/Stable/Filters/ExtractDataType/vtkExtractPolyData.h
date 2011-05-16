/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkExtractPolyData.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkExtractPolyData - extracts PolyData blocks from a multiblock dataset.
// .SECTION Description
// vtkExtractPolyData is a filter that extracts blocks from a multiblock dataset.
// Each node in the multi-block tree is identified by an \c index. The index can
// be obtained by performing a preorder traversal of the tree (including empty 
// nodes). eg. A(B (D, E), C(F, G)). 
// Inorder traversal yields: A, B, D, E, C, F, G
// Index of A is 0, while index of C is 4.

#ifndef __vtkExtractPolyData_h
#define __vtkExtractPolyData_h

//#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkPolyDataAlgorithm.h"
//#include "vtkDataObjectAlgorithm.h"
//#include <vtkstd/set>

class vtkCompositeDataIterator;
class vtkMultiPieceDataSet;
class vtkMultiBlockDataSet;
class vtkAppendPolyData;

class VTK_EXPORT vtkExtractPolyData : public vtkPolyDataAlgorithm
{
public:
  static vtkExtractPolyData* New();
  vtkTypeRevisionMacro(vtkExtractPolyData, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Select the block indices to extract. 
  // Each node in the multi-block tree is identified by an \c index. The index can
  // be obtained by performing a preorder traversal of the tree (including empty 
  // nodes). eg. A(B (D, E), C(F, G)). 
  // Inorder traversal yields: A, B, D, E, C, F, G
  // Index of A is 0, while index of C is 4.
  void AddIndex(unsigned int index);
  void RemoveIndex(unsigned int index);
  void RemoveAllIndices();

  // Description:
  // When set, the output mutliblock dataset will be pruned to remove empty
  // nodes. On by default.
  vtkSetMacro(PruneOutput, int);
  vtkGetMacro(PruneOutput, int);
  vtkBooleanMacro(PruneOutput, int);

//BTX
protected:
  vtkExtractPolyData();
  ~vtkExtractPolyData();

  // Description:
  // Internal key, used to avoid pruning of a branch.
  static vtkInformationIntegerKey* DONT_PRUNE();

  /// Implementation of the algorithm.
  virtual int RequestData(vtkInformation *, 
                          vtkInformationVector **, 
                          vtkInformationVector *);

  /// Extract subtree
  void CopySubTree(vtkCompositeDataIterator* loc, 
    vtkMultiBlockDataSet* output, vtkMultiBlockDataSet* input);
  bool Prune(vtkMultiBlockDataSet* mblock);
  bool Prune(vtkMultiPieceDataSet* mblock);
  bool Prune(vtkDataObject* mblock);

  int PruneOutput;

	virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
  vtkExtractPolyData(const vtkExtractPolyData&); // Not implemented.
  void operator=(const vtkExtractPolyData&); // Not implemented.

  class vtkSet;
  vtkSet *Indices;
//ETX
};

#endif


