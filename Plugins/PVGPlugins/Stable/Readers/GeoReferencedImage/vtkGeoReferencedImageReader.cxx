// .NAME vtkGeoReferencedImageReader.cxx
// By: Matthew Livingstone
// Uses libtiff and geotiff libraries from http://trac.osgeo.org/geotiff/
// Reads standard .tiff, .jpg and .png files.
// Also reads GeoTIFF (with both embedded info and external text files).
// Supports .tfw files for any .tiff, .jpg or .png

#include "vtkGeoReferencedImageReader.h"
#include "vtkPolyData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkPlaneSource.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkToolkits.h"
#include "vtkTIFFReader.h"
#include <vtksys/ios/sstream>

// GeoTIFF specific includes
#include "xtiffio.h"
#include "geotiffio.h"
#include "tiffio.h"

vtkCxxRevisionMacro(vtkGeoReferencedImageReader, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkGeoReferencedImageReader);

// Constructor
vtkGeoReferencedImageReader::vtkGeoReferencedImageReader()
{
	this->FileName = 0;
	this->SetNumberOfOutputPorts(1);
	this->SetNumberOfInputPorts(0);

	this->FileExt = "";
  this->FilePath = "";
  this->FileShortName = "";
	this->NorthernHemisphere = 0;
	this->TFWFound = false;

};

// --------------------------------------
// Destructor
vtkGeoReferencedImageReader::~vtkGeoReferencedImageReader()
{
	this->SetFileName(0);
}

// --------------------------------------
void vtkGeoReferencedImageReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

// --------------------------------------
int vtkGeoReferencedImageReader::RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	this->FindFileInfo();

	// Generate the name of the TFW we are checking for
	vtkStdString tfwName = "";
	tfwName.append(this->FilePath);
	tfwName.append(this->FileShortName);
	tfwName.append(".tfw");

	// Make sure we have a file to read.
	if(!tfwName)  {
		vtkErrorMacro("TFW file does not exist.");
		return 0;
	}
	if(tfwName.length()<=0)  {
		vtkErrorMacro("TFW file does not exist.");
		return 0;
	}

	// Test TFW existence
	ifstream file;
	file.open(tfwName, ios::in);

	if(file)
	{
		this->TFWFound = true;
	}

	return 1;
}

