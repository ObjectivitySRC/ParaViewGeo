/*=========================================================================
  Program:   ParaviewGeo
  Module:    $ vtkDistanceCellsToDrillhole.cxx $
  Author:    Arolde VIDJINNAGNI	    
  Date:      MIRARCO May 08, 2009 
  Version:   0.2
 =========================================================================*/
#include "vtkPVDistanceCellsToDrillhole.h"
#include "vtkCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include <vtkMath.h>
#include <time.h>
#include "vtkstd/vector"
#include "vtkstd/list"
#include "vtkPoints.h"
#include "vtkAppendPolyData.h"
#include "vtkMultiProcessController.h"
#include "vtkSmartPointer.h"
#include "vtkAttributeDataReductionFilter.h"
#include "vtkCollectPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkToolkits.h"

#include <assert.h>

#ifdef VTK_USE_MPI
#include "vtkMPICommunicator.h"
class vtkMPIController;
#endif

#include <vtkstd/string>
#include <vtksys/RegularExpression.hxx>


vtkCxxRevisionMacro ( vtkPVDistanceCellsToDrillhole, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkPVDistanceCellsToDrillhole );
vtkCxxSetObjectMacro(vtkPVDistanceCellsToDrillhole, Controller, vtkMultiProcessController);

//--------------------------------------------------------------------------
vtkPVDistanceCellsToDrillhole::vtkPVDistanceCellsToDrillhole()
{ 
   const int ports = 2;
   this->SetNumberOfInputPorts ( ports );
   this->NbClosestDrillholes=5;
   this->MinimumDistance=5;
   this->FileName = NULL;
   this->ComputeDistanceTo=0;

   this->Controller = vtkMultiProcessController::GetGlobalController();
   if (this->Controller)
    {
    this->Controller->Register(this);
    }

}
//--------------------------------------------------------------------------
vtkPVDistanceCellsToDrillhole::~vtkPVDistanceCellsToDrillhole()
{	
	if (this->Controller)
    {
    this->Controller->Delete();
    this->Controller = 0;
    }
}
void vtkPVDistanceCellsToDrillhole::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
{
   this->SetInputConnection ( 1, algOutput );
}

//----------------------------------------------------------------------------
int vtkPVDistanceCellsToDrillhole::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 1 );
  return 1;
  }


//----------------------------------------------------------------------------
int vtkPVDistanceCellsToDrillhole::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid");
  return 1;
}

//----------------------------------------------------------------------------
// Description:
int vtkPVDistanceCellsToDrillhole::RequestInformation(
  vtkInformation *request, 
  vtkInformationVector **inputVector, 
  vtkInformationVector *outputVector)
{
  if(!this->Superclass::RequestInformation(request,inputVector,outputVector))
    {
    return 0;
    }
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  inInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject ( 0 );
  sourceInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

  vtkInformation *outInfo=outputVector->GetInformationObject(0);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
  return 1;
}


