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
// Gocad PolyData Writer
// Export polygonal data from paraview into a format that can be read by Gocad.
// Written by Matthew Livingstone & Eric Daoust - 05/2008

#include "vtkGocadWriter.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"
#include "vtkCellDataToPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDataSetSurfaceFilter.h"

vtkCxxRevisionMacro(vtkGocadWriter, "$Revision: 1.28 $");
vtkStandardNewMacro(vtkGocadWriter);

vtkGocadWriter::vtkGocadWriter()
{
	this->Cells = NULL;
	this->PointData = NULL;
	this->CellData = NULL;
	this->PointCoords = NULL;
}

vtkGocadWriter::~vtkGocadWriter()
{ 
	this->Cells=NULL;
	this->CellData=NULL;
	this->PointCoords=NULL;
}


void vtkGocadWriter::WritePointData(ostream* fp, vtkStringArray* propNames, char* vrtxLabel)
{
	double* currentTuple;
	double* currentCellPropTuple;

	// Need to keep track of point numbers, as we are iterating over teh cells, not the points
	int pointCounter = 0;
	// Stores number of points in current cell
	vtkIdType* npts = new vtkIdType();

	// Create array of points in current cell.
	vtkIdType *pts;
	
	int numberOfCells = (this->Cells)->GetNumberOfCells();
	
	// Write cell data to file.
	//initialize iterator to traverse cells
	(this->Cells)->InitTraversal();
	for(int currentCell = 0; currentCell < numberOfCells; currentCell++) 
	{
		(this->Cells)->GetNextCell(*npts, *&pts);
		
		// Write triangle of current surface to file.
		for(int pointIterator = 0; pointIterator < *npts; pointIterator++)
		{
			currentTuple =this->PointCoords->GetTuple(pts[pointIterator]);
			*fp << vrtxLabel << " " << pointCounter << " ";
			pointCounter++;

			// Write (x,y,z) coords to file for current point.
			for(int coordinate = 0; coordinate < (this->PointCoords)->GetNumberOfComponents(); coordinate++) 
			{
				*fp << currentTuple[coordinate] << " ";
			}

			// Write point properties
			this->WriteProperties(fp, propNames, pts[pointIterator]);

			// Write cell properties
			for(int cellPropCount = 0; cellPropCount < this->CellData->GetNumberOfArrays(); cellPropCount++)
			{
				if(this->CellData->GetArray(cellPropCount))
				{
					const char* dataType = this->CellData->GetArray(cellPropCount)->GetName();
					currentCellPropTuple = this->CellData->GetArray(cellPropCount)->GetTuple(currentCell);
					for(int tupleCount = 0; tupleCount < this->CellData->GetArray(cellPropCount)->GetNumberOfComponents(); tupleCount++)
					{
						*fp << currentCellPropTuple[tupleCount] << " ";
					}
				}
			}
			// End of properties for this point
			*fp<<" "<<endl;
		}
	}
	delete npts;
}

void vtkGocadWriter::WriteStripSurfaces(ostream* fp, vtkPolyData* input, char* vrtxLabel,
											  vtkStringArray* propNames)
{	
	// Ensure pointCoords contain data.
	if((this->PointCoords) && this->Cells && (this->PointCoords)->GetNumberOfTuples() > 0
		&& (this->Cells)->GetNumberOfCells() > 0) 
	{
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType();

		// Create array of points in current cell.
		vtkIdType *pts;

		// Keep track of points as we are iterating for cells not points
		int pointCounter = 0;
		
		int numberOfCells = (this->Cells)->GetNumberOfCells();

		//Write the point data to the output file
		*fp << "TFACE" << endl;
		this->WritePointData(fp, propNames, vrtxLabel);
		
		// Write cell data to file.
		//initialize iterator to traverse cells
		(this->Cells)->InitTraversal();
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++) 
		{
			(this->Cells)->GetNextCell(*npts, *&pts);
			
			// Write triangle of current surface to file.
			for(int cellIterator = 0; cellIterator < *npts-2; cellIterator++)
			{
				*fp << "TRGL " << pointCounter << " " << pointCounter+1 << " " << pointCounter+2 << endl;
				pointCounter += 3;
			}
		}
		delete npts;
	}
}

