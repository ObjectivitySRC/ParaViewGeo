// ->NAME vtkDataMineWireFrameReader->cxx
// Read DataMine binary files for single objects->
// point, perimeter (polyline), wframe<points/triangle>
// With or without properties (each property name < 8 characters)
// The binary file reading is done by 'dmfile->cxx'
//     99-04-12: Written by Jeremy Maccelari, visualn@iafrica->com
#include "PropertyStorage.h"

#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkVariantArray.h"

#include <vector>
#include <sstream>
using vtkstd::vector;
typedef vtkstd::vector< PropertyItem* > InternalBase;
// --------------------------------------
class InternalVector: public InternalBase {};
// --------------------------------------
PropertyItem::PropertyItem( const vtkStdString &name, const bool &numeric, const int& pos, const int &status )
	{
	this->isNumeric = numeric;
	this->name = name;	
	this->startPos = pos;
	this->endPos = pos+1; //will be updated by AddProperty

	//setup the defaults before we run the case statement
	//notice case 2: is a fall through
	this->isSegmentable = false;
	this->isActive = false;
	this->storage = NULL;
	switch(status)
		{
		case 2:
			this->isSegmentable = true;						
		case 1:			
			this->isActive = true;			
			this->storage = vtkVariantArray::New( );			
			break;
		case 0:
		default:		
			break;
		}	
	//right trim the name
	size_t endpos = this->name.find_last_not_of(" \t");
	if( vtkstd::string::npos != endpos )
		{
    this->name = this->name.substr( 0, endpos+1 );
		}
	}
PropertyItem::~PropertyItem()
	{
	if ( this->storage )
		{
		this->storage->Delete( );
		}
	}

// --------------------------------------
PropertyStorage::PropertyStorage()
{
	this->properties = new InternalVector( );
}		
// --------------------------------------
PropertyStorage::~PropertyStorage()
{	
	//clear() docs states it calls all classes destructors
	this->properties->clear( );
}		

// --------------------------------------
void PropertyStorage::AddProperty (char* name, const bool &numeric, const int &pos, const int &status)
{				
	vtkStdString vname(name);
	if ( this->properties->size() > 0 )
		{
		PropertyItem *lastItem = this->properties->back( );
		//check the last item in properties. We check both name and postion continunity to confirm if we
		//need to expand the item or create a new item	

		//note: lastItem->name is trimmed, so find on vname
		if ( vname.find(lastItem->name) == 0 && pos == lastItem->endPos )
			{
			//item is continous
			lastItem->endPos++;
			return;
			}
		}

	//this is hit on the first item, and even new item afterwards	
	PropertyItem *pi = new PropertyItem( vname, numeric, pos, status );
	this->properties->push_back( pi );
	return;	
}	

// --------------------------------------
void PropertyStorage::AddValues( Data *values )
{
	vtkstd::stringstream buffer;
	PropertyItem *item;
	vtkStdString tempBuf;

	for ( unsigned int i=0; i < this->properties->size( ); ++i)
		{
		item = this->properties->at( i );			
		if ( item->isActive )  //ignore non active 
			{			
			for ( int pos = item->startPos; pos < item->endPos; ++pos )
				{
				//only the first 4 characters have valid data in the non numeric use case
				item->isNumeric ? buffer << values[pos].v : buffer << vtkStdString(values[pos].c).substr(0,4);
				}		
		
			item->storage->InsertNextValue( vtkVariant(buffer.str( )) );
			buffer.str("");
			buffer.clear();
			}
		}		
}

// --------------------------------------
void PropertyStorage::Segment( const int &records )
{
	//only support the ability to segment a double property	
	PropertyItem *item;
	double value;
	unsigned int pos, end;
	for ( unsigned int i=0; i < this->properties->size( ); ++i)
		{
		item = this->properties->at( i );			
		if ( item->isSegmentable && item->isNumeric )
			{			
			end = item->storage->GetNumberOfValues( );
			for ( pos = end - records; pos < end; ++pos )
				{
				value = item->storage->GetValue( pos ).ToDouble( );			
				value /= records;
				item->storage->SetValue( pos, value );
				}
			
			}
		}
}

#define ConvertAndAdd(type, convertCommand )\
	{\
	type *tmp = type::New( );\
	tmp->SetName( item->name.c_str() );\
	tmp->SetNumberOfValues( size );\
	for ( vtkIdType i=0; i < size; ++i )\
		{\
		tmp->SetValue( i, variants->GetValue( i ).convertCommand( ) );\
		}\
	if ( numPoints == size )\
		{\
		dataSet->GetPointData( )->AddArray(tmp);\
		}\
	else\
		{\
		dataSet->GetCellData( )->AddArray(tmp);\
		}\
	}\
	
// --------------------------------------
void PropertyStorage::PushToDataSet( vtkDataSet *dataSet )
{
	//add the properties to the dataset, we detect if the property is point or cell based
	//If both(point&cell) are the same it goes to points
	PropertyItem *item;
	vtkVariantArray *variants;
	vtkIdType size;
	vtkIdType numPoints = dataSet->GetNumberOfPoints( );	
	bool AddArray= false;
	for ( unsigned int i=0; i < this->properties->size( ); ++i )
		{				
		item = this->properties->at( i );
		if ( !item->isActive )
			{
			continue;
			}	
		variants = item->storage;
		size = variants->GetNumberOfValues( );	

		if( numPoints == size )
			{
			if(dataSet->GetPointData()->GetAbstractArray(item->name.c_str())==NULL)
				AddArray = true;
			}
		else 
			{
			if(dataSet->GetCellData()->GetAbstractArray(item->name.c_str())==NULL)
				AddArray = true;
			}

		if(AddArray)
			{
			if ( item->isNumeric )
				{
				ConvertAndAdd( vtkDoubleArray, ToDouble );	
				AddArray = false;
				}
			else
				{
				ConvertAndAdd( vtkStringArray, ToString );		
				AddArray = false;
				}
			}
		}	
}