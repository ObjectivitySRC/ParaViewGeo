/*=========================================================================

  Program:   ParaView
  Module:    vtkCaveSynchronizedRenderers.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkCaveSynchronizedRenderers.h"

#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"
#include "vtkPVServerOptions.h"
#include "vtkPerspectiveTransform.h"
#include "vtkProcessModule.h"
#include "vtkRenderer.h"
#include "vtkTimerLog.h"
#include "vtkTransform.h"
#include "vtkUnsignedCharArray.h"

#include <vtksys/SystemTools.hxx>
#include "assert.h"


vtkStandardNewMacro(vtkCaveSynchronizedRenderers);
//----------------------------------------------------------------------------
vtkCaveSynchronizedRenderers::vtkCaveSynchronizedRenderers()
{
  this->NumberOfDisplays = 0;
  this->Displays= 0;
  this->SetNumberOfDisplays(1);

  this->DisplayOrigin[0] = 0.0;
  this->DisplayOrigin[1] = 0.0;
  this->DisplayOrigin[2] = 0.0;
  this->DisplayX[0] = 0.0;
  this->DisplayX[1] = 0.0;
  this->DisplayX[2] = 0.0;
  this->DisplayY[0] = 0.0;
  this->DisplayY[1] = 0.0;
  this->DisplayY[2] = 0.0;


  once =1;
  this->SetParallelController(vtkMultiProcessController::GetGlobalController());

  // Initilize using pvx file specified on the command line options.
  vtkPVServerOptions* options = vtkPVServerOptions::SafeDownCast(
    vtkProcessModule::GetProcessModule()->GetOptions());
  if (!options)
    {
    vtkErrorMacro("Are you sure vtkCaveSynchronizedRenderers is crated on "
      "an appropriate processes?");
    }
  else
    {
    this->SetNumberOfDisplays(options->GetNumberOfMachines());
    for (int cc=0; cc < this->NumberOfDisplays; cc++)
      {
      if (options->GetDisplayName(cc))
        {
        if (this->ParallelController &&
          this->ParallelController->GetLocalProcessId() == cc)
          {
          // PutEnv() avoids memory leak.
          vtksys::SystemTools::PutEnv(options->GetDisplayName(cc));
          }
        }
      this->DefineDisplay(cc, options->GetLowerLeft(cc),
        options->GetLowerRight(cc), options->GetUpperRight(cc));
      }
    }
}

//----------------------------------------------------------------------------
vtkCaveSynchronizedRenderers::~vtkCaveSynchronizedRenderers()
{
  this->SetNumberOfDisplays(0);
}

//----------------------------------------------------------------------------
void vtkCaveSynchronizedRenderers::HandleStartRender()
{
  this->ImageReductionFactor = 1;
  this->Superclass::HandleStartRender();
  this->ComputeCamera(this->GetRenderer()->GetActiveCamera());
  this->GetRenderer()->ResetCameraClippingRange();
}

//-----------------------------------------------------------------------------
void vtkCaveSynchronizedRenderers::SetNumberOfDisplays(int numberOfDisplays)
{
  if (numberOfDisplays == this->NumberOfDisplays)
    {
    return;
    }
  double** newDisplays = 0;
  if (numberOfDisplays > 0)
    {
    newDisplays = new double*[numberOfDisplays];
    for (int i = 0; i < numberOfDisplays; ++i)
      {
      newDisplays[i] = new double[12];
      if (i < this->NumberOfDisplays)
        {

        memcpy(newDisplays[i], this->Displays[i], 12 * sizeof(double));
        }
      else
        {
        newDisplays[i][0] = -1.;
        newDisplays[i][1] = -1.;
        newDisplays[i][2] = -1.;
        newDisplays[i][3] = 1.0;

        newDisplays[i][4] = 1.0;
        newDisplays[i][5] = -1.0;
        newDisplays[i][6] = -1.0;
        newDisplays[i][7] = 1.0;

        newDisplays[i][8] = -1.0;
        newDisplays[i][9] = 1.0;
        newDisplays[i][10] = -1.0;
        newDisplays[i][11] = 1.0;
        }
      }
    }
  for (int i = 0; i < this->NumberOfDisplays; ++i)
    {
    delete [] this->Displays[i];
    }
  delete [] this->Displays;
  this->Displays = newDisplays;

  this->NumberOfDisplays = numberOfDisplays;
  this->Modified();
}

//-------------------------------------------------------------------------
void vtkCaveSynchronizedRenderers::DefineDisplay(
  int idx, double origin[3], double x[3], double y[3])
{
  if (idx >= this->NumberOfDisplays)
    {
    vtkErrorMacro("idx is too high !");
    return;
    }
  memcpy(&this->Displays[idx][0], origin, 3 * sizeof(double));
  memcpy(&this->Displays[idx][4], x, 3 * sizeof(double));
  memcpy(&this->Displays[idx][8], y, 3 * sizeof(double));
  if (idx == this->GetParallelController()->GetLocalProcessId())
    {
    memcpy(this->DisplayOrigin, origin, 3 * sizeof(double));
    memcpy(this->DisplayX, x, 3 * sizeof(double));
    memcpy(this->DisplayY, y, 3 * sizeof(double));
    }
  this->Modified();
}

//-------------------------------------------------------------------------
// Room camera is a camera in room coordinates that points at the display.
// Client camera is the camera on the client.  The out camera is the
// combination of the two used for the final cave display.
// It is the room camera transformed by the world camera.
void vtkCaveSynchronizedRenderers::ComputeCamera(vtkCamera* camera)
{
  if(once)
    {
    double eyePosition[3] = {0.0, 0.0, 10.0};
    camera->SetScreenBottomLeft(this->DisplayOrigin);
    camera->SetScreenBottomRight(this->DisplayX);
    camera->SetScreenTopRight(this->DisplayY);
    camera->SetUseOffAxisProjection(true);
    camera->SetEyePosition(eyePosition);
    camera->SetEyeSeparation(0.05);

    // cam->SetHeadTracked( true );
    // cam->SetScreenConfig( this->DisplayOrigin,
    //                       this->DisplayX,
    //                       this->DisplayY ,0.065, 1.0);
    once =0;
    }
}


//----------------------------------------------------------------------------
void vtkCaveSynchronizedRenderers::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "NumberOfDisplays: "
    << this->NumberOfDisplays << endl;
  vtkIndent rankIndent = indent.GetNextIndent();
  for (int i = 0; i < this->NumberOfDisplays; ++i)
    {
    os << rankIndent;
    for (int j = 0; j < 12; ++j)
      {
      os << this->Displays[i][j] << " ";
      }
    os << endl;
    }
  os << indent << "Origin: "
     << this->DisplayOrigin[0] << " "
     << this->DisplayOrigin[1] << " "
     << this->DisplayOrigin[2] << endl;
  os << indent << "X: "
     << this->DisplayX[0] << " "
     << this->DisplayX[1] << " "
     << this->DisplayX[2] << endl;
  os << indent << "Y: "
     << this->DisplayY[0] << " "
     << this->DisplayY[1] << " "
     << this->DisplayY[2] << endl;
}
