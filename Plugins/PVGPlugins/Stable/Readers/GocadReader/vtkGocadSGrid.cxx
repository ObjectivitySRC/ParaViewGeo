/* .NAME vtkGocadSGrid.cxx
* Latest revision: March 4, 2009
*	  By: Matthew Livingstone
* ** I appologise in advance for the older style and "poor" naming convention
* ** when dealing with binary file reading.
* Read Gocad SGrid file (.sg) for single objects.
* Handles SGrids with binary properties as well as ASCII properties
*
* Does no Region processing.
*
* In all cases, the .sg file provides the description of the data files.
*
* If binary, points are read from a <objectname>__points@@ file (binary)
* and properties, each property is read fom the respective <objectname>__<property>@@ file
*
* If ASCII properties, points and properties are read from a <objectname>__ascii@@ file
*
* *See Bob for more details about below information
* A binary '~_flags@@'file is always created (~4 byte per point), which we don't use; and
* if there are Regions defined, there is a '~_regions_flags@@' file with flag bits indicating which
* points belong to which regions (if > 8 regions, 2 or more bytes are used per 'flag word', with
* byte reversal employed - 1st byte for region index 7-0, 2nd byte for 15-8, etc).
*/
#include "vtkGocadSGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkStringList.h"
#include "vtkStringArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#define characterSkipSix 6
#define characterSkipTen 10
#define characterSkipTwelve 12
#define characterSkipFifteen 15
#define characterSkipSixteen 16
#define maxProperties 50

vtkCxxRevisionMacro(vtkGocadSGrid, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkGocadSGrid);

// Constructor
vtkGocadSGrid::vtkGocadSGrid()
{
	this->DataType = VTK_STRUCTURED_GRID;
	this->propertyCells = false;
	this->isASCIIFile = false;
};

// --------------------------------------
// Destructor
vtkGocadSGrid::~vtkGocadSGrid()
{
}

// --------------------------------------
int vtkGocadSGrid::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkStructuredGrid *output = vtkStructuredGrid::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()) );
  
	vtkPoints *points = vtkPoints::New();

	this->CreateStructure( output, outInfo, points );

	// Property Names
	vtkStringArray* propertyNames = vtkStringArray::New();
	// Property Files (if they use external files to hold property data)
	vtkStringArray* propertyFiles = vtkStringArray::New();
	// ASCII Data File Names
	vtkStringArray* asciiFiles = vtkStringArray::New();

	// Will read them as ASCII or Binary as necessary
	this->ReadSGridProperties(propertyNames, propertyFiles, asciiFiles, output);

	int continueReading = true;
  
	// Housekeeping
	points->Delete();
	propertyNames->Delete();
	propertyFiles->Delete();
	asciiFiles->Delete();

	//call atomic requestData
	continueReading = vtkGocadAtomic::RequestData(request, inputVector, outputVector);
  return continueReading;
	
	return 1;
}

void vtkGocadSGrid::ApplyBinaryProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
																				vtkStringArray* asciiFiles, int propertyCount, int* propertySize,
																				vtkStructuredGrid *output)
{

	// NOTE: I am sorry for the old style used.
	//			Following the original style was the only way I was able to get
	//			the binary properties working properly
	char inbuff[20], revord[6];
	float* fptr = (float*)&revord[0];
	float fprop;
	int i, j, readcnt, iprop =0;
	char* icp = (char*)&iprop;
	FILE* bfp;
	vtkStdString binaryWithPath = "";
	// Iterate over all binary property files
	for (int pi=0; pi<propertyCount; pi++)
	{
		binaryWithPath = this->FilePath + propertyFiles->GetValue(pi);
		bfp = fopen(binaryWithPath,"rb");
		if(!bfp)
		{
			vtkErrorMacro("Error opening file " << binaryWithPath.c_str());
			return;
		}

		// Read data in 16 bytes at a time
		while ((readcnt=fread(inbuff,1,16,bfp))!=0)
		{
			// propertySize is ESIZE - 3, so anything != 1 is < 4
			if (propertySize[pi]!=1)
			{
				// With smaller property size, we will use every byte as a property value
        for (i=0; i<readcnt; i++)
        {
					// See variable delaration to see how these variables are related
          *icp = inbuff[i];
          fprop = float(iprop);
          if (fprop)
					{
						// Note the cast to double, as method only takes double
						if(this->propertyCells)
						{
							this->InsertCellPropertyValue(pi, (double)fprop);
						} else
						{
							this->InsertPropertyValue(pi, (double)fprop);
						}
					}
				}
			} else
			{
				// For properties of size 4, we read every 4 bytes as a property value
				for (i=0; i<readcnt; i+=4)
				{
					// See variable delaration to see how these variables are related
					for (j=0; j<4; j++)
					{
						revord[j] = inbuff[i+3-j];
					}
					if (fptr)
					{
						// Note the cast to double, as method only takes double
						if(this->propertyCells)
						{
							this->InsertCellPropertyValue(pi, (double)*fptr);
						} else
						{
							this->InsertPropertyValue(pi, (double)*fptr);
						}
					}
				}
			}
		}
	}
	// Done with binary file
	fclose(bfp);
}

