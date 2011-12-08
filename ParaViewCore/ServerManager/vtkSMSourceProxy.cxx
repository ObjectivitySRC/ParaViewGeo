/*=========================================================================

  Program:   ParaView
  Module:    vtkSMSourceProxy.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMSourceProxy.h"

#include "vtkClientServerStream.h"
#include "vtkCollection.h"
#include "vtkCommand.h"
#include "vtkDataSetAttributes.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkPVAlgorithmPortsInformation.h"
#include "vtkPVArrayInformation.h"
#include "vtkPVDataInformation.h"
#include "vtkPVDataSetAttributesInformation.h"
#include "vtkPVXMLElement.h"
#include "vtkSmartPointer.h"
#include "vtkSMDocumentation.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMIdTypeVectorProperty.h"
#include "vtkSMInputProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMOutputPort.h"
#include "vtkSMProxyManager.h"
#include "vtkSMStringVectorProperty.h"

#include <vtkstd/string>
#include <vtkstd/vector>
#include <vtksys/ios/sstream>

#define OUTPUT_PORTNAME_PREFIX "Output-"

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkSMSourceProxy);

// This struct will keep all information associated with the output port.
struct vtkSMSourceProxyOutputPort
{
  vtkSmartPointer<vtkSMOutputPort> Port;
  vtkSmartPointer<vtkSMDocumentation> Documentation;
  vtkstd::string Name;
};

struct vtkSMSourceProxyInternals
{
  typedef vtkstd::vector<vtkSMSourceProxyOutputPort> VectorOfPorts;
  VectorOfPorts OutputPorts;
  vtkstd::vector<vtkSmartPointer<vtkSMSourceProxy> > SelectionProxies;

  // Resizes output ports and ensures that Name for each port is initialized to
  // the default.
  void ResizeOutputPorts(unsigned int newsize)
    {
    this->OutputPorts.resize(newsize);
    VectorOfPorts::iterator it = this->OutputPorts.begin();
    for (unsigned int idx=0; it != this->OutputPorts.end(); it++, idx++)
      {
      if (it->Name.empty())
        {
        vtksys_ios::ostringstream nameStream;
        nameStream << OUTPUT_PORTNAME_PREFIX << idx;
        it->Name = nameStream.str();
        }
      }
    }

  void EnsureOutputPortsSize(unsigned int size)
    {
    if (this->OutputPorts.size() < size)
      {
      this->ResizeOutputPorts(size);
      }
    }
};

//---------------------------------------------------------------------------
vtkSMSourceProxy::vtkSMSourceProxy()
{
  this->SetSIClassName("vtkSISourceProxy");

  this->PInternals = new  vtkSMSourceProxyInternals;
  this->OutputPortsCreated = 0;

  this->ExecutiveName = 0;
  this->SetExecutiveName("vtkCompositeDataPipeline");

  this->SelectionProxiesCreated = false;

  this->NumberOfAlgorithmOutputPorts = VTK_UNSIGNED_INT_MAX;
  this->NumberOfAlgorithmRequiredInputPorts = VTK_UNSIGNED_INT_MAX;
  this->ProcessSupport = vtkSMSourceProxy::BOTH;
}

//---------------------------------------------------------------------------
vtkSMSourceProxy::~vtkSMSourceProxy()
{
  delete this->PInternals;

  this->SetExecutiveName(0);
}

//---------------------------------------------------------------------------
unsigned int vtkSMSourceProxy::GetNumberOfOutputPorts()
{
  return this->PInternals->OutputPorts.size();
}

//---------------------------------------------------------------------------
vtkSMOutputPort* vtkSMSourceProxy::GetOutputPort(unsigned int idx)
{
  return idx==VTK_UNSIGNED_INT_MAX? NULL:
    this->PInternals->OutputPorts[idx].Port.GetPointer();
}

//---------------------------------------------------------------------------
vtkSMOutputPort* vtkSMSourceProxy::GetOutputPort(const char* portname)
{
  return this->GetOutputPort(this->GetOutputPortIndex(portname));
}

//---------------------------------------------------------------------------
unsigned int vtkSMSourceProxy::GetOutputPortIndex(const char* portname)
{
  // Since there are not really going to be hundreds of output ports, the is not
  // going to be any noticable difference in accessing output ports by index or
  // by name.
  vtkSMSourceProxyInternals::VectorOfPorts::iterator it =
    this->PInternals->OutputPorts.begin();
  for (unsigned int idx=0; it != this->PInternals->OutputPorts.end(); it++, idx++)
    {
    if (it->Name == portname)
      {
      return idx;
      }
    }

  return VTK_UNSIGNED_INT_MAX;
}

//---------------------------------------------------------------------------
const char* vtkSMSourceProxy::GetOutputPortName(unsigned int index)
{
  if (index >= this->PInternals->OutputPorts.size())
    {
    return 0;
    }
  
  return this->PInternals->OutputPorts[index].Name.c_str();
}

//---------------------------------------------------------------------------
vtkSMDocumentation* vtkSMSourceProxy::GetOutputPortDocumentation(
  unsigned int index)
{
  if (index >= this->PInternals->OutputPorts.size())
    {
    return 0;
    }

  return this->PInternals->OutputPorts[index].Documentation;
}

//---------------------------------------------------------------------------
vtkSMDocumentation* vtkSMSourceProxy::GetOutputPortDocumentation(
  const char* portname)
{
  return this->GetOutputPortDocumentation(
    this->GetOutputPortIndex(portname));
}

//---------------------------------------------------------------------------
void vtkSMSourceProxy::UpdatePipelineInformation()
{
  if (this->ObjectsCreated)
    {
    vtkClientServerStream stream;
    stream << vtkClientServerStream::Invoke
           << SIPROXY(this)
           << "UpdatePipelineInformation"
           << vtkClientServerStream::End;
    this->ExecuteStream(stream);
    }

  // This simply iterates over subproxies and calls UpdatePropertyInformation();
  this->Superclass::UpdatePipelineInformation();

  this->InvokeEvent(vtkCommand::UpdateInformationEvent);
  // this->MarkModified(this);  
}
//---------------------------------------------------------------------------
int vtkSMSourceProxy::ReadXMLAttributes(vtkSMProxyManager* pm, 
                                        vtkPVXMLElement* element)
{
  const char* executiveName = element->GetAttribute("executive");
  if (executiveName)
    {
    this->SetExecutiveName(executiveName);
    }
  const char* mp = element->GetAttribute("multiprocess_support");
  if (mp)
    {
    if (strcmp(mp, "multiple_processes") == 0)
      {
      this->ProcessSupport = vtkSMSourceProxy::MULTIPLE_PROCESSES;
      }
    else if (strcmp(mp, "single_process") == 0)
      {
      this->ProcessSupport = vtkSMSourceProxy::SINGLE_PROCESS;
      }
    else
      {
      this->ProcessSupport = vtkSMSourceProxy::BOTH;
      }
    }

  int port_count = 0;
  unsigned int numElems = element->GetNumberOfNestedElements();
  for (unsigned int cc=0; cc < numElems; cc++)
    {
    vtkPVXMLElement* child = element->GetNestedElement(cc);
    if (child && child->GetName() && 
      strcmp(child->GetName(), "OutputPort")==0)
      {
      // Load output port configuration.
      int index = 0;
      if (!child->GetScalarAttribute("index", &index))
        {
        index = port_count;
        }
      const char* portname= child->GetAttribute("name");
      if (!portname)
        {
        vtkErrorMacro("Missing OutputPort attribute 'name'.");
        return 0;
        }
      port_count++;
      this->PInternals->EnsureOutputPortsSize(index+1); 
      this->PInternals->OutputPorts[index].Name = portname;

      // Load output port documentation.
      for (unsigned int kk=0; kk < child->GetNumberOfNestedElements(); ++kk)
        {
        vtkPVXMLElement* subElem = child->GetNestedElement(kk);
        if (strcmp(subElem->GetName(), "Documentation") == 0)
          {
          this->Documentation->SetDocumentationElement(subElem);
          vtkSMDocumentation* doc = vtkSMDocumentation::New();
          doc->SetDocumentationElement(subElem);
          this->PInternals->OutputPorts[index].Documentation = doc;
          doc->Delete();
          }
        }
      }
    }
  return this->Superclass::ReadXMLAttributes(pm, element);
}

//---------------------------------------------------------------------------
// Call Update() on all sources
// TODO this should update information properties.
void vtkSMSourceProxy::UpdatePipeline()
{
  if (!this->NeedsUpdate)
    {
    return;
    }

  this->CreateOutputPorts(); 
  int num = this->GetNumberOfOutputPorts();
  for (int i=0; i < num; ++i)
    {
    this->GetOutputPort(i)->UpdatePipeline();
    }

  this->PostUpdateData();
  //this->InvalidateDataInformation();
}

//---------------------------------------------------------------------------
// Call Update() on all sources with given time
// TODO this should update information properties.
void vtkSMSourceProxy::UpdatePipeline(double time)
{
  int i;

  this->CreateOutputPorts();
  int num = this->GetNumberOfOutputPorts();
  for (i=0; i < num; ++i)
    {
    this->GetOutputPort(i)->UpdatePipeline(time);
    }

  // When calling UpdatePipeline() we check if this->NeedsUpdate is true and
  // call the real update only if that's the case. We don't do that when one
  // uses UpdatePipeline(time) since we can never be too sure what time was
  // used. In that case, we assume the pipeline needs update and hence we should
  // set the NeedsUpdate ivar to true as well. Otherwise PostUpdateData()
  // doesn't fire the necessary events and that can cause problems (BUG
  // #12571).
  this->NeedsUpdate = true;

  this->PostUpdateData();
  //this->InvalidateDataInformation();
}

//---------------------------------------------------------------------------
void vtkSMSourceProxy::CreateVTKObjects()
{
  if (this->ObjectsCreated)
    {
    return;
    }
  this->Superclass::CreateVTKObjects();

  // We are going to fix the ports such that we don't have to update the
  // pipeline or even UpdateInformation() to create the ports.
  this->CreateOutputPorts();
}

//---------------------------------------------------------------------------
unsigned int vtkSMSourceProxy::GetNumberOfAlgorithmOutputPorts()
{
  if (this->NumberOfAlgorithmOutputPorts != VTK_UNSIGNED_INT_MAX)
    {
    // avoids unnecessary information gathers.
    return this->NumberOfAlgorithmOutputPorts;
    }

  if (this->ObjectsCreated)
    {
    vtkSmartPointer<vtkPVAlgorithmPortsInformation> info =
      vtkSmartPointer<vtkPVAlgorithmPortsInformation>::New();
    this->GatherInformation(info);
    this->NumberOfAlgorithmOutputPorts = info->GetNumberOfOutputs();
    this->NumberOfAlgorithmRequiredInputPorts = info->GetNumberOfRequiredInputs();
    return this->NumberOfAlgorithmOutputPorts;
    }

  return 0;
}

//---------------------------------------------------------------------------
unsigned int vtkSMSourceProxy::GetNumberOfAlgorithmRequiredInputPorts()
{
  this->GetNumberOfAlgorithmOutputPorts();

  if (this->NumberOfAlgorithmRequiredInputPorts != VTK_UNSIGNED_INT_MAX)
    {
    // avoid unnecessary information gathers.
    return this->NumberOfAlgorithmRequiredInputPorts;
    }

  return 0;
}

//---------------------------------------------------------------------------
void vtkSMSourceProxy::CreateOutputPorts()
{
  if (this->OutputPortsCreated && this->GetNumberOfOutputPorts())
    {
    return;
    }
  this->OutputPortsCreated = 1;

  // This will only create objects if they are not already created.
  // This happens when connecting a filter to a source which is not
  // initialized. In other situations, SetInput() creates the VTK
  // objects before this gets called.
  this->CreateVTKObjects();

  // We simply set/replace the Port pointers in the
  // this->PInternals->OutputPorts. This ensures that the port names,
  // port documentation is preserved.

  // Create one output port proxy for each output of the filter
  int numOutputs = this->GetNumberOfAlgorithmOutputPorts();

  // Ensure that output ports size matches the number of output ports provided
  // by the algorithm.
  this->PInternals->ResizeOutputPorts(numOutputs);
  for (int j=0; j<numOutputs; j++)
    {
    vtkSMOutputPort* opPort = vtkSMOutputPort::New();
    opPort->SetPortIndex(j);
    opPort->SetSourceProxy(this);
    this->PInternals->OutputPorts[j].Port = opPort;
    opPort->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkSMSourceProxy::SetOutputPort(unsigned int index, const char* name, 
  vtkSMOutputPort* port, vtkSMDocumentation* doc)
{
  this->PInternals->EnsureOutputPortsSize(index+1);
  this->PInternals->OutputPorts[index].Name = name;
  this->PInternals->OutputPorts[index].Port = port;
  this->PInternals->OutputPorts[index].Documentation = doc;
  if (port && port->GetSourceProxy() == NULL)
    {
    port->SetSourceProxy(this);
    }
}

//----------------------------------------------------------------------------
void vtkSMSourceProxy::RemoveAllOutputPorts()
{
  this->PInternals->OutputPorts.clear();
}

//----------------------------------------------------------------------------
void vtkSMSourceProxy::PostUpdateData()
{
  this->InvalidateDataInformation();
  this->Superclass::PostUpdateData();
}

//----------------------------------------------------------------------------
void vtkSMSourceProxy::MarkDirty(vtkSMProxy* modifiedProxy)
{
  // Mark the extract selection proxies modified as well.
  // This is needed to be done explicitly since we don't use vtkSMInputProperty
  // to connect this proxy to the input of the extract selection filter.
  vtkstd::vector<vtkSmartPointer<vtkSMSourceProxy> >::iterator iter;
  for (iter = this->PInternals->SelectionProxies.begin();
    iter != this->PInternals->SelectionProxies.end(); ++iter)
    {
    iter->GetPointer()->MarkDirty(modifiedProxy);
    }

  this->Superclass::MarkDirty(modifiedProxy);
  // this->InvalidateDataInformation();
}

//----------------------------------------------------------------------------
vtkPVDataInformation* vtkSMSourceProxy::GetDataInformation(
  unsigned int idx)
{
  this->CreateOutputPorts();
  if (idx >= this->GetNumberOfOutputPorts())
    {
    return 0;
    }

  return this->GetOutputPort(idx)->GetDataInformation();
}

//----------------------------------------------------------------------------
void vtkSMSourceProxy::InvalidateDataInformation()
{
  if (this->OutputPortsCreated)
    {
    vtkSMSourceProxyInternals::VectorOfPorts::iterator it =
      this->PInternals->OutputPorts.begin();
    for (; it != this->PInternals->OutputPorts.end(); it++)
      {
      it->Port.GetPointer()->InvalidateDataInformation();
      }
    }
}

//---------------------------------------------------------------------------
void vtkSMSourceProxy::CreateSelectionProxies()
{
  if (this->SelectionProxiesCreated)
    {
    return;
    }
  this->CreateOutputPorts();

  vtkClientServerStream stream;
  vtkSMProxyManager* pxm = this->GetProxyManager();
  unsigned int numOutputPorts = this->GetNumberOfOutputPorts();
  for (unsigned int cc=0; cc < numOutputPorts; cc++)
    {
    vtkSmartPointer<vtkSMSourceProxy> esProxy;
    esProxy.TakeReference(vtkSMSourceProxy::SafeDownCast(
        pxm->NewProxy("filters", "PVExtractSelection")));
    if (esProxy)
      {
      esProxy->SetLocation(this->Location);
      // since we don't want the ExtractSelection proxy to further create
      // extract selection proxies even by accident :).
      esProxy->SelectionProxiesCreated = true;
      esProxy->UpdateVTKObjects();

      // We don't use input property since that leads to reference loop cycles
      // and I don't feel like doing the garbage collection thing right now.
      stream << vtkClientServerStream::Invoke
             << SIPROXY(this)
             << "SetupSelectionProxy"
             << cc
             << SIPROXY(esProxy)
             << vtkClientServerStream::End;
      }

    this->PInternals->SelectionProxies.push_back(esProxy);
    }
  this->ExecuteStream(stream);
  this->SelectionProxiesCreated = true;
}

//---------------------------------------------------------------------------
void vtkSMSourceProxy::SetSelectionInput(unsigned int portIndex,
  vtkSMSourceProxy* input, unsigned int outputport)
{
  this->CreateSelectionProxies();

  if (this->PInternals->SelectionProxies.size() <= portIndex)
    {
    return;
    }
  vtkSMSourceProxy* esProxy = this->PInternals->SelectionProxies[portIndex];
  if (esProxy)
    {
    vtkSMInputProperty* pp = vtkSMInputProperty::SafeDownCast(
      esProxy->GetProperty("Selection"));
    pp->RemoveAllProxies();
    pp->AddInputConnection(input, outputport);
    esProxy->UpdateVTKObjects();
    this->InvokeEvent(vtkCommand::SelectionChangedEvent,&portIndex);
    }
}

//---------------------------------------------------------------------------
vtkSMSourceProxy* vtkSMSourceProxy::GetSelectionInput(unsigned int portIndex)
{
  if (this->PInternals->SelectionProxies.size() > portIndex)
    {
    vtkSMSourceProxy* esProxy = this->PInternals->SelectionProxies[portIndex];
    if (esProxy)
      {
      vtkSMInputProperty* pp = vtkSMInputProperty::SafeDownCast(
        esProxy->GetProperty("Selection"));
      if (pp->GetNumberOfProxies() == 1)
        {
        return vtkSMSourceProxy::SafeDownCast(pp->GetProxy(0));
        }
      }
    }
  return 0;
}

//---------------------------------------------------------------------------
unsigned int vtkSMSourceProxy::GetSelectionInputPort(unsigned int portIndex)
{
  if (this->PInternals->SelectionProxies.size() > portIndex)
    {
    vtkSMSourceProxy* esProxy = this->PInternals->SelectionProxies[portIndex];
    if (esProxy)
      {
      vtkSMInputProperty* pp = vtkSMInputProperty::SafeDownCast(
        esProxy->GetProperty("Selection"));
      if (pp->GetNumberOfProxies() == 1)
        {
        return pp->GetOutputPortForConnection(portIndex);
        }
      }
    }
  return 0;
}

//---------------------------------------------------------------------------
void vtkSMSourceProxy::CleanSelectionInputs(unsigned int portIndex)
{
  if (this->PInternals->SelectionProxies.size() <= portIndex)
    {
    return;
    }
  vtkSMSourceProxy* esProxy = this->PInternals->SelectionProxies[portIndex];
  if (esProxy)
    {
    vtkSMInputProperty* pp = vtkSMInputProperty::SafeDownCast(
      esProxy->GetProperty("Selection"));
    pp->RemoveAllProxies();
    esProxy->UpdateVTKObjects();
    this->InvokeEvent(vtkCommand::SelectionChangedEvent,&portIndex);
    }
}

//---------------------------------------------------------------------------
vtkSMSourceProxy* vtkSMSourceProxy::GetSelectionOutput(unsigned int portIndex)
{
  if (this->PInternals->SelectionProxies.size() > portIndex)
    {
    return this->PInternals->SelectionProxies[portIndex];
    }

  return 0;
}

//---------------------------------------------------------------------------
void vtkSMSourceProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "OutputPortsCreated: " << this->OutputPortsCreated << endl;
  os << indent << "ProcessSupport: " << this->ProcessSupport << endl;
}