void vtkGocadWriter::WriteSurfaces(ostream* fp, vtkPolyData* input, char* vrtxLabel, vtkStringArray* propNames)
{	
	// Ensure pointCoords contain data.
	if((this->PointCoords) && this->Cells && (this->PointCoords)->GetNumberOfTuples() > 0 && (this->Cells)->GetNumberOfCells() > 0) 
	{
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType();

		// Create array of points in current cell.
		vtkIdType *pts;

		// Keep track of points as we are iterating for cells not points
		int pointCounter = 0;
		
		int numberOfCells = (this->Cells)->GetNumberOfCells();

		//Write the point data to the output file
		*fp << "TFACE" << endl;
		this->WritePointData(fp, propNames, vrtxLabel);
		
		// Write cell data to file.
		//initialize iterator to traverse cells
		(this->Cells)->InitTraversal();
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++) 
		{
			(this->Cells)->GetNextCell(*npts, *&pts);
			
			// Surface triangles must have at least three points.
			if(*npts == TRIANGLE_POINTS)
			{
				for(int a = 0; a < *npts-2; a++)
				{
					//Use pts[0] so that all the triangles are connected to a single point
					//This is most useful when there are cylindrical objects that are being written
					*fp << "TRGL " << pointCounter << " " << pointCounter+1 << " " << pointCounter+2 << endl;
					pointCounter += 3;
				}
			}
			else if(*npts == TRIANGLE_POINTS+1)
			{
				for(int a = 0; a < *npts-3; a++)
					{
						//Use pts[0] so that all the triangles are connected to a single point
						//This is most useful when there are cylindrical objects that are being written
						*fp << "TRGL " << pointCounter << " " << pointCounter+1 << " " << pointCounter+2 << endl;
						*fp << "TRGL " << pointCounter << " " << pointCounter+2 << " " << pointCounter+3 << endl;
						pointCounter += 4;
					}
			}else
			{
				//this is not a critical error, as triangles with only 2 points are incorrect and really are not needed
				//at a future time we would write this out as a line if 2 or a point if 1
				vtkDebugMacro("Too few points in cell; Triangle needs " << TRIANGLE_POINTS << " cell has " << *npts);
			}
		}
		delete npts;
	}
}

void vtkGocadWriter::WriteLines(ostream* fp, vtkPolyData* input, char* vrtxLabel, vtkStringArray* propNames)
{	
	// Ensure pointCoords contain data.
	if((this->PointCoords) && this->Cells && (this->PointCoords)->GetNumberOfTuples() > 0 && (this->Cells)->GetNumberOfCells() > 0) 
	{		
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType();

		// Create array of points in current cell.
		vtkIdType *pts;

		// Keep track of points as we are iterating for cells not points
		int pointCounter = 0;
		
		int numberOfCells = (this->Cells)->GetNumberOfCells();
		
		//Write the point data to the output file
		*fp << "ILine" << endl;
		this->WritePointData(fp, propNames, vrtxLabel);

		//initialize iterator to traverse cells
		(this->Cells)->InitTraversal();

		//Go through the cells, and print out the line segments inside of each cell
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++) 
		{
			(this->Cells)->GetNextCell(*npts, *&pts);
			
			// lines must have at least two points.
			if(*npts >= LINE_POINTS)
			{
				//Write the segments of the line
				for(int n = 0; n < *npts-1; n++) 
				{
					*fp << "SEG " << pointCounter << " " << pointCounter+1 << endl;
					pointCounter+=2;
				}
			}
		}
	}
}

void vtkGocadWriter::WritePoints(ostream* fp, vtkPolyData* input, char* vrtxLabel, vtkStringArray* propNames)
{
	// Ensure pointCoords contain data.
	if((this->PointCoords) && (this->Cells) && (this->PointCoords)->GetNumberOfTuples() > 0 && (this->Cells)->GetNumberOfCells() > 0) 
	{		
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType();

		int numberOfCells = (this->Cells)->GetNumberOfCells();

		//Write the point data to the output file
		this->WritePointData(fp, propNames, vrtxLabel);
	}
}