void vtkGocadSGrid::ApplyASCIIProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
																				vtkStringArray* asciiFiles, int propertyCount, int* propertySize,
																				vtkStructuredGrid *output)
{
	bool readFile = false;
	vtkStdString ASCIIWithPath = this->FilePath + this->ASCIIFilePath;
  ifstream *asciiStream = new ifstream(ASCIIWithPath, ios::in);
	// Ensure the file exists and we can read it
  if(!asciiStream)
	{
    vtkErrorMacro("Error opening file " << asciiStream);
    return;
  }
	vtkStdString asciiLine;
  for (int hcnt=0; hcnt<4; hcnt++)
	{  // skip header lines
		getline( *asciiStream, asciiLine );
    if (asciiLine.length() == 0)
		{
      asciiStream->close();
      vtkErrorMacro("Error reading ASCII file " << ASCIIWithPath.c_str());
      return;
    }
  }
	// Enable file reading
	readFile = true;

	// Calculate how many doubles are expected on each line
	int size = 3;
	for ( int numProp = 0; numProp < propertyCount; numProp++)
	{
		if ( propertySize[numProp] > 0 && propertySize[numProp] < 10 )
		{
			size += propertySize[numProp];
		}
	}
	// Will hold all the double values in the line
  double *propLine = new double[size];

  while (readFile == true)
	{
		// Read all the double values from the line
		this->ParseLine( asciiLine, size, propLine );

		// Reset propCounter for each point
		int propCounter = 0;
		// Add the properties to each point
		for ( int numProp = 0; numProp < propertyCount; numProp++ )
		{
			if ( propertySize[numProp] > 1 )
			{ // Property has more than one double
				double *tuple = new double[propertySize[numProp]];
				for (int pi2=0; pi2<propertySize[numProp]; pi2++)
				{
					tuple[pi2] = propLine[3 + propCounter + pi2];
				}

				// Add the property
				this->InsertPropertyTuple(numProp, tuple);
				propCounter += propertySize[numProp];
			}
			else
			{
				// Add the property
				this->InsertPropertyValue(numProp, propLine[3+propCounter]);
				propCounter++;
			}
		}

		getline( *asciiStream, asciiLine );
		if (asciiLine.length() <= 0)
		{
			getline( *asciiStream, asciiLine );
			if (asciiLine.length() <= 0)
			{
				// end of file
				readFile = false;
			}
		}
  }
  asciiStream->close();
	// House keeping
	delete asciiStream;
	delete propLine;
}

