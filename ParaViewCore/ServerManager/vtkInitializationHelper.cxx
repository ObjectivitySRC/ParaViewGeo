/*=========================================================================

Program:   ParaView
Module:    vtkInitializationHelper.cxx

Copyright (c) Kitware, Inc.
All rights reserved.
See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInitializationHelper.h"
#include "vtkPVConfig.h"
/*
 * Make sure all the kits register their classes with vtkInstantiator.
 * Since ParaView uses Tcl wrapping, all of VTK is already compiled in
 * anyway.  The instantiators will add no more code for the linker to
 * collect.
 */

#include "vtkCommonInstantiator.h"
#include "vtkFilteringInstantiator.h" // MANDATORY gives runtime problems otherwise
#include "vtkGenericFilteringInstantiator.h"
#include "vtkIOInstantiator.h"
#include "vtkImagingInstantiator.h"
#include "vtkGraphicsInstantiator.h"
#include "vtkRenderingInstantiator.h"
#include "vtkVolumeRenderingInstantiator.h"
#include "vtkHybridInstantiator.h"
#include "vtkParallelInstantiator.h"
#include "vtkPVCommonInstantiator.h"
#include "vtkPVVTKExtensionsInstantiator.h"
#include "vtkPVClientServerCoreInstantiator.h"
#include "vtkPVServerImplementationInstantiator.h"
#include "vtkPVServerManagerInstantiator.h"
#include "vtkClientServerInterpreter.h"

#include "vtkClientServerInterpreterInitializer.h"
#include "vtkOutputWindow.h"
#include "vtkProcessModule.h"
#include "vtkPVOptions.h"
#include "vtkSmartPointer.h"
#include "vtkSMMessage.h"
#include "vtkSMProperty.h"
#include "vtkSMProxyManager.h"

#include <vtkstd/string>
#include <vtksys/ios/sstream>

static void vtkInitializationHelperInit(vtkClientServerInterpreter*);

//----------------------------------------------------------------------------
// ClientServer wrapper initialization functions.
extern "C" void vtkParaviewMinInit_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkCommonCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkFilteringCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkGenericFilteringCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkImagingCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkInfovisCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkGraphicsCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkIOCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkRenderingCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkVolumeRenderingCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkHybridCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkWidgetsCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkParallelCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkPVCommonCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkPVVTKExtensionsCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkPVClientServerCoreCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkPVServerImplementationCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkPVServerManager_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkXdmfCS_Initialize(vtkClientServerInterpreter *);

#ifdef PARAVIEW_USE_VISITBRIDGE
extern "C" void vtkVisItAVTAlgorithmsCS_Initialize(vtkClientServerInterpreter *);
extern "C" void vtkVisItDatabasesCS_Initialize(vtkClientServerInterpreter *);
#endif

//----------------------------------------------------------------------------
void vtkInitializationHelper::Initialize(const char* executable, int type)
{
  vtkInitializationHelper::Initialize(executable, type, NULL);
}

//----------------------------------------------------------------------------
void vtkInitializationHelper::Initialize(const char* executable,
  int type, vtkPVOptions* options)
{
  if (!executable)
    {
    vtkGenericWarningMacro("Executable name has to be defined.");
    return;
    }

  // Pass the program name to make option parser happier
  char* argv = new char[strlen(executable)+1];
  strcpy(argv, executable);

  vtkSmartPointer<vtkPVOptions> newoptions = options;
  if (!options)
    {
    newoptions = vtkSmartPointer<vtkPVOptions>::New();
    }
  vtkInitializationHelper::Initialize(1, &argv, type, newoptions);
  delete[] argv;
}

//----------------------------------------------------------------------------
void vtkInitializationHelper::Initialize(int argc, char**argv,
  int type, vtkPVOptions* options)
{
  if (vtkProcessModule::GetProcessModule())
    {
    vtkGenericWarningMacro("Process already initialize. Skipping.");
    return;
    }

  if (!options)
    {
    vtkGenericWarningMacro("vtkPVOptions must be specified.");
    return;
    }

  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  vtkClientServerInterpreterInitializer::GetInitializer()->
    RegisterCallback(&::vtkInitializationHelperInit);

  vtkProcessModule::Initialize(
    static_cast<vtkProcessModule::ProcessTypes>(type), argc, argv);

  vtksys_ios::ostringstream sscerr;
  if (argv && !options->Parse(argc, argv) )
    {
    if ( options->GetUnknownArgument() )
      {
      sscerr << "Got unknown argument: " << options->GetUnknownArgument() << endl;
      }
    if ( options->GetErrorMessage() )
      {
      sscerr << "Error: " << options->GetErrorMessage() << endl;
      }
    options->SetHelpSelected(1);
    }
  if (options->GetHelpSelected())
    {
    sscerr << options->GetHelp() << endl;
    vtkOutputWindow::GetInstance()->DisplayText( sscerr.str().c_str() );
    // TODO: indicate to the caller that application must quit.
    }

  if (options->GetTellVersion() )
    {
    vtksys_ios::ostringstream str;
    str << "paraview version " << PARAVIEW_VERSION_FULL << "\n";
    vtkOutputWindow::GetInstance()->DisplayText(str.str().c_str());
    // TODO: indicate to the caller that application must quit.
    }

  vtkProcessModule::GetProcessModule()->SetOptions(options);

  vtkSMProxyManager* pxm = vtkSMProxyManager::New();
  vtkSMObject::SetProxyManager(pxm);
  pxm->Delete();
}

//----------------------------------------------------------------------------
void vtkInitializationHelper::Finalize()
{
  vtkSMObject::SetProxyManager(NULL);
  vtkProcessModule::Finalize();

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();
}

//-----------------------------------------------------------------------------
/*
 * PARAVIEW_MINIMIUM if enabled, initializes only required set of classes.
 * The mechanism is specified in the Servers/ServerManager/CMakeList.txt
 * Otherwise the entire vtk+paraview class list will be included.
 *
 * @param pm IN used to pass the interpreter for every *_Initialize function.
 */
void vtkInitializationHelperInit(vtkClientServerInterpreter* interp)
{

#ifdef PARAVIEW_MINIMAL_BUILD
  vtkParaviewMinInit_Initialize(interp);
#else
  // Initialize built-in wrapper modules.
  vtkCommonCS_Initialize(interp);
  vtkFilteringCS_Initialize(interp);
  vtkGenericFilteringCS_Initialize(interp);
  vtkImagingCS_Initialize(interp);
  vtkInfovisCS_Initialize(interp);
  vtkGraphicsCS_Initialize(interp);
  vtkIOCS_Initialize(interp);
  vtkRenderingCS_Initialize(interp);
  vtkVolumeRenderingCS_Initialize(interp);
  vtkHybridCS_Initialize(interp);
  vtkWidgetsCS_Initialize(interp);
  vtkParallelCS_Initialize(interp);
  vtkPVCommonCS_Initialize(interp);
  vtkPVVTKExtensionsCS_Initialize(interp);
  vtkPVClientServerCoreCS_Initialize(interp);
  vtkPVServerImplementationCS_Initialize(interp);
  vtkPVServerManager_Initialize(interp);
  vtkXdmfCS_Initialize(interp);

# ifdef PARAVIEW_USE_VISITBRIDGE
  vtkVisItAVTAlgorithmsCS_Initialize(interp);
  vtkVisItDatabasesCS_Initialize(interp);
# endif
#endif

}
