#include "vtkLineNetworkReader.h"

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


using namespace std;

//------------------------------------------------------------------------------------------

struct LNR_Internals
{
	vtkstd::map < vtkStdString, vtkDoubleArray* > doubleArrayMap;
	vtkstd::map < vtkStdString, vtkStringArray* > stringArrayMap;
	vtkstd::map < vtkStdString, vtkStringArray* > labelArrayMap;

	vtkstd::set < vtkStdString > arrays;

	//**************************************************************
	void split(const vtkStdString& str,
												vector<vtkStdString>& tokens,
												const vtkStdString& delimiters = " ")
	{
		tokens.clear();

		if( !str.length() )
			return;
		// Skip delimiters at beginning.
		vtkStdString::size_type lastPos = 0;
		// Find first "non-delimiter".
		vtkStdString::size_type pos     = str.find_first_of(delimiters, lastPos);

		//vars for deleting white spaces
		vtkstd::string tmp;
		size_t start,end;
		while (vtkStdString::npos != pos)
			{
			// Found a token
			tmp = str.substr(lastPos, pos - lastPos);
			 
			//remove leading and trailing white spaces
			start = tmp.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces
			end = tmp.find_last_not_of(" \t"); // Find the first characte			
			tmp = (start == end) ? "" : tmp.substr( start, end-start+1 );			 

			
			//add it to the vector
			tokens.push_back(tmp);
			// Skip delimiters.  Note the "not_of"
			lastPos = pos+1;
			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
			}
		
		tmp = str.substr(lastPos, str.length()-lastPos);
			 
		//remove leading and trailing white spaces
		start = tmp.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces
		end = tmp.find_last_not_of(" \t"); // Find the first characte			
		tmp = (start == end) ? "" : tmp.substr( start, end-start+1 );			 

			
		//add it to the vector
		tokens.push_back(tmp);		
}

};

//------------------------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkLineNetworkReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkLineNetworkReader);

// Constructor
vtkLineNetworkReader::vtkLineNetworkReader()
{
	this->Internal = new LNR_Internals;
  this->FileName = NULL;
	this->ArrowProp = NULL;
	this->x1 = NULL;
	this->y1 = NULL;
	this->z1 = NULL;
	this->x2 = NULL;
	this->y2 = NULL;
	this->z2 = NULL;
	this->ScaleArrowSize = 1;
	this->withTunnels = 0;

	this->SetNumberOfInputPorts(0);
};

//----------------------------------------------------------------------------
// Destructor
vtkLineNetworkReader::~vtkLineNetworkReader()
{
  this->SetFileName(0);
	delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkLineNetworkReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}



//----------------------------------------------------------------------------
int vtkLineNetworkReader::RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	return 1;
}


//----------------------------------------------------------------------------
void vtkLineNetworkReader::SetArrays(const char *a, int s)
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
	//vtkWarningMacro("" << arrayName.c_str() << "  " << s);
	this->Modified();
	
}


