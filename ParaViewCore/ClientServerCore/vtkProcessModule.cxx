/*=========================================================================

  Program:   ParaView
  Module:    vtkProcessModule.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkProcessModule.h"
#include "vtkProcessModuleInternals.h"

#include "vtkAlgorithm.h"
#include "vtkCommand.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkDummyController.h"
#include "vtkDynamicLoader.h"
#include "vtkFloatingPointExceptions.h"
#include "vtkMapper.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkOutputWindow.h"
#include "vtkPVConfig.h"
#include "vtkPVOptions.h"
#include "vtkSessionIterator.h"
#include "vtkTCPNetworkAccessManager.h"
#include "vtkToolkits.h"

#include <vtksys/SystemTools.hxx>

#ifdef VTK_USE_MPI
# include "vtkMPIController.h"
# include "vtkPVMPICommunicator.h"
# include <mpi.h>
#endif

#include <assert.h>

//----------------------------------------------------------------------------
// * STATICS
vtkProcessModule::ProcessTypes
vtkProcessModule::ProcessType = vtkProcessModule::PROCESS_INVALID;

bool vtkProcessModule::FinalizeMPI = false;

vtkSmartPointer<vtkProcessModule> vtkProcessModule::Singleton;
vtkSmartPointer<vtkMultiProcessController> vtkProcessModule::GlobalController;

//----------------------------------------------------------------------------
bool vtkProcessModule::Initialize(ProcessTypes type, int &argc, char** &argv)
{
  setlocale(LC_NUMERIC,"C");

  vtkProcessModule::ProcessType = type;

  vtkProcessModule::GlobalController = vtkSmartPointer<vtkDummyController>::New();

#ifdef VTK_USE_MPI
  bool use_mpi = (type != PROCESS_CLIENT);
  // initialize MPI only on non-client processes.
  if (use_mpi)
    {
    int mpi_already_initialized = 0;
    MPI_Initialized(&mpi_already_initialized);
    if (mpi_already_initialized == 0)
      {
      // MPICH changes the current working directory after MPI_Init. We fix that
      // by changing the CWD back to the original one after MPI_Init.
      vtkstd::string cwd = vtksys::SystemTools::GetCurrentWorkingDirectory(true);

      // This is here to avoid false leak messages from vtkDebugLeaks when
      // using mpich. It appears that the root process which spawns all the
      // main processes waits in MPI_Init() and calls exit() when
      // the others are done, causing apparent memory leaks for any objects
      // created before MPI_Init().
      MPI_Init(&argc, &argv);

      // restore CWD to what it was before the MPI intialization.
      vtksys::SystemTools::ChangeDirectory(cwd.c_str());

      vtkProcessModule::FinalizeMPI = true;
      }

    vtkProcessModule::GlobalController = vtkSmartPointer<vtkMPIController>::New();
    vtkProcessModule::GlobalController->Initialize(
      &argc, &argv, /*initializedExternally*/1);

    // Replace the communicator with vtkPVMPICommunicator which handles progress
    // events better than the conventional vtkMPICommunicator.
    vtkPVMPICommunicator* comm = vtkPVMPICommunicator::New();
    comm->CopyFrom(vtkMPICommunicator::GetWorldCommunicator());
    vtkMPIController::SafeDownCast(
      vtkProcessModule::GlobalController)->SetCommunicator(comm);
    comm->Delete();
    }
#else
  static_cast<void>(argc); // unused warning when MPI is off
  static_cast<void>(argv); // unused warning when MPI is off
#endif // VTK_USE_MPI
  vtkMultiProcessController::SetGlobalController(
    vtkProcessModule::GlobalController);

#ifdef VTK_USE_X
  // Hack to support -display parameter.  vtkPVOptions requires parameters to be
  // specified as -option=value, but it is generally expected that X window
  // programs allow you to set the display as -display host:port (i.e. without
  // the = between the option and value).  Unless someone wants to change
  // vtkPVOptions to work with or without the = (which may or may not be a good
  // idea), then this is the easiest way around the problem.
  for (int i = 1; i < argc-1; i++)
    {
    if (strcmp(argv[i], "-display") == 0)
      {
      char *displayenv = new char[strlen(argv[i+1]) + 10];
      sprintf(displayenv, "DISPLAY=%s", argv[i+1]);
      vtksys::SystemTools::PutEnv(displayenv);
      delete [] displayenv;
      // safe to delete since PutEnv keeps a copy of the string.
      argc -= 2;
      for (int j = i; j < argc; j++)
        {
        argv[j] = argv[j+2];
        }
      argv[argc] = NULL;
      break;
      }
    }
