/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkCompositing.cxx $
  Author:   Arolde VIDJINNAGNI
  Date:      Febrary 04, 2009
  Version:   0.1
=========================================================================*/

#include "vtkThresholdProcessId.h"

#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkSmartPointer.h"
#include "vtkReductionFilter.h"
#include "vtkAttributeDataReductionFilter.h"
#include "vtkCollectPolyData.h"


vtkCxxRevisionMacro(vtkThresholdProcessId, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkThresholdProcessId);

vtkCxxSetObjectMacro(vtkThresholdProcessId,Controller,
                     vtkMultiProcessController);

//----------------------------------------------------------------------------
vtkThresholdProcessId::vtkThresholdProcessId()
{
  this->ProccesId = 0;
  this->OutData  = 0;
  this->WholeOutput =0;

  this->Controller = vtkMultiProcessController::GetGlobalController();
  if (this->Controller)
    {
    this->Controller->Register(this);
    }
}

//----------------------------------------------------------------------------
vtkThresholdProcessId::~vtkThresholdProcessId()
{
  if (this->Controller)
    {
    this->Controller->Delete();
    this->Controller = 0;
    }
}

//----------------------------------------------------------------------------
// Append data sets into single unstructured grid
int vtkThresholdProcessId::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  
  vtkDataArray *pieceColors;
  vtkIdType numCells;
  
  numCells = input->GetNumberOfCells();
 // cout << "Points set nbpt=" << numCells << endl;

  int processid = (this->Controller?this->Controller->GetLocalProcessId():0);
  
  pieceColors = this->MakeProcessIdScalars(processid, numCells);
  
  if(WholeOutput==1)
  {
	vtkSmartPointer<vtkReductionFilter> reduceFilter = 
    vtkSmartPointer<vtkReductionFilter>::New();
    reduceFilter->SetController(this->Controller);

	if(processid== 0)
	{
		// PostGatherHelper needs to be set only on the root node.
		vtkSmartPointer<vtkAttributeDataReductionFilter> rf = 
		  vtkSmartPointer<vtkAttributeDataReductionFilter>::New();
		rf->SetAttributeType(vtkAttributeDataReductionFilter::CELL_DATA|
		  vtkAttributeDataReductionFilter::FIELD_DATA);
		rf->SetReductionType(vtkAttributeDataReductionFilter::ADD);
		reduceFilter->SetPostGatherHelper(rf);
	}

	vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkDataSet *output = vtkDataSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	
	  vtkSmartPointer<vtkDataObject> copy;
	  copy.TakeReference(output->NewInstance());
	  output->ShallowCopy(input);	  
	  copy->ShallowCopy(output);
	  reduceFilter->SetInput(copy); 
	  reduceFilter->Update();
	  //output->ShallowCopy(reduceFilter->GetOutput());

	if(processid== 0)
	{
		output->ShallowCopy(reduceFilter->GetOutput());
	}
  
  }
  else 
  {
	  vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkDataSet *output = vtkDataSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	  if( (processid== this->ProccesId) && (this->OutData==0))
	  {
		  output->ShallowCopy(input);
		  pieceColors->SetName("ProcessId");  
		  
		  output->GetCellData()->AddArray(pieceColors);
		  output->GetCellData()->SetActiveScalars(pieceColors->GetName());
	  }	  
	  else if( (processid!= this->ProccesId) && (this->OutData!=0) )
	  {
		  output->ShallowCopy(input);
		  pieceColors->SetName("ProcessId");  
		  
		  output->GetCellData()->AddArray(pieceColors);
		  output->GetCellData()->SetActiveScalars(pieceColors->GetName());
	  }	
  }
  pieceColors->Delete();

  return 1;
}

//----------------------------------------------------------------------------
vtkIntArray *vtkThresholdProcessId::MakeProcessIdScalars(int processid, vtkIdType numCells)
{
  vtkIdType i;
  vtkIntArray *pieceColors = NULL;

  pieceColors = vtkIntArray::New();
  pieceColors->SetNumberOfTuples(numCells);
  
  for (i = 0; i < numCells; ++i)
    {
    pieceColors->SetValue(i, processid);
    }

  return pieceColors;
}

//----------------------------------------------------------------------------
void vtkThresholdProcessId::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);  
  
  os << indent << "ProcessId " << this->ProccesId << endl;
  os << indent << "OutData  "<< this->OutData << endl;
    
}

