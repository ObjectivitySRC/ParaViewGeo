/*=========================================================================

Program:   ParaView
Module:    pvserver.cxx

Copyright (c) Kitware, Inc.
All rights reserved.
See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInitializationHelper.h"
#include "vtkMultiProcessController.h"
#include "vtkNetworkAccessManager.h"
#include "vtkPVServerOptions.h"
#include "vtkProcessModule.h"
#include "vtkPVSessionServer.h"
#include "vtkPVConfig.h"

#ifndef BUILD_SHARED_LIBS
#include "pvStaticPluginsInit.h"
#endif

static bool RealMain(int argc, char* argv[],
  vtkProcessModule::ProcessTypes type)
{
  // Marking this static avoids the false leak messages from vtkDebugLeaks when
  // using mpich. It appears that the root process which spawns all the
  // main processes waits in MPI_Init() and calls exit() when
  // the others are done, causing apparent memory leaks for any non-static objects
  // created before MPI_Init().
  static vtkSmartPointer<vtkPVServerOptions> options =
    vtkSmartPointer<vtkPVServerOptions>::New();

  // Init current process type
  vtkInitializationHelper::Initialize( argc, argv, type, options );
  if (options->GetTellVersion() || options->GetHelpSelected())
    {
    vtkInitializationHelper::Finalize();
    return 1;
    }

  // load static plugins
#ifndef BUILD_SHARED_LIBS
  paraview_static_plugins_init();
#endif

  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  vtkMultiProcessController* controller = pm->GetGlobalController();
  vtkPVSessionServer* session = vtkPVSessionServer::New();
  int process_id = controller->GetLocalProcessId();
  if (process_id == 0)
    {
    cout << "Waiting for client" << endl;
    }
  bool success = false;
  if (session->Connect())
    {
    success = true;
    pm->RegisterSession(session);
    if (controller->GetLocalProcessId() == 0)
      {
      while (pm->GetNetworkAccessManager()->ProcessEvents(0) != -1)
        {
        }
      }
    else
      {
      controller->ProcessRMIs();
      }
    pm->UnRegisterSession(session);
    }

  cout << "Exiting..." << endl;
  session->Delete();
  // Exit application
  vtkInitializationHelper::Finalize();
  return success;
}