#endif // VTK_USE_X

#ifdef _WIN32
  // Avoid Ghost windows on windows XP
  typedef void (* VOID_FUN)();
  vtkLibHandle lib = vtkDynamicLoader::OpenLibrary("user32.dll");
  if(lib)
    {
    VOID_FUN func = (VOID_FUN)
      vtkDynamicLoader::GetSymbolAddress(lib, "DisableProcessWindowsGhosting");
    if(func)
      {
      (*func)();
      }
    }
#endif // _WIN32

#ifdef PARAVIEW_ENABLE_FPE
  vtkFloatingPointExceptions::Enable();
#endif //PARAVIEW_ENABLE_FPE

  // Don't prompt the user with startup errors on unix.
  // On windows, don't prompt when running on the dashboard.
#if defined(_WIN32) && !defined(__CYGWIN__)
  if (getenv("DASHBOARD_TEST_FROM_CTEST") || getenv("DART_TEST_FROM_DART"))
    {
    vtkOutputWindow::GetInstance()->PromptUserOff();
    }
  else
    {
    vtkOutputWindow::GetInstance()->PromptUserOn();
    }
#else
  vtkOutputWindow::GetInstance()->PromptUserOff();
#endif

#ifdef PARAVIEW_USE_MPI_SSEND
  vtkMPIController::SetUseSsendForRMI(1);
#endif

  vtkMultiThreader::SetGlobalMaximumNumberOfThreads(1);

  // Create the process module.
  vtkProcessModule::Singleton = vtkSmartPointer<vtkProcessModule>::New();
  return true;
}

//----------------------------------------------------------------------------
bool vtkProcessModule::Finalize()
{
  if(vtkProcessModule::Singleton)
    {
    vtkProcessModule::Singleton->InvokeEvent(vtkCommand::ExitEvent);
    }

  // destroy the process-module.
  vtkProcessModule::Singleton = NULL;

  // We don't really need to call SetGlobalController(NULL) since
  // it's really stored with a weak pointer.  We set it to null anyways
  // in case it gets changed later to reference counting the pointer
  vtkMultiProcessController::SetGlobalController(NULL);
  vtkProcessModule::GlobalController->Finalize(/*finalizedExternally*/1);
  vtkProcessModule::GlobalController = NULL;

#ifdef VTK_USE_MPI
  if (vtkProcessModule::FinalizeMPI)
    {
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    }
#endif

  return true;
}

//----------------------------------------------------------------------------
vtkProcessModule::ProcessTypes vtkProcessModule::GetProcessType()
{
  return vtkProcessModule::ProcessType;
}

//----------------------------------------------------------------------------
vtkProcessModule* vtkProcessModule::GetProcessModule()
{ 
  return vtkProcessModule::Singleton.GetPointer();
}

//----------------------------------------------------------------------------
// * vtkProcessModule non-static methods
vtkStandardNewMacro(vtkProcessModule);
vtkCxxSetObjectMacro(vtkProcessModule, NetworkAccessManager,
  vtkNetworkAccessManager);
//----------------------------------------------------------------------------
vtkProcessModule::vtkProcessModule()
{
  this->NetworkAccessManager = vtkTCPNetworkAccessManager::New();
  this->Options = 0;
  this->Internals = new vtkInternals();
  this->MaxSessionId = 0;
  this->ReportInterpreterErrors = true;
  this->SymmetricMPIMode = false;

  vtkCompositeDataPipeline* cddp = vtkCompositeDataPipeline::New();
  vtkAlgorithm::SetDefaultExecutivePrototype(cddp);
  cddp->Delete();

  vtkMapper::SetResolveCoincidentTopologyToShiftZBuffer();
  vtkMapper::SetResolveCoincidentTopologyZShift(2.0e-3);
}

//----------------------------------------------------------------------------
vtkProcessModule::~vtkProcessModule()
{
  vtkAlgorithm::SetDefaultExecutivePrototype(NULL);

  this->SetNetworkAccessManager(NULL);
  this->SetOptions(NULL);

  delete this->Internals;
  this->Internals = 0;
}

//----------------------------------------------------------------------------
vtkIdType vtkProcessModule::RegisterSession(vtkSession* session)
{
  assert(session != NULL);
  this->MaxSessionId++;
  this->Internals->Sessions[this->MaxSessionId] = session;
  this->InvokeEvent(vtkCommand::ConnectionCreatedEvent, &this->MaxSessionId);
  return this->MaxSessionId;
}

