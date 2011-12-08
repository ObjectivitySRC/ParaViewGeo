/*=========================================================================

  Program:   ParaView
  Module:    vtkCompositeDataToUnstructuredGridFilter.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkCompositeDataToUnstructuredGridFilter.h"

#include "vtkAppendFilter.h"
#include "vtkCleanArrays.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGrid.h"

vtkStandardNewMacro(vtkCompositeDataToUnstructuredGridFilter);
//----------------------------------------------------------------------------
vtkCompositeDataToUnstructuredGridFilter::vtkCompositeDataToUnstructuredGridFilter()
{
  this->SubTreeCompositeIndex = 0;
  this->MergePoints = true;
}

//----------------------------------------------------------------------------
vtkCompositeDataToUnstructuredGridFilter::~vtkCompositeDataToUnstructuredGridFilter()
{
}

//----------------------------------------------------------------------------
int vtkCompositeDataToUnstructuredGridFilter::RequestData(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkCompositeDataSet* cd = vtkCompositeDataSet::GetData(inputVector[0], 0);
  vtkUnstructuredGrid* ug = vtkUnstructuredGrid::GetData(inputVector[0], 0);
  vtkDataSet* ds = vtkDataSet::GetData(inputVector[0], 0);
  vtkUnstructuredGrid* output = vtkUnstructuredGrid::GetData(outputVector, 0);

  if (ug)
    {
    output->ShallowCopy(ug);
    return 1;
    }


  vtkAppendFilter* appender = vtkAppendFilter::New();
  appender->SetMergePoints(this->MergePoints? 1 : 0);  
  if (ds)
    {
    this->AddDataSet(ds, appender);
    }
  else if (cd)
    {
    if (this->SubTreeCompositeIndex == 0)
      {
      this->ExecuteSubTree(cd, appender);
      }

    vtkCompositeDataIterator* iter = cd->NewIterator();
    iter->VisitOnlyLeavesOff();
    for (iter->InitTraversal(); !iter->IsDoneWithTraversal() && 
      iter->GetCurrentFlatIndex() <= this->SubTreeCompositeIndex;
      iter->GoToNextItem())
      {
      if (iter->GetCurrentFlatIndex() == this->SubTreeCompositeIndex)
        {
        vtkDataObject* curDO = iter->GetCurrentDataObject();
        vtkCompositeDataSet* curCD = vtkCompositeDataSet::SafeDownCast(curDO);
        vtkUnstructuredGrid* curUG = vtkUnstructuredGrid::SafeDownCast(curDO);
        vtkDataSet* curDS = vtkUnstructuredGrid::SafeDownCast(curDO);
        if (curUG)
          {
          output->ShallowCopy(curUG);
          // NOTE: Not using the appender at all.
          }
        else if (curDS && curCD->GetNumberOfPoints() > 0)
          {
          this->AddDataSet(curDS, appender);
          }
        else if (curCD)
          {
          this->ExecuteSubTree(curCD, appender);
          }
        break;
        }
      }
    iter->Delete();
    }

  if (appender->GetNumberOfInputConnections(0) > 0)
    {
    appender->Update();
    output->ShallowCopy(appender->GetOutput());
    }

  appender->Delete();
  this->RemovePartialArrays(output);
  return 1;
}

//----------------------------------------------------------------------------
void vtkCompositeDataToUnstructuredGridFilter::ExecuteSubTree(
  vtkCompositeDataSet* curCD, vtkAppendFilter* appender)
{
  vtkCompositeDataIterator* iter2 = curCD->NewIterator();
  for (iter2->InitTraversal(); !iter2->IsDoneWithTraversal();
    iter2->GoToNextItem())
    {
    vtkDataSet* curDS = 
      vtkDataSet::SafeDownCast(iter2->GetCurrentDataObject());
    if (curDS)
      {
      appender->AddInput(curDS);
      }
    }
  iter2->Delete();
}

//----------------------------------------------------------------------------
void vtkCompositeDataToUnstructuredGridFilter::AddDataSet(
  vtkDataSet* ds, vtkAppendFilter* appender)
{
  vtkDataSet* clone = ds->NewInstance();
  clone->ShallowCopy(ds);
  appender->AddInput(clone);
  clone->Delete();
}

//----------------------------------------------------------------------------
int vtkCompositeDataToUnstructuredGridFilter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkCompositeDataSet");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

//----------------------------------------------------------------------------
void vtkCompositeDataToUnstructuredGridFilter::RemovePartialArrays(
  vtkUnstructuredGrid* data)
{
  vtkUnstructuredGrid* clone = vtkUnstructuredGrid::New();
  clone->ShallowCopy(data);
  vtkCleanArrays* cleaner = vtkCleanArrays::New();
  cleaner->SetInput(clone);
  cleaner->Update();
  data->ShallowCopy(cleaner->GetOutput());
  cleaner->Delete();
  clone->Delete();
}

//----------------------------------------------------------------------------
void vtkCompositeDataToUnstructuredGridFilter::PrintSelf(ostream& os,
  vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "SubTreeCompositeIndex: " 
    << this->SubTreeCompositeIndex << endl;
}


