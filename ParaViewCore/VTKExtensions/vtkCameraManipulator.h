/*=========================================================================

  Program:   ParaView
  Module:    vtkCameraManipulator.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkCameraManipulator - Abstraction of style away from button.
// .SECTION Description
// vtkCameraManipulator is a superclass foractions inside an
// interactor style and associated with a single button. An example
// might be rubber-band bounding-box zoom. This abstraction allows a
// camera manipulator to be assigned to any button.  This super class
// might become a subclass of vtkInteractorObserver in the future.

#ifndef __vtkCameraManipulator_h
#define __vtkCameraManipulator_h

#include "vtkObject.h"

class vtkCameraManipulatorGUIHelper;
class vtkRenderer;
class vtkRenderWindowInteractor;

class VTK_EXPORT vtkCameraManipulator : public vtkObject
{
public:
  static vtkCameraManipulator *New();
  vtkTypeMacro(vtkCameraManipulator, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Event bindings controlling the effects of pressing mouse buttons
  // or moving the mouse.
  virtual void StartInteraction();
  virtual void EndInteraction();

  virtual void OnMouseMove(int x, int y, vtkRenderer *ren,
                           vtkRenderWindowInteractor *iren);
  virtual void OnButtonDown(int x, int y, vtkRenderer *ren,
                            vtkRenderWindowInteractor *iren);
  virtual void OnButtonUp(int x, int y, vtkRenderer *ren,
                          vtkRenderWindowInteractor *iren);
  
  // Description:
  // These settings determine which button and modifiers the
  // manipulator responds to. Button can be either 1 (left), 2
  // (middle), and 3 right.
  vtkSetMacro(Button, int);
  vtkGetMacro(Button, int);
  vtkSetMacro(Shift, int);
  vtkGetMacro(Shift, int);
  vtkBooleanMacro(Shift, int);
  vtkSetMacro(Control, int);
  vtkGetMacro(Control, int);
  vtkBooleanMacro(Control, int);

  // Description:
  // For setting the center of rotation.
  vtkSetVector3Macro(Center, double);
  vtkGetVector3Macro(Center, double);

  // Description:
  // Set and get the manipulator name.
  vtkSetStringMacro(ManipulatorName);
  vtkGetStringMacro(ManipulatorName);

  // Description:
  // Get/Set the GUI helper. 
  void SetGUIHelper(vtkCameraManipulatorGUIHelper*);
  vtkGetObjectMacro(GUIHelper, vtkCameraManipulatorGUIHelper);
protected:
  vtkCameraManipulator();
  ~vtkCameraManipulator();

  char* ManipulatorName;

  int Button;
  int Shift;
  int Control;

  double Center[3];
  double DisplayCenter[2];
  void ComputeDisplayCenter(vtkRenderer *ren);

  vtkCameraManipulatorGUIHelper* GUIHelper;

private:
  vtkCameraManipulator(const vtkCameraManipulator&); // Not implemented
  void operator=(const vtkCameraManipulator&); // Not implemented
};

#endif
