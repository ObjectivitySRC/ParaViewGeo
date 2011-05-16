
#include "vtkApplySchedule.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkDataSet.h"
#include <math.h>
#include <iostream>
#include <vtkstd/map>
#include <vtkStdString.h>
#include <vtkstd/vector>
#include <vtkstd/set>
#include <vtkstd/algorithm>
#include <stdlib.h>
#include "vtkPPolyDataNormals.h"
#include "vtkCleanPolyData.h"

#include "vtkIntArray.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include<sstream>
#include<fstream> 

#include "TimeConverter.h"


using vtkstd::map;
using vtkstd::vector;
using vtkstd::set;


namespace
{
	// each property with a given name, have a double value
	typedef map < vtkStdString, double > PROPERTIES;

	// each segment with a given name, have a PROPERTIES
	typedef map < vtkStdString, PROPERTIES > SEGMENTS;
}


struct Internal
{
	vtkstd::set < vtkStdString > propertiesNames;

	vector < int > indexes;
	map < vtkStdString, double > uniformPropertiesStep;
	map < vtkStdString, double > linearPropertiesStep;

	// will be used to store the properties parssed from the schedule file
	SEGMENTS segments;

	int ScheduleSegID;

};






void split(const vtkStdString& str,vector<vtkStdString>& tokens,const vtkStdString& delimiters);
void stringToLower(vtkStdString &word);
vtkStdString intToString(int in);

vtkCxxRevisionMacro(vtkApplySchedule, "$Revision: 1.34 $");
vtkStandardNewMacro(vtkApplySchedule);


//----------------------------------------------------------------------------
vtkApplySchedule::vtkApplySchedule()
{
	this->fileName = NULL;
	this->dateFormat_0 = 0;
	this->dateFormat_1 = 0;
	this->dateFormat_2 = 0;
	this->durationUnits = 0;
	this->durationFactor = 0.0;
	this->DurationPropertyName = NULL;
	this->finishDatePropertyName = NULL;
	this->ScheduleSegName = NULL;
	this->SegmentID = NULL;
	this->DateType = 0;
	this->resetProperties = false;

	this->internals = new Internal;
}


//----------------------------------------------------------------------------
vtkApplySchedule:: ~vtkApplySchedule()
{
	delete this->internals;
}



//----------------------------------------------------------------------------
void vtkApplySchedule::SetArraySelection(const char*name, int uniform, int time, int clone)
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
void vtkApplySchedule::parseProperties(ifstream& myFile)
{
	vector < vtkStdString > dateTester;
	vtkStdString propName, segName;

	vtkStdString line;
	vector<vtkStdString> lineSplit;

	int index;

	getline(myFile, line);
	while( !(myFile.eof() || line == "1,Dependencies") )
	{
		split(line,lineSplit,",");
		index = 0;
		PROPERTIES properties;

		for(set<vtkStdString>::iterator it = this->internals->propertiesNames.begin();
			it != this->internals->propertiesNames.end(); it++ )
		{
			propName = it->substr(0,it->length()-1);

			split(lineSplit[this->internals->indexes[index]], dateTester, "-");
			if(dateTester.size() == 3)
			{
				properties[propName] = this->convertDate(lineSplit[this->internals->indexes[index]], "-");
			}
			else
			{
				split(lineSplit[this->internals->indexes[index]], dateTester, "/");
				if(dateTester.size() == 3)
				{
					properties[propName] = this->convertDate(lineSplit[this->internals->indexes[index]], "/");
				}
				else
				{
					properties[propName] = atof(lineSplit[this->internals->indexes[index]].c_str());
				}
			}

			index++;
		}
		// remove the quotes and white spaces from the segment name 
		// and insert the new segment  in the map
		segName = lineSplit[this->internals->ScheduleSegID];
		if(segName.at(0) == '"')
		{
			segName = segName.substr(1, segName.length()-2);
		}

		this->removeWhiteSpaces(segName);

		this->internals->segments[segName] = properties;

		getline(myFile, line);
	}
}



//----------------------------------------------------------------------------
int vtkApplySchedule::RequestData(vtkInformation *vtkNotUsed(request), 
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	this->resetProperties = true;

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  output = vtkDataSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

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


	
	if(this->durationUnits < 0)
	{
		this->durationFactor = -1.0/this->durationUnits;
	}
	else
		this->durationFactor = static_cast<double>(this->durationUnits);

	int index;

	// the name of the segment id property in the schedule file
	vtkStdString scheduleSegName(this->ScheduleSegName);

	vtkStdString line;
	vector<vtkStdString> lineSplit;
	vector<vtkStdString>::iterator iter;



	vtkStdString propName;

	// if the extention of the file is exf
	// skip some lines
	split(this->fileName, lineSplit, ".");
	if(lineSplit[lineSplit.size()-1] == "exf")
	{
		while(!myFile.eof())
		{
			getline(myFile, line);
			if(line == "1,Tasks")
			{
				break;
			}
		}
	}

	getline(myFile, line);
	split(line,lineSplit,",");

	for(set<vtkStdString>::iterator it = this->internals->propertiesNames.begin();
		it != this->internals->propertiesNames.end(); it++)
	{
		propName = it->substr(0,it->length()-1);

		// each index in the vector "indexes" will correpond to the position
		// of one property in the vector lineSplit
		iter = find(lineSplit.begin(), lineSplit.end(), propName);
		index = iter - lineSplit.begin();
		this->internals->indexes.push_back(index);

		// if the last letter in the property name is
		// 0 -> the repartition of the property on a given segment is uniform
		// 1 -> the repartition of the property is linear

		if( it->at(it->length()-1) == '0' )
		{
			this->internals->uniformPropertiesStep[propName] = 0; // just add it to the map for now
		}
		else if( it->at(it->length()-1) == '1' )
		{
			this->internals->linearPropertiesStep[propName] = 0; // just add it to the map for now
		}	
	}

	iter = find(lineSplit.begin(), lineSplit.end(), scheduleSegName);
	this->internals->ScheduleSegID = iter - lineSplit.begin();



	this->parseProperties(myFile);

	this->distributeProperties();

  return 1;
}





