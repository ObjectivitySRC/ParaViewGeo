/*=========================================================================

  Program:   ParaView
  Module:    vtkPVContourEntry.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2000-2001 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither the name of Kitware nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific 
   prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
// .NAME vtkPVContourEntry maintains a list of floats for contouring.
// .SECTION Description
// This widget lets the user add or delete floats from a list.
// It is used for contours, but could be generalized for arbitrary lists.

#ifndef __vtkPVContourEntry_h
#define __vtkPVContourEntry_h

#include "vtkPVWidget.h"

class vtkKWListBox;
class vtkKWEntry;
class vtkKWPushButton;
class vtkKWLabel;
class vtkContourValues;
class vtkPVScalarListWidgetProperty;

class VTK_EXPORT vtkPVContourEntry : public vtkPVWidget
{
public:
  static vtkPVContourEntry* New();
  vtkTypeRevisionMacro(vtkPVContourEntry, vtkPVWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Create the widget.
  virtual void Create(vtkKWApplication *app);

  // Description:
  // Set the label.  The label can be used to get this widget
  // from a script.
  void SetLabel (const char* label);
  const char* GetLabel();
  
  // Description:
  // Access to this widget from a script.
  void AddValueInternal(float val);
  void AddValue(float val);
  void RemoveAllValues();

  // Description:
  // Button callbacks.
  void AddValueCallback();
  void DeleteValueCallback();

  // Description:
  // adds a script to the menu of the animation interface.
  virtual void AddAnimationScriptsToMenu(vtkKWMenu *menu, 
                                         vtkPVAnimationInterfaceEntry *ai);

  // Description:
  // We need to make the callback here so the animation selection
  // can be traced properly.
  void AnimationMenuCallback(vtkPVAnimationInterfaceEntry *ai);

  // Description:
  // This class redefines SetBalloonHelpString since it
  // has to forward the call to a widget it contains.
  virtual void SetBalloonHelpString(const char *str);

//BTX
  // Description:
  // Creates and returns a copy of this widget. It will create
  // a new instance of the same type as the current object
  // using NewInstance() and then copy some necessary state 
  // parameters.
  vtkPVContourEntry* ClonePrototype(vtkPVSource* pvSource,
                                 vtkArrayMap<vtkPVWidget*, vtkPVWidget*>* map);
//ETX

  // Description:
  // Gets called when the accept button is pressed.
  virtual void AcceptInternal(const char* sourceTclName);

  // Description:
  // Gets called when the reset button is pressed.
  virtual void ResetInternal();

  // Description:
  // This serves a dual purpose.  For tracing and for saving state.
  virtual void Trace(ofstream *file);

  virtual void SetProperty(vtkPVWidgetProperty *prop);
  virtual vtkPVWidgetProperty* CreateAppropriateProperty();
  
protected:
  vtkPVContourEntry();
  ~vtkPVContourEntry();

  // Update UI from ContourValues object.
  void Update();

  vtkContourValues *ContourValues;
  vtkKWLabel* ContourValuesLabel;
  vtkKWListBox *ContourValuesList;
  vtkKWWidget* NewValueFrame;
  vtkKWLabel* NewValueLabel;
  vtkKWEntry* NewValueEntry;
  vtkKWPushButton* AddValueButton;
  vtkKWPushButton* DeleteValueButton;

  vtkContourValues *LastAcceptedContourValues;
  int AcceptCalled;
  void UpdateProperty();
  vtkPVScalarListWidgetProperty *Property;
  
  vtkPVContourEntry(const vtkPVContourEntry&); // Not implemented
  void operator=(const vtkPVContourEntry&); // Not implemented

//BTX
  virtual void CopyProperties(vtkPVWidget* clone, vtkPVSource* pvSource,
                              vtkArrayMap<vtkPVWidget*, vtkPVWidget*>* map);
//ETX
  
  int ReadXMLAttributes(vtkPVXMLElement* element,      
                        vtkPVXMLPackageParser* parser);

  // Description:
  // The widget saves it state/command in the vtk tcl script.
  virtual void SaveInBatchScriptForPart(ofstream *file, const char* sourceTclName);
};

#endif
