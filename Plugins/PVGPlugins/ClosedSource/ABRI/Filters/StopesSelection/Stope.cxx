
#include "Stope.h"
#include <vtkstd/map>

namespace
{
	
struct Key
{
	vtkIdType Id;
};
struct  key_comp
{
	bool operator()(const Key& k1, const Key& k2) const{
		if(k1.Id < k2.Id)	return 1;
		else return 0;
	}
};

//map
typedef vtkstd::map<Key, Stope *, key_comp > StopeMapBase;
typedef vtkstd::map<Key, Stope *, key_comp >::iterator StopeMapIterator;
}
// --------------------------------------
class InternalStopeMap: public StopeMapBase {};


/*========================================================================================
																	Class Stope
//========================================================================================*/
Stope::Stope() {this->ListOfBlocks = vtkIdList::New();} 

// --------------------------------------
Stope::Stope(const vtkIdType &Id, const vtkIdType &nBlocks) : StopeId( Id ), NumberOfBlocks( nBlocks )
{
	this->ListOfBlocks = vtkIdList::New();
} 

// --------------------------------------
Stope::~Stope(){ this->ListOfBlocks->Delete(); }  

// --------------------------------------
int Stope::AddBlock( vtkIdType id)
{
	if(this->ListOfBlocks->IsId(id)==-1)
	{
		this->ListOfBlocks->InsertNextId(id);
		this->NumberOfBlocks++;
		return 1;
	}
	else return 0;
}

//--------------------------------------
int  Stope::RemoveBlock( vtkIdType id)
{
	if(this->ListOfBlocks->IsId(id)!=-1)
	{	
		this->ListOfBlocks->DeleteId(id);
		this->NumberOfBlocks--;
		return 1;
	}
	else return 0;
}


//--------------------------------------
vtkIdType Stope::GetBlockId(vtkIdType i)
{
	return this->ListOfBlocks->GetId(i);
}


/*========================================================================================
																	Class StopeContainer
//========================================================================================*/
StopeContainer::StopeContainer(){	this->map = new InternalStopeMap();}

// --------------------------------------
StopeContainer::~StopeContainer()
	{
	if ( this->map )
		{
		StopeMapIterator it;
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
void StopeContainer::AddStope(Stope *stope)
{
	//this->map
	Key key = { stope->GetStopeId()};
	StopeMapIterator it = this->map->begin();
	this->map->insert( it, vtkstd::pair< Key ,Stope*>( key, stope ) );	
}

// --------------------------------------
Stope* StopeContainer::GetStope( const int Id)
{
	Key key = {Id};
	StopeMapIterator it;
  it = this->map->find( key );
	return ( it != this->map->end() ) ? it->second : NULL;
}

int StopeContainer::RemoveBlock(vtkIdType StopeId, vtkIdType id)
{
	Key key = {StopeId};
	StopeMapIterator it;
  it = this->map->find( key );
	if(it != this->map->end())
	{
		it->second->RemoveBlock(id);
		return 1;
	}
	else return 0;
}
