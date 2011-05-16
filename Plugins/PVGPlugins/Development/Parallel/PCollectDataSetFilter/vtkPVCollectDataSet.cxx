/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkCompositing.cxx $
  Author:   Arolde VIDJINNAGNI
  Date:      Febrary 04, 2009
  Version:   0.1
=========================================================================*/

#include "vtkPVCollectDataSet.h"

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
#include "vtkCleanCells.h"


vtkCxxRevisionMacro(vtkPVCollectDataSet, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkPVCollectDataSet);

vtkCxxSetObjectMacro(vtkPVCollectDataSet,Controller,
                     vtkMultiProcessController);

//----------------------------------------------------------------------------
vtkPVCollectDataSet::vtkPVCollectDataSet()
{
  this->PassThrough=0;
  this->Controller = vtkMultiProcessController::GetGlobalController();
  if (this->Controller)
    {
    this->Controller->Register(this);
    }
}

//----------------------------------------------------------------------------
vtkPVCollectDataSet::~vtkPVCollectDataSet()
{
  if (this->Controller)
    {
    this->Controller->Delete();
    this->Controller = 0;
    }
}

//----------------------------------------------------------------------------
// Append data sets into single unstructured grid
int vtkPVCollectDataSet::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
   
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

   vtkInformation *outInfo = outputVector->GetInformationObject(0);
	  vtkPolyData *output = vtkPolyData::SafeDownCast(
	  outInfo->Get(vtkDataObject::DATA_OBJECT()));


   int num_processes= 0;
   num_processes= this->Controller->GetNumberOfProcesses();
   
   int processid = (this->Controller?this->Controller->GetLocalProcessId():0);


 
  if (input->IsA("vtkPolyData"))
  {

	    vtkCollectPolyData *collectall= vtkCollectPolyData::New();
		collectall->SetController(this->Controller);
		collectall->SetInput(input);
		
		collectall->SetPassThrough(this->PassThrough);
		collectall->Update();		

		if(processid==0)
		{
		   vtkCleanCells *clean = vtkCleanCells::New();
		   clean->SetInput(collectall->GetOutput());
		   clean->Update();
		   output->ShallowCopy(clean->GetOutput());		    	  
	    }
  }

  /*
  else if(input->IsA("vtkUnstructureGrid"))
  {
  //code
  }
  */
  
  return 1;
}

//----------------------------------------------------------------------------
void vtkPVCollectDataSet::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);  
  
}

