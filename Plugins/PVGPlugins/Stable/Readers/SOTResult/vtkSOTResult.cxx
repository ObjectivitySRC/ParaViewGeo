// .NAME vtkSOTResult.cxx
// By: Yasmine HADDAD && Matthew Livingstone
// Re-written by: Nehme Bilal (March 2009)
// Read SOTResult file (.csv).
#include "vtkSOTResult.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkStdString.h"
#include "vtkStringList.h"
#include "vtkDiskSource.h"
#include "vtkSuperquadricSource.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkTubeFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkVectorText.h"
#include "math.h"
#include "vtkDoubleArray.h"
#include "vtkStringArray.h"
#include <vtksys/ios/sstream>
#include <sstream>

#include "vtkImplicitFunction.h"

#include <vtkstd/vector>
#include <vtkstd/map>
#include <vtkstd/list>
#include <vtkstd/algorithm>
#include <vtkstd/set>

#define PI 3.1415926535

// Each point read from the file will populate the following properties
// TODO: Rename properly
struct pointDetails
	{
	double radius, distance, angle;
	vtkStdString thName;
	};

struct Internal
	{
	vtkstd::map < int, vtkDoubleArray* > doubleArrayMap;
	vtkstd::map < int, vtkStringArray* > stringArrayMap;

	vtkstd::vector< pointDetails > coordVector;
	vtkstd::vector< vtkstd::pair <double, double> > references;

	vtkstd::set < vtkStdString > thSet;

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
	int readLineSkipEmpty(ifstream &file, vtkStdString &line, vtkstd::vector<vtkStdString> &lineSplit, unsigned int minLenght)
		{
		getline(file, line);
		this->splitSkipEmpty(line, lineSplit, ",");
		if(lineSplit.size()<minLenght && !file.eof())
			{
			this->readLine(file, line, lineSplit, minLenght);
			}

		if(lineSplit.size() < minLenght)
			{
			return 0;
			}

		return 1;
		}

	//----------------------------------------------------------------------------
	int readLine(ifstream &file, vtkStdString &line, vtkstd::vector<vtkStdString> &lineSplit, unsigned int minLenght)
		{
		getline(file, line);
		this->split(line, lineSplit, ",");
		if(lineSplit.size()<minLenght && !file.eof())
			{
			this->readLine(file, line, lineSplit, minLenght);
			}

		if(lineSplit.size() < minLenght)
			{
			return 0;
			}

		return 1;
		}

	};

struct Internal_PropertiesStatus
	{
		vtkstd::map < vtkStdString, int > PStatus;
	};


vtkCxxRevisionMacro(vtkSOTResult, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkSOTResult);
vtkCxxSetObjectMacro(vtkSOTResult,CutFunction,vtkImplicitFunction);

// Constructor
vtkSOTResult::vtkSOTResult(vtkImplicitFunction *cf)
	{
	this->FileName = 0;
	this->Headers = 0;
	this->ApplyPressed = 0;
	this->SetNumberOfOutputPorts(3); // ***** (4)
	this->SetNumberOfInputPorts(0);

	this->RadiusVariable = 0;
	this->DistanceVariable = 0;
	this->AngleVariable = 0;
	this->ThVariable = 0;
	this->ReverseRadius = false;
	this->ReverseDistance = false;
	this->ReverseAngle = false;
	this->AlignCircles = false;
	this->DistanceBetween = 0;
	this->ReferenceCirclesSize = 0;
	this->AxisThickness = 0;
	this->CircleThickness = 0;
	this->PlaneDistanceVariable = 0;
	this->EnableClip = false;
	this->BTransXVariable = 0;
	this->BTransYVariable = 0;
	this->BTransZVariable = 0;
	this->BScaleXVariable = 0;
	this->BScaleYVariable = 0;
	this->BScaleZVariable = 0;

	this->numberOfGrids = 0;
	this->numberOfCircles = 0;

	this->internals = new Internal();

	this->guiLoaded = false;

	this->CutFunction = cf;

  this->SotProperties= new Internal_PropertiesStatus;
	}

// --------------------------------------
// Destructor
vtkSOTResult::~vtkSOTResult()
	{
	this->SetFileName(0);
	delete this->internals;

	this->SetCutFunction(NULL);

	this->SotProperties->PStatus.clear();
	delete this->SotProperties;
	}

// --------------------------------------
void vtkSOTResult::PrintSelf(ostream& os, vtkIndent indent)
	{
	this->Superclass::PrintSelf(os,indent);
	os << indent <<  "FileName: "
		<< (this->FileName ? this->FileName : "(none)") << "\n";
	}


