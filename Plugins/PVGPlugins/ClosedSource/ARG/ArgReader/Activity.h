//Wrapper class for a stl map

#ifndef __Activity_h
#define __Activity_h

#include "vtkStdString.h"
class InternalVector;
class InternalMap;
class InternalMapIterator;
class InternalLevelPointMap;
class InternalLevelGuidMap;

class Activity
{
public:
	Activity();
	Activity(const int &type, const int& levelId, const int &pointId);	
	~Activity();

	static int GetNextId(){ return NextId++;};
	
	bool AddSuccessor( Activity *act );

	virtual vtkStdString GetDependencies( ); //generates the activities successors as a string

	inline void SetGUID( const vtkStdString & guid ) { Guid = guid; };
	inline void SetType( const int & type ) { Type = type; };
	inline void SetLevelId( const int & id ){ PointId = id; };

	inline const unsigned int& GetId() const { return Id; };

	inline vtkStdString GetGUID( ) const { return Guid; };
	inline int GetPointId() const { return PointId; };
	inline int GetLevelId() const { return LevelId; };
	inline int GetType( ) const { return Type; };

	vtkStdString GetName( );
	static vtkStdString GenerateName( const int &type, const int &pointId );

	//BTX
	enum ActivityType{ UNDEFINED=-1, DECLINE, DEVELOPMENT, STOPES };
	//ETX

protected:  	
	InternalVector *Successors;

	//pointId & Type & levelId are needed to figure out what actual line this activity relates back too
	int Type;	
	int PointId; 	
	int LevelId;

	//Internal ID we might need in the future
	const unsigned int Id;  

	//GUID from the input file
	vtkStdString Guid;

private:
	static unsigned int NextId;
};

class ActivityStorage
{
public:
	ActivityStorage();
	~ActivityStorage();

	void AddActivity(Activity *act);
	Activity* GetActivity( const int &type, const int &levelId, const int &pointId );

	//method for printing Dependecy links
	vtkStdString GenerateDependencies( );


private:	
	InternalMap *map;
	InternalMapIterator *iterator;

};

class LevelGuidMap
{
public:
	LevelGuidMap();
	~LevelGuidMap();
  void AddGuid( const int &level, const int &point, const vtkStdString &guid);
  vtkStdString GetGuid( const int &level, const int &point );  

protected:  
  InternalLevelGuidMap *map;  
};

class LevelPointMap
{
public:
	LevelPointMap();
	~LevelPointMap();
  void AddPoint( const int &level, const int &point, const int &id);
  int GetPointId( const int &level, const int &point );  

protected:  
  InternalLevelPointMap *map;  
};
#endif