/*
.NAME vtkDXFObjectMap.h
By MIRARCO::Tim Anema
.SECTION Description
This is a Wrapper Class to make DXFParser use
vtkstd::map instead of vtkCollection
to facilitate constant time searches for layers
*/

#ifndef __vtkDXFObjectMap_h
#define __vtkDXFObjectMap_h

#include "vtkObject.h"
#include "vtkDXFLayer.h"
#include "vtkDXFBlock.h"
#include <vtkstd/map>

class vtkDXFObjectMap
{
public:
	int GetNumberOfItems();
	
	//origonal method used to find the object needed
	//still used for output itteration though
	vtkDXFBlock* GetItemAsObject(int i);
	//new method to get the object in constant time
	//just fund the object with the key name
	vtkDXFBlock* GetItemAsObject(vtkstd::string name);

	void AddItem(vtkstd::string name, vtkDXFBlock *object);
	static vtkDXFObjectMap *New();
	void Delete();

protected:
	vtkDXFObjectMap();
	~vtkDXFObjectMap();

private:
	typedef vtkstd::map<vtkstd::string, vtkDXFBlock*> ObjectMap;
	ObjectMap map;
	
};

#endif