// --------------------------------------
int vtkSOTResult::CanReadFile( const char* fname )
	{
	return 1;	
	}

// --------------------------------------
// Request information is called as soon as the file is opened in Paraview
// before the apply button even appears.
// NOTE: This is run any time changes are made and the apply button is clicked.
// It would not be wise to use variable incrementation (++) on class variables
// if they have not been reset.
int vtkSOTResult::RequestInformation(vtkInformation* request,
																		 vtkInformationVector** inputVector,
																		 vtkInformationVector* outputVector)
	{
	// We only need to parse the headers before loading the GUI
	if(this->guiLoaded)
		{
		return 1;
		}
	this->guiLoaded = true;


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
	std::vector<vtkStdString> lineSplit2;

	vtkStdString numericHeaders;
	vtkStdString stringHeaders;

	// read the headers and save them in lineSplit
	if(!this->internals->readLineSkipEmpty(file, line, lineSplit, 3))
		{
		vtkErrorMacro("the SOT Result file is empty");
		file.close();
		return 0;
		}

	// read the first line so we can check which property is numeric
	if(!this->internals->readLine(file, line, lineSplit2, lineSplit.size()))
		{
		vtkErrorMacro("the SOT Result file is empty");
		file.close();
		return 0;
		}

	for(unsigned int i=0; i< lineSplit.size(); i++)
		{
		// if numeric
		if(    isdigit(lineSplit2[i].at(0))   ||   ( atof(lineSplit2[i].c_str()) && (lineSplit2[i].at(0) == '-') )    )
			{
			// Add numeric variable to string containing all numeric properties
			//   String is separated using "|"
			numericHeaders += lineSplit[i] + "|";
			this->SotProperties->PStatus.insert(this->SotProperties->PStatus.begin(), 
						vtkstd::pair<vtkStdString, int>(lineSplit[i], 0) );//adding for properties selection
			this->internals->doubleArrayMap[i] = NULL;
			}
		else
			{
			// Add string variable to string containing all string properties
			//   String is separated using "|"
			stringHeaders += lineSplit[i] + "|";
			this->SotProperties->PStatus.insert(this->SotProperties->PStatus.begin(), 
					  vtkstd::pair<vtkStdString, int>(lineSplit[i], 0) ); //adding for properties selection
			this->internals->stringArrayMap[i] = NULL;
			}
		}

	// Both the numeric and string properies are now passed to the GUI
	// This is done with "information_only" properties in the XML
	// They will be used to populate the combo boxes that will be used to
	//    select the property used for radius, distance and angle

	this->Headers = new char[numericHeaders.size()];
	strcpy(this->Headers, numericHeaders.c_str());
	this->StringHeaders = new char[stringHeaders.size()];
	strcpy(this->StringHeaders, stringHeaders.c_str());
	file.close();
	
	return 1;
	}

// --------------------------------------
// This method creates the three axis inside of the "cylinder"
int vtkSOTResult::CreateAxes(double x, double y, double dz)
	{
	vtkPoints *axesPoints = vtkPoints::New();
	vtkCellArray *axesCells = vtkCellArray::New();
	vtkDoubleArray *axisProp = vtkDoubleArray::New();

	double zPos = 0.0;
	if(this->EnableClip)
		{
		zPos = -dz;
		dz = -(zPos-0.5);
		}

	// Z Axis
	axesPoints->InsertNextPoint(x, y, zPos);
	axesPoints->InsertNextPoint(x, y, -dz);
	axesCells->InsertNextCell(2);
	axesCells->InsertCellPoint(0);
	axesCells->InsertCellPoint(1);
	// Green colour
	axisProp->InsertNextValue(1.0);
	axisProp->InsertNextValue(1.0);

	// X Axis
	axesPoints->InsertNextPoint(x, y, zPos);
	axesPoints->InsertNextPoint(x+ReferenceCirclesSize, y, zPos);
	axesCells->InsertNextCell(2);
	axesCells->InsertCellPoint(2);
	axesCells->InsertCellPoint(3);
	// Red colour
	axisProp->InsertNextValue(2.0);
	axisProp->InsertNextValue(2.0);

	// Y Axis
	axesPoints->InsertNextPoint(x, y, zPos);
	axesPoints->InsertNextPoint(x, y+ReferenceCirclesSize, zPos);
	axesCells->InsertNextCell(2);
	axesCells->InsertCellPoint(4);
	axesCells->InsertCellPoint(5);
	// Blue colour
	axisProp->InsertNextValue(0.0);
	axisProp->InsertNextValue(0.0);


	vtkPolyData *Axes = vtkPolyData::New();
	Axes->SetPoints(axesPoints);
	Axes->SetLines(axesCells);
	axisProp->SetName("Axis Colour");
	// Set scalars is used as it forces the object to be coloured
	// by that property as soon as the scene is drawn.
	Axes->GetPointData()->SetScalars(axisProp);

	// The tube filter is used to make the lines that were created
	// into tubes (cylinders)
	vtkTubeFilter *tubes = vtkTubeFilter::New();
	tubes->SetCapping(1);
	tubes->SetNumberOfSides(15);
	tubes->SetRadius(this->AxisThickness);
	// SetInput sets the object that will be used to create the tubes from (must be lines).
	tubes->SetInput(Axes);
	tubes->Update();

	// Generates normals on the input object
	vtkPolyDataNormals *pdn = vtkPolyDataNormals::New();
	pdn->AddInput(tubes->GetOutput());
	pdn->Update();

	this->axes->AddInput(pdn->GetOutput());

	//cleanup
	Axes->Delete();
	axesPoints->Delete();
	axesCells->Delete();
	axisProp->Delete();
	tubes->Delete();
	pdn->Delete();

	return 1;

	}


