/*=========================================================================

  Program:   ParaView
  Module:    vtkPVSession.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPVSession - extends vtkSession to add API for ParaView sessions.
// .SECTION Description
// vtkPVSession adds APIs to vtkSession for ParaView-specific sessions, namely
// those that are used to communicate between data-server,render-server and
// client. This is an abstract class.

#ifndef __vtkPVSession_h
#define __vtkPVSession_h

#include "vtkSession.h"

class vtkMPIMToNSocketConnection;
class vtkMultiProcessController;
class vtkPVProgressHandler;
class vtkPVServerInformation;

class VTK_EXPORT vtkPVSession : public vtkSession
{
public:
  vtkTypeMacro(vtkPVSession, vtkSession);
  void PrintSelf(ostream& os, vtkIndent indent);

  enum ServerFlags
    {
    NONE = 0,
    DATA_SERVER = 0x01,
    DATA_SERVER_ROOT = 0x02,
    RENDER_SERVER = 0x04,
    RENDER_SERVER_ROOT = 0x08,
    SERVERS = DATA_SERVER | RENDER_SERVER,
    CLIENT = 0x10,
    CLIENT_AND_SERVERS = DATA_SERVER | CLIENT | RENDER_SERVER
    };

  // Description:
  // Returns a ServerFlags indicate the nature of the current processes. e.g. if
  // the current processes acts as a data-server and a render-server, it returns
  // DATA_SERVER | RENDER_SERVER.
  virtual ServerFlags GetProcessRoles();

  // Description:
  // Returns the controller used to communicate with the process. Value must be
  // DATA_SERVER_ROOT or RENDER_SERVER_ROOT or CLIENT.
  // Default implementation returns NULL.
  virtual vtkMultiProcessController* GetController(ServerFlags processType);

  // Description:
  // This is socket connection, if any to communicate between the data-server
  // and render-server nodes.
  virtual vtkMPIMToNSocketConnection* GetMPIMToNSocketConnection()
    { return NULL; }

  // Description:
  // vtkPVServerInformation is an information-object that provides information
  // about the server processes. These include server-side capabilities as well
  // as server-side command line arguments e.g. tile-display parameters. Use
  // this method to obtain the server-side information.
  // NOTE: For now, we are not bothering to provide separate informations from
  // data-server and render-server (as was the case earlier). We can easily add
  // API for the same if needed.
  virtual vtkPVServerInformation* GetServerInformation()=0;

  // Description:
  // Provides access to the progress handler.
  vtkGetObjectMacro(ProgressHandler, vtkPVProgressHandler);

  // Description:
  // Should be called to begin/end receiving progresses on this session.
  void PrepareProgress();
  void CleanupPendingProgress();

  // Description:
  // Returns true if the session is within a PrepareProgress() and
  // CleanupPendingProgress() block.
  bool GetPendingProgress();

//BTX
protected:
  vtkPVSession();
  ~vtkPVSession();

  enum 
    {
    EXCEPTION_EVENT_TAG=31416
    };

  // Description:
  // Callback when any vtkMultiProcessController subclass fires a WrongTagEvent.
  virtual void OnWrongTagEvent(vtkObject* caller, unsigned long eventid,
    void* calldata);

  // Description:
  // Virtual methods subclasses can override.
  virtual void PrepareProgressInternal();
  virtual void CleanupPendingProgressInternal();

  vtkPVProgressHandler* ProgressHandler;
private:
  vtkPVSession(const vtkPVSession&); // Not implemented
  void operator=(const vtkPVSession&); // Not implemented

  int ProgressCount;
  // This flags ensures that while we are waiting for an previous progress-pair
  // to finish, we don't start new progress-pairs.
  bool InCleanupPendingProgress;
//ETX
};

#endif
