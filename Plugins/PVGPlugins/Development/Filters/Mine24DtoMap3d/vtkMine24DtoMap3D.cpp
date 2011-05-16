//===================================================================================//
//				Mirarco Mining Innovation
//
// Filter:   Mine24DtoMap3D
// Class:    vtkMine24DtoMap3D
// Author:   Nehmeh Bilal
// Director: Marie-Grabrielle Vallet 
// Date:     July 2008
// contact: nehmebilal@gmail.com 
//====================================================================================//

#include <map>
#include <algorithm>
#include <vector>
#include <deque>
#include <list>
#include<iostream>
#include<string>
#include<string.h>
#include<sstream>
#include<fstream>
#include "vtkDataArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "Segments.h"
#include <cfloat>
#include <set>

using namespace std;


struct triplet	// this triplet is used for x,y and z coordinates of a point
{
	double first;
	double second;
	double third;
};


void split(const string& str,vector<string>& tokens,const string& delimiters);
double roundZ(double Z);
void bestBoundingBox(deque<pair<double,double>> inPoints,   
			deque<pair<double,double>> &outPoints, double z, double stepAngle);
bool generateActivities(map<string,int> &activityNumbers, char* file, 
												int ActivityStep, int dateFormat);






///////////////////////////////////////////////////////////////////////////////////////////
void build_hull(deque<pair<double,double>> raw_points, 
								deque<pair<double,double>> &output);
void build_half_hull(std::deque< std::pair<double,double> > input,
                      std::deque< std::pair<double,double> > &output,
											std::pair<double,double> left, std::pair<double,double> right,
                      int factor );
static int direction( std::pair<double,double> p0,
                      std::pair<double,double> p1,
                      std::pair<double,double> p2 );
void simplifyHull(std::deque< std::pair<double,double> > &input,int numberOfEdges);
bool findIntersectionPoint( pair<double,double> p0, 
													  pair<double,double> p1,
														pair<double,double> p2,
														pair<double,double> p3,
														double &x, double &y, double &surface);
///////////////////////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////////////////////
// Computes the squared distance between segment [a,b] and point p
double squaredDistance(std::pair<double,double> const& a,
                       std::pair<double,double> const& b,
                       std::pair<double,double> const& p);

// Finds the index of the nearest polygon's edge to the point p
std::size_t findNearesEdge(std::deque< std::pair<double,double> > const& polygon,
                           std::pair<double,double> const& p);

// Mean least squares fitter of a line segment
struct LineFitter;

// Computes the intersection point between line 'a' and line 'b'
bool intersection(LineFitter const& a, LineFitter const& b, std::pair<double,double>& p);

// Improves polygon approximation
bool adjustPolygonApproximation(std::deque< std::pair<double,double> > const& points,
                                std::deque< std::pair<double,double> >& polygon);

void findBestQuadrangle(std::deque< std::pair<double,double> > const& points,
                        std::deque< std::pair<double,double> >& result);
///////////////////////////////////////////////////////////////////////////////////////////


void addEdge(int id1, int id2, map<pair<int,int>,int> &edgeNumber);





const double pi = 3.1415926535897931;



vtkCxxRevisionMacro(vtkMine24DtoMap3D, "$Revision: 1.34 $");
vtkStandardNewMacro(vtkMine24DtoMap3D);

