/*=========================================================================

  Program:   ParaView
  Module:    vtkUndoSet.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkUndoSet - Maintains a collection of vtkUndoElement that can be
// undone/redone in a single step.
// .SECTION Description
// This is a concrete class that stores a collection of vtkUndoElement objects.
// A vtkUndoSet object represents an atomic undo-redoable operation. It can 
// contain one or more vtkUndoElement objects. When added vtkUndoElement objects
// to a vtkUndoSet they must be added in the sequence of operation. When undoing
// the operations are performed in reverse order, while when redoing they are 
// performed in forward order.
// 
// vtkUndoElement, vtkUndoSet and vtkUndoStack form the undo/redo framework core.
// .SECTION See Also
// vtkUndoStack vtkUndoElement

#ifndef __vtkUndoSet_h
#define __vtkUndoSet_h

#include "vtkObject.h"

class vtkCollection;
class vtkPVXMLElement;
class vtkUndoElement;

class VTK_EXPORT vtkUndoSet : public vtkObject
{
public:
  static vtkUndoSet* New();
  vtkTypeMacro(vtkUndoSet, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Perform an Undo.
  virtual int Undo();

  // Description:
  // Perform a Redo.
  virtual int Redo();

  // Description:
  // Add an element to this set. If the newly added element, \c elem, and
  // the most recently added element are both \c Mergeable, then an
  // attempt is made to merge the new element with the previous one. On
  // successful merging, the new element is discarded, otherwise
  // it is appended to the set.
  // \returns the index at which the element got added/merged.
  int AddElement(vtkUndoElement* elem);

  // Description:
  // Remove an element at a particular index.
  void RemoveElement(int index);

  // Description:
  // Get an element at a particular index
  vtkUndoElement* GetElement(int index);

  // Description:
  // Remove all elemments.
  void RemoveAllElements();

  // Description:
  // Get number of elements in the set.
  int GetNumberOfElements();

protected:
  vtkUndoSet();
  ~vtkUndoSet();

  vtkCollection* Collection;
  vtkCollection* TmpWorkingCollection;
private:
  vtkUndoSet(const vtkUndoSet&); // Not implemented.
  void operator=(const vtkUndoSet&); // Not implemented.
};


#endif