// --------------------------------------
int vtkGeoReferencedImageReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	this->Flip = false;

	// Make sure we have a file to read.
	if(!this->FileName)
	{
		vtkErrorMacro("A FileName must be specified.");
		return 0;
	}
	if(strlen(this->FileName)==0)
	{
		vtkErrorMacro("A NULL FileName.");
		return 0;
	}

  vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );
	 
	vtkPolyData *output = vtkPolyData::SafeDownCast (
                         outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	// TIFF-level descriptor
	TIFF *tif=(TIFF*)0;
	// GeoKey-level descriptor
	GTIF *gtif=(GTIF*)0;

	int width, length;
	double  adfCoeff[6], x, y;
	// all key-codes are of this type
	geocode_t model;
	// Used only with TFW files
	bool YInversion = false;

	// Initialize coords
	adfCoeff[0] = adfCoeff[1] = adfCoeff[2] = adfCoeff[3] = adfCoeff[4] = adfCoeff[5] = 0.0;

	if(this->FileExt == ".tif" || this->FileExt == ".tiff" || this->FileExt == ".TIF" ||
	this->FileExt == ".TIFF")
	{
		// Open TIFF descriptor to read GeoTIFF tags
		tif = XTIFFOpen(this->FileName,"r");  
		if(!tif)
		{
			vtkErrorMacro("TIFF file failed to open.");
			return 0;
		}

		// Get some TIFF info on this image
		TIFFGetField(tif,TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(tif,TIFFTAG_IMAGELENGTH, &length);
			
		if( !this->TFWFound )
		{	
			// No TFW, read GeoTIFF
			// Open GTIF Key parser; keys will be read at this time.
			gtif = GTIFNew(tif);
			if (!gtif)
			{
				vtkDebugMacro("GeoTIFF file failed to open. Not a valid GeoTIFF file.");
				return 0;
			}

			if (GTIFKeyGet(gtif, GTModelTypeGeoKey, &model, 0, 1))
				{
				// Compute the coefficients.
				x = 0.5;
				y = 0.5;
				if( GTIFImageToPCS( gtif, &x, &y ) )
					{
					// X Reference Point
					adfCoeff[4] = x;
					// Y Reference Point
					adfCoeff[5] = y;

					x = 1.5;
					y = 1.5;
					if(GTIFImageToPCS( gtif, &x, &y ) )
						{
						// X Scale
						adfCoeff[0] = x - adfCoeff[4];
						// Y Rotation
						adfCoeff[1] = y - adfCoeff[5];

						x = 0.5;
						y = 1.5;
						if(GTIFImageToPCS( gtif, &x, &y ) )
							{
							// X Rotation
							adfCoeff[2] = x - adfCoeff[4];
							// Y Scale
							adfCoeff[3] = y - adfCoeff[5];
							}
						}
					}
				}
			else
				{
				//TANEMA  bug fix some compression types for some reason render
				//upside down so this is to switch it back and to get normal tiffs
				//rendering the return statements from GTIFImagetoPCS made the reader error
				vtkTIFFReader *tifread = vtkTIFFReader::New();
				int compType = tifread->CompressionType(this->FileName);
				if(compType == COMPRESSION_LZW ||
					compType == COMPRESSION_PACKBITS ||
					compType == COMPRESSION_NONE)
					{
					this->Flip = true;
					}
				tifread->Delete();
				}

			// get rid of the key parser
			GTIFFree(gtif);
		}
		else
		{
			// A TFW exists, read it
			this->ParseTFW( adfCoeff, YInversion );

			if (!this->NorthernHemisphere)
			{					
				//okay here is a bug fix for UTM coords in the southern hemisphere
				//for a proper explantion go fuck your self ( http://en.wikipedia.org/wiki/Universal_Transverse_Mercator_coordinate_system )

				int offset = 10000000;
				adfCoeff[5] = offset - adfCoeff[5];
			}
		}

		// close the TIFF file descriptor
		XTIFFClose(tif);
	}
	else
	{
		if (this->FileExt == ".jpg" || this->FileExt == ".jpeg" || this->FileExt == ".JPG" ||
			this->FileExt == ".JPEG")
		{
			// Open jpg for reading
			vtkJPEGReader *jpg = vtkJPEGReader::New();
			jpg->SetFileName(this->FileName);
			jpg->Update();

			// Grab dimensions
			int *dimensions = new int[6];
			jpg->GetDataExtent(dimensions);
			// Calculate dimensions
			width = dimensions[1] - dimensions[0] + 1;
			length = dimensions[3] - dimensions[2] + 1;
			// Tidy up
			jpg->Delete();
		}
		else if(this->FileExt == ".png" || this->FileExt == ".PNG")
		{
			// Open jpg for reading
			vtkPNGReader *jpg = vtkPNGReader::New();
			jpg->SetFileName(this->FileName);
			jpg->Update();

			// Grab dimensions
			int *dimensions = new int[6];
			jpg->GetDataExtent(dimensions);
			// Calculate dimensions
			width = dimensions[1] - dimensions[0] + 1;
			length = dimensions[3] - dimensions[2] + 1;
			// Tidy up
			jpg->Delete();
		}

		// Parse the TFW is the jpg or png has one
		if( this->TFWFound )
		{
			this->ParseTFW( adfCoeff, YInversion );
		}
	}

	// Create our plane that the texture will be applied to later on
	vtkPlaneSource *plane = vtkPlaneSource::New();
	if (YInversion)
	{
		// Y value is inverted, adjust plane points
		double lowerYCoord = (length-1) * adfCoeff[3];
		plane->SetOrigin(adfCoeff[4], adfCoeff[5]-lowerYCoord, 0);
		plane->SetPoint1(adfCoeff[4], adfCoeff[5]+length-lowerYCoord, 0);
		plane->SetPoint2(adfCoeff[4]+width, adfCoeff[5]-lowerYCoord, 0);
	}
	else
	{
		plane->SetOrigin(adfCoeff[4],adfCoeff[5],0);
		plane->SetPoint1(adfCoeff[4], adfCoeff[5]+length, 0);
		plane->SetPoint2(adfCoeff[4]+width, adfCoeff[5], 0);
	}

	plane->Update();
	output->ShallowCopy(plane->GetOutput());
	plane->Delete();

	// Creating new texture coords, as the default ones are wrong.
	double coords[2], coordTemp;
	// NOTE: Do not delete texture coordinates array
	//			 If done, Paraview will crash when iterating of the active arrays
	vtkFloatArray *newTCoords;
	newTCoords = vtkFloatArray::SafeDownCast( output->GetPointData()->GetArray("TextureCoordinates") );

	for( int ptNum = 0; ptNum < newTCoords->GetNumberOfTuples(); ptNum++)
	{
		newTCoords->GetTuple(ptNum, coords);
		// Swap X and Y coords, as the image is rotated 90 degrees
		//if needed fliping depending on compression of tiff then make x = -x inverting the image
		coordTemp = coords[0];
		coords[0] = coords[1];
		if(this->Flip)
			{
			coords[1] = -coordTemp;
			}
		else
			{
			coords[1] = coordTemp;
			}
		newTCoords->InsertTuple(ptNum, coords);
	}

	newTCoords->SetName("TextureCoordinates");
	output->GetPointData()->SetTCoords(newTCoords);

	return 1;
}

void vtkGeoReferencedImageReader::FindFileInfo()
{
	vtkStdString fName = this->FileName;

	//Find position final '\' that occurs just before the file name
	int slashPosition = fName.find_last_of('\\');
	//sometimes path contains the other slash ('/')
	if(slashPosition == -1)
		slashPosition = fName.find_last_of('/');
	//Add one to slashPosition so that the slash is not included
	slashPosition = slashPosition+1;

	//Find position of '.' that occurs before the file extension
	int dotPosition = fName.find_last_of('.');
	//Save the file extention into a stdString, so that the length that be found
	this->FileExt = fName.substr(dotPosition);
	
	
	//Save the file name AND the file extention into a stdString, so that the length that be found
	vtkStdString nameWithExt = fName.substr(slashPosition);
	int nameExtLen = nameWithExt.length();

	//Determine the length of the word, so that it can be taken from fName, which has the file name and extension
	int finalNameLength = nameExtLen - this->FileExt.length();	
	
	this->FileShortName = fName.substr(slashPosition, finalNameLength);
	this->FilePath = fName.substr(0, slashPosition);
}

void vtkGeoReferencedImageReader::ParseTFW(double adfCoeff[6], bool &YInversion)
{
	//vtkStdString name = 
	vtkStdString tfwName = "";
	tfwName.append(this->FilePath);
	tfwName.append(this->FileShortName);
	// Add the extension
	tfwName.append(".tfw");

	ifstream file;
	file.open(tfwName, ios::in);

	vtkStdString strHolder;

	// Used to push the string value from the file into a double
	// XScale
	std::getline(file, strHolder);
	std::stringstream sstream;
	sstream << strHolder;
	sstream >> adfCoeff[0];
	sstream.clear();
	// YRot
	std::getline(file, strHolder);
	sstream << strHolder;
	sstream >> adfCoeff[1];
	sstream.clear();
	// XRot
	std::getline(file, strHolder);
	sstream << strHolder;
	sstream >> adfCoeff[2];
	sstream.clear();
	// YScale
	std::getline(file, strHolder);
	sstream << strHolder;
	sstream >> adfCoeff[3];
	sstream.clear();
	if(adfCoeff[3] < 0.0)
	{
		adfCoeff[3] *= -1.0;
		YInversion = true;
	}
	// XRef
	std::getline(file, strHolder);
	sstream << strHolder;
	sstream >> adfCoeff[4];
	sstream.clear();
	// YRef
	std::getline(file, strHolder);
	sstream << strHolder;
	sstream >> adfCoeff[5];
	sstream.clear();
}