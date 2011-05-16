#include "Inc3dCHull_Vertex.h"
#include "Inc3dCHull_Face.h"
#include "Inc3dCHull_Edge.h"


namespace Inc3dCHull
{

	void Vertex::clean(Vertex* vertices, Edge* edges)
	{
		Edge* e;
		Vertex *v,*t;

		e = edges;
		do
		{
			e->vertices[0]->onHull = e->vertices[1]->onHull = true;
			e = e->next;
		} while(e != edges);

		while( vertices && vertices->isProcessed && (!vertices->onHull) )
		{
			v = vertices;
			Vertex::erase(vertices, v);
		}

		v = vertices->next;
		do
		{
			if(v->isProcessed &! v->onHull)
			{
				t = v;
				v = v->next;
				Vertex::erase(vertices, v);
			}
			else
				v = v->next;
		} while( v != vertices);

		v = vertices;
		do
		{
			v->duplicate = NULL;
			v->onHull = false;
			v = v->next;
		} while(v != vertices);

	}




}