void vtkGocadWriter::WriteData()
{
	// Output stream that will be used to write the data to the file
	ostream *fp;
	// Grab object data from paraview
	vtkPolyData *input = vtkPolyData::SafeDownCast(this->GetInput());

	// input returns NULL if the data is not Polygonal
	if(input == NULL)
	{
		vtkUnstructuredGrid *inputUnstruc = vtkUnstructuredGrid::SafeDownCast(this->GetInput());

		if (inputUnstruc == NULL)
			{
			vtkErrorMacro("Writer only accepts PolyData and UnstructuredGrid, input type is invalid.");
			return;
			}

		vtkDataSetSurfaceFilter *UnStrucGridConverter = vtkDataSetSurfaceFilter::New();	
		UnStrucGridConverter->SetPassThroughCellIds(1);
		UnStrucGridConverter->SetPassThroughPointIds(1);
		UnStrucGridConverter->SetInput(inputUnstruc);
		UnStrucGridConverter->Update();

		input = vtkPolyData::SafeDownCast( UnStrucGridConverter->GetOutput() );
		
	}
	if ( input && input->GetNumberOfCells() > 0 && input->GetNumberOfPoints() )
	  {
	  this->PointData = input->GetPointData();
	  this->CellData = input->GetCellData();
	  this->PointCoords = input->GetPoints()->GetData();
	  }
  else
    {
    return;
    }
	
	//if called from composite data writer, append to the passed in file
	fp = this->OpenFile( this->FileName );
	
	if ( !fp )
	{
		vtkErrorMacro("Error creating/opening the file for writing.");
		return;
	}

  //write the file out
	this->WriteFile(fp,input);

	// The file has been written correctly, close connection to the file
  this->CloseFile( fp );
}

void vtkGocadWriter::WriteFile(ostream* fp, vtkPolyData* input)
{

	// Determine the type of data that is being exported from paraview, 
	// set FileType to identify what type of file we are using (will be used later),
	// and write the header file for the corresponding type of data
	if(input->GetPolys()->GetNumberOfCells() > 0)
	{
		this->FileType = TSURF_MODE;
		this->Cells = input->GetPolys();		
		this->WriteHeader(fp, "TSurf", this->FindFileName() );		
		this->WriteObject( fp, input );
	}
	if(input->GetLines()->GetNumberOfCells() > 0)
	{
		this->FileType = PLINE_MODE;
		this->Cells = input->GetLines();
		this->WriteHeader(fp, "PLine", this->FindFileName() );
		this->WriteObject( fp, input );
	}
	if(input->GetVerts()->GetNumberOfCells() > 0)
	{
		this->FileType = VSET_MODE;
		this->Cells = input->GetVerts();
		this->WriteHeader(fp, "VSet", this->FindFileName() );
		this->WriteObject( fp, input );
	}
	if(input->GetStrips()->GetNumberOfCells() > 0)
	{
		this->FileType = TSURF_STRIP_MODE;
		this->Cells = input->GetStrips();
		this->WriteHeader(fp, "TSurf", this->FindFileName() );
		this->WriteObject( fp, input );
	}	

}

