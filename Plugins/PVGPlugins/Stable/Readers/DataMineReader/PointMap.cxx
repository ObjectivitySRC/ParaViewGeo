//wrapper for stl map class
//it is used to keep the id reference for points from a datamine file
//to the point ids used in vtkPointData

#include "PointMap.h"
#include <vtkstd/map>

typedef vtkstd::map< int, int > MapBase;
typedef vtkstd::map< int, int >::iterator MapIterator;

// --------------------------------------
class InternalMap: public MapBase {};

// --------------------------------------
PointMap::PointMap()
{
	this->map = new InternalMap();
} 

// --------------------------------------
PointMap::~PointMap()
{
	this->map->clear();
	delete this->map;
}  

// --------------------------------------
int PointMap::GetID( int oldID )
{
	MapIterator it;
  it = this->map->find( oldID );
  if (it != this->map->end() )		
		{
		//id is located
		return it->second;
		}
	else
		{
		//return a bad ID
		return -1;
		}
}

// --------------------------------------
void PointMap::SetID( int oldID, int newId )
{
	MapIterator it;
	it = this->map->begin();
	this->map->insert( it, vtkstd::pair<int,int>( oldID, newId ) );	  
}