//----------------------------------------------------------------------------
int vtkLineNetworkReader::RequestData(vtkInformation* request,
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


	vtkStdString line;
	vector<vtkStdString> lineSplit;
	vector<vtkStdString> selection; // will hold the properties selected by the user in the gui
	vector<int> indexes; // will hold the indexes of properties in the file
	vector<vtkStdString>::iterator finder;

	vtkPoints *points = vtkPoints::New();
	vtkCellArray *lines = vtkCellArray::New();

	getline(myFile, line);
	this->Internal->split(line,lineSplit,",");
	unsigned int numberOfHeaders = lineSplit.size();


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

	int x1Pos, x2Pos, y1Pos, y2Pos, z1Pos, z2Pos;

	// find the position of each point coordinate in the file
	finder = find(lineSplit.begin(), lineSplit.end(), this->x1);
	x1Pos = finder - lineSplit.begin();

	finder = find(lineSplit.begin(), lineSplit.end(), this->y1);
	y1Pos = finder - lineSplit.begin();

	finder = find(lineSplit.begin(), lineSplit.end(), this->z1);
	z1Pos = finder - lineSplit.begin();

	finder = find(lineSplit.begin(), lineSplit.end(), this->x2);
	x2Pos = finder - lineSplit.begin();

	finder = find(lineSplit.begin(), lineSplit.end(), this->y2);
	y2Pos = finder - lineSplit.begin();

	finder = find(lineSplit.begin(), lineSplit.end(), this->z2);
	z2Pos = finder - lineSplit.begin();


	double currentDouble;
	vtkStdString currentString;
	vtkStdString currentProp;
	vtkStdString name;

	getline(myFile, line);
	this->Internal->split(line,lineSplit,",");
	for( unsigned int i=0; i<selection.size(); i++)
	{
		currentString = lineSplit[indexes[i]];
		currentProp = selection[i];

		name = currentProp+" Label";
		this->Internal->labelArrayMap[currentProp] = vtkStringArray::New();
		this->Internal->labelArrayMap[currentProp]->SetName(name.c_str());

		if( this->isNumeric( currentString ) )
		{
			currentDouble =  atof(currentString.c_str());
			this->Internal->doubleArrayMap[currentProp] = vtkDoubleArray::New();
			this->Internal->doubleArrayMap[currentProp]->SetName(currentProp.c_str());
		}
		else
		{
			this->Internal->stringArrayMap[currentProp] = vtkStringArray::New();
			this->Internal->stringArrayMap[currentProp]->SetName(currentProp.c_str());

		}
	}

	double point1[3];
	double point2[3];
	vtkIdType pointCounter = 0;
	vtkIdType numberOfIncompleteLines = 0;

	myFile.clear();
	myFile.seekg(0,ios::beg);

	int ArrowPropPos;
	double currentArrowProp;
	double maxArrowProp = 0;
	double lenght;
	double lenghtMin = VTK_DOUBLE_MAX;

	// if arrow is used, we need to read the file first and get the default size of an arrow
	// corresponding to the data
	if(strcmp(this->ArrowProp,"none")!=0)
	{

		if(this->ScaleArrowSize<0)
		{
			this->ScaleArrowSize = -1/this->ScaleArrowSize;
		}
		getline(myFile, line);
		this->Internal->split(line, lineSplit, ",");

		finder = find(lineSplit.begin(), lineSplit.end(), this->ArrowProp);
		if(finder == lineSplit.end())
		{
			vtkErrorMacro("can find the property "<< this->ArrowProp << " in the file");
			return 1;
		}
		ArrowPropPos = finder - lineSplit.begin();

		while(!myFile.eof())
		{
			getline(myFile, line);
			this->Internal->split(line, lineSplit, ",");

			numberOfIncompleteLines++;
			if(lineSplit.size() < numberOfHeaders)
			{
				vtkWarningMacro("Line number "<<numberOfIncompleteLines<<" is incomplete and will be ignored");
				continue;
			}

			point1[0] = atof(lineSplit[x1Pos].c_str());
			point1[1] = atof(lineSplit[y1Pos].c_str());
			point1[2] = atof(lineSplit[z1Pos].c_str());

			point2[0] = atof(lineSplit[x2Pos].c_str());
			point2[1] = atof(lineSplit[y2Pos].c_str());
			point2[2] = atof(lineSplit[z2Pos].c_str());

			currentArrowProp = atof( (lineSplit[ArrowPropPos]).c_str() );
			if(  currentArrowProp > maxArrowProp  )
			{
				maxArrowProp = currentArrowProp;
			}

			lenght = sqrt(vtkMath::Distance2BetweenPoints(point1,point2));
			if(lenght < lenghtMin)
			{
				lenghtMin = lenght;
			}

		}

		myFile.clear();
		myFile.seekg(0,ios::beg);
	}

	else
	{
		getline(myFile, line);

		while (!myFile.eof())
		{
			getline(myFile, line);
			this->Internal->split(line,lineSplit,",");

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
					//this->Internal->doubleArrayMap[selection[i]]->InsertNextValue(currentDouble);
					this->Internal->doubleArrayMap[selection[i]]->InsertNextValue(currentDouble);
				}
				else
				{
					//this->Internal->stringArrayMap[selection[i]]->InsertNextValue(currentString);
					this->Internal->stringArrayMap[selection[i]]->InsertNextValue(currentString);
				}
			}

			point1[0] = atof(lineSplit[x1Pos].c_str());
			point1[1] = atof(lineSplit[y1Pos].c_str());
			point1[2] = atof(lineSplit[z1Pos].c_str());

			point2[0] = atof(lineSplit[x2Pos].c_str());
			point2[1] = atof(lineSplit[y2Pos].c_str());
			point2[2] = atof(lineSplit[z2Pos].c_str());

			points->InsertNextPoint(point1);
			points->InsertNextPoint(point2);

			lines->InsertNextCell(2);
			lines->InsertCellPoint(pointCounter++);
			lines->InsertCellPoint(pointCounter++);
		}

		realOutput->SetPoints(points);
		realOutput->SetLines(lines);
		

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
		lines->Delete();
		this->Internal->labelArrayMap.clear();
		this->Internal->doubleArrayMap.clear();
		this->Internal->stringArrayMap.clear();
		return 1;

	}


	int numberOfArrowPoints;
	vtkAppendPolyData *append = vtkAppendPolyData::New();

	getline(myFile, line);

	while (!myFile.eof())
	{
		getline(myFile, line);
		this->Internal->split(line,lineSplit,",");
		if(lineSplit.size() < numberOfHeaders)
		{
			continue;
		}

		point1[0] = atof(lineSplit[x1Pos].c_str());
		point1[1] = atof(lineSplit[y1Pos].c_str());
		point1[2] = atof(lineSplit[z1Pos].c_str());

		point2[0] = atof(lineSplit[x2Pos].c_str());
		point2[1] = atof(lineSplit[y2Pos].c_str());
		point2[2] = atof(lineSplit[z2Pos].c_str());

		this->Size = this->ScaleArrowSize*atof((lineSplit[ArrowPropPos].c_str()))*lenghtMin/maxArrowProp;
		if(this->withTunnels)
		{
			numberOfArrowPoints = this->AddTunnel(append, point1, point2, 8);
		}
		else
		{
			numberOfArrowPoints = this->AddArrow(append, point1, point2, 8);
		}


		for( unsigned int i=0; i<selection.size(); i++)
		{
			currentString = lineSplit[indexes[i]];
			if( this->Internal->doubleArrayMap.find(selection[i]) != this->Internal->doubleArrayMap.end() )
			{
				currentDouble =  atof(currentString.c_str());

				// here we have to for loop because we want the label to be displayed only on the cone of
				// the arrow, witch is the third point of the arrow. we can do it in the same for loop
				// with a "if condition" on k=3 but this will slow the execution
				for( int k=0; k<3; k++)
				{
					this->Internal->labelArrayMap[selection[i]]->InsertNextValue("");
					this->Internal->doubleArrayMap[selection[i]]->InsertNextValue(currentDouble);
				}

				this->Internal->labelArrayMap[selection[i]]->InsertNextValue(currentString);
				this->Internal->doubleArrayMap[selection[i]]->InsertNextValue(currentDouble);

				for(int j=4; j<numberOfArrowPoints; j++)
				{
					this->Internal->labelArrayMap[selection[i]]->InsertNextValue("");
					this->Internal->doubleArrayMap[selection[i]]->InsertNextValue(currentDouble);
				}
			}
			else
			{
				for(int k=0; k<3; k++)
				{
					this->Internal->labelArrayMap[selection[i]]->InsertNextValue("");
					this->Internal->stringArrayMap[selection[i]]->InsertNextValue(currentString);
				}
				this->Internal->labelArrayMap[selection[i]]->InsertNextValue(currentString);
				this->Internal->stringArrayMap[selection[i]]->InsertNextValue(currentString);

				for(int j=4; j<numberOfArrowPoints; j++)
				{
					this->Internal->labelArrayMap[selection[i]]->InsertNextValue("");
					this->Internal->stringArrayMap[selection[i]]->InsertNextValue(currentString);
				}
			}
		}
	}

  append->Update();
	realOutput->DeepCopy(append->GetOutput());

	for(map<vtkStdString,vtkDoubleArray*>::iterator it = this->Internal->doubleArrayMap.begin();
		it != this->Internal->doubleArrayMap.end(); it++)
	{
		realOutput->GetPointData()->AddArray(it->second);
		it->second->Delete();
	}

	for(map<vtkStdString,vtkStringArray*>::iterator it = this->Internal->stringArrayMap.begin();
		it != this->Internal->stringArrayMap.end(); it++)
	{
		realOutput->GetPointData()->AddArray(it->second);
		it->second->Delete();
	}

	for(map<vtkStdString,vtkStringArray*>::iterator it = this->Internal->labelArrayMap.begin();
		it != this->Internal->labelArrayMap.end(); it++)
	{
		realOutput->GetPointData()->AddArray(it->second);
		it->second->Delete();
	}
	
	append->Delete();
	this->Internal->labelArrayMap.clear();
	this->Internal->doubleArrayMap.clear();
	this->Internal->stringArrayMap.clear();
	myFile.close();
	return 1;
}




