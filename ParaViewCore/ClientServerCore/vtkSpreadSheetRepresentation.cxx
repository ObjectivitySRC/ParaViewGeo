/*=========================================================================

  Program:   ParaView
  Module:    vtkSpreadSheetRepresentation.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSpreadSheetRepresentation.h"

#include "vtkBlockDeliveryPreprocessor.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"

vtkStandardNewMacro(vtkSpreadSheetRepresentation);
//----------------------------------------------------------------------------
vtkSpreadSheetRepresentation::vtkSpreadSheetRepresentation()
{
  this->SetNumberOfInputPorts(3);
  this->DataConditioner = vtkBlockDeliveryPreprocessor::New();
  this->DataConditioner->SetGenerateOriginalIds(1);

  this->ExtractedDataConditioner = vtkBlockDeliveryPreprocessor::New();
  this->ExtractedDataConditioner->SetGenerateOriginalIds(0);
}

//----------------------------------------------------------------------------
vtkSpreadSheetRepresentation::~vtkSpreadSheetRepresentation()
{
  this->DataConditioner->Delete();
  this->ExtractedDataConditioner->Delete();
}

//----------------------------------------------------------------------------
void vtkSpreadSheetRepresentation::SetFieldAssociation(int val)
{
  this->DataConditioner->SetFieldAssociation(val);
  this->ExtractedDataConditioner->SetFieldAssociation(val);
  this->MarkModified();
}

//----------------------------------------------------------------------------
int vtkSpreadSheetRepresentation::GetFieldAssociation()
{
  return this->DataConditioner->GetFieldAssociation();
}

//----------------------------------------------------------------------------
void vtkSpreadSheetRepresentation::SetCompositeDataSetIndex(int val)
{
  this->DataConditioner->SetCompositeDataSetIndex(val);
  this->ExtractedDataConditioner->SetCompositeDataSetIndex(val);
  this->MarkModified();
}

//----------------------------------------------------------------------------
int vtkSpreadSheetRepresentation::FillInputPortInformation(
  int port, vtkInformation* info)
{
  switch (port)
    {
  case 0:
  case 1:
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
    break;

  case 2:
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkSelection");
    break;

  default:
    return 0;
    }

  info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  return 1;
}

//----------------------------------------------------------------------------
int vtkSpreadSheetRepresentation::RequestData(
  vtkInformation* request, vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  if (vtkProcessModule::GetProcessType() ==
    vtkProcessModule::PROCESS_RENDER_SERVER)
    {
    return this->Superclass::RequestData(request, inputVector, outputVector);
    }

  this->DataConditioner->RemoveAllInputs();
  this->ExtractedDataConditioner->RemoveAllInputs();

  if (inputVector[0]->GetNumberOfInformationObjects() == 1)
    {
    this->DataConditioner->SetInputConnection(this->GetInternalOutputPort(0, 0));
    }
  if (inputVector[1]->GetNumberOfInformationObjects() == 1)
    {
    this->ExtractedDataConditioner->SetInputConnection(
      this->GetInternalOutputPort(1, 0));
    }

  if (this->GetNumberOfInputConnections(2) == 1)
    {
    this->GetInternalOutputPort(2, 0);
    }

  return this->Superclass::RequestData(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
vtkAlgorithmOutput* vtkSpreadSheetRepresentation::GetDataProducer()
{
  return this->DataConditioner->GetNumberOfInputConnections(0)==1?
    this->DataConditioner->GetOutputPort(0) : NULL;
}

//----------------------------------------------------------------------------
vtkAlgorithmOutput* vtkSpreadSheetRepresentation::GetExtractedDataProducer()
{
  return this->ExtractedDataConditioner->GetNumberOfInputConnections(0)==1?
    this->ExtractedDataConditioner->GetOutputPort(0) : NULL;
}

//----------------------------------------------------------------------------
vtkAlgorithmOutput* vtkSpreadSheetRepresentation::GetSelectionProducer()
{
  if (this->GetNumberOfInputConnections(2) == 1)
    {
    return this->GetInternalOutputPort(2, 0);
    }
  return NULL;
}


//----------------------------------------------------------------------------
void vtkSpreadSheetRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
