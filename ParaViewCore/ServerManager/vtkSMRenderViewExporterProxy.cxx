/*=========================================================================

  Program:   ParaView
  Module:    vtkSMRenderViewExporterProxy.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMRenderViewExporterProxy.h"

#include "vtkExporter.h"
#include "vtkObjectFactory.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMRenderViewProxy.h"

vtkStandardNewMacro(vtkSMRenderViewExporterProxy);
//----------------------------------------------------------------------------
vtkSMRenderViewExporterProxy::vtkSMRenderViewExporterProxy()
{
}

//----------------------------------------------------------------------------
vtkSMRenderViewExporterProxy::~vtkSMRenderViewExporterProxy()
{
}

//----------------------------------------------------------------------------
bool vtkSMRenderViewExporterProxy::CanExport(vtkSMProxy* view)
{
  return (view && view->IsA("vtkSMRenderViewProxy"));
}

//----------------------------------------------------------------------------
void vtkSMRenderViewExporterProxy::Write()
{
  this->CreateVTKObjects();

  vtkExporter* exporter = vtkExporter::SafeDownCast(this->GetClientSideObject());
  vtkSMRenderViewProxy* rv = vtkSMRenderViewProxy::SafeDownCast(this->View);
  if (exporter && rv)
    {
    int old_threshold = -1;
    if (rv->GetProperty("RemoteRenderThreshold"))
      {
      vtkSMPropertyHelper helper(rv, "RemoteRenderThreshold");
      old_threshold = helper.GetAsInt();
      helper.Set(VTK_INT_MAX);
      rv->StillRender();
      }

    vtkRenderWindow* renWin = rv->GetRenderWindow();
    exporter->SetRenderWindow(renWin);
    exporter->Write();
    exporter->SetRenderWindow(0);
    if (rv->GetProperty("RemoteRenderThreshold"))
      {
      vtkSMPropertyHelper(rv, "RemoteRenderThreshold").Set(old_threshold);
      }
    }
}

//----------------------------------------------------------------------------
void vtkSMRenderViewExporterProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


