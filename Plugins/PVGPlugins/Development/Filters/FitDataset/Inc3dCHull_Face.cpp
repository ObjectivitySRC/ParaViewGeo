#include "Inc3dCHull_Vertex.h"
#include "Inc3dCHull_Face.h"
#include "Inc3dCHull_Edge.h"

namespace Inc3dCHull
{
	Face::Face(Vertex* v0 /*= NULL*/, Vertex* v1 /*= NULL*/, Vertex* v2 /*= NULL*/, 
			Edge* e0 /*= NULL*/, Edge* e1 /*= NULL*/, Edge* e2 /*= NULL*/)
	{
		this->vertices[0] = v0;
		this->vertices[1] = v1;
		this->vertices[2] = v2;

		this->edges[0] = e0;
		this->edges[1] = e1;
		this->edges[2] = e2;

		this->isVisible = false;
	}
	
	
	//--------------------------------------------
	int Face::vertexVisible(Vertex* v)
	{
		double ax = this->vertices[0]->x() - v->x();
		double ay = this->vertices[0]->y() - v->y();
		double az = this->vertices[0]->z() - v->z();

		double bx = this->vertices[1]->x() - v->x();
		double by = this->vertices[1]->y() - v->y();
		double bz = this->vertices[1]->z() - v->z();

		double cx = this->vertices[2]->x() - v->x();
		double cy = this->vertices[2]->y() - v->y();
		double cz = this->vertices[2]->z() - v->z();

		double det = ax * (by*cz - bz*cy) +
								 ay * (bz*cx - bx*cz) +
								 az * (bx*cy - by*cx);

		if(det > colinearEpsilon){return 1;}  // invisible
		if(det < colinearEpsilon){return -1;} // visible
		return 0;	// coplanar
	}	
	
	
	//--------------------------------------------
	void Face::clean(Face* list)
	{
		Face* f;
		Face* tmp;

		while(list && list->isVisible)
		{
			f = list;
			Face::erase(list, f);
		}
		do
		{
			if(f->isVisible)
			{
				tmp = f;
				f = f->next;
				Face::erase(list, tmp);
			}
			else
				f = f->next;
		} while(f != list);
	}	
	
	
}