//--------------------------------------------------------
bool vtkSOTResult::parseFile(int lineSize, ifstream &file)
	{
	vtkStdString line;
	vtkstd::vector<vtkStdString> lineSplit;

	if(!this->internals->readLine(file, line, lineSplit, lineSize))
		{
		vtkErrorMacro("the SOT Result file is invalid");
		return 0;
		}

	this->minRadius =  atof(lineSplit[this->radiusIndex].c_str());
	this->maxRadius = this->minRadius;

	this->minDistance = atof(lineSplit[this->distanceIndex].c_str());
	this->maxDistance = this->minDistance;

	this->minAngle = atof(lineSplit[this->angleIndex].c_str());
	this->maxAngle = this->minAngle;

	// Grab the value that will be used to separate the the data in comparison view
	// Stored in a set to ensure only the unique values are kept
	this->internals->thSet.insert(lineSplit[this->thIndex]);

	// Tuple will be populated from the file,
	//   then pushed into the coordVector 
	pointDetails tuple;

	do
		{
		// Read all double properties from the current line (current point)
		for(vtkstd::map<int, vtkDoubleArray*>::iterator it = this->internals->doubleArrayMap.begin();
			it != this->internals->doubleArrayMap.end(); it++)
			{
			it->second->InsertNextValue(atof(lineSplit[it->first].c_str()));
			}

		// Read all string properties from the current line (current point)
		for(vtkstd::map<int, vtkStringArray*>::iterator it = this->internals->stringArrayMap.begin();
			it != this->internals->stringArrayMap.end(); it++)
			{
			it->second->InsertNextValue(lineSplit[it->first]);
			}

		// Point details are converted from vtkStdString to double before being
		//    stored in their respective variables
		tuple.radius = atof(lineSplit[this->radiusIndex].c_str());
		tuple.distance = atof(lineSplit[this->distanceIndex].c_str());
		tuple.angle = atof(lineSplit[this->angleIndex].c_str());
		tuple.thName = lineSplit[this->thIndex];
		// Grab the value that will be used to separate the the data in comparison view
		// Stored in a set to ensure only the unique values are kept
		this->internals->thSet.insert(lineSplit[this->thIndex]);

		// Calculate the min/max of radius, distance, angle
		if(tuple.radius < this->minRadius)
			{
			this->minRadius = tuple.radius;
			}
		else if(tuple.radius > this->maxRadius)
			{
			this->maxRadius = tuple.radius;
			}

		if(tuple.distance < this->minDistance)
			{
			this->minDistance = tuple.distance;
			}
		else if(tuple.distance > this->maxDistance)
			{
			this->maxDistance = tuple.distance;
			}

		if(tuple.angle < this->minAngle)
			{
			this->minAngle = tuple.angle;
			}
		else if(tuple.angle > this->maxAngle)
			{
			this->maxAngle = tuple.angle;
			}

		// Add the new point data
		this->internals->coordVector.push_back(tuple);
		
		// Grab the value that will be used to separate the the data in comparison view
		// Stored in a set to ensure only the unique values are kept
		this->internals->thSet.insert(lineSplit[this->thIndex]);

	}  // End condition for 'do' loop:
	while(this->internals->readLine(file, line, lineSplit, lineSize) &! file.eof());

	if (this->ArtificialMaxNPV > 0)
		{
		this->maxRadius = this->ArtificialMaxNPV;
		}
	if (this->ArtificialMinNPV > 0)
		{
		this->minRadius = this->ArtificialMinNPV;
		}
	if (this->ArtificialMaxMineLife > 0)
		{
		this->maxAngle = this->ArtificialMaxMineLife;
		}
	if (this->ArtificialMinMineLife > 0)
		{
		this->minAngle = this->ArtificialMinMineLife;
		}
		return 1;
	}


