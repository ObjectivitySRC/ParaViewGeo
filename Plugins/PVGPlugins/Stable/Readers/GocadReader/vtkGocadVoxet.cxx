/*=========================================================================

  Program:   ParaViewGeo
  Module:    $RCSfile: vtkGocadVoxet.cxx,v $

  Copyright (c) Matthew Livingstone
  All rights reserved.  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkGocadVoxet.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPoints.h"
#include "vtkStructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStringArray.h"
#include "vtkDoubleArray.h"
#include "vtkVoxel.h"

//BTX
class vtkInternalMap;
class vtkInternalProps;
//ETX

#define characterSkipSix 6
#define characterSkipTen 10
#define characterSkipFifteen 15
#define characterSkipNineteen 19
#define maxProperties 50


vtkCxxRevisionMacro(vtkGocadVoxet, "$Revision: 0.3 $");
vtkStandardNewMacro(vtkGocadVoxet);

// --------------------------------------
vtkGocadVoxet::vtkGocadVoxet()
{    
	this->DataType = VTK_STRUCTURED_GRID;	
}

// --------------------------------------
vtkGocadVoxet::~vtkGocadVoxet()
{
}

// --------------------------------------
int vtkGocadVoxet::RequestData(vtkInformation* request,
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

	// Read all the properties from the .vo
	// Will read them as ASCII or Binary as necessary
	this->ReadVoxetProperties(propertyNames, propertyFiles, asciiFiles);

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

void vtkGocadVoxet::ApplyBinaryProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
																				vtkStringArray* asciiFiles, int propertyCount, int* propertySize,
																				int* propertyNoDataValues)
{

	// NOTE: I am sorry for the old style used.
	//			Following the original style was the only way I was able to get
	//			the binary properties working properly
	char inbuff[20], revord[6];
	float* fptr = (float*)&revord[0];
	float fprop;
	int i, j, readcnt, iprop = 0;
	char* icp = (char*)&iprop;
	vtkStdString binaryWithPath = "";
	FILE* bfp;
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
		
		while ( ( readcnt = fread(inbuff,1,16,bfp) ) !=0 )
		{
			// propertySize is ESIZE - 3, so anything != 1 is < 4
			if ( propertySize[pi] != 1 )
			{
				// With smaller property size, we will use every byte as a property value
        for ( i = 0; i < readcnt; i++ )
        {
					// See variable delaration to see how these variables are related
          *icp = inbuff[i];
          fprop = float(iprop);
          if (fprop)
					{
						// Note the cast to double, as method only takes double
						this->InsertCellPropertyValue(pi, (double)fprop);
					}
					else
					{
						// If there is no value, replace it with the NO_DATA_VALUE
						this->InsertCellPropertyValue(pi, propertyNoDataValues[pi]);
					}
				}
			}
			else
			{
				// For properties of size 4, we read every 4 bytes as a property value
				for ( i = 0; i < readcnt; i+=4 )
				{
					// See variable delaration to see how these variables are related
					for ( j = 0; j < 4; j++ )
					{
						revord[j] = inbuff[i+3-j];
					}
					if (fptr)
					{
						// Note the cast to double, as method only takes double
						this->InsertCellPropertyValue(pi, (double)*fptr);
					}
					else
					{
						// If there is no value, replace it with the NO_DATA_VALUE
						this->InsertCellPropertyValue(pi, propertyNoDataValues[pi]);
					}
				}
			}
		}		
	}
	// Done with binary file
	fclose(bfp);
}

void vtkGocadVoxet::ApplyASCIIProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
																				vtkStringArray* asciiFiles, int propertyCount, int* propertySize,
																				int* propertyNoDataValues)
{
	bool readFile = false;
	vtkStdString ASCIIWithPath = this->FilePath + asciiFiles->GetValue( asciiFiles->GetNumberOfValues()-1 );
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

	int pointCount = 0;
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
				this->InsertCellPropertyTuple(numProp, tuple);
				propCounter += propertySize[numProp];
			}
			else
			{
				// Add the property
				this->InsertCellPropertyValue(numProp, propLine[3+propCounter]);
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

void vtkGocadVoxet::ReadVoxetProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
																				vtkStringArray* asciiFiles)
{
	// Used when counting property tuples
	int propCounter = 0;

	// Is the property data in a separate ASCII file?
	bool ascii_file_flag = false;
	
	// Used to control file reading loops
	bool readFile = true;

	// Stores total number of properties for this object
	int propertyCount = 0;

	// Property ID & Size
	int propertyIDSizeTemp[2];
	// Size of each property
	int propertySize[maxProperties];
	// Hold NO_DATA_VALUE for each property
	int propertyNoDataValues[maxProperties];

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
			this->SetFileCellProperties( tempLine );
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
			propertyFiles->InsertValue( propertyCount-1, tempLine );
    }
		else if( this->Line->find("PROP_NO_DATA_VALUE ") != vtkstd::string::npos )
		{
			// Use substr starting at 10 as line looks something like:
			// PROP_NO_DATA_VALUE 1 -99999
			// So substr turns out as '1 -99999'
			vtkStdString tempLine = this->Line->substr( characterSkipNineteen, this->Line->length() );
			this->ParseLine( tempLine, 2, propertyIDSizeTemp );

			// propertyCounts counts from 1 up, and arrays are referenced 0 up
			// So propertyCount-1 must be used to reference the current property
			propertyNoDataValues[propertyCount-1] = propertyIDSizeTemp[1];
    }
		// Is the data file ASCII?
    else if ( this->Line->find("ASCII_DATA_FILE ") != vtkstd::string::npos )
		{
      ascii_file_flag = true;

			// Use find_first_of starting at 15 as line looks something like:
			// ASCII_DATA_FILE "C:/.../ImageDataTest1Small3"
			// So substr turns out as "C:/.../ImageDataTest1Small3"
			vtkStdString tempLine = this->Line->substr(
															this->Line->find_first_of(" ", characterSkipFifteen), this->Line->length()
															);

			this->ParseASCIIFileLine( tempLine, maxProperties, asciiFiles );
			// NOTE: the data file may not contain the path information, if so the path may have
			//			 to be taken from the file we are currently reading, will need to talk to Rob about it
    }
		
		if (this->Line->find("END") != vtkstd::string::npos)
		{
			readFile = false;
		}
		else {
			this->NextLine();
		}

  }

	if ( ascii_file_flag )
	{
		this->ApplyASCIIProperties(propertyNames, propertyFiles, asciiFiles, propertyCount, propertySize, propertyNoDataValues);
	}
	else
	{
		this->ApplyBinaryProperties(propertyNames, propertyFiles, asciiFiles, propertyCount, propertySize, propertyNoDataValues);
	}
}

void vtkGocadVoxet::CreateStructure(vtkStructuredGrid *output, vtkInformation *outInfo, vtkPoints *points)
{

	// Axis and grid data

	double AXIS_O[3], AXIS_U[3], AXIS_V[3], AXIS_W[3];
	double AXIS_N[3] = {1,1,1};
	int AXIS_MAX[3] = {1,1,1};

	// Read Axis data	
	int requiredProperties = 0;
	while( (requiredProperties < 6) &&!this->File->eof() )
	{
		if( this->Line->find("AXIS_O ") != vtkstd::string::npos )
			{			
			requiredProperties++;
			this->ParseAxis( AXIS_O );
			}
		else if( this->Line->find("AXIS_U ") != vtkstd::string::npos )
			{		
			requiredProperties++;
			this->ParseAxis( AXIS_U );
			}
		else if( this->Line->find("AXIS_V ") != vtkstd::string::npos )
			{	
			requiredProperties++;
			this->ParseAxis( AXIS_V );
			}
		else if( this->Line->find("AXIS_W ") != vtkstd::string::npos )
			{			
			requiredProperties++;
			this->ParseAxis( AXIS_W );
			}
		// Watch for AXIS_N
		else if( this->Line->find("AXIS_N ") != vtkstd::string::npos	)		
			{			
			requiredProperties++;
			this->ParseAxis( AXIS_N );
			}			
		else if( this->Line->find("AXIS_MAX ") != vtkstd::string::npos	)		
			{			
			requiredProperties++;
			this->ParseAxis( AXIS_MAX );
			}			
		this->NextLine();
	}

	if ( requiredProperties < 4 )
		{
		vtkErrorMacro(<<"File does not have all the axis information");
		return;
		}
	
	//we are done parsing the file for information on the structure of the grid
	//now we have to actually construct it
	double cellSize[3][3];
	double actualOrigin[3];	
	int dimensions[3];
	int extents[6];


	if ( AXIS_MAX[0] == 1 && AXIS_MAX[1] == 1 && AXIS_MAX[2] == 1 )
		{
		//u,v,w define the full box
		double zDim;

		//basically we have to figure out the cellsize for each component, since of rotation
		//is described by using all the values in each axis, and we need support for non axis aligned
		for (int i=0; i < 3; i++)
			{
			cellSize[0][i] = AXIS_U[i] / (AXIS_N[0]-1);
			cellSize[1][i] = AXIS_V[i] / (AXIS_N[1]-1);

			zDim = (AXIS_N[2]-1) == 0 ? 1 : (AXIS_N[2]-1);
			cellSize[2][i] = AXIS_W[i] / zDim;
			}		
		}
	else
		{
		//u,v,w define a single cell
		vtkWarningMacro(<<"Support for single cell defintion voxets is still in testing.");

		//basically we have to figure out the cellsize for each component, since of rotation
		//is described by using all the values in each axis, and we need support for non axis aligned
		for (int i=0; i < 3; i++)
			{
			cellSize[0][i] = AXIS_U[i];
			cellSize[1][i] = AXIS_V[i];
			cellSize[2][i] = AXIS_W[i];			
			}					
		}

	int extentPos =0;
	//set the extents and dimensions
	for ( int i=0; i < 3; ++i)
		{
		extentPos = i*2;
		actualOrigin[i] = AXIS_O[i] - cellSize[i][i]/2;
		dimensions[i] = AXIS_N[i] + 1;
		extents[ extentPos++ ] = 0;
		extents[ extentPos ] = AXIS_N[i];
		}

	//set the number of points
	points->SetNumberOfPoints( dimensions[0] * dimensions[1] * dimensions[2] );

	vtkIdType id=0;
	double pos[3];
	double rotationPosition[3][3];
	
	//we need to calculate out the rotation of each axis so we support rotated voxets
	for ( int k=0; k < dimensions[2]; k++ )
		{	
		//we have unrolled the loops for speed
		rotationPosition[2][0] = k * cellSize[2][0];
		rotationPosition[2][1] = k * cellSize[2][1];
		rotationPosition[2][2] = k * cellSize[2][2];
		for ( int j=0; j < dimensions[1]; j++ )
			{
			rotationPosition[1][0] = j * cellSize[1][0];
			rotationPosition[1][1] = j * cellSize[1][1];
			rotationPosition[1][2] = j * cellSize[1][2];
			for ( int i=0; i < dimensions[0]; i++ )
				{
				rotationPosition[0][0] = i * cellSize[0][0];
				rotationPosition[0][1] = i * cellSize[0][1];
				rotationPosition[0][2] = i * cellSize[0][2];

				//find the position of the point in the voxet
				//we look at each rotation index so we support non axis aligned voxets
				pos[0] = actualOrigin[0] + rotationPosition[0][0] +  rotationPosition[1][0] +  rotationPosition[2][0];
				pos[1] = actualOrigin[0] + rotationPosition[0][1] +  rotationPosition[1][1] +  rotationPosition[2][1];
				pos[2] = actualOrigin[0] + rotationPosition[0][2] +  rotationPosition[1][2] +  rotationPosition[2][2];
				points->InsertPoint(id++,pos);
				}
			}
		}	

	//set all the information for the output
	// Set the extents of the space
	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extents, 6);
	output->SetPoints( points );
  output->SetDimensions(dimensions);
}

// --------------------------------------
void vtkGocadVoxet::ParseAxis( double axis[3] )
{
	this->ParseLine( 
		// substr taken due to "AXIS_X" at the beginning of the axis line
		this->Line->substr( 6, this->Line->length() ),
		3, axis );
}

// --------------------------------------
void vtkGocadVoxet::ParseAxis( int axis[3] )
{
	this->ParseLine( 
		// substr taken due to "AXIS_X" at the beginning of the axis line
		this->Line->substr( characterSkipSix, this->Line->length() ),
		3, axis );
}
