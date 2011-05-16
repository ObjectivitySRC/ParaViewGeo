#ifndef _INC_3D_CONVEX_HULL_FACE
#define _INC_3D_CONVEX_HULL_FACE

#include "Inc3dCHull_Edge.h"
#include "Inc3dCHull_Vertex.h"


namespace Inc3dCHull
{

	class Vertex;
	class Edge;

	typedef class Face* ptrFace;

	class Face
	{
		friend class ConvexHull;
		friend class Vertex;
		friend class Edge;

		Edge* edges[3];
		Vertex* vertices[3]; // the redundance of vertices is introduced on purpose
												 // because it simplifies the handling of faces
												 // must be maintained counter-clockwise
		
		bool isVisible; // used for temporary purpose

		ptrFace prev, next;

	public:

		Face(Vertex* v0 = NULL, Vertex* v1= NULL, Vertex* v2= NULL, 
		Edge* e0= NULL, Edge* e1= NULL, Edge* e2= NULL);

		virtual Face* Next(){return this->next;}
		
		virtual Face* Prev(){return this->prev;}
		
		static void append(Face* list, Face* f){APPEND(list, f);}
	
		static void erase(Face* list, Face* f){ERASE(list, f);}

		int vertexVisible(Vertex* v);

		static void clean(Face* list);
	};
}

#endif