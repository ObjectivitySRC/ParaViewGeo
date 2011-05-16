/*Matthew Livingstone
March 2009

Point File Reader:
- Based on Line Network Reader
- Can read any separated file
- User can specify separator, as well as a no data value for empty values
*/

#include "vtkPointSetReader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkStringArray.h"
#include "vtkIntArray.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"

#include "vtkConeSource.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPolyLine.h"
#include "vtkCollection.h"
#include "vtkFloatArray.h"
#include "vtkCylinderSource.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkMath.h"
#include "vtkVectorText.h"
#include "vtkLineSource.h"
#include "vtkTextMapper.h"
#include "vtkPointSource.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkStdString.h"
#include <vtkstd/map>

using namespace std;

//------------------------------------------------------------------------------------------

namespace
{
  typedef vtkstd::map<vtkStdString,int*> charArraymap;
  typedef vtkstd::map<vtkStdString,int*>::iterator charArraymapit;
  typedef vtkstd::pair<map<vtkStdString,int*>::iterator,bool> ret;
}

struct LNR_Internals
{
	vtkstd::map < vtkStdString, vtkDoubleArray* > doubleArrayMap;
	vtkstd::map < vtkStdString, vtkStringArray* > stringArrayMap;
	vtkstd::map < vtkStdString, vtkStringArray* > labelArrayMap;
	vtkstd::set < vtkStdString > arrays;
	//**************************************************************
	void split(const vtkStdString& str,
		vector<vtkStdString>& tokens,
		const vtkStdString& delimiters = " ",
		const vtkStdString& noDataVal = " ")
	{
		if( !str.length() )
			return;
		
		tokens.clear();
		// Skip delimiters at beginning.
		vtkStdString::size_type lastPos = 0;
		// Find first "non-delimiter".
		vtkStdString::size_type pos     = str.find_first_of(delimiters, lastPos);
		std::string temp_str;
		while (vtkStdString::npos != pos)
		{
				// Found a token, add it to the vector.
				temp_str= str.substr(lastPos, pos - lastPos);
				tokens.push_back( trim (temp_str) );
				// Skip delimiters.  Note the "not_of"
				lastPos = pos+1;
				// Find next "non-delimiter"
				pos = str.find_first_of(delimiters, lastPos);
		}

		if(str.substr(lastPos, str.length()-lastPos).length() == 0)
		{
			tokens.push_back( noDataVal );
		}
		else
		{					
			temp_str= str.substr(lastPos, str.length()-lastPos);
			tokens.push_back( trim(temp_str) );
		}
}

	// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

// trim from start
static inline std::string &ltrim(std::string &str) 
	{
	size_t startpos = str.find_first_not_of(" \t");
	if( string::npos != startpos )
		{
    str = str.substr( startpos );
		}
	return str;
	}	

// trim from end
static inline std::string &rtrim(std::string &str) 
	{
  size_t endpos = str.find_last_not_of(" \t");
	if( string::npos != endpos )
		{
    str = str.substr( 0, endpos+1 );
		}
	return str;
	}


};

//------------------------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPointSetReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkPointSetReader);

// Constructor
vtkPointSetReader::vtkPointSetReader()
{
  this->FileName = NULL;
	this->Properties = NULL;
	this->Separator = NULL;
	this->NoDataValue = NULL;
	this->x1 = NULL;
	this->y1 = NULL;
	this->z1 = NULL;

	this->Internal = new LNR_Internals();

	this->SetNumberOfInputPorts(0);
};

//----------------------------------------------------------------------------
// Destructor
vtkPointSetReader::~vtkPointSetReader()
{
  this->SetFileName(0);
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkPointSetReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

void freeMap(charArraymap& mymap)
{
	charArraymapit it;
	if(!mymap.empty())
	{
		it = mymap.begin();
		while(it!=mymap.end())
		{
			if((*it).second)
			delete (*it).second;
			it++;
		}
	}
}
bool vtkPointSetReader::validateLine(vtkStdString line)
{
	charArraymap mymap;
	char* tempLine= new char[line.length()];
	strcpy(tempLine,line.c_str());
	vtkStdString temp;
	charArraymapit it;
	
	for(int i=0; i<line.length();i++)
	{
		char* element = new char[2];
		element[0] = tempLine[i];
		element[1] = '\0';
		temp= element;
		delete[] element;
		if( !this->isNumeric(temp))
		{
				it= mymap.begin();
				it = mymap.find(temp);
				if(it != mymap.end())
				{
					if( (*(*it).second) >=2)
					{
						freeMap(mymap);
						return true;
					}
					else (*(*it).second)++;
				}	
				else 
				{
					it= mymap.begin();
					int* value= new int;
					*value= 1;
					mymap.insert (it, pair<vtkStdString,int*>(temp,value)); 
				}
		}
	}
  
	if(!mymap.empty())
	{
		it = mymap.begin();
		while(it!=mymap.end())
		{
			if((*(*it).second)>=2)
			{		
				freeMap(mymap);
				return true;
			}
			it++;
		}
	
		freeMap(mymap);
	}

	return false;
}
// --------------------------------------
int vtkPointSetReader::CanReadFile( const char* fname)
{
	int result=0;

	if (fname == NULL || fname == "" || fname == " ")
		return 0;
	char* tmpName = const_cast<char *>(fname);
	
	//load the File
	ifstream myFile;
	myFile.open(tmpName);

	if(myFile)
		{

		vtkstd::string line;
		vector<vtkStdString> lineSplit;
		getline(myFile, line);
		getline(myFile, line);				
		result =  validateLine(line);
	   }

	myFile.close();
  return result;  
}


//----------------------------------------------------------------------------
int vtkPointSetReader::RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	return 1;
}


