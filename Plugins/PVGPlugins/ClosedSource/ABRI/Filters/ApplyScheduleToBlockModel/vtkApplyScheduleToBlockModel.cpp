
#include "vtkApplyScheduleToBlockModel.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkUnstructuredGrid.h"
#include <math.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
//#include <conio.h>
//#include <windows.h>
#include <stdlib.h>
#include "vtkPPolyDataNormals.h"
#include "vtkCleanPolyData.h"
#include "vtkStringArray.h"
#include "vtkIntArray.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include<sstream>
#include<fstream> 

#include "TimeConverter.h"

using namespace std;


struct Internal
{
	vtkstd::set < string > propertiesNames;

};






void split(const string& str,vector<string>& tokens,const string& delimiters);
void stringToLower(string &word);
string intToString(int in);

vtkCxxRevisionMacro(vtkApplyScheduleToBlockModel, "$Revision: 1.34 $");
vtkStandardNewMacro(vtkApplyScheduleToBlockModel);


//----------------------------------------------------------------------------
vtkApplyScheduleToBlockModel::vtkApplyScheduleToBlockModel()
{
	this->fileName = NULL;
	this->dateFormat_0 = 0;
	this->dateFormat_1 = 0;
	this->dateFormat_2 = 0;
	this->durationUnits = 0;
	this->finishDatePropertyName = NULL;
	this->ScheduleSegName = NULL;
	this->SegmentID = NULL;
	this->DateType = 0;
	this->resetProperties = false;

	this->internals = new Internal;
}


//----------------------------------------------------------------------------
vtkApplyScheduleToBlockModel:: ~vtkApplyScheduleToBlockModel()
{
	delete this->internals;
}



//----------------------------------------------------------------------------
void vtkApplyScheduleToBlockModel::SetArraySelection(const char*name, int uniform, int time, int clone)
{
	if(this->resetProperties)
	{
		this->internals->propertiesNames.clear();
		this->resetProperties = false;
	}
	int index = -1;

	if(uniform > 0)
	{
		index = 0;
	}
	else if(time > 0)
	{
		index = 1;
	}
	else if(clone > 0)
	{
		index = 2;
	}

	if(index >= 0)
	{
		vtkStdString arrayName(name);
		this->internals->propertiesNames.insert(arrayName + intToString(index));
	}

	this->Modified();
}

