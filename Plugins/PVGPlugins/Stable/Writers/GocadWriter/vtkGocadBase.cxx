/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkWriter.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Gocad Base Class
// Written by Robert Maynard

#include "vtkGocadBase.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkStringArray.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"

vtkCxxRevisionMacro(vtkGocadBase, "$Revision: 0.14 $");
vtkStandardNewMacro(vtkGocadBase);

//----------------------------------------------------------------------------
vtkGocadBase::vtkGocadBase()
{
	this->X_Axis = -1;
	this->Y_Axis = -1;
	this->Z_Axis = -1;

	this->FileType = -1;
	this->Append = false;
	
	this->Precision = 12;
	
	this->PointData = NULL;
	
	this->HeaderName = "";
}

//----------------------------------------------------------------------------
vtkGocadBase::~vtkGocadBase()
{ 
  this->PointData=NULL;
  this->SetFileName(0);
}

//----------------------------------------------------------------------------
void vtkGocadBase::SetHeaderName(vtkStdString headerName)
{
	this->HeaderName = headerName;

}

//----------------------------------------------------------------------------
void vtkGocadBase::WriteProperties(ostream* fp, vtkStringArray* propNames, int pointId)
{
	//propertyTuple is initialized to NULL, so that it can be sized dynamically inside the for loop
	double* propertyTuple = NULL;
	for(int currentProperty = 0; currentProperty < propNames->GetNumberOfValues(); currentProperty++) 
	{
		// Used to make sure we don't try to read an invalid array (if there are any)
		if((this->PointData)->HasArray(propNames->GetValue(currentProperty)))
		{
			vtkIdType numberOfPropComponents = ( (this->PointData)->GetArray(propNames->GetValue(currentProperty)) )->GetNumberOfComponents();
			propertyTuple = new double[numberOfPropComponents];
			//Fill propertyTuple with current property information
			( (this->PointData)->GetArray(propNames->GetValue(currentProperty)) )->GetTuple(pointId, propertyTuple);
			// Print out all components of the property to the file
			for(int propertyComponent = 0; propertyComponent < numberOfPropComponents; propertyComponent++)
			{
				*fp << propertyTuple[propertyComponent] << "\t";
			}
			//delete allocated array at each loop iteration because it must correspond to each "new" statement
			delete [] propertyTuple;
		}
	}
}


//----------------------------------------------------------------------------
void vtkGocadBase::FindFileName(vtkStdString &nameWithPath, vtkStdString &name)
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
	vtkStdString extension = fName.substr(dotPosition);
	int extLen = extension.length();

	//Save the file name AND the file extention into a stdString, so that the length that be found
	vtkStdString nameWithExt = fName.substr(slashPosition);
	int nameExtLen = nameWithExt.length();

	//Determine the length of the word, so that it can be taken from fName, which has the file name and extension
	int finalNameLength = nameExtLen - extLen;

	//Pull the file name out of the full path (fName)
	
	name = fName.substr(slashPosition, finalNameLength);
	
	nameWithPath = fName.substr(0, dotPosition);
	
	//exception is that if we have set the headername in compositewriter we override the file path name
	if (this->HeaderName)
		{
		name = this->HeaderName;
		}
}

//----------------------------------------------------------------------------
vtkStdString vtkGocadBase::FindFileName()
{
	//exception is that if we have set the headername in compositewriter we override the file path name
	if (this->HeaderName && this->HeaderName != "")
		{
		return this->HeaderName;
		}
		
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

//----------------------------------------------------------------------------
/*
Method is used to determine if the user has selected
meters (m) or feet (ft) as the axis units.
After determining what the axis units are, a string
will be returned containing the AXIS_UNIT line.
*/
vtkStdString vtkGocadBase::GetUnitString()
{
	vtkStdString text = "AXIS_UNIT \"";
	if(this->X_Axis == METRES)
	{
		text.append("m");
	}
	else
	{
		text.append("ft");
	}
	text.append("\" \"");
	if(this->Y_Axis == METRES)
	{
		text.append("m");
	}
	else
	{
		text.append("ft");
	}
	text.append("\" \"");
	if(this->Z_Axis == METRES)
	{
		text.append("m");
	}
	else
	{
		text.append("ft");
	}
	text.append("\"");
	return text;
}


//----------------------------------------------------------------------------
/*
Writes GoCad header to the file. It will remain almost the same for all
different types of files that will be exported from Paraview.
The only information that changes is the GoCad object type, which is given
by the caller (objLabel parameter).
*/
void vtkGocadBase::WriteHeader(ostream* fp, char* objLabel, vtkStdString name )
{	
	*fp << "GOCAD " << objLabel << " 1" << endl;
	*fp << "HEADER {" << endl;
	*fp << "name:" << name << endl;
	*fp << "}" << endl;
	*fp << "GOCAD_ORIGINAL_COORDINATE_SYSTEM" << endl;
	*fp << "NAME Default" << endl;
	*fp << "AXIS_NAME \"X\" \"Y\" \"Z\"" << endl;
	//Print the axis units to the file (meters or feet)
	*fp << this->GetUnitString() << endl;
	*fp << "ZPOSITIVE Elevation" << endl;
	*fp << "END_ORIGINAL_COORDINATE_SYSTEM" << endl;
}


//----------------------------------------------------------------------------
void vtkGocadBase::PrintSelf(ostream& fp, vtkIndent indent)
{
	this->Superclass::PrintSelf(fp,indent);

	// Print all local variables
	// File Type will be 1, 2, 3 or 4 which stand for TSurf(1), PLine(2), VSet(3), TSurf Strip(4)
	// X, Y and Z axes will be 1 or 2, which stand for meters(1) or feet(2)
	fp << indent << "File Type: " << this->FileType << endl;
	fp << indent << "X Axis Units: " << this->X_Axis << endl;
	fp << indent << "Y Axis Units: " << this->Y_Axis << endl;
	fp << indent << "Z Axis Units: " << this->Z_Axis << endl;
	fp << indent << "Precision Level: " << this->Precision << endl;
	fp << indent << "Append On: " << this->Append << endl;
}

//----------------------------------------------------------------------------
//converts given string to lower case.
char* vtkGocadBase::StringToLower(const char* strToConvert)
{
	//change each element of the string to lower case
	int i = 0;
	while(strToConvert[i] != '\0')
	{
		i++;
	}
	//make room for null termination by incrementing "i"
	i++; 
	char* newStr = new char[i];

	for(int x = 0; x < i; x++)
	{
		//convert current character to lower case
		newStr[x] = tolower(strToConvert[x]);
	}
	return newStr;
}

//----------------------------------------------------------------------------
ostream* vtkGocadBase::OpenFile( const char* filename )
{
  ostream *fp;
  if (this->Append)
    {
    fp = new ofstream(filename, ios::app);
    }
  else
    {
    fp = new ofstream(filename, ios::out);
    }  
  fp->precision( this->Precision );
  return fp;
}
 
 //----------------------------------------------------------------------------
void vtkGocadBase::CloseFile( ostream* fp ){  
  if (fp)
    {    
    delete fp;
    }
}