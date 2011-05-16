/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkWriter.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

 =======================================================================
 "vtkSGeMSWriter"
 - Written and Expanded by John Mavity and Bob Anderson
 - July, 2008 A.D.

 This program enables Paraview to write two ["Cgrid","Point_set"]  
 of the three  [reduced grid, cartesian grid, and point set] SGeMS* file formats.

   It also reads in vector properties as well as scalar properties; i.e. each cell
/point has multiple properties, and each property value for the cell is stored as a
tuple; a 1-element tuple if the property is a scalar (eg. property "Is blue") or a
multi-element tuple if the property is a vector (eg. property "xyz/pointCoordinates"),
  and now the SGeMS writer can write both paraviewScalar properties and paraviewVector
properties.

   It is also expanded to interpret solid, non-rectilinear polydata objects 
(spheres etc.) as grid models and save them in a format SGeMS can process.
 
 NOTE: For this interpretation, the original object in paraview must be of 
type "vtkImageData"; this object must be then be translated to cell data 
via the "PointDataToCellData" filter.

*[Stanford Geostatistical Earth  Modeling Software] 
=========================================================================*/
#include "vtkSGeMSWriter.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"
#include "vtkAlgorithmOutput.h"
#include <vtkPointDataToCellData.h>
#include <sstream>
#include <cmath>

vtkCxxRevisionMacro(vtkSGeMSWriter, "$Revision: 1.27 $");
vtkStandardNewMacro(vtkSGeMSWriter);

/***********************************/
vtkSGeMSWriter::vtkSGeMSWriter()
{
  this->PointData = NULL;
  this->PointCoords = NULL;
	this->inputPS = NULL;
	this->inputCG = NULL;
}

/***********************************/
vtkSGeMSWriter::~vtkSGeMSWriter()
{
  this->PointData = NULL;
  this->PointCoords = NULL;
}

/***********************************/
/*
"this->FileName" returns the absolute path of a file, including its
extension.
"this->FindFileName()" returns only the filename, excluding both the
path and the extension.
*/
vtkStdString vtkSGeMSWriter::FindFileName()
{
  vtkStdString fName = this->FileName;

  //Find position final '\' that occurs just before the file name
  int slashPosition = fName.find_last_of('\\');

	//Regardless of the specific synax 
	//(Windows uses backslash, paraview uses frontslash).
	if(slashPosition = -1)
	{
		slashPosition = fName.find_last_of('/');
	}

  //Add one to slashPosition so that the '\' is not included
  slashPosition = slashPosition+1;

  //Find position of '.' that occurs before the file extension
  int dotPosition = fName.find_last_of('.');

  //Save the file extention into a stdString, so that the length that be found
  vtkStdString extension = fName.substr(dotPosition);
  int extLen = extension.length();

  //Save the file name AND the file extention into a stdString, so that the length that be found
  vtkStdString nameWithExt = fName.substr(slashPosition);
  int nameExtLen = nameWithExt.length();

  //Determine the length of the word, so that it can be taken from fName, which has the file name and extension
  int finalNameLength = nameExtLen - extLen;

  //Pull the file name out of the full path (fName)
  vtkStdString newName = fName.substr(slashPosition, finalNameLength);

  return newName;
}


/***********************************/
void vtkSGeMSWriter::WriteData()
{
	// If the data can be converted to(ie is stored as) PolyData, is pointset.
	if (this->inputPS!=NULL)
	{
		this->WritePointSet(inputPS);
	}
	// If the data can be converted to(ie is stored as)ImageData, is cgrid.
	else if(this->inputCG!=NULL)
	{
		this->WriteCgrid(inputCG);
	}
	// Else, it is not currently handled.
	return;
}
/***********************************/



/***********************************/

int vtkSGeMSWriter::FillInputPortInformation(int port, vtkInformation *info)
{
	// Hack;  first pass fails to aquire input (sets input to null);
	// Catch that, wait for second pass.
	vtkDataObject* obp = this->GetInput();
	if (obp==NULL)
		return 0;

	// Check if data is vtkPolyData; set Algorithm appropriately.
	this->inputPS = vtkPolyData::SafeDownCast(obp);
  if(inputPS != NULL)
	{
		info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
		return 1;
	}
	
	// Check if data is vtkImageData; set Algorithm appropriately.
	this->inputCG = vtkImageData::SafeDownCast(obp);
	if(inputCG != NULL)
	{
		info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
		return 1;
	}

	// Let the user know if the format is unknown/unhandled.
	else
	{
		vtkErrorMacro("Format not currently handled; cannot continue.");
		return 0;
	}
}

