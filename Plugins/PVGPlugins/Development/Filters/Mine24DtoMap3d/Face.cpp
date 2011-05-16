#include <fstream>
#include <windows.h>
#include <conio.h>
#include <math.h>

#include "Face.h"

using namespace std;


void Face::addTriangle( int* triangle)
{
	triangles.push_back(triangle);
}

void Face::setVertices(int p1, int p2, int p3, int p4)
{
	vertices[0] = p1;
	vertices[1] = p2;
	vertices[2] = p3;
	vertices[3] = p4;
}

list<int*> Face::getTriangles(){return triangles;}

void Face::getVertices(int* vert)
{
	vert[0] = vertices[0];
	vert[1] = vertices[1];
	vert[2] = vertices[2];
	vert[3] = vertices[3];
}

/*
Face::~Face()
{
	for(list<int*>::iterator it = triangles.begin(); it!= triangles.end(); it++)
		 delete[] (*it);
}
*/