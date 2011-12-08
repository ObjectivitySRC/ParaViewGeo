/*=========================================================================

  Program:   ParaView
  Module:    vtkPVCacheKeeper.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVCacheKeeper.h"

#include "vtkCacheSizeKeeper.h"
#include "vtkDataObject.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkPVCacheKeeperPipeline.h"
#include "vtkSmartPointer.h"

#include <vtkstd/map>
//----------------------------------------------------------------------------
class vtkPVCacheKeeper::vtkCacheMap :
  public vtkstd::map<double, vtkSmartPointer<vtkDataObject> >
{
public:
  unsigned long GetActualMemorySize() 
    {
    unsigned long actual_size = 0;
    vtkCacheMap::iterator iter;
    for (iter = this->begin(); iter != this->end(); ++iter)
      {
      actual_size += iter->second.GetPointer()->GetActualMemorySize();
      }
    return actual_size;
    }
};

vtkStandardNewMacro(vtkPVCacheKeeper);
vtkCxxSetObjectMacro(vtkPVCacheKeeper, CacheSizeKeeper, vtkCacheSizeKeeper);
//----------------------------------------------------------------------------
vtkPVCacheKeeper::vtkPVCacheKeeper()
{
  this->Cache = new vtkPVCacheKeeper::vtkCacheMap();
  this->CacheTime = 0.0;
  this->CachingEnabled = true; 
  this->CacheSizeKeeper = 0;
  this->SetCacheSizeKeeper(vtkCacheSizeKeeper::GetInstance());

  this->GetInformation()->Set(vtkAlgorithm::PRESERVES_DATASET(), 1);
}

//----------------------------------------------------------------------------
vtkPVCacheKeeper::~vtkPVCacheKeeper()
{
  this->RemoveAllCaches();

  // Unset cache keeper only after having cleared the cache.
  this->SetCacheSizeKeeper(0);

  delete this->Cache;
  this->Cache = 0;
}

//----------------------------------------------------------------------------
void vtkPVCacheKeeper::RemoveAllCaches()
{
  // cout << this << " RemoveAllCaches" << endl;
  unsigned long freed_size = this->Cache->GetActualMemorySize();
  this->Cache->clear();
  if (freed_size > 0 && this->CacheSizeKeeper)
    {
    // Tell the cache size keeper about the newly freed memory size.
    this->CacheSizeKeeper->FreeCacheSize(freed_size);
    }

  // this method should never mark the filter modified !!!
}

//----------------------------------------------------------------------------
bool vtkPVCacheKeeper::IsCached(double cacheTime)
{
  vtkPVCacheKeeper::vtkCacheMap::iterator iter = this->Cache->find(cacheTime);
  return (iter != this->Cache->end());
}

//----------------------------------------------------------------------------
bool vtkPVCacheKeeper::SaveData(vtkDataObject* output)
{
  if (!this->CacheSizeKeeper  || !this->CacheSizeKeeper->GetCacheFull())
    {
    vtkSmartPointer<vtkDataObject> cache;
    cache.TakeReference(output->NewInstance());
    cache->ShallowCopy(output);
    (*this->Cache)[this->CacheTime] = cache;

    if (this->CacheSizeKeeper)
      {
      // Register used cache size.
      this->CacheSizeKeeper->AddCacheSize(cache->GetActualMemorySize());
      }
    return true;
    }
  return false;
}

//----------------------------------------------------------------------------
vtkExecutive* vtkPVCacheKeeper::CreateDefaultExecutive()
{
  vtkPVCacheKeeperPipeline* executive = vtkPVCacheKeeperPipeline::New();
  return executive;
}

//----------------------------------------------------------------------------
int vtkPVCacheKeeper::RequestDataObject(
  vtkInformation* vtkNotUsed(reqInfo), 
  vtkInformationVector** inputVector , 
  vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  if (!inInfo)
    {
    return 0;
    }
  
  vtkDataObject *input = vtkDataObject::GetData(inInfo);
  if (input)
    {
    // for each output
    for(int i=0; i < this->GetNumberOfOutputPorts(); ++i)
      {
      vtkDataObject *output = vtkDataObject::GetData(outputVector, 0);
      if (!output || !output->IsA(input->GetClassName())) 
        {
        vtkDataObject* newOutput = input->NewInstance();
        newOutput->SetPipelineInformation(outputVector->GetInformationObject(0));
        newOutput->Delete();
        this->GetOutputPortInformation(i)->Set(
          vtkDataObject::DATA_EXTENT_TYPE(), newOutput->GetExtentType());
        }
      }
    return 1;
    }

  return 0;
}

//----------------------------------------------------------------------------
int vtkPVCacheKeeper::RequestData(vtkInformation* vtkNotUsed(reqInfo),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataObject *input = inInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkDataObject *output = outInfo->Get(vtkDataObject::DATA_OBJECT());

  if (this->CachingEnabled)
    {
    if (this->IsCached(this->CacheTime))
      {
      output->ShallowCopy((*this->Cache)[this->CacheTime]);
      //cout << this << " using Cache: " << this->CacheTime << endl;
      }
    else
      {
      output->ShallowCopy(input);
      this->SaveData(output);
      //cout << this << " Saving cache: " << this->CacheTime << endl;
      }
    }
  else
    {
    output->ShallowCopy(input);
    //cout << this << " Not using cache" << endl;
    }
  return 1;
}

//----------------------------------------------------------------------------
void vtkPVCacheKeeper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


