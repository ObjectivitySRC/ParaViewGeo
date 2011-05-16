#include "Inc3dCHull_ConvexHull.h"

#include "Inc3dCHull_Vertex.h"
#include "Inc3dCHull_Face.h"
#include "Inc3dCHull_Edge.h"

namespace Inc3dCHull
{
	// TODO: Clean up memory in the destructor

	//--------------------------------------------
	ConvexHull::ConvexHull(double *points[3], const int n)
	{
		Vertex* v0;
		for(int i=0; i<n; ++i)
		{
			v0 = new Vertex(i, points[i]);
			Vertex::append(this->vertices, v0);
		}

		while( collinearTest(v0, v0->next, v0->next->next) )
		{
			v0 = v0->next;
			if(v0 == this->vertices)
			{
				return;
			}
		}
		v0->isProcessed = true;
		Vertex* v1 = v0->next; v1->isProcessed = true;
		Vertex* v2 = v1->next; v2->isProcessed = true;

		Edge* e0 = new Edge(v0, v1); Edge::append(this->edges, e0);
		Edge* e1 = new Edge(v1, v2); Edge::append(this->edges, e1);
		Edge* e2 = new Edge(v2, v0); Edge::append(this->edges, e2);

		Face* f1 = new Face(v0, v1, v2, e0, e1, e2); Face::append(this->faces, f1);
		Face* f2 = new Face(v2, v1, v0, e1, e0, e2); Face::append(this->faces, f2);

		e0->faces[0] = f1;
		e1->faces[0] = f1;
		e2->faces[0] = f1;
		e0->faces[1] = f2;
		e1->faces[1] = f2;
		e2->faces[1] = f2;

		Vertex* v3 = v2->next;
		while(f1->vertexVisible(v3))
		{
			v3 = v3->next;
			if(v3->isProcessed)
				return;
		}
		vertices = v3;


		Vertex *v, *vNext;

		v = vertices;
		do
		{
			vNext = v->next;
			if( !v->isProcessed )
			{
				v->isProcessed = true;
				this->IncrementHull(v);
				Edge::clean(this->edges);
				Face::clean(this->faces);
				Vertex::clean(this->vertices, this->edges);
			}
			v = vNext;
		} while(v != vertices);
	}
	
	//---------------------------------------------
	void ConvexHull::getHull(vtkstd::list< vtkstd::pair<int,double*> > &points, vtkstd::list<int*> &triangles ) 
	{
		Face* f = this->faces;
		Vertex* v = this->vertices;

		if(!this->faces || !this->vertices)
		{
			return;
		}

		double* p;
		int* t;

		do
		{
			t = new int[3];
			t[0] = f->vertices[0]->id;
			t[1] = f->vertices[1]->id;
			t[2] = f->vertices[2]->id;
			triangles.push_back(t);
			f = f->next;
		}
		while (f != this->faces);

		do
		{
			p = new double[3];
 			p[0] = v->x(); p[1] = v->y(); p[3] = v->z();
			points.push_back(vtkstd::pair<int, double*> (v->id, p));
			v = v->next;
		}
		while (v != this->vertices);
	}

	//--------------------------------------------
	void ConvexHull::IncrementHull(Vertex* v)
	{
		bool onHull;
		Face* f = this->faces;
		do
		{
			if(f->vertexVisible(v) < 0) 
			{
				f->isVisible = true;
				onHull = true; 
			}
			f = f->next;
		} while(f != faces);

		if(!onHull)
		{
			v->onHull = false;
			return; 
		}

		Edge* e = this->edges;
		Edge* temp;
		do
		{
			temp = e->next;
			if(e->faces[0]->isVisible && e->faces[1]->isVisible)
			{
				e->toDelete = true;
			}
			else if(e->faces[0]->isVisible || e->faces[1]->isVisible)
			{
				e->newFace = this->AddNewFace(e, v);
			}
			e = temp;
		} while(e != this->edges);
	}

	//--------------------------------------------
	Face* ConvexHull::AddNewFace(Edge* e, Vertex*v)
	{
		Edge* newEdges[2];

		for(int i=0; i<2; ++i)
		{
			if( !(newEdges[i] = e->vertices[i]->duplicate) )
			{
				newEdges[i] = new Edge(e->vertices[i], v);
				Edge::append(this->edges, newEdges[i]);
				e->vertices[i]->duplicate = newEdges[i];
			}
		}

		Face* newFace = new Face(NULL, NULL, NULL, e, newEdges[0], newEdges[1]);
		Face::append(this->faces, newFace);
		this->reOrderCCW(newFace, e, v);

		for(int i=0; i<2; ++i)
		{
			for(int j=0; j<2; ++j)
			{
				if( ! newEdges[i]->faces[j] )
				{
					newEdges[i]->faces[j] = newFace;
					break;
				}
			}
		}
		return newFace;
	}

	//--------------------------------------------
	void ConvexHull::reOrderCCW(Face* f, Edge* e, Vertex* v)
	{
		Face* visibleFace;
		int i;
		Edge* tempEdge;

		if( e->faces[0]->isVisible)
			visibleFace = e->faces[0];
		else
			visibleFace = e->faces[1];

		for(i=0; visibleFace != e->faces[0]; ++i);

		if( visibleFace->vertices[(i+1)%3] != e->vertices[1] )
		{
			f->vertices[0] = e->vertices[1];
			f->vertices[1] = e->vertices[0];
		}
		else
		{
			f->vertices[0] = e->vertices[0];
			f->vertices[1] = e->vertices[1];
			// swap
			tempEdge = f->edges[1];
			f->edges[1] = f->edges[2];
			f->edges[2] = tempEdge;
		}
		f->vertices[2] = v;
	}



}