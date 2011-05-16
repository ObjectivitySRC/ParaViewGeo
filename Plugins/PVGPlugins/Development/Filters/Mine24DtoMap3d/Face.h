#ifndef __Face_h
#define __Face_h

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



using namespace std;

class Face 
{

protected:

	int vertices[4];
	list<int*> triangles;

public:

	//Face(void);
	//~Face(void);
	void addTriangle( int* triangle);
	list<int*> getTriangles();
	void setVertices(int p1, int p2, int p3, int p4);
	void getVertices(int* vert);
	



};

#endif