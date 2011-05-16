#include <vtkstd/map>
#include <vtkstd/vector>
#include "vtkDOTReader.h"
#include <fstream>
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPointSource.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkUnsignedIntArray.h"
#include "vtkPointData.h"


struct DOT_Internal
{

	//-----------------------------------------------------------
	void split(const vtkStdString& str,
		vtkstd::vector<vtkStdString>& tokens,
												const vtkStdString& delimiters = " ")
	{
		if( !str.length() )
			return;

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
	void splitSkipEmpty(const vtkStdString& str,
			vtkstd::vector<vtkStdString>& tokens,
													const vtkStdString& delimiters = " ")
	{
			tokens.clear();

			// Skip delimiters at beginning.
			vtkStdString::size_type lastPos = str.find_first_not_of(delimiters, 0);
			// Find first "non-delimiter".
			vtkStdString::size_type pos     = str.find_first_of(delimiters, lastPos);

			while (vtkStdString::npos != pos || vtkStdString::npos != lastPos)
			{
					// Found a token, add it to the vector.
					tokens.push_back(str.substr(lastPos, pos - lastPos));
					// Skip delimiters.  Note the "not_of"
					lastPos = str.find_first_not_of(delimiters, pos);
					// Find next "non-delimiter"
					pos = str.find_first_of(delimiters, lastPos);
			}
	}


	//----------------------------------------------------------------------------
	int readLine(ifstream &file, vtkStdString &line, vtkstd::vector<vtkStdString> &lineSplit)
	{
		getline(file, line);
		this->splitSkipEmpty(line, lineSplit, ",");
		if(lineSplit.size() < 4 && !file.eof())
		{
			this->readLine(file, line, lineSplit);
		}

		if(lineSplit.size() < 4)
		{
			return 0;
		}
		else if(lineSplit.size() == 4)
		{
			return 1;
		}
		else
		{
			return 2;
		}

	}

};


vtkCxxRevisionMacro(vtkDOTReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkDOTReader);



//----------------------------------------------------------------------------
vtkDOTReader::vtkDOTReader()
{
	this->FileName = NULL;
	this->FromServer = NULL;

  this->Center[0] = 0.0;
  this->Center[1] = 0.0;
  this->Center[2] = 0.0;

  //this->Center1[0] = 5;
  //this->Center1[1] = 5;
  //this->Center1[2] = 5;

	this->SetNumberOfInputPorts(0);
}

//----------------------------------------------------------------------------
vtkDOTReader::~vtkDOTReader()
{
	this->SetFileName(0);
}


//----------------------------------------------------------------------------

void vtkDOTReader::SetCenter1(double* c)
{
	int i=0;
	while(c[i])
	{
		vtkWarningMacro(""<<c[i]);
		i++;
	}
}


//----------------------------------------------------------------------------
//void vtkDOTReader::SetCenter1(vtkSMDoubleVectorProperty* v)
//{
//	vtkWarningMacro(""<< v->GetNumberOfElements());
//}

//----------------------------------------------------------------------------
void vtkDOTReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}



//----------------------------------------------------------------------------
int vtkDOTReader::RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	return 1;
}



//----------------------------------------------------------------------------
int vtkDOTReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{


	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

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

	ifstream myFile;
	myFile.open(this->FileName);
	if(!myFile)
	{
		vtkErrorMacro("File Error");
		return 0;
	}

	this->Internal = new DOT_Internal;

	//vtkstd::map <  vtkStdString, vtkstd::vector < double >  >  sourcePoints;
	vtkStdString sourcePoints;

	vtkStdString line;
	vtkstd::vector<vtkStdString> lineSplit;

	vtkPoints* points = vtkPoints::New();
	vtkCellArray *lines = vtkCellArray::New();

	vtkIdType pointCounter = 0;
	unsigned int declineId = 0;
	vtkUnsignedIntArray *declineIdsArray = vtkUnsignedIntArray::New();
	declineIdsArray->SetName("ID");

	switch(this->Internal->readLine(myFile, line, lineSplit))
	{
	case 0:
		vtkErrorMacro("the file is empty");
		break;
	case 2:
		sourcePoints += line;
		sourcePoints += "|";
		declineId++;
	default:
		points->InsertNextPoint(atof(lineSplit[1]), atof(lineSplit[2]), atof(lineSplit[3]) );
		declineIdsArray->InsertNextValue(declineId);
	}

	int ok;
	while(!myFile.eof())
	{
		ok = this->Internal->readLine(myFile, line, lineSplit);
		if(!ok)
		{
			break;
		}
		else if(ok == 2)
		{
			sourcePoints += line;
			sourcePoints += "|";
			declineId++;
			points->InsertNextPoint(atof(lineSplit[1]), atof(lineSplit[2]), atof(lineSplit[3]) );
			declineIdsArray->InsertNextValue(declineId);
		}
		else
		{
			points->InsertNextPoint(atof(lineSplit[1]), atof(lineSplit[2]), atof(lineSplit[3]) );
			declineIdsArray->InsertNextValue(declineId);
			lines->InsertNextCell(2);
			lines->InsertCellPoint(pointCounter++);
			lines->InsertCellPoint(pointCounter);
		}

	}

	this->FromServer = new char[sourcePoints.length()];
	strcpy(this->FromServer,sourcePoints.c_str());

	output->SetPoints(points);
	output->SetLines(lines);
	output->GetPointData()->AddArray(declineIdsArray);

	points->Delete();
	lines->Delete();
	declineIdsArray->Delete();


	delete this->Internal;

	return 1;
}



