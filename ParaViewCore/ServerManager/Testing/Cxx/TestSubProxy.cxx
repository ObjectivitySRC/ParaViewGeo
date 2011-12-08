/*=========================================================================

Program:   ParaView
Module:    TestSubProxy.cxx

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
#include "vtkProcessModule.h"
#include "vtkPVFileInformation.h"
#include "vtkPVServerOptions.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMProxyManager.h"
#include "vtkSMSession.h"
#include "vtkSMSourceProxy.h"
#include "vtkPVDataInformation.h"

#include "vtkPVXMLElement.h"

//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  vtkPVServerOptions* options = vtkPVServerOptions::New();
  bool success = true;
  vtkInitializationHelper::Initialize(argc, argv,
    vtkProcessModule::PROCESS_BATCH, options);
  if (!success)
    {
    return -1;
    }

  vtkSMSession* session = vtkSMSession::New();
  cout << "Starting..." << endl;

  vtkSMProxyManager* pxm = vtkSMObject::GetProxyManager();

  // *******************************************************************
  // Test specific code
  // *******************************************************************
  double value, valueA, valueB;
  if(options->GetUnknownArgument())
    {
    cout << "Load Proxy definition: " << options->GetUnknownArgument() << endl;
    pxm->LoadConfigurationXML(options->GetUnknownArgument());
    }
  else
    {
    pxm->LoadConfigurationXML("/home/seb/Kitware/Projects/DOE-Collaboration-SBIR-II/code/git/ParaView4/Servers/ServerManager/Testing/Cxx/TestCustomSubProxyDefinition.xml");
    }

  // Create proxy and change main radius value
  vtkSMProxy* proxy = pxm->NewProxy("sources", "SphereSource3");
  vtkSMPropertyHelper(proxy, "Radius").Set(20);
  proxy->UpdateVTKObjects();

  // Read radius info
  vtkSMPropertyHelper(proxy, "Radius").Get(&value);
  vtkSMPropertyHelper(proxy, "RadiusA").Get(&valueA);
  vtkSMPropertyHelper(proxy, "RadiusB").Get(&valueB);
  cout << "Radius: " << value << " A: " << valueA << " B: " << valueB << endl;

  // Update radius for sub proxy A
  cout << "Update radiusA to 123.456" << endl;
  vtkSMPropertyHelper(proxy, "RadiusA").Set(123.456);
  proxy->UpdateVTKObjects();

  // Read radius info
  vtkSMPropertyHelper(proxy, "Radius").Get(&value);
  vtkSMPropertyHelper(proxy, "RadiusA").Get(&valueA);
  vtkSMPropertyHelper(proxy, "RadiusB").Get(&valueB);
  cout << "Radius: " << value << " A: " << valueA << " B: " << valueB << endl;

  //    vtkSmartPointer<vtkPVXMLElement> root = vtkSmartPointer<vtkPVXMLElement>::New();
  //    proxy->SaveState(root);
  //    root->PrintXML();

  // Update radius for sub proxy B
  cout << "Update radiusB to 654.321" << endl;
  vtkSMPropertyHelper(proxy, "RadiusB").Set(654.321);
  proxy->UpdateVTKObjects();

  // Read radius info
  vtkSMPropertyHelper(proxy, "Radius").Get(&value);
  vtkSMPropertyHelper(proxy, "RadiusA").Get(&valueA);
  vtkSMPropertyHelper(proxy, "RadiusB").Get(&valueB);
  cout << "Radius: " << value << " A: " << valueA << " B: " << valueB << endl;

  // *******************************************************************

  proxy->Delete();
  session->Delete();
  cout << "Exiting..." << endl;

  vtkInitializationHelper::Finalize();
  options->Delete();
  return EXIT_SUCCESS;
}
