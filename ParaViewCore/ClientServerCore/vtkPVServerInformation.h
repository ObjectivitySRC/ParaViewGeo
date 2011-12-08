/*=========================================================================

  Program:   ParaView
  Module:    vtkPVServerInformation.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPVServerInformation - Gets features of the server.
// .SECTION Description
// This objects is used by the client to get the features 
// suported by the server.
// At the moment, server information is only on the root.


#ifndef __vtkPVServerInformation_h
#define __vtkPVServerInformation_h

#include "vtkPVInformation.h"

class vtkClientServerStream;
class vtkPVServerOptionsInternals;

class VTK_EXPORT vtkPVServerInformation : public vtkPVInformation
{
public:
  static vtkPVServerInformation* New();
  vtkTypeMacro(vtkPVServerInformation, vtkPVInformation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This flag indicates whether the server can render remotely.
  // If it is off, all rendering has to be on the client.
  // This is only off when the user starts the server with 
  // the --disable-composite command line option.
  vtkSetMacro(RemoteRendering, int);
  vtkGetMacro(RemoteRendering, int);

  void DeepCopy(vtkPVServerInformation *info);

  // Description:
  // Transfer information about a single object into this object.
  virtual void CopyFromObject(vtkObject*);

  // Description:
  // Merge another information object.
  virtual void AddInformation(vtkPVInformation*);

  // Description:
  // Manage a serialized version of the information.
  virtual void CopyToStream(vtkClientServerStream*);
  virtual void CopyFromStream(const vtkClientServerStream*);

  // Description:
  // Varibles (command line argurments) set to render to a tiled display.
  vtkSetVector2Macro(TileDimensions, int);
  vtkGetVector2Macro(TileDimensions, int);
  vtkSetVector2Macro(TileMullions, int);
  vtkGetVector2Macro(TileMullions, int);

  // Description:
  // Variable (command line argument) to use offscreen rendering.
  vtkSetMacro(UseOffscreenRendering, int);
  vtkGetMacro(UseOffscreenRendering, int);

  // Description:
  // Returns 1 if IceT is available.
  vtkSetMacro(UseIceT, int);
  vtkGetMacro(UseIceT, int);

  // Description:
  // Name of render module to use.
  vtkSetStringMacro(RenderModuleName);
  vtkGetStringMacro(RenderModuleName);

  // Description:
  // Get/Set if the server supports saving OGVs.
  vtkSetMacro(OGVSupport, int);
  vtkGetMacro(OGVSupport, int);

  // Description:
  // Get/Set if the server supports saving AVIs.
  vtkSetMacro(AVISupport, int);
  vtkGetMacro(AVISupport, int);

  // Description:
  // Get/Set the time after which the server timesout.
  vtkSetMacro(Timeout, int);
  vtkGetMacro(Timeout, int);

  // Description:
  // Number of machines to use in data or render server
  // Setting the number of machines has the side effect of wiping out any
  // machine parameters previously set.
  void SetNumberOfMachines(unsigned int num);
  unsigned int GetNumberOfMachines() const;

  // Description:
  // Value of DISPLAY environment variable for this cave node
  void SetEnvironment(unsigned int idx, const char* name);
  const char* GetEnvironment(unsigned int idx) const;

  // Description:
  // Coordinates of lower left corner of this cave display
  void SetLowerLeft(unsigned int idx, double coord[3]);
  double* GetLowerLeft(unsigned int idx) const;

  // Description:
  // Coordinates of lower right corner of this cave display
  void SetLowerRight(unsigned int idx, double coord[3]);
  double* GetLowerRight(unsigned int idx) const;

  // Description:
  // Coordinates of lower left corner of this cave display
  void SetUpperRight(unsigned int idx, double coord[3]);
  double* GetUpperRight(unsigned int idx) const;

  // Description:
  // Get the number of processes.
  vtkGetMacro(NumberOfProcesses, int);
protected:
  vtkPVServerInformation();
  ~vtkPVServerInformation();

  int NumberOfProcesses;
  int OGVSupport;
  int AVISupport;
  int RemoteRendering;
  int TileDimensions[2];
  int TileMullions[2];
  int Timeout;
  int UseIceT;
  int UseOffscreenRendering;
  char* RenderModuleName;

  vtkPVServerOptionsInternals* MachinesInternals;

  vtkPVServerInformation(const vtkPVServerInformation&); // Not implemented
  void operator=(const vtkPVServerInformation&); // Not implemented
};

#endif