//---------------------------------------------------------------------------------
void vtkSOTResult::freeMemory()
	{
	// Iterate over all double arrays (double properties) and delete them
	for(vtkstd::map<int, vtkDoubleArray*>::iterator it = this->internals->doubleArrayMap.begin();
		it != this->internals->doubleArrayMap.end(); it++)
		{
		if(it->second)
			{
			it->second->Delete();
			}
		}

	// Iterate over all string arrays (string properties) and delete them
	for(vtkstd::map<int, vtkStringArray*>::iterator it = this->internals->stringArrayMap.begin();
		it != this->internals->stringArrayMap.end(); it++)
		{
		if(it->second)
			{
			it->second->Delete();
			}
		}

	if(this->verts)
		{
		this->verts->Delete();
		}

	if(this->points)
		{
		this->points->Delete();
		}

	this->internals->coordVector.clear();
	this->internals->references.clear();
	this->internals->thSet.clear();

	}




//---------------------------------------------------------------------------------
int vtkSOTResult::addToVerts(double radius, double distance,  
														 double angle, vtkIdType counter, double dx, double dy)
	{
	double x,y,z;

	// Calculate radius information
	// Higher values will be farther from 0.0 if not reversed
	double initialY;
	if(!this->ReverseRadius)
		{
		if ( radius < this->minRadius )  initialY = 0;
		else initialY = ( radius - this->minRadius ) * ReferenceCirclesSize/this->deltaRadius;
		}
	else
		{
		if ( this->maxRadius < radius )  initialY = 0;
		initialY = ( this->maxRadius - radius ) * ReferenceCirclesSize/this->deltaRadius;
		}

	// Calculate the X/Y based on the angle

	// 2PI gives a whole circle. Taking 5/6 of that leaves a small
	// "pie piece" or "pizza slice" missing.
	double var = ((2 * PI) * (5.0 / 6.0));

	// The angle is shifted by 
	double angl = 2.0 * (PI / 3.0);
	if (angle > this->maxAngle)
		angl += var;
	else if (angle > this->minAngle)
		angl += var * ( (angle - this->minAngle) / this->deltaAngle );

	// The -(val) is used to make the lowest value start in the top right,
	//	and the highest value end in the top left
	x = -(initialY * cos(angl));
	y = initialY * sin(angl);

	z = ( (distance-this->minDistance) ) * this->DistanceBetween/this->deltaDistance;

	if(this->EnableClip)
		{
			// Calculate the ends of each side of the box
			double endZ = this->BTransZVariable-(this->BScaleZVariable*this->DistanceBetween);
			double endY = this->BTransYVariable+(this->BScaleYVariable*this->ReferenceCirclesSize);
			double endX = this->BTransXVariable+(this->BScaleXVariable*this->ReferenceCirclesSize);

			// Ensure the points fall within the box (not fudging)
			if( (-z <= this->BTransZVariable && -z >= endZ)
				&& (x >= (this->BTransXVariable-(this->BScaleXVariable*this->ReferenceCirclesSize)) && x <= endX) 
				&& (y >= (this->BTransYVariable-(this->BScaleYVariable*this->ReferenceCirclesSize)) && y <= endY)
				)
				{
				this->points->InsertNextPoint(x + dx, y + dy, -z);
				this->verts->InsertNextCell(1);
				this->verts->InsertCellPoint(counter);
				// Tell the method that called us to add to point counter
				return 1;
				}
			else
				{
				return 0;
				}
		}
	else
		{
		// Not clipping, add everything
		this->points->InsertNextPoint(x + dx, y + dy, -z);
		this->verts->InsertNextCell(1);
		this->verts->InsertCellPoint(counter);
		// Add to point counter
		return 1;
		}

	return 0;
	}


