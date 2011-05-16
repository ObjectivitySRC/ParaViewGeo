//===================================================================================//
//				Mirarco Mining Innovation
//
// Filter:   Mine24DtoMap3D
// Class:    vtkMine24DtoMap3D
// Author:   Nehmeh Bilal
// Director: Marie-Grabrielle Valet 
// Date:     July 2008
// contact: nehmebilal@gmail.com 
//====================================================================================//
#include "vtkPolyDataAlgorithm.h"
#include "vtkMine24DtoMap3D.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellData.h"
#include <math.h>
#include <map>
#include <algorithm>
#include <vector>
#include <list>
#include<iostream>
#include<string>
#include<string.h>
#include<sstream>
#include<fstream>
#include<ostream>
#include "vtkCleanPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkDataArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"

using namespace std;

void split(const string& str,vector<string>& tokens,const string& delimiters);

vtkCxxRevisionMacro(vtkMine24DtoMap3D, "$Revision: 1.34 $");
vtkStandardNewMacro(vtkMine24DtoMap3D);

//----------------------------------------------------------------------------
vtkMine24DtoMap3D::vtkMine24DtoMap3D ()
{
	this->blue = 1;
	this->green = 2;
	this->yellow = 3;
	this->red = 4;
	this->grey = 5;

	this->ActivityFile = 0;
	this->activityFound = false;
	this->ActivityStep = 0;
	this->dateFormat = 0;
}

//----------------------------------------------------------------------------
vtkMine24DtoMap3D::~vtkMine24DtoMap3D()
{

}

//----------------------------------------------------------------------------