//----------------------------------------------------------------------------
void vtkApplySchedule::distributeProperties()
{
	vtkStdString propName;

	map < vtkStdString, vtkDoubleArray* > Arrays;

	// creating and initializing the output arrays
	for(set<vtkStdString>::iterator it = this->internals->propertiesNames.begin();
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
		return;
	}

	vtkStdString currentSegment;
	vtkStdString lastSegment;
	vtkIdType i=0;
	vtkIdType j = 0;
	vtkIdType n=1;
	double durationStep;
	vtkStdString durationName(this->DurationPropertyName);

	vtkIdType numberOfValue;



	if(segArray1)
		numberOfValue = segArray1->GetNumberOfValues();
	else
		numberOfValue = segArray2->GetNumberOfTuples();

	while(i < numberOfValue)
	{
		if(segArray1)
		{
			currentSegment = segArray1->GetValue(i);

			// remove white spaces
			this->removeWhiteSpaces(currentSegment);
		}
		else
			currentSegment = intToString(segArray2->GetComponent(i,0));


		lastSegment = currentSegment;

		// if linear or uniform properties repartition is used
		// we have to find the number of cell in the current segment
		if(this->internals->linearPropertiesStep.size() || this->internals->uniformPropertiesStep.size())
		{
			j = i+1;

			while(j < numberOfValue)
			{
				if(segArray1)
				{
					currentSegment = segArray1->GetValue(j);
					this->removeWhiteSpaces(currentSegment);
				}
				else
					currentSegment = intToString(segArray2->GetComponent(j,0));

				if(currentSegment != lastSegment)
				{
					break;
				}

				j++;
			}
			n = j-i;

			for (map < vtkStdString, double >::iterator it = this->internals->linearPropertiesStep.begin();
				it != this->internals->linearPropertiesStep.end(); it++)
			{
				durationStep = this->durationFactor*(this->internals->segments[lastSegment][durationName]/n);
				it->second = durationStep;
			}

			for (map < vtkStdString, double >::iterator it = this->internals->uniformPropertiesStep.begin();
				it != this->internals->uniformPropertiesStep.end(); it++)
			{
				it->second = this->internals->segments[lastSegment][it->first]/n;
			}

		}

		for(int counter=0 ; counter<n; counter++)
		{
			for( map < vtkStdString, vtkDoubleArray* >::iterator it = Arrays.begin();
				it != Arrays.end(); it++)
			{
				// if the segment contains only one cell, there is no properties repartition
				if(n == 1)
				{
					it->second->InsertNextValue(this->internals->segments[lastSegment][it->first]);
				}
				else
				{
					// if the repartition is linear
					map < vtkStdString, double >::iterator itt = this->internals->linearPropertiesStep.find(it->first);
					if(itt != this->internals->linearPropertiesStep.end())
					{
						it->second->InsertNextValue(this->internals->segments[lastSegment][it->first] + (counter* itt->second));
						continue;
					}

					// if the repartition is uniform
					itt = this->internals->uniformPropertiesStep.find(it->first);
					if(itt != this->internals->uniformPropertiesStep.end())
					{
						it->second->InsertNextValue(itt->second);
						continue;
					}
					// if there is no repartition
					it->second->InsertNextValue(this->internals->segments[lastSegment][it->first]);
				}
			}
		}
		i = j; // go to next segment
	}


	output->ShallowCopy(input);


	for( map < vtkStdString, vtkDoubleArray* >::iterator it = Arrays.begin();
		it != Arrays.end(); it++)
	{
		output->GetCellData()->AddArray(it->second);
		it->second->Delete();
	}
}







//----------------------------------------------------------------------------
void vtkApplySchedule::PrintSelf(ostream& os, vtkIndent indent)
{
}



//----------------------------------------------------------------------------
double vtkApplySchedule::convertDate(vtkStdString date, vtkStdString separator)
{
	vector <vtkStdString> lineSplit;

	//this->removeWhiteSpaces(date);

	split(date, lineSplit, " ");
	date = lineSplit[0];

	TimeConverter timeC = TimeConverter();

	if(lineSplit.size() == 2)
	{
		vtkStdString time(lineSplit[1]); 
		split(time, lineSplit, ":");
		timeC.setTime(atoi(lineSplit[0].c_str()), atoi(lineSplit[1].c_str()), 0);
	}

	split(date, lineSplit, separator);


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
		timeC.setDay(atoi(lineSplit[2].c_str()));
		break;
	case 30:
		timeC.setMonth(atoi(lineSplit[2].c_str()));
		break;
	case 365:
		timeC.setYear(atoi(lineSplit[2].c_str()));
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
int vtkApplySchedule::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}


//---------------------------------------------------------------------------------------------
void stringToLower(vtkStdString &word)
{
	for(vtkStdString::iterator it= word.begin(); it != word.end(); it++)
	{
		*it = tolower(*it);
	}
}


//---------------------------------------------------------------------------------------------
void split(const vtkStdString& str,
                      vector<vtkStdString>& tokens,
                      const vtkStdString& delimiters = " ")
{
		tokens.clear();
    // Skip delimiters at beginning.
    vtkStdString::size_type lastPos = 0;
    // Find first "non-delimiter".
    vtkStdString::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (vtkStdString::npos != pos)
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
vtkStdString intToString(int in)
{
	std::stringstream out;				
	out << in;				
	return out.str();
}
//---------------------------------------------------------------------------------------------