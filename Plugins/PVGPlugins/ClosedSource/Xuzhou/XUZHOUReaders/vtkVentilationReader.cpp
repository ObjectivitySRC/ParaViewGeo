#include "vtkVentilationReader.h"

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
#include "vtkVectorText.h"
#include "vtkLineSource.h"
#include "vtkTextMapper.h"
#include "vtkPointSource.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"

/*
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkIdFilter.h"
#include "vtkRenderer.h"
#include "vtkSelectVisiblePoints.h"
#include "vtkLabeledDataMapper.h"
#include "vtkCellCenters.h"
#include "vtkLabeledDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTextProperty.h"
*/



enum
{
	VELOCITY=0,
	AREA, 
	VOLUM
};


using namespace std;

void split(const string& str,vector<string>& tokens,const string& delimiters);
void stringToLower(string &word);

vtkCxxRevisionMacro(vtkVentilationReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkVentilationReader);

// Constructor
vtkVentilationReader::vtkVentilationReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkVentilationReader::~vtkVentilationReader()
{
  this->SetFileName(0);
	this->Size = 10;
}

// --------------------------------------
void vtkVentilationReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}


// --------------------------------------
int vtkVentilationReader::CanReadFile( const char* fname )
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
int vtkVentilationReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{

	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *realOutput = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::New();

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
	vtkDoubleArray* msdateProp;
	msdateProp= vtkDoubleArray::New();
	msdateProp->SetName("msdate");
	//--------------------------------------
	vtkDoubleArray* velocityProp;
	velocityProp= vtkDoubleArray::New();
	velocityProp->SetName("velocity");
	//--------------------------------------
	vtkDoubleArray* areaProp;
	areaProp= vtkDoubleArray::New();
	areaProp->SetName("area");
	//--------------------------------------
	vtkDoubleArray* volumnProp;
	volumnProp= vtkDoubleArray::New();
	volumnProp->SetName("volume");
	//--------------------------------------
	vtkDoubleArray* lenghtProp;
	lenghtProp= vtkDoubleArray::New();
	lenghtProp->SetName("lenght");
	//-------------------------------------
	vtkIntArray* GazType = vtkIntArray::New();
	GazType->SetName("gaseous type");


	//--------------------------------------
	vtkStringArray* velocityLabel;
	velocityLabel= vtkStringArray::New();
	velocityLabel->SetName("Velocity Label");
	//--------------------------------------
	vtkStringArray* areaLabel;
	areaLabel= vtkStringArray::New();
	areaLabel->SetName("Area Label");
	//--------------------------------------
	vtkStringArray* volumeLabel;
	volumeLabel= vtkStringArray::New();
	volumeLabel->SetName("Volume Label");
	//--------------------------------------


	vtkPoints *points = vtkPoints::New();
	vtkCellArray *lines = vtkCellArray::New();
	vtkCellArray *verts = vtkCellArray::New();


	vtkIdType i = 0;

	double maxValue = 0;
	double lenght;
	double lenghtMin = VTK_DOUBLE_MAX; 
	double tab[3];

	double point1[3];
	double point2[3];
	double point3[3];

	//need a storage spot for all the cones
  vtkAppendPolyData* append = vtkAppendPolyData::New();

	vtkIdType k = 0;
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

		tab[VELOCITY] = atof(lineSplit[velocity].c_str()) ;
		tab[AREA] = atof(lineSplit[area].c_str()) ;
		tab[VOLUM] = atof(lineSplit[volum].c_str()) ;

		points->InsertNextPoint(point1);
		points->InsertNextPoint(point2);

		if(this->ShowArrowOn == 3)
		{
			msdateProp->InsertNextValue(atof(lineSplit[msdate].c_str()));
			msdateProp->InsertNextValue(atof(lineSplit[msdate].c_str()));

			velocityLabel->InsertNextValue("");
			velocityLabel->InsertNextValue("");

			areaLabel->InsertNextValue("");
			areaLabel->InsertNextValue("");

			volumeLabel->InsertNextValue("");
			volumeLabel->InsertNextValue("");

			point3[0] = (point1[0] + point2[0])/2;
			point3[1] = (point1[1] + point2[1])/2;
			point3[2] = (point1[2] + point2[2])/2;

			points->InsertNextPoint(point3);

			msdateProp->InsertNextValue(atof(lineSplit[msdate].c_str()));
			velocityLabel->InsertNextValue(lineSplit[velocity].c_str());
			areaLabel->InsertNextValue(lineSplit[area].c_str());
			volumeLabel->InsertNextValue(lineSplit[volum].c_str());

			lines->InsertNextCell(2);
			lines->InsertCellPoint(k++);
			lines->InsertCellPoint(k++);

			verts->InsertNextCell(1);
			verts->InsertCellPoint(k++);

			velocityProp->InsertNextValue(tab[VELOCITY]);
			velocityProp->InsertNextValue(tab[VELOCITY]);
			velocityProp->InsertNextValue(tab[VELOCITY]);

			areaProp->InsertNextValue(tab[AREA]);
			areaProp->InsertNextValue(tab[AREA]);
			areaProp->InsertNextValue(tab[AREA]);

			volumnProp->InsertNextValue(tab[VOLUM]);
			volumnProp->InsertNextValue(tab[VOLUM]);
			volumnProp->InsertNextValue(tab[VOLUM]);
		}

		else
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
	}

	if(this->ShowArrowOn == 3)
	{
		realOutput->SetPoints(points);
		realOutput->SetLines(lines);
		realOutput->SetVerts(verts);

		realOutput->GetPointData()->AddArray(msdateProp);
		realOutput->GetPointData()->AddArray(velocityProp);
		realOutput->GetPointData()->AddArray(areaProp);
		realOutput->GetPointData()->AddArray(volumnProp);

		realOutput->GetPointData()->AddArray(velocityLabel);
		realOutput->GetPointData()->AddArray(areaLabel);
		realOutput->GetPointData()->AddArray(volumeLabel);

		// free memory 
		msdateProp->Delete();
		velocityProp->Delete();
		areaProp->Delete();
		volumnProp->Delete();
		velocityLabel->Delete();
		areaLabel->Delete();
		volumeLabel->Delete();

		points->Delete();
		lines->Delete();
		verts->Delete();

		myFile.close();

		return 1;
	}


	myFile.clear();
	myFile.seekg(0,ios::beg);
	getline(myFile,line);


	i=0;
	double Msdate;
	double Velocity;
	double Area;
	double Volume;
	double temp;
	int gazType;

	vtkIdType counter = 0;

	while(!myFile.eof())
	{
		getline(myFile, line);
		split(line,lineSplit,",");
		if(lineSplit.size() < numberOfColumns)
		{
			continue;
		}

		points->GetPoint(i++ , point1);
		points->GetPoint(i++ , point2);
		lenght = lenghtProp->GetValue(i-1);

		Msdate = atof(lineSplit[msdate].c_str());
		Velocity = atof(lineSplit[velocity].c_str());
		Area = atof(lineSplit[area].c_str());
		Volume = atof(lineSplit[volum].c_str());

		msdateProp->InsertNextValue(Msdate);
		velocityProp->InsertNextValue(Velocity);
		areaProp->InsertNextValue(Area);
		volumnProp->InsertNextValue(Volume);


		switch(this->ShowArrowOn)
		{
		case 0:
			temp = Velocity;
			break;
		case 1:
			temp = Area;
			break;
		case 2:
			temp = Volume;
		}

		gazType = atoi(lineSplit[gazeousType].c_str());
		GazType->InsertNextValue(gazType);
		this->Size = this->ScaleArrowSize*(temp/maxValue)*(lenghtMin);
		int numberOfPoints;
		if(gazType)
		{
			numberOfPoints = this->AddArrow( append, point1, point2, lenght, 8 );
		}
		else
		{
			numberOfPoints = this->AddArrow( append, point1, point2, lenght, 2 );
		}

		velocityLabel->InsertNextValue(lineSplit[velocity].c_str());
		areaLabel->InsertNextValue(lineSplit[area].c_str());
		volumeLabel->InsertNextValue(lineSplit[volum].c_str());

		counter++;

		for (int j=1; j < numberOfPoints; j++)
			{        
				msdateProp->InsertNextValue(Msdate);
				velocityProp->InsertNextValue(Velocity);
				areaProp->InsertNextValue(Area);
				volumnProp->InsertNextValue(Volume);
				GazType->InsertNextValue(gazType);

				velocityLabel->InsertNextValue("");
				areaLabel->InsertNextValue("");
				volumeLabel->InsertNextValue("");
				counter++;
			}
		
	}

  append->Update();
	realOutput->DeepCopy(append->GetOutput());

	realOutput->GetPointData()->AddArray(msdateProp);
	realOutput->GetPointData()->AddArray(velocityProp);
	realOutput->GetPointData()->AddArray(areaProp);
	realOutput->GetPointData()->AddArray(volumnProp);

	realOutput->GetPointData()->AddArray(velocityLabel);
	realOutput->GetPointData()->AddArray(areaLabel);
	realOutput->GetPointData()->AddArray(volumeLabel);
	realOutput->GetPointData()->AddArray(GazType);

  append->Delete();

	msdateProp->Delete();
	velocityProp->Delete();
	areaProp->Delete();
	volumnProp->Delete();
	velocityLabel->Delete();
	areaLabel->Delete();
	volumeLabel->Delete();
	GazType->Delete();

	points->Delete();
	lines->Delete();
	verts->Delete();

	myFile.close();
  return 1;


	/*
	double bounds[6];
	realOutput->GetBounds(bounds);

	vtkSelectVisiblePoints * visPts;
	vtkSelectVisiblePoints * visCells;
	vtkPoints * pts;
	vtkRenderWindow * renWin;



	
  int xmin = 200;
  int xLength = 100;
  int xmax = xmin + xLength;
  int ymin = 200;
  int yLength = 100;
  int ymax = ymin + yLength;

  pts = vtkPoints::New();
  pts->InsertPoint( 0, xmin, ymin, 0 );
  pts->InsertPoint( 1, xmax, ymin, 0 );
  pts->InsertPoint( 2, xmax, ymax, 0 );
  pts->InsertPoint( 3, xmin, ymax, 0 );

  vtkCellArray * rect = vtkCellArray::New();
  rect->InsertNextCell( 5 );
  rect->InsertCellPoint( 0 );
  rect->InsertCellPoint( 1 );
  rect->InsertCellPoint( 2 );
  rect->InsertCellPoint( 3 );
  rect->InsertCellPoint( 0 );

  vtkPolyData * selectRect = vtkPolyData::New();
  selectRect->SetPoints( pts );
  selectRect->SetLines( rect );

	vtkPolyDataMapper2D * rectMapper = vtkPolyDataMapper2D::New();
  rectMapper->SetInput( selectRect );

	vtkActor2D * rectActor = vtkActor2D::New();
  rectActor->SetMapper( rectMapper );

  // Create a sphere and its associated mapper and actor.
	vtkSphereSource * sphere = vtkSphereSource::New();
	vtkPolyDataMapper *sphereMapper = vtkPolyDataMapper::New();
  sphereMapper->SetInputConnection(sphere->GetOutputPort());
  sphereMapper->GlobalImmediateModeRenderingOn();

	vtkActor *sphereActor = vtkActor::New();
  sphereActor->SetMapper( sphereMapper );

  // Generate data arrays containing point and cell ids
	vtkIdFilter * ids = vtkIdFilter::New();
  ids->SetInputConnection( sphere->GetOutputPort() );
  ids->PointIdsOn();
  ids->CellIdsOn();
  ids->FieldDataOn();

  // Create the renderer here because vtkSelectVisiblePoints needs it.
  vtkRenderer * ren1 = vtkRenderer::New();

  // Create labels for points
  visPts = vtkSelectVisiblePoints::New();
  visPts->SetInputConnection( ids->GetOutputPort() );
  visPts->SetRenderer( ren1 );
  visPts->SelectionWindowOn();
  visPts->SetSelection( xmin, xmin + xLength, ymin, ymin + yLength );

  // Create the mapper to display the point ids.  Specify the
  // format to use for the labels.  Also create the associated actor.
  vtkLabeledDataMapper * ldm = vtkLabeledDataMapper::New();
  ldm->SetInputConnection( visPts->GetOutputPort() );
  ldm->SetLabelModeToLabelFieldData();

  vtkActor2D * pointLabels = vtkActor2D::New();
  pointLabels->SetMapper( ldm );

  // Create labels for cells
  vtkCellCenters * cc = vtkCellCenters::New();
  cc->SetInputConnection( ids->GetOutputPort() );

  visCells = vtkSelectVisiblePoints::New();
  visCells->SetInputConnection( cc->GetOutputPort() );
  visCells->SetRenderer( ren1 );
  visCells->SelectionWindowOn();
  visCells->SetSelection( xmin, xmin + xLength, ymin, ymin + yLength );

  // Create the mapper to display the cell ids.  Specify the
  // format to use for the labels.  Also create the associated actor.
  vtkLabeledDataMapper * cellMapper = vtkLabeledDataMapper::New();
  cellMapper->SetInputConnection( visCells->GetOutputPort() );
  cellMapper->SetLabelModeToLabelFieldData();
  cellMapper->GetLabelTextProperty()->SetColor( 0, 1, 0 );

  vtkActor2D * cellLabels = vtkActor2D::New();
  cellLabels->SetMapper( cellMapper );

  // Create the RenderWindow and RenderWindowInteractor
  renWin = vtkRenderWindow::New();
  renWin->AddRenderer( ren1 );

  vtkRenderWindowInteractor * iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow( renWin );

  // Add the actors to the renderer; set the background and size; render
  ren1->AddActor( sphereActor );
  ren1->AddActor2D( rectActor );
  ren1->AddActor2D( pointLabels );
  ren1->AddActor2D( cellLabels );

  ren1->SetBackground( 1, 1, 1 );
  renWin->SetSize( 500, 500 );
  renWin->Render();

  iren->Initialize();
  iren->Start();*/

}