//---------------------------------------------------------------------------------
int vtkSOTResult::transformCoordinates()
	{
	vtkIdType pointCounter = 0;
	pointDetails PointDetail; // TODO: Reverse the camelback notation
	this->verts = vtkCellArray::New();
	this->points = vtkPoints::New();
	int retVal = 0;

  for(unsigned int i = 0; i < this->internals->coordVector.size(); i++)
		{
		PointDetail = this->internals->coordVector[i];
		retVal = this->addToVerts( PointDetail.radius, PointDetail.distance, PointDetail.angle, pointCounter, 0, 0 );

		// Only add point to count if the point was added in addToVerts
		// Inside addToVerts is where points are excluded during a clip
		if(retVal == 1)
			{
			pointCounter++;
			}
		}

	return 1;
	}


//---------------------------------------------------------------------------------
int vtkSOTResult::transformComparisonCoordinates()
	{
	vtkIdType pointCounter = 0;
	pointDetails PointDetail;
	this->verts = vtkCellArray::New();
	this->points = vtkPoints::New();
	int retVal = 0;

	double dx=0;
	double dy=0;
	unsigned int m;

	for(unsigned int i = 0; i < this->internals->coordVector.size(); i++)
		{
		PointDetail = this->internals->coordVector[i];
		m = 0;
		// Determine which comparison group the point belongs to
		for(vtkstd::set< vtkStdString > ::iterator it2 = this->internals->thSet.begin();
			it2 != this->internals->thSet.end(); it2++)
			{
			if( PointDetail.thName == (*it2) )
				{
				break;
				}
			m++;
			}

		// Offset point based on the comparison group it belongs to
		dx = this->internals->references[m].first; // x of reference m
		dy = this->internals->references[m].second; // y of reference m
		retVal = this->addToVerts( PointDetail.radius, PointDetail.distance, PointDetail.angle, pointCounter, dx, dy );

		// Only add point to count if the point was added in addToVerts
		// Inside addToVerts is where points are excluded during a clip
		if(retVal)
			{
			pointCounter++;
			}
		}

	return 1;
	}



//--------------------------------------------------------------------------------
int vtkSOTResult::AddGrid(double x, double y, double z, bool showDistLabel, double Label)
	{
	double r;

	for(int i=0; i<this->numberOfCircles; i++)
		{
		vtkSuperquadricSource *quad = vtkSuperquadricSource::New();
		vtkTransform *transform = vtkTransform::New();
		vtkTransformFilter *tf = vtkTransformFilter::New();

		// ReferenceCirclesSize units on each side of x axis
		quad->SetCenter(x, -z, -y);
		r = 2*(ReferenceCirclesSize - ((ReferenceCirclesSize*i)/this->numberOfCircles));
		quad->SetScale(r, r, r);
		quad->SetThickness(this->CircleThickness);
		quad->SetThetaResolution(30);
		quad->SetToroidal(1);

		transform->RotateX(90.0);

		tf->SetInput(quad->GetOutput());
		tf->SetTransform(transform);

		tf->Update();
		this->circles->AddInput( tf->GetPolyDataOutput() );

		//cleanup 
		quad->Delete();
		transform->Delete();
		tf->Delete();
		}

	if(!showDistLabel)
		{
		return 1;
		}

	vtkstd::stringstream textStream;
	vtkStdString text;
	double textSize = this->ReferenceCirclesSize/10;

	vtkTransform *transform = vtkTransform::New();
	vtkTransformFilter *tf = vtkTransformFilter::New();

	textStream << Label;
	textStream >> text;
	textStream.clear();

	vtkVectorText *label = vtkVectorText::New();
	label->SetText(text);

	transform->Translate(x + this->ReferenceCirclesSize, y - this->ReferenceCirclesSize, -z );
	transform->Scale(textSize, textSize, textSize);
	tf->SetInput(label->GetOutput());
	tf->SetTransform(transform);
	tf->Update();

	this->circles->AddInput(tf->GetPolyDataOutput());

	label->Delete();
	transform->Delete();
	tf->Delete();	

	return 1;
	}




//--------------------------------------------------------------------------------
int vtkSOTResult::addComparisonGrids()
	{
	int j;
	int num = this->internals->thSet.size();
	int n = num;

	for(int i=1; i<=num; i++)
		{
		if( abs((num/i)-i) <= n ) 
			{
			n = abs((num/i)-i);
			j = i;
			}
		}

	double x=0;
	double y=0;
	double posx = 0;
	n = j;

	double labelStep = this->deltaDistance/this->numberOfGrids;

	vtkstd::set< vtkStdString > ::iterator it = this->internals->thSet.begin();

	vtkstd::stringstream textStream;
	vtkStdString text;

	for(int i=1; i<=num; i++)
		{
		textStream << (*it);
		textStream >> text;
		textStream.clear();
		it++;

		this->addNormalViewGrids(x+posx, y, 0, text, true);
		this->CreateAxes(x+posx, y, this->DistanceBetween/this->numberOfGrids);
		this->internals->references.push_back(vtkstd::make_pair(x+posx, y));

		posx += this->ReferenceCirclesSize*3;

		if( i == j)
			{
			x = 0;
			y = y - (this->ReferenceCirclesSize*3);
			j += n;
			posx = 0;
			}
		}

	return 1;
	}


