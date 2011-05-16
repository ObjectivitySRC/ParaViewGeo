#include "vtkTransportationReader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <vector>
#include <algorithm>

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
#include "vtkPointSource.h"


enum
{
	CAPACITY=0,
	VELOCITY, 
	WIDTH,
	DIP
};


using namespace std;

void split(const string& str,vector<string>& tokens,const string& delimiters);
void stringToLower(string &word);
string doubleToString(double in);



vtkCxxRevisionMacro(vtkTransportationReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkTransportationReader);

// Constructor
vtkTransportationReader::vtkTransportationReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkTransportationReader::~vtkTransportationReader()
{
  this->SetFileName(0);
	this->Size = 10;
}

// --------------------------------------
void vtkTransportationReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}


// --------------------------------------
int vtkTransportationReader::CanReadFile( const char* fname )
{
  // Make sure we have a file to read.
  if(!fname)  {
    vtkErrorMacro("A FileName must be specified.");
    return 0;
  }
  if(strlen(fname)==0)  {
    vtkErrorMacro("A NULL FileName.");
    return 0;
  }

	ifstream myFile;
	myFile.open(fname);
	if(!myFile)
	{
		vtkErrorMacro("File Error");
		return 0;
	}

	if(!this->loadHeaders(myFile))
	{
		return 0;
	}

	myFile.close();
	return 1;
}

