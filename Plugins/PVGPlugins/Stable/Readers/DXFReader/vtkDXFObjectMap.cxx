/*
.NAME vtkDXFObjectMap.cxx
By MIRARCO::Tim Anema
.SECTION Description
This is a Wrapper Class to make DXFParser use
vtkstd::map instead of vtkCollection
to facilitate constant time searches for layers
*/

#include "vtkDXFObjectMap.h"

//---------------------------------------------------------------------------------
vtkDXFObjectMap::vtkDXFObjectMap(){}

//---------------------------------------------------------------------------------
vtkDXFObjectMap::~vtkDXFObjectMap()
{
	ObjectMap::iterator iter;
	for(iter = this->map.begin(); (iter != this->map.end()); ++iter)
		{	
		iter->second->Delete();
		iter->second = NULL;
		}
	this->map.erase(this->map.begin(), this->map.end());
}

//---------------------------------------------------------------------------------
void vtkDXFObjectMap::Delete()
{
	this->~vtkDXFObjectMap();
}

//---------------------------------------------------------------------------------
vtkDXFObjectMap *vtkDXFObjectMap::New()
{
	return new vtkDXFObjectMap;
}

//---------------------------------------------------------------------------------
vtkDXFBlock* vtkDXFObjectMap::GetItemAsObject(int i)
{	
	ObjectMap::iterator iter;
	int j = 0;
	for(iter = this->map.begin(); (iter != this->map.end()) && (j != i); ++iter, j++){}
	if(iter != this->map.end())
		{
		//iter->second->setName(iter->first);
		return iter->second;
		}
	return NULL;
}

//---------------------------------------------------------------------------------
vtkDXFBlock* vtkDXFObjectMap::GetItemAsObject(vtkstd::string name)
{	
	if(this->map.find(name)->second)
		{
		return this->map.find(name)->second;
		}
	return NULL;
}

//---------------------------------------------------------------------------------
void vtkDXFObjectMap::AddItem(vtkstd::string name, vtkDXFBlock *object)
{
	vtkDXFBlock *in = vtkDXFBlock::New();
	in->ShallowCopy(object);
	this->map.insert(vtkstd::pair<vtkstd::string, vtkDXFBlock*>(name, in));
}

//---------------------------------------------------------------------------------
int vtkDXFObjectMap::GetNumberOfItems()
{
	return this->map.size();
}