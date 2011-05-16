#include "vtkElectricalReader.h"

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
	CABLE=0,
	SWITCH, 
	TRANSFORMER
};


using namespace std;

void split(const string& str,vector<string>& tokens,const string& delimiters);
void stringToLower(string &word);
string doubleToString(double in);

vtkCxxRevisionMacro(vtkElectricalReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkElectricalReader);

// Constructor
vtkElectricalReader::vtkElectricalReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkElectricalReader::~vtkElectricalReader()
{
  this->SetFileName(0);
	this->Size = 10;
}

// --------------------------------------
void vtkElectricalReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}


// --------------------------------------
int vtkElectricalReader::CanReadFile( const char* fname )
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
int vtkElectricalReader::RequestData(vtkInformation* request,
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
	vtkDoubleArray* idProp;
	idProp= vtkDoubleArray::New();
	idProp->SetName("id");
	//--------------------------------------
	vtkStringArray* tunnel_nameProp;
	tunnel_nameProp= vtkStringArray::New();
	tunnel_nameProp->SetName("Tunnel_Name");
	//--------------------------------------
	vtkStringArray* modelProp;
	modelProp= vtkStringArray::New();
	modelProp->SetName("model");
	//--------------------------------------
	vtkDoubleArray* cableProp;
	cableProp= vtkDoubleArray::New();
	cableProp->SetName("cable");
	//--------------------------------------
	vtkDoubleArray* switchProp;
	switchProp= vtkDoubleArray::New();
	switchProp->SetName("switch");
	//--------------------------------------
	vtkDoubleArray* transformerProp;
	transformerProp= vtkDoubleArray::New();
	transformerProp->SetName("transformer");
	//--------------------------------------
	vtkDoubleArray* lenghtProp;
	lenghtProp= vtkDoubleArray::New();
	lenghtProp->SetName("lenght");
	//--------------------------------------


	//--------------------------------------
	vtkDoubleArray* idArray;
	idArray= vtkDoubleArray::New();
	idArray->SetName("id");
	//--------------------------------------
	vtkStringArray* tunnel_nameArray;
	tunnel_nameArray= vtkStringArray::New();
	tunnel_nameArray->SetName("Tunnel_Name");
	//--------------------------------------
	vtkStringArray* modelArray;
	modelArray= vtkStringArray::New();
	modelArray->SetName("model");
	//--------------------------------------
	vtkDoubleArray* cableArray;
	cableArray= vtkDoubleArray::New();
	cableArray->SetName("cable");
	//--------------------------------------
	vtkDoubleArray* switchArray;
	switchArray= vtkDoubleArray::New();
	switchArray->SetName("switch");
	//--------------------------------------
	vtkDoubleArray* transformerArray;
	transformerArray = vtkDoubleArray::New();
	transformerArray->SetName("transformer");
	//--------------------------------------

	//--------------------------------------
	vtkStringArray* cableLabel = vtkStringArray::New();
	cableLabel->SetName("Cable Array");
	//--------------------------------------
	vtkStringArray* switchLabel = vtkStringArray::New();
	switchLabel->SetName("Switch Label");
	//--------------------------------------
	vtkStringArray* transformerLabel = vtkStringArray::New();
	transformerLabel->SetName("Transformer Label");
	//--------------------------------------
	vtkStringArray* idLabel = vtkStringArray::New();;
	idLabel->SetName("Id Label");
	//--------------------------------------
	vtkStringArray* tunnel_nameLabel = vtkStringArray::New();
	tunnel_nameLabel->SetName("Tunnel_Name Label");
	//--------------------------------------
	vtkStringArray* modelLabel = vtkStringArray::New();
	modelLabel->SetName("Model Label");
	//--------------------------------------

	vector<vtkDoubleArray*> properties;
	properties.push_back(cableProp);
	properties.push_back(switchProp);
	properties.push_back(transformerProp);

	vtkPoints *points = vtkPoints::New();
	vtkCellArray *cells = vtkCellArray::New();

	int i = 0;

	double maxValue = 0;
	double lenght;
	double lenghtMin = VTK_DOUBLE_MAX; 
	double tab[3];

	double point1[3];
	double point2[3];

	//need a storage spot for all the cones
  vtkAppendPolyData* append = vtkAppendPolyData::New();

	int k=0;
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

		idProp->InsertNextValue(atof(lineSplit[id].c_str()));
		idProp->InsertNextValue(atof(lineSplit[id].c_str()));

		tunnel_nameProp->InsertNextValue(lineSplit[tunnel_name].c_str());
		tunnel_nameProp->InsertNextValue(lineSplit[tunnel_name].c_str());

		modelProp->InsertNextValue(lineSplit[model].c_str());
		modelProp->InsertNextValue(lineSplit[model].c_str());

		tab[CABLE] = atof(lineSplit[cable].c_str());
		tab[SWITCH] = atof(lineSplit[Switch].c_str()) ;
		tab[TRANSFORMER] = atof(lineSplit[transformer].c_str());


		if(this->ShowArrowOn < 3)
		{
			if(tab[this->ShowArrowOn] > maxValue)
			{
				maxValue = tab[this->ShowArrowOn];
			}

			lenght = sqrt(vtkMath::Distance2BetweenPoints(point1,point2));
			if(lenght < lenghtMin)
			{
				lenghtMin = lenght;
			}

			lenghtProp->InsertNextValue(lenght);
			lenghtProp->InsertNextValue(lenght);
		}

		else
		{
			cells->InsertNextCell(2);
			cells->InsertCellPoint(k++);
			cells->InsertCellPoint(k++);
		}

		cableProp->InsertNextValue(tab[CABLE]);
		cableProp->InsertNextValue(tab[CABLE]);

		switchProp->InsertNextValue(tab[SWITCH]);
		switchProp->InsertNextValue(tab[SWITCH]);

		transformerProp->InsertNextValue(tab[TRANSFORMER]);
		transformerProp->InsertNextValue(tab[TRANSFORMER]);
	}


	if(this->ShowArrowOn == 3)
	{
		output->SetPoints(points);
		output->SetLines(cells);

		output->GetPointData()->AddArray(idProp);
		output->GetPointData()->AddArray(tunnel_nameProp);
		output->GetPointData()->AddArray(modelProp);
		output->GetPointData()->AddArray(cableProp);
		output->GetPointData()->AddArray(switchProp);
		output->GetPointData()->AddArray(transformerProp);

		append->Delete();

		idProp->Delete();
		tunnel_nameProp->Delete();
		modelProp->Delete();
		cableProp->Delete();
		switchProp->Delete();
		transformerProp->Delete();
		lenghtProp->Delete();

		idArray->Delete();
		tunnel_nameArray->Delete();
		modelArray->Delete();
		cableArray->Delete();
		switchArray->Delete();
		transformerArray->Delete();

		points->Delete();
		cells->Delete();

		myFile.close();
		return 1;
	}

	for(i = 0; i < points->GetNumberOfPoints(); i+=2)
	{
		points->GetPoint(i , point1);
		points->GetPoint(i+1 , point2);

		lenght = lenghtProp->GetValue(i);
		this->Size = this->ScaleArrowSize*((properties[this->ShowArrowOn]->GetValue(i))/maxValue)*(lenghtMin);
		int numberOfPoints = this->AddArrow( append, point1, point2, lenght );

		idArray->InsertNextValue(idProp->GetValue(i));
		tunnel_nameArray->InsertNextValue(tunnel_nameProp->GetValue(i));
		modelArray->InsertNextValue(modelProp->GetValue(i));
		cableArray->InsertNextValue(cableProp->GetValue(i));
		switchArray->InsertNextValue(switchProp->GetValue(i));
		transformerArray->InsertNextValue(transformerProp->GetValue(i));

		idLabel->InsertNextValue(doubleToString(idProp->GetValue(i)));
		tunnel_nameLabel->InsertNextValue(tunnel_nameProp->GetValue(i));
		modelLabel->InsertNextValue(modelProp->GetValue(i));
		cableLabel->InsertNextValue(doubleToString(cableProp->GetValue(i)));
		switchLabel->InsertNextValue(doubleToString(switchProp->GetValue(i)));
		transformerLabel->InsertNextValue(doubleToString(transformerProp->GetValue(i)));

    for (int j=0; j < numberOfPoints-1; j++)
      {        
				idArray->InsertNextValue(idProp->GetValue(i));
				tunnel_nameArray->InsertNextValue(tunnel_nameProp->GetValue(i));
				modelArray->InsertNextValue(modelProp->GetValue(i));
				cableArray->InsertNextValue(cableProp->GetValue(i));
				switchArray->InsertNextValue(switchProp->GetValue(i));
				transformerArray->InsertNextValue(transformerProp->GetValue(i));

				idLabel->InsertNextValue("");
				tunnel_nameLabel->InsertNextValue("");
				modelLabel->InsertNextValue("");
				cableLabel->InsertNextValue("");
				switchLabel->InsertNextValue("");
				transformerLabel->InsertNextValue("");
      }
		
	}


  append->Update();
  output->DeepCopy(append->GetOutput());

	output->GetPointData()->AddArray(idArray);
	output->GetPointData()->AddArray(tunnel_nameArray);
	output->GetPointData()->AddArray(modelArray);
	output->GetPointData()->AddArray(cableArray);
	output->GetPointData()->AddArray(switchArray);
	output->GetPointData()->AddArray(transformerArray);

	output->GetPointData()->AddArray(idLabel);
	output->GetPointData()->AddArray(tunnel_nameLabel);
	output->GetPointData()->AddArray(modelLabel);
	output->GetPointData()->AddArray(cableLabel);
	output->GetPointData()->AddArray(switchLabel);
	output->GetPointData()->AddArray(transformerLabel);

  append->Delete();

	idProp->Delete();
	tunnel_nameProp->Delete();
	modelProp->Delete();
	cableProp->Delete();
	switchProp->Delete();
	transformerProp->Delete();
	lenghtProp->Delete();

	idArray->Delete();
	tunnel_nameArray->Delete();
	modelArray->Delete();
	cableArray->Delete();
	switchArray->Delete();
	transformerArray->Delete();

	idLabel->Delete();
	tunnel_nameLabel->Delete();
	modelLabel->Delete();
	cableLabel->Delete();
	switchLabel->Delete();
	transformerLabel->Delete();

	points->Delete();

	myFile.close();
  return 1;
}


//----------------------------------------------------------------------------------------
unsigned int vtkElectricalReader::loadHeaders(ifstream &myFile)
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
	it = find(lineSplit.begin(),lineSplit.end(),"id");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	id = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"tunnel_name");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	tunnel_name = it - lineSplit.begin();
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
	it = find(lineSplit.begin(),lineSplit.end(),"cable");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	cable = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"switch");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	Switch = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"transformer");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	transformer = it - lineSplit.begin();


	return lineSplit.size();
}



//----------------------------------------------------------------------------
void vtkElectricalReader::CreateMatrix( double rotation[16], double *direction, double *center )
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
int vtkElectricalReader::AddArrow( vtkAppendPolyData* &append, double* point1, double* point2, double height)
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

