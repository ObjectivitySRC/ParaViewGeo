
#include "vtkDriftsAndStopesReconstruction.h"

#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include <math.h>
#include<iostream>
#include<conio.h>
#include "vtkCleanPolyData.h"
#include <map>
#include <algorithm>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include "vtkMath.h"
#include "vtkIntArray.h"
#include "vtkDataArray.h"


using namespace std;

vtkStandardNewMacro(vtkDriftsAndStopesReconstruction);
vtkCxxRevisionMacro(vtkDriftsAndStopesReconstruction, "$Revision: 1.34 $");

//----------------------------------------------------------------------------
vtkDriftsAndStopesReconstruction::vtkDriftsAndStopesReconstruction()
{
	this->numberOfCells = 0;
	count = 0;
  this->ToleranceIsAbsolute  = 1;
  this->Tolerance            = 0.0;
  this->AbsoluteTolerance    = 0.01;

}

//----------------------------------------------------------------------------
vtkDriftsAndStopesReconstruction::~vtkDriftsAndStopesReconstruction()
{

}



//----------------------------------------------------------------------------
int vtkDriftsAndStopesReconstruction::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  realInput = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  input = vtkPolyData::New();


	count++;


//---------------------------------------- Clean Filter ----------------------------------------------//
																																																	//
	vtkCleanPolyData *connect = vtkCleanPolyData::New();
	connect->SetInput(realInput);
	connect->SetPieceInvariant(1);
	connect->SetTolerance(this->Tolerance);
	connect->SetAbsoluteTolerance(this->AbsoluteTolerance);
	connect->SetToleranceIsAbsolute(this->ToleranceIsAbsolute);
	connect->SetConvertLinesToPoints(0);
	connect->SetConvertPolysToLines(0);
	connect->SetConvertLinesToPoints(0);
	connect->SetPointMerging(1);
	connect->Update();
	 
	input->ShallowCopy(connect->GetOutput());
	connect->Delete();
																																																	//	
//----------------------------------------------------------------------------------------------------*/



	buildDriftsWithTopology();

	return 1;
}



//----------------------------------------------------------------------------
void vtkDriftsAndStopesReconstruction::PrintSelf(ostream& os, vtkIndent indent)
{

}

//----------------------------------------------------------------------------
int vtkDriftsAndStopesReconstruction::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}