int vtkMine24DtoMap3D::RequestData(vtkInformation *vtkNotUsed(request), 
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *realInput = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *input = vtkPolyData::New();

//---------------------------------------- Clean Filter ----------------------------------------------//
																																																		//
	vtkCleanPolyData *connect = vtkCleanPolyData::New();
	connect->SetInput(realInput);
	connect->SetPieceInvariant(1);
	connect->SetTolerance(0);
	connect->SetAbsoluteTolerance(1);
	connect->SetToleranceIsAbsolute(0);
	connect->SetConvertLinesToPoints(1);
	connect->SetConvertPolysToLines(1);
	connect->SetConvertLinesToPoints(1);
	connect->SetPointMerging(1);
	connect->Update();
	 
	input->ShallowCopy(connect->GetOutput());
	connect->Delete();
																																																		//	
//----------------------------------------------------------------------------------------------------//

	vtkCellArray *inCells = input->GetPolys();
	vtkPoints *inPoints = input->GetPoints();

	int numberOfCells = inCells->GetNumberOfCells();


	vtkCellArray *outCells;
	outCells = vtkCellArray::New();
	vtkPoints *outPoints;
	outPoints = vtkPoints::New();

	vtkDataArray* M4DNUM;
	M4DNUM = input->GetCellData()->GetArray("M4DNUM  ");
	vtkDataArray* COLOUR;
	COLOUR = input->GetCellData()->GetArray("COLOUR  ");

	vtkIntArray* BlkNum = vtkIntArray::New();
	BlkNum->SetName("BlkNumber");

	vtkIntArray* MiningBlock = vtkIntArray::New();
	MiningBlock->SetName("Mining Block");

	map<int,list<int*>> segments;
	map<int,list<int*>> face1Triangles;
	map<int,list<int*>> face2Triangles;
	map<int,int*> face1Vertices;
	map<int,int*> face2Vertices;
	map<int,list<double>> zSegments;
	map<int,int> color;
	int stopeId = 0;
	int *triangle;
	int *facePoints;
	int *facePoints2;
	double *currentTuple;
	double vect1[3];
	double vect2[3];
	double vect3[3];
	double point1[3];
	double point2[3];
	double point3[3];
	double point4[3];

	double normal[3];
	const double pi = 3.1415926535897931;
	double zmed = 0;
	int num = 0;
	bool configFound = false;
	double angle = 0;
	double angle2 = 0;
	double angle3 = 0;
	bool *config;
	double norm1;
	double norm2;
	int k = 0;
	double cosAngle = 0;
	int countCells = 0;
	int ActNum;

	
	
	

	

	(inCells)->InitTraversal();

	
	vtkIdType *npts = new vtkIdType();	// will store the number of points in current cell 
	vtkIdType *pts;	// will store a array of points id's in current cell 
	//triangle = new int(3);

	for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
	{
		//inCells->GetCell(currentCell, *npts, *&pts);
		inCells->GetNextCell(*npts, *&pts);

		currentTuple = inPoints->GetPoint(pts[0]);
		point1[0] = currentTuple[0];
		point1[1] = currentTuple[1];
		point1[2] = currentTuple[2];
		
		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(pts[1]);
		point2[0] = currentTuple[0];
		point2[1] = currentTuple[1];
		point2[2] = currentTuple[2];

		currentTuple = NULL;

		vect1[0]= point2[0]-point1[0];
		vect1[1]= point2[1]-point1[1];
		vect1[2]= point2[2]-point1[2];

		currentTuple = inPoints->GetPoint(pts[2]);
		point3[0] = currentTuple[0];
		point3[1] = currentTuple[1];
		point3[2] = currentTuple[2];

		currentTuple = NULL;

		vect2[0]= point3[0]-point1[0];
		vect2[1]= point3[1]-point1[1];
		vect2[2]= point3[2]-point1[2];

		vtkMath::Cross(vect1, vect2, normal);
		vtkMath::Normalize(normal);

		

		if(abs(normal[2])> cos(this->AngleTolerance*(pi/180)))
		{
			triangle = new int[3];
			triangle[0] = pts[0];
			triangle[1] = pts[1];
			triangle[2] = pts[2];
			num = M4DNUM->GetComponent(currentCell,0);
			segments[num].push_back(triangle);
			zSegments[num].push_back((point1[2] + point2[2] + point3[2])/3);
			color[num] = COLOUR->GetComponent(currentCell,0);

			//triangle = NULL;
			//delete []triangle;
			
		}

	}

	

	
	
	for(map<int,list<int*>>::iterator iter=segments.begin(); iter!=segments.end(); iter++)
	{
		//zSegments[iter->first].sort();
		zmed = 0;
		int countZseg = 0;
		for(list<double>::iterator itZseg = zSegments[iter->first].begin(); itZseg != zSegments[iter->first].end(); itZseg++)
		{
			//zmed = (zSegments[iter->first].front() + zSegments[iter->first].back())/2;
			zmed += *itZseg;
			countZseg++;
		}
		zmed = zmed/countZseg;

		for(list<int*>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{
			currentTuple = NULL;
			currentTuple = inPoints->GetPoint((*iter2)[0]);
			if(currentTuple[2] > zmed)
			{
				face1Triangles[iter->first].push_back(*iter2);
			}
			else
				face2Triangles[iter->first].push_back(*iter2);
		}
		if((face1Triangles[iter->first].size()) < 2 || (face2Triangles[iter->first].size() < 2))
		{
			vtkErrorMacro(" Error in stope number: "<<iter->first);
			face1Triangles.erase(iter->first);
			face2Triangles.erase(iter->first);
		}
	}





	for(map<int,list<int*>>::iterator iter=face1Triangles.begin(); iter!=face1Triangles.end(); iter++)
	{
		map<int,double> angles;
		multimap<double,int> sortedAngles;
		for(list<int*>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{
			currentTuple = NULL;
			currentTuple = inPoints->GetPoint((*iter2)[0]);
			point1[0] = currentTuple[0];
			point1[1] = currentTuple[1];
			point1[2] = currentTuple[2];

			currentTuple = NULL;

			currentTuple = inPoints->GetPoint((*iter2)[1]);
			point2[0] = currentTuple[0];
			point2[1] = currentTuple[1];
			point2[2] = currentTuple[2];

			currentTuple = NULL;

			currentTuple = inPoints->GetPoint((*iter2)[2]);
			point3[0] = currentTuple[0];
			point3[1] = currentTuple[1];
			point3[2] = currentTuple[2];

			currentTuple = NULL;

			vect1[0]= point2[0]-point1[0];
			vect1[1]= point2[1]-point1[1];
			vect1[2]= point2[2]-point1[2];

			vect2[0]= point3[0]-point1[0];
			vect2[1]= point3[1]-point1[1];
			vect2[2]= point3[2]-point1[2];

			vect3[0]= point3[0]-point2[0];
			vect3[1]= point3[1]-point2[1];
			vect3[2]= point3[2]-point2[2]; 

			angle = (vect1[0]*vect2[0]) + (vect1[1]*vect2[1]) + (vect1[2]*vect2[2]);
			angle = angle /((vtkMath::Norm(vect1))*(vtkMath::Norm(vect2)));
			angle = acos(angle);

			angle2 = (-vect1[0]*vect3[0]) + (-vect1[1]*vect3[1]) + (-vect1[2]*vect3[2]);
			angle2 = angle2 /((vtkMath::Norm(vect1))*(vtkMath::Norm(vect3)));
			angle2 = acos(angle2);

			angle3 = (vect2[0]*vect3[0]) + (vect2[1]*vect3[1]) + (vect2[2]*vect3[2]);
			angle3 = angle3 /((vtkMath::Norm(vect2))*(vtkMath::Norm(vect3)));
			angle3 = acos(angle3);			

			//try
			//{
				angles[(*iter2)[0]] += angle;
				angles[(*iter2)[1]] += angle2; 
				angles[(*iter2)[2]] += angle3; 
			//}
			//catch(...)
			//{
			//	angles[(*iter2)[0]] = angle;
			//	angles[(*iter2)[1]] = angle2;
			//	angles[(*iter2)[2]] = angle3;
			//}
		}
		if(angles.size()<4)
		{
			vtkErrorMacro(" Error in stope number: "<<iter->first);
			continue;
		}

		

		for(map<int,double>::iterator it = angles.begin(); it != angles.end(); it++)
			sortedAngles.insert(pair<double,int>(it->second,it->first));

		facePoints = new int[4];
		facePoints2 = new int[4];

		int iAngle = 0;

		for(multimap<double,int>::iterator iter3 = sortedAngles.begin(); iter3 != sortedAngles.end(); iter3++)
		{
			facePoints[iAngle] = iter3->second;
			if(iAngle == 3)
				break;
			iAngle += 1;
		}
		

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(facePoints[0]);
		point1[0] = currentTuple[0];
		point1[1] = currentTuple[1];
		point1[2] = currentTuple[2];

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(facePoints[1]);
		point2[0] = currentTuple[0];
		point2[1] = currentTuple[1];
		point2[2] = currentTuple[2];

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(facePoints[2]);
		point3[0] = currentTuple[0];
		point3[1] = currentTuple[1];
		point3[2] = currentTuple[2];

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(facePoints[3]);
		point4[0] = currentTuple[0];
		point4[1] = currentTuple[1];
		point4[2] = currentTuple[2];

		currentTuple = NULL;

		config = goodConfig(point1,point2,point3,point4);
		if(config[0])
		{
			if(config[1])
			{
				face1Vertices[iter->first] = facePoints;
				
			}
			else
			{
				facePoints2[0] = facePoints[3];
				facePoints2[1] = facePoints[2];
				facePoints2[2] = facePoints[1];
				facePoints2[3] = facePoints[0];
				face1Vertices[iter->first] = facePoints2;
			}
		}
		
		else
		{
			config = goodConfig(point1,point2,point4,point3);
			if(config[0])
			{
				if(config[1])
				{
					facePoints2[0] = facePoints[0];
					facePoints2[1] = facePoints[1];
					facePoints2[2] = facePoints[3];
					facePoints2[3] = facePoints[2];
					face1Vertices[iter->first] = facePoints2;
					
				}
				else
				{
					facePoints2[0] = facePoints[2];
					facePoints2[1] = facePoints[3];
					facePoints2[2] = facePoints[1];
					facePoints2[3] = facePoints[0];
					face1Vertices[iter->first] = facePoints2;
				}
			}
			else
			{
				config = goodConfig(point1,point3,point2,point4);
				if(config[0])
				{
					if(config[1])
					{
						facePoints2[0] = facePoints[0];
						facePoints2[1] = facePoints[2];
						facePoints2[2] = facePoints[1];
						facePoints2[3] = facePoints[3];
						face1Vertices[iter->first] = facePoints2;

					}
					else
					{
						facePoints2[0] = facePoints[3];
						facePoints2[1] = facePoints[1];
						facePoints2[2] = facePoints[2];
						facePoints2[3] = facePoints[0];
						face1Vertices[iter->first] = facePoints2;
					}
				}
				else
				{
					config = goodConfig(point1,point3,point4,point2);
					if(config[0])
					{
						if(config[1])
						{
							facePoints2[0] = facePoints[0];
							facePoints2[1] = facePoints[2];
							facePoints2[2] = facePoints[3];
							facePoints2[3] = facePoints[1];
							face1Vertices[iter->first] = facePoints2;

						}
						else
						{
							facePoints2[0] = facePoints[1];
							facePoints2[1] = facePoints[3];
							facePoints2[2] = facePoints[2];
							facePoints2[3] = facePoints[0];
							face1Vertices[iter->first] = facePoints2;
						}
					}
					else
					{
						face1Vertices.erase(iter->first);
						face2Vertices.erase(iter->first);
						vtkErrorMacro("Error in Stope "<<iter->first);
					}
				}
			}
		}
		

	}
	
	

	for(map<int,list<int*>>::iterator iter=face2Triangles.begin(); iter!=face2Triangles.end(); iter++)
	{
		map<int,double> angles2;
		multimap<double,int> sortedAngles2;
		for(list<int*>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{
			currentTuple = NULL;

			currentTuple = inPoints->GetPoint((*iter2)[0]);
			point1[0] = currentTuple[0];
			point1[1] = currentTuple[1];
			point1[2] = currentTuple[2];

			currentTuple = NULL;

			currentTuple = inPoints->GetPoint((*iter2)[1]);
			point2[0] = currentTuple[0];
			point2[1] = currentTuple[1];
			point2[2] = currentTuple[2];

			currentTuple = NULL;

			currentTuple = inPoints->GetPoint((*iter2)[2]);
			point3[0] = currentTuple[0];
			point3[1] = currentTuple[1];
			point3[2] = currentTuple[2];

			currentTuple = NULL;

			vect1[0]= point2[0]-point1[0];
			vect1[1]= point2[1]-point1[1];
			vect1[2]= point2[2]-point1[2];

			vect2[0]= point3[0]-point1[0];
			vect2[1]= point3[1]-point1[1];
			vect2[2]= point3[2]-point1[2];

			vect3[0]= point3[0]-point2[0];
			vect3[1]= point3[1]-point2[1];
			vect3[2]= point3[2]-point2[2]; 

			angle = (vect1[0]*vect2[0]) + (vect1[1]*vect2[1]) + (vect1[2]*vect2[2]);
			//angle = vtkMath::Dot(vect1,vect2);
			angle = angle /((vtkMath::Norm(vect1))*(vtkMath::Norm(vect2)));
			angle = acos(angle);

			angle2 = (-vect1[0]*vect3[0]) + (-vect1[1]*vect3[1]) + (-vect1[2]*vect3[2]);
			angle2 = angle2 /((vtkMath::Norm(vect1))*(vtkMath::Norm(vect3)));
			angle2 = acos(angle2);

			angle3 = (vect2[0]*vect3[0]) + (vect2[1]*vect3[1]) + (vect2[2]*vect3[2]);
			angle3 = angle3 /((vtkMath::Norm(vect3))*(vtkMath::Norm(vect2)));
			angle3 = acos(angle3);			

			//try
			//{
				angles2[(*iter2)[0]] += angle;
				angles2[(*iter2)[1]] += angle2; 
				angles2[(*iter2)[2]] += angle3; 
			//}
			//catch(...)
			//{
			//	angles2[(*iter2)[0]] = angle;
			//	angles2[(*iter2)[1]] = angle2;
			//	angles2[(*iter2)[2]] = angle3;
			//}
		}

		if(angles2.size()<4)
		{
			vtkErrorMacro(" Error in stope number: "<<iter->first);
			continue;
		}

		for(map<int,double>::iterator it = angles2.begin(); it != angles2.end(); it++)
			sortedAngles2.insert(pair<double,int>(it->second,it->first));

		facePoints = new int[4];
		facePoints2 = new int[4];

		int iAngle = 0;
		for(multimap<double,int>::iterator iter3 = sortedAngles2.begin(); iter3 != sortedAngles2.end(); iter3++)
		{			
			facePoints[iAngle] = iter3->second;
			if(iAngle == 3)
				break;
			iAngle += 1;
		}
		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(facePoints[0]);
		point1[0] = currentTuple[0];
		point1[1] = currentTuple[1];
		point1[2] = currentTuple[2];

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(facePoints[1]);
		point2[0] = currentTuple[0];
		point2[1] = currentTuple[1];
		point2[2] = currentTuple[2];

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(facePoints[2]);
		point3[0] = currentTuple[0];
		point3[1] = currentTuple[1];
		point3[2] = currentTuple[2];

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(facePoints[3]);
		point4[0] = currentTuple[0];
		point4[1] = currentTuple[1];
		point4[2] = currentTuple[2];

		currentTuple = NULL;

		config = goodConfig(point1,point2,point3,point4);
		if(config[0])
		{
			if(config[1])
			{
				face2Vertices[iter->first] = facePoints;
				
			}
			else
			{
				facePoints2[0] = facePoints[3];
				facePoints2[1] = facePoints[2];
				facePoints2[2] = facePoints[1];
				facePoints2[3] = facePoints[0];
				face2Vertices[iter->first] = facePoints2;
			}
		}
		
		else
		{
			config = goodConfig(point1,point2,point4,point3);
			if(config[0])
			{
				if(config[1])
				{
					facePoints2[0] = facePoints[0];
					facePoints2[1] = facePoints[1];
					facePoints2[2] = facePoints[3];
					facePoints2[3] = facePoints[2];
					face2Vertices[iter->first] = facePoints2;
					
				}
				else
				{
					facePoints2[0] = facePoints[2];
					facePoints2[1] = facePoints[3];
					facePoints2[2] = facePoints[1];
					facePoints2[3] = facePoints[0];
					face2Vertices[iter->first] = facePoints2;
				}
			}
			else
			{
				config = goodConfig(point1,point3,point2,point4);
				if(config[0])
				{
					if(config[1])
					{
						facePoints2[0] = facePoints[0];
						facePoints2[1] = facePoints[2];
						facePoints2[2] = facePoints[1];
						facePoints2[3] = facePoints[3];
						face2Vertices[iter->first] = facePoints2;

					}
					else
					{
						facePoints2[0] = facePoints[3];
						facePoints2[1] = facePoints[1];
						facePoints2[2] = facePoints[2];
						facePoints2[3] = facePoints[0];
						face2Vertices[iter->first] = facePoints2;
					}
				}
				else
				{
					config = goodConfig(point1,point3,point4,point2);
					if(config[0])
					{
						if(config[1])
						{
							facePoints2[0] = facePoints[0];
							facePoints2[1] = facePoints[2];
							facePoints2[2] = facePoints[3];
							facePoints2[3] = facePoints[1];
							face2Vertices[iter->first] = facePoints2;

						}
						else
						{
							facePoints2[0] = facePoints[1];
							facePoints2[1] = facePoints[3];
							facePoints2[2] = facePoints[2];
							facePoints2[3] = facePoints[0];
							face2Vertices[iter->first] = facePoints2;
						}
					}
					else
					{
						face2Vertices.erase(iter->first);
						face1Vertices.erase(iter->first);
						vtkErrorMacro("Error in Stope "<<iter->first);
					}
				}
			
			}
		}
	}


	
	map<string,int> activityNumbers;
	//this->dateFormat = 2;

	if(WithActivity)
	{
		multimap<int,string> activityDate;
		string line;
		ifstream myFile;
		string date1;
		vector<string> date2;
		vector<string> lineSplit;
		int dateIndex = 0;
		int m4dnumIndex = 0;
		int date;
		int tasks = 0;
		string id;
		int act = 1;
		int previousDate = 0;

		myFile.open(this->ActivityFile);
		if(myFile.fail())
		{
			vtkErrorMacro("Activity file not found !"<<endl<<"   your path must look like: C:"<<"\\abc.txt ");
		}
		else
		{
			this->activityFound = true;

			while(!myFile.eof())
			{
				getline(myFile, line);
				if(line.substr(0,14) == "1,Dependencies")
					break;

				if(tasks == 2)
				{
					split(line,lineSplit,",");
					if(lineSplit.size()<m4dnumIndex)
					{
						vtkErrorMacro("Activity Error: check activity file"<<endl);
						this->activityFound = false;
						break;
					}

					id = lineSplit[1];
					if(id[0] == 'S')
					{
						date1 = lineSplit[dateIndex];
						if(this->dateFormat == 1)
						{
							split(date1,date2,"-");
							if(date2.size()<3)
							{
								vtkErrorMacro("Activity Error: check date format"<<endl);
								this->activityFound = false;
								break;
							}
							date2[2] = date2[2].substr(0,2);
							date = (atoi(date2[0].c_str()))*365 + (atoi(date2[1].c_str()))*30 + (atoi(date2[2].c_str()));
						}
						else if(this->dateFormat == 2)
						{
							split(date1,date2,"/");
							if(date2.size()<3)
							{
								vtkErrorMacro("Activity Error: check date format"<<endl);
								this->activityFound = false;
								break;
							}
							date2[2] = date2[2].substr(0,4);
							date = (atoi(date2[2].c_str()))*365 + (atoi(date2[1].c_str()))*30 + (atoi(date2[0].c_str()));
						}
						else
						{
							vtkErrorMacro("Activity Error: Select a date format"<<endl);
							this->activityFound = false;
							break;
						}
						activityDate.insert(pair<int,string>(date,lineSplit[m4dnumIndex])) ;

					}
				}
				if(tasks == 1)
				{
					tasks++;
					split(line,lineSplit,",");
					vector<string>::iterator it;
					it = find(lineSplit.begin(),lineSplit.end(),"Finish");
					dateIndex = it - lineSplit.begin();
					m4dnumIndex = find(lineSplit.begin(),lineSplit.end(),"M4DNUM") - lineSplit.begin();
				}
				if(line.substr(0,7) == "1,Tasks")
					tasks++;
			}
			myFile.close();

			if(this->activityFound)
			{
				act = 1;
				multimap<int,string>::iterator itAct = activityDate.begin();

				previousDate = itAct->first;

				for( itAct = activityDate.begin(); itAct != activityDate.end(); itAct++)
				{
					if(((itAct->first) - previousDate) > this->ActivityStep)
					{
						previousDate = itAct->first;
						activityNumbers[itAct->second] = act++;
						
					}
					else
						activityNumbers[itAct->second] = act;
				}
			}
		}
	}

			
	
	
	//outPoints->Allocate(8*(face1Vertices.size()));
	outCells->Allocate(2*(face1Vertices.size()));

	double cosAngleTemp = 0;
	int count = 0;
	
	for(map<int,int*>::iterator iter = face1Vertices.begin(); iter != face1Vertices.end(); iter++)
	{
		try{
		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(iter->second[0]);
		point1[0] = currentTuple[0];
		point1[1] = currentTuple[1];
		point1[2] = currentTuple[2];

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(iter->second[1]);
		point2[0] = currentTuple[0];
		point2[1] = currentTuple[1];
		point2[2] = currentTuple[2];

		currentTuple = NULL;

		vect1[0] = point2[0] - point1[0];
		vect1[1] = point2[1] - point1[1];
		vect1[2] = point2[2] - point1[2];
		norm1 = vtkMath::Norm(vect1);



		k = 0;
		cosAngle = 0;
		int index = 0;
		int face2[4];

		
		face2[0] = face2Vertices[iter->first][0];
		face2[1] = face2Vertices[iter->first][1];
		face2[2] = face2Vertices[iter->first][2];
		face2[3] = face2Vertices[iter->first][3];
		
		

		for(int i=0; i<4; i++)
		{
			currentTuple = NULL;
			currentTuple = inPoints->GetPoint(face2[k]);
			point1[0] = currentTuple[0];
			point1[1] = currentTuple[1];
			point1[2] = currentTuple[2];
			if(k==3)
			{
				currentTuple = NULL;
				currentTuple = inPoints->GetPoint(face2[0]);
				point2[0] = currentTuple[0];
				point2[1] = currentTuple[1];
				point2[2] = currentTuple[2];
			}
			else
			{
				currentTuple = NULL;
				currentTuple = inPoints->GetPoint(face2[k+1]);
				point2[0] = currentTuple[0];
				point2[1] = currentTuple[1];
				point2[2] = currentTuple[2];
			}

			currentTuple = NULL;

			k += 1;

			vect2[0] = point2[0] - point1[0];
			vect2[1] = point2[1] - point1[1];
			vect2[2] = point2[2] - point1[2];
			norm2 = vtkMath::Norm(vect2);

			cosAngleTemp = (vtkMath::Dot(vect1,vect2))/(norm1*norm2);

			if(cosAngleTemp > cosAngle)
			{
				cosAngle = cosAngleTemp;
				index = k-1;
			}
			//delete point1;
			//delete point2;
			//point1 = NULL;
			//point2 = NULL;
		}

	

		int idTemp[4];
		for(int i = 0; i<4; i++)
		{
			idTemp[i] = face2[index];
			index += 1;
			if(index == 4)
				index = 0;
		}

		//face2Vertices[iter->first] = idTemp;

		

		currentTuple = NULL;
		currentTuple = inPoints->GetPoint(iter->second[0]);
		outPoints->InsertNextPoint(currentTuple[0],currentTuple[1],currentTuple[2]);
		
		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(iter->second[1]);
		outPoints->InsertNextPoint(currentTuple[0],currentTuple[1],currentTuple[2]);

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(iter->second[2]);
		outPoints->InsertNextPoint(currentTuple[0],currentTuple[1],currentTuple[2]);

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(iter->second[3]);
		outPoints->InsertNextPoint(currentTuple[0],currentTuple[1],currentTuple[2]);

		currentTuple = NULL;

		
		currentTuple = inPoints->GetPoint(idTemp[0]);
		outPoints->InsertNextPoint(currentTuple[0],currentTuple[1],currentTuple[2]);

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(idTemp[1]);
		outPoints->InsertNextPoint(currentTuple[0],currentTuple[1],currentTuple[2]);

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(idTemp[2]);
		outPoints->InsertNextPoint(currentTuple[0],currentTuple[1],currentTuple[2]);

		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(idTemp[3]);
		outPoints->InsertNextPoint(currentTuple[0],currentTuple[1],currentTuple[2]);

		stopeId = color[iter->first];
		stopeId = this->getColor(stopeId);

		currentTuple = NULL;
		
		outCells->InsertNextCell(4);
		outCells->InsertCellPoint(count);
		outCells->InsertCellPoint(count + 1);
		outCells->InsertCellPoint(count + 2);
		outCells->InsertCellPoint(count + 3);

		BlkNum->InsertValue(countCells,stopeId);
		countCells ++;
		
		outCells->InsertNextCell(4);
		outCells->InsertCellPoint(count + 4);
		outCells->InsertCellPoint(count + 5);
		outCells->InsertCellPoint(count + 6);
		outCells->InsertCellPoint(count + 7);

		BlkNum->InsertValue(countCells,stopeId);
		countCells ++;

		outCells->InsertNextCell(4);
		outCells->InsertCellPoint(count + 0);
		outCells->InsertCellPoint(count + 1);
		outCells->InsertCellPoint(count + 5);
		outCells->InsertCellPoint(count + 4);

		BlkNum->InsertValue(countCells,stopeId);
		countCells ++;

		outCells->InsertNextCell(4);
		outCells->InsertCellPoint(count + 3);
		outCells->InsertCellPoint(count + 2);
		outCells->InsertCellPoint(count + 6);
		outCells->InsertCellPoint(count + 7);

		BlkNum->InsertValue(countCells,stopeId);
		countCells ++;

		outCells->InsertNextCell(4);
		outCells->InsertCellPoint(count + 0);
		outCells->InsertCellPoint(count + 4);
		outCells->InsertCellPoint(count + 7);
		outCells->InsertCellPoint(count + 3);

		BlkNum->InsertValue(countCells,stopeId);
		countCells ++;

		outCells->InsertNextCell(4);
		outCells->InsertCellPoint(count + 1);
		outCells->InsertCellPoint(count + 5);
		outCells->InsertCellPoint(count + 6);
		outCells->InsertCellPoint(count + 2);

		BlkNum->InsertValue(countCells,stopeId);

		if(this->activityFound)
		{
			ostringstream stringActNum;
			stringActNum << iter->first;
			ActNum = activityNumbers[stringActNum.str()];
			MiningBlock->InsertValue(countCells,ActNum);
			MiningBlock->InsertValue(countCells-1,ActNum);
			MiningBlock->InsertValue(countCells-2,ActNum);
			MiningBlock->InsertValue(countCells-3,ActNum);
			MiningBlock->InsertValue(countCells-4,ActNum);
			MiningBlock->InsertValue(countCells-5,ActNum);
		}

		countCells ++;
		
		count += 8;
	}
	catch(...)
	{
		vtkErrorMacro(" Error in Stope or Drifts number: "<<iter->first);
		continue;
	}
	}
	

	output->SetPoints(outPoints);
	output->SetPolys(outCells);
	output->GetCellData()->AddArray(BlkNum);

	if(this->activityFound)
	{
		output->GetCellData()->AddArray(MiningBlock);
		MiningBlock->Delete();
	}

	BlkNum->Delete();
	

	this->blue = 1;
	this->green = 2;
	this->yellow = 3;
	this->red = 4;
	this->grey = 5;



  return 1;
}

//----------------------------------------------------------------------------
int vtkMine24DtoMap3D::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

//----------------------------------------------------------------------------
void vtkMine24DtoMap3D::PrintSelf(ostream& os, vtkIndent indent)
{
}
//----------------------------------------------------------------------------


/*
int* getVertices(vector<vector<int>> face, inPoints)
{
	map<int,double> angles;
	for(vector<vector<int>>::iterator iter = face.begin(); iter = face.end(); iter++)
	{
		double *point1;
		double *point2;
		double *point3;

		point1 = this->inPoints->GetPoint(pts[0]);
		point2 = this->inPoints->GetPoint(pts[1]);

	}
	
}
*/

//----------------------------------------------------------------------------
bool* vtkMine24DtoMap3D::goodConfig(double* pt1, double* pt2, double*pt3, double* pt4)
{
	double v1[3];
	double v2[3];
	double v3[3];
	double v4[3];

	bool *config = new bool[2];

	double normal1[3];
	double normal2[3];

	v1[0] = pt1[0]-pt2[0];
	v1[1] = pt1[1]-pt2[1];
	v1[2] = pt1[2]-pt2[2];

	v2[0] = pt3[0]-pt2[0];
	v2[1] = pt3[1]-pt2[1];
	v2[2] = pt3[2]-pt2[2];

	v3[0] = pt1[0]-pt3[0];
	v3[1] = pt1[1]-pt3[1];
	v3[2] = pt1[2]-pt3[2];

	v4[0] = pt4[0]-pt3[0];
	v4[1] = pt4[1]-pt3[1];
	v4[2] = pt4[2]-pt3[2];

	vtkMath::Cross(v2,v1,normal1);
	vtkMath::Cross(v4,v3,normal2);

	if(normal1[2]*normal2[2]>0)
	{
		config[0] = true;
		if(normal1[2]>0)
		{
			config[1] = true;
			return config;
		}
		else
		{
			config[1] = false;
			return config;
		}
	}
	else
	{
		config[0] = false;
		config[1] = false;
		return config;
	}

}


//----------------------------------------------------------------------------
int vtkMine24DtoMap3D::getColor(int color)
{
	int id;

	if( (color-1) % 5 == 0)
	{
		id = this->blue;
		this->blue += 5;
		return id;
	}

	if( (color-2) % 5 == 0)
	{
		id = this->green;
		this->green += 5;
		return id;
	}

	if( (color-3) % 5 == 0)
	{
		id = this->yellow;
		this->yellow += 5;
		return id;
	}

		if( (color-4) % 5 == 0)
	{
		id = this->red;
		this->red += 5;
		return id;
	}
	
		id = this->grey;
		this->grey += 5;
		return this->grey;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vtkMine24DtoMap3D::readActivity(char* ActivityFile)
{

}






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