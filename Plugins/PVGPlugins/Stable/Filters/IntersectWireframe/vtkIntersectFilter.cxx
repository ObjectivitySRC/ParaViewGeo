/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkIntersectFilter.cxx $
  Author:   Arolde VIDJINNAGNI	    
  Date:    MIRARCO Febrary 06, 2009 
  Version:   0.2
 =========================================================================*/
#include "vtkIntersectFilter.h"
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


//what we need to add when we want to use mpi in our code
#ifdef VTK_USE_MPI
#include "vtkMPICommunicator.h"
#endif

#include <vtkstd/string>
#include <vtksys/RegularExpression.hxx>


vtkCxxRevisionMacro ( vtkIntersectFilter, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkIntersectFilter );
vtkCxxSetObjectMacro(vtkIntersectFilter, Controller, vtkMultiProcessController);

/**************************MIRARCO*****************************************
* Function: 		vtkIntersectFilter   	                              *
* Description:                                                            *
* Input parameters :													  *		                                                                 
* Output Parametres:													  * 
*Author Arolde VIDJINNAGNI                                                *
*Date:					                                                  *
***************************************************************************/
vtkIntersectFilter::vtkIntersectFilter()
{ 
   const int ports = 2;
   this->SetNumberOfInputPorts ( ports );
   this->TypeOfPoints=0;

  // this->SocketController = NULL;

   this->Controller = vtkMultiProcessController::GetGlobalController();
   if (this->Controller)
    {
    this->Controller->Register(this);
    }

}
/**************************MIRARCO*****************************************
* Function: 		GetSource()					                          *
* Description:                                                            *
* Input parameters :													  *		                                                                 
* Output Parametres:													  * 
*Author Arolde VIDJINNAGNI                                                *
*Date:					                                                  *
***************************************************************************/
vtkDataSet *vtkIntersectFilter::GetSource()
{
   if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
   {
      return NULL;
   }

   vtkDataObject  *src = this->GetExecutive()->GetInputData ( 1, 0 );
   vtkDataSet *source = vtkDataSet::SafeDownCast ( src );
   
   return source;
}
/**************************MIRARCO*****************************************
* Function: 		~vtkIntersectFilter()		                          *
* Description:                                                            *
* Input parameters :													  *		                                                                 
* Output Parametres:													  * 
*Author Arolde VIDJINNAGNI                                                *
*Date:					                                                  *
***************************************************************************/
vtkIntersectFilter::~vtkIntersectFilter()
{	
	if (this->Controller)
    {
    this->Controller->Delete();
    this->Controller = 0;
    }
}
void vtkIntersectFilter::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
{
   this->SetInputConnection ( 1, algOutput );
}

