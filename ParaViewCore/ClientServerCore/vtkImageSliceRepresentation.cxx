/*=========================================================================

  Program:   ParaView
  Module:    vtkImageSliceRepresentation.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkImageSliceRepresentation.h"

#include "vtkCommand.h"
#include "vtkExtractVOI.h"
#include "vtkImageData.h"
#include "vtkImageSliceDataDeliveryFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkPVCacheKeeper.h"
#include "vtkPVImageSliceMapper.h"
#include "vtkPVLODActor.h"
#include "vtkPVRenderView.h"
#include "vtkRenderer.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkStandardNewMacro(vtkImageSliceRepresentation);
//----------------------------------------------------------------------------
vtkImageSliceRepresentation::vtkImageSliceRepresentation()
{
  this->Slice = 0;
  this->SliceMode = XY_PLANE;

  this->SliceData = vtkImageData::New();
  this->CacheKeeper = vtkPVCacheKeeper::New();
  this->CacheKeeper->SetInput(this->SliceData);

  this->DeliveryFilter = vtkImageSliceDataDeliveryFilter::New();
  this->SliceMapper = vtkPVImageSliceMapper::New();
  this->Actor = vtkPVLODActor::New();
  this->Actor->SetMapper(this->SliceMapper);
}

//----------------------------------------------------------------------------
vtkImageSliceRepresentation::~vtkImageSliceRepresentation()
{
  this->SliceData->Delete();
  this->CacheKeeper->Delete();
  this->DeliveryFilter->Delete();
  this->SliceMapper->SetInput(0);
  this->SliceMapper->Delete();
  this->Actor->Delete();
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetColorAttributeType(int type)
{
  switch (type)
    {
  case POINT_DATA:
    this->SliceMapper->SetScalarMode(VTK_SCALAR_MODE_USE_POINT_FIELD_DATA);
    break;

  case CELL_DATA:
    this->SliceMapper->SetScalarMode(VTK_SCALAR_MODE_USE_CELL_FIELD_DATA);
    break;

  default:
    vtkErrorMacro("Attribute type not supported: " << type);
    }
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetSliceMode(int mode)
{
  if (this->SliceMode != mode)
    {
    this->SliceMode = mode;
    this->MarkModified();
    }
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetSlice(unsigned int val)
{
  if (this->Slice != val)
    {
    this->Slice = val;
    this->MarkModified();
    }
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetColorArrayName(const char* name)
{
  if (name && name[0])
    {
    this->SliceMapper->SelectColorArray(name);
    }
  else
    {
    this->SliceMapper->SelectColorArray(static_cast<const char*>(NULL));
    }
}

//----------------------------------------------------------------------------
int vtkImageSliceRepresentation::FillInputPortInformation(
  int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  return 1;
}

//----------------------------------------------------------------------------
int vtkImageSliceRepresentation::ProcessViewRequest(
  vtkInformationRequestKey* request_type,
  vtkInformation* inInfo, vtkInformation* outInfo)
{
  if (request_type == vtkPVView::REQUEST_INFORMATION())
    {
    // Here we need to tell the view about the geometry size and if we need
    // ordered compositing.
    vtkDataObject* slice = this->SliceData;
    if (slice)
      {
      outInfo->Set(vtkPVRenderView::GEOMETRY_SIZE(), slice->GetActualMemorySize());
      }
    if (this->Actor->GetProperty()->GetOpacity() < 1.0)
      {
      outInfo->Set(vtkPVRenderView::NEED_ORDERED_COMPOSITING(), 1);
      }
    }
  else if (request_type == vtkPVView::REQUEST_PREPARE_FOR_RENDER())
    {
    // In REQUEST_PREPARE_FOR_RENDER, we need to ensure all our data-deliver
    // filters have their states updated as requested by the view.
    this->DeliveryFilter->ProcessViewRequest(inInfo);

    // we have to use DeliveryTimeStamp since when image-data has invalid
    // extents the executive goes berserk and always keeps on re-executing the
    // pipeline which breaks when running in parallel.
    if (this->DeliveryTimeStamp < this->DeliveryFilter->GetMTime())
      {
      outInfo->Set(vtkPVRenderView::NEEDS_DELIVERY(), 1);
      }
    }
  else if (request_type == vtkPVView::REQUEST_DELIVERY())
    {
    this->DeliveryFilter->Modified();
    this->DeliveryFilter->Update();

    // since there's no direct connection between the mapper and the collector,
    // we don't put an update-suppressor in the pipeline.

    // essential to break the pipeline link between the mapper and the delivery
    // filter since when the delivery filter produces an empty image, the
    // executive keeps on re-executing it every time.
    vtkImageData* clone = vtkImageData::New();
    clone->ShallowCopy(this->DeliveryFilter->GetOutputDataObject(0));
    this->SliceMapper->SetInput(clone);
    clone->Delete();

    this->DeliveryTimeStamp.Modified();
    }
  else if (request_type == vtkPVView::REQUEST_RENDER())
    {
    // this->SliceMapper->Update();
    }

  return this->Superclass::ProcessViewRequest(request_type, inInfo, outInfo);
}

//----------------------------------------------------------------------------
int vtkImageSliceRepresentation::RequestData(vtkInformation* request,
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // Mark delivery filter modified.
  this->DeliveryFilter->Modified();

  // Pass caching information to the cache keeper.
  this->CacheKeeper->SetCachingEnabled(this->GetUseCache());
  this->CacheKeeper->SetCacheTime(this->GetCacheKey());

  if (inputVector[0]->GetNumberOfInformationObjects()==1)
    {
    this->UpdateSliceData(inputVector);
    this->DeliveryFilter->SetInputConnection(this->CacheKeeper->GetOutputPort());
    }
  else
    {
    this->DeliveryFilter->RemoveAllInputs();
    }

  return this->Superclass::RequestData(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
bool vtkImageSliceRepresentation::IsCached(double cache_key)
{
  return this->CacheKeeper->IsCached(cache_key);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::UpdateSliceData(
  vtkInformationVector** inputVector)
{
  if (this->GetUsingCacheForUpdate())
    {
    return;
    }

  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkImageData* input = vtkImageData::GetData(inputVector[0], 0);

  int inWholeExtent[6], outExt[6];
  memset(outExt, 0, sizeof(int)*6);

  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inWholeExtent);
  int dataDescription = vtkStructuredData::SetExtent(inWholeExtent, outExt);

  if (vtkStructuredData::GetDataDimension(dataDescription) != 3)
    {
    this->SliceData->ShallowCopy(input);
    return;
    }

  int dims[3];
  dims[0] = inWholeExtent[1]-inWholeExtent[0]+1;
  dims[1] = inWholeExtent[3]-inWholeExtent[2]+1;
  dims[2] = inWholeExtent[5]-inWholeExtent[4]+1;

  unsigned int slice = this->Slice;
  switch (this->SliceMode)
    {
  case YZ_PLANE:
    slice = (static_cast<int>(slice)>=dims[0])?dims[0]-1:slice;
    outExt[0] = outExt[1] = outExt[0]+slice;
    break;

  case XZ_PLANE:
    slice = (static_cast<int>(slice)>=dims[1])?dims[1]-1:slice;
    outExt[2] = outExt[3] = outExt[2]+slice;
    break;

  case XY_PLANE:
  default:
    slice = (static_cast<int>(slice)>=dims[2])?dims[2]-1:slice;
    outExt[4] = outExt[5] = outExt[4]+slice;
    break;
    }

  vtkImageData* clone= vtkImageData::New();
  clone->ShallowCopy(input);

  vtkExtractVOI* voi = vtkExtractVOI::New();
  voi->SetVOI(outExt);
  voi->SetInput(clone);
  voi->Update();

  this->SliceData->ShallowCopy(voi->GetOutput());
  // vtkExtractVOI is not passing correct origin. Until that's fixed, I
  // will just use the input origin/spacing to compute the bounds.
  this->SliceData->SetOrigin(input->GetOrigin());
  voi->Delete();
  clone->Delete();
}

//----------------------------------------------------------------------------
bool vtkImageSliceRepresentation::AddToView(vtkView* view)
{
  vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
  if (rview)
    {
    rview->GetRenderer()->AddActor(this->Actor);
    return true;
    }
  return false;
}

//----------------------------------------------------------------------------
bool vtkImageSliceRepresentation::RemoveFromView(vtkView* view)
{
  vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
  if (rview)
    {
    rview->GetRenderer()->RemoveActor(this->Actor);
    return true;
    }
  return false;
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::MarkModified()
{
  if (!this->GetUseCache())
    {
    // Cleanup caches when not using cache.
    this->CacheKeeper->RemoveAllCaches();
    }
  this->Superclass::MarkModified();
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//****************************************************************************
// Calls forwarded to internal objects.

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetOrientation(double x, double y, double z)
{
  this->Actor->SetOrientation(x, y, z);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetOrigin(double x, double y, double z)
{
  this->Actor->SetOrigin(x, y, z);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetPickable(int val)
{
  this->Actor->SetPickable(val);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetPosition(double x, double y, double z)
{
  this->Actor->SetPosition(x, y, z);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetScale(double x, double y, double z)
{
  this->Actor->SetScale(x, y, z);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetVisibility(bool val)
{
  this->Actor->SetVisibility(val? 1 : 0);
  this->Superclass::SetVisibility(val);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetOpacity(double val)
{
  this->Actor->GetProperty()->SetOpacity(val);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetLookupTable(vtkScalarsToColors* val)
{
  this->SliceMapper->SetLookupTable(val);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetMapScalars(int val)
{
  this->SliceMapper->SetColorMode(val);
}

//----------------------------------------------------------------------------
void vtkImageSliceRepresentation::SetUseXYPlane(int val)
{
  this->SliceMapper->SetUseXYPlane(val);
}
