#ifndef _INC_3D_CONVEX_HULL_CONVEX_HULL
#define _INC_3D_CONVEX_HULL_CONVEX_HULL

#include "Inc3dCHull_Vertex.h"
#include <vtkstd/list>

namespace Inc3dCHull
{	

	class ConvexHull
	{
		Vertex* vertices;
		Edge* edges;
		Face* faces;

	public:

		
		
		ConvexHull(double *points[3], const int n);

		void IncrementHull(Vertex* v);

		Face* AddNewFace(Edge* e, Vertex*v);

		void reOrderCCW(Face* f, Edge* e, Vertex* v);

		//--------------------------------------------
		bool collinearTest(Vertex* a, Vertex* b, Vertex* c)
		{
			return fabs( 
				( c->z() - a->z() ) * ( b->y() - a->y() ) - 
				( b->z() - a->z() ) * ( c->y() - a->y() )  ) < colinearEpsilon
					&& fabs(
				( b->z() - a->z() ) * ( c->x() - a->x() ) - 
				( b->x() - a->x() ) * ( c->z() - a->z() )  ) < colinearEpsilon
					&& fabs(	
				( b->x() - a->x() ) * ( c->y() - a->y() ) - 
				( b->y() - a->y() ) * ( c->x() - a->x() )  ) < colinearEpsilon ;
		}

		//--------------------------------------------
		void getHull(vtkstd::list< vtkstd::pair<int,double*> > &points, vtkstd::list<int*> &triangles );
	};




}


#endif