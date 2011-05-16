// .NAME vtkDXFReader.cxx
// By: Robert Maynard && Matthew Livingstone && Eric Daoust
// made to remove memory leaks in the original class

#include "vtkDXFReader.h"
#include "vtkDXFObject.h"
#include "vtkObjectFactory.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkCxxRevisionMacro(vtkDXFReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkDXFReader);

// Constructor
vtkDXFReader::vtkDXFReader()
{
	this->SetNumberOfOutputPorts(1);
	this->FileName = 0;
  this->DrawHidden  = false;
  this->DrawPoints = false;
	this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkDXFReader::~vtkDXFReader()
{
	this->SetFileName(0);
}

// --------------------------------------
void vtkDXFReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent <<  "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

// --------------------------------------
int vtkDXFReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	// Make sure we have a file to read.
	if(!this->FileName)  {
		vtkErrorMacro("A FileName must be specified.");
		return 0;
	}
	if(strlen(this->FileName)==0)  {
		vtkErrorMacro("A NULL FileName.");
		return 0;
	}

	// Added to fix crash that would occur if a DXF was on the recent file menu
	// but had be deleted from the HDD, therefore no longer existing...
	// MLivingstone
	ifstream file;
	file.open(this->FileName, ios::in);
	if(!file)
	{
		vtkErrorMacro("DXF file does not exist!");
		file.close();
		return 1;
	}
	file.close();

  vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );
	 
	vtkMultiBlockDataSet *output = vtkMultiBlockDataSet::SafeDownCast (
                         outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
	
  //this fixes the problem with vtkDXFObject holding references to way to many instances
  //of classes, causing it to uses roughly 60mb of memory it does not need
  //by doing a deep copy of the data, we can make sure we release all the instances that dxf created but could not delete
	vtkDXFObject *dxfObject = vtkDXFObject::New();
	dxfObject->SetFileName( this->FileName );
	dxfObject->SetDrawPoints( this->GetDrawPoints() );
	dxfObject->SetDrawHidden( this->GetDrawHidden() );
	dxfObject->SetAutoScale( this->GetAutoScale() );
	
	dxfObject->Update();
	output->DeepCopy( dxfObject->GetOutput() );

	dxfObject->Delete();

	return 1;
}