//----------------------------------------------------------------------------
bool vtkLineNetworkReader::isNumeric(vtkStdString value)
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



//----------------------------------------------------------------------------
int vtkLineNetworkReader::AddArrow( vtkAppendPolyData* &append, double* point1,
																	 double* point2, int coneResolution)
{

  int numberOfPoints = 0;
  double* direction = new double[3];
  double *center = new double[3];
  double radius = this->Size ;
	double height = sqrt(vtkMath::Distance2BetweenPoints(point1,point2));

  for (int i=0; i < 3; i++)
	{
    direction[i] = point2[i]-point1[i];
    center[i] = (point2[i]+point1[i])/2.0;
	}

	vtkLineSource *line = vtkLineSource::New();
	line->SetPoint1(point1);
	line->SetPoint2(point2);
	line->Update();
	append->AddInput(line->GetOutput());

	numberOfPoints += line->GetOutput()->GetNumberOfPoints();


  //set up the transform for the cylinder
  double rotation[16];
  this->CreateMatrix( rotation, direction, center );
  vtkTransform *transform = vtkTransform::New();
  transform->SetMatrix( rotation );

  //cylinder that is the base of the
  vtkCylinderSource *cylinder = vtkCylinderSource::New();
	cylinder->SetRadius( this->ScaleArrowSize );
	cylinder->SetHeight( this->Size  );
  cylinder->SetCenter( 0, 0, 0 );
  cylinder->Update();

	numberOfPoints += ( cylinder->GetOutput()->GetNumberOfPoints() );

  //move the cylinder
  vtkTransformFilter *tf = vtkTransformFilter::New();
  tf->SetTransform( transform );
  tf->SetInput( cylinder->GetOutput() );
  tf->Update();

	double k = (this->Size+height+(2*this->ScaleArrowSize))/(2*height);
	center[0] = point1[0] + k*(direction[0]);
	center[1] = point1[1] + k*(direction[1]);
	center[2] = point1[2] + k*(direction[2]);

//here is the cone that composes the middle section of the line
	vtkConeSource *midCone = vtkConeSource::New();
	midCone->SetHeight(4*this->ScaleArrowSize);
	midCone->SetRadius( 2*this->ScaleArrowSize );
	midCone->SetCenter( center );
	midCone->SetDirection( direction );
	midCone->SetResolution( coneResolution );
	midCone->Update();

	numberOfPoints += ( midCone->GetOutput()->GetNumberOfPoints() );
	append->AddInput( midCone->GetOutput() );
	midCone->Delete();


  append->AddInput( (vtkPolyData*) tf->GetOutput() );

  //clean up the memory we have used
  tf->Delete();
  transform->Delete();
  cylinder->Delete();

	line->Delete();

  delete[] direction;
  delete[] center;
  return numberOfPoints;
}




