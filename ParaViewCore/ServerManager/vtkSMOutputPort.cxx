/*=========================================================================

  Program:   ParaView
  Module:    vtkSMOutputPort.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMOutputPort.h"

#include "vtkAlgorithm.h"
#include "vtkCollection.h"
#include "vtkCollectionIterator.h"
#include "vtkCommand.h"
#include "vtkDataObject.h"
#include "vtkObjectFactory.h"
#include "vtkPVClassNameInformation.h"
#include "vtkPVDataInformation.h"
#include "vtkPVTemporalDataInformation.h"
#include "vtkPVXMLElement.h"
#include "vtkProcessModule.h"
#include "vtkSMMessage.h"
#include "vtkSMSession.h"
#include "vtkSMSourceProxy.h"
#include "vtkTimerLog.h"

#include <vtksys/ios/sstream>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSMOutputPort);

bool vtkSMOutputPort::UseStreaming = false;
int vtkSMOutputPort::DefaultPass = 0;
int vtkSMOutputPort::DefaultNumPasses = 0;
double vtkSMOutputPort::DefaultResolution = 0;
//----------------------------------------------------------------------------
vtkSMOutputPort::vtkSMOutputPort()
{
  this->ClassNameInformation = vtkPVClassNameInformation::New();
  this->DataInformation = vtkPVDataInformation::New();
  this->TemporalDataInformation = vtkPVTemporalDataInformation::New();
  this->ClassNameInformationValid = 0;
  this->DataInformationValid = false;
  this->TemporalDataInformationValid = false;
  this->PortIndex = 0;
  this->SourceProxy = 0;
  this->ObjectsCreated = 1;
}

//----------------------------------------------------------------------------
vtkSMOutputPort::~vtkSMOutputPort()
{
  this->SetSourceProxy(0);
  this->ClassNameInformation->Delete();
  this->DataInformation->Delete();
  this->TemporalDataInformation->Delete();
}

//----------------------------------------------------------------------------
vtkPVDataInformation* vtkSMOutputPort::GetDataInformation()
{
  if (!this->DataInformationValid)
    {
    vtksys_ios::ostringstream mystr;
    mystr << this->GetSourceProxy()->GetXMLName() << "::GatherInformation";
    vtkTimerLog::MarkStartEvent(mystr.str().c_str());
    this->GatherDataInformation();
    vtkTimerLog::MarkEndEvent(mystr.str().c_str());
    }
  return this->DataInformation;
}

//----------------------------------------------------------------------------
vtkPVTemporalDataInformation* vtkSMOutputPort::GetTemporalDataInformation()
{
  if (!this->TemporalDataInformationValid)
    {
    this->GatherTemporalDataInformation();
    }
  return this->TemporalDataInformation;
}

//----------------------------------------------------------------------------
vtkPVClassNameInformation* vtkSMOutputPort::GetClassNameInformation()
{
  if(this->ClassNameInformationValid == 0)
    {
    this->GatherClassNameInformation();
    }
  return this->ClassNameInformation;
}

//----------------------------------------------------------------------------
const char* vtkSMOutputPort::GetDataClassName()
{
  return this->GetClassNameInformation()->GetVTKClassName();
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::InvalidateDataInformation()
{
  this->DataInformationValid = false;
  this->ClassNameInformationValid = false;
  this->TemporalDataInformationValid = false;
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::GatherDataInformation()
{
  if (!this->SourceProxy)
    {
    vtkErrorMacro("Invalid vtkSMOutputPort.");
    return;
    }

  this->SourceProxy->GetSession()->PrepareProgress();
  this->DataInformation->Initialize();
  this->DataInformation->SetPortNumber(this->PortIndex);
  this->SourceProxy->GatherInformation(this->DataInformation);
  this->DataInformationValid = true;
  this->SourceProxy->GetSession()->CleanupPendingProgress();
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::GatherTemporalDataInformation()
{
  if (!this->SourceProxy)
    {
    vtkErrorMacro("Invalid vtkSMOutputPort.");
    return;
    }

  this->SourceProxy->GetSession()->PrepareProgress();
  this->TemporalDataInformation->Initialize();
  this->TemporalDataInformation->SetPortNumber(this->PortIndex);
  this->SourceProxy->GatherInformation(this->TemporalDataInformation);

  this->TemporalDataInformationValid = true;
  this->SourceProxy->GetSession()->CleanupPendingProgress();
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::GatherClassNameInformation()
{
  if (!this->SourceProxy)
    {
    vtkErrorMacro("Invalid vtkSMOutputPort.");
    return;
    }


  this->ClassNameInformation->SetPortNumber(this->PortIndex);
  vtkObjectBase* cso = this->GetSourceProxy()->GetClientSideObject();
  if (cso)
    {
    this->ClassNameInformation->CopyFromObject(
      vtkAlgorithm::SafeDownCast(cso)->GetOutputDataObject(
        this->PortIndex));
    }
  else
    {
    this->SourceProxy->GatherInformation(this->ClassNameInformation);
    }
  this->ClassNameInformationValid = 1;
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::UpdatePipeline()
{
  this->UpdatePipelineInternal(0.0, false);
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::UpdatePipeline(double time)
{
  this->UpdatePipelineInternal(time, true);
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::UpdatePipelineInternal(double time,
                                             bool doTime)
{
  this->SourceProxy->GetSession()->PrepareProgress();
  vtkClientServerStream stream;
  if (vtkSMOutputPort::UseStreaming)
    {
    stream << vtkClientServerStream::Invoke
           << SIPROXY(this->SourceProxy)
           << "UpdateStreamingPipeline"
           << vtkSMOutputPort::DefaultPass //pass
           << vtkSMOutputPort::DefaultNumPasses //number of passes
           << vtkSMOutputPort::DefaultResolution //resolution
           << this->PortIndex << time << (doTime? 1:0)
           << vtkClientServerStream::End;
    }
  else
    {
    stream << vtkClientServerStream::Invoke
           << SIPROXY(this->SourceProxy)
           << "UpdatePipeline"
           << this->PortIndex << time << (doTime? 1 : 0)
           << vtkClientServerStream::End;
    }
  this->SourceProxy->ExecuteStream(stream);
  this->SourceProxy->GetSession()->CleanupPendingProgress();
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "PortIndex: " << this->PortIndex << endl;
  os << indent << "SourceProxy: " << this->SourceProxy << endl;
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::SetUseStreaming(bool value)
{
  vtkSMOutputPort::UseStreaming = value;
}

//----------------------------------------------------------------------------
void vtkSMOutputPort::SetDefaultPiece(int dp, int dnp, double dr)
{
  vtkSMOutputPort::DefaultPass = dp;
  vtkSMOutputPort::DefaultNumPasses = dnp;
  vtkSMOutputPort::DefaultResolution = dr;
}
