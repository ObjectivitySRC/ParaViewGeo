/*=========================================================================

  Program:   ParaView
  Module:    vtkPVEnvironmentInformation.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVEnvironmentInformation.h"

#include "vtkObjectFactory.h"
#include <vtksys/SystemTools.hxx>
#include "vtkClientServerStream.h"
#include "vtkPVEnvironmentInformationHelper.h"

vtkStandardNewMacro(vtkPVEnvironmentInformation);

//-----------------------------------------------------------------------------
vtkPVEnvironmentInformation::vtkPVEnvironmentInformation()
{
  this->RootOnly = 1;
  this->Variable = NULL;
}

//-----------------------------------------------------------------------------
vtkPVEnvironmentInformation::~vtkPVEnvironmentInformation()
{
  this->SetVariable(NULL);
}

//-----------------------------------------------------------------------------
void vtkPVEnvironmentInformation::CopyFromObject(vtkObject* object)
{
  vtkPVEnvironmentInformationHelper* helper = 
    vtkPVEnvironmentInformationHelper::SafeDownCast(object);
  if (!helper)
    {
    vtkErrorMacro(
      "Can collect information only from a vtkPVEnvironmentInformationHelper.");
    return;
    }
  this->SetVariable(vtksys::SystemTools::GetEnv(helper->GetVariable()));
}

//-----------------------------------------------------------------------------
void vtkPVEnvironmentInformation::CopyToStream(vtkClientServerStream* stream)
{
  *stream << vtkClientServerStream::Reply
    << this->Variable;
  *stream << vtkClientServerStream::End;
}

//-----------------------------------------------------------------------------
void vtkPVEnvironmentInformation::CopyFromStream(const vtkClientServerStream* css)
{
  const char* temp = 0;
  if (!css->GetArgument(0, 0, &temp))
    {
    vtkErrorMacro("Error parsing Variable.");
    return;
    }
  this->SetVariable(temp);
}

//-----------------------------------------------------------------------------
void vtkPVEnvironmentInformation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Variable: " 
    << (this->Variable? this->Variable : "(none)") << endl;
}