//----------------------------------------------------------------------------
int vtkLineNetworkReader::AddTunnel( vtkAppendPolyData* &append,
																			 double* point1, double* point2, double resolution)
{
  double numberOfPoints = 0;
  double* direction = new double[3];
  double *center = new double[3];
  double radius = this->Size ;
	double height = sqrt(vtkMath::Distance2BetweenPoints(point1,point2));

  for (int i=0; i < 3; i++)
    {
    direction[i] = point2[i]-point1[i];
    center[i] = (point2[i]+point1[i])/2.0;
    }

	vtkLineSource *line = vtkLineSource::New();
	line->SetPoint1(point1);
	line->SetPoint2(point2);
	line->Update();
	append->AddInput(line->GetOutput());

	numberOfPoints +=line->GetOutput()->GetNumberOfPoints();

	if(3*radius < height)
	{
		vtkConeSource *midCone = vtkConeSource::New();
		midCone->SetHeight(3*radius);
		midCone->SetRadius( 2*radius );
		midCone->SetCenter( center );
		midCone->SetDirection( direction );
		midCone->SetResolution( resolution );
		midCone->Update();

		numberOfPoints += ( midCone->GetOutput()->GetNumberOfPoints() );
		append->AddInput( midCone->GetOutput() );
		midCone->Delete();
	}



  //set up the transform for the cylinder
  double rotation[16];
  this->CreateMatrix( rotation, direction, center );
  vtkTransform *transform = vtkTransform::New();
  transform->SetMatrix( rotation );

  //cylinder that is the base of the
  vtkCylinderSource *cylinder = vtkCylinderSource::New();
  cylinder->SetRadius( radius ); //our cylinder is always have a thick as the cone
  cylinder->SetHeight( height  );
  cylinder->SetCenter( 0 , 0, 0 );
  cylinder->Update();

  numberOfPoints += ( cylinder->GetOutput()->GetNumberOfPoints() );

  //move the cylinder
  vtkTransformFilter *tf = vtkTransformFilter::New();
  tf->SetTransform( transform );
  tf->SetInput( cylinder->GetOutput() );
  tf->Update();

	append->AddInput( (vtkPolyData*) tf->GetOutput() );


  //clean up the memory we have used
  tf->Delete();
  transform->Delete();
  cylinder->Delete();

  delete[] direction;
  delete[] center;
  return numberOfPoints;
}



