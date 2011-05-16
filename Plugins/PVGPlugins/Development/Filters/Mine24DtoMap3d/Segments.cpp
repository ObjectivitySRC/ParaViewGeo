#include <fstream>
#include <math.h>
#include <vector>
#include "Segments.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------
void Segments::setFacesTriangles(map<int,list<int*>> &segments, map<int,pair<double,int>> &zSegments,
																 vtkPoints *inPoints)
{
	double *currentTuple;
	pair<double,int> Ztemp;
	Face faceTemp;
	int numberOfTopTriangles;
	int numberOfBottomTriangles;
	double zmed;

	for(map<int,list<int*>>::iterator stope=segments.begin(); stope!=segments.end(); stope++)
	{
		numberOfTopTriangles = 0;
		numberOfBottomTriangles = 0;

		zmed = 0;
		Ztemp = zSegments[stope->first];
		zmed = Ztemp.first / Ztemp.second;	// z value of the median plan of the current segment
																				// z med = sum(z(triangles))/ number of triangles	
		

		for(list<int*>::iterator Triangle = stope->second.begin(); Triangle != stope->second.end(); Triangle++)
		{
			currentTuple = NULL;
			currentTuple = inPoints->GetPoint((*Triangle)[0]);
			if(currentTuple[2] > zmed)
			{
				topFaces[stope->first].addTriangle(*Triangle);
				numberOfTopTriangles++;
			}
			else
			{
				bottomFaces[stope->first].addTriangle(*Triangle);
				numberOfBottomTriangles++;
			}
		}
		// if a face has less than 2 triangles, this face is incorrect
		if(numberOfTopTriangles < 2 || numberOfBottomTriangles < 2)
		{
			vtkErrorWithObjectMacro(inPoints," Error in stope number: "<<stope->first);
			topFaces.erase(stope->first);
			bottomFaces.erase(stope->first);
		}
	}

}
//--------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------
map<int,Face> Segments::getFaces(int i)
{
	if(i==1)
		return topFaces;
	return bottomFaces;
}
//--------------------------------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------------------------------
// this method will find the vertices of a given face.
// 1 -> top face
// 2 -> bottom face
//--------------------------------------------------------------------------------------------------------------
void Segments::setFacesVertices(vtkPoints *inPoints, int i)
{
	if(i == 1)
		setFacesVertices2(inPoints, topFaces);
	else if(i == 2)
		setFacesVertices2(inPoints, bottomFaces);	
}






//--------------------------------------------------------------------------------------------------------------
// this method will find the vertices of a given face.
//--------------------------------------------------------------------------------------------------------------
void Segments::setFacesVertices2(vtkPoints *inPoints, map<int,Face> &Faces )
{
	
	double point1[3];
	double point2[3];
	double point3[3];
	double vect1[3];
	double vect2[3];
	double vect3[3];
	double angle;
	double angle2;
	double angle3;
	list<int*> Triangles;

	vector<pair<int,double*>> facePoints(4);
	facePoints[0].second = new double[3];
	facePoints[1].second = new double[3];
	facePoints[2].second = new double[3];
	facePoints[3].second = new double[3];

	for(map<int,Face>::iterator face=Faces.begin(); face!=Faces.end(); face++)
	{
		map<int,double> angles;
		multimap<double,int> sortedAngles;

		Triangles = (face->second).getTriangles();
		for(list<int*>::iterator triangle = Triangles.begin(); triangle != Triangles.end(); triangle++)
		{
			inPoints->GetPoint((*triangle)[0], point1);
			inPoints->GetPoint((*triangle)[1], point2);
			inPoints->GetPoint((*triangle)[2], point3);

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
			angle3 = angle3 /((vtkMath::Norm(vect3))*(vtkMath::Norm(vect2)));
			angle3 = acos(angle3);			

			angles[(*triangle)[0]] += angle;
			angles[(*triangle)[1]] += angle2; 
			angles[(*triangle)[2]] += angle3; 

			// delete triangle to free memory ?
		}

		if(angles.size()<4)
		{
			vtkErrorWithObjectMacro(inPoints," Error in stope number: "<<face->first);
			continue;
		}

		for(map<int,double>::iterator it = angles.begin(); it != angles.end(); it++)
			sortedAngles.insert(pair<double,int>(it->second,it->first));

		int iAngle = 0;
		for(multimap<double,int>::iterator iter3 = sortedAngles.begin(); iter3 != sortedAngles.end(); iter3++)
		{			
			facePoints[iAngle].first = iter3->second;
			inPoints->GetPoint(iter3->second, facePoints[iAngle].second);
			if(iAngle == 3)
				break;
			iAngle += 1;
		}

		if(goodConfig(facePoints[0],facePoints[1],facePoints[2],facePoints[3], face->second) == 1)
			continue;
		if(goodConfig(facePoints[0],facePoints[1],facePoints[3],facePoints[2], face->second) == 1)
			continue;
		if(goodConfig(facePoints[0],facePoints[2],facePoints[1],facePoints[3], face->second) == 1)
			continue;
		goodConfig(facePoints[0],facePoints[2],facePoints[3],facePoints[1], face->second);
	}
	}




//--------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
bool Segments::goodConfig(pair<int,double*>&pt1, pair<int,double*>&pt2,
																	pair<int,double*>&pt3, pair<int,double*>&pt4, Face &face)
{
	double v1[3];
	double v2[3];
	double v3[3];
	double v4[3];

	double normal1[3];
	double normal2[3];

	v1[0] = pt1.second[0]-pt2.second[0];
	v1[1] = pt1.second[1]-pt2.second[1];
	v1[2] = pt1.second[2]-pt2.second[2];

	v2[0] = pt3.second[0]-pt2.second[0];
	v2[1] = pt3.second[1]-pt2.second[1];
	v2[2] = pt3.second[2]-pt2.second[2];

	v3[0] = pt1.second[0]-pt3.second[0];
	v3[1] = pt1.second[1]-pt3.second[1];
	v3[2] = pt1.second[2]-pt3.second[2];

	v4[0] = pt4.second[0]-pt3.second[0];
	v4[1] = pt4.second[1]-pt3.second[1];
	v4[2] = pt4.second[2]-pt3.second[2];

	vtkMath::Cross(v2,v1,normal1);
	vtkMath::Cross(v4,v3,normal2);

	if(normal1[2]*normal2[2]>0)
	{
		if(normal1[2]>0)
		{
			face.setVertices(pt1.first,pt2.first,pt3.first,pt4.first);
			return true;
		}
		face.setVertices(pt4.first,pt3.first,pt2.first,pt1.first);
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------------------------------------------