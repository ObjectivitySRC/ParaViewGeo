/*=========================================================================

  Program:   ParaView
  Module:    vtkMPIMToNSocketConnection.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMPIMToNSocketConnection
// .SECTION Description

#ifndef __vtkMPIMToNSocketConnection_h
#define __vtkMPIMToNSocketConnection_h

#include "vtkObject.h"
class vtkMultiProcessController;
class vtkSocketCommunicator;
class vtkMPIMToNSocketConnectionPortInformation;
class vtkMPIMToNSocketConnectionInternals;

class VTK_EXPORT vtkMPIMToNSocketConnection : public vtkObject
{
public:
  static vtkMPIMToNSocketConnection* New();
  vtkTypeRevisionMacro(vtkMPIMToNSocketConnection,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Set the number of connections to be made.
  void SetNumberOfConnections(int);
  vtkGetMacro(NumberOfConnections, int);
  
  // Description:
  // Setup the wait for connection, but do not wait yet.
  // This should determine the network to be used and the port to be used.
  void SetupWaitForConnection();

  // Description:
  // SetupStartWaitForConnection must be called first.  This
  // method will start waiting for a connection to be made to it.
  void WaitForConnection();

  // Description:
  // Connect to remote server.
  void Connect();
  
  // Description:
  // Set up information about the remote connection.
  // 
  void SetPortInformation(unsigned int processNumber, int portNumber, const char* hostName);
  
  // Description:
  // Return the socket communicator for this process.
  vtkGetObjectMacro(SocketCommunicator, vtkSocketCommunicator);
  
  // Description:
  // Fill the port information values into the port information object.
  void GetPortInformation(vtkMPIMToNSocketConnectionPortInformation*);
  
protected:
  virtual void SetController(vtkMultiProcessController*);
  virtual void SetSocketCommunicator(vtkSocketCommunicator*);
  vtkMPIMToNSocketConnection();
  ~vtkMPIMToNSocketConnection();
private:
  int PortNumber;
  char* HostName;
  vtkSetStringMacro(HostName);
  int NumberOfConnections;
  vtkMPIMToNSocketConnectionInternals* Internals;
  vtkMultiProcessController *Controller;
  vtkSocketCommunicator* SocketCommunicator;
  vtkMPIMToNSocketConnection(const vtkMPIMToNSocketConnection&); // Not implemented
  void operator=(const vtkMPIMToNSocketConnection&); // Not implemented
};

#endif