//----------------------------------------------------------------------------
vtkMine24DtoMap3D::vtkMine24DtoMap3D ()
{
	this->blue = 1;			// 1, 6, 11, ... = blue
	this->green = 2;		// 2, 7, 12, ... = green
	this->yellow = 3;		// 3, 8, 13, ... = yellow
	this->red = 4;			// 4, 9, 14, ... = red
	this->grey = 5;			// 0, 5, 10, ... = grey

	this->ActivityFile = 0;		// requierd for vtkSetStringMacro/vtkGetStringMacro

	this->activityFound = false;
	this->ActivityStep = 0;
	this->dateFormat = 0;

	this->SegmentID = NULL;
	this->SegmentColor = NULL;
	orientedRectangle = 1;

	// CleanPolyData
	this->PieceInvariant = 1;
	this->Tolerance = 0;
	this->AbsoluteTolerance = 0;
	this->ToleranceIsAbsolute = 0;
	this->ConvertLinesToPoints = 0;
	this->ConvertPolysToLines = 0;
	this->ConvertStripsToPolys = 1;
	this->PointMerging = 1;

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
  realInput = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  input = vtkPolyData::New();


//---------------------------------------- Clean Filter ----------------------------------------------//
																																																		//
	 vtkCleanPolyData *connect = vtkCleanPolyData::New();
	 connect->SetInput(realInput);
	 connect->SetPieceInvariant(this->PieceInvariant);
	 connect->SetTolerance(this->Tolerance);
	 connect->SetAbsoluteTolerance(this->AbsoluteTolerance);
	 connect->SetToleranceIsAbsolute(this->ToleranceIsAbsolute);
	 connect->SetConvertLinesToPoints(this->ConvertLinesToPoints);
	 connect->SetConvertPolysToLines(this->ConvertPolysToLines);
	 connect->SetConvertStripsToPolys(this->ConvertStripsToPolys);
	 connect->SetPointMerging(this->PointMerging);
	 connect->Update();
	 
	 input->ShallowCopy(connect->GetOutput());
	 connect->Delete();
																																																		//	
//----------------------------------------------------------------------------------------------------//

	inCells = input->GetPolys();
	inPoints = input->GetPoints();
	outCells = vtkCellArray::New();
	outPoints = vtkPoints::New();
	numberOfCells = inCells->GetNumberOfCells();

	
	M4DNUM = input->GetCellData()->GetArray(this->SegmentID);
	COLOUR = input->GetCellData()->GetArray(this->SegmentColor);

	if(M4DNUM == NULL || COLOUR == NULL)
	{
		vtkErrorMacro("The selected data array are invalid");
		return 1;
	}

	

	BlkNum = vtkIntArray::New();
	BlkNum->SetName("BlkNumber");
	MiningBlock = vtkIntArray::New();
	MiningBlock->SetName("Mining Block");		// map3d activity block


	if(this->Approximation)
		approximateStopes();	// see definition
	else if(this->Simplification)
		simplifyStopes();			// see definition
	else if(this->SimplifyMesh)
		simplifyMesh();
	else if(this->Conversion)
		convertStopes();			// see definition
	else
	{
		vtkErrorMacro("  you have to select a conversion option  ");
		return 1;
	}




	output->SetPoints(outPoints);
	output->SetPolys(outCells);

	if(BlkNum != NULL)
		output->GetCellData()->AddArray(BlkNum); 

	if(this->activityFound)	// if activities are loaded correcly
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



void vtkMine24DtoMap3D::simplifyMesh()
{

	int num = 0;
	int id1,id2,id3,id4;
	double point1[3];
	double point2[3];
	double point3[3];
	double point4[3];
	double vect1[3];
	double vect2[3];
	double vect3[3];
	double normal1[3];
	double normal2[3];
	double normalsProduct;
	int stopeId = 0;

  //map <   segment id, map < edge<id1,id2> , deque<cell id> >  > 
	map <    int, map <  pair<int,int> , deque<int>  >    > segments ;

	map< int, vector<int> > neighbors;
	map< pair<int,int>,int > edgeNumber;

	(inCells)->InitTraversal();

	vtkIdType *npts = new vtkIdType();	// will store the number of points in current cell 
	vtkIdType *pts;	// will store a array of points id's in current cell 

	for(int currentCell = 0; currentCell < inCells->GetNumberOfCells();
		currentCell++)
	{
		inCells->GetNextCell(*npts, *&pts);
		num = M4DNUM->GetComponent(currentCell,0);
		if(*npts != 3)
		{
			vtkErrorMacro(" Input must be a triangulation ");
			return;
		}

		if(pts[0] < pts[1])
			segments[num][pair<int,int>(pts[0],pts[1])].push_back(pts[2]);
		else
			segments[num][pair<int,int>(pts[1],pts[0])].push_back(pts[2]);

		if(pts[1] < pts[2])
			segments[num][pair<int,int>(pts[1],pts[2])].push_back(pts[0]);
		else
			segments[num][pair<int,int>(pts[2],pts[1])].push_back(pts[0]);		

		if(pts[0] < pts[2])
			segments[num][pair<int,int>(pts[0],pts[2])].push_back(pts[1]);
		else
			segments[num][pair<int,int>(pts[2],pts[0])].push_back(pts[1]);

	}

	for (map <    int, map <  pair<int,int> , deque<int>  >    >::iterator
		segment = segments.begin(); segment != segments.end(); segment++)
	{
		stopeId = this->getColor(segment->first);

		for(map <  pair<int,int> , deque<int>  >::iterator 
			edge = segment->second.begin(); edge != segment->second.end();
			edge++)
		{
			if(edge->second.size() != 2)
				continue;		// each edge must be included in 2 triangles

			id1 = edge->first.first;	// first point of current edge
			id2 = edge->first.second;	// second point of current edge
			id3 = (edge->second)[0];
			id4 = (edge->second)[1];

			inPoints->GetPoint(id1,point1);
			inPoints->GetPoint(id2,point2);
			inPoints->GetPoint(id3,point3);
			inPoints->GetPoint(id4,point4);

			vect1[0] = point2[0] - point1[0];
			vect1[1] = point2[1] - point1[1];
			vect1[2] = point2[2] - point1[2];

			vect2[0] = point3[0] - point1[0];
			vect2[1] = point3[1] - point1[1];
			vect2[2] = point3[2] - point1[2];

			vtkMath::Cross(vect1,vect2,normal1);
			vtkMath::Normalize(normal1); 

			vect3[0] = point4[0] - point1[0];
			vect3[1] = point4[1] - point1[1];
			vect3[2] = point4[2] - point1[2];

			vtkMath::Cross(vect3,vect1,normal2);
			vtkMath::Normalize(normal2);

			normalsProduct = vtkMath::Dot(normal1,normal2);

			if(  fabs(normalsProduct)> cos( this->AngleTolerance*(pi/180) )  )
			{
				num = outCells->InsertNextCell(4);
				outCells->InsertCellPoint(id1);
				outCells->InsertCellPoint(id4);
				outCells->InsertCellPoint(id2);
				outCells->InsertCellPoint(id3);
				BlkNum->InsertValue(num,stopeId);

				if(id1 < id4)
				{
					segment->second.erase(pair<int,int>(id1,id4));
					edgeNumber[pair<int,int>(id1,id4)] += 1;
				}
				else
				{
					segment->second.erase(pair<int,int>(id4,id1));
					edgeNumber[pair<int,int>(id4,id1)] += 1;
				}
				neighbors[id1].push_back(id4);
				neighbors[id4].push_back(id1);


				if(id1 < id3)
				{
					segment->second.erase(pair<int,int>(id1,id3));
					edgeNumber[pair<int,int>(id1,id3)] += 1;
				}
				else
				{
					segment->second.erase(pair<int,int>(id3,id1));
					edgeNumber[pair<int,int>(id3,id1)] += 1;
				}
				neighbors[id1].push_back(id3);
				neighbors[id3].push_back(id1);

				if(id2 < id4)
				{
					segment->second.erase(pair<int,int>(id2,id4));
					edgeNumber[pair<int,int>(id2,id4)] += 1;
				}
				else
				{
					segment->second.erase(pair<int,int>(id4,id2));
					edgeNumber[pair<int,int>(id4,id2)] += 1;
				}
				neighbors[id2].push_back(id4);
				neighbors[id4].push_back(id2);

				if(id2 < id3)
				{
					segment->second.erase(pair<int,int>(id2,id3));
					edgeNumber[pair<int,int>(id2,id3)] += 1;
				}
				else
				{
					segment->second.erase(pair<int,int>(id3,id2));
					edgeNumber[pair<int,int>(id3,id2)] += 1;
				}
				neighbors[id2].push_back(id2);
				neighbors[id3].push_back(id3);
			}
		}

		for(map< pair<int,int>,int >::iterator edge = edgeNumber.begin();
			edge!= edgeNumber.end(); edge++)
		{
			if(edge->second == 1)
			{
				for(vector<int>::iterator neighbor = neighbors[edge->first.first].begin();
					neighbor != neighbors[edge->first.first].end(); neighbor++)
				{


					for(vector<int>::iterator neighbor2 = neighbors[edge->first.second].begin();
						neighbor2 != neighbors[edge->first.second].end(); neighbor2++)
					{
						if(*neighbor2 == *neighbor)
						{
							num = outCells->InsertNextCell(3);
							outCells->InsertCellPoint(edge->first.first);
							outCells->InsertCellPoint(edge->first.second);
							outCells->InsertCellPoint(*neighbor);
							BlkNum->InsertValue(num,stopeId);
						}
					}
				}
			}
		}

		edgeNumber.clear();
		neighbors.clear();

	}
	outPoints->ShallowCopy(inPoints);
	}



/*

	map<int,int> singleVertices;
	

	for(map< pair<int,int>,int >::iterator edge = edgeNumber.begin();
		edge!= edgeNumber.end(); edge++)
	{
		if(edge->second == 1)
		{
			singleVertices[edge->first.first] += 1;
			singleVertices[edge->first.first] += 1;
		}
	}

	vector<int> vect;
	vector<int>* vecT;
	bool found = false;

	for(map< pair<int,int>,int >::iterator edge = edgeNumber.begin();
		edge != edgeNumber.end(); edge++)
	{
		if(edge->second == 1)
		{
			id1 = edge->first.first;
			id2 = edge->first.second;

			found = false;

			vecT = &(neighbors[id1]);
			vect.assign(vecT->begin(),vecT->end());

			for(vector<int>::iterator neighbor = vect.begin();
				neighbor != vect.end(); neighbor++)
			{
				if(*neighbor == id2 || *neighbor == id1)
					continue;

				id3 = *neighbor;
				map<int,int>::iterator finder = singleVertices.find(id3);
				if(finder != singleVertices.end())
				{
					if(finder->second <= 1)
						singleVertices.erase(finder);
					else
						finder->second += -1;

					outCells->InsertNextCell(3);
					outCells->InsertCellPoint(id1);
					outCells->InsertCellPoint(id2);
					outCells->InsertCellPoint(id3);

					//edgeNumber.erase(edge);
					addEdge(id1,id2,edgeNumber);
					addEdge(id1,*neighbor,edgeNumber);
					addEdge(id2,*neighbor,edgeNumber);
					neighbors[id1].push_back(id3);
					neighbors[id3].push_back(id1);
					neighbors[id2].push_back(id3);
					neighbors[id3].push_back(id2);

					finder = singleVertices.find(id1);
					if(finder != singleVertices.end())
					{
						if(finder->second <= 1)
							singleVertices.erase(finder);
					else
						finder->second += -1;
					}

					finder = singleVertices.find(id2);
					if(finder != singleVertices.end())
					{
						if(finder->second <= 1)
							singleVertices.erase(finder);
					else
						finder->second += -1;
					}
					found = true;
					break;
				}
			}

			if(!found)
			{
				vecT = &(neighbors[id2]);
				vect.assign(vecT->begin(),vecT->end());

				for(vector<int>::iterator neighbor = vect.begin();
					neighbor != vect.end(); neighbor++)
				{
					if(*neighbor == id2 || *neighbor == id1)
						continue;

					map<int,int>::iterator finder = singleVertices.find(*neighbor);
					if(finder != singleVertices.end())
					{
						if(finder->second <= 1)
							singleVertices.erase(finder);
						else
							finder->second += -1;

						outCells->InsertNextCell(3);
						outCells->InsertCellPoint(id1);
						outCells->InsertCellPoint(id2);
						outCells->InsertCellPoint(*neighbor);

						//edgeNumber.erase(edge);
						addEdge(id1,id2,edgeNumber);
						addEdge(id1,*neighbor,edgeNumber);
						addEdge(id2,*neighbor,edgeNumber);
						neighbors[id1].push_back(*neighbor);
						neighbors[*neighbor].push_back(id1);
						neighbors[id2].push_back(*neighbor);
						neighbors[*neighbor].push_back(id2);

						finder = singleVertices.find(id1);
						if(finder != singleVertices.end())
						{
							if(finder->second <= 1)
								singleVertices.erase(finder);
						else
							finder->second += -1;
						}

						finder = singleVertices.find(id2);
						if(finder != singleVertices.end())
						{
							if(finder->second <= 1)
								singleVertices.erase(finder);
						else
							finder->second += -1;
						}
						found = true;
						break;
					}
				}
			}
		}
	}



*/



//-----------------------------------------------------------------------------------------------------------
// Methode: approximateStopes
// Description: This methode is used for stopes having a complex geometry. This methode 
//							may not work correctly on very small stopes. To build the approximated stopes in paraview,
//							this methode approximate stopes faces with an oriented bounding rectangle.	
//							The speed of this methode depend on the parameter AngleStep. if AngleStep is small, the result
//							is better but the algorithme will be longer.
//-----------------------------------------------------------------------------------------------------------
void vtkMine24DtoMap3D::approximateStopes()
{
	map<pair<double,int>,deque<pair<double,double>>> segments;
	//segments<pair<z,segmentID>,deque<pair<x,y>>>: each element of this map
	// will contain all the points having the same elevation z and the same segment id.
	// that's mean each element of segments, contain all points of a single stope face or subface.
	// each stope may have more than 2 faces, because in this methode, we devied complex stopes, to 
	// a number of blocks depending on the complexity of the stope.
	//	================================================================
	//							 |
	//		z1, segID1 | point1(x1,y1) , point2(x2,y2), ..., pointn(xn,yn)
	//		z2, segID2 | point1(x1,y1) , point2(x2,y2), ..., pointn(xn,yn)		
	//				.			 |
	//				.			 |	
	//				.			 |
	//				.			 |	
	//		zn, segIDn | point1(x1,y1) , point2(x2,y2), ..., pointn(xn,yn)
	//							 |
	//	================================================================	
	//
	// because segments is a map, it will be automaticly sorted from lower to higher value of z


	map<int, deque<deque<triplet>>> SegmentsFaces;
	// map< segID, deque<deque<triplet(x,y,z)>> 
	//
	//	================================================================
	//							 |================================================
	//							 | (Face)1: (x1,y1,Z1) ,(x2,y2,Z1), ..., (xn,yn,Z1)
	//							 | (Face)2: (x1,y1,Z2) ,(x2,y2,Z2), ..., (xn,yn,Z2)
	//				segID1 |		.
	//							 |		.
	//							 |		.
	//							 | (Face)n: (x1,y1,Zn) ,(x2,y2,Zn), ..., (xn,yn,Zn)
  //								=================================================
	//							 |================================================
	//							 | (Face)1: (x1,y1,Z1) ,(x2,y2,Z1), ..., (xn,yn,Z1)
	//							 | (Face)2: (x1,y1,Z2) ,(x2,y2,Z2), ..., (xn,yn,Z2)
	//				segID2 |		.
	//							 |		.
	//							 |		.
	//							 | (Face)n: (x1,y1,Zn) ,(x2,y2,Zn), ..., (xn,yn,Zn)
  //								=================================================
	//					.
	//					.
	//					.
	//							 |================================================
	//							 | (Face)1: (x1,y1,Z1) ,(x2,y2,Z1), ..., (xn,yn,Z1)
	//							 | (Face)2: (x1,y1,Z2) ,(x2,y2,Z2), ..., (xn,yn,Z2)
	//				segIDn |		.
	//							 |		.
	//							 |		.
	//							 | (Face)n: (x1,y1,Zn) ,(x2,y2,Zn), ..., (xn,yn,Zn)
  //								=================================================
	//	================================================================	

	double *currentTuple;
	double point1[3];
	int num;

	map<int,int> color;		// map<segmentID, color>
	int countCells = 0;
	int lastCountCells = 0;
	int stopeId;
	int ActNum;

	(inCells)->InitTraversal();

	
	vtkIdType *npts = new vtkIdType();	// will store the number of points in current cell 
	vtkIdType *pts;	// will store a array of points id's in current cell 

	for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
	{
		inCells->GetNextCell(*npts, *&pts);

		num = M4DNUM->GetComponent(currentCell,0);

		currentTuple = inPoints->GetPoint(pts[0]);
		point1[0] = currentTuple[0];
		point1[1] = currentTuple[1];
		point1[2] = roundZ(currentTuple[2]);	// we round z because we want all the points 
																					// in the same to have a unique value of Z
																					// this will help to separate faces in the 
																					// map segments.

		segments[make_pair(point1[2], num)].push_back(make_pair(point1[0],point1[1]));
		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(pts[1]);
		point1[0] = currentTuple[0];
		point1[1] = currentTuple[1];
		point1[2] = roundZ(currentTuple[2]);
		segments[make_pair(point1[2], num)].push_back(make_pair(point1[0],point1[1]));
		currentTuple = NULL;

		currentTuple = inPoints->GetPoint(pts[2]);
		point1[0] = currentTuple[0];
		point1[1] = currentTuple[1];
		point1[2] = roundZ(currentTuple[2]);
		segments[make_pair(point1[2], num)].push_back(make_pair(point1[0],point1[1]));
		currentTuple = NULL;

		color[num] = COLOUR->GetComponent(currentCell,0);
			
	}
	
	deque<triplet> outBounds(4);		// 4 boundary points of a face
	deque<pair<double,double>> outB(4);
	deque<pair<double,double>> outHull(4);
	int vertsCounter=0;
	triplet pTemp;
	double zN = 0;
	double zO = 0;
	double vect1[2];
	double vect2[2];
	int k;
	double cosAngle;
	int index;
	double norm1;
	double norm2;
	double cosAngleTemp;
	int idN = 0;
	int idO = 0;
	int badId = -1;

/*
	map<pair<double,int>,deque<pair<double,double>>>::iterator it = segments.begin();
	if(!it->second.size()<4)
	{
		build_hull(it->second, outHull);
		for(int i=0; i<4; i++)
		{
			pTemp.first = outHull[i].first;
			pTemp.second = outHull[i].second;
			pTemp.third = it->first.first;
			outBounds.push_back(pTemp);
		}

	}
*/

	for(map<pair<double,int>,deque<pair<double,double>>>::iterator 
		it = segments.begin(); 
		it!= segments.end(); it++)
	{
		if(it->second.size()<4)	// if the face has less than 4 points, go to next face
			continue;

		zN = it->first.first;
		idN = it->first.second;

		// the folowing function will find the best bounding rectangle 
		// enclosing the face the output is writen into "outBounds" vector
		
		if(orientedRectangle)
		{

		bestBoundingBox(it->second, outHull, (it->first).first, 
			(this->AngleStep)*pi/180);
			//build_hull(it->second, outHull);
			//findBestQuadrangle(it->second, outHull);
		}
		//else
		//{
		//	findBestQuadrangle(it->second, outHull);
			//build_hull(it->second, outHull);

			if(outHull.size()<4)
			{
				continue;
			}

			for(int i=0; i<4; i++)
			{
				pTemp.first = outHull[i].first;
				pTemp.second = outHull[i].second;
				pTemp.third = zN;
				outBounds[i] = pTemp;
			}
		//}


		SegmentsFaces[idN].push_back(outBounds);
			
	}
				

/*
		for(deque<pair<double,double>>::iterator iter = outHull.begin();
			iter != outHull.end(); iter++)
		{
			outPoints->InsertNextPoint(iter->first,iter->second,it->first.first);
			outCells->InsertNextCell(1);
			outCells->InsertCellPoint(vertsCounter);
			vertsCounter++;
		}
	}
	output->SetPoints(outPoints);
	output->SetVerts(outCells);
	return;
*/



	map<string,int> activityNumbers;
	if(WithActivity)
		this->activityFound = generateActivities(activityNumbers, this->ActivityFile,
																						this->ActivityStep, this->dateFormat);


	int pid = 0;
	for(map<int, deque<deque<triplet>>>::iterator it = SegmentsFaces.begin();
		it!= SegmentsFaces.end(); it++)
	{
		if(it->second.size()<2)
			continue;

		deque<deque<triplet>>::iterator iter = (it->second).begin();
		outPoints->InsertNextPoint((*iter)[0].first,(*iter)[0].second,(*iter)[0].third);
		outPoints->InsertNextPoint((*iter)[1].first,(*iter)[1].second,(*iter)[1].third);
		outPoints->InsertNextPoint((*iter)[2].first,(*iter)[2].second,(*iter)[2].third);
		outPoints->InsertNextPoint((*iter)[3].first,(*iter)[3].second,(*iter)[3].third);

		// bottom face:
		outCells->InsertNextCell(4);
		outCells->InsertCellPoint(pid);
		outCells->InsertCellPoint(pid+1);
		outCells->InsertCellPoint(pid+2);
		outCells->InsertCellPoint(pid+3);

		stopeId = color[it->first];
		stopeId = this->getColor(stopeId);

		BlkNum->InsertValue(countCells,stopeId);
		countCells ++;

		pid += 4; 


		vect1[0] = (*iter)[1].first - (*iter)[0].first;
		vect1[1] = (*iter)[1].second - (*iter)[0].second;
		norm1 = vtkMath::Norm2D(vect1);

		for(iter =(it->second).begin()+1; iter!=(it->second).end(); iter++)
		{

			k = 0;
			cosAngle = 0;
			index = 0;
			zN = (*iter)[0].third;

			for(int i = 0; i < 4; i++)
			{
				if(k==3)
				{
					vect2[0] = (*iter)[0].first - (*iter)[3].first;
					vect2[1] = (*iter)[0].second - (*iter)[3].second;
				}
				else
				{
					vect2[0] = (*iter)[k+1].first - (*iter)[k].first;
					vect2[1] = (*iter)[k+1].second - (*iter)[k].second;
				}
				k += 1;

				norm2 = vtkMath::Norm2D(vect2);
				cosAngleTemp = (vtkMath::Dot(vect1,vect2))/(norm1*norm2);

				if(cosAngleTemp > cosAngle)
				{
					cosAngle = cosAngleTemp;
					index = k-1;
				}
			}

			for(int i = 0; i<4 ; i++)
			{
				pTemp.first = (*iter)[index].first;
				pTemp.second = (*iter)[index].second;
				//pTemp.third = zN;
				outBounds[i] = pTemp;
				index += 1;
				if(index == 4)
					index = 0;
			}
			vect1[0] = outBounds[1].first - outBounds[0].first;
			vect1[1] = outBounds[1].second - outBounds[0].second;

			outPoints->InsertNextPoint(outBounds[0].first,outBounds[0].second,zN);
			outPoints->InsertNextPoint(outBounds[1].first,outBounds[1].second,zN);
			outPoints->InsertNextPoint(outBounds[2].first,outBounds[2].second,zN);
			outPoints->InsertNextPoint(outBounds[3].first,outBounds[3].second,zN);

			// side faces:
			// for each block cells must be inserted in this way  
			// to meet the topology of an hexahedron
			// the number of side faces is variable depending on stopes geometry

			outCells->InsertNextCell(4);
			outCells->InsertCellPoint(pid-3);
			outCells->InsertCellPoint(pid-2);
			outCells->InsertCellPoint(pid+2);
			outCells->InsertCellPoint(pid+1);

			BlkNum->InsertValue(countCells,stopeId);
			countCells ++;

			outCells->InsertNextCell(4);
			outCells->InsertCellPoint(pid-4);
			outCells->InsertCellPoint(pid-1);
			outCells->InsertCellPoint(pid+3);
			outCells->InsertCellPoint(pid);

			BlkNum->InsertValue(countCells,stopeId);
			countCells ++;

			outCells->InsertNextCell(4);
			outCells->InsertCellPoint(pid-4);
			outCells->InsertCellPoint(pid-3);
			outCells->InsertCellPoint(pid+1);
			outCells->InsertCellPoint(pid);

			BlkNum->InsertValue(countCells,stopeId);
			countCells ++;

			outCells->InsertNextCell(4);
			outCells->InsertCellPoint(pid-1);
			outCells->InsertCellPoint(pid-2);
			outCells->InsertCellPoint(pid+2);
			outCells->InsertCellPoint(pid+3);

			BlkNum->InsertValue(countCells,stopeId);
			countCells ++;

			pid += 4;
		}
		// top face:
		outCells->InsertNextCell(4);
		outCells->InsertCellPoint(pid-4);
		outCells->InsertCellPoint(pid-3);
		outCells->InsertCellPoint(pid-2);
		outCells->InsertCellPoint(pid-1);

		BlkNum->InsertValue(countCells,stopeId);
		countCells ++;

		// if the user want to insert activity, and no errors
		// when reading activity file
		if(this->activityFound)
		{
			ostringstream stringActNum;							// converting int to string
			stringActNum << it->first;								//
			ActNum = activityNumbers[stringActNum.str()];	//

			for( int i = lastCountCells ; i < countCells; i++)
			{
				MiningBlock->InsertValue(i,ActNum);	// insert the activity propertie
																						// for each cell of the last segment
			}
		}
		lastCountCells = countCells;
		

	}




}
//----------------------------------------------------------------------------
// this method is used to simplify simple stopes. for each stopes, this method
// will find the 8 vertices of the top and bottom faces and link them togother.
//----------------------------------------------------------------------------
void vtkMine24DtoMap3D::simplifyStopes()
{
	map<int,list<int*>> segments;	// map<segment id, list<triangle<p1,p2,p3>>
	//	================================================================
	//							 |================================================
	//							 | (triangle)1: id(point1),id(point2),id(point3) 
	//							 | (triangle)2: id(point1),id(point2),id(point3)
	//				segID1 |		.
	//							 |		.
	//							 |		.
	//							 | (triangle)n: id(point1),id(point2),id(point3)
  //								=================================================
	//							 |================================================
	//							 | (triangle)1: id(point1),id(point2),id(point3) 
	//							 | (triangle)2: id(point1),id(point2),id(point3)
	//				segID2 |		.
	//							 |		.
	//							 |		.
	//							 | (triangle)n: id(point1),id(point2),id(point3)
  //								=================================================
	//					.
	//					.
	//					.
	//							 |================================================
	//							 | (triangle)1: id(point1),id(point2),id(point3) 
	//							 | (triangle)2: id(point1),id(point2),id(point3)
	//				segIDn |		.
	//							 |		.
	//							 |		.
	//							 | (triangle)n: id(point1),id(point2),id(point3)
  //								=================================================
	//	================================================================	



	map<int,pair<double,int>> zSegments;
	//map<segment id, pair<z(triangle),number of triangle perpendicular to z>
	// this map will contain only the triangles perpendicular to Z axis
	// the number of triangle pependicular to z will be used to find the 
	// median plan of the segment. this median plan will be used to find out
	// wich triangles are in the bottom face of the segment and wich triangle
	// are in the top face. 
	
	map<int,int> color;	// map<segment id, color>

	int stopeId = 0;
	int *triangle;
	double currentTuple[3];
	double vect1[3];
	double vect2[3];
	double point1[3];
	double point2[3];
	double point3[3];

	double normal[3];
	double zmed = 0;
	int num = 0;
	double angle = 0;
	double angle2 = 0;
	double angle3 = 0;
	double norm1;
	double norm2;
	int k = 0;
	double cosAngle = 0;
	int countCells = 0;
	int ActNum;

	(inCells)->InitTraversal();

	
	vtkIdType *npts = new vtkIdType();	// will store the number of points in current cell 
	vtkIdType *pts;	// will store a array of points id's in current cell 

	for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
	{
		inCells->GetNextCell(*npts, *&pts);

		inPoints->GetPoint(pts[0],point1);
		inPoints->GetPoint(pts[1],point2);
		inPoints->GetPoint(pts[2],point3);

		vect1[0]= point2[0]-point1[0];
		vect1[1]= point2[1]-point1[1];
		vect1[2]= point2[2]-point1[2];

		vect2[0]= point3[0]-point1[0];
		vect2[1]= point3[1]-point1[1];
		vect2[2]= point3[2]-point1[2];

		vtkMath::Cross(vect1, vect2, normal);
		vtkMath::Normalize(normal);

		
		// if the angle between the normal of the current triangle and
		// the Z axis is less than AngleTolerance, this triangle 
		// is consdired perpendicular to the z axis. the value
		// of AngleTolerance can be choosen by the user in the filter interface. 
		if(fabs(normal[2])> cos(this->AngleTolerance*(pi/180)))
		{
			triangle = new int[3];
			triangle[0] = pts[0];
			triangle[1] = pts[1];
			triangle[2] = pts[2];
			num = M4DNUM->GetComponent(currentCell,0);
			segments[num].push_back(triangle);
			zSegments[num].first += ((point1[2] + point2[2] + point3[2])/3);
			zSegments[num].second++; 
			color[num] = COLOUR->GetComponent(currentCell,0);
		}

	}

	Segments seg;

	// from the segment triangles, this methode will find wich triangles
	// are in the bottom faces and wich triangle are in the top face.
	seg.setFacesTriangles(segments, zSegments, inPoints);

	// this method will find the vertices of each faces.
	// 1 -> top face
	// 2 -> bottom face
	seg.setFacesVertices(inPoints, 1);
	seg.setFacesVertices(inPoints, 2);

	map<string,int> activityNumbers;	// map< segment id , block number>
	// the segment id is a string because we reading it from a file

	if(WithActivity)	// if the user want to add activity
		// generateActivities will read activities from the file specified by the user
		// if an error happened, generateActivities will return false and will print
		// an error to inform the user what he did wrong.
		this->activityFound = generateActivities(activityNumbers,this->ActivityFile,
																					this->ActivityStep, this->dateFormat );

	double cosAngleTemp = 0;
	int count = 0;
	map<int,Face> topFaces = seg.getFaces(1);
	map<int,Face> bottomFaces = seg.getFaces(2);

	int vertices[4];
	
	for(map<int,Face>::iterator iter = topFaces.begin(); iter != topFaces.end(); iter++)
	{
		iter->second.getVertices(vertices);
		try{
			inPoints->GetPoint(vertices[0], point1); 
			inPoints->GetPoint(vertices[1], point2);

			vect1[0] = point2[0] - point1[0];
			vect1[1] = point2[1] - point1[1];
			vect1[2] = point2[2] - point1[2];
			norm1 = vtkMath::Norm(vect1);

			k = 0;
			cosAngle = 0;
			int index = 0;
			int face2[4];

			bottomFaces[iter->first].getVertices(face2);


			// this for loop is used to sort the bottom face vertices
			// in the same order of the top face vertices.
			// the goal is to have an hexaedron with a correct topology:
			// vertices connection wanted:  1-5; 2-6; 3-7; 4-8
			// so if top faces is sorted like : [1, 3, 4, 2]
			// the result will be [5, 7, 8, 6] for the bottom face
			// [1, 3, 4, 2]
			//  |  |  |  |
			// [5, 7, 8, 6]
			// to do this we take a vector from the top face,
			// for example 13 and we do a scalar vector with
			// all vector in the bottom face . the vector 57 will
			// give the max value of scalar vector with 13
			// max ( 13 dot xx) = 13 dot 57
			for(int i=0; i<4; i++)
			{
				inPoints->GetPoint(face2[k],point1);
				if(k==3)
				{
					inPoints->GetPoint(face2[0],point2);
				}
				else
				{
					inPoints->GetPoint(face2[k+1],point2);
				}
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
			}

			int idTemp[4];
			for(int i = 0; i<4; i++)
			{
				idTemp[i] = face2[index];
				index += 1;
				if(index == 4)
					index = 0;
			}

			inPoints->GetPoint(vertices[0], currentTuple);
			outPoints->InsertNextPoint(currentTuple);

			inPoints->GetPoint(vertices[1], currentTuple);
			outPoints->InsertNextPoint(currentTuple);

			inPoints->GetPoint(vertices[2], currentTuple);
			outPoints->InsertNextPoint(currentTuple);

			inPoints->GetPoint(vertices[3], currentTuple);
			outPoints->InsertNextPoint(currentTuple);

			inPoints->GetPoint(idTemp[0], currentTuple);
			outPoints->InsertNextPoint(currentTuple);

			inPoints->GetPoint(idTemp[1], currentTuple);
			outPoints->InsertNextPoint(currentTuple);

			inPoints->GetPoint(idTemp[2], currentTuple);
			outPoints->InsertNextPoint(currentTuple);

			inPoints->GetPoint(idTemp[3], currentTuple);
			outPoints->InsertNextPoint(currentTuple);

			stopeId = color[iter->first];
			stopeId = this->getColor(stopeId);

			outCells->InsertNextCell(4);
			outCells->InsertCellPoint(count);
			outCells->InsertCellPoint(count + 1);
			outCells->InsertCellPoint(count + 2);
			outCells->InsertCellPoint(count + 3);

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
			countCells ++;

			outCells->InsertNextCell(4);
			outCells->InsertCellPoint(count + 4);
			outCells->InsertCellPoint(count + 5);
			outCells->InsertCellPoint(count + 6);
			outCells->InsertCellPoint(count + 7);

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


}
//-------------------------------------------------------------------------------------------------------
// this method will transfer points and cells from the input to the output  
// and give them the BlockNumber, color and MiningBlock property
// so the map3d writer can convert it to map3d format.
//-------------------------------------------------------------------------------------------------------
void vtkMine24DtoMap3D::convertStopes()
{

	map<int,list<pair<int,int*>>> segments;
	int num;
	map<int,int> color;
	int colour;
	int countCells = 0;
	int ActNum;

	(inCells)->InitTraversal();
	vtkIdType *npts = new vtkIdType();	// will store the number of points in current cell 
	vtkIdType *pts;	// will store a array of points id's in current cell 


	
	for (int currentCell = 0; currentCell<numberOfCells ; currentCell++)
	{
		inCells->GetNextCell(*npts, *&pts);
		num = M4DNUM->GetComponent(currentCell,0);
		color[num] = COLOUR->GetComponent(currentCell,0);
		segments[num].push_back(pair<int,int*>(*npts,pts)); 
	}

	map<string,int> activityNumbers;

	if(WithActivity)
		this->activityFound = generateActivities(activityNumbers,this->ActivityFile,
																					this->ActivityStep, this->dateFormat );


	outPoints->ShallowCopy(inPoints);

	for(map<int,list<pair<int,int*>>>::iterator cells = segments.begin(); cells != segments.end(); cells++)
	{
		colour = color[cells->first];
		colour = this->getColor(colour);

		for(list<pair<int,int*>>::iterator cell = cells->second.begin(); cell != cells->second.end(); cell++)
		{
			outCells->InsertNextCell(cell->first);
			BlkNum->InsertValue(countCells,colour);
			if(this->activityFound)
			{
				ostringstream stringActNum;
				stringActNum << cells->first;
				ActNum = activityNumbers[stringActNum.str()];
				MiningBlock->InsertValue(countCells,ActNum);
			}


			for(int i = 0; i < cell->first ; i++)
				outCells->InsertCellPoint(cell->second[i]);

			countCells++;
		}

	}


}
//-------------------------------------------------------------------------------------------------------
// this function is used to read activities file and generate Mining Block 
// property for the map3d segments 
//-------------------------------------------------------------------------------------------------------
bool generateActivities(map<string,int> &activityNumbers, char* file, int ActivityStep, int dateFormat)
	{
		vtkPoints *p = vtkPoints::New();
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

		myFile.open(file);
		if(myFile.fail())
		{
			vtkErrorWithObjectMacro(p,"Activity file not found !"<<endl<<"   your path must look like: C:"<<"\\abc.txt ");
			return false;
		}

		while(!myFile.eof())
		{
			getline(myFile, line);
			if(line.substr(0,14) == "1,Dependencies")	// this indicates that we past on all activities
				break;

			if(tasks == 2)
			{
				split(line,lineSplit,",");
				if(lineSplit.size()<m4dnumIndex)	//m4dnumIndex is the index of M4DNUM header
				{
					vtkErrorWithObjectMacro(p,"Activity Error: check activity file"<<endl);
					return false;
				}

				id = lineSplit[1];
				if(id[0] == 'S')
				{
					date1 = lineSplit[dateIndex];
					if(dateFormat == 1)		// dateFormat = YYYY-MM-DD hh:mm:ss
					{
						split(date1,date2,"-");
						if(date2.size()<3)
						{
							vtkErrorWithObjectMacro(p,"Activity Error: check date format"<<endl);
							return false;
						}
						date2[2] = date2[2].substr(0,2);	// to delete hh:mm:ss
						date = (atoi(date2[0].c_str()))*365 + (atoi(date2[1].c_str()))*30 
							+ (atoi(date2[2].c_str())); // converting date to x days:
																					// 0001-02-03 = 1*365 + 2*30 + 3
					}
					else if(dateFormat == 2) // MM/DD/YY
					{
						split(date1,date2,"/");
						if(date2.size()<3)
						{
							vtkErrorWithObjectMacro(p,"Activity Error: check date format"<<endl);
							return false;
						}
						date2[2] = date2[2].substr(0,4);
						date = (atoi(date2[2].c_str()))*365 + (atoi(date2[1].c_str()))*30 + (atoi(date2[0].c_str()));
					}
					else
					{
						vtkErrorWithObjectMacro(p,"Activity Error: Select a date format"<<endl);
						return false;
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

		act = 1;
		multimap<int,string>::iterator itAct = activityDate.begin();

		previousDate = itAct->first;

		for( itAct = activityDate.begin(); itAct != activityDate.end(); itAct++)
		{
			if(((itAct->first) - previousDate) > ActivityStep)
			{
				previousDate = itAct->first;
				activityNumbers[itAct->second] = act++;
				
			}
			else
				activityNumbers[itAct->second] = act;
		}
		
		return true;
	}

//---------------------------------------------------------------------------------------------
// 1, 6, 11, ... = blue
// 2, 7, 12, ... = green
// 3, 8, 13, ... = yellow
// 4, 9, 14, ... = red
// 0, 5, 10, ... = grey
//---------------------------------------------------------------------------------------------
int vtkMine24DtoMap3D::getColor(int color)
{
	switch(color % 5)
	{
		case 1:
			this->blue += 5;
			return (this->blue -5);

		case 2:
			this->green += 5;
			return (this->green -5);

		case 3:
			this->yellow += 5;
			return (this->yellow - 5);

		case 4:
			this->red += 5;
			return (this->red - 5);

		case 0:
			this->grey += 5;
			return (this->grey - 5);
	}
}
//---------------------------------------------------------------------------------------------




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
double roundZ(double Z) 
{
	int f=1;
	if(Z<0)
		f=-1;
	else
		f=1;

	int z = abs(floor(Z + 0.5));
	int a = z%10;

	if(a<=1)
		return f*(z - a);
	else if(a <= 3)
		return f*(z - a + 2.5);

	else if(a <= 6)
		return f*(z - a + 5);
	else if(a <= 8)
		return f*(z - a + 7.5);
	else
		return f*(z - a + 10);

}
//---------------------------------------------------------------------------------------------






//-----------------------------------------------------------------------------------------------------------
void bestBoundingBox(deque<pair<double,double>> inPoints, deque<pair<double,double>> &outPoints,
										 double z, double stepAngle)
{
	deque<double> v(4);
	double angle;
	double x,y;
	double xmin, xmax, ymin, ymax;
	map<pair<int,double>,deque<double>> bound;

	xmin = inPoints[0].first;
	xmax = xmin;
	ymin = inPoints[0].second;
	ymax = ymin;


	//search for xmin.xmax,ymin,ymax
	for(deque<pair<double,double>>::iterator it = inPoints.begin()+1; it!= inPoints.end(); it++)
	{
		if(it->first < xmin)
			xmin = it->first;
		else if(it->first > xmax)
			xmax = it->first;
		if(it->second < ymin)
			ymin = it->second;
		else if(it->second > ymax)
			ymax = it->second;
	}


	v[0] = xmin;
	v[1] = xmax;
	v[2] = ymin;
	v[3] = ymax;
	bound[make_pair((xmax - xmin)*(ymax - ymin),0)] = v;


	angle = stepAngle;
	// rotate all points step by step, and find the bounding smaller bounding rectangle
	while(angle < pi/2)
	{
		xmin = ( (inPoints[0].first)*cos(angle) ) - ( (inPoints[0].second)*sin(angle) );
		ymin = ( (inPoints[0].first)*sin(angle) ) + ( (inPoints[0].second)*cos(angle) );
		xmax = xmin;
		ymax = ymin;
		for(deque<pair<double,double>>::iterator it = inPoints.begin()+1 ; it!= inPoints.end(); it++)
		{
			x = ( (it->first)*cos(angle) ) - ( (it->second)*sin(angle) );
			y = ( (it->first)*sin(angle) ) + ( (it->second)*cos(angle) );
			if(x < xmin)
				xmin = x;
			else if(x > xmax)
				xmax = x;
			if(y < ymin)
				ymin = y;
			else if(y > ymax)
				ymax = y;
		}
		v[0] = xmin;
		v[1] = xmax;
		v[2] = ymin;
		v[3] = ymax;
		bound[make_pair((xmax - xmin)*(ymax - ymin),angle)] = v; // in this map, the elements will be sorted
		// from the smaller bounding rectangle to the bigger one.

		angle += stepAngle; 
	}

	
	map<pair<int,double>,deque<double>>::iterator it = bound.begin();  // the smaller rectangle is the
																  // first element of the map
	angle = (it->first).second;
	xmin = (it->second[0]);
	xmax = (it->second[1]);
	ymin = (it->second[2]);
	ymax = (it->second[3]);

	// rotate the points back to their original position
	outPoints[0].first = xmin*cos(angle) + ymin*sin(angle) ;															
	outPoints[0].second = -xmin*sin(angle) + ymin*cos(angle);
	//outPoints[0].third = z;

	outPoints[1].first = xmax*cos(angle) + ymin*sin(angle) ; 
	outPoints[1].second = -xmax*sin(angle) + ymin*cos(angle);
	//outPoints[1].third = z;

	outPoints[2].first = xmax*cos(angle) + ymax*sin(angle) ; 
	outPoints[2].second = -xmax*sin(angle) + ymax*cos(angle);
	//outPoints[2].third = z;

	outPoints[3].first =  xmin*cos(angle) + ymax*sin(angle) ;;
	outPoints[3].second = -xmin*sin(angle) + ymax*cos(angle);
	//outPoints[3].third = z;

}
//-------------------------------------------------------------------------------------------------------











void build_hull(deque<pair<double,double>> raw_points,
								deque<pair<double,double>> &output)
{
	if(raw_points.size()<=4)
	{
		output = raw_points;
		return;
	}
	output.clear();
//
// The initial array of points is stored in deque raw_points. I first
// sort it, which gives me the far left and far right points of the hull.
// These are special values, and they are stored off separately in the left
// and right members.
//
// I then go through the list of raw_points, and one by one determine whether
// each point is above or below the line formed by the right and left points.
// If it is above, the point is moved into the upper_partition_points sequence. If it
// is below, the point is moved into the lower_partition_points sequence. So the output
// of this routine is the left and right points, and the sorted points that are in the
// upper and lower partitions.
//

  std::pair<double,double> left;
  std::pair<double,double> right;
  std::deque< std::pair<double,double> > upper_partition_points;
  std::deque< std::pair<double,double> > lower_partition_points;


	//
	// Step one in partitioning the points is to sort the raw data
	//
	std::sort( raw_points.begin(), raw_points.end() );
	//
	// The the far left and far right points, remove them from the
	// sorted sequence and store them in special members
	//
	left = raw_points.front();
	raw_points.pop_front();
	//raw_points.erase(raw_points.begin());
	right = raw_points.back();
	raw_points.pop_back();
	//
	// Now put the remaining points in one of the two output sequences
	//
	for ( size_t i = 0 ; i < raw_points.size() ; i++ )
	{
			int dir = direction( left, right, raw_points[ i ] );
			if ( dir < 0 )
					upper_partition_points.push_back( raw_points[ i ] );
			else
					lower_partition_points.push_back( raw_points[ i ] );
	}

  //std::deque< std::pair<double,double> > lower_hull;
  std::deque< std::pair<double,double> > upper_hull;

  build_half_hull(lower_partition_points, output, left, right, 1 );
  build_half_hull(upper_partition_points, upper_hull, left, right, -1 );

	for(deque< pair<double,double> >::reverse_iterator it = upper_hull.rbegin()+1;
		it != upper_hull.rend()-1; it++)
	{
		output.push_back(*it);
	}

	if(output.size()<=4)
		return;

	simplifyHull(output,4);
}
//
// Building the hull consists of two procedures: building the lower and
// then the upper hull. The two procedures are nearly identical - the main
// difference between the two is the test for convexity. When building the upper
// hull, our rull is that the middle point must always be *above* the line formed
// by its two closest neighbors. When building the lower hull, the rule is that point
// must be *below* its two closest neighbors. We pass this information to the 
// building routine as the last parameter, which is either -1 or 1.
//






// This is the method that builds either the upper or the lower half convex
// hull. It takes as its input a copy of the input array, which will be the
// sorted list of points in one of the two halfs. It produces as output a list
// of the points in the corresponding convex hull.
//
// The factor should be 1 for the lower hull, and -1 for the upper hull.

void build_half_hull(std::deque< std::pair<double,double> > input,
                      std::deque< std::pair<double,double> > &output,
											std::pair<double,double> left, 
											std::pair<double,double> right,
                      int factor )
{
  //
  // The hull will always start with the left point, and end with the right
  // point. According, we start by adding the left point as the first point
  // in the output sequence, and make sure the right point is the last point 
  // in the input sequence.
  //
  input.push_back( right );
  output.push_back( left );
  //
  // The construction loop runs until the input is exhausted
  //
  while ( input.size() != 0 ) {
      //
      // Repeatedly add the leftmost point to the null, then test to see 
      // if a convexity violation has occured. If it has, fix things up
      // by removing the next-to-last point in the output suqeence until 
      // convexity is restored.
      //
      output.push_back( input.front() );
      //plot_hull( f, "adding a new point" );
			input.pop_front();
      //input.erase( input.begin() );
      while ( output.size() >= 3 ) {
          size_t end = output.size() - 1;
          if ( factor * direction( output[ end - 2 ], 
                                   output[ end ], 
                                   output[ end - 1 ] ) <= 0 ) {
              output.erase( output.begin() + end - 1 );
              //plot_hull( f, "backtracking" );
          }
          else
              break;
      }
  }
}


// We can do this by by translating the points so that p1 is at the origin,
// then taking the cross product of p0 and p2. The result will be positive,
// negative, or 0, meaning respectively that p2 has turned right, left, or
// is on a straight line.
//
static int direction( std::pair<double,double> p0,
                      std::pair<double,double> p1,
                      std::pair<double,double> p2 )
{
    return ( (p0.first - p1.first ) * (p2.second - p1.second ) )
           - ( (p2.first - p1.first ) * (p0.second - p1.second ) );
}





void simplifyHull(std::deque< std::pair<double,double> > &input,int numberOfEdges)
{
	int size = input.size();
	int i=0;
	int j;
	double a1,a2,b1,b2;
	double x, y, x1, y1, x2, y2;
	map<double,int> air;
	map<int,pair<double,double>> newPoint;
	double surface;

	while(size>numberOfEdges)
	{
		if(findIntersectionPoint(input[i],input[i+1],input[i+2],input[i+3],x,y,surface))
		{
			air[surface] = i;
			newPoint[i] = pair<double,double>(x,y);
		}
		i++;

		if(i == size-3)
		{
			if(findIntersectionPoint(input[i],input[i+1],input[i+2],input[0],x,y,surface))
			{
				air[surface] = i;
				newPoint[i] = pair<double,double>(x,y);
			}
			i++;

			if(findIntersectionPoint(input[i],input[i+1],input[0],input[1],x,y,surface))
			{
				air[surface] = i;
				newPoint[i] = pair<double,double>(x,y);
			}
			i++;

			if(findIntersectionPoint(input[i],input[0],input[1],input[2],x,y,surface))
			{
				air[surface] = i;
				newPoint[i] = pair<double,double>(x,y);
			}

			map<double,int>::iterator it = air.begin();
			j = it->second;
			if(j > size-4)
			{
				if(j == size-1)
				{
					input.pop_front();
					input.pop_front();
					input.push_front(newPoint[j]);
				}
				else if(j == size-2)
				{
					input.pop_back();
					input.pop_front();
					input.push_back(newPoint[j]);
				}
				else if(j == size-3)
				{
					input.pop_back();
					input.pop_back();
					input.push_back(newPoint[j]);
				}
			}
			else
			{
				input.erase(input.begin()+j+1, input.begin() + j+3);
				input.insert(input.begin()+j+1, newPoint[j]);
			}
			size += -1;
			i=0;
			air.clear();
		}

		

	}

}




bool findIntersectionPoint( pair<double,double> p0, 
													  pair<double,double> p1,
														pair<double,double> p2,
														pair<double,double> p3,
														double &x, double &y, double &surface)

{
	double deltax1;
	double deltay1;
	double deltax2;
	double deltay2;
	double x1,x2,y1,y2;
	double a1,a2,b1,b2;

	x1 = p1.first;
	y1 = p1.second;
	x2 = p2.first;
	y2 = p2.second;

	deltax1 = x1 - p0.first;
	deltay1 = y1-p0.second;
	deltax2 = x2 - p3.first;
	deltay2 = y2-p3.second;

	if(deltax1*deltay1*deltax2*deltay2 == 0)
	{
		if(deltax1 == 0 && deltax2 == 0) // the 2 segments are vertical
			return false;

		if(deltay1 == 0 && deltay2 == 0)	// the 2 segments are horizontal
			return false;

		if(deltax1 == 0 && deltay2 == 0)
		{
			x = x1;
			y = y2;
			surface = 0.5*fabs( (x - x2)*(y - y1) ); // right triangle
			return true;
		}

		if(deltay1 == 0 && deltax2 == 0)
		{
			x = x2;
			y = y1;
			surface = 0.5*fabs( (x - x1)*(y - y2) ); // right triangle
			return true;
		}

		if(deltax1 == 0)
		{
			x = x1;
			a2 = (y2 - p3.second)/(x2 - p3.first);
			b2 = y2-(a2*x2);
			y = a2*x + b2;
			surface = 0.5*fabs( (x2 - x)*(y1 - y) );
			return true;
		}
		if(deltay1 == 0)
		{
			y = y1;
			a2 = (y2 - p3.second)/(x2 - p3.first);
			b2 = y2-(a2*x2);
			x = (y - b2)/a2;
			surface = 0.5*fabs( (x1 - x)*(y2 - y) );
			return true;
		}
		if(deltax2 == 0)
		{
			x = x2;
			a1 = (y1 - p0.second)/(x1 - p0.first);
			b1 = y1-(a1*x1);
			y = a1*x + b1;
			surface = 0.5*fabs( (x1 - x)*(y2 - y) );
			return true;
		}
		if(deltay2 == 0)
		{
			y = y2;
			a1 = (y1 - p0.second)/(x1 - p0.first);
			b1 = y1-(a1*x1);
			x = (y - b1)/a1;
			surface = 0.5*fabs( (x2 - x)*(y1 - y) );
			return true;
		}
	}
	else
	{
		a1 = (y1-p0.second)/(x1 - p0.first);
		a2 = (y2-p3.second)/(x2 - p3.first);

		b1 = y1-(a1*x1);
		b2 = y2-(a2*x2);
		x = (b2-b1)/(a1-a2);
		y = (a1*x) + b1;
		surface = 0.5*fabs( ( (x1 - x)*(y2 - y) ) - ( (x2 - x)*(y1 - y) ) );
		return true;
	}

}


















// Computes the squared distance between segment [a,b] and point p
double squaredDistance(std::pair<double,double> const& a,
                       std::pair<double,double> const& b,
                       std::pair<double,double> const& p)
{
  double abx = b.first - a.first;
  double aby = b.second - a.second;
  double apx = p.first - a.first;
  double apy = p.second - a.second;
  double abap = abx*apx + aby*apy;
  if (abap <= 0)
    return apx*apx + apy*apy;		
  double abab = abx*abx + aby*aby;
  if (abab <= abap)
  {
    double dx = b.first - p.first;
    double dy = b.second - p.second;
    return dx*dx + dy*dy;
  }
  else
  {
    double abh = apx*aby - apy*abx;
    return abh*abh/abab;
  }
}

// Finds the index of the nearest polygon's edge to the point p
std::size_t findNearesEdge(std::deque< std::pair<double,double> > const& polygon,
                           std::pair<double,double> const& p)
{
  std::size_t result = 0;
  double smallerD2 = 1000000;
  for(std::size_t i = 0; i < polygon.size(); ++i)
  {
		// for each edge of the polygon, find the squaredDistance from the point 
    double d2 = squaredDistance(i == 0 ? polygon.back() : polygon[i - 1], polygon[i], p);
    if (d2 < smallerD2)
    {
      smallerD2 = d2;
      result = i;
    }
  }
  return result;
}

// Mean least squares fitter of a line segment
struct LineFitter
{
  double s1;
  double sx;
  double sy;
  double sxx;
  double sxy;
  double syy;

  LineFitter()
  {
    s1 = sx = sy = sxx = sxy = syy = 0.0;
  }

  void addPoint(std::pair<double,double> const& p)
  {
    s1 += 1.0;		// number of points
    sx += p.first;
    sy += p.second;
    sxx += p.first*p.first;
    sxy += p.first*p.second;
    syy += p.second*p.second;
  }

  void getCoefs(double& a, double& b, double& c) const
  {
    double y = (sxy*s1 - sx*sy)*2.0;
    double x = sy*sy - sx*sx + (sxx - syy)*s1;
    double theta = atan2(y, x)/2.0;
		a = -sin(theta);
		b = cos(theta);
    c = (sx*a + sy*b)/-s1;
  }
};



// Computes the intersection point between line 'a' and line 'b'
bool intersection(LineFitter const& a, LineFitter const& b, std::pair<double,double>& p)
{
  double a1, b1, c1;
  a.getCoefs(a1, b1, c1);
  double a2, b2, c2;
  b.getCoefs(a2, b2, c2);
  double det = a2*b1 - b2*a1;
  if (det == 0)
    return false;
  p.first = (b2*c1 - b1*c2)/det;
  p.second = (a1*c2 - a2*c1)/det;
  return true;
}

// Improves polygon approximation
bool adjustPolygonApproximation(std::deque< std::pair<double,double> > const& points,
                                std::deque< std::pair<double,double> >& polygon)
{
  // Update edge fitters
  std::vector<LineFitter> edgeFitters(polygon.size());
  for(std::size_t i = 0; i < points.size(); ++i)
    edgeFitters[findNearesEdge(polygon, points[i])].addPoint(points[i]);

  // Update polygon points
  for(std::size_t j = 0; j < polygon.size(); ++j)
  {
    if (!intersection(edgeFitters[j == 0 ? polygon.size() - 1 : j - 1], edgeFitters[j], polygon[j]))
      return false;
  }
  return true;
}


void findBestQuadrangle(std::deque< std::pair<double,double> > const& points,
                        std::deque< std::pair<double,double> >& result)
{

	std::deque< std::pair<double,double> > resultTemp(result);

  /* Finds an initial polygon approximation (the bounding box for example)
	double xMin = points[0].first;
  double xMax = xMin;
	double yMin = points[0].second;
  double yMax = yMin;

  for(std::size_t i = 0; i < points.size(); ++i)
  {
    double x = points[i].first;
    if (x < xMin) xMin = x;
    if (x > xMax) xMax = x;
    double y = points[i].second;
    if (y < yMin) yMin = y;
    if (y > yMax) yMax = y;
  }
  result.clear();
  result.push_back(std::make_pair(xMin, yMin));
  result.push_back(std::make_pair(xMin, yMax));
  result.push_back(std::make_pair(xMax, yMax));
  result.push_back(std::make_pair(xMax, yMin));
	*/

  // Iterative improvements
  for(int i = 0; i < 5; ++i) // 20 iterations, for example
    if(!adjustPolygonApproximation(points, result))
		{
			result = resultTemp;
			return;
		}
}













/*
// Computes the squared distance between segment [a,b] and point p
double squaredDistance(std::pair<double,double> const& a,
                       std::pair<double,double> const& b,
                       std::pair<double,double> const& p)
{
  double abx = b.first - a.first;
  double aby = b.second - a.second;
  double apx = p.first - a.first;
  double apy = p.second - a.second;
  double abap = abx*apx + aby*apy;
  if (abap <= 0)
    return apx*apx + apy*apy;
  double abab = abx*abx + aby*aby;
  if (abab <= abap)
  {
    double dx = b.first - p.first;
    double dy = b.second - p.second;
    return dx*dx + dy*dy;
  }
  else
  {
    double abh = apx*aby - apy*abx;
    return abh*abh/abab;
  }
}


// Finds the index of the nearest polygon's edge to the point p
std::size_t findNearesEdge(std::deque< std::pair<double,double> > const& polygon,
                           std::pair<double,double> const& p)
{
  std::size_t result = 0;
  double smallerD2 = 1000000;
  for(std::size_t i = 0; i < polygon.size(); ++i)
  {
    double d2 = squaredDistance(i == 0 ? polygon.back() : polygon[i - 1], polygon[i], p);
    if (d2 < smallerD2)
    {
      smallerD2 = d2;
      result = i;
    }
  }
  return result;
}


// Mean least squares fitter of a line segment
struct LineFitter
{
  double s1;
  double sx;
  double sy;
  double sxx;
  double sxy;
  double syy;

  LineFitter()
  {
    s1 = sx = sy = sxx = sxy = syy = 0.0;
  }

  void addPoint(std::pair<double,double> const& p, double w)
  {
    s1 += w;
    sx += w*p.first;
    sy += w*p.second;
    sxx += w*p.first*p.first;
    sxy += w*p.first*p.second;
    syy += w*p.second*p.second;
  }

  void getCoefs(double& a, double& b, double& c) const
  {
    double y = (sxy*s1 - sx*sy)*2.0;
    double x = sy*sy - sx*sx + (sxx - syy)*s1;
    double theta = atan2(y, x)/2.0;
    a = -sin(theta);
    b = cos(theta);
    c = (sx*a + sy*b)/-s1;
  }
};

// Computes the intersection point between line 'a' and line 'b'
bool intersection(LineFitter const& a, LineFitter const& b, std::pair<double,double>& p)
{
  double a1, b1, c1;
  a.getCoefs(a1, b1, c1);
  double a2, b2, c2;
  b.getCoefs(a2, b2, c2);
  double det = a2*b1 - b2*a1;
  if (det == 0)
    return false;
  p.first = (b2*c1 - b1*c2)/det;
  p.second = (a1*c2 - a2*c1)/det;
  return true;
}

// Improves polygon approximation
bool adjustPolygonApproximation(std::deque< std::pair<double,double> > const& points,
                                std::deque< std::pair<double,double> >& polygon)
{
  // Update edge fitters
  std::vector<LineFitter> edgeFitters(polygon.size());
  for(std::size_t i = 0; i < points.size(); ++i)
  {
    for(std::size_t j = 0; j < polygon.size(); ++j)
    {
      double d2 = squaredDistance(polygon[j], polygon[(j + 1)%polygon.size()], points[i]);
      edgeFitters[j].addPoint(points[i], exp(-d2));
    }
  }

  // Update polygon points
  for(std::size_t j = 0; j < polygon.size(); ++j)
  {
    if (!intersection(edgeFitters[j == 0 ? polygon.size() - 1 : j - 1], edgeFitters[j], polygon[j]))
      return false;
  }
  return true;
}

void findBestQuadrangle(std::deque< std::pair<double,double> > const& points,
                        std::deque< std::pair<double,double> >& result)
{
  // Finds an initial polygon approximation (the bounding box, for example)
  double xMin = DBL_MAX;
  double xMax = -DBL_MAX;
  double yMin = DBL_MAX;
  double yMax = -DBL_MAX;
  for(std::size_t i = 0; i < points.size(); ++i)
  {
    double x = points[i].first;
    if (x < xMin) xMin = x;
    if (x > xMax) xMax = x;
    double y = points[i].second;
    if (y < yMin) yMin = y;
    if (y > yMax) yMax = y;
  }
  result.clear();
  result.push_back(std::make_pair(xMin, yMin));
  result.push_back(std::make_pair(xMin, yMax));
  result.push_back(std::make_pair(xMax, yMax));
  result.push_back(std::make_pair(xMax, yMin));

  // Iterative improvements
  for(int i = 0; i < 20; ++i) // 20 iterations, for example
    adjustPolygonApproximation(points, result);
}

*/




void addEdge(int id1, int id2, map<pair<int,int>,int> &edgeNumber)
{
	if(id1 < id2)
		edgeNumber[pair<int,int>(id1,id2)] += 1;
	else
		edgeNumber[pair<int,int>(id2,id1)] += 1;
}