//wrapper for stl map class
//it is used to keep the id reference for points from a datamine file
//to the point ids used in vtkPointData

#include "Activity.h"

#include "vtkStdString.h"
#include <vtkstd/vector>
#include <vtkstd/map>
#include <sstream>

//init variables
unsigned int Activity::NextId=0; 

namespace
{
struct MapKey
{
	int type;
	int level;
	int point;
};

struct  key_compare {
  bool operator()( const MapKey& k1, const MapKey& k2 ) const {
    return k1.type < k2.type
      || ( k1.type == k2.type && ( k1.level < k2.level 
      || ( k1.level == k2.level && k1.point < k2.point ) 
      )
      );
  }
};
}

//vector
typedef vtkstd::vector< Activity * > VectorBase;
typedef vtkstd::vector< Activity * >::iterator VectorIterator;

//storage map
typedef vtkstd::map< MapKey, Activity*, key_compare > MapBase;
typedef vtkstd::map< MapKey, Activity*, key_compare >::iterator MapIterator;

//point map
typedef vtkstd::pair< int, int > PMapKey;
typedef vtkstd::map< PMapKey, vtkStdString > GMapBase;
typedef vtkstd::map< PMapKey, vtkStdString >::iterator GMapIterator;
typedef vtkstd::map< PMapKey, int > PMapBase;
typedef vtkstd::map< PMapKey, int >::iterator PMapIterator;


// --------------------------------------
class InternalVector: public VectorBase {};
class InternalMap: public MapBase {};
class InternalMapIterator : public MapIterator {};
class InternalLevelPointMap: public PMapBase {};
class InternalLevelGuidMap: public GMapBase {};


// --------------------------------------
//		CLASS: Activity
// --------------------------------------

// --------------------------------------
Activity::Activity() : Id(Activity::GetNextId())
{
	this->Successors = new InternalVector();
} 

// --------------------------------------
Activity::Activity( const int &type, const int &levelId, const int &pointId) : Id(Activity::GetNextId()),
	Type( type ), LevelId( levelId ), PointId( pointId )
{
	this->Successors = new InternalVector();
} 

// --------------------------------------
Activity::~Activity()
{
	if ( this->Successors )
		{
		delete this->Successors;
		}
}  

// --------------------------------------
bool Activity::AddSuccessor( Activity *act )
{
	if (this && act && act->GetId( ) != this->GetId( ) && act->GetGUID() != this->GetGUID( ) )
		{
		this->Successors->push_back( act );
		return true;
		}
	return false;
}
// --------------------------------------
vtkStdString Activity::GetDependencies( )
{
	vtkstd::stringstream buffer;
	Activity *act;
	for ( unsigned int i=0; i < this->Successors->size( ); ++i )
		{
		act = this->Successors->at( i );		
		buffer << "3," << act->GetGUID( ) <<","<< this->GetGUID( )<<",FS,0d"<<vtkstd::endl;
		}
	return buffer.str( );
	
}
// --------------------------------------
vtkStdString Activity::GetName( )
	{
	return Activity::GenerateName( this->Type, this->PointId);
	}
// --------------------------------------
vtkStdString Activity::GenerateName( const int &type, const int &pointId )
{
	vtkstd::stringstream buffer;
	buffer << type << ":" << pointId;
	return buffer.str();
};

// --------------------------------------
//		CLASS: Activity Storage
// --------------------------------------

// --------------------------------------
ActivityStorage::ActivityStorage()
	{	
	this->map = new InternalMap();	
	}

// --------------------------------------
ActivityStorage::~ActivityStorage()
	{
	if ( this->map )
		{
		MapIterator it;
		it = this->map->begin();
		while ( it != this->map->end() )
			{
			delete it->second;
			++it;
			}	
		delete this->map;
		}	
	}

// --------------------------------------
void ActivityStorage::AddActivity(Activity *act)
{
	MapKey key = { act->GetType(), act->GetLevelId() , act->GetPointId() };
	MapIterator it;
	it = this->map->begin();
	this->map->insert( it, vtkstd::pair< MapKey ,Activity*>( key, act ) );		
}

// --------------------------------------
Activity* ActivityStorage::GetActivity( const int &type, const int &levelId, const int &pointId )
{
	MapKey key = {type, levelId, pointId };
	MapIterator it;
  it = this->map->find( key );
	return ( it != this->map->end() ) ? it->second : NULL;
}

// --------------------------------------
vtkStdString ActivityStorage::GenerateDependencies( )
{
	vtkstd::stringstream buffer;
	MapIterator it;	
	it = this->map->begin( );

	//header for proper file support
	buffer << "1,Dependencies"<<endl;
	buffer << "2,From,To,Type,Lag" << endl;

	while ( it != this->map->end() )
		{
		buffer << it->second->GetDependencies( );	
		++it;	
		}	
	
	return buffer.str();	
}

// --------------------------------------
//		CLASS: Guid Map Storage
// --------------------------------------

// --------------------------------------
LevelGuidMap::LevelGuidMap()
{
	this->map = new InternalLevelGuidMap();
} 

// --------------------------------------
LevelGuidMap::~LevelGuidMap()
{
	this->map->clear();
	delete this->map;
}  

// --------------------------------------
vtkStdString LevelGuidMap::GetGuid(  const int &level, const int &point )
{
	PMapKey key( level, point );
	GMapIterator it;
  it = this->map->find( key );
	return ( it != this->map->end() ) ? it->second : "-1";
}

// --------------------------------------
void LevelGuidMap::AddGuid( const int &level, const int &point, const vtkStdString &guid )
{
	PMapKey key( level, point );
	GMapIterator it;	
	it = this->map->begin();
	this->map->insert( it, vtkstd::pair< PMapKey ,vtkStdString>( key, guid ) );	  	
}

// --------------------------------------
//		CLASS: Point Map Storage
// --------------------------------------

// --------------------------------------
LevelPointMap::LevelPointMap()
{
	this->map = new InternalLevelPointMap();
} 

// --------------------------------------
LevelPointMap::~LevelPointMap()
{
	this->map->clear();
	delete this->map;
}  

// --------------------------------------
int LevelPointMap::GetPointId(  const int &level, const int &point )
{
	PMapKey key( level, point );
	PMapIterator it;
  it = this->map->find( key );
	return ( it != this->map->end() ) ? it->second : -1;
}

// --------------------------------------
void LevelPointMap::AddPoint( const int &level, const int &point, const int &id )
{
	PMapKey key( level, point );
	PMapIterator it;	
	it = this->map->begin();
	this->map->insert( it, vtkstd::pair< PMapKey ,int>( key, id ) );	  	
}  