/*=========================================================================

  Program:   ParaView
  Module:    vtkSMStateLocator.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMStateLocator - Class used to retreive a given message state based
// on its GlobalID.
// .SECTION Description
// vtkSMStateLocator allow a hierarchical way of finding a message state.

#ifndef __vtkSMStateLocator_h
#define __vtkSMStateLocator_h

#include "vtkSMObject.h"
#include "vtkSMMessageMinimal.h" // needed for vtkSMMessage.

class VTK_EXPORT vtkSMStateLocator : public vtkSMObject
{
public:
  static vtkSMStateLocator* New();
  vtkTypeMacro(vtkSMStateLocator, vtkSMObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get a parent locator to search which is used as a backup location
  // to search from if a given state was not found locally.
  vtkSetObjectMacro(ParentLocator, vtkSMStateLocator);
  vtkGetObjectMacro(ParentLocator, vtkSMStateLocator);

//BTX
  // Description:
  // Fill the provided State message with the state found inside the current
  // locator or one of its parent. The method return true if the state was
  // successfully filled.
  virtual bool FindState(vtkTypeUInt32 globalID, vtkSMMessage* stateToFill);

  // Description:
  // Register the given state in the current locator. If a previous state was
  // available, the new one will replace it.
  virtual void RegisterState(const vtkSMMessage* state);

  // Description:
  // Remove a state for a given proxy inside the local locator.
  // if force is true, it will also remove it from its hierarchical parents.
  virtual void UnRegisterState(vtkTypeUInt32 globalID, bool force);

  // Description:
  // Return true if the given state can be found locally whitout the help of
  // on the hierarchical parent
  virtual bool IsStateLocal(vtkTypeUInt32 globalID);

  // Description:
  // Return true if the given state do exist in the locator hierachy
  virtual bool IsStateAvailable(vtkTypeUInt32 globalID);

protected:
  vtkSMStateLocator();
  ~vtkSMStateLocator();

  vtkSMStateLocator* ParentLocator;

private:
  vtkSMStateLocator(const vtkSMStateLocator&); // Not implemented
  void operator=(const vtkSMStateLocator&); // Not implemented

  class vtkInternal;
  vtkInternal* Internals;
//ETX
};

#endif
