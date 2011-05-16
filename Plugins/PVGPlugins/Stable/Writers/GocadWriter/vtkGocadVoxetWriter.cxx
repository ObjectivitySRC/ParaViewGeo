/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkWriter.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Gocad Voxet Writer
// Export structured grid data from paraview into a format that can be read by Gocad.
// Written by Matthew Livingstone & Eric Daoust - 05/2008

#include "vtkGocadVoxetWriter.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkDataArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"
#include "vtkCellDataToPointData.h"
#include <vtksys/ios/sstream>

vtkCxxRevisionMacro(vtkGocadVoxetWriter, "$Revision: 1.27 $");
vtkStandardNewMacro(vtkGocadVoxetWriter);

vtkGocadVoxetWriter::vtkGocadVoxetWriter()
{
	this->I_Dimension = -1;
	this->J_Dimension = -1;
	this->K_Dimension = -1;
	this->PointData = NULL;
	this->CellData = NULL;
}

vtkGocadVoxetWriter::~vtkGocadVoxetWriter()
{
	this->CellData=NULL;
}


void vtkGocadVoxetWriter::WritePointData(vtkImageData* input, ostream* file, vtkStringArray* propNames)
{
	//copy header information, which includes I/J/K and property names
	*file << "*" << endl;
	*file << "*\tI\tJ\tK\t";
	for(int nameIndex = 0; nameIndex < propNames->GetNumberOfValues(); nameIndex++)
	{
		*file << propNames->GetValue(nameIndex) << "\t";
	}
	*file << endl;
	*file << "*" << endl;

	//index of each point in the voxet
	vtkIdType i = 0;
	vtkIdType j = 0;
	vtkIdType k = 0;

	// Write point data to file for current cell.
	int numOfTuples = input->GetNumberOfPoints();
	for(int currentPoint = 0; currentPoint < numOfTuples; currentPoint++) 
	{
		//print out I/J/K index to file, plus property information
		*file << "\t" << i << "\t" << j << "\t" << k << "\t";
		this->WriteProperties(file, propNames, currentPoint);
		*file << endl;

		i++;
		if(i == this->I_Dimension)
		{
			i = 0;
			j++;
		}
		if(j == this->J_Dimension)
		{
			j = 0;
			k++;
		}
	}
}

