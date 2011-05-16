/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkExtractUnstrucGrid.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkExtractUnstrucGrid - extracts PolyData blocks from a multiblock dataset.
// .SECTION Description
// vtkExtractUnstrucGrid is a filter that extracts blocks from a multiblock dataset.
// Each node in the multi-block tree is identified by an \c index. The index can
// be obtained by performing a preorder traversal of the tree (including empty 
// nodes). eg. A(B (D, E), C(F, G)). 
// Inorder traversal yields: A, B, D, E, C, F, G
// Index of A is 0, while index of C is 4.

#ifndef __vtkExtractUnstrucGrid_h
#define __vtkExtractUnstrucGrid_h

#include "vtkUnstructuredGridAlgorithm.h"

class vtkCompositeDataIterator;
class vtkMultiPieceDataSet;
class vtkMultiBlockDataSet;
class vtkAppendFilter;

class VTK_EXPORT vtkExtractUnstrucGrid : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkExtractUnstrucGrid* New();
  vtkTypeRevisionMacro(vtkExtractUnstrucGrid, vtkUnstructuredGridAlgorithm);
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
  vtkExtractUnstrucGrid();
  ~vtkExtractUnstrucGrid();

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
  vtkExtractUnstrucGrid(const vtkExtractUnstrucGrid&); // Not implemented.
  void operator=(const vtkExtractUnstrucGrid&); // Not implemented.

  class vtkSet;
  vtkSet *Indices;
//ETX
};

#endif