//--------------------------------------------------------------------------------
int vtkSOTResult::addNormalViewGrids(double x, double y, double z, vtkStdString compareLabel, bool showCompLabel)
	{
	if(this->numberOfGrids>0)
		{
		if(this->EnableClip)
			{
				// Get current distance for front reference circles
				double zLabelDbl = (this->BTransZVariable/this->DistanceBetween) * this->deltaDistance;
				this->addInitialGrid(x, y, z, -zLabelDbl, true, compareLabel, showCompLabel);
			}
		else
			{
			this->addInitialGrid(x, y, z, this->minDistance, true, compareLabel, showCompLabel);
			}
		}
	else
		{
		return 1;
		}

	if(!this->EnableClip)
		{
		double distanceStep = this->deltaDistance/(this->numberOfGrids);

		for(int i=1; i<=this->numberOfGrids; i++)
			{
			this->AddGrid( x, y, (this->DistanceBetween*i)/(this->numberOfGrids), true, this->minDistance + (distanceStep*i) );
			}
		}
	else if(this->EnableClip /*&& strcmp(this->ClipType, "Box") == 0*/)// && !this->AlignCircles)
		{
		// Calculate the ends of each side of the box
		double endZ = this->BTransZVariable-(this->BScaleZVariable*this->DistanceBetween);
		double distanceStep = (this->BTransZVariable-endZ)/(this->numberOfGrids);

		for(int i=1; i<=this->numberOfGrids; i++)
			{
			double zPos = -(this->BTransZVariable+(((-this->DistanceBetween-this->BTransZVariable)/this->numberOfGrids)*i));
			double distScaled = (zPos/this->DistanceBetween)*(this->maxDistance - this->minDistance);
			this->AddGrid( x, y, zPos, true, distScaled);
			}
		}
	// Removing, as when clipping with Plane, we only need 1 set of reference circles
	/*else if(this->EnableClip && strcmp(this->ClipType, "Plane") == 0)// && !this->AlignCircles)
	{
	// Calculate the ends of each side of the box
	double endZ = this->PlaneDistanceVariable-(this->PlaneDistanceVariable*this->DistanceBetween);
	double distanceStep = (this->PlaneDistanceVariable-endZ)/(this->numberOfGrids);

	for(int i=1; i<=this->numberOfGrids; i++)
	{
	double zPos = -(this->PlaneDistanceVariable+(((-this->DistanceBetween-this->PlaneDistanceVariable)/this->numberOfGrids)*i));
	double distScaled = (zPos/this->DistanceBetween)*(this->maxDistance - this->minDistance);
	this->AddGrid( x, y, zPos, true, distScaled);
	}
	}*/
	return 1;
	}