int vtkGocadWriter::WriteObject(ostream* fp, vtkPolyData* input)
{
  
  fp->flush();
	// fail() detects if failbit or badbit is set. If so, there was an error in writing
	if(fp->fail())
	{
		vtkErrorMacro("Error writing to the file; deleting file: " << this->FileName);
		// Close the stream
		this->CloseFile(fp);
		// Delete the file
		this->SetFileName(0);
		return 0;
	}

	// Names of point properties
	// This is passed into other methods
	vtkSmartPointer<vtkStringArray> names = vtkSmartPointer<vtkStringArray>::New();
	names.TakeReference(vtkStringArray::New());
	// Names of cell properties
	// This is only used to print property names to the file. Cell properties are iterated
	//    over when writing the points to the file.
	vtkSmartPointer<vtkStringArray> cellNames = vtkSmartPointer<vtkStringArray>::New();
	cellNames.TakeReference(vtkStringArray::New());

	// This decides whether object is PVRTX or VRTX (with or without properties)
	char* vrtxLabel = "VRTX";

	// Any additional properties are stored here.
	if((this->PointData)->GetNumberOfArrays() > 0 || (this->CellData)->GetNumberOfArrays() > 0)
	{ //has props
		int numberOfArrays = (this->PointData)->GetNumberOfArrays();
		vtkStdString name;

		// NOTE: startNum is used due to the fact that when a LINE object is being exported,
		//       there will be an array at index 0 in PointData that does not have a name,
		//       and does not contain and property data that we will be using. Therefore it
		//       is skipped.
		int startNum = 0;
		if(input->GetLines()->GetNumberOfCells() > 0)
		{
			// Ensure we do not read the first array
			startNum = 1;
		}

		// For each property, extract the value.
		for(int i = startNum; i < numberOfArrays; i++) 
		{
			name = (this->PointData)->GetArrayName(i);
			
			// If the property is valid, then add property name to names.
			//we check if we can get the array, so we skip string arrays, since we don't support them
			if(name && this->PointData->GetArray(i)) 
			{
				names->InsertNextValue(name);
			}
		}

		// Cell Property names
		numberOfArrays = (this->CellData)->GetNumberOfArrays();
		for(int i = 0; i < numberOfArrays; i++)
		{
			name = (this->CellData)->GetArrayName(i);
			
			// If the property is valid, then add property name to names.
			//we check if we can get the array, so we skip string arrays, since we don't support them
			if(name && this->CellData->GetArray(i)) 
			{
				// This is used to ensure that we do not have duplicate property names
			  // Cell data that has the same name as point data is prepended with "CD"
				if((this->PointData)->HasArray(name))
				{
					name.insert(0,"CD");
				}
				cellNames->InsertNextValue(name);
			}
		}
	}
	//If the points have properties, we are using PVRTXs
	if(names->GetNumberOfValues() || cellNames->GetNumberOfValues()) 
	{
		vrtxLabel = "PVRTX";
		*fp << "PROPERTIES ";

		// Point Properties
		for(int i = 0; i < names->GetNumberOfValues(); i++) 
		{
			vtkStdString newName = names->GetValue(i);

			// Replace " " with "_" as GoCad files are space delimited
			int loc = newName.find_first_of(" ");
			while(loc > -1)
				{
				newName.replace(loc,1,"_");
				loc = newName.find_first_of(" ");
				}

			*fp << newName << " ";
		}

		// Cell Properties
		for(int i = 0; i < cellNames->GetNumberOfValues(); i++) 
		{
			vtkStdString newName = cellNames->GetValue(i);

			// Replace " " with "_" as GoCad files are space delimited
			int loc = newName.find_first_of(" ");
			while(loc > -1)
				{
				newName.replace(loc,1,"_");
				loc = newName.find_first_of(" ");
				}

			*fp << newName << " ";
		}
		//All Gocad-specific information
		*fp << endl << "NO_DATA_VALUES ";
		
		for(int i = 0; i < names->GetNumberOfValues(); i++) 
		{
			*fp << NO_VALUE << " ";
		}
		for(int i = 0; i < cellNames->GetNumberOfValues(); i++) 
		{
			*fp << NO_VALUE << " ";
		}
		*fp << endl << "PROPERTY_CLASSES ";

		// Point Properties
		for(int i = 0; i < names->GetNumberOfValues(); i++) 
		{
			vtkStdString newName = names->GetValue(i);

			// Replace " " with "_"
			int loc = newName.find_first_of(" ");
			while(loc > -1)
				{
				newName.replace(loc,1,"_");
				loc = newName.find_first_of(" ");
				}

			*fp << StringToLower(newName) << " ";
		}

		// Cell Properties
		for(int i = 0; i < cellNames->GetNumberOfValues(); i++) 
		{
			vtkStdString newName = cellNames->GetValue(i);

			// Replace " " with "_"
			int loc = newName.find_first_of(" ");
			while(loc > -1)
				{
				newName.replace(loc,1,"_");
				loc = newName.find_first_of(" ");
				}

			*fp << StringToLower(newName) << " ";
		}
		
		*fp << endl << "ESIZES ";
		for(int i = 0; i < names->GetNumberOfValues(); i++) 
		{
			vtkIdType arrayLen;
		  if(this->PointData->HasArray(names->GetValue(i)))
			{
				arrayLen = ( (this->PointData)->GetArray(names->GetValue(i)) )->GetNumberOfComponents();
			}
			*fp << arrayLen << " ";
		}
		for(int i = 0; i < cellNames->GetNumberOfValues(); i++) 
		{
			vtkIdType arrayLen;
		  if(this->CellData->HasArray(cellNames->GetValue(i)))
			{
				arrayLen = ( (this->CellData)->GetArray(cellNames->GetValue(i)) )->GetNumberOfComponents();
			}
			*fp << arrayLen << " ";
		}
		*fp << endl;
	}

	//write line information to file
	if(this->FileType == PLINE_MODE)
	{
		this->WriteLines(fp, input, vrtxLabel, names);
	}
	//write surface information to file
	else if(this->FileType == TSURF_MODE)
	{
		this->WriteSurfaces(fp, input, vrtxLabel, names);
	}
	//write strip surface information to file
	else if(this->FileType == TSURF_STRIP_MODE)
	{
		this->WriteStripSurfaces(fp, input, vrtxLabel, names);
	}
	//write point information to file
	else if(this->FileType == VSET_MODE) 
	{
		this->WritePoints(fp, input, vrtxLabel, names);	  
	}
	else
	{
		// The output data was not Polygonal data.
		vtkErrorMacro("Output object did not match Poly data types; deleting file: " << this->FileName);
		// Close the stream
		this->CloseVTKFile(fp);
		// Delete the file
		this->SetFileName(0);
		return 0;
	} 	
	*fp << "END" << endl;
  return 1;
}


