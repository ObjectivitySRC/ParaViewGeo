/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPythonAnimationCue
// .SECTION Description
//

#ifndef __vtkPythonAnimationCue_h
#define __vtkPythonAnimationCue_h

#include "vtkAnimationCue.h"

class vtkPVPythonInterpretor;

class VTK_EXPORT vtkPythonAnimationCue : public vtkAnimationCue
{
public:
  static vtkPythonAnimationCue* New();
  vtkTypeMacro(vtkPythonAnimationCue, vtkAnimationCue);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Enable/Disable this cue.
  vtkSetMacro(Enabled, bool);
  vtkGetMacro(Enabled, bool);
  vtkBooleanMacro(Enabled, bool);

  // Description:
  // Get/Set the python script to execute. The script must have the following
  // functions:
  // \li start_cue(cue): (optional) if present, called when the cue starts.
  // \li tick(cue) : (required) called on every tick.
  // \li end_cue(cue): (optional) if present, called when the cue ends.
  vtkSetStringMacro(Script);
  vtkGetStringMacro(Script);

//BTX
protected:
  vtkPythonAnimationCue();
  ~vtkPythonAnimationCue();

  // Description:
  // Returns the interpretor. Creates a new one the first time this method is
  // called.
  vtkPVPythonInterpretor* GetInterpretor();

  // Description:
  // Callbacks that forward the call to corresponding Python function.
  virtual void HandleStartCueEvent();
  virtual void HandleTickEvent();
  virtual void HandleEndCueEvent();

  // Description:
  // Cleans the interpretor.
  void DeleteInterpretor();

  bool Enabled;
  char* Script;
  vtkPVPythonInterpretor* Interpretor;  

private:
  vtkPythonAnimationCue(const vtkPythonAnimationCue&); // Not implemented
  void operator=(const vtkPythonAnimationCue&); // Not implemented
//ETX
};

#endif