//--------------------------------------------------------------------------------
int vtkSOTResult::addInitialGrid(double x, double y, double z, double distLabel, bool showDistLabel,
																 vtkStdString compareLabel, bool showCompLabel)
	{
	vtkstd::stringstream textStream;
	vtkStdString text;
	vtkstd::vector < vtkStdString > textSplit;
	unsigned int numberOfDigits;

	textStream << this->maxRadius;
	textStream >> text;
	textStream.clear();

	this->AddGrid(x, y, z, true, distLabel);

	// Has to be flipped, as all other data is passed in already flipped.
	// Not sure exactly how this happens.
	if(this->EnableClip)
		{
		z = -z;
		}

	numberOfDigits = text.size();
	double radiusStep = this->deltaRadius/this->numberOfCircles;
	double xStep = this->ReferenceCirclesSize/this->numberOfCircles;
	double textSize = xStep/numberOfDigits;
	double r;


	for(int i=0; i<=this->numberOfCircles; i++)
		{
		r = this->minRadius + (i*radiusStep);
		textStream << r;
		textStream >> text;
		textStream.clear();

		text = text.substr(0, numberOfDigits);
		if(text.substr(text.size()-1) == ".")
			{
			text = text.substr(0, text.size()-1);
			}

		vtkVectorText *label = vtkVectorText::New();
		label->SetText(text);

		vtkTransform *transform = vtkTransform::New();
		vtkTransformFilter *tf = vtkTransformFilter::New();

		// Ensure the Radius labels are increasing/decreasing in the right direction
		if(this->ReverseRadius)
			{ // Start at max step and move backwards
			transform->Translate(x + (1.2*this->AxisThickness) + (xStep*((this->numberOfCircles)-i)), y + (1.2*this->AxisThickness), z);
			} else
			{ // Start at first step and move forwards
			transform->Translate(x + (1.2*this->AxisThickness) + (xStep*i), y + (1.2*this->AxisThickness), z);
				}

			transform->Scale(textSize, textSize, textSize);
			tf->SetInput(label->GetOutput());
			tf->SetTransform(transform);
			tf->Update();

			this->circles->AddInput(tf->GetPolyDataOutput());

			label->Delete();
			transform->Delete();
			tf->Delete();
		}

	textSize = this->ReferenceCirclesSize/10;

	vtkTransform *transform = vtkTransform::New();
	vtkTransformFilter *tf = vtkTransformFilter::New();
	textStream << this->minAngle;
	textStream >> text;
	textStream.clear();

	vtkVectorText *label = vtkVectorText::New();
	label->SetText(text);

	double X = (this->ReferenceCirclesSize*cos(PI/3)) + x;
	double Y = (this->ReferenceCirclesSize*sin(PI/3)) + y;

	transform->Translate(X, Y, z);
	transform->Scale(textSize, textSize, textSize);
	tf->SetInput(label->GetOutput());
	tf->SetTransform(transform);
	tf->Update();

	this->circles->AddInput(tf->GetPolyDataOutput());
	label->Delete();
	transform->Delete();
	tf->Delete();

	transform = vtkTransform::New();
	tf = vtkTransformFilter::New();
	textStream << this->maxAngle;
	textStream >> text;
	textStream.clear();

	label = vtkVectorText::New();
	label->SetText(text);

	X = (-this->ReferenceCirclesSize*cos(PI/3)) + x;
	X = X - (0.9*textSize*text.size());
	transform->Translate(X, Y, z);
	transform->Scale(textSize, textSize, textSize);
	tf->SetInput(label->GetOutput());
	tf->SetTransform(transform);
	tf->Update();

	this->circles->AddInput(tf->GetPolyDataOutput());
	label->Delete();
	transform->Delete();
	tf->Delete();

	if(showCompLabel)
		{
		vtkTransform *transform = vtkTransform::New();
		vtkTransformFilter *tf = vtkTransformFilter::New();
		vtkStdString text2;

		text = this->ThVariable;

		textStream << compareLabel;
		textStream >> text2;
		textStream.clear();

		text.append(": ");
		text.append(text2);

		vtkVectorText *label = vtkVectorText::New();
		label->SetText(text);

		transform->Translate(x-((text.length()/2.5)*textSize), y - this->ReferenceCirclesSize - (2*textSize), -z );
		transform->Scale(textSize, textSize, textSize);
		tf->SetInput(label->GetOutput());
		tf->SetTransform(transform);
		tf->Update();

		this->circles->AddInput(tf->GetPolyDataOutput());

		label->Delete();
		transform->Delete();
		tf->Delete();	
		}

	return 1;
	}



