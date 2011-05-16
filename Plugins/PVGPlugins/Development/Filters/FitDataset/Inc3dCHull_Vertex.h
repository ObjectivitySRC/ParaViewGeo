#ifndef _INC_3D_CONVEX_HULL_VERTEX
#define _INC_3D_CONVEX_HULL_VERTEX

//#include "Inc3dCHull_Edge.h"

#include "vtkMath.h"

namespace Inc3dCHull
{

//--------------------------------------------
#define APPEND( head, p ) if ( head) {\
		p->next = head;\
		p->prev = head->prev;\
		head->prev = p;\
		p->prev->next = p;\
	}\
	else {\
		head = p;\
		head->next = head->prev = p;\
	}
	

#define ERASE( head, p ) if ( head) {\
	if ( head == head->next ) \
		head = NULL; \
	else if ( p == head ) \
		head = head->next; \
	p->next->prev = p->prev; \
	p->prev->next = p->next; \
	delete p; \
	}

#define colinearEpsilon 0.0001


	class Edge;
	class Face;

	typedef class Vertex* ptrVertex;

	class Vertex
		{
			friend class ConvexHull;
			friend class Edge;
			friend class Face;

			double coord[3];
			int id;
			Edge* duplicate;
			bool onHull;
			bool isProcessed;
			ptrVertex prev, next;


		public: 

			
			Vertex(int i, const double p[3])
			{
				this->id = i;
				this->duplicate = NULL;
				this->onHull = false;
				this->isProcessed = false;
				for(int i=0; i<3; ++i){this->coord[i] = p[i];}
			}
			
			Vertex* Next(){return this->next;}
			
			Vertex* Prev(){return this->prev;}
			
			double* getCoordinates(){return this->coord;}
			
			double x(){return this->coord[0];}
			
			double y(){return this->coord[1];}
			
			double z(){return this->coord[2];}
			
			void setCoord(double p[3])
			{
				for(int i=0; i<3; ++i){this->coord[i] = p[i];}
			}
			
			static void append(Vertex* list, Vertex* v){APPEND(list, v);}
			
			static void erase(Vertex*list, Vertex* v){ERASE(list, v);}

			static void clean(Vertex* vertices, Edge* edges);
		};

}


#endif