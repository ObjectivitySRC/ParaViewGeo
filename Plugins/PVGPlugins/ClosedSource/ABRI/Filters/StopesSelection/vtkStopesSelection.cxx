/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkStopesSelection.cxx $
  Authors:    Arolde VIDJINNAGNI - Kiel Daoust - Darren - Rafik FAHEM
  MIRARCO, Laurentian University
  Date:    May 2009 
  Version:   0.1
=========================================================================*/

#include "vtkStopesSelection.h"
#include "vtkStopesResearch.h"
#include "vtkCompleteBlockFilter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkCxxRevisionMacro ( vtkStopesSelection, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkStopesSelection );


//========================================================================================
vtkStopesSelection::vtkStopesSelection(void)
{
	InstituteTone = 250.0 ;
	Swell = 0.6;
	ExtractionCapacity = 100000.0;

	XINC = NULL;
	YINC = NULL;
	ZINC = NULL;
	this->stopeWidth=10;
	this->stopeDepth = 30.0;
	this->densityInput = NULL;
	this->fileName = NULL;
	this->fileButton = 0;
	Function=NULL;
	FitnessArrayName= NULL;
  ReplacementValue= 0.0;
	ReplaceInvalidValues=0;
	BlockModelType=0;	
}


//========================================================================================
vtkStopesSelection::~vtkStopesSelection(void)
{
}
//----------------------------------------------------------------------------
int vtkStopesSelection::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  return 1;
  }

//----------------------------------------------------------------------------
int vtkStopesSelection::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");	
  return 1;
}

//----------------------------------------------------------------------------
int  vtkStopesSelection::TestBlockType(vtkPolyData* dataset)
{
	int test=0;
	double range[2]={0.0};

	this->XINCArray->GetRange(range);
	if(range[0]==range[1])
		test++;

	range[0]=0; range[1]=0;
	this->YINCArray->GetRange(range);
	if(range[0]==range[1])
		test++;

	range[0]=0; range[1]=0;
	this->ZINCArray->GetRange(range);
	if(range[0]==range[1])
		test++;

	if(test==3)
		return 0;
	else return 1;	
}

//--------------------------------------------------------------------------
int vtkStopesSelection::RequestData( vtkInformation *request, 
                         vtkInformationVector **InputVector, 
                         vtkInformationVector *outputVector )
{
   vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

   vtkPolyData* input = vtkPolyData::SafeDownCast (
                         inputInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   vtkPolyData *output = vtkPolyData::SafeDownCast (
   outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );


	 this->XINCArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->XINC));
	 this->YINCArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->YINC));
	 this->ZINCArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->ZINC));
	
   if((this->XINCArray==NULL)||(this->YINCArray==NULL)||(this->ZINCArray==NULL))
		{
			vtkErrorMacro("Size arrays are not set on the block model");
		}	

	 this->BlockModelType = TestBlockType(input);
	 vtkCompleteBlockFilter* CompleteBlock= vtkCompleteBlockFilter::New();
	 CompleteBlock->SetInput(input);
	 CompleteBlock->SetXINC(this->XINC);
	 CompleteBlock->SetYINC(this->YINC);
	 CompleteBlock->SetZINC(this->ZINC);
	 CompleteBlock->SetBlockModelType(this->BlockModelType);
	 CompleteBlock->Update();

	 vtkStopesResearch* temp= vtkStopesResearch::New();
	 temp->SetInput(CompleteBlock->GetOutput());
	 temp->SetBlockModelType(this->BlockModelType);
	 temp->SetSwell(this->Swell);
	 temp->SetInstituteTone(this->InstituteTone);
	 temp->SetExtractionCapacity(this->ExtractionCapacity);
	 temp->SetstopeDepth(this->stopeDepth);
	 temp->SetstopeWidth(this->stopeWidth);
	 temp->SetXINC(this->XINC);
	 temp->SetYINC(this->YINC);
	 temp->SetZINC(this->ZINC);
	 temp->SetdensityInput(this->densityInput);
	 temp->SetFitnessArrayName(this->FitnessArrayName);
	 temp->SetFunction(this->Function);
	 temp->SetReplacementValue(this->ReplacementValue);
	 temp->SetReplaceInvalidValues(this->ReplaceInvalidValues);
	 temp->Update();

	 CompleteBlock->Delete();
	 output->ShallowCopy(temp->GetOutput());
	 temp->Delete();	

  return 1;
}