void vtkGocadSGrid::ReadSGridProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
																				vtkStringArray* asciiFiles, vtkStructuredGrid *output)
{
	// Used when counting property tuples
	int propCounter = 0;

	// Used to control file reading loops
	bool readFile = true;

	// Stores total number of properties for this object
	int propertyCount = 0;

	// Property ID & Size
	int propertyIDSizeTemp[2];
	// Size of each property
	int propertySize[maxProperties];

	// Enabling file reading again
	readFile = true;

	// find 'PROPERTY', 'PROP_FILE', possibly 'ASCII_DATA_FILE' and 'END'
	while( readFile && !this->File->eof() )
	{
		// Gets name of property, manages propertyCount
		if( this->Line->find("PROPERTY ") != vtkstd::string::npos )
		{
			// Use find_first_of starting at 10 as line looks something like:
			// PROPERTY 1 "PNGImage"
			// So substr turns out as "PNGImage"
			vtkStdString tempLine = this->Line->substr( 
															this->Line->find_first_of("\"", characterSkipTen), this->Line->length()
															);
			// Cell property flag is ignored when using ASCII file
			if(this->propertyCells && !this->isASCIIFile)
			{
				this->SetFileCellProperties( tempLine );
			} else
			{
				this->SetFileProperties( tempLine );
			}
			propertyCount++;
    }
		// Get tuple size for property
    else if( this->Line->find("PROP_ESIZE ") != vtkstd::string::npos )
		{
			// Use substr starting at 10 as line looks something like:
			// PROP_ESIZE 1 6
			// So substr turns out as '1 6'
			vtkStdString tempLine = this->Line->substr( characterSkipTen, this->Line->length() );
			this->ParseLine( tempLine, 2, propertyIDSizeTemp );

			// Using propertyIDSizeTemp[1] because the first element in the array is the property ID, not the size
			propertySize[propertyCount-1] = propertyIDSizeTemp[1]-3;
    }
		// Get property data file (if there is one)
    else if( this->Line->find("PROP_FILE ") != vtkstd::string::npos )
		{
			// Use find_first_of starting at 10 as line looks something like:
			// PROP_FILE 1 ImageDataTest1
			// So substr turns out as ImageDataTest1
			vtkStdString tempLine = this->Line->substr(
															//this->Line->find_first_not_of(" ", characterSkipTen), this->Line->length()
															this->Line->find_first_not_of(" ", 
															this->Line->find_first_of(" ", characterSkipTen)), this->Line->length()
															);

			// propertyCounts counts from 1 up, and arrays are referenced 0 up
			// So propertyCount-1 must be used to reference the current property
			propertyFiles->InsertValue(propertyCount-1, tempLine	);
    }
		
		if (this->Line->find("END") != vtkstd::string::npos)
		{
			readFile = false;
		}
		else {
			this->NextLine();
		}

  }

	if ( this->isASCIIFile )
	{
		this->ApplyASCIIProperties(propertyNames, propertyFiles, asciiFiles, propertyCount, propertySize, output);
	} else
	{
		this->ApplyBinaryProperties(propertyNames, propertyFiles, asciiFiles, propertyCount, propertySize, output);
	}
}