vtkPolyData* vtkGocadWriter::GetInput()
{
	return vtkPolyData::SafeDownCast(this->Superclass::GetInput());
}

vtkPolyData* vtkGocadWriter::GetInput(int port)
{
	return vtkPolyData::SafeDownCast(this->Superclass::GetInput(port));
}



void vtkGocadWriter::SetFileName(const char* filename)
{	
	//Was going to be used to auto-detect the type of object being exported,
	// and attatch the proper extension. Unfortunately, the GetInput()
	// method cannot be used, as it is not populated until the main
	// WritData method is called.
	vtkStdString fName = filename;
	//nameNoExtension will be used to define the new file name afterwards
	vtkStdString nameNoExtension = filename;

	//Find position of '.' that occurs before the file extension
	int dotPosition = fName.find_last_of('.');
	vtkStdString fileExtension = fName.substr(dotPosition+1);

	//It was intended that we were going to let users have their own file extensions
	// but if they give their own extension, the writer fails to initialize.
	// This error is not due to our changes, but occurs even without changing
	// the SetFileName method...
	// This apparently cannot be fixed by our hands, it is a paraview issue

  vtkPolyData *input = this->GetInput(); //make sure in the case of multiblocks the input is not null  
	if( input && (fileExtension == "ts" || fileExtension == "vs" || fileExtension == "pl") )
	{
		int nameLength = fName.length();
		int extLength = fileExtension.length();
		int finalLength = nameLength - extLength;
		nameNoExtension = fName.substr(0, finalLength);
		if(input->GetPolys()->GetNumberOfCells() > 0)
		{
			nameNoExtension.append("ts");
		}
		else if(input->GetLines()->GetNumberOfCells() > 0)
		{
			nameNoExtension.append("pl");
		}
		else if(input->GetVerts()->GetNumberOfCells() > 0)
		{
			nameNoExtension.append("vs");
		}
		else if(input->GetStrips()->GetNumberOfCells() > 0)
		{
			nameNoExtension.append("ts");
		}
	}

	this->Superclass::SetFileName(nameNoExtension);
}

//----------------------------------------------------------------------------
int vtkGocadWriter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}