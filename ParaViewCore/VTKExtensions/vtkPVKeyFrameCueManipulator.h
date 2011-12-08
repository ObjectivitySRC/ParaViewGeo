/*=========================================================================

  Program:   ParaView
  Module:    vtkPVKeyFrameCueManipulator.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPVKeyFrameCueManipulator - animation manipulator
// that uses keyframes to generate the animation.
// .SECTION Description
// This is a Manipulator that support key framing.
// Key frames are stored in a vector ordered by their keyframe time. Ordering
// of keyframes with same key time is arbritary. This class ensures that the
// keyframes are always maintained in the correct order.
// How the values for the animated property are interpolated between successive
// keyframes depends on the the type of the preceding keyframe. Thus this class
// doesn't perform the interpolation instead delegates it to the keyframe object
// affecting the property at the current time value.
// \li \c vtkPVCueManipulator::StateModifiedEvent -
// This event is fired when the manipulator modifies the animated proxy.
// \li \c vtkCommand::ModifiedEvent -
// is fired when the keyframes are changed i.e. added/removed/modified.
//
// .SECTION See Also
// vtkPVAnimationCue vtkPVCueManipulator

#ifndef __vtkPVKeyFrameCueManipulator_h
#define __vtkPVKeyFrameCueManipulator_h

#include "vtkPVCueManipulator.h"

class vtkPVKeyFrameCueManipulatorInternals;
class vtkPVKeyFrameCueManipulatorObserver;
class vtkPVKeyFrame;

class VTK_EXPORT vtkPVKeyFrameCueManipulator :
  public vtkPVCueManipulator
{
public:
  vtkTypeMacro(vtkPVKeyFrameCueManipulator, vtkPVCueManipulator);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkPVKeyFrameCueManipulator* New();

  // Description:
  // Add a key frame. Key frames are stored in a map, keyed by the
  // KeyFrameTime. If two keyframes have the same
  // key time, only one of then will be considered. It returns the index
  // of the added frame in the collection.
  int AddKeyFrame (vtkPVKeyFrame* keyframe);

  // Description:
  // This method returns the index of the last added key frame.
  // Note that this index is valid only until none of the keyframes
  // are modified. This is even provided as a method so that
  // this value can be accessed via properties.
  vtkGetMacro(LastAddedKeyFrameIndex, int);

  // Description:
  // Removes a key frame at the specified time, if any.
  void RemoveKeyFrame(vtkPVKeyFrame* keyframe);

  // Description:
  // Removes all key frames, if any.
  void RemoveAllKeyFrames();

  // Description:
  // Returns a pointer to the key frame at the given time.
  // If no key frame exists at the given time, it returns NULL.
  vtkPVKeyFrame* GetKeyFrame(double time);

  // Description:
  // Given the current time, determine the key frames
  // between which the current time lies.
  // Returns the key frame time.
  // If the current time
  // coincides with a key frame, both methods (GetStartKeyFrameTime
  // and GetEndKeyFrameTime) return that key keyframes time which is
  // same as time. If the current time is before the first key frame
  // or after the last key frame, then this method return -1.
  vtkPVKeyFrame* GetStartKeyFrame(double time);
  vtkPVKeyFrame* GetEndKeyFrame(double time);

  // Description:
  // Get the next/previous key frame relative to argument key frame.
  // Returns NULL when no next/previous frame exists.
  vtkPVKeyFrame* GetNextKeyFrame(vtkPVKeyFrame* keyFrame);
  vtkPVKeyFrame* GetPreviousKeyFrame(vtkPVKeyFrame* keyFrame);

  // Description:
  // Get the number of keyframes.
  unsigned int GetNumberOfKeyFrames();

  // Description:
  // Access the keyframe collection using the indices.
  // Keyframes are sorted according to increasing key frame time.
  vtkPVKeyFrame* GetKeyFrameAtIndex(int index);

  // Description:
  // This method iterates over all added keyframe proxies and updates the
  // domains for all keyframes, such that for every keyframe J, with keytime
  // between keyframes I and K, the domain for keytime of J is
  // (DomainJ)min = KeyTimeI and (DomainJ)max = KeyTimeK.
  // void UpdateKeyTimeDomains();
protected:
  vtkPVKeyFrameCueManipulator();
  ~vtkPVKeyFrameCueManipulator();

  // Description:
  // This method is called when the AnimationCue's StartAnimationCueEvent is
  // triggerred, to let the animation manipulator know that the cue has
  // been restarted. This is here for one major reason: after the last key
  // frame, the state of the scene must be as it was left a the the last key
  // frame. This does not happend automatically, since if while animating the
  // currentime never coincides with the last key frame's key time, then it
  // never gets a chance to update the properties value.
  // Hence, we note when the cue begins. Then, if the currentime is beyond
  // that of the last key frame we pretend that the current time coincides
  // with that of the last key frame and let it update the properties. This
  // is done only once per Animation cycle. The Initialize method is used to
  // indicate that a new animation cycle has begun.
  virtual void Initialize(vtkPVAnimationCue*);

  virtual void Finalize(vtkPVAnimationCue*);

  vtkPVKeyFrameCueManipulatorInternals* Internals;
  // Description:
  // This updates the values based on currenttime.
  // currenttime is normalized to the time range of the Cue.
  virtual void UpdateValue(double currenttime,
    vtkPVAnimationCue* cueproxy);

  int AddKeyFrameInternal(vtkPVKeyFrame* keyframe);
  int RemoveKeyFrameInternal(vtkPVKeyFrame* keyframe);

  //BTX
  friend class vtkPVKeyFrameCueManipulatorObserver;
  vtkPVKeyFrameCueManipulatorObserver* Observer;
  void ExecuteEvent(vtkObject* obj, unsigned long event, void* );
  //ETX
  int SendEndEvent;
  int LastAddedKeyFrameIndex;

private:
  vtkPVKeyFrameCueManipulator(const vtkPVKeyFrameCueManipulator&); // Not implemented.
  void operator=(const vtkPVKeyFrameCueManipulator&); // Not implemented.
};

#endif