//--------------------------------------------------------------------------------
int vtkSOTResult::RequestData(vtkInformation* request,
															vtkInformationVector** inputVector,
															vtkInformationVector* outputVector)
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
	vtkstd::vector<vtkStdString> lineSplit;
	// read the headers and save them in lineSplit
	if(!this->internals->readLineSkipEmpty(file, line, lineSplit, 3))
		{
		vtkErrorMacro("the SOT Result file is empty");
		file.close();
		return 0;
		}

	// find the positions of radius, distance, angle and save them
	vtkstd::vector<vtkStdString>::iterator finder;
	finder = vtkstd::find(lineSplit.begin(), lineSplit.end(), this->RadiusVariable);
	if(finder == lineSplit.end())
		{
		vtkErrorMacro("Cannot find Radius variable in the file");
		file.close();
		return 0;
		}
	this->radiusIndex = finder - lineSplit.begin();

	finder = vtkstd::find(lineSplit.begin(), lineSplit.end(), this->DistanceVariable);
	if(finder == lineSplit.end())
		{
		vtkErrorMacro("Cannot find Distance variable in the file");
		file.close();
		return 0;
		}
	this->distanceIndex = finder - lineSplit.begin();

	finder = vtkstd::find(lineSplit.begin(), lineSplit.end(), this->AngleVariable);
	if(finder == lineSplit.end())
		{
		vtkErrorMacro("Cannot find Radius variable in the file");
		file.close();
		return 0;
		}
	this->angleIndex = finder - lineSplit.begin();

	finder = vtkstd::find(lineSplit.begin(), lineSplit.end(), this->ThVariable);
	if(finder == lineSplit.end())
		{
		vtkErrorMacro("Cannot find Threshold variable in the file");
		file.close();
		return 0;
		}
	this->thIndex = finder - lineSplit.begin();

	unsigned int lineSize = lineSplit.size();

	for(vtkstd::map<int, vtkDoubleArray*>::iterator it = this->internals->doubleArrayMap.begin();
		it != this->internals->doubleArrayMap.end(); it++)
		{
		it->second = vtkDoubleArray::New();
		it->second->SetName(lineSplit[it->first].c_str());
		}

	for(vtkstd::map<int, vtkStringArray*>::iterator it = this->internals->stringArrayMap.begin();
		it != this->internals->stringArrayMap.end(); it++)
		{
		it->second = vtkStringArray::New();
		it->second->SetName(lineSplit[it->first].c_str());
		}


	if(!this->parseFile(lineSize, file))
		{
		file.close();
		this->freeMemory();
		return 0;
		}

	this->DistMin = this->minDistance;
	this->DistMax = this->maxDistance;

	this->deltaRadius = this->maxRadius - this->minRadius;
	this->deltaDistance = this->maxDistance - this->minDistance;
	this->deltaAngle = this->maxAngle - this->minAngle;

	if(this->deltaRadius == 0)
		{
		this->deltaRadius = 1;
		}

	if(this->deltaDistance == 0)
		{
		this->deltaDistance = 1;
		}

	if(this->deltaAngle == 0)
		{
		this->deltaAngle = 1;
		}

	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *pointDataOutput = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));

	outInfo = outputVector->GetInformationObject(1);
	this->circlesOutput = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));

	outInfo = outputVector->GetInformationObject(2);
	this->axesOutput = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));

	this->circles = vtkAppendPolyData::New();
	this->axes = vtkAppendPolyData::New();

	if(!this->AlignCircles)
		{
			
		if(this->EnableClip)
			{
				this->addNormalViewGrids(0, 0, -this->BTransZVariable, "", false);
				this->CreateAxes(0, 0, -this->BTransZVariable);
			}
		else
			{
			this->addNormalViewGrids(0, 0, 0, "", false);
			this->CreateAxes(0, 0, this->DistanceBetween);
			} 
		}
	else
		{
		this->addComparisonGrids();
		}
	this->circles->Update();
	this->circlesOutput->ShallowCopy(this->circles->GetOutput());
	this->circles->Delete();

	this->axes->Update();
	this->axesOutput->ShallowCopy(this->axes->GetOutput());
	this->axes->Delete();

	// Check if drawing comparison view
	if(this->AlignCircles)
		{ // Break points into comparison sections
		this->transformComparisonCoordinates();
		}
	else
		{ // All points drawn in same set of reference circles
		this->transformCoordinates();
		}

	pointDataOutput->SetPoints(this->points);
	pointDataOutput->SetVerts(this->verts);

	for(vtkstd::map<int, vtkDoubleArray*>::iterator it = this->internals->doubleArrayMap.begin();
		it != this->internals->doubleArrayMap.end(); it++)
		{
		if(this->SotProperties->PStatus[it->second->GetName()]==1)
		pointDataOutput->GetPointData()->AddArray(it->second);
		}

	for(vtkstd::map<int, vtkStringArray*>::iterator it = this->internals->stringArrayMap.begin();
		it != this->internals->stringArrayMap.end(); it++)
		{
		if(this->SotProperties->PStatus[it->second->GetName()]==1)
		pointDataOutput->GetPointData()->AddArray(it->second);
		}

	file.close();
	this->freeMemory();	

	// this will tel the client that the server has finished applying the algorithm
	this->ApplyPressed = this->AlignCircles;

	return 1;
	}

//----------------------------------------------------------------------------
void vtkSOTResult::SetArraySelection(const char* name, int status)
{	
	vtkStdString temp= name;
	
	if(status==1)
		this->SotProperties->PStatus[temp]=1;
	else if(status==0)
		this->SotProperties->PStatus[temp]=0;

	this->Modified();
}

