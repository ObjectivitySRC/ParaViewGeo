//Wrapper class for a stl map

#ifndef __vtkDataMinePointMap_h
#define __vtkDataMinePointMap_h

class InternalMap;

class PointMap
{
public:
	PointMap();
	~PointMap();
  void SetID( int oldID, int newId );
  int GetID( int oldID );  

protected:  
  InternalMap *map;  
};
#endif
