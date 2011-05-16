#ifndef _INC_3D_CONVEX_HULL_EDGE
#define _INC_3D_CONVEX_HULL_EDGE

#include "Inc3dCHull_Vertex.h"
#include "Inc3dCHull_Face.h"

namespace Inc3dCHull
{
	class Face;
	class Vertex;
	typedef class Edge* ptrEdge;

	class Edge
	{
		friend class ConvexHull;
		friend class Face;
		friend class Vertex;

		Face* faces[2];
		Vertex* vertices[2];

		Face* newFace;
		bool toDelete;
		ptrEdge prev, next;

	public:
		//--------------------------------------------
		Edge()
		{
			this->faces[0] = this->faces[1] = this->newFace = NULL;
			this->vertices[0] = this->vertices[1] = NULL;
			this->toDelete = false;
		}
		//--------------------------------------------
		Edge(Vertex* first, Vertex* second)
		{
			this->vertices[0] = first;
			this->vertices[1] = second;
			this->faces[0] = this->faces[1] = this->newFace = NULL;
			this->toDelete = false;
		}
		//--------------------------------------------
		virtual Edge* Next(){return this->next;}
		//--------------------------------------------
		virtual Edge* Prev(){return this->prev;}
		//--------------------------------------------
		Vertex* firstVertex() {return this->vertices[0];}
		//--------------------------------------------
		Vertex* secondVertex(){return this->vertices[1];}
		//--------------------------------------------
		static void append(Edge* list, Edge* e){APPEND(list, e);}
		//--------------------------------------------
		static void erase(Edge* list, Edge* e){ERASE(list, e);}

		//--------------------------------------------
		static void clean(Edge* list);

	};

}

#endif