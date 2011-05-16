/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkPVIntersectFilter.cxx $
  Author:   Arolde VIDJINNAGNI	    
  Date:    MIRARCO Febrary 06, 2009 
  Version:   0.2
 =========================================================================*/
#include "vtkPVIntersectFilter.h"
#include "vtkIntersectWireframe.h"
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
#include <math.h>
#include <time.h>
#include "vtkstd/vector"
#include "vtkstd/list"
#include "vtkPoints.h"
#include "vtkAppendPolyData.h"
#include "vtkMultiProcessController.h"
#include "vtkSmartPointer.h"
#include "vtkAttributeDataReductionFilter.h"
#include "vtkCollectPolyData.h"
#include "vtkThresholdProcessId.h"
#include "vtkCleanPolyData.h"
#include "vtkToolkits.h"
#include <assert.h>


#ifdef VTK_USE_MPI
#include "vtkMPICommunicator.h"
class vtkMPIController;
#endif

#include <vtkstd/string>
#include <vtksys/RegularExpression.hxx>


vtkCxxRevisionMacro ( vtkPVIntersectFilter, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkPVIntersectFilter );
vtkCxxSetObjectMacro(vtkPVIntersectFilter, Controller, vtkMultiProcessController);

//--------------------------------------------------------------------------
vtkPVIntersectFilter::vtkPVIntersectFilter()
{ 
   const int ports = 2;
   this->SetNumberOfInputPorts ( ports );
   this->TypeOfPoints=0;
   this->Reconstruction=0;
  
   this->Controller = vtkMultiProcessController::GetGlobalController();
   if (this->Controller)
    {
    this->Controller->Register(this);
    }

}
//--------------------------------------------------------------------------
vtkPolyData *vtkPVIntersectFilter::GetSource()
{
   if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
   {
      return NULL;
   }

   vtkDataObject  *src = this->GetExecutive()->GetInputData ( 1, 0 );
   vtkPolyData *source = vtkPolyData::SafeDownCast ( src );
   
   return source;
}
//--------------------------------------------------------------------------
vtkPVIntersectFilter::~vtkPVIntersectFilter()
{	
	if (this->Controller)
    {
    this->Controller->Delete();
    this->Controller = 0;
    }
}
void vtkPVIntersectFilter::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
{
   this->SetInputConnection ( 1, algOutput );
}

//----------------------------------------------------------------------------
// Description:
int vtkPVIntersectFilter::RequestInformation(
  vtkInformation *request, 
  vtkInformationVector **inputVector, 
  vtkInformationVector *outputVector)
{
  if(!this->Superclass::RequestInformation(request,inputVector,outputVector))
    {
    return 0;
    }
  vtkInformation *inInfo = inputVector[1]->GetInformationObject ( 0 );
  inInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

  vtkInformation *sourceInfo = inputVector[0]->GetInformationObject ( 0 );
  sourceInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

  vtkInformation *outInfo=outputVector->GetInformationObject(0);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
  return 1;
}