void vtkGocadVoxetWriter::WriteData()
{
	//this writer has 2 files, one with the specification and properties, the other containing point information
	ostream *file1, *file2;
	int errorOccurred = 0;

	//coordinates of point having ID=0, this is assumed to be the origin of the voxet
	double* originCoords;

	//the number of points in each of the 3 dimensions
	int* dimensions;

	//spefifies the min and max positions on each axis
	double* bounds;

	vtkImageData *input = vtkImageData::SafeDownCast(this->GetInput());
	if(input == NULL)
	{
		vtkErrorMacro("invalid input type.");
		return;
	}
	this->CellData = input->GetCellData();
	this->PointData = input->GetPointData();
	originCoords = input->GetOrigin();
	dimensions = input->GetDimensions();
	this->I_Dimension = dimensions[0];
	this->J_Dimension = dimensions[1];
	this->K_Dimension = dimensions[2];

	vtkSmartPointer<vtkCellDataToPointData> cellDataConverter = vtkSmartPointer<vtkCellDataToPointData>::New();
	//if the input has cell data, we want to convert it to point data, since Gocad does not support cell data
	if(this->CellData->GetNumberOfArrays() > 0)
	{
		cellDataConverter.TakeReference(vtkCellDataToPointData::New());
		cellDataConverter->SetInput(input);
		cellDataConverter->Update();
		int numberOfPointProps = this->PointData->GetNumberOfArrays();
		this->PointData = cellDataConverter->GetOutput()->GetPointData();
		const char* currentName = NULL;
		//property names are not transfered from CellData to PointData, so we do it by hand
		for(int i = numberOfPointProps; i < this->PointData->GetNumberOfArrays(); i++)
		{
			currentName = this->CellData->GetArrayName(i-numberOfPointProps);
			this->PointData->GetArray(i)->SetName(currentName);
		}
	}
	
	// Ensure there is no error when opening the file.
	file1 = this->OpenFile( this->FileName );
	if ( !file1 )
	{
		vtkErrorMacro("Error creating/opening the file for writing.");
		return;
	}
	//nameWithPath is the name of the 2nd file, full path included, while name is just the name of the 2nd file (without path)
	vtkStdString nameWithPath, name;

	this->FindFileName(*&nameWithPath, *&name);

	//here we want to see if the 2nd file exists on the disk, if yes we do not want to continue
	ifstream fin;
	fin.open(nameWithPath, ios::in);
	if(!fin.fail())
	{
		vtkErrorMacro("File2 already exists: " << *name);
		fin.close();
		return;
	}
	fin.close();

	//file2 does not previously exist, so we can begin writing
	file2 = this->OpenFile( nameWithPath );
	
	// Write header.
	if(input->GetNumberOfCells() > 0)
	{
		this->WriteHeader(file1, "Voxet", this->FindFileName() );
	}
	else
	{
		errorOccurred = 1;
	}

	file1->flush();
	//fail() detects if failbit or badbit is set. If so, there was an error in writing
	if(file1->fail())
	{
		vtkErrorMacro("Error writing to the file; deleting file: " << this->FileName);
		this->CloseVTKFile(file1);
		this->SetFileName(0);
		delete file1;
		return;
	}
	if(!errorOccurred)
	{
		//write coordinates of grid origin
		*file1 << "AXIS_O " << originCoords[0] << " " << originCoords[1] << " " << originCoords[2] << endl;
		bounds = input->GetBounds();
		//AXIS_U/V/W requires the length of each axis, which is specified by MAX-MIN
		*file1 << "AXIS_U " << (bounds[X_MAX]-bounds[X_MIN]) << " 0 0" << endl;
		*file1 << "AXIS_V 0 " << (bounds[Y_MAX]-bounds[Y_MIN]) << " 0" << endl;
		*file1 << "AXIS_W 0 0 " << (bounds[Z_MAX]-bounds[Z_MIN]) << endl;
		//leave these values to 0 and 1, for Gocad visibility purposes
		*file1 << "AXIS_MIN " << 0 << " " << 0 << " " << 0 << endl;
		*file1 << "AXIS_MAX " << 1 << " " << 1 << " " << 1 << endl;
		//the number of points on each axis
		*file1 << "AXIS_N " << this->I_Dimension << " " << this->J_Dimension << " " << this->K_Dimension << endl;
		//paraview does not support axis names (that we know of) so hardcode our own names
		*file1 << "AXIS_NAME \"axis-X\" \"axis-Y\" \"axis-Z\"" << endl;
		*file1 << "AXIS_UNIT \" number\" \" number\" \" number\" " << endl;
		*file1 << "AXIS_TYPE even even even" << endl;
		
		// Create StringArray for property names.
		//vtkStringArray* names = vtkStringArray::New();
		vtkSmartPointer<vtkStringArray> names = vtkSmartPointer<vtkStringArray>::New();
		names.TakeReference(vtkStringArray::New());

		// Any additional properties are stored here.
		if((this->PointData)->GetNumberOfArrays() > 0)//has props
		{ 
			int numberOfArrays = (this->PointData)->GetNumberOfArrays();
			const char* name;

			// For each property, extract the value.
			for(int i = 0; i < numberOfArrays; i++) 
			{
				name = (this->PointData)->GetArrayName(i);
				
				// If the property is valid, then add property name to names.
				if(name) 
				{
					names->InsertNextValue(name);
				}
			}
		}
		//write the name of the 2nd file (without extension) to the 1st file
		*file1 << "ASCII_DATA_FILE " << name << endl;
		vtkIdType propNumber;
		vtkIdType esize;
		char* lowerCaseName;
		for(int propIndex = 0; propIndex < names->GetNumberOfValues(); propIndex++)
		{
			//Property numbers have numbering of 1,2,3,4...
			propNumber = propIndex+1;
			*file1 << "PROPERTY " << propNumber << " \"" << names->GetValue(propIndex) << "\"" << endl;
			//must convert name to lower case for writing on these lines
			lowerCaseName = this->StringToLower(names->GetValue(propIndex));
			*file1 << "PROPERTY_CLASS " << propNumber << " \"" << lowerCaseName << "\"" << endl;
			*file1 << "PROPERTY_CLASS_HEADER " << propNumber << " \"" << lowerCaseName << "\" {" << endl;
			*file1 << "}" << endl;
			*file1 << "PROP_ORIGINAL_UNIT " << propNumber << " none" << endl;
			*file1 << "PROP_UNIT " << propNumber << " none" << endl;
			*file1 << "PROP_NO_DATA_VALUE " << propNumber << " " << NO_VALUE << endl;
			//ESIZE assumes I/J/K therefore we must add 3 to the number of properties
			esize = 3 + ( (this->PointData)->GetArray(names->GetValue(propIndex)) )->GetNumberOfComponents();
			*file1 << "PROP_ESIZE " << propNumber << " " << esize << endl;
		}

		//write cell information to file
		if(input->GetNumberOfCells() > 0)
		{
			this->WritePointData(input, file2, names);
		}
		else
		{
			vtkErrorMacro("Error writing: no cells the write; deleting file: " << this->FileName);
			// Close the stream
			this->CloseFile(file1);
			// Delete the file
			this->SetFileName(0);
			return;
		}
	}

	*file1 << "END" << endl;
	//close connection to the file
	this->CloseFile(file1);
	this->CloseFile(file2);
}


int vtkGocadVoxetWriter::FillInputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
	return 1;
}

vtkImageData* vtkGocadVoxetWriter::GetInput()
{
	return vtkImageData::SafeDownCast(this->Superclass::GetInput());
}

vtkImageData* vtkGocadVoxetWriter::GetInput(int port)
{
	return vtkImageData::SafeDownCast(this->Superclass::GetInput(port));
}

