/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkDistanceToGrid.cxx $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      June 1, 2008
	Revised: February 23, 2009
	Revision by: Matthew Livingstone
  Version:   0.8

  =========================================================================*/
#include "vtkAnnotation.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStringArray.h"
#include "vtkTable.h"
#include "vtkDataSet.h"
#include "vtkTetgen.h"
#include "vtkComputeVolumes.h"
#include "vtkStdString.h"
#include "vtkStringArray.h"

#include <vtkstd/string>
#include <sstream>

vtkStandardNewMacro(vtkAnnotation);
vtkCxxRevisionMacro(vtkAnnotation, "$Revision: 1.1 $");
//----------------------------------------------------------------------------
vtkAnnotation::vtkAnnotation()
{
  this->Format = 0;
  this->SetFormat("Volume for object");  
}

//----------------------------------------------------------------------------
vtkAnnotation::~vtkAnnotation()
{
  this->SetFormat(0);
}

//----------------------------------------------------------------------------
int vtkAnnotation::FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");  
  return 1;
}

//----------------------------------------------------------------------------
int vtkAnnotation::RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkTable* output = vtkTable::SafeDownCast(outInfo->Get( vtkDataObject::DATA_OBJECT() ));//vtkTable::GetData(outputVector);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataSet *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

	// MLivingstone
	// New way of getting volume, much more accurate
	// Use tetgen to tetrahedralize the data
	vtkTetgen *tetgenObj = vtkTetgen::New();
	tetgenObj->SetInput(input);
	tetgenObj->SettetgenCommand(1);
	tetgenObj->SetWithRegions(1);
	tetgenObj->Update();

	// Use ComputeVolumes to calculate the volume of the tetrahedrons
	vtkComputeVolumes *compVol = vtkComputeVolumes::New();
	compVol->SetInput(tetgenObj->GetOutput());
	compVol->SetRegionArray("Region Numbers");
	compVol->Update();

	// regionVols will hold the volume for each region (each individual closed surface)
	vtkStringArray *regionVols = vtkStringArray::New();
	// String of all volumes, delimited with "|"
	// Ex: 0.234|123.212|3214.12
	vtkStdString volumes = compVol->GetVolumesArray();

	// Find first instance of "|"
	int loc = volumes.find("|",0);
	// Stores values of where substr will start
	int startLoc = 0;
	// While we are finding "|"
	while (loc != -1)
		{
		// Grab the volume
		regionVols->InsertNextValue(volumes.substr(startLoc, (loc)-startLoc));
		// Move the starting location up for the next substr
		startLoc = loc+1;
		// See if there is another volume
		loc = volumes.find("|",loc+1);
		}

	double volumeTotal = 0;
	double tempVol;
	// buffer is used to hold the output for the billboard
	vtkstd::ostringstream buffer;
	for (int i = 0; i < regionVols->GetNumberOfValues(); i++)
		{
		buffer << this->Format << " " << i << " is: " << regionVols->GetValue(i) << " units3" << endl;

		// Convert the string to a double
		tempVol = atof(regionVols->GetValue(i).c_str());
		volumeTotal += tempVol;
		}
	buffer << "Total volume is: " << volumeTotal << " units3" << endl;

  vtkStringArray * data = vtkStringArray::New();  
  data->SetName("Text");
  data->SetNumberOfComponents(1);
  data->InsertNextValue(buffer.str().c_str()); 
  output->AddColumn(data);
  
  //cleanup
  data->Delete();
	tetgenObj->Delete();
	compVol->Delete();
	regionVols->Delete();
  return 1;
}

//----------------------------------------------------------------------------
void vtkAnnotation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Format: " << (this->Format? this->Format : "(none)") << endl;
}