/***********************************/

/***********************************/
//Called with this->PrintSelf(*fp, indent)
/***********************************/
void vtkSGeMSWriter::PrintSelf(ostream& fp, vtkIndent indent)
{
  this->Superclass::PrintSelf(fp,indent);

	fp << endl << " -- Local Variables --" << endl;
	if(this->numPoints != 0)
	{
		fp << indent << "Output format:  SGeMS \"Point_set\" format. " << endl;
		fp << indent << "Number of points: " << this->numPoints << endl;
		fp << indent << "Number of Props: " << this->numProps << endl;
	}
	if(this->numCells != 0)
	{
		fp << indent << "Output format:  SGeMS \"Cgrid\" format. " << endl;
		fp << indent << "Number of cells: " << this->numCells << endl;
		fp << indent << "Number of Props: " << this->numProps << endl << endl;

		fp << indent << "Cell, x dimension: " << this->cell_xdim << endl;
		fp << indent << "Cell, y dimension: " << this->cell_xdim << endl;
		fp << indent << "Cell, z dimension: " << this->cell_xdim << endl;

		fp << indent << "Grid, x dimension: " << this->grid_xdim << endl;
		fp << indent << "Grid, y dimension: " << this->grid_ydim<< endl;
		fp << indent << "Grid, z dimension: " << this->grid_zdim << endl;

	}
	

}
/***********************************/


void vtkSGeMSWriter::WritePointSet(vtkPolyData* inputPS)
{
		this->PointData = inputPS->GetPointData();
		this->PointCoords = inputPS->GetPoints()->GetData();

		if(inputPS->GetVerts()->GetNumberOfCells() == 0)
		{
			vtkErrorMacro("WritePointSet: PolyData->GetVerts->GetNumberOfCells = 0;  cannot proceed.");
			return;
		}

		// Open file.
		FILE* fp=fopen(this->FileName,"wb");

		// Trouble proofing.
		if(fp==NULL)
		{
			vtkErrorMacro("Unable to open output file for writing.");
			return;
		}
  
		// Write a header with a "magic number" and the grid type
		this->WriteInt(fp,0xB211175D);

		// Write the class name spacer.
		std::string className = "Point_set";
		int spacer = strlen( className.c_str() ) + 1;
		this->WriteInt(fp, spacer);

		// Write the class name.
		fwrite( className.c_str(), spacer, 1, fp);
	  
		// write the object name spacer.
		// (let objectName = the userdefined savefile name)
		std::string objectName = this->FindFileName();
		spacer = strlen( objectName.c_str() ) + 1;
		this->WriteInt(fp, spacer);

		// Write the object name.
		fwrite( objectName.c_str(), spacer, 1, fp);

		// write a version number
		int version = 100;
		this->WriteInt(fp, version);
	  

		// Initialize numPoints and write to stream.
		this->numPoints = PointCoords->GetNumberOfTuples();
		this->WriteInt(fp,this->numPoints);

		// Create an array to hold the property names.
		vtkSmartPointer<vtkStringArray> propNames = vtkSmartPointer<vtkStringArray>::New();
		propNames.TakeReference(vtkStringArray::New());	// <-- TODO: Is this really needed?

		// Prepare to preprocess "numProps" and "propNames[]" for later in program.
		this->numProps = (this->PointData)->GetNumberOfArrays();
		vtkDataArray* currentPropArray;
		int propComponents = -1;
		int totalComponents = 0;
		vtkStdString number;
		vtkStdString propertyName;
		vtkStdString componentName;
		vtkStdString name;


		// Preprocess "numProps" and "propNames[]" for later in program.
		for(int currentProp=0;currentProp<this->numProps;currentProp++)
		{
			// Check if property is a vector (x=1 component) or scalar (x>1 component).
			currentPropArray = (this->PointData)->GetArray( currentProp );
			propComponents = currentPropArray->GetNumberOfComponents();
			totalComponents += propComponents;

			// Get the name of the property
			propertyName = (this->PointData)->GetArrayName(currentProp);

			// If property is scalar, save property name directly.
			if(propComponents == 1)
			{
				propNames->InsertNextValue(propertyName);
			}

			// If property is vector, process then save new property name.
			// (process = append number)
			if(propComponents > 1)
			{
				for(int component=0; component < propComponents; component++)
				{
					// NOTE: convert int to vtkStdString, append to base vtkStdString.
					std::string s;
					std::stringstream out;
					out << component+1;
					out >> number;

					componentName = propertyName + number;
					propNames->InsertNextValue(componentName);
				}
				name = propertyName + "_ApproxVector";
				propNames->InsertNextValue(name);
				totalComponents += 1;
			}
		}
		this->WriteInt(fp,totalComponents);


		//Write the property names.
		vtkStdString propName;
		if(this->numProps != 0)
		{
			for(int prop = 0; prop < totalComponents; prop++)
			{
				// Get the property name.
				propName = propNames->GetValue(prop);
				
				// Write the property name spacer.
				spacer = propName.length() + 1;
				this->WriteInt(fp, spacer);

				// Write the property name.
				fwrite(propName.data(),spacer,1,fp);
			}
		}

		// Write the point coordinates.
		double xyz[3];
		for (int currentPoint=0; currentPoint < (this->numPoints); currentPoint++)
		{
			PointCoords->GetTuple(currentPoint,xyz);
			for (int coordinateIndex=0; coordinateIndex<3; coordinateIndex++)
			{
				this->WriteFloat(fp,xyz[coordinateIndex]);
			}
		}


	// Write the properties.
	float propVal;
	int numComponents;
	for (int currentProp=0; currentProp < (this->numProps); currentProp++)
	{
		currentPropArray = (this->PointData)->GetArray( currentProp );
		numComponents = currentPropArray->GetNumberOfComponents();

		if (numComponents == 1)
		{
			for(int currentPoint=0; currentPoint < (this->numPoints); currentPoint++)
			{
				propVal = currentPropArray->GetTuple(currentPoint)[0];
				this->WriteFloat( fp,(float)propVal );
			}
		}
		else
		{
			vtkWarningMacro("Property '"<< currentPropArray->GetName() << "' is not scalar. Vector components saved as separate properties.");
			this->WriteVector(currentPropArray,numComponents, numPoints,fp);
		}
	}
		fclose(fp);
}