//--------------------------------------------------------------------------
int vtkPVIntersectFilter::RequestData( vtkInformation *request, 
                                         vtkInformationVector **InputVector, 
                                         vtkInformationVector *outputVector )
{
   vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
   vtkInformation *sourceInfo = InputVector[1]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

   vtkPolyData *input = vtkPolyData::SafeDownCast (
                         inputInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

   vtkPolyData *source = vtkPolyData::SafeDownCast (
                         sourceInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

   vtkPolyData *output = vtkPolyData::SafeDownCast (
                        outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   if ( !source ){
      return 0;
   }
	

    vtkPolyData *connect1= vtkPolyData::New();
	vtkPolyData *connect2= vtkPolyData::New();
	vtkAppendPolyData *tempOutput=vtkAppendPolyData::New(); 
  
    if (!this->Controller || this->Controller->GetNumberOfProcesses() <= 1)
    {
		
		vtkPolyData *tempInput= vtkPolyData::New();
		tempInput->ShallowCopy(input);
		vtkPolyData *tempSource= vtkPolyData::New();
		tempSource->ShallowCopy(source);

		this->Superclass::ConstructOutput(tempInput, connect1, tempSource, this->TypeOfPoints, this->Reconstruction);
		//this->Superclass::ConstructOutput(tempSource, connect2, tempInput, this->TypeOfPoints, this->Reconstruction);
		tempOutput->AddInput(connect1);		
		//tempOutput->AddInput(connect2);	
		tempOutput->Update();
		//output->ShallowCopy(tempOutput->GetOutput());



		vtkCleanPolyData *clean = vtkCleanPolyData::New();
        clean->SetInput(tempOutput->GetOutput());
        clean->Update();
        output->ShallowCopy(clean->GetOutput());

		// output->ShallowCopy(tempOutput->GetOutput());
		//free up memory
		tempOutput->Delete();
		connect1->Delete();
		connect2->Delete();	
		
		//tempInput->Delete();
		//tempSource->Delete();
		return 1;		
		
    }


#ifndef VTK_USE_MPI

	vtkPolyData *tempInput= vtkPolyData::SafeDownCast(input);
	vtkPolyData *tempSource= vtkPolyData::SafeDownCast(source);	
	
	this->Superclass::ConstructOutput(tempInput, connect1, tempSource, this->TypeOfPoints, this->Reconstruction);
	this->Superclass::ConstructOutput(tempSource, connect2, tempInput, this->TypeOfPoints, this->Reconstruction);
	tempOutput->AddInput(connect1);

	tempOutput->AddInput(connect2);
	tempOutput->Update();

	vtkCleanPolyData *clean = vtkCleanPolyData::New();
    clean->SetInput(tempOutput->GetOutput());
    clean->Update();
    output->ShallowCopy(clean->GetOutput());	

	// output->ShallowCopy(tempOutput->GetOutput());

	//free up memory
	tempOutput->Delete();
	connect1->Delete();
	connect2->Delete();	
	
	//tempInput->Delete();
	//tempSource->Delete();

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


   vtkIdType numCellsInput;   
   numCellsInput = input->GetNumberOfPoints();
   
   vtkIdType numCellsSource;   
   numCellsSource = source->GetNumberOfPoints();
   vtkPolyData *input1= vtkPolyData::SafeDownCast(input);
   vtkPolyData *source1= vtkPolyData::SafeDownCast(source); 
    

   if( ( (this->Lines>0)||(this->Surfaces>0)||(this->StripSurfaces>0))/*&&(this->Points==0)*/ )
   {
	  
	  vtkThresholdProcessId *allSource = vtkThresholdProcessId::New();
	   allSource->SetInput(source);
	   allSource->SetWholeOutput(1);
	   allSource->Update();
		
	   vtkPolyData *outSource =  vtkPolyData::New();
	   outSource->ShallowCopy( allSource->GetOutput() );
	   vtkIdType numOut;   
	   numOut = outSource->GetNumberOfPoints();

	   //vtkPolyData *connect1= vtkPolyData::New();	  
	   this->Superclass::ConstructOutput(input1, connect1, outSource, this->TypeOfPoints, this->Reconstruction);
	   tempOutput->AddInput(connect1);
   
	   vtkIdType numCon;   
	   numCon = connect1->GetNumberOfPoints();
	  
	   vtkThresholdProcessId *allInput = vtkThresholdProcessId::New();
	   allInput->SetInput(input);
	   allInput->SetWholeOutput(1);
	   allInput->Update();
	   vtkPolyData *outInput =  vtkPolyData::New();
	   outInput->ShallowCopy( allInput->GetOutput() );
	
	   vtkIdType num1;   
	   num1 = outInput->GetNumberOfPoints();
	 
	   //vtkPolyData *connect2= vtkPolyData::New();
	   this->Superclass::ConstructOutput(source1, connect2, outInput, this->TypeOfPoints,this->Reconstruction);

	   vtkIdType numCon1 = connect2->GetNumberOfPoints();	 
	   tempOutput->AddInput(connect2);
	   tempOutput->Update(); 
	   

	   vtkCollectPolyData *collect= vtkCollectPolyData::New();
	   collect->SetController(this->Controller);
	   collect->SetInput(tempOutput->GetOutput() );
	   collect->Update();
	   

	    if(processid==0)
	   {
		  	vtkCleanPolyData *clean = vtkCleanPolyData::New();
	        clean->SetInput(collect->GetOutput());
	        clean->Update();
	        output->ShallowCopy(clean->GetOutput());
	   }
		//free up memory
		connect1->Delete();
		connect2->Delete();
		tempOutput->Delete();
		//collect->Delete();
		allInput->Delete();
		allSource->Delete();
		outSource->Delete();
		outInput->Delete();
    }   

   return 1;
#endif

}
//--------------------------------------------------------------------------
void vtkPVIntersectFilter::PrintSelf ( ostream& os, vtkIndent indent)
{   
  vtkDataSet *source = this->GetSource();
  this->Superclass::PrintSelf ( os, indent );
  os << indent << "source: " << source << "\n";

}