//--------------------------------------------------------------------------
int vtkPVDistanceCellsToDrillhole::RequestData( vtkInformation *request, 
                                         vtkInformationVector **InputVector, 
                                         vtkInformationVector *outputVector )
{
  vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
   vtkInformation *sourceInfo = InputVector[1]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

    vtkUnstructuredGrid* input = vtkUnstructuredGrid::SafeDownCast (
                         inputInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

   vtkPolyData* source = vtkPolyData::SafeDownCast (
                         sourceInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

   vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast (
                        outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   if ( !source ){
      return 0;
   }

   if(this->ComputeDistanceTo==2)
   {
      // Make sure we have a file to read.
	  if(!this->FileName)  
	  {
       vtkErrorMacro("A FileName must be specified.");
       return 1;
	  }
	  if(strlen(this->FileName)==0)  
	  {
		vtkErrorMacro("A NULL FileName.");
		return 1;
	  }
   }

    if (!this->Controller || this->Controller->GetNumberOfProcesses() <= 1)
    {	
		this->Superclass::SetComputeDistanceTo(this->ComputeDistanceTo);
	    this->Superclass::SetNbClosestDrillholes(this->NbClosestDrillholes);
	    this->Superclass::SetMinimumDistance(this->MinimumDistance);	   
		output->ShallowCopy(this->Superclass::ComputeDistanceToHoles(input, source));
		int nbCellHole= source->GetNumberOfCells();
		if(this->ComputeDistanceTo==2)
	    {
			 vtkDoubleArray* calcul= vtkDoubleArray::New();
			 calcul->DeepCopy(this->Superclass::ProcessCalcul());			 
			 ofstream myFile;
			 myFile.open(this->FileName);
			 if(!myFile)
			 {
				vtkErrorMacro("Error writing to the file");
				return 0;
			 }

			 int size= calcul->GetNumberOfTuples();
			 for(int k=0; k<size; k++)
			 {
			   if( (k%(nbCellHole+1))==0)
			   myFile<<"CellId "<<calcul->GetTuple1(k)<<endl;
			   else myFile<<"H"<<(k%(nbCellHole+1))<<": "<<calcul->GetTuple1(k)<<endl;		
			 }
			calcul->Delete();		
			myFile.close();
		}
	  
		return 1;	
    }


#ifndef VTK_USE_MPI
   this->Superclass::SetComputeDistanceTo(this->ComputeDistanceTo);
   this->Superclass::SetNbClosestDrillholes(this->NbClosestDrillholes);
   this->Superclass::SetMinimumDistance(this->MinimumDistance);  
   output->ShallowCopy(this->Superclass::ComputeDistanceToHoles(input, source));
   int nbCellHole= source->GetNumberOfCells();
   if(this->ComputeDistanceTo==2)
    {
		 vtkDoubleArray* calcul= vtkDoubleArray::New();
		 calcul->DeepCopy(this->Superclass::ProcessCalcul());			 
		 ofstream myFile;
		 myFile.open(this->FileName);
		 if(!myFile)
		 {
			vtkErrorMacro("Error writing to the file");
			return 0;
		 }

		 int size= calcul->GetNumberOfTuples();
		 for(int k=0; k<size; k++)
		 {
		   if( (k%(nbCellHole+1))==0)
		   myFile<<"CellId "<<calcul->GetTuple1(k)<<endl;
		   else myFile<<"H"<<(k%(nbCellHole+1))<<": "<<calcul->GetTuple1(k)<<endl;		
		 }
		calcul->Delete();		
		myFile.close();
	}
   return 1;


#else
	
	int num_processes = this->Controller->GetNumberOfProcesses();
    vtkMPICommunicator* comm = vtkMPICommunicator::SafeDownCast(
    this->Controller->GetCommunicator());
	if (!comm)
    {
		vtkErrorMacro("vtkMPICommunicator is needed.");
		return false;
    }

   int processid = (this->Controller?this->Controller->GetLocalProcessId():0);

   vtkCollectPolyData *collectall= vtkCollectPolyData::New();
   collectall->SetController(this->Controller);
   collectall->SetInput(source);
   collectall->SetPassThrough(0);
   collectall->Update();
   int tag;
   vtkPolyData* tempSource= vtkPolyData::New();
   if(processid==0)
    {	   
	   vtkCleanPolyData *clean = vtkCleanPolyData::New();
	   clean->SetInput(collectall->GetOutput());
	   clean->Update();
       
	  
	   tempSource->ShallowCopy(clean->GetOutput());
	   clean->Delete();
		
       for(int idx=1; idx<num_processes;idx++)
	   {
		 tag= 126717+ idx;	
		 this->Controller->Send(tempSource, idx, tag);		
	   }	   
    }
   if(processid!=0)
   {
	   tag= 126717+ processid;
	   this->Controller->Receive(tempSource, 0, tag);
   }

   cout<< "Process Id "<< processid <<endl;
   cout<<" my input contain "<< input->GetNumberOfCells()<<endl;
   cout<<" my source contain "<< source->GetNumberOfCells()<<endl;
   cout<<" my all source contain "<< tempSource->GetNumberOfCells()<<endl;
   int nbCellHole= tempSource->GetNumberOfCells();

   this->Superclass::SetComputeDistanceTo(this->ComputeDistanceTo);
   this->Superclass::SetNbClosestDrillholes(this->NbClosestDrillholes);
   this->Superclass::SetMinimumDistance(this->MinimumDistance);
   output->ShallowCopy(this->Superclass::ComputeDistanceToHoles(input, tempSource));
   
	   if(this->ComputeDistanceTo==2)
	   {
		 vtkDoubleArray* calcul= vtkDoubleArray::New();
		 calcul->DeepCopy(this->Superclass::ProcessCalcul());
		 if(processid!=0)
		 {
		   tag= 126717+ processid;
		   this->Controller->Send(calcul, 0, tag);
		 }
		
		  if(processid==0)
		  {
			   ofstream myFile;

				myFile.open(this->FileName);
				if(!myFile)
				{
					vtkErrorMacro("Error writing to the file");
					return 0;
				}

				int size= calcul->GetNumberOfTuples();

				for(int k=0; k<size; k++)
				{
				  if( (k%(nbCellHole+1))==0)
				  myFile<<"CellId "<<calcul->GetTuple1(k)<<endl;
				  else myFile<<"H"<<(k%(nbCellHole+1))<<": "<<calcul->GetTuple1(k)<<endl;		
				}
				calcul->Delete();
				//copy the calcul of other process
				for(int p=1; p<num_processes; p++)
				{
				  vtkDoubleArray* calcul_copy= vtkDoubleArray::New();
				  tag= 126717+ p;
				  this->Controller->Receive(calcul_copy, p, tag);
				  //write the job in the file
				  for(int k=0; k<size; k++)
				  {
					if( (k%(nbCellHole+1))==0)
					myFile<<"CellId "<<calcul_copy->GetTuple1(k)<<endl;
					else myFile<<"H"<<(k%(nbCellHole+1))<<": "<<calcul->GetTuple1(k)<<endl;		
				  }
				  calcul_copy->Delete();
				  
				}
				myFile.close();
		  }
	   }
		


   return 1;
#endif

}