//----------------------------------------------------------------------------
int vtkApplyScheduleToBlockModel::RequestData(vtkInformation *vtkNotUsed(request), 
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	this->resetProperties = true;

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	//string selection(this->ArraySelection);
	//if(selection == "")
	//{
		//vtkErrorMacro("you must select one or more properties");
		//return 1;
	//}

  // Make sure we have a file to read.
 	if(!this->fileName)  
	{
    vtkErrorMacro("A FileName must be specified.");
    return 1;
  }

  if(strlen(this->fileName)==0)  
	{
    vtkErrorMacro("A NULL FileName.");
    return 1;
  }

	ifstream myFile;
	myFile.open(this->fileName);
	if(!myFile)
	{
		vtkErrorMacro("File Error");
		return 1;
	}
	

	// each property with a given name, have a double value
	typedef map < string, double > PROPERTIES;

	// each segment with a given name, have a PROPERTIES
	typedef map < string, PROPERTIES > SEGMENTS;

	// map < segmentId, map < propertyName, propertyValue > > 
	SEGMENTS segments;

	vector < int > indexes;

	int index;
	int ScheduleSegID;
	string scheduleSegName(this->ScheduleSegName);
	string line;
	vector<string> lineSplit;
	vector<string>::iterator iter;

	map < string, double > uniformPropertiesStep;
	map < string, double > linearPropertiesStep;

	string propName;


	//getline(myFile, line);
	getline(myFile, line);
	split(line,lineSplit,",");

	for(set<string>::iterator it = this->internals->propertiesNames.begin();
		it != this->internals->propertiesNames.end(); it++)
	{
		propName = it->substr(0,it->length()-1);

		// each index in the vector "indexes" will correpond to the position
		// of one property in the vector lineSplit
		iter = find(lineSplit.begin(), lineSplit.end(), propName);
		index = iter - lineSplit.begin();
		indexes.push_back(index);
		/*stringToLower(*it);
		stringToLower(propName);*/

		if( it->at(it->length()-1) == '0' )
		{
			uniformPropertiesStep[propName] = 0;
		}
		else if( it->at(it->length()-1) == '1' )
		{
			linearPropertiesStep[propName] = 0;
		}
		
	}

	iter = find(lineSplit.begin(), lineSplit.end(), scheduleSegName);
	ScheduleSegID = iter - lineSplit.begin();


	vector < string > dateTester;

	getline(myFile, line);
	while( !(myFile.eof() || line == "1,Dependencies") )
	{
		split(line,lineSplit,",");
		index = 0;
		PROPERTIES properties;

		for(set<string>::iterator it = this->internals->propertiesNames.begin();
			it != this->internals->propertiesNames.end(); it++ )
		{
			propName = it->substr(0,it->length()-1);

			// check if the current property is a date with "-" separator
			split(lineSplit[indexes[index]], dateTester, "-");
			if(dateTester.size() == 3)
			{
				properties[propName] = this->convertDate(lineSplit[indexes[index]], "-");
			}

			else
			{
				// check if the current property is a date with "/" separator
				split(lineSplit[indexes[index]], dateTester, "/");
				if(dateTester.size() == 3)
				{
					properties[propName] = this->convertDate(lineSplit[indexes[index]], "/");
				}

				else
				{
					// the property is not a date but a double value
					// here am supposing that the use wont select a string property
					properties[propName] = atof(lineSplit[indexes[index]].c_str());
				}

			}

			index++;
		}
		// remove the quotes for the segment name and insert the new segment 
		// in the map

		if(lineSplit[ScheduleSegID][0] == '"')
			segments[lineSplit[ScheduleSegID].substr(1,lineSplit[ScheduleSegID].length()-2)] = properties;
		else
			segments[lineSplit[ScheduleSegID]] = properties;

		getline(myFile, line);
	}


	map < string, vtkDoubleArray* > Arrays;

	// creating and initializing the output arrays
	for(set<string>::iterator it = this->internals->propertiesNames.begin();
		it != this->internals->propertiesNames.end(); it++ )
	{
		propName = it->substr(0,it->length()-1);
		Arrays[propName] = vtkDoubleArray::New();
		Arrays[propName]->SetName(propName.c_str());
	}

	vtkStringArray *segArray1 = vtkStringArray::SafeDownCast(
							input->GetCellData()->GetAbstractArray(this->SegmentID));
	vtkDataArray *segArray2 = input->GetCellData()->GetArray(this->SegmentID);

	if( !(segArray1 || segArray2) )
	{
		vtkErrorMacro("Can't find SEGMENT array in the input data");
		return 1;
	}

	string currentSegment;
	string lastSegment;
	vtkIdType i=0;
	vtkIdType j;
	vtkIdType n=1;
	double durationStep;
	string finishDate(this->finishDatePropertyName);
	//stringToLower(finishDate);
	double finish;

	vtkIdType numberOfValue;

	if(segArray1)
		numberOfValue = segArray1->GetNumberOfValues();
	else
		numberOfValue = segArray2->GetNumberOfTuples();

	while(i < numberOfValue)
	{
		if(segArray1)
			currentSegment = segArray1->GetValue(i);
		else
			currentSegment = intToString(segArray2->GetComponent(i,0));

		lastSegment = currentSegment;

		// if linear or uniform properties repartition is used
		// we have to find the number of cell in the current segment
		if(linearPropertiesStep.size() || uniformPropertiesStep.size())
		{
			j = i+1;
			while(lastSegment == currentSegment)
			{
				if(segArray1)
					currentSegment = segArray1->GetValue(j);
				else
					currentSegment = intToString(segArray2->GetComponent(j,0));

				j++;
			}
			n = j-i-1;

			finish = segments[lastSegment][finishDate];
			for (map < string, double >::iterator it = linearPropertiesStep.begin();
				it != linearPropertiesStep.end(); it++)
			{
				durationStep = (finish - segments[lastSegment][it->first])/n;
				it->second = durationStep;
			}

			for (map < string, double >::iterator it = uniformPropertiesStep.begin();
				it != uniformPropertiesStep.end(); it++)
			{
				it->second = segments[lastSegment][it->first]/n;
			}

		}

		for(int counter=0 ; counter<n; counter++)
		{
			for( map < string, vtkDoubleArray* >::iterator it = Arrays.begin();
				it != Arrays.end(); it++)
			{
				// if the segment contains only one cell, there is no properties repartition
				if(n == 1)
				{
					it->second->InsertNextValue(segments[lastSegment][it->first]);
				}
				else
				{
					// if the repartition is linear
					map < string, double >::iterator itt = linearPropertiesStep.find(it->first);
					if(itt != linearPropertiesStep.end())
					{
						it->second->InsertNextValue(segments[lastSegment][it->first] + (counter* itt->second));
						continue;
					}

					// if the repartition is uniform
					itt = uniformPropertiesStep.find(it->first);
					if(itt != uniformPropertiesStep.end())
					{
						it->second->InsertNextValue(itt->second);
						continue;
					}
					// if there is no repartition
					it->second->InsertNextValue(segments[lastSegment][it->first]);
				}
			}
		}
		i += n; // go to next segment
		
	}

	output->ShallowCopy(input);
	
	//------------------------------------------------------------
	for( map < string, vtkDoubleArray* >::iterator it = Arrays.begin();
		it != Arrays.end(); it++)
	{
		output->GetCellData()->AddArray(it->second);
		it->second->Delete();
	}

  return 1;
}


