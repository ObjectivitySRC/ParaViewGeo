/*=========================================================================

  Program:   ParaView
  Module:    vtkPVXMLParser.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVXMLParser.h"
#include "vtkObjectFactory.h"
#include "vtkPVXMLElement.h"
#include "vtksys/ios/sstream"

vtkStandardNewMacro(vtkPVXMLParser);

//----------------------------------------------------------------------------
vtkPVXMLParser::vtkPVXMLParser()
{
  this->FileName = 0;
  this->InputString = 0;
  this->NumberOfOpenElements = 0;
  this->OpenElementsSize = 10;
  this->OpenElements = new vtkPVXMLElement*[this->OpenElementsSize];
  this->ElementIdIndex = 0;
  this->RootElement = 0;
  this->SuppressErrorMessages = 0;
}

//----------------------------------------------------------------------------
vtkPVXMLParser::~vtkPVXMLParser()
{
  unsigned int i;
  for(i=0;i < this->NumberOfOpenElements;++i)
    {
    this->OpenElements[i]->Delete();
    }
  delete [] this->OpenElements;
  if(this->RootElement)
    {
    this->RootElement->Delete();
    }
  this->SetFileName(0);
}

//----------------------------------------------------------------------------
void vtkPVXMLParser::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "FileName: " << (this->FileName? this->FileName : "(none)")
     << "\n";
  os << indent << "SuppressErrorMessages: " << this->SuppressErrorMessages
     << "\n";
}

//----------------------------------------------------------------------------
void vtkPVXMLParser::StartElement(const char* name, const char** atts)
{
  vtkPVXMLElement* element = vtkPVXMLElement::New();
  element->SetName(name);
  element->ReadXMLAttributes(atts);
  const char* id = element->GetAttribute("id");
  if(id)
    {
    element->SetId(id);
    }
  else
    {
    vtksys_ios::ostringstream idstr;
    idstr << this->ElementIdIndex++ << ends;
    element->SetId(idstr.str().c_str());
    }
  this->PushOpenElement(element);
}

//----------------------------------------------------------------------------
void vtkPVXMLParser::EndElement(const char* vtkNotUsed(name))
{
  vtkPVXMLElement* finished = this->PopOpenElement();
  unsigned int numOpen = this->NumberOfOpenElements;
  if(numOpen > 0)
    {
    this->OpenElements[numOpen-1]->AddNestedElement(finished);
    finished->Delete();
    }
  else
    {
    this->RootElement = finished;
    }
}

//----------------------------------------------------------------------------
void vtkPVXMLParser::CharacterDataHandler(const char* data, int length)
{
  unsigned int numOpen = this->NumberOfOpenElements;
  if(numOpen > 0)
    {
    this->OpenElements[numOpen-1]->AddCharacterData(data, length);
    }
}

//----------------------------------------------------------------------------
void vtkPVXMLParser::PushOpenElement(vtkPVXMLElement* element)
{
  if(this->NumberOfOpenElements == this->OpenElementsSize)
    {
    unsigned int newSize = this->OpenElementsSize*2;
    vtkPVXMLElement** newOpenElements = new vtkPVXMLElement*[newSize];
    unsigned int i;
    for(i=0; i < this->NumberOfOpenElements;++i)
      {
      newOpenElements[i] = this->OpenElements[i];
      }
    delete [] this->OpenElements;
    this->OpenElements = newOpenElements;
    this->OpenElementsSize = newSize;
    }

  unsigned int pos = this->NumberOfOpenElements++;
  this->OpenElements[pos] = element;
}

//----------------------------------------------------------------------------
vtkPVXMLElement* vtkPVXMLParser::PopOpenElement()
{
  if(this->NumberOfOpenElements > 0)
    {
    --this->NumberOfOpenElements;
    return this->OpenElements[this->NumberOfOpenElements];
    }
  return 0;
}

//----------------------------------------------------------------------------
void vtkPVXMLParser::PrintXML(ostream& os)
{
  this->RootElement->PrintXML(os, vtkIndent());
}

//----------------------------------------------------------------------------
int vtkPVXMLParser::ParseXML()
{
  if (this->RootElement)
    {
    this->RootElement->Delete();
    this->RootElement = 0;
    }
  return this->Superclass::ParseXML();
}

//----------------------------------------------------------------------------
vtkPVXMLElement* vtkPVXMLParser::GetRootElement()
{
  return this->RootElement;
}

//-----------------------------------------------------------------------------
void vtkPVXMLParser::ReportXmlParseError()
{
  if (!this->SuppressErrorMessages)
    {
    this->Superclass::ReportXmlParseError();
    }
}
