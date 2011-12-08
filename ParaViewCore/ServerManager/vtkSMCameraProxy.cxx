/*=========================================================================

  Program:   ParaView
  Module:    vtkSMCameraProxy.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMCameraProxy.h"

#include "vtkObjectFactory.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkCamera.h"

vtkStandardNewMacro(vtkSMCameraProxy);
//-----------------------------------------------------------------------------
vtkSMCameraProxy::vtkSMCameraProxy()
{
}

//-----------------------------------------------------------------------------
vtkSMCameraProxy::~vtkSMCameraProxy()
{
}

//-----------------------------------------------------------------------------
void vtkSMCameraProxy::UpdatePropertyInformation()
{
  if (this->InUpdateVTKObjects)
    {
    return;
    }

  vtkCamera* camera = vtkCamera::SafeDownCast(this->GetClientSideObject());
  if (!camera)
    {
    this->Superclass::UpdatePropertyInformation();
    return;
    }

  vtkSMDoubleVectorProperty* dvp;
  dvp = vtkSMDoubleVectorProperty::SafeDownCast(
    this->GetProperty("CameraPositionInfo"));
  dvp->SetElements(camera->GetPosition());

  dvp = vtkSMDoubleVectorProperty::SafeDownCast(
    this->GetProperty("CameraFocalPointInfo"));
  dvp->SetElements(camera->GetFocalPoint());

  dvp = vtkSMDoubleVectorProperty::SafeDownCast(
    this->GetProperty("CameraViewUpInfo"));
  dvp->SetElements(camera->GetViewUp());

  dvp = vtkSMDoubleVectorProperty::SafeDownCast(
    this->GetProperty("CameraClippingRangeInfo"));
  dvp->SetElements(camera->GetClippingRange());

  dvp = vtkSMDoubleVectorProperty::SafeDownCast(
    this->GetProperty("CameraParallelScaleInfo"));
  dvp->SetElement(0, camera->GetParallelScale());

  dvp = vtkSMDoubleVectorProperty::SafeDownCast(
    this->GetProperty("CameraViewAngleInfo"));
  dvp->SetElement(0, camera->GetViewAngle());
}

//-----------------------------------------------------------------------------
void vtkSMCameraProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