//----------------------------------------------------------------------------
void vtkLineNetworkReader::CreateMatrix( double rotation[16], double *direction, double *center )
{

  //storage for cross products
  double norm[3]={0,1,0};
  double firstVector[3];
  double secondVector[3];
  double thirdVector[3];

  //copy the point, so that we do not destroy it
  firstVector[0] = direction[0];
  firstVector[1] = direction[1];
  firstVector[2] = direction[2];
  vtkMath::Normalize( firstVector );

  //have to find the other 2 vectors, to create a proper transform matrix
  vtkMath::Cross( firstVector, norm, secondVector );
  vtkMath::Cross( firstVector, secondVector, thirdVector );

  //rotate and centre according to normalized axes and centre point
  //column 1
  rotation[0] = secondVector[0];
  rotation[1] = firstVector[0];
  rotation[2] = thirdVector[0];
  rotation[3] = center[0];

  //column 2
  rotation[4] = secondVector[1];
  rotation[5] = firstVector[1];
  rotation[6] = thirdVector[1];
  rotation[7] = center[1];

  //column 3
  rotation[8] = secondVector[2];
  rotation[9] = firstVector[2];
  rotation[10] = thirdVector[2];
  rotation[11] = center[2];

  //column 4
  rotation[12] = 0.0;
  rotation[13] = 0.0;
  rotation[14] = 0.0;
  rotation[15] = 1.0;

}



