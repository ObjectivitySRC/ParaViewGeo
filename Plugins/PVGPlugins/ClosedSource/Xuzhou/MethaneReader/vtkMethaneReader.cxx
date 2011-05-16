// .NAME vtkMethaneReader.cxx
// Read methane data (including coverted msdate, methane level and temperature) from SQLite database

#include "vtkMethaneReader.h"
#include "vtkExecutive.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkStdString.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkCellArray.h"
#include <vtksys/ios/sstream>

vtkCxxRevisionMacro (vtkMethaneReader, "$Revison: 1.0 $");
vtkStandardNewMacro (vtkMethaneReader);

// Forward declaration for using string stream
typedef vtkstd::stringstream vtkSBufferBase;
class vtkSBuffer: public vtkSBufferBase
{};

// Constructor
vtkMethaneReader::vtkMethaneReader()
{
	this->SetNumberOfOutputPorts(1);
	this->FileName = NULL;
	this->SetNumberOfInputPorts(0);
}

// Destructor
vtkMethaneReader::~vtkMethaneReader()
{
	this->SetFileName(0);
}

// Printing
void vtkMethaneReader::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	os << indent << "FileName: "
		<< (this->FileName ? this->FileName : "(none)") << "\n";
}

// Checking input file
int vtkMethaneReader::RequestData(vtkInformation* request,
								  vtkInformationVector** inputVector,
								  vtkInformationVector* outputVector)
{
	// make sure of there is a file
	if (!this->FileName)
	{
		vtkErrorMacro ("A FileName must be specified.");
		return 0;
	}
	if (strlen (this->FileName)==0)
	{
		vtkErrorMacro ("A NULL FileName. ");
		return 0;
	}

	// make sure of file existence
	// use ifstream to remove any warnings about fopen
	ifstream fin;
	fin.open (this->FileName, ios::in);
	if (fin.fail())
	{
		vtkErrorMacro ("Error in file path: " << this->FileName);
		fin.close();
		return 0;
	}
	fin.close();

	// create a stream to hold SQL information
	vtkSBuffer sStream;
	vtkStdString urlString;
	// The string stream must be passed into a StdString, which clens up the garbage at the end of the line

	sStream << "sqlite://";
	sStream << this->FileName;
	urlString = sStream.str(); // place stream into vtkStdString
	const char* URL = urlString; // convert to char*, so SQL can use it

	// Initialize SQlite connection and cursor
	this->db = vtkSQLiteDatabase::SafeDownCast (vtkSQLiteDatabase::CreateFromURL(URL));
	bool status = this->db->Open();
	if (!status)
	{
		vtkErrorMacro("Cannot open database.");
		return 0;
	}
	this->query = this->db->GetQueryInstance();
	
	this->ProcessMethane();

	// Close the database after query is inactive, otherwise error code 5 = SQLITE_BUSY
	this->query->SetQuery("");
	this->query->Execute();

	// Close the database connection
	this->db->Close();

	return 1;
}

void vtkMethaneReader::ProcessMethane()
{
	vtkMultiBlockDataSet *output;
	output = (vtkMultiBlockDataSet*) this->GetExecutive()->GetOutputData(0);
	int blockCounter = 0;

	// Create a new object that will be added to the MultiBlockDataSet
	vtkPolyData *obj = vtkPolyData::New();

	vtkMultiBlockDataSet *appender = vtkMultiBlockDataSet::New();
	this->CreateBlock(obj, appender); // fill obj with desired object information

	// Add object to the output
	output->SetBlock(blockCounter, obj);

	// Change the name of the block using "meta-data"
	output->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), "Methane Analysis");

	// Delete the object as it is now in the output
	obj->Delete();
	appender->Delete();
	blockCounter++;
}