void vtkGocadSGrid::CreateStructure(vtkStructuredGrid *output, vtkInformation *outInfo, vtkPoints *points)
{
	bool voxet2D = false;

	// Axis and grid data
	int VOXDim[3], VOXDimFull[6];

	vtkStringArray *pointsFile = vtkStringArray::New();

	// Read Axis data
	int axisCounter = 0;
	bool pointsFound = false;
	bool nSpacingFound = false;
	bool propTypeFound = false;
	bool asciiFound = false;
	while( ( (!pointsFound && !asciiFound) || !nSpacingFound || !propTypeFound)  && !this->File->eof() )
	{
		// Watch for AXIS_N
		if( this->Line->find("AXIS_N ") != vtkstd::string::npos	)		
		{
			nSpacingFound = true;
			this->ParseAxis( VOXDim );
		}
		if( this->Line->find("POINTS_FILE ") != vtkstd::string::npos )
		{
			pointsFound = true;
			pointsFile->InsertNextValue( this->Line->substr( characterSkipTwelve, this->Line->length() ) );
		}
		if( this->Line->find("ASCII_DATA_FILE ") != vtkstd::string::npos )
		{
			asciiFound = true;
			this->isASCIIFile = true;
			this->ASCIIFilePath = this->Line->substr( characterSkipSixteen, this->Line->length() );
		}
		if( this->Line->find("PROP_ALIGNMENT ") != vtkstd::string::npos )
		{
			propTypeFound = true;

			if( this->Line->substr( characterSkipFifteen, this->Line->length() ) == "CELLS" )
			{
				this->propertyCells = true;
			}
		}

		this->NextLine();
	}
	
	//we are done parsing the file for information on the structure of the grid
	//now we have to actually construct it

	//set up the extents
	if ( nSpacingFound )
		{
		// Only use axis 1, 3 and 5 for the dimensions
		VOXDimFull[1] = VOXDim[0] - 1;
		VOXDimFull[3] = VOXDim[1] - 1;
		VOXDimFull[5] = VOXDim[2] - 1;
		
		// Ensure unused axes are set to 0
		VOXDimFull[0] = VOXDimFull[2] = VOXDimFull[4] = 0;			
		}

	//lets construct the points
	// Construct grid
	double* XYZVal = new double[3];
	
	// Determine if we have to read the points from an ASCII or binary file
	if ( this->isASCIIFile )
	{ // ASCII reading

		bool readFile = true;
		vtkStdString ASCIIWithPath = this->FilePath + this->ASCIIFilePath;
		ifstream *asciiStream = new ifstream(ASCIIWithPath, ios::in);
		// Ensure the file exists and we can read it
		if(!asciiStream)
		{
			vtkErrorMacro("Error opening file " << asciiStream);
			return;
		}
		vtkStdString asciiLine;
		for (int hcnt=0; hcnt<4; hcnt++)
		{  // skip header lines
			getline( *asciiStream, asciiLine );
			if (asciiLine.length() == 0)
			{
				asciiStream->close();
				vtkErrorMacro("Error reading ASCII file " << ASCIIWithPath.c_str());
				return;
			}
		}

		while (readFile == true)
		{
			// Read XYZ Values
			this->ParseLine( asciiLine, 3, XYZVal );

			// Add point
			points->InsertNextPoint(XYZVal);

			getline( *asciiStream, asciiLine );
			if (asciiLine.length() <= 0)
			{
				getline( *asciiStream, asciiLine );
				if (asciiLine.length() <= 0)
				{
					// end of file
					readFile = false;
				}
			}
		}
		asciiStream->close();
		// House keeping
		delete asciiStream;

	} else
	{ // Binary
		// Points are in a binary file, so we will read them just like we read properties
		char inbuff[20], revord[6];
		float* fptr = (float*)&revord[0];
		int i, j, readcnt, iprop, xyzCount = 0;
		char* icp = (char*)&iprop;
		FILE* bfp;
		vtkStdString binaryWithPath = "";

		binaryWithPath = this->FilePath + pointsFile->GetValue(0);
		bfp = fopen(binaryWithPath,"rb");
		if(!bfp)
		{
			vtkErrorMacro("Error opening file " << binaryWithPath.c_str());
			return;
		}
		// Read data in 16 bytes at a time
		while ((readcnt=fread(inbuff,1,12,bfp))!=0)
		{
			xyzCount = 0;
			// For properties of size 4, we read every 4 bytes as a property value
			for (i=0; i<readcnt; i+=4)
			{
				// See variable delaration to see how these variables are related
				for (j=0; j<4; j++)
				{
					revord[j] = inbuff[i+3-j];
				}
				if (fptr)
				{
					// Note the cast to double, as method only takes double
					XYZVal[xyzCount] = (double)*fptr;
					xyzCount++;
				}
			}
			points->InsertNextPoint(XYZVal);
		}
		// Done with binary file
		fclose(bfp);
	}
	pointsFile->Delete();

	delete XYZVal;
	// -- Grid constructed

	
	//set all the information for the output
	// Set the extents of the space
	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), VOXDimFull, 6);
	output->SetPoints( points );
  output->SetDimensions(VOXDim[0],VOXDim[1],VOXDim[2]);

}

// --------------------------------------
void vtkGocadSGrid::ParseAxis( double axis[3] )
{
	this->ParseLine( 
		// substr taken due to "AXIS_X" at the beginning of the axis line
		this->Line->substr( 6, this->Line->length() ),
		3, axis );
}

// --------------------------------------
void vtkGocadSGrid::ParseAxis( int axis[3] )
{
	this->ParseLine( 
		// substr taken due to "AXIS_X" at the beginning of the axis line
		this->Line->substr( characterSkipSix, this->Line->length() ),
		3, axis );
}