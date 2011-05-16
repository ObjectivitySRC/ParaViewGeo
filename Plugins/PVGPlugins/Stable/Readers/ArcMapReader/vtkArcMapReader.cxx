// .NAME vtkArcMapReader.cxx
// By: Eric Daoust
// Read ArcMap file (.csv) for single objects.
#include "vtkArcMapReader.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCollection.h"
#include <vtksys/ios/sstream>
#include "vtkStringList.h"
#include "vtkVoxel.h"
#include "vtkCleanUnstructuredGrid.h"
#include "vtkCellData.h"
#include <sstream>

#define SMALL_ELEVATION_VALUE 0.0001
#define INTERPOLATION 2

vtkCxxRevisionMacro(vtkArcMapReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkArcMapReader);

// Constructor
vtkArcMapReader::vtkArcMapReader()
{
	this->SetNumberOfOutputPorts(1);
	this->FileName = 0;
	this->SetNumberOfInputPorts(0);
	this->xAxisJumpValue = 0;
	this->yAxisJumpValue = 0;
};

// --------------------------------------
// Destructor
vtkArcMapReader::~vtkArcMapReader()
{
	this->SetFileName(0);
}

// --------------------------------------
void vtkArcMapReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent <<  "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

// --------------------------------------
int vtkArcMapReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	// Make sure we have a file to read.
	if(!this->FileName)  {
		vtkErrorMacro("No file name specified.  Cannot open.");
		return 0;
	}
	if(strlen(this->FileName)==0)  {
		vtkErrorMacro("File name is null.  Cannot open.");
		return 0;
	}
	ifstream file;
	file.open(this->FileName, ios::in);
	// Check to prevent crashing if file does not exist!
	if(!file)
	{
		vtkErrorMacro("ArcMap data file does not exist!");
		file.close();
		return 1;
	}

	vtkUnstructuredGrid *output = vtkUnstructuredGrid::GetData(outputVector);
	output->ReleaseDataFlagOn();

	double currentX, currentY;
	
	vtkFloatArray *zList = vtkFloatArray::New();
	vtkIdTypeArray *props = vtkIdTypeArray::New();

	//READ HEADER LINE - IGNORE CONTENTS
	this->Read(&file, *&currentX, *&currentY, zList, props, 1);
	zList->Reset();
	props->Reset();
	
	double baseX, baseY, baseZ;
	double lengthX, lengthY;
	
	lengthX = this->xAxisJumpValue;
	lengthY = this->yAxisJumpValue;
	
	vtkIdType currentPointId = 0;
	
	vtkCellArray *cells = vtkCellArray::New();
	vtkPoints *points = vtkPoints::New();
	vtkFloatArray *cellProperties = vtkFloatArray::New();
	cellProperties->SetName("GEOLOGY");
	
	int iterations = 0;
	int prevX = 0;
	
	while (this->Read(&file, *&currentX, *&currentY, zList, props, INTERPOLATION))
	{
		if(currentX != prevX)
		{
			//skip current row of X
			if(!this->SkipXRow(&file, *&currentX, *&currentY, zList, props, INTERPOLATION, currentX))
				break;
			prevX = currentX;
			zList->Reset();
			props->Reset();
		}
		this->Sort(zList, props);
		double previousZ = 0;
		baseX = currentX - (lengthX/2.0);
		baseY = currentY - (lengthY/2.0);
		for(int j = 0; j < zList->GetNumberOfTuples(); j++)
			{
			currentPointId = points->GetNumberOfPoints();
			vtkVoxel *voxel = vtkVoxel::New();
			
			baseZ = zList->GetTuple1(j);
			
			//we must insert all 8 points in both voxel and final grid
			voxel->GetPointIds()->SetId(0,currentPointId);
			points->InsertPoint(currentPointId, baseX, baseY, previousZ);
			currentPointId++;

			voxel->GetPointIds()->SetId(1,currentPointId);
			points->InsertPoint(currentPointId, baseX+lengthX, baseY, previousZ);
			currentPointId++;

			voxel->GetPointIds()->SetId(2,currentPointId);
			points->InsertPoint(currentPointId, baseX, baseY+lengthY, previousZ);
			currentPointId++;

			voxel->GetPointIds()->SetId(3,currentPointId);
			points->InsertPoint(currentPointId, baseX+lengthX, baseY+lengthY, previousZ);
			currentPointId++;

			voxel->GetPointIds()->SetId(4,currentPointId);
			points->InsertPoint(currentPointId, baseX, baseY, baseZ);
			currentPointId++;

			voxel->GetPointIds()->SetId(5,currentPointId);
			points->InsertPoint(currentPointId, baseX+lengthX, baseY, baseZ);
			currentPointId++;

			voxel->GetPointIds()->SetId(6,currentPointId);
			points->InsertPoint(currentPointId, baseX, baseY+lengthY, baseZ);
			currentPointId++;
			
			voxel->GetPointIds()->SetId(7,currentPointId);
			points->InsertPoint(currentPointId, baseX+lengthX, baseY+lengthY, baseZ);
			
			cells->InsertNextCell(voxel);
			
			cellProperties->InsertNextTuple1(props->GetValue(j));
			voxel->Delete();
			previousZ = baseZ;
			
			}
		zList->Reset();
		props->Reset();
		iterations++;
	} 
	
	//cleanup memory
	file.close();
	zList->Delete();	
	props->Delete();	
	
	//need a temp storage for the grid, so we can clean it
	vtkUnstructuredGrid *temp = vtkUnstructuredGrid::New();	
	temp->SetPoints(points);
	temp->SetCells(VTK_VOXEL, cells);
	temp->GetCellData()->AddArray(cellProperties);
	
	//I expect we can delete here
	points->Delete();
	cells->Delete();
	cellProperties->Delete();
		
	//clean the grid
	vtkCleanUnstructuredGrid *clean = vtkCleanUnstructuredGrid::New();
	clean->SetInput(temp);
	clean->Update();
	output->ShallowCopy( clean->GetOutput() );
	
	//more memory cleanup
	clean->Delete();
	temp->Delete();
	
	return 1;
}

