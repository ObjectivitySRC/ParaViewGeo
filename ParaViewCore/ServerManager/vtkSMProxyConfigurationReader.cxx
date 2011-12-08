/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSMProxyConfigurationReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMProxyConfigurationReader.h"
#include "vtkSMCameraConfigurationFileInfo.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkPVXMLElement.h"
#include "vtkPVXMLParser.h"
#include "vtkSMProxy.h"

#include <vtkstd/string>

#define safeio(a) ((a)?(a):"NULL")

vtkStandardNewMacro(vtkSMProxyConfigurationReader);

//-----------------------------------------------------------------------------
vtkSMProxyConfigurationReader::vtkSMProxyConfigurationReader()
        :
    FileName(0),
    ValidateProxyType(1),
    Proxy(0),
    FileIdentifier(0),
    FileDescription(0),
    FileExtension(0)
{
  vtkSMCameraConfigurationFileInfo info;
  this->SetFileIdentifier(info.FileIdentifier);
  this->SetFileDescription(info.FileDescription);
  this->SetFileExtension(info.FileExtension);
}

//-----------------------------------------------------------------------------
vtkSMProxyConfigurationReader::~vtkSMProxyConfigurationReader()
{
  this->SetFileName(0);
  this->SetProxy(0);
  this->SetFileIdentifier(0);
  this->SetFileDescription(0);
  this->SetFileExtension(0);
}

//-----------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkSMProxyConfigurationReader,Proxy,vtkSMProxy);

//-----------------------------------------------------------------------------
bool vtkSMProxyConfigurationReader::CanReadVersion(const char *version)
{
  return vtkstd::string(version)==this->GetReaderVersion();
}

//-----------------------------------------------------------------------------
int vtkSMProxyConfigurationReader::ReadConfiguration()
{
  return this->ReadConfiguration(this->FileName);
}

//-----------------------------------------------------------------------------
int vtkSMProxyConfigurationReader::ReadConfiguration(const char *filename)
{
  if (filename==0)
    {
    vtkErrorMacro("Cannot read from filename NULL.");
    return 0;
    }

  vtkSmartPointer<vtkPVXMLParser> parser=vtkSmartPointer<vtkPVXMLParser>::New();
  parser->SetFileName(filename);
  if (parser->Parse()==0)
    {
    vtkErrorMacro("Invalid XML in file: " << filename << ".");
    return 0;
    }

  vtkPVXMLElement *xmlStream=parser->GetRootElement();
  if (xmlStream==0)
    {
    vtkErrorMacro("Invalid XML in file: " << filename << ".");
    return 0;
    }

  return this->ReadConfiguration(xmlStream);
}

//-----------------------------------------------------------------------------
int vtkSMProxyConfigurationReader::ReadConfiguration(vtkPVXMLElement *configXml)
{
  vtkstd::string requiredIdentifier(this->GetFileIdentifier());
  const char *foundIdentifier=configXml->GetName();
  if (foundIdentifier==0 || foundIdentifier!=requiredIdentifier)
    {
    vtkErrorMacro(
        << "This is not a valid " << this->GetFileDescription()
        << " XML hierarchy.");
    return 0;
    }

  const char *foundVersion=configXml->GetAttribute("version");
  if (foundVersion==0)
    {
    vtkErrorMacro("No \"version\" attribute was found.");
    return 0;
    }

  if (!this->CanReadVersion(foundVersion))
    {
    vtkErrorMacro("Unsupported version " << foundVersion << ".");
    return 0;
    }

  // Find a proxy emlement, this hierarchy is expected to contain one
  // and only one Proxy element.
  vtkPVXMLElement *proxyXml=configXml->FindNestedElementByName("Proxy");
  if (proxyXml==0)
    {
    vtkErrorMacro("No \"Proxy\" element was found.");
    return 0;
    }

  // Compare type of proxy in the file with the one we have to make
  // sure they match.
  const char *foundType=proxyXml->GetAttribute("type");
  vtkstd::string requiredType=this->Proxy->GetXMLName();
  if (this->ValidateProxyType
    && (foundType==0 || foundType!=requiredType))
    {
    vtkErrorMacro(
        << "This is not a valid " << requiredType
        << " XML hierarchy.");
    return 0;
    }

  // Push hierarchy to the proxy.
  int ok=this->Proxy->LoadXMLState(proxyXml,0);
  if (!ok)
    {
    vtkErrorMacro("Proxy::LoadState failed.");
    return 0;
    }
  this->Proxy->UpdateVTKObjects();

  return 1;
}

//-----------------------------------------------------------------------------
void vtkSMProxyConfigurationReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "FileName: " << safeio(this->FileName) << endl
     << indent << "Proxy: " << Proxy << endl
     << indent << "FileIdentifier: " << safeio(this->GetFileIdentifier()) << endl
     << indent << "FileDescription: " << safeio(this->GetFileDescription()) << endl
     << indent << "FileExtension: " << safeio(this->GetFileExtension()) << endl
     << indent << "ReaderVersion: " << safeio(this->GetReaderVersion()) << endl;
}