void vtkMethaneReader::CreateBlock(vtkPolyData *obj, vtkMultiBlockDataSet *appender)
{
	int numProperties = 0;
	// array to hold the names for all properties
	// note: a "normals" property will be added for lines and points later on
	// it is not included in this count of properties
	vtkStringArray *names = this->GetPropertyNames(&numProperties);

	// actual property information, indexed by the property name
	vtkDoubleArray **properties = this->CreatePropertyArrays(names, numProperties);

	vtkIntArray *indices = this->GetQueryIndices();

	// get query for particular table
	vtkSBuffer buffer;
	this->GetQuery(&buffer);
	this->query->SetQuery(buffer.str().c_str());

	// Check complete query
	if (!this->query->Execute())
	{
		vtkErrorMacro("No data for table entry!");
		return;
	}

	vtkIdType* nodesa = NULL;

	vtkSmartPointer<vtkPoints> myPointsPtr = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> myCellsPtr = vtkSmartPointer<vtkCellArray>::New();

	while (this->query->NextRow())
	{
		// points that represent the methane level
		this->ProcessMethaneGeology(nodesa, myPointsPtr, myCellsPtr, query);

		// Add the properties one of each row
		for (int i = 0; i < numProperties; i++)
		{
			properties[i]->InsertNextValue(this->query->DataValue(indices->GetValue(i)).ToDouble());
		}

		// free up nodesa memory
		delete[] nodesa;
	}

	// Checking for actual points before setting them
	if (myPointsPtr->GetNumberOfPoints() < 1)
	{
		vtkErrorMacro ("No points to display!");
		return;
	}

	obj->SetPoints(myPointsPtr);
	obj->SetVerts(myCellsPtr);

	// property arrays
	for (int i = 0; i < numProperties; i++) 
	{
		obj->GetPointData()->AddArray(properties[i]);
	}

	// "Normals" property must be created manually, as there is no way to create normals from points
	// an array will hold a "Normals" value for each point
	vtkDoubleArray *norms = vtkDoubleArray::New();

	// Paraview will look for a property named "Normals"
	norms->SetName("Normals");

	// Fill the array with values of "1.0"
	for (int index = 0; index < myPointsPtr->GetNumberOfPoints(); index++)
	{
		norms->InsertNextValue(1.0);
	}
	obj->GetPointData()->AddArray(norms);
}

void vtkMethaneReader::ProcessMethaneGeology(vtkIdType *nodesa, vtkPoints *pointsPtr, vtkCellArray *cellsPtr, vtkSQLQuery *query)
{
	nodesa = new vtkIdType[1];
	double x = 0.0, y = 0.0, z = 0.0;
	x = query->DataValue(1).ToFloat();
	y = query->DataValue(2).ToFloat();
	z = query->DataValue(3).ToFloat();
	nodesa[0] = pointsPtr->InsertNextPoint(x,y,z);
	cellsPtr->InsertNextCell(1, nodesa);
}

void vtkMethaneReader::SetPropertyName(const char* field, vtkStringArray *names)
{
	vtkSBuffer propName;
	propName << field;
	names->InsertNextValue(propName.str().c_str());
}

vtkStringArray* vtkMethaneReader::GetPropertyNames(int *numProperties)
{
	vtkStringArray *names = vtkStringArray::New();
	this->SetPropertyName("msdate", names);
	this->SetPropertyName("methane", names);
	this->SetPropertyName("temperature", names);
	*numProperties = 3;

	return names;
}

vtkDoubleArray** vtkMethaneReader::CreatePropertyArrays(vtkStringArray *names, int numProperties)
{
	vtkDoubleArray **d = new vtkDoubleArray*[numProperties];
	for (int i = 0; i < numProperties; i++)
	{
		d[i] = vtkDoubleArray::New();
		d[i]->SetName(names->GetValue(i));
	}
	return d;
}

vtkIntArray* vtkMethaneReader::GetQueryIndices()
{
	vtkIntArray *indices = vtkIntArray::New();
	indices->InsertNextValue(4);
	indices->InsertNextValue(5);
	indices->InsertNextValue(6);
	return indices;
}

void vtkMethaneReader::GetQuery(vtkSBuffer *buffer)
{
	*buffer << "select sensor_id, x, y, z, msdate, methane, temperature from methane";
}

