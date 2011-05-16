//Wrapper class for a stl map

#ifndef __vtkDataMinePropertyStorage_h
#define __vtkDataMinePropertyStorage_h

#include "dmfile.h"

class vtkDataSet;
class vtkVariantArray;
class InternalVector;

class PropertyItem
{
public:
	PropertyItem( const vtkStdString &name, const bool &numeric, const int& pos, const int &status);
	~PropertyItem();

	bool isNumeric;
	bool isSegmentable;
	bool isActive;

	int startPos;
	int endPos;

	vtkStdString name;

	vtkVariantArray *storage;

};

class PropertyStorage
{
public:
	PropertyStorage();
	~PropertyStorage();
	
	enum PropertyType{ UNIFORM=1, SEGMENTABLE=2 };

	//method to replace the old add methods
	void AddProperty( char* name, const bool &numeric, const int &pos, const int &status );
	
	//new method to replace the old get methods
	void AddValues( Data *values );
				
	//function added to allow support for 
	//segmentable properties from a stope summary file
	void Segment( const int &records );

	//add all our properties to the dataset
	void PushToDataSet( vtkDataSet *dataSet );

private:	
	InternalVector *properties;
  
};

#endif
