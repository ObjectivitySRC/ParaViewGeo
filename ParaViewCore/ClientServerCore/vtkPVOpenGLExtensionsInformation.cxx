/*=========================================================================

  Program:   ParaView
  Module:    vtkPVOpenGLExtensionsInformation.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVOpenGLExtensionsInformation.h"

#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkProcessModule.h"
#include "vtkPVDisplayInformation.h"
#include "vtkRenderWindow.h"
#include "vtkSmartPointer.h"

#include <vtkstd/string>
#include <vtkstd/vector>
#include <vtksys/SystemTools.hxx>
#include <vtkstd/set>
#include <vtkstd/algorithm>
#include <vtkstd/iterator>

//-----------------------------------------------------------------------------
class vtkPVOpenGLExtensionsInformationInternal
{
public:
  typedef vtkstd::set<vtkstd::string> SetOfStrings;
  SetOfStrings Extensions;
};

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPVOpenGLExtensionsInformation);
//-----------------------------------------------------------------------------
vtkPVOpenGLExtensionsInformation::vtkPVOpenGLExtensionsInformation()
{
  this->Internal = new vtkPVOpenGLExtensionsInformationInternal;
  this->RootOnly = 1;
}

//-----------------------------------------------------------------------------
vtkPVOpenGLExtensionsInformation::~vtkPVOpenGLExtensionsInformation()
{
  delete this->Internal;
}

//-----------------------------------------------------------------------------
void vtkPVOpenGLExtensionsInformation::CopyFromObject(vtkObject* obj)
{
  this->Internal->Extensions.clear();


  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  if (!pm)
    {
    vtkErrorMacro("No vtkProcessModule!");
    return;
    }
  vtkSmartPointer<vtkPVDisplayInformation> di = 
    vtkSmartPointer<vtkPVDisplayInformation>::New();
  di->CopyFromObject(pm);
  if (!di->GetCanOpenDisplay())
    {
    return;
    }

  vtkRenderWindow* renWin = vtkRenderWindow::SafeDownCast(obj);
  if (!renWin)
    {
    vtkErrorMacro("Cannot downcast to render window.");
    return;
    }
  vtkOpenGLExtensionManager* mgr = vtkOpenGLExtensionManager::New();
  mgr->SetRenderWindow(renWin);
  mgr->Update();

  vtkstd::vector<vtkstd::string> extensions;
  vtksys::SystemTools::Split(mgr->GetExtensionsString(), 
    extensions, ' ');

  this->Internal->Extensions.clear();
  vtkstd::vector<vtkstd::string>::iterator iter;
  for (iter =  extensions.begin(); iter != extensions.end(); iter++)
    {
    this->Internal->Extensions.insert(*iter);
    }
  mgr->Delete();
}

//-----------------------------------------------------------------------------
void vtkPVOpenGLExtensionsInformation::AddInformation(vtkPVInformation* pvinfo)
{
  if (!pvinfo)
    {
    return;
    }

  vtkPVOpenGLExtensionsInformation* info = 
    vtkPVOpenGLExtensionsInformation::SafeDownCast(pvinfo);
  if (!info)
    {
    vtkErrorMacro("Could not downcast to vtkPVOpenGLExtensionsInformation.");
    return;
    }
  vtkstd::set<vtkstd::string> setSelf = this->Internal->Extensions;
  vtkstd::set<vtkstd::string> &setOther = info->Internal->Extensions;

  this->Internal->Extensions.clear();
  vtkstd::set_intersection(setSelf.begin(), setSelf.end(),
    setOther.begin(), setOther.end(),
    vtkstd::inserter(this->Internal->Extensions, this->Internal->Extensions.begin()));
}

//-----------------------------------------------------------------------------
void vtkPVOpenGLExtensionsInformation::CopyToStream(vtkClientServerStream* css)
{
  css->Reset();
  *css << vtkClientServerStream::Reply;

  vtkstd::string data;
  vtkPVOpenGLExtensionsInformationInternal::SetOfStrings::iterator iter;
  for (iter = this->Internal->Extensions.begin(); 
    iter != this->Internal->Extensions.end();
    ++iter)
    {
    data += (*iter) + " ";
    }

  *css << data.c_str();
  *css << vtkClientServerStream::End;
}

//-----------------------------------------------------------------------------
void vtkPVOpenGLExtensionsInformation::CopyFromStream(
  const vtkClientServerStream* css)
{
  this->Internal->Extensions.clear();
  const char* ext = 0;
  if (!css->GetArgument(0, 0, &ext))
    {
    vtkErrorMacro("Error parsing extensions string from message.");
    return;
    }

  vtkstd::vector<vtkstd::string> extensions;
  vtksys::SystemTools::Split(ext, extensions, ' ');
  vtkstd::vector<vtkstd::string>::iterator iter;
  for (iter =  extensions.begin(); iter != extensions.end(); ++iter)
    {
    this->Internal->Extensions.insert(*iter);
    }
}

//-----------------------------------------------------------------------------
bool vtkPVOpenGLExtensionsInformation::ExtensionSupported(const char* ext)
{
  vtkPVOpenGLExtensionsInformationInternal::SetOfStrings::iterator iter
    = this->Internal->Extensions.find(ext);
  return (iter != this->Internal->Extensions.end());
}

//-----------------------------------------------------------------------------
void vtkPVOpenGLExtensionsInformation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Supported Extensions: " << endl;
  vtkPVOpenGLExtensionsInformationInternal::SetOfStrings::iterator iter;
  for (iter = this->Internal->Extensions.begin(); 
    iter != this->Internal->Extensions.end();
    ++iter)
    {
    os << indent.GetNextIndent() << *iter << endl;
    }
}
