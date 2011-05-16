#ifndef __Segments_h
#define __Segments_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include <math.h>
#include <algorithm>
#include <ostream>
#include<iostream>
#include "vtkMath.h"
#include "vtkMine24DtoMap3D.h"
#include <list>
#include "Face.h"
#include <map>



using namespace std;

class Segments 
{

protected:

	//Face face;
	map<int,Face> bottomFaces;
	map<int,Face> topFaces;

public:

	void setFacesTriangles(map<int,list<int*>> &segments, map<int,pair<double,int>> &zSegments, 
		vtkPoints *inPoints);
	void setFacesVertices(vtkPoints *inPoints, int i);
	void setFacesVertices2(vtkPoints *inPoints, map<int,Face> &Faces);
	bool goodConfig(pair<int,double*>&pt1, pair<int,double*>&pt2,
						pair<int,double*>&pt3, pair<int,double*>&pt4, Face &face);
	map<int,Face> getFaces(int i);



};


#endif