//--------------------------------------
int vtkTransportationReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{

	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

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

	unsigned int numberOfColumns = this->loadHeaders(myFile);
	if(!numberOfColumns)
	{
		vtkErrorMacro("Please check the file and try again");
		return 0;
	}


	string line;
	vector<string> lineSplit;


	//--------------------------------------
	vtkDoubleArray* lenProp;
	lenProp = vtkDoubleArray::New();
	lenProp->SetName("len");
	//--------------------------------------
	vtkDoubleArray* lengthProp;
	lengthProp= vtkDoubleArray::New();
	lengthProp->SetName("length");
	//--------------------------------------
	vtkDoubleArray* capacityProp;
	capacityProp= vtkDoubleArray::New();
	capacityProp->SetName("capacity");
	//--------------------------------------
	vtkDoubleArray* velocityProp;
	velocityProp= vtkDoubleArray::New();
	velocityProp->SetName("velocity");
	//--------------------------------------
	vtkDoubleArray* widthProp;
	widthProp= vtkDoubleArray::New();
	widthProp->SetName("width");
	//--------------------------------------
	vtkDoubleArray* dipProp;
	dipProp= vtkDoubleArray::New();
	dipProp->SetName("dip");
	//--------------------------------------
	vtkStringArray* idProp;
	idProp= vtkStringArray::New();
	idProp->SetName("id");
	//--------------------------------------
	vtkStringArray* modelProp;
	modelProp= vtkStringArray::New();
	modelProp->SetName("model");
	//--------------------------------------


	//--------------------------------------
	vtkDoubleArray* lengthArray;
	lengthArray= vtkDoubleArray::New();
	lengthArray->SetName("length (m)");
	//--------------------------------------
	vtkDoubleArray* capacityArray;
	capacityArray= vtkDoubleArray::New();
	capacityArray->SetName("capacity (t/h)");
	//--------------------------------------
	vtkDoubleArray* velocityArray;
	velocityArray= vtkDoubleArray::New();
	velocityArray->SetName("velocity (m/s)");
	//--------------------------------------
	vtkDoubleArray* widthArray;
	widthArray= vtkDoubleArray::New();
	widthArray->SetName("width (mm)");
	//--------------------------------------
	vtkDoubleArray* dipArray;
	dipArray= vtkDoubleArray::New();
	dipArray->SetName("slope degree");
	//--------------------------------------
	vtkStringArray* idArray;
	idArray= vtkStringArray::New();
	idArray->SetName("drift dame");
	//--------------------------------------
	vtkStringArray* modelArray;
	modelArray= vtkStringArray::New();
	modelArray->SetName("model");
	//--------------------------------------


	//--------------------------------------
	vtkStringArray* lengthLabel = vtkStringArray::New();
	lengthLabel->SetName("Length Label");
	//--------------------------------------
	vtkStringArray* capacityLabel = vtkStringArray::New();
	capacityLabel->SetName("Capacity Label");
	//--------------------------------------
	vtkStringArray* velocityLabel = vtkStringArray::New();
	velocityLabel->SetName("velocity Label");
	//--------------------------------------
	vtkStringArray* widthLabel = vtkStringArray::New();
	widthLabel->SetName("Width Label");
	//--------------------------------------
	vtkStringArray* dipLabel = vtkStringArray::New();
	dipLabel->SetName("Dip Label");
	//--------------------------------------
	vtkStringArray* idLabel = vtkStringArray::New();
	idLabel->SetName("Drift Name Label");
	//--------------------------------------
	vtkStringArray* modelLabel = vtkStringArray::New();
	modelLabel->SetName("Model Label");
	//--------------------------------------


	vector<vtkDoubleArray*> properties;
	properties.push_back(capacityProp);
	properties.push_back(velocityProp);
	properties.push_back(widthProp);
	properties.push_back(dipProp);

	vtkPoints *points = vtkPoints::New();
	vtkCellArray * cells = vtkCellArray::New();

	int i = 0;

	double maxValue = 0;
	double len;
	double lenghtMin = VTK_DOUBLE_MAX; 
	double tab[4];

	double point1[3];
	double point2[3];

	//need a storage spot for all the cones
  vtkAppendPolyData* append = vtkAppendPolyData::New();

	int k = 0;
	while(!myFile.eof())
	{
		getline(myFile, line);
		split(line,lineSplit,",");
		if(lineSplit.size() < numberOfColumns)
		{
			continue;
		}

		point1[0] = atof(lineSplit[x1].c_str());
		point1[1] = atof(lineSplit[y1].c_str());
		point1[2] = atof(lineSplit[z1].c_str());
		
		point2[0] = atof(lineSplit[x2].c_str());
		point2[1] = atof(lineSplit[y2].c_str());
		point2[2] = atof(lineSplit[z2].c_str());

		points->InsertNextPoint(point1);
		points->InsertNextPoint(point2);

		lengthProp->InsertNextValue(atof(lineSplit[length].c_str()));
		lengthProp->InsertNextValue(atof(lineSplit[length].c_str()));

		modelProp->InsertNextValue(lineSplit[model]);
		modelProp->InsertNextValue(lineSplit[model]);


		idProp->InsertNextValue(lineSplit[id].c_str());
		idProp->InsertNextValue(lineSplit[id].c_str());

		tab[CAPACITY] = atof(lineSplit[capacity].c_str());
		tab[VELOCITY] = atof(lineSplit[velocity].c_str());
		tab[WIDTH] = atof(lineSplit[width].c_str());
		tab[DIP] = atof(lineSplit[dip].c_str());


		if(this->ShowArrowOn < 4)
		{
			if(tab[this->ShowArrowOn] > maxValue)
			{
				maxValue = tab[this->ShowArrowOn];
			}


			len = sqrt(vtkMath::Distance2BetweenPoints(point1,point2));
			if(len < lenghtMin)
			{
				lenghtMin = len;
			}

			lenProp->InsertNextValue(len);
			lenProp->InsertNextValue(len);
		}

		else
		{
			cells->InsertNextCell(2);
			cells->InsertCellPoint(k++);
			cells->InsertCellPoint(k++);
		}

		capacityProp->InsertNextValue(tab[CAPACITY]);
		capacityProp->InsertNextValue(tab[CAPACITY]);

		velocityProp->InsertNextValue(tab[VELOCITY]);
		velocityProp->InsertNextValue(tab[VELOCITY]);

		widthProp->InsertNextValue(tab[WIDTH]);
		widthProp->InsertNextValue(tab[WIDTH]);

		dipProp->InsertNextValue(tab[DIP]);
		dipProp->InsertNextValue(tab[DIP]);
	}

	if(this->ShowArrowOn == 4)
	{
		output->SetPoints(points);
		output->SetLines(cells);

		output->GetPointData()->AddArray(modelProp);
		output->GetPointData()->AddArray(lengthProp);
		output->GetPointData()->AddArray(capacityProp);
		output->GetPointData()->AddArray(velocityProp);
		output->GetPointData()->AddArray(widthProp);
		output->GetPointData()->AddArray(dipProp);
		output->GetPointData()->AddArray(idProp);

		append->Delete();

		modelArray->Delete();
		lengthArray->Delete();
		capacityArray->Delete();
		velocityArray->Delete();
		widthArray->Delete();
		dipArray->Delete();
		idArray->Delete();

		modelProp->Delete();
		lengthProp->Delete();
		capacityProp->Delete();
		velocityProp->Delete();
		widthProp->Delete();
		dipProp->Delete();
		idProp->Delete();

		modelLabel->Delete();
		lengthLabel->Delete();
		capacityLabel->Delete();
		velocityLabel->Delete();
		widthLabel->Delete();
		dipLabel->Delete();
		idLabel->Delete();

		points->Delete();
		cells->Delete();

		myFile.close();
		
		return 1;
	}

	for(i = 0; i < points->GetNumberOfPoints(); i+=2)
	{
		points->GetPoint(i , point1);
		points->GetPoint(i+1 , point2);

		len = lenProp->GetValue(i);
		this->Size = this->ScaleArrowSize*((properties[this->ShowArrowOn]->GetValue(i))/maxValue)*(lenghtMin);
		int numberOfPoints = this->AddArrow( append, point1, point2, len );

		modelArray->InsertNextValue(modelProp->GetValue(i));
		lengthArray->InsertNextValue(lengthProp->GetValue(i));
		capacityArray->InsertNextValue(capacityProp->GetValue(i));
		velocityArray->InsertNextValue(velocityProp->GetValue(i));
		widthArray->InsertNextValue(widthProp->GetValue(i));
		dipArray->InsertNextValue(dipProp->GetValue(i));
		idArray->InsertNextValue(idProp->GetValue(i));

		modelLabel->InsertNextValue(modelProp->GetValue(i));
		lengthLabel->InsertNextValue(doubleToString(lengthProp->GetValue(i)));
		capacityLabel->InsertNextValue(doubleToString(capacityProp->GetValue(i)));
		velocityLabel->InsertNextValue(doubleToString(velocityProp->GetValue(i)));
		widthLabel->InsertNextValue(doubleToString(widthProp->GetValue(i)));
		dipLabel->InsertNextValue(doubleToString(dipProp->GetValue(i)));
		idLabel->InsertNextValue(idProp->GetValue(i));

    for (int j=0; j < numberOfPoints-1; j++)
      {
				modelArray->InsertNextValue(modelProp->GetValue(i));
				lengthArray->InsertNextValue(lengthProp->GetValue(i));
				capacityArray->InsertNextValue(capacityProp->GetValue(i));
				velocityArray->InsertNextValue(velocityProp->GetValue(i));
				widthArray->InsertNextValue(widthProp->GetValue(i));
				dipArray->InsertNextValue(dipProp->GetValue(i));
				idArray->InsertNextValue(idProp->GetValue(i));

				modelLabel->InsertNextValue("");
				lengthLabel->InsertNextValue("");
				capacityLabel->InsertNextValue("");
				velocityLabel->InsertNextValue("");
				widthLabel->InsertNextValue("");
				dipLabel->InsertNextValue("");
				idLabel->InsertNextValue("");
      }
		
	}


  append->Update();
  output->DeepCopy(append->GetOutput());

	output->GetPointData()->AddArray(modelArray);
	output->GetPointData()->AddArray(lengthArray);
	output->GetPointData()->AddArray(capacityArray);
	output->GetPointData()->AddArray(velocityArray);
	output->GetPointData()->AddArray(widthArray);
	output->GetPointData()->AddArray(dipArray);
	output->GetPointData()->AddArray(idArray);

	output->GetPointData()->AddArray(modelLabel);
	output->GetPointData()->AddArray(lengthLabel);
	output->GetPointData()->AddArray(capacityLabel);
	output->GetPointData()->AddArray(velocityLabel);
	output->GetPointData()->AddArray(widthLabel);
	output->GetPointData()->AddArray(dipLabel);
	output->GetPointData()->AddArray(idLabel);

  append->Delete();

	modelArray->Delete();
	lengthArray->Delete();
	capacityArray->Delete();
	velocityArray->Delete();
	widthArray->Delete();
	dipArray->Delete();
	idArray->Delete();

	modelProp->Delete();
	lengthProp->Delete();
	capacityProp->Delete();
	velocityProp->Delete();
	widthProp->Delete();
	dipProp->Delete();
	idProp->Delete();

	modelLabel->Delete();
	lengthLabel->Delete();
	capacityLabel->Delete();
	velocityLabel->Delete();
	widthLabel->Delete();
	dipLabel->Delete();
	idLabel->Delete();

	points->Delete();

	myFile.close();
	
  return 1;
	
	}