//----------------------------------------------------------------------------
void vtkApplyScheduleToBlockModel::PrintSelf(ostream& os, vtkIndent indent)
{
}

//----------------------------------------------------------------------------
double vtkApplyScheduleToBlockModel::convertDate(string &date, string separator)
{
	vector <string> lineSplit;
	split(date, lineSplit, separator);

	// remove the time from the date
	string str = lineSplit[2].substr(0, lineSplit[2].find_first_of(" ",0));

  TimeConverter timeC = TimeConverter();
	switch(this->dateFormat_0)
	{
	case 1:
		timeC.setDay(atoi(lineSplit[0].c_str()));
		break;
	case 30:
		timeC.setMonth(atoi(lineSplit[0].c_str()));
		break;
	case 365:
		timeC.setYear(atoi(lineSplit[0].c_str()));
	}

	switch(this->dateFormat_1)
	{
	case 1:
		timeC.setDay(atoi(lineSplit[1].c_str()));
		break;
	case 30:
		timeC.setMonth(atoi(lineSplit[1].c_str()));
		break;
	case 365:
		timeC.setYear(atoi(lineSplit[1].c_str()));
	}

	switch(this->dateFormat_2)
	{
	case 1:
		timeC.setDay(atoi(str.c_str()));
		break;
	case 30:
		timeC.setMonth(atoi(str.c_str()));
		break;
	case 365:
		timeC.setYear(atoi(str.c_str()));
	}

  switch(this->DateType)
    { 
    case VTK_MSDATE:
      return timeC.GetMSDate();  
   
    case VTK_JULIAN:
      return timeC.GetJulianDate();

    case VTK_JULIAN_M:                  
      return timeC.GetModifiedJulianDate();        

     case VTK_MATLAB:                  
      return timeC.GetMatlabDate();          
    }   

	return 0;

}



//----------------------------------------------------------------------------
int vtkApplyScheduleToBlockModel::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}


//---------------------------------------------------------------------------------------------
void stringToLower(string &word)
{
	for(string::iterator it= word.begin(); it != word.end(); it++)
	{
		*it = tolower(*it);
	}
}


//---------------------------------------------------------------------------------------------
void split(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
		tokens.clear();
    // Skip delimiters at beginning.
    string::size_type lastPos = 0;
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = pos+1;
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
		tokens.push_back( str.substr(lastPos, str.length()-lastPos) );
}
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
string intToString(int in)
{
	std::stringstream out;				
	out << in;				
	return out.str();
}
//---------------------------------------------------------------------------------------------