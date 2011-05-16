#include "vtkTriReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>

#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkFloatArray.h"
#include "vtkStringList.h"
#include "vtkLongArray.h"
#include <vtkOutputWindow.h>
// Added for MultiBlockDataSet usage
#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkMultiBlockDataSet.h"

#include "vtkExecutive.h"

#define TRIAG 3
#define TRIAG_POINTS 9
#define X 0
#define Y 1
#define Z 2

vtkCxxRevisionMacro(vtkTriReader, "$Revision: 1.5 $");
vtkStandardNewMacro(vtkTriReader);

// Forward declaration for using stringstream
typedef vtkstd::stringstream vtkSBufferBase;
class vtkSBuffer: public vtkSBufferBase
{};

// Constructor
vtkTriReader::vtkTriReader()
{
  this->HeaderExists = 0;
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkTriReader::~vtkTriReader()
{
  this->SetFileName(0);
}

// --------------------------------------
void vtkTriReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

// --------------------------------------
int vtkTriReader::RequestData(vtkInformation* request,
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

  vtkPoints* myPointsPtr = vtkPoints::New();
	vtkCellArray* myCellsPtr = vtkCellArray::New();

	vtkMultiBlockDataSet *output;
  output = (vtkMultiBlockDataSet*)this->GetExecutive()->GetOutputData(0);

	bool lastLineEmpty = false;
	int blockCounter = 0;

  vtkStdString line;
  ifstream myfile (this->FileName);
  double *cords;
  int counter = 0;
  if (myfile.is_open())
		{
    if (this->HeaderExists == true)
      {
      getline(myfile,line);
			lastLineEmpty = true;
      }
    while (! myfile.eof() ) //make sure the line exists
      {
      
      getline (myfile,line);
      if (line.size() > 0 ) //dont read in whitespace lines
				{
				// Push the first character of the line into a stream,
				// so that it can be converted to a char
				vtkSBuffer sStream;
				sStream << line.substr(0, 1);
				char num;
				sStream >> num;
				// Is the first character a number?
				if (isdigit(num))
					{
					lastLineEmpty = false;
					cords = parseString(line);

					myCellsPtr->InsertNextCell(TRIAG);
					for (int i=0; i < TRIAG_POINTS; i+=TRIAG)
						{
						myPointsPtr->InsertNextPoint(cords[i],cords[i+Y],cords[i+Z]);
						myCellsPtr->InsertCellPoint(counter++);
						}
					}
				else
				  {
					// Make sure we have actually added data,
					// and we are not writing an empty block
					if(lastLineEmpty == false)
						{
						lastLineEmpty = true;
						// Create an object to store your tri in
						vtkPolyData* tri = vtkPolyData::New();
						tri->SetPoints(myPointsPtr);
						tri->SetPolys(myCellsPtr);

						// Add the tri to the MultiBlockDataSet
						output->SetBlock(blockCounter, tri);

						vtkSBuffer sStream;
						sStream << "Surface ";
						sStream << blockCounter;
						// Change the name of the block using "meta-data"
						output->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), sStream.str().c_str());

						tri->Delete();

						// Move to the next block
						blockCounter++;

						// Reset the counter to 0, so that the next tri will
						// number it's InsertCellPoint properly
						counter = 0;

						// Reset point and cell information for next block
						myPointsPtr = vtkPoints::New();
						myCellsPtr = vtkCellArray::New();
						}
				  }
				}
			}
    myfile.close();
    delete[] cords;
		}

	// Create an object to store your tri in
	vtkPolyData* tri = vtkPolyData::New();
	tri->SetPoints(myPointsPtr);
	tri->SetPolys(myCellsPtr);

	// Add the tri to the MultiBlockDataSet
	output->SetBlock(blockCounter, tri);

	// Create the name for the block
	vtkSBuffer sStream;
	sStream << "Surface ";
	sStream << blockCounter;
	// Change the name of the block using "meta-data"
	output->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), sStream.str().c_str());

	// Keep things tidy
	tri->Delete();
	myPointsPtr->Delete();
	myCellsPtr->Delete();
  return 1;
}

double* vtkTriReader::parseString(vtkStdString line){
  vtkStdString::size_type loc;
  vtkStdString substr = line;
  double *values = new double[TRIAG_POINTS];
  int x = 0;
  do{
   loc = substr.find(" ", 1);
   fromString(substr.substr(0,loc),values[x++]);
   if (loc != vtkStdString::npos){
    substr = substr.substr(loc);
   }
  }while(loc != vtkStdString::npos);
  return values;
}

void vtkTriReader::fromString(const vtkStdString &s, double &result){
  std::istringstream stream(s);
  stream >> result;
}
