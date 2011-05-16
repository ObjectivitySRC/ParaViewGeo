/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkExtractPolyData.cxx,v $

  By:				 Matthew Livingstone

=========================================================================*/
// Modified By: Matthew Livingstone
#include "vtkExtractPolyData.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkDataSet.h"
#include "vtkCompositeDataIterator.h"
#include "vtkInformationIntegerKey.h"
#include "vtkMultiPieceDataSet.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkExecutive.h"
#include "vtkSmartPointer.h"
#include "vtkCompositeDataSet.h"

#include <vtkstd/set>

class vtkExtractPolyData::vtkSet : public vtkstd::set<unsigned int>
{
};

vtkStandardNewMacro(vtkExtractPolyData);
vtkCxxRevisionMacro(vtkExtractPolyData, "$Revision: 1.4 $");
vtkInformationKeyMacro(vtkExtractPolyData, DONT_PRUNE, Integer);
//----------------------------------------------------------------------------
vtkExtractPolyData::vtkExtractPolyData()
{
  this->Indices = new vtkExtractPolyData::vtkSet();
  this->PruneOutput = 1;
}

//----------------------------------------------------------------------------
vtkExtractPolyData::~vtkExtractPolyData()
{
  delete this->Indices;
}

//----------------------------------------------------------------------------
void vtkExtractPolyData::AddIndex(unsigned int index)
{
  this->Indices->insert(index);
  this->Modified();
}