//----------------------------------------------------------------------------
void vtkDriftsAndStopesReconstruction::buildDriftsWithTopology()
{
	vtkIntArray* BlkNum;
	BlkNum= vtkIntArray::New();
	BlkNum->SetName("BlkNumber");
	

	inPoints = input->GetPoints();

	inCells =  input->GetLines();
	outCells = vtkCellArray::New();

	vtkIdType *pts;
	vtkIdType *npts = new vtkIdType();


	int countFaces = 0;
	int id1,id2;
	map< int, list<int> > vertices;
	set<pair<int,int>> edges;
	list<int> neighbors1;
	list<int> neighbors2;

	inCells->InitTraversal();

	for(int currentCell = 0; currentCell < inCells->GetNumberOfCells();
		currentCell++)
	{
		inCells->GetNextCell(*npts, *&pts);

		for(int currentPoint = 0; currentPoint < *npts-1; currentPoint++)
		{
			id1 = pts[currentPoint];
			id2 = pts[currentPoint+1];
			if(id1 < id2)
			{
				edges.insert(pair<int,int>(id1,id2));
			}
			else
			{
				edges.insert(pair<int,int>(id2,id1));
			}
			vertices[id1].push_back(id2);
			vertices[id2].push_back(id1);
		}

	}

	double point1[3];
	double point2[3];
	double vect1[3];
	double vect2[3];
	double norm1, norm2;
	map<double,int> id;
	double cosAngle;
	pair<int,int> edgeTemp;
	int ids[4];
	bool error = false;

	int countCells = 0;

	for(set<pair<int,int>>::iterator edge = edges.begin();
															edge != edges.end(); edge++)
	{
		ids[0] = edge->first;
		ids[1] = edge->second;
		neighbors1 = vertices[ids[0]];
		neighbors2 = vertices[ids[1]];

		inPoints->GetPoint(ids[0],point1);
		inPoints->GetPoint(ids[1],point2);
		vect1[0] = point2[0] - point1[0];
		vect1[1] = point2[1] - point1[1];
		vect1[2] = point2[2] - point1[2];
		norm1 = vtkMath::Norm(vect1);

		//id3 = neighbors1.front();
		for(list<int>::iterator vert1 = neighbors1.begin();
			vert1 != neighbors1.end(); vert1++)
		{
			if(*vert1 == ids[1])
				continue;

			ids[3] = *vert1;

			inPoints->GetPoint(*vert1,point1);
			for(list<int>::iterator vert = neighbors2.begin(); 
							vert!= neighbors2.end(); vert++)
			{
				if(*vert == ids[0])
					continue;

				if(*vert1 < *vert)
				{
					if(edges.count(pair<int,int>(*vert1,*vert))==0)
						continue;
				}
				else
					if(edges.count(pair<int,int>(*vert,*vert1))==0)
						continue;

				inPoints->GetPoint(*vert,point2);
				vect2[0] = point2[0] - point1[0];
				vect2[1] = point2[1] - point1[1];
				vect2[2] = point2[2] - point1[2];
				norm2 = vtkMath::Norm(vect2);
				cosAngle = (vtkMath::Dot(vect1,vect2))/(norm1*norm2);
				if(cosAngle>0.85)
					id[cosAngle] = *vert;
			}

			if(id.size()>0)
			{
				ids[2] = (id.rbegin())->second;
				/*
				for(int i = 0; i<4; i++)
				{
					id1 = ids[i];

					if(i == 3)
						id2 = ids[0];
					else 
						id2 = ids[i+1];

					
					if(id1<id2)
					{
						if(edges.count(pair<int,int>(id1,id2)) == 0)
						{
							error = true;
							break;
						}
					}
					else
					{
						if(edges.count(pair<int,int>(id2,id1)) == 0)
						{
							error = true;
							break;
						}
					}
				}
				if(error)
				{
					error = false;
					id.clear();
					continue;
				}
				*/

				outCells->InsertNextCell(4);
				outCells->InsertCellPoint(ids[0]);
				outCells->InsertCellPoint(ids[1]);
				outCells->InsertCellPoint(ids[2]);
				outCells->InsertCellPoint(ids[3]);
				BlkNum->InsertValue(countCells,count);
				countCells++;
				

				id.clear();
			}
		}
	}

	if(outCells->GetNumberOfCells() == 0)
		return ;
	
	vtkPolyData *outputTemp = vtkPolyData::New();
	outputTemp->SetPoints(inPoints);
	outputTemp->SetPolys(outCells);

	outputTemp->GetCellData()->AddArray(BlkNum); 

	vtkCleanCells *connect1 = vtkCleanCells::New();
	connect1->SetInput(outputTemp);
	connect1->Update();
	output->ShallowCopy(connect1->GetOutput());
	connect1->Delete();

	outCells->Delete();
	inCells->Delete();
	inPoints->Delete();
}


//----------------------------------------------------------------------------
void vtkDriftsAndStopesReconstruction::buildDriftsWithGeometry()
{

	vtkIntArray* BlkNum;
	BlkNum= vtkIntArray::New();
	BlkNum->SetName("BlkNumber");
	
	inPoints = realInput->GetPoints();
	inCells =  realInput->GetLines();

	outCells = vtkCellArray::New();

	vtkIdType *pts;
	vtkIdType *npts = new vtkIdType();

	for(int currentCell = 0; currentCell<inCells->GetNumberOfCells();
		currentCell++)
	{
		inCells->GetNextCell(*npts, *&pts);

	}
}