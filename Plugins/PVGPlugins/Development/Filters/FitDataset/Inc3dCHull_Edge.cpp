#include "Inc3dCHull_Vertex.h"
#include "Inc3dCHull_Face.h"
#include "Inc3dCHull_Edge.h"


namespace Inc3dCHull
{

	//--------------------------------------------
	void Edge::clean(Edge* list)
	{
		Edge *e, *tmp;
		e = list;

		do
		{
			if( e->newFace)
			{
				if(e->faces[0]->isVisible)
					e->faces[0] = e->newFace;
				else
					e->faces[1] = e->newFace;
				e->newFace = NULL;
			}
			e = e->next;
		}while(e != list);

		while(list && list->toDelete)
		{
			e = list;
			Edge::erase(list, e);
		}

		e = list->next;
		do
		{
			if(e->toDelete)
			{
				tmp = e;
				e = e->next;
				Edge::erase(list, tmp);
			}
			else e = e->next;
		} while(e != list);
	}
	
	
	
}