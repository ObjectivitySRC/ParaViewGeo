/*=========================================================================

  Program:   ParaView
  Module:    vtkPVKeyFrame.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPVKeyFrame
// .SECTION Description
// Base class for key frames.
// A key frame is responsible to interpolate the curve
// between it self and a consequent key frame. A new subclass is
// needed for each type of interpolation available between two
// key frames. This class can be instantiated to create a no-action key
// frame.

#ifndef __vtkPVKeyFrame_h
#define __vtkPVKeyFrame_h

#include "vtkObject.h"

class vtkPVKeyFrameInternals;
class vtkPVAnimationCue;
struct vtkClientServerID;

class VTK_EXPORT vtkPVKeyFrame : public vtkObject
{
public:
  vtkTypeMacro(vtkPVKeyFrame, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkPVKeyFrame* New();

  // Description:;
  // Key time is the time at which this key frame is
  // associated. KeyTime ranges from [0,1], where 0 is the
  // start time of the cue for which this is a key frame and
  // 1 is that cue's end time.
  vtkSetMacro(KeyTime, double);
  vtkGetMacro(KeyTime, double);

  // Description:
  // Get/Set the animated value at this key frame.
  // Note that is the number of values is adjusted to fit the index
  // specified in SetKeyValue.
  virtual void SetKeyValue(double val) { this->SetKeyValue(0, val); }
  virtual void SetKeyValue(unsigned int index, double val);
  double GetKeyValue() { return this->GetKeyValue(0); }
  double GetKeyValue(unsigned int index);

  // Description:
  // Removes all key values.
  virtual void RemoveAllKeyValues();

  // Description:
  // Set/Get the number of key values this key frame currently stores.
  unsigned int GetNumberOfKeyValues();
  virtual void SetNumberOfKeyValues(unsigned int num);

  // Description:
  // This method will do the actual interpolation.
  // currenttime is normalized to the time range between
  // this key frame and the next key frame.
  virtual void UpdateValue(double currenttime,
    vtkPVAnimationCue* cue, vtkPVKeyFrame* next);

protected:
  vtkPVKeyFrame();
  ~vtkPVKeyFrame();

  double KeyTime;
  vtkPVKeyFrameInternals* Internals;

private:
  vtkPVKeyFrame(const vtkPVKeyFrame&); // Not implemented.
  void operator=(const vtkPVKeyFrame&); // Not implemented.
};

#endif