void vtkSGeMSWriter::WriteVector(vtkDataArray* currentPropArray, int numComponents, int numUnits, FILE* fp)
{
	float propVal = 0;
	for(int currentComponent=0; currentComponent<numComponents; currentComponent++)
	{
		for(int currentUnit=0; currentUnit < numUnits; currentUnit++)
		{
			propVal = currentPropArray->GetTuple(currentUnit)[currentComponent];
			this->WriteFloat( fp,(float)propVal );
		}
	}
	this->WriteVectorApproximation(currentPropArray,numComponents,numUnits,fp);
}
void vtkSGeMSWriter::WriteVectorApproximation(vtkDataArray* currentPropArray, int numComponents, int numUnits, FILE* fp)
{
	float currentVal = 0;
	float propVal;

	for(int currentUnit=0; currentUnit < numUnits; currentUnit++)
	{
		propVal = 0;
		for(int currentComponent=0; currentComponent<numComponents; currentComponent++)
		{
			currentVal = currentPropArray->GetTuple(currentUnit)[currentComponent];
			propVal += abs(currentVal);
		}
		this->WriteFloat( fp,propVal );
	}
}

void vtkSGeMSWriter::WriteCgrid(vtkImageData* inputCG)
{
	//If there is any point data, convert it to cell data.
	//Regardless, load the object's data into CellData.
	vtkPointDataToCellData* newCellData = vtkPointDataToCellData::New();
	this->PointData = inputCG->GetPointData();
	if ((this->PointData)->GetNumberOfArrays() != 0)
	{
		newCellData->SetInput(this->GetInput());
		newCellData->Update();
		this->CellData = newCellData->GetOutput()->GetCellData();
	}
	else
	{
		this->CellData = inputCG->GetCellData();
	}

	// Open file.
	FILE* fp=fopen(this->FileName,"wb");

	// Trouble proofing.
	if(fp==NULL)
	{
    vtkErrorMacro("Unable to open output file for writing.");
    return;
  }

	// Write a header with a "magic number"
	this->WriteInt(fp,0xB211175D);

	// Write the class name spacer.
	std::string className = "Cgrid";
	int spacer = strlen( className.c_str() ) + 1;
	this->WriteInt(fp, spacer);

	// Write the class name.
	fwrite( className.c_str(), spacer, 1, fp);
  
  // write the object name spacer.
	std::string objectName = this->FindFileName();
	spacer = strlen( objectName.c_str() ) + 1;
	this->WriteInt(fp, spacer);

	// Write the object name.
	fwrite( objectName.c_str(), spacer, 1, fp);

	// write a version number
	int version = 100;
	this->WriteInt(fp, version);

	// Write the grid dimensions
	int grid_extent[6];
	inputCG->GetWholeExtent(grid_extent);

	// If the ImageData is not volumetric, consider it volumetric
	this->grid_xdim = (grid_extent[1]>0)?grid_extent[1]:1 ;
	this->grid_ydim = (grid_extent[3]>0)?grid_extent[3]:1 ;
	this->grid_zdim = (grid_extent[5]>0)?grid_extent[5]:1 ;

	this->WriteInt(fp, grid_xdim);
	this->WriteInt(fp, grid_ydim);
	this->WriteInt(fp, grid_zdim);

	// Set the numCells variable.
	this->numCells = grid_xdim * grid_ydim * grid_zdim;

	// Write the cell dimensions
	double cell_dim[3];
	inputCG->GetSpacing(cell_dim);

	// If the ImageData is planar etc, consider it volumetric
	this->cell_xdim = (cell_dim[0]>0)?cell_dim[0]:1 ;
	this->cell_ydim = (cell_dim[1]>0)?cell_dim[1]:1 ;
	this->cell_zdim = (cell_dim[2]>0)?cell_dim[2]:1 ;

	this->WriteFloat(fp, cell_xdim);
	this->WriteFloat(fp, cell_ydim);
	this->WriteFloat(fp, cell_zdim);

	// Write the origin
	double origin_coords[3];
	inputCG->GetOrigin(origin_coords);
	this->WriteFloat(fp, (float)origin_coords[0]);
	this->WriteFloat(fp, (float)origin_coords[1]);
	this->WriteFloat(fp, (float)origin_coords[2]);

	// Create an array to hold the property names.
	vtkSmartPointer<vtkStringArray> propNames = vtkSmartPointer<vtkStringArray>::New();
	propNames.TakeReference(vtkStringArray::New());	// <-- Do we need this?

	// Prepare to preprocess "numProps" and "propNames[]" for later in program.
	this->numProps = (this->CellData)->GetNumberOfArrays();
	vtkDataArray* currentPropArray;
	int propComponents = -1;
	int totalComponents = 0;
	vtkStdString number;
	vtkStdString propertyName;
	vtkStdString componentName;
	vtkStdString name;

	// Preprocess "numProps" and "propNames[]" for later in program.
	for(int currentProp=0;currentProp<this->numProps;currentProp++)
	{
		// Check if property is a vector (x=1 component) or scalar (x>1 component).
		currentPropArray = (this->CellData)->GetArray( currentProp );
		propComponents = currentPropArray->GetNumberOfComponents();
		totalComponents += propComponents;

		// Get the name of the property
		propertyName = (this->CellData)->GetArrayName(currentProp);

		// If property is scalar, save property name directly.
		if(propComponents == 1)
		{
			propNames->InsertNextValue(propertyName);
		}

		// If property is vector, process then save new property name.
		// (process = append number)
		if(propComponents > 1)
		{
			for(int component=0; component < propComponents; component++)
			{
				// NOTE: convert int to vtkStdString, append to base vtkStdString.
				std::string s;
				std::stringstream out;
				out << component+1;
				out >> number;

				componentName = propertyName + number;
				propNames->InsertNextValue(componentName);
			}
			name = propertyName + "_ApproxVector";
			propNames->InsertNextValue(name);
			totalComponents += 1;
		}
	}
	this->WriteInt(fp,totalComponents);

	//Write the property names.
	vtkStdString propName;
  if(this->numProps != 0)
  {
    for(int prop = 0; prop < totalComponents; prop++)
    {
			// Get the property name.
			propName = propNames->GetValue(prop);

			// Write the property name spacer.
			spacer = propName.length() + 1;
			this->WriteInt(fp, spacer);

			// Write the property name.
			fwrite(propName.data(),spacer,1,fp);
    }
  }

	// Write the properties.
	float propVal;
	int numComponents;
	for (int currentProp=0; currentProp < (this->numProps); currentProp++)
	{
		currentPropArray = (this->CellData)->GetArray( currentProp );
		numComponents = currentPropArray->GetNumberOfComponents();

		if (numComponents == 1)
		{
			for(int currentCell=0; currentCell < (this->numCells); currentCell++)
			{
				propVal = currentPropArray->GetTuple(currentCell)[0];
				this->WriteFloat( fp,(float)propVal );
			}
		}
		else
		{
			vtkWarningMacro("Property '"<< currentPropArray->GetName() << "' is not scalar. Vector components saved as separate properties.");
			this->WriteVector(currentPropArray,numComponents, numCells,fp);
		}
	}
	fclose(fp);
	newCellData->Delete();
	return;
}

	/*
	// This is an alternative color-interpreter; it determines the basic spectral "color"
	// [i.e. the cumulative value of the vector property if all components were to be combined].
	// it checks to find the primary color, thus finding the section of the spectrum in which to specify;
	// it then finds the secondary color, thus indicating if the final colour should be up-shifted or
	// down-shifted with respect to the primary (dominant) color.

	// Interpret separate RGB
	float red;
	float green;
	float blue;
	float propVal;
	for(int currentCell=0; currentCell<numCells; currentCell++)
	{
		//  Spectrum ===>>> R -> (Y=r+g) -> G -> (c=g+b) -> B -> (v=r+b)
		red = currentPropArray->GetTuple(currentCell)[0];
		green = currentPropArray->GetTuple(currentCell)[1];
		blue = currentPropArray->GetTuple(currentCell)[2];		

		// If red is dominant, color is in (violet)red(yellow) spectrum
		if (red > green & red > blue)
		{
			// If red-green is dominant, bias toward yellow.
			if (green > blue)
			{
				propVal = green;
			}
			// if red-blue is dominant, bias toward purple.
			else
			{
				propVal = 768 - blue;
			}
		}

		// If green is dominant, color is in (yellow)green(cyan) spectrum.
		else if(green > red & green > blue)
		{
			// If green-red is dominant, bias toward yellow.
			if (red > blue)
			{
				propVal = 256 - red;
			}
			// If green-blue is dominant, bias toward cyan.
			else
			{
				propVal = 256 + blue;
			}
		}

		// If blue is dominant, color is in (cyan)blue(violet) spectrum
		else if(blue > red & blue > green)
		{
			// If blue-green is dominant, bias toward cyan
			if (green > red)
			{
				propVal = 512 - green;
			}
			// If blue-red is dominant, bias toward purple.
			else
			{
				propVal = 512 + red;
			}
		}

		// If monochrome-red,
		else if(green==0 & blue ==0)
		{
			propVal = 0;
		}

		// If monochrome-green,
		else if(red==0 & blue==0)
		{
			propVal = 256;
		}

		// If monochrome-blue
		else if(red==0 & green==0)
		{
			propVal = 512;
		}

		// Else, mark spot with a null value (black?)
		else
		{
			propVal = 0;
		}
		this->WriteFloat(fp2,propVal);
	}
	*/


// This is a big-endian binary writer for integers.
void vtkSGeMSWriter::WriteInt(FILE* fp, int number)
{
	char buff[4];
	char* revBuff = (char*)&number;
	for (int i=0; i<4; i++)
	{
		buff[i] = revBuff[3-i];
	}
	fwrite(buff,4,1,fp);
}

// This is a big-endian binary writer for floats.
void vtkSGeMSWriter::WriteFloat(FILE* fp, float number)
{
	char buff[4];
	char* revBuff = (char*)&number;
	for (int i=0; i<4; i++)
	{
		buff[i] = revBuff[3-i];
	}
	fwrite(buff,4,1,fp);
}