//----------------------------------------------------------------------------
bool vtkProcessModule::UnRegisterSession(vtkIdType sessionID)
{
  vtkInternals::MapOfSessions::iterator iter =
    this->Internals->Sessions.find(sessionID);
  if (iter != this->Internals->Sessions.end())
    {
    this->InvokeEvent(vtkCommand::ConnectionClosedEvent,
      &sessionID);
    this->Internals->Sessions.erase(iter);
    return true;
    }

  return false;
}


//----------------------------------------------------------------------------
bool vtkProcessModule::UnRegisterSession(vtkSession* session)
{
  vtkInternals::MapOfSessions::iterator iter;
  for (iter = this->Internals->Sessions.begin();
    iter != this->Internals->Sessions.end(); ++iter)
    {
    if (iter->second == session)
      {
      vtkIdType sessionID = iter->first;
      this->InvokeEvent(vtkCommand::ConnectionClosedEvent, &sessionID);
      this->Internals->Sessions.erase(iter);
      return true;
      }
    }
  vtkErrorMacro("Session has not been registered. Cannot unregister : " <<
    session);
  return false;
}

//----------------------------------------------------------------------------
vtkSession* vtkProcessModule::GetSession(vtkIdType sessionID)
{
  vtkInternals::MapOfSessions::iterator iter =
    this->Internals->Sessions.find(sessionID);
  if (iter != this->Internals->Sessions.end())
    {
    return iter->second.GetPointer();
    }

  return NULL;
}

//----------------------------------------------------------------------------
vtkIdType vtkProcessModule::GetSessionID(vtkSession* session)
{
  vtkInternals::MapOfSessions::iterator iter;
  for (iter = this->Internals->Sessions.begin();
    iter != this->Internals->Sessions.end(); ++iter)
    {
    if (iter->second == session)
      {
      return iter->first;
      }
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkSessionIterator* vtkProcessModule::NewSessionIterator()
{
  vtkSessionIterator* iter = vtkSessionIterator::New();
  return iter;
}

//----------------------------------------------------------------------------
vtkMultiProcessController* vtkProcessModule::GetGlobalController()
{
  return vtkMultiProcessController::GetGlobalController();
}

//----------------------------------------------------------------------------
int vtkProcessModule::GetNumberOfLocalPartitions()
{
  return this->GetGlobalController()?
    this->GetGlobalController()->GetNumberOfProcesses() : 1;
}

//----------------------------------------------------------------------------
int vtkProcessModule::GetPartitionId()
{
  return this->GetGlobalController()?
    this->GetGlobalController()->GetLocalProcessId() : 0;
}

//----------------------------------------------------------------------------
void vtkProcessModule::PushActiveSession(vtkSession* session)
{
  assert(session != NULL);

  this->Internals->ActiveSessionStack.push_back(session);
}

//----------------------------------------------------------------------------
void vtkProcessModule::PopActiveSession(vtkSession* session)
{
  assert(session != NULL);

  if (this->Internals->ActiveSessionStack.back() != session)
    {
    vtkErrorMacro("Mismatch in active-session stack. Aborting for debugging.");
    abort();
    }
  this->Internals->ActiveSessionStack.pop_back();
}

//----------------------------------------------------------------------------
vtkSession* vtkProcessModule::GetActiveSession()
{
  if (this->Internals->ActiveSessionStack.size() == 0)
    {
    return NULL;
    }
  return this->Internals->ActiveSessionStack.back();
}

//----------------------------------------------------------------------------
vtkSession* vtkProcessModule::GetSession()
{
  vtkSession* activeSession = this->GetActiveSession();
  if (activeSession)
    {
    return activeSession;
    }

  vtkInternals::MapOfSessions::iterator iter;
  iter = this->Internals->Sessions.begin();
  return (iter != this->Internals->Sessions.end()?
    iter->second.GetPointer() : NULL);
}

//----------------------------------------------------------------------------
void vtkProcessModule::SetOptions(vtkPVOptions* options)
{
  vtkSetObjectBodyMacro(Options, vtkPVOptions, options);
  if (options)
    {
    this->SetSymmetricMPIMode(
      options->GetSymmetricMPIMode() != 0);
    }
}

//----------------------------------------------------------------------------
void vtkProcessModule::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "NetworkAccessManager: " << endl;
  this->NetworkAccessManager->PrintSelf(os, indent.GetNextIndent());

  if (this->Options)
    {
    os << indent << "Options: " << endl;
    this->Options->PrintSelf(os, indent.GetNextIndent());
    }
  else
    {
    os << indent << "Options: " << "(null)" << endl;
    }
}