//----------------------------------------------------------------------------
int vtkVentilationReader::AddArrow( vtkAppendPolyData* &append, double* point1, 
																	 double* point2, double height, int coneResolution)
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

	vtkLineSource *line = vtkLineSource::New();
	line->SetPoint1(point1);
	line->SetPoint2(point2);
	line->Update();
	append->AddInput(line->GetOutput());

	numberOfPoints +=3;

	
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
  //append->AddInput( startCone->GetOutput() );
  
        
    
  //clean up the memory we have used
  tf->Delete();
  transform->Delete();
  cylinder->Delete();

  //midCone->Delete();
  //startCone->Delete();
  
	vert->Delete();
	line->Delete();

  delete[] direction;
  delete[] center;
  return numberOfPoints;
}





//----------------------------------------------------------------------------------------
void vtkVentilationReader::addItem(const char* text, double* point1, double* point2, vtkAppendPolyData* append)
{
  double direction[3];
  double center[3];
  
  for (int i=0; i < 3; i++)
    {
    direction[i] = point2[i]-point1[i];  
    center[i] = (point2[i]+point1[i])/2.0;
    } 

	vtkPointSource *point = vtkPointSource::New();


	//vtkTextMapper *textM = vtkTextMapper::New();
	//textM->SafeDownCast(point);
	//textM->SetInput("bla bla bla");

  //set up the transform for the cylinder
  double rotation[16];
  this->CreateMatrix( rotation, direction, center );
  vtkTransform *transform = vtkTransform::New();  
  transform->SetMatrix( rotation ); 
	transform->Scale(20,20,20);
	//transform->RotateY(90);
	transform->RotateY(90);

	vtkVectorText *label = vtkVectorText::New();
	label->SetText(text);

  vtkTransformFilter *tf = vtkTransformFilter::New();
  tf->SetTransform( transform );
	tf->SetInput( label->GetOutput() );  
  tf->Update();  

	append->AddInput( (vtkPolyData*) tf->GetOutput() );

	vtkLineSource *line = vtkLineSource::New();
	line->SetPoint1(point1);
	line->SetPoint2(point2);
	line->Update();

	append->AddInput(line->GetOutput());
	append->AddInput(point->GetOutput());

	//transform->Delete();
	//tf->Delete();
	//line->Delete();

}



//----------------------------------------------------------------------------------------
unsigned int vtkVentilationReader::loadHeaders(ifstream &myFile)
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
	it = find(lineSplit.begin(),lineSplit.end(),"node");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	Node = it - lineSplit.begin();
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
	it = find(lineSplit.begin(),lineSplit.end(),"msdate");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	msdate = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"velocity m/min");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	velocity = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"area m2");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	area = it - lineSplit.begin();
	//--------------------------------------
	it = find(lineSplit.begin(),lineSplit.end(),"volum m3/min");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	volum = it - lineSplit.begin();
	//--------------------------------------

	it = find(lineSplit.begin(),lineSplit.end(),"gaseous type");
	if(it == lineSplit.end())
	{
		myFile.close();
		return 0;
	}
	this->gazeousType = it - lineSplit.begin();
	//--------------------------------------

	return lineSplit.size();
}



//----------------------------------------------------------------------------
void vtkVentilationReader::CreateMatrix( double rotation[16], double *direction, double *center )
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
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
string doubleToString(double in)
{
	std::stringstream out;				
	out << in;				
	return out.str();
}
//---------------------------------------------------------------------------------------------