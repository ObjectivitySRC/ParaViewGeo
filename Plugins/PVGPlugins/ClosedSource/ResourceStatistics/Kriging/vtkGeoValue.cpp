#include "vtkGeoValue.h"

// constants
const double GeoValue::invalidCoord = (const double)-9.9e30;
const double GeoValue::nullData = (const double)-9966699;


// Constructor
GeoValue::GeoValue()
: dataGrid(0), propArray(0), nodeID(-1), 
locations( invalidCoord, invalidCoord, invalidCoord ) {}


GeoValue::GeoValue(const GeoValue &rhs)
: dataGrid(rhs.dataGrid), 
	propArray(rhs.propArray), 
	nodeID(rhs.nodeID),
	locations( rhs.locations ) {}


GeoValue::GeoValue(vtkDataSet *grid, double *prop, vtkIdType nodeid)
: dataGrid( grid ),
	propArray( prop ),
	nodeID( nodeid )
	{
	double pt[3];
	this->dataGrid->GetPoint(nodeid, pt);
	this->locations = location_type(pt[0],pt[1],pt[2]);
	}

// operator assignment (=) overriding
GeoValue& GeoValue::operator = ( const GeoValue &rhs )
	{
	if (*this != rhs )
		{
		this->dataGrid = rhs.dataGrid;
		this->propArray = rhs.propArray;
		this->nodeID = rhs.nodeID;
		this->locations = rhs.locations;
		}

	return *this;
	}

// Initializing
void GeoValue::init ( vtkDataSet *grid, double *prop, vtkIdType nodeid )
	{
	this->dataGrid = grid;
	this->propArray = prop;
	this->nodeID = nodeid;
	double pt[3];
	grid->GetPoint(nodeid, pt);
	this->locations = location_type(pt[0],pt[1],pt[2]);
	}


// Setters
void GeoValue::setPropertyArray( double *prop )
	{
	this->propArray = prop;
	}

void GeoValue::setNodeID( vtkIdType id )
	{
	if (this->nodeID != id )
		{
		this->nodeID = id;
		double pt[3];
	  this->dataGrid->GetPoint(nodeID, pt);
	  this->locations = location_type(pt[0],pt[1],pt[2]);
		}
	}

// operator == and != overriding
bool GeoValue::operator == ( const GeoValue& rhs ) const
	{
	return (this->dataGrid == rhs.dataGrid &&
					this->propArray == rhs.propArray &&
					this->nodeID == rhs.nodeID ); //location = rhs.location is commented out in original
	}

bool GeoValue::operator != ( const GeoValue& rhs ) const
	{
	return !(*this == rhs );
	}