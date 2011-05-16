/*Dips File reader By Tim Anema*/

#include "vtkDipsReader.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

vtkCxxRevisionMacro(vtkDipsReader, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDipsReader);

class vtkInternalParse
{
public:
	//----------------------------------------------------------------------------
	int readLine(ifstream &file, vtkStdString &line, vtkstd::vector<vtkStdString> &lineSplit, const vtkStdString& delimiters = " ",unsigned int minLenght = 1)
	{
		getline(file, line);
		this->split(line, lineSplit, delimiters);

		if(file.eof())
		{
			return 0;
		}
		
		if(lineSplit.size() < minLenght || lineSplit.at(0).c_str()[0] == '*')
			{
				this->readLine(file, line, lineSplit, delimiters, minLenght);
			}

		return 1;
	}

private:
	//-----------------------------------------------------------
	void split(const vtkStdString& str, vtkstd::vector<vtkStdString>& tokens, const vtkStdString& delimiters = " ")
	{
		tokens.clear();

		if( !str.length() )
			{
			return;
			}

		// Skip delimiters at beginning.
		vtkStdString::size_type lastPos = str.find_first_not_of(delimiters, 0);
		// Find first "non-delimiter".
		vtkStdString::size_type pos = str.find_first_of(delimiters, lastPos);

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

};

//----------------------------------------------------------------------------
vtkDipsReader::vtkDipsReader()
{
  this->FileName = 0;
	this->FileTitle = vtkStdString();
	this->numofTraverse = 0;
	this->GlobalOrient = 0;
	this->Declination = 0;
	this->Quantity = true;

  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkDipsReader::~vtkDipsReader()
{
  this->SetFileName(0);
}

//----------------------------------------------------------------------------
int vtkDipsReader::CanReadFile(const char *fname)
{
  return 1;
}

//----------------------------------------------------------------------------
int vtkDipsReader::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // Make sure we have a file to read.
	if(!this->FileName)  {
		vtkErrorMacro("No file name specified.  Cannot open.");
		return 0;
	}
	if(strlen(this->FileName)==0)  {
		vtkErrorMacro("File name is null.  Cannot open.");
		return 0;
	}
	
	ifstream file;
	file.open(this->FileName, ios::in);
	if(!file)
	{
		vtkErrorMacro("File Error: cannot open file: "<< this->FileName);
		return 0;
	}

	vtkStdString line;
	std::vector<vtkStdString> lineSplit;

	int result = this->parseHeaderData(file);

	while(this->Internal->readLine(file, line, lineSplit))
	{
		if(line.compare("-1") == 0)
			{
			file.close();
			return 1;
			}
	}

	vtkErrorMacro("Unexpected End of File.");

	file.close();
  return result;
}

//----------------------------------------------------------------------------
int vtkDipsReader::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// Make sure we have a file to read.
	if(!this->FileName)  {
		vtkErrorMacro("No file name specified.  Cannot open.");
		return 0;
	}
	if(strlen(this->FileName)==0)  {
		vtkErrorMacro("File name is null.  Cannot open.");
		return 0;
	}
	
	ifstream file;
	file.open(this->FileName, ios::in);
	if(!file)
	{
		vtkErrorMacro("File Error: cannot open file: "<< this->FileName);
		return 0;
	}

	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkCellArray *polys = vtkCellArray::New();
	vtkCellArray *lines = vtkCellArray::New();
	vtkPoints *points = vtkPoints::New();

	int result = this->parseHeaderData(file) && this->parseData(file, polys, lines, points);

	output->SetLines( polys );
	output->SetVerts(lines);
	output->SetPoints( points );
	
	polys->Delete();
	lines->Delete();
	points->Delete();

	file.close();

  return result;
}

//----------------------------------------------------------------------------
bool vtkDipsReader::parseHeaderData( ifstream &file )
{
	vtkStdString line;
	std::vector<vtkStdString> lineSplit;

	//Title of File
	this->Internal->readLine(file, line, lineSplit);
	this->FileTitle = line;
	this->Internal->readLine(file, line, lineSplit);
	this->FileTitle.append(" : " + line);

	//number of traverse
	this->Internal->readLine(file, line, lineSplit);
	if(isdigit(lineSplit.at(0).c_str()[0]))
		{
		this->numofTraverse = atoi(lineSplit.at(0).c_str());
		}
	else
		{
		this->numofTraverse = atoi(lineSplit.at(lineSplit.size()-1).c_str());
		}
	
	//Traverse definitions
	for(int i = 0; i < this->numofTraverse; i++)
		{
		this->Internal->readLine(file, line, lineSplit);
		}

	//Global Orientation
	this->Internal->readLine(file, line, lineSplit);
	this->GlobalOrient = this->GetGlobalFormat(lineSplit.at(0));
	if(this->GlobalOrient == -1)
		{
		return false;//no global orient found
		}

	//Declination
	this->Internal->readLine(file, line, lineSplit);
	this->Declination = atof(lineSplit.at(0).c_str());

	//Quantity
	this->Internal->readLine(file, line, lineSplit);
	this->Quantity = lineSplit.at(0).compare("NO") == 0 ? false : true;

	//Extra Columns
	this->Internal->readLine(file, line, lineSplit);
	if(isdigit(lineSplit.at(0).c_str()[0]))
		{
		this->ExtraColumns = atoi(lineSplit.at(0).c_str());
		}
	else
		{
		this->ExtraColumns = atoi(lineSplit.at(lineSplit.size()-1).c_str());
		}

	return true;
}

//----------------------------------------------------------------------------
bool vtkDipsReader::parseData(ifstream &file, vtkCellArray* planes, vtkCellArray* lines, vtkPoints* points)
{
	return 1;
}

//http://www.rocscience.com/downloads/dips/WebHelp/dips/Global_Orientation_Format.htm
//----------------------------------------------------------------------------
int vtkDipsReader::GetGlobalFormat(vtkStdString format)
{
	if(format.compare("DIP/DIPDIRECTION") == 0)
		{
		return 0;
		}
	if(format.compare("STRIKE/DIPR") == 0)
		{
		return 1;
		}
	if(format.compare("STRIKE/DIPL") == 0)
		{
		return 2;
		}
	if(format.compare("TREND/PLUNGE") == 0)
		{
		return 3;
		}
	return -1;
}

//----------------------------------------------------------------------------
void vtkDipsReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