//----------------------------------------------------------------------------
void vtkExtractPolyData::RemoveIndex(unsigned int index)
{
  this->Indices->erase(index);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkExtractPolyData::RemoveAllIndices()
{
  this->Indices->clear();
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkExtractPolyData::CopySubTree(vtkCompositeDataIterator* loc, 
  vtkMultiBlockDataSet* output, vtkMultiBlockDataSet* input)
{
  vtkDataObject* inputNode = input->GetDataSet(loc);
  if (!inputNode->IsA("vtkCompositeDataSet"))
    {
    vtkDataObject* clone = inputNode->NewInstance();
    clone->ShallowCopy(inputNode);
    output->SetDataSet(loc, clone);
    clone->Delete();
    }
  else
    {
    vtkCompositeDataSet* cinput = vtkCompositeDataSet::SafeDownCast(inputNode);
    vtkCompositeDataSet* coutput = vtkCompositeDataSet::SafeDownCast(
      output->GetDataSet(loc));
    vtkCompositeDataIterator* iter = cinput->NewIterator();
    for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
      {
      vtkDataObject* curNode = iter->GetCurrentDataObject();
      vtkDataObject* clone = curNode->NewInstance();
      clone->ShallowCopy(curNode);
      coutput->SetDataSet(iter, clone);
      clone->Delete();
      }
    iter->Delete();
    }
}

//----------------------------------------------------------------------------
int vtkExtractPolyData::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
	
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
	vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

  vtkMultiBlockDataSet *input =  vtkMultiBlockDataSet::SafeDownCast (
                          inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	vtkPolyData *polyOutput = vtkPolyData::SafeDownCast (
                         outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	
	vtkMultiBlockDataSet *selectedItems = vtkMultiBlockDataSet::New();
	selectedItems->DeepCopy(input);

  // Copy selected blocks over to the output.
  vtkCompositeDataIterator* iter = input->NewIterator();
  iter->VisitOnlyLeavesOff();

  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
    if (this->Indices->find(iter->GetCurrentFlatIndex()) != this->Indices->end())
      {
      this->CopySubTree(iter, selectedItems, input);
      // TODO: avoid copying if subtree has already been copied over.
      }
    }
  iter->Delete();

	if (this->PruneOutput)
		{
		// Now prune the output tree.

		// Since in case multiple processes are involved, this process may have some
		// data-set pointers NULL. Hence, pruning cannot simply trim NULL ptrs, since
		// in that case we may end up with different structures on different
		// processess, which is a big NO-NO. Hence, we first flag nodes based on
		// whether they are being pruned or not.

		iter = selectedItems->NewIterator();
		iter->VisitOnlyLeavesOff();
		iter->SkipEmptyNodesOff();
		//iter->SkipEmptyNodesOn();
		for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
			{
			if (this->Indices->find(iter->GetCurrentFlatIndex()) != this->Indices->end())
				{
				iter->GetCurrentMetaData()->Set(DONT_PRUNE(), 1);
				}
			else if (iter->HasCurrentMetaData() && iter->GetCurrentMetaData()->Has(DONT_PRUNE()))
				{
				iter->GetCurrentMetaData()->Remove(DONT_PRUNE());
				}
			}
		iter->Delete();

		// Do the actual pruning. Only those branches are pruned which don't have
		// DONT_PRUNE flag set.
		this->Prune(selectedItems);
		}

	if(selectedItems->GetNumberOfBlocks() <= 0)
	{
		// Ensure empty output
		vtkPolyData *temp = vtkPolyData::New();
		polyOutput->ShallowCopy(temp);
		temp->Delete();	
		selectedItems->Delete();
		return 1;
	}

	// Final PolyData output object
	vtkAppendPolyData *polyDataGroup = vtkAppendPolyData::New();
	
	// Append all blocks to the PolyData output
	vtkCompositeDataIterator *iter2 = selectedItems->NewIterator();
	iter2->VisitOnlyLeavesOn();
  iter2->TraverseSubTreeOn();
	while( !iter2->IsDoneWithTraversal() )
		{
		polyDataGroup->AddInput(vtkPolyData::SafeDownCast( iter2->GetCurrentDataObject() ) );
		iter2->GoToNextItem();
		}

	iter2->Delete();
	polyDataGroup->Update();

	// Copy points & cells & properties
	polyOutput->ShallowCopy(polyDataGroup->GetOutput());

	polyDataGroup->Delete();
	selectedItems->Delete();

  return 1;
}

//----------------------------------------------------------------------------
bool vtkExtractPolyData::Prune(vtkDataObject* branch)
{
  if (branch->IsA("vtkMultiBlockDataSet"))
    {
    return this->Prune(vtkMultiBlockDataSet::SafeDownCast(branch));
    }
  else if (branch->IsA("vtkMultiPieceDataSet"))
    {
    return this->Prune(vtkMultiPieceDataSet::SafeDownCast(branch));
    }

  return true;
}

//----------------------------------------------------------------------------
bool vtkExtractPolyData::Prune(vtkMultiPieceDataSet* mpiece)
{
  // * Remove any children on mpiece that don't have DONT_PRUNE set.
  vtkMultiPieceDataSet* clone = vtkMultiPieceDataSet::New();
  unsigned int index=0;
  unsigned int numChildren = mpiece->GetNumberOfPieces();
  for (unsigned int cc=0; cc<numChildren; cc++)
    {
    if (mpiece->HasMetaData(cc) && mpiece->GetMetaData(cc)->Has(DONT_PRUNE()))
      {
      clone->SetPiece(index, mpiece->GetPiece(cc));
      clone->GetMetaData(index)->Copy(mpiece->GetMetaData(cc));
      index++;
      }
    }
  mpiece->ShallowCopy(clone);
  clone->Delete();

  // tell caller to prune mpiece away if num of pieces is 0.
  return (mpiece->GetNumberOfPieces() == 0);
}

//----------------------------------------------------------------------------
bool vtkExtractPolyData::Prune(vtkMultiBlockDataSet* mblock)
{
  vtkMultiBlockDataSet* clone = vtkMultiBlockDataSet::New();
  unsigned int index=0;
  unsigned int numChildren = mblock->GetNumberOfBlocks();
  for (unsigned int cc=0; cc < numChildren; cc++)
    {
    vtkDataObject* block = mblock->GetBlock(cc);
    if (mblock->HasMetaData(cc) && mblock->GetMetaData(cc)->Has(DONT_PRUNE()))
      {
      clone->SetBlock(index, block);
      clone->GetMetaData(index)->Copy(mblock->GetMetaData(cc));
      index++;
      }
    else if (block)
      {
      bool prune = this->Prune(block);
      if (!prune)
        {
        vtkMultiBlockDataSet* prunedBlock = vtkMultiBlockDataSet::SafeDownCast(block);
        if (prunedBlock && prunedBlock->GetNumberOfBlocks()==1)
          {
          // shrink redundant branches.
          clone->SetBlock(index, prunedBlock->GetBlock(0));
          if (prunedBlock->HasMetaData(static_cast<unsigned int>(0)))
            {
            clone->GetMetaData(index)->Copy(prunedBlock->GetMetaData(
                static_cast<unsigned int>(0)));
            }
          }
        else
          {
          clone->SetBlock(index, block);
          if (mblock->HasMetaData(cc))
            {
            clone->GetMetaData(index)->Copy(mblock->GetMetaData(cc));
            }
          }
        index++;
        }
      }
    }
  mblock->ShallowCopy(clone);
  clone->Delete();

  return (mblock->GetNumberOfBlocks() == 0);
}

//----------------------------------------------------------------------------
void vtkExtractPolyData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "PruneOutput: " << this->PruneOutput << endl;
}

int vtkExtractPolyData::FillInputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkMultiBlockDataSet");
	return 1;
}

/*int vtkExtractPolyData::FillOutputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkAlgorithm::O, "vtkMultiBlockDataSet");
	return 1;
}*/