void vtkPointSetReader::SetArrays(const char *a, int s)
{
	vtkStdString arrayName(a);
	if(s)
	{
		this->Internal->arrays.insert(arrayName);
	}
	else
	{
		this->Internal->arrays.erase(arrayName);
	}
	this->Modified();
}
//----------------------------------------------------------------------------
int vtkPointSetReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *realOutput = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Make sure we have a file to read.
  if(!this->FileName)  {
    vtkErrorMacro("A FileName must be specified.");
    return 0;
  }
  if(strlen(this->FileName)==0)  {
    vtkErrorMacro("A NULL FileName.");
    return 0;
  }

	ifstream myFile;
	myFile.open(this->FileName);
	if(!myFile)
	{
		vtkErrorMacro("File Error");
		return 0;
	}

	//this->Internal = new LNR_Internals;

	vtkStdString line;
	vector<vtkStdString> lineSplit;
	vector<vtkStdString> selection; // will hold the properties selected by the user in the gui
	vector<int> indexes; // will hold the indexes of properties in the file
	vector<vtkStdString>::iterator finder;

	vtkPoints *points = vtkPoints::New();
	vtkCellArray *verts = vtkCellArray::New();

	getline(myFile, line);
	this->Internal->split(line,lineSplit,this->Separator,this->NoDataValue);
	unsigned int numberOfHeaders = lineSplit.size();

	// Handle "empty" separator
	if(this->Separator == "")
	{
		points->Delete();
		verts->Delete();
		myFile.close();
		this->Internal->labelArrayMap.clear();
		this->Internal->doubleArrayMap.clear();
		this->Internal->stringArrayMap.clear();
		delete this->Internal;
		vtkErrorMacro("Empty separator detected. Stopping.");
		return 0;
	}

	/*if(this->Properties)
	{
		this->Internal->split(this->Properties,selection,this->Separator,this->NoDataValue);
	}*/

	// iterate on selected properties, find the position
	// of each property in the file and add this position
	// to the vector indexes
	for(vtkstd::set<vtkStdString>::iterator it = this->Internal->arrays.begin();
		it != this->Internal->arrays.end(); it++)
	{
		selection.push_back(*it);
		finder = find(lineSplit.begin(), lineSplit.end(), *it);
		indexes.push_back( finder - lineSplit.begin() );
	}

	// Handle escape characters in separator (i.e. "\," )
	if(indexes.size() > lineSplit.size())
	{
		points->Delete();
		verts->Delete();
		myFile.close();
		this->Internal->labelArrayMap.clear();
		this->Internal->doubleArrayMap.clear();
		this->Internal->stringArrayMap.clear();
		delete this->Internal;
		vtkErrorMacro("Referencing non-existant property. Stopping.");
		return 0;
	}

	int x1Pos, y1Pos, z1Pos;

	// find the position of each point coordinate in the file
	finder = find(lineSplit.begin(), lineSplit.end(), this->x1);
	x1Pos = finder - lineSplit.begin();
	
  finder = find(lineSplit.begin(), lineSplit.end(), this->y1);
	y1Pos = finder - lineSplit.begin();
	
	finder = find(lineSplit.begin(), lineSplit.end(), this->z1);
	z1Pos = finder - lineSplit.begin();
	
	double currentDouble;
	vtkStdString currentString;
	vtkStdString currentProp;
	vtkStdString name;

	getline(myFile, line);
	this->Internal->split(line,lineSplit,this->Separator,this->NoDataValue);
	for( unsigned int i=0; i<selection.size(); i++)
	{
		unsigned int pos = indexes[i];
		if ( pos >= lineSplit.size() || pos < 0 )
			{
			points->Delete();
			verts->Delete();
			myFile.close();
			this->Internal->labelArrayMap.clear();
			this->Internal->doubleArrayMap.clear();
			this->Internal->stringArrayMap.clear();
			delete this->Internal;
			vtkErrorMacro("File has changed since you last loaded it. Stopping.");
			return 1;
			}


		currentString = lineSplit[indexes[i]];
		currentProp = selection[i];

		name = currentProp+" Label";
		this->Internal->labelArrayMap[currentProp] = vtkStringArray::New();
		this->Internal->labelArrayMap[currentProp]->SetName(name.c_str());

		if( currentString != "" && this->isNumeric( currentString ) )
		{
			currentDouble =  atof(currentString.c_str());
			this->Internal->doubleArrayMap[currentProp] = vtkDoubleArray::New();
			this->Internal->doubleArrayMap[currentProp]->SetName(currentProp.c_str());
		}
		else
		{
			if(currentString == "")
			{
				currentString = this->NoDataValue;
			}
			this->Internal->stringArrayMap[currentProp] = vtkStringArray::New();
			this->Internal->stringArrayMap[currentProp]->SetName(currentProp.c_str());
		}		
	}

	double point1[3];
	vtkIdType pointCounter = 0;
	vtkIdType numberOfIncompleteLines = 0;

	myFile.clear();
	myFile.seekg(0,ios::beg);

	double lenghtMin = VTK_DOUBLE_MAX;

	getline(myFile, line);

	while (!myFile.eof())
	{
		getline(myFile, line);
		this->Internal->split(line,lineSplit,this->Separator,this->NoDataValue);

		numberOfIncompleteLines++;
		if(lineSplit.size() < numberOfHeaders)
		{
			vtkWarningMacro("Line number "<<numberOfIncompleteLines<<" is incomplete and will be ignored");
			continue;
		}


		for( unsigned int i=0; i<selection.size(); i++)
		{
			currentString = lineSplit[indexes[i]];
			if( this->Internal->doubleArrayMap.find(selection[i]) != this->Internal->doubleArrayMap.end() )
			{
				currentDouble =  atof(currentString.c_str());
				this->Internal->doubleArrayMap[selection[i]]->InsertNextValue(currentDouble);
			}
			else
			{
				this->Internal->stringArrayMap[selection[i]]->InsertNextValue(currentString);
			}
		}

		// Check for empty x coordinate
		if(lineSplit[x1Pos] == "" && this->isNumeric(this->NoDataValue))
		{
			point1[0] = atof(this->NoDataValue);
		}
		else
		{
			point1[0] = atof(lineSplit[x1Pos].c_str());
		}

		// Check for empty y coordinate
		if(lineSplit[y1Pos] == "" && this->isNumeric(this->NoDataValue))
		{
			point1[1] = atof(this->NoDataValue);
		}
		else
		{
			point1[1] = atof(lineSplit[y1Pos].c_str());
		}
		
		// Check for empty z coordinate
		if(lineSplit[z1Pos] == "" && this->isNumeric(this->NoDataValue))
		{
			point1[2] = atof(this->NoDataValue);
		}
		else
		{
			point1[2] = atof(lineSplit[z1Pos].c_str());
		}
		

		//point1[0] = atof(lineSplit[x1Pos].c_str());
		//point1[1] = atof(lineSplit[y1Pos].c_str());
		//point1[2] = atof(lineSplit[z1Pos].c_str());

		points->InsertNextPoint(point1);

		verts->InsertNextCell(1);
		verts->InsertCellPoint(pointCounter++);
	}

	realOutput->SetPoints(points);
	realOutput->SetVerts(verts);
	

	for(map<vtkStdString,vtkStringArray*>::iterator it = this->Internal->labelArrayMap.begin();
	it != this->Internal->labelArrayMap.end(); it++)
	{
		it->second->Delete();
	}

	for(map<vtkStdString,vtkDoubleArray*>::iterator it = this->Internal->doubleArrayMap.begin();
		it != this->Internal->doubleArrayMap.end(); it++)
	{
		realOutput->GetCellData()->AddArray(it->second);
		it->second->Delete();
	}

	for(map<vtkStdString,vtkStringArray*>::iterator it = this->Internal->stringArrayMap.begin();
		it != this->Internal->stringArrayMap.end(); it++)
	{
		realOutput->GetCellData()->AddArray(it->second);
		it->second->Delete();
	}

	myFile.close();
	points->Delete();
	verts->Delete();
	this->Internal->labelArrayMap.clear();
	this->Internal->doubleArrayMap.clear();
	this->Internal->stringArrayMap.clear();
	//delete this->Internal;
	return 1;
}




//----------------------------------------------------------------------------
bool vtkPointSetReader::isNumeric(vtkStdString value)
{
	if( (value.at(0)!='-') &! isdigit(value.at(0)) )
	{
		return false;
	}

	for(unsigned int i=1; i<value.length(); i++)
	{
		if( (!isdigit(value[i])) &! (value[i] == '.') )
		{
			return false;
		}
	}
	return true;
	}

