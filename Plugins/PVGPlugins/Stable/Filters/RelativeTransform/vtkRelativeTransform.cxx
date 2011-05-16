/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkRelativeTransform.cxx $
  Author:    Chris Cameron
             MIRARCO Mining Innovation
  Date:      August 29, 2008
  Version:   0.1

  =========================================================================*/

#include "vtkRelativeTransform.h"

#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include "vtkPointData.h"
#include "vtkPointLocator.h"

#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"

#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"

#include "vtkTransformFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

vtkCxxRevisionMacro(vtkRelativeTransform, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkRelativeTransform);

//----------------------------------------------------------------------------
vtkRelativeTransform::vtkRelativeTransform()
{
  //this->Trans = new double[3];
  //this->Rot = new double[3];
  //this->Scale = new double[3];
  for (int i = 0; i < 3; i++)
  {
    this->Trans[i] = 0;
    this->Rot[i] = 0;
    this->Scale[i] = 1;
		this->Origin[i] = 0;
  }
}

//-----------------------------------------------------------------------------
int vtkRelativeTransform::RequestInformation(
  vtkInformation* vtkNotUsed( request ),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
	
	// get the input
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataSet *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

	if ( input )
		{
		double *bounds = new double[6];

		input->GetBounds(bounds);

		this->Origin[0] = (bounds[0] + bounds[1]) / 2;
		this->Origin[1] = (bounds[2] + bounds[3]) / 2;
		this->Origin[2] = (bounds[4] + bounds[5]) / 2;

		delete[] bounds;  
		}

	return 1;
}


//-----------------------------------------------------------------------------
int vtkRelativeTransform::RequestData(
  vtkInformation* vtkNotUsed( request ),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  // get the input
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataSet *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  // get the output
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkDataSet *finalOutput = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    
  vtkTransformFilter *originFilter = vtkTransformFilter::New();
  vtkTransform *origin = vtkTransform::New();
	origin->Translate(-this->Origin[0], -this->Origin[1], -this->Origin[2]); //move it relative to the origin
  originFilter->SetInput(input);
  originFilter->SetTransform(origin);
  originFilter->Update();

  vtkDataSet *originInput = originFilter->GetOutput();
  
  vtkTransformFilter *transFilter = vtkTransformFilter::New();
  vtkTransform *trans = vtkTransform::New();
  trans->Translate(this->Trans);
  trans->RotateX(this->Rot[0]);
  trans->RotateY(this->Rot[1]);
  trans->RotateZ(this->Rot[2]);
  trans->Scale(this->Scale);
  transFilter->SetInput(originInput);
  transFilter->SetTransform(trans);
  transFilter->Update();

  vtkDataSet *originTransformInput = transFilter->GetOutput();

  vtkTransformFilter *backFilter = vtkTransformFilter::New();
  vtkTransform *back = vtkTransform::New();
	back->Translate(this->Origin); //move it to where the user wants
  backFilter->SetInput(originTransformInput);
  backFilter->SetTransform(back);
  backFilter->Update();

  finalOutput->ShallowCopy(backFilter->GetOutput());
  
  backFilter->Delete();
  back->Delete();

  transFilter->Delete();
  trans->Delete();

  originFilter->Delete();
  origin->Delete();


  return 1;
}


//-----------------------------------------------------------------------------
void vtkRelativeTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Trans:" << this->Trans << "\n";
  
  os << indent << "Rot:" << this->Rot << "\n";
  
  os << indent << "Scale:" << this->Scale << "\n";
}



