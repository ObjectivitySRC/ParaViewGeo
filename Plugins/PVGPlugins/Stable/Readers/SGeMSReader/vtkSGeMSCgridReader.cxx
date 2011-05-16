// .NAME vtkSGeMSCgridReader.cxx
// Read SGeMS binary files for single objects.
// point, (or Cartesian Grids via vtkSGeMSCgridReader
// With or without properties 

#include "vtkSGeMSCgridReader.h"
#include <list>
#include <string>
#include <sstream>
#include <iostream>

#include "vtkCellArray.h"
#include "vtkCallbackCommand.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkObjectFactory.h"

#include "vtkCellData.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkStringArray.h"
#include "vtkStringList.h"
#include "vtkLongArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include <vtkOutputWindow.h>

//going to help find leaks
#include "vtkDebugLeaks.h"


vtkCxxRevisionMacro(vtkSGeMSCgridReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkSGeMSCgridReader);
//---------------------------------------
// Constructor
vtkSGeMSCgridReader::vtkSGeMSCgridReader()
  {
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
  };

// --------------------------------------
// Destructor
vtkSGeMSCgridReader::~vtkSGeMSCgridReader()
  {
  //deleting object variables
  cout << "starting delete" << endl;
  this->SetFileName(0);
  }

// --------------------------------------
void vtkSGeMSCgridReader::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
  }

// --------------------------------------
int vtkSGeMSCgridReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
  {

  

	FILE* fp = fopen(this->FileName,"rb");
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	vtkSmartPointer<vtkCellArray> myCellsPtr = vtkSmartPointer<vtkCellArray>::New();
	vtkFloatArray* fap = NULL;
	vtkLongArray* fapLAP = vtkLongArray::New();

  
// =============================================== John Start Here

	// Read the metadata from the file
	int magicNumber = getInt(fp);
	if (magicNumber != 0xB211175D)
	{
		vtkErrorMacro("File does not begin with proper key.");
    return 0;
	}

	int nextWordSize = getInt(fp);
	char* className = new char[nextWordSize];
		fread(className,1,nextWordSize,fp);
	if (strcmp(className,"Cgrid") != 0)
	{
		vtkErrorMacro("File is not an SGEMS cartesian grid (Cgrid). ");
    return 0;
	}

	nextWordSize = getInt(fp);
	char* objectName = new char[nextWordSize];
		fread(objectName,1,nextWordSize,fp);
	int versionNumber = getInt(fp);

	int grid_xdim = getInt(fp);
	int grid_ydim = getInt(fp);
	int grid_zdim = getInt(fp);

	float cell_xdim = getFloat(fp);
	float cell_ydim = getFloat(fp);
	float cell_zdim = getFloat(fp);

	float origin_xcoord = getFloat(fp);
	float origin_ycoord = getFloat(fp);
	float origin_zcoord = getFloat(fp);

	int numCells = grid_xdim * grid_ydim * grid_zdim;
	int numProps = getInt(fp);
	
	char propName[20];
	for(int i=0;i<numProps;i++)
	{
		nextWordSize = getInt(fp);
		fread(propName,1,nextWordSize,fp);
		fap = vtkFloatArray::New();								//Create an array for each property
		fap->Allocate(numCells);									//(pre-allocates, to opt. mem. usage)
		fapLAP->InsertNextValue((long)fap);				//Stores the reference to this array
		((vtkDataArray*)fap)->SetName(propName);	//Set the name of this array
	}

	//myCellsPtr->Allocate(numCells+1); <-- Do we need this?


	// Read the property values from the file, into the vtk data structure.
	float propVal;
	for(int currentProp=0; currentProp<numProps; currentProp++)
	{
		//Retrieve the reference to the current property array.
		fap = (vtkFloatArray*)fapLAP->GetValue(currentProp);

		for (int currentCell=0; currentCell<numCells; currentCell++)
		{
			propVal = getFloat(fp);
			fap->InsertNextValue(propVal);
		}
	}
	fclose(fp);
			

	// Set reader's output to type "ImageData"
	vtkImageData *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	output->SetOrigin(origin_xcoord,origin_ycoord,origin_zcoord);
	output->SetDimensions(grid_xdim+1, grid_ydim+1, grid_zdim+1);
	output->SetSpacing(cell_xdim, cell_ydim, cell_zdim);
	output->SetWholeExtent(0,grid_xdim, 0,grid_ydim, 0,grid_zdim);
	output->SetExtent(0,grid_xdim, 0,grid_ydim, 0,grid_zdim);

  //add property arrays to points or cells

	/*  <<< we don't know how the properties are stored for ImageData >>> */

  for (int currentProperty=0; currentProperty < numProps; currentProperty++) 
    {
			// Add property data to the point data arrays.
      output->GetCellData()->AddArray(((vtkDataArray*)fapLAP->GetValue(currentProperty)));

			// Delete the original property data arrays.
      ((vtkDataArray*)fapLAP->GetValue(currentProperty))->Delete();
    }
		/* */

	delete className;
	delete objectName;

  return 1;
}

//--------------------------------------------------------------------------------
int vtkSGeMSCgridReader::getInt(FILE* fp)
{
	char buff[4];					// Prep for reading 4 bytes
	fread(buff,1,4,fp);		// Read 4 bytes
	
	char reverseBuff[4];
	for (int i=0; i<4; i++)
	{
		reverseBuff[i] = buff[3-i];
	}

	return *(int*)reverseBuff;		// Translate the 4-byte data chunk.
}

float vtkSGeMSCgridReader::getFloat(FILE* fp)
{
	char buff[4];
	fread(buff,1,4,fp);

	char reverseBuff[4];
	for (int i=0; i<4; i++)
	{
		reverseBuff[i] = buff[3-i];
	}
	return *(float*)reverseBuff;
}