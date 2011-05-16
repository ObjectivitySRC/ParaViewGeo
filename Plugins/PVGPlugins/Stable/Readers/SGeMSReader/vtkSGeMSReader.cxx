 /*=======================================================================
 "vtkSGeMSReader"
 - Written by John Mavity and Bob Anderson
 - July, 2008 A.D.
 
 This program was re-written and expanded by John Mavity and Bob Anderson;
 it now enables Paraview to read two ["Cgrid","Point_set"]  of the three 
 [reduced grid, cartesian grid, and point set] SGeMS* file formats.

*[Stanford Geostatistical Earth  Modeling Software] 
=========================================================================*/

#include "vtkSGeMSReader.h"
#include "vtkSGeMSCgridReader.h"
#include <list>
#include <string>
#include <sstream>
#include <iostream>

#include "vtkCellArray.h"
#include "vtkCallbackCommand.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"
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


vtkCxxRevisionMacro(vtkSGeMSReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkSGeMSReader);
//---------------------------------------
// Constructor
vtkSGeMSReader::vtkSGeMSReader()
  {
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
  };

// --------------------------------------
// Destructor
vtkSGeMSReader::~vtkSGeMSReader()
  {
  //deleting object variables
  cout << "starting delete" << endl;
  this->SetFileName(0);
  }

// --------------------------------------
void vtkSGeMSReader::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
  }

// --------------------------------------
int vtkSGeMSReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
  {
	// Prepare to branch, if Cgrid.
	if (this->fileType == 'C') 
  {
		vtkSGeMSCgridReader *cgridrdr = vtkSGeMSCgridReader::New();
		cgridrdr->SetFileName(this->FileName);
		cgridrdr->Update();
	  
		//hack stolen from vtkDataSetReader
		vtkDataObject *output = vtkImageData::New();
		this->GetExecutive()->SetOutputData(0, output);
		output->Delete();
	  
		output->ShallowCopy(cgridrdr->GetOutput());
		cgridrdr->Delete();
		return 1;
  }

	FILE* fp = fopen(this->FileName,"rb");
	vtkIdType nodes[1];
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	vtkSmartPointer<vtkPoints> myPointsPtr = vtkSmartPointer<vtkPoints>::New();
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
	if (strcmp(className,"Point_set") != 0)
	{
		vtkErrorMacro("File is not an SGEMS point set.");
    return 0;
	}

	nextWordSize = getInt(fp);
	char* objectName = new char[nextWordSize];
		fread(objectName,1,nextWordSize,fp);
	int versionNumber = getInt(fp);
	int numPoints = getInt(fp);
	int numProps = getInt(fp);
	

	char propName[2048];
	for(int i=0;i<numProps;i++)
	{
		nextWordSize = getInt(fp);
		fread(propName,1,nextWordSize,fp);
		fap = vtkFloatArray::New();								//Create an array for each property
		fap->Allocate(numPoints);									// (pre-allocates, to opt. mem. usage)
		fapLAP->InsertNextValue((long)fap);				//Stores the reference to this array
		((vtkDataArray*)fap)->SetName(propName);	//Set the name of this array
	}

	myPointsPtr->Allocate(numPoints+1);
	myCellsPtr->Allocate(numPoints+1);
	float xCoord;
	float yCoord;
	float zCoord;

	// Read the point coordinates from the file, into the vtk data structure.
	for(int currentPoint=0;currentPoint<numPoints;currentPoint++)
	{
		xCoord = getFloat(fp);
		yCoord = getFloat(fp);
		zCoord = getFloat(fp);

		// fill the vtk coordinate array
		myPointsPtr->InsertPoint(currentPoint, xCoord, yCoord, zCoord);

		// Store the integer "currentPoint" so it can be read by vtk
		nodes[0] = currentPoint;

		// Add another cell, containing the point index, to the vtk cell array.
    myCellsPtr->InsertNextCell(1,nodes);

	}
	
	// Read the property values from the file, into the vtk data structure.
	float propVal;
	for(int currentProp=0; currentProp<numProps; currentProp++)
	{
		//Retrieve the reference to the current property array.
		fap = (vtkFloatArray*)fapLAP->GetValue(currentProp);

		for (int currentPoint=0; currentPoint<numPoints; currentPoint++)
		{
			propVal = getFloat(fp);
			fap->InsertNextValue(propVal);
		}
	}
	fclose(fp);
			

	// Set reader's output to type "PolyData"
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	//Store the points and cells in the output data object.
  output->SetPoints(myPointsPtr);
  output->SetVerts(myCellsPtr);

  //add property arrays to points or cells
  for (int currentProperty=0; currentProperty < numProps; currentProperty++) 
    {
			// Add property data to the point data arrays.
      output->GetPointData()->AddArray(((vtkDataArray*)fapLAP->GetValue(currentProperty)));

			// Delete the original property data arrays.
      ((vtkDataArray*)fapLAP->GetValue(currentProperty))->Delete();
    }

	delete className;
	delete objectName;

  return 1;
}


int vtkSGeMSReader::FillOutputPortInformation(int,  vtkInformation* info)
{
	FILE* fp = fopen(this->FileName,"rb");
	int magicNumber = getInt(fp);
	if (magicNumber != 0xB211175D)
	{
		vtkErrorMacro("File does not begin with proper key.");
    return 0;
	}
	int nextWordSize = getInt(fp);
	char* className = new char[nextWordSize];
	fread(className,1,nextWordSize,fp);
	fclose(fp);

	if (strcmp(className,"Point_set") == 0)
		this->fileType = 'P';
	else if (strcmp(className,"Cgrid") == 0)
		this->fileType = 'C';
	else if (strcmp(className,"Rgrid") == 0)
	{
		vtkErrorMacro("This file type (Rgrid) is not yet supported.");
    return 0;
	}
	else
	{
		vtkErrorMacro("This file type cannot be supported.");
    return 0;
	}
	
	if (this->fileType=='P')
		info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	else
		info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
  return 1;
}

//--------------------------------------------------------------------------------
int vtkSGeMSReader::getInt(FILE* fp)
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

float vtkSGeMSReader::getFloat(FILE* fp)
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