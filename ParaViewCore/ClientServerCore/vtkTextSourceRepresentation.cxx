/*=========================================================================

  Program:   ParaView
  Module:    vtkTextSourceRepresentation.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkTextSourceRepresentation.h"

#include "vtk3DWidgetRepresentation.h"
#include "vtkAbstractArray.h"
#include "vtkAbstractWidget.h"
#include "vtkDataSetAttributes.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMPIMoveData.h"
#include "vtkObjectFactory.h"
#include "vtkPointSource.h"
#include "vtkPolyData.h"
#include "vtkPVCacheKeeper.h"
#include "vtkPVRenderView.h"
#include "vtkTable.h"
#include "vtkTextRepresentation.h"
#include "vtkUnstructuredDataDeliveryFilter.h"
#include "vtkVariant.h"

vtkStandardNewMacro(vtkTextSourceRepresentation);
vtkCxxSetObjectMacro(vtkTextSourceRepresentation, TextWidgetRepresentation,
  vtk3DWidgetRepresentation);
//----------------------------------------------------------------------------
vtkTextSourceRepresentation::vtkTextSourceRepresentation()
{
  this->TextWidgetRepresentation = 0;

  this->CacheKeeper = vtkPVCacheKeeper::New();
  this->DataCollector = vtkUnstructuredDataDeliveryFilter::New();
  this->DataCollector->SetOutputDataType(VTK_POLY_DATA);

  vtkInformation* info = vtkInformation::New();
  info->Set(vtkPVRenderView::DATA_DISTRIBUTION_MODE(),
    vtkMPIMoveData::CLONE);
  this->DataCollector->ProcessViewRequest(info);
  info->Delete();

  vtkPointSource* source = vtkPointSource::New();
  source->SetNumberOfPoints(1);
  source->Update();

  this->DummyPolyData = vtkPolyData::New();
  this->DummyPolyData->ShallowCopy(source->GetOutputDataObject(0));
  source->Delete();

  this->CacheKeeper->SetInput(this->DummyPolyData);
}

//----------------------------------------------------------------------------
vtkTextSourceRepresentation::~vtkTextSourceRepresentation()
{
  this->SetTextWidgetRepresentation(0);
  this->DataCollector->Delete();
  this->DummyPolyData->Delete();
  this->CacheKeeper->Delete();
}

//----------------------------------------------------------------------------
void vtkTextSourceRepresentation::SetVisibility(bool val)
{
  this->Superclass::SetVisibility(val);
  if (this->TextWidgetRepresentation)
    {
    this->TextWidgetRepresentation->GetRepresentation()->SetVisibility(val);
    this->TextWidgetRepresentation->SetEnabled(val);
    }
}

//----------------------------------------------------------------------------
void vtkTextSourceRepresentation::SetInteractivity(bool val)
{
  if (this->TextWidgetRepresentation &&
    this->TextWidgetRepresentation->GetWidget())
    {
    this->TextWidgetRepresentation->GetWidget()->SetProcessEvents(val);
    }
}

//----------------------------------------------------------------------------
int vtkTextSourceRepresentation::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkTable");
  info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  return 1;
}

//----------------------------------------------------------------------------
bool vtkTextSourceRepresentation::AddToView(vtkView* view)
{
  if (this->TextWidgetRepresentation)
    {
    view->AddRepresentation(this->TextWidgetRepresentation);
    }
  return true;
}

//----------------------------------------------------------------------------
bool vtkTextSourceRepresentation::RemoveFromView(vtkView* view)
{
  if (this->TextWidgetRepresentation)
    {
    view->RemoveRepresentation(this->TextWidgetRepresentation);
    }
  return true;
}

//----------------------------------------------------------------------------
void vtkTextSourceRepresentation::MarkModified()
{
  if (!this->GetUseCache())
    {
    // Cleanup caches when not using cache.
    this->CacheKeeper->RemoveAllCaches();
    }
  this->Superclass::MarkModified();
}

//----------------------------------------------------------------------------
bool vtkTextSourceRepresentation::IsCached(double cache_key)
{
  return this->CacheKeeper->IsCached(cache_key);
}

//----------------------------------------------------------------------------
int vtkTextSourceRepresentation::RequestData(
  vtkInformation* request, vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  this->DataCollector->Modified();

  // Pass caching information to the cache keeper.
  this->CacheKeeper->SetCachingEnabled(this->GetUseCache());
  this->CacheKeeper->SetCacheTime(this->GetCacheKey());

  if (inputVector[0]->GetNumberOfInformationObjects()==1)
    {
    if (!this->GetUsingCacheForUpdate())
      {
      vtkTable* input = vtkTable::GetData(inputVector[0], 0);
      if (input->GetNumberOfRows() > 0 && input->GetNumberOfColumns() > 0)
        {
        this->DummyPolyData->GetFieldData()->ShallowCopy(input->GetRowData());
        }
      }
    this->DataCollector->SetInputConnection(this->CacheKeeper->GetOutputPort());
    }
  else
    {
    this->DataCollector->RemoveAllInputs();
    }

  // It is tempting to try to do the data delivery in RequestData() itself.
  // However, whenever a representation updates, ParaView GUI may have some
  // GatherInformation() requests that happen. That messes up with any
  // data-delivery code placed here. So we leave the data delivery to the
  // REQUEST_PREPARE_FOR_RENDER() pass.
  return this->Superclass::RequestData(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
int vtkTextSourceRepresentation::ProcessViewRequest(
  vtkInformationRequestKey* request_type,
  vtkInformation* inInfo, vtkInformation* outInfo)
{
  if (!this->GetVisibility())
    {
    return false;
    }

  if (request_type == vtkPVView::REQUEST_PREPARE_FOR_RENDER())
    {

    if (this->DeliveryTimeStamp < this->DataCollector->GetMTime())
      {
      outInfo->Set(vtkPVRenderView::NEEDS_DELIVERY(), 1);
      }
    }
  else if (request_type == vtkPVView::REQUEST_DELIVERY())
    {
    this->DataCollector->Modified();
    this->DataCollector->Update();

    // since there's no direct connection between the mapper and the collector,
    // we don't put an update-suppressor in the pipeline.

    vtkstd::string text;

    vtkFieldData* fieldData =
      this->DataCollector->GetOutputDataObject(0)->GetFieldData();
    vtkAbstractArray* array = fieldData->GetAbstractArray(0);
    if (array && array->GetNumberOfTuples() > 0)
      {
      text = array->GetVariantValue(0).ToString();
      }

    vtkTextRepresentation* repr = vtkTextRepresentation::SafeDownCast(
      this->TextWidgetRepresentation?
      this->TextWidgetRepresentation->GetRepresentation() : NULL);
    if (repr)
      {
      repr->SetText(text.c_str());
      }
    this->DeliveryTimeStamp.Modified();
    }

  return this->Superclass::ProcessViewRequest(request_type, inInfo, outInfo);
}


//----------------------------------------------------------------------------
void vtkTextSourceRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