//----------------------------------------------------------------------------------------
	unsigned int vtkTransportationReader::loadHeaders(ifstream &myFile)
{
	string line;
	vector<string> lineSplit;
	vector<string>::iterator it;

	getline(myFile, line);
	split(line,lineSplit,",");

	for(it = lineSplit.begin(); it!=lineSplit.end(); it++)
	{
		stringToLower(*it);	
	}

	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"drift name");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	id = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"x1");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	x1 = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"y1");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	y1 = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"z1");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	z1 = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"x2");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	x2 = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"y2");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	y2 = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"z2");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	z2 = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"model");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	model = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"length m");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	length = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"capacity t/h");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	capacity = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"velocity m/s");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	velocity = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"width mm");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	width = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"slope degree");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	dip = it - lineSplit.begin();


	return lineSplit.size();
}



//----------------------------------------------------------------------------
void vtkTransportationReader::CreateMatrix( double rotation[16], double *direction, double *center )
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

//----------------------------------------------------------------------------
int vtkTransportationReader::AddArrow( vtkAppendPolyData* &append, double* point1, double* point2, double height)
{
  double numberOfPoints = 0;
  double* direction = new double[3];
  double *center = new double[3];
  double radius = this->Size ;
  
  for (int i=0; i < 3; i++)
    {
    direction[i] = point2[i]-point1[i];  
    center[i] = (point2[i]+point1[i])/2.0;
    }    
   
	vtkPointSource *vert = vtkPointSource::New();
	vert->SetCenter(center);
	vert->SetNumberOfPoints(1);
	vert->Update();
	append->AddInput(vert->GetOutput());
	numberOfPoints += 1;

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
        
  
  
  /*here is the cone at the base of the line
  vtkConeSource *startCone = vtkConeSource::New();
  startCone->SetHeight( this->Size );
  startCone->SetRadius( radius );  
  startCone->SetCenter( point1 );
  startCone->SetDirection( direction );
  startCone->Update(); */
  
  //numberOfPoints += ( startCone->GetOutput()->GetNumberOfPoints() );
  
  //here is the cone that composes the middle section of the line
	if(3*radius < height)
	{
		vtkConeSource *midCone = vtkConeSource::New();
		midCone->SetHeight(3*radius);
		midCone->SetRadius( 2*radius );  
		midCone->SetCenter( center );
		midCone->SetDirection( direction );
		midCone->Update();  
	  
		numberOfPoints += ( midCone->GetOutput()->GetNumberOfPoints() );
		append->AddInput( midCone->GetOutput() ); 
		midCone->Delete();
	}
  append->AddInput( (vtkPolyData*) tf->GetOutput() );
  //append->AddInput( startCone->GetOutput() );
  
        
    
  //clean up the memory we have used
  tf->Delete();
  transform->Delete();
  cylinder->Delete();
	vert->Delete();
  
  delete[] direction;
  delete[] center;
  return numberOfPoints;
}





/*---------------------------------------------------------------------------------------------
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
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
//---------------------------------------------------------------------------------------------*/