/**************************MIRARCO*****************************************
* Function: 		RequestData         	                              *
* Description:                                                            *
* Input parameters :													  *		                                                                 
* Output Parametres:													  * 
*Author  Arolde VIDJINNAGNI                                               *
*Date:					                                                  *
***************************************************************************/
int vtkIntersectFilter::RequestData( vtkInformation *request, 
                                         vtkInformationVector **InputVector, 
                                         vtkInformationVector *outputVector )
{
   vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
   vtkInformation *sourceInfo = InputVector[1]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

   vtkDataSet *input = vtkPolyData::SafeDownCast (
                         inputInfo->Get ( vtkPolyData::DATA_OBJECT() ) );

    vtkDataSet *source = vtkPolyData::SafeDownCast (
                         sourceInfo->Get ( vtkPolyData::DATA_OBJECT() ) );

   vtkDataSet *output = vtkPolyData::SafeDownCast (
                        outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   if ( !source ){
      return 0;
   }

    vtkPolyData *connect1= vtkPolyData::New();
	vtkPolyData *connect2= vtkPolyData::New();
	vtkAppendPolyData *tempOutput=vtkAppendPolyData::New(); //this is set to append each piece of output


   //this check is realy impiortant when we just have a single process
    if (!this->Controller || this->Controller->GetNumberOfProcesses() <= 1)
    {
		// Nothing extra to do for single process.
		vtkPolyData *tempInput= vtkPolyData::SafeDownCast(input);
		vtkPolyData *tempSource= vtkPolyData::SafeDownCast(source);
		
		this->Superclass::ConstructOutput(tempInput, connect1, tempSource, this->TypeOfPoints);
		this->Superclass::ConstructOutput(tempSource, connect2, tempInput, this->TypeOfPoints);

		tempOutput->AddInput(connect1);
		
		tempOutput->AddInput(connect2);	

		tempOutput->Update();

		

		output->ShallowCopy(tempOutput->GetOutput());


		//free up memory
		tempOutput->Delete();
		connect1->Delete();
		connect2->Delete();	
		
		//tempInput->Delete();
		//tempSource->Delete();
	

		return 1;		
		
    }

#ifndef VTK_USE_MPI

	cout << "MPI is off " << endl;
	vtkPolyData *tempInput= vtkPolyData::SafeDownCast(input);
	vtkPolyData *tempSource= vtkPolyData::SafeDownCast(source);	
	this->Superclass::ConstructOutput(tempInput, connect1, tempSource, this->TypeOfPoints);
	this->Superclass::ConstructOutput(tempSource, connect2, tempInput, this->TypeOfPoints);
	tempOutput->AddInput(connect1);

	tempOutput->AddInput(connect2);
	tempOutput->Update();

	output->ShallowCopy(tempOutput->GetOutput());	

	//free up memory
	tempOutput->Delete();
	connect1->Delete();
	connect2->Delete();	
	
	//tempInput->Delete();
	//tempSource->Delete();

	return 1;

#else
	cout << "MPI is on " << endl;

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
   cout << "vtkPVIntersectWireframe Process Id " << processid << "+++++++++ Points set input  nbpt=" << numCellsInput << endl;

   vtkIdType numCellsSource;   
   numCellsSource = source->GetNumberOfPoints();

   cout <<"vtkPVIntersectWireframe Process Id " << processid << "++++++++++ Points set source  nbpt=" << numCellsSource << endl;
     

   vtkPolyData *input1= vtkPolyData::SafeDownCast(input);


   vtkPolyData *source1= vtkPolyData::SafeDownCast(source); 
    



   //two closed surfaces
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

	   cout <<"vtkPVIntersectWireframe Process ID " << processid << "++++++++++ Points set Allsource  nbpt=" << numOut << endl; 

	   //vtkPolyData *connect1= vtkPolyData::New();	  
       this->Superclass::ConstructOutput(input1, connect1, outSource, this->TypeOfPoints);
	   tempOutput->AddInput(connect1);
   
	   vtkIdType numCon;   
	   numCon = connect1->GetNumberOfPoints();
	   cout <<"vtkPVIntersectWireframe Process ID " << processid << "++++++++++ Points set connect1  nbpt=" << numCon << endl; 



	   vtkThresholdProcessId *allInput = vtkThresholdProcessId::New();
	   allInput->SetInput(input);
	   allInput->SetWholeOutput(1);
	   allInput->Update();
	   vtkPolyData *outInput =  vtkPolyData::New();
	   outInput->ShallowCopy( allInput->GetOutput() );
	
	   vtkIdType num1;   
	   num1 = outInput->GetNumberOfPoints();
	   cout <<"vtkPVIntersectWireframe Process ID " << processid << "++++++++++ Points set Allinput  nbpt=" << num1 << endl; 


	   //vtkPolyData *connect2= vtkPolyData::New();
	   this->Superclass::ConstructOutput(source1, connect2, outInput, this->TypeOfPoints);

	    vtkIdType numCon1 = connect2->GetNumberOfPoints();
	   cout <<"vtkPVIntersectWireframe Process ID" << processid << "++++++++++ Points set connect2  nbpt=" << numCon1 << endl; 


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
		collect->Delete();
		allInput->Delete();
		allSource->Delete();
		outSource->Delete();
		outInput->Delete();
    }   

   return 1;
#endif

   vtkErrorMacro("je suis la");
   //cout <<"je suis la"<< endl;
}
/**************************MIRARCO*****************************************
* Function: 		PrintSelf           	                              *
* Description:                                                            *
* Input parameters :													  *		                                                                 
* Output Parametres:													  * 
*Author Arolde VIDJINNAGNI                                                *
*Date:					                                                  *
***************************************************************************/
void vtkIntersectFilter::PrintSelf ( ostream& os, vtkIndent indent)
{   
/*  vtkPolyData *source = this->GetSource();
  this->Superclass::PrintSelf ( os, indent );
  os << indent << "source: " << source << "\n";
*/
}