void vtkArcMapReader::Sort(vtkFloatArray *list, vtkIdTypeArray *props)
{
	int minId;
	double minVal;
	double temp;
	double minProp;
	for(int i = 0; i < list->GetNumberOfTuples()-1; i++)
	{
		minId = i;
		minVal = list->GetTuple1(i);
		minProp = props->GetTuple1(i);
		for(int j = i+1; j < list->GetNumberOfTuples(); j++)
		{
			if(list->GetTuple1(j) <= minVal)
			{
				temp = list->GetTuple1(j);
				if(temp == minVal)
				{
					list->SetTuple1(j, temp - SMALL_ELEVATION_VALUE);
				}
				minId = j;
				minVal = list->GetTuple1(j);
				minProp = props->GetTuple1(j);
			}
			
		}
		if(minId > i)
		{
			//slide values down the array so that the lower property values maintain priority
			for(int k = minId; k > i; k--)
			{
				list->SetTuple1(k, list->GetTuple1(k-1));
				props->SetTuple1(k, props->GetTuple1(k-1));
			}
			list->SetTuple1(i, minVal);
			props->SetTuple1(i, minProp);
		}
	}
}

bool vtkArcMapReader::SkipXRow(ifstream* file, double &x, double &y, vtkFloatArray *z, vtkIdTypeArray *prop, int linesToRead, int currentX)
{
	//int x = currentX;
	while(x == currentX)
	{
		if(!this->Read(file, x, y, z, prop, INTERPOLATION))
		{
			return false;
		}
	}
	return true;
}

bool vtkArcMapReader::Read(vtksys_ios::ifstream* file, double &x, double &y, vtkFloatArray *z, vtkIdTypeArray *props, int linesToRead)
{
	vtkStdString line;
	for(int i = 0; i < linesToRead; i++)
	{
		std::getline(*file, line);
		if(line == "" || line == NULL)
		{
			return false;
		}
		//line.clear();
	}
	vtkStringList* entries = this->parseString(line);
	vtkStdString id = entries->GetString(0);
	
	//index 0 contains the row ID, we do not need it
	vtkStdString currentString = entries->GetString(1);
	this->textStream.str(currentString); //X value
	this->textStream >> x;
	this->textStream.clear();
	currentString = entries->GetString(2);
	this->textStream.str(currentString); //Y value
	this->textStream >> y;
	this->textStream.clear();
	//int i = 3; 
	double value;
	//while(i < entries->GetNumberOfStrings())
	for(int i = 3; i < entries->GetNumberOfStrings(); i++) //Z values begin at index 3 on the line	
	{
		currentString = entries->GetString(i);
		this->textStream.str(currentString);
		this->textStream >> value;
		if(value > 0)
		{
			z->InsertNextTuple1(value);
			props->InsertNextTuple1(i-3);
		}
		this->textStream.clear();
	}
	entries->Delete();
	return true;
}

vtkStringList* vtkArcMapReader::parseString(vtkStdString line){
  vtkStdString::size_type loc;
  vtkStdString substr = line;
  vtkStringList *values = vtkStringList::New();
  int x = 0;
  do{
   loc = substr.find(",", 1);
   values->AddString(substr.substr(0,loc).c_str());
   x++;
   if (loc != vtkStdString::npos){
    substr = substr.substr(loc+1);
   }
  }while(loc != vtkStdString::npos);
  return values;
}

int vtkArcMapReader::RequestInformation(vtkInformation *,vtkInformationVector **,vtkInformationVector *outputVector)
{
	// Make sure we have a file to read.
	if(!this->FileName)  {
		vtkErrorMacro("No file name specified.  Cannot open.");
		return 0;
	}
	if(strlen(this->FileName)==0)  {
		vtkErrorMacro("File name is null.  Cannot open.");
		return 0;
	}
	ifstream file;
	file.open(this->FileName, ios::in);
	// Check to prevent crashing if file does not exist!
	if(!file)
	{
		vtkErrorMacro("ArcMap data file does not exist!");
		file.close();
		return 1;
	}
	bool xJumpFound = false;
	bool yJumpFound = false;
	double x, y;
	double oldX, oldY;
	vtkFloatArray *z = vtkFloatArray::New();
	vtkIdTypeArray *p = vtkIdTypeArray::New();
	//HEADER - ignore it
	this->Read(&file, *&x, *&y, z, p, 1);
	//first line, init oldX and oldY
	this->Read(&file, *&x, *&y, z, p, 1);
	oldX = x;
	oldY = y;
	
	while(this->Read(&file, *&x, *&y, z, p, 1) &&  (yJumpFound == false || xJumpFound == false))
		{
		if(!xJumpFound && oldX != x)
			{
			this->xAxisJumpValue = (x - oldX) * INTERPOLATION;
			xJumpFound = true;
			}
		if(!yJumpFound && oldY != y)
			{
			this->yAxisJumpValue = (y - oldY) * INTERPOLATION;
			yJumpFound = true;
			}
		z->Reset();
		p->Reset();
		}
	z->Delete();
	p->Delete();
	file.close();
	
	return 1;
}