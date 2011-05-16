#ifndef _vtkNeighborhood_h
#define _vtkNeighborhood_h

#include "vtkGeoValue.h"
#include <vector>


class Neighborhood
	{
	public:
		typedef GeoValue value_type;
		typedef std::vector<GeoValue>::iterator iterator;
		typedef std::vector<GeoValue>::const_iterator const_iterator;

	protected:
		std::vector<GeoValue> neighbors;
		bool includes_center;

	public:
		Neighborhood():includes_center(true) {}
		virtual ~Neighborhood() {}

		virtual void findNeighbors( const GeoValue& center ) = 0;
		virtual GeoValue center() const = 0;

		virtual bool selectProperty( const std::string& prop_name ) = 0;
		virtual const double* selectedProperty() const = 0;

		bool includesCenter() const { return includes_center; }
		virtual void includesCenter( bool on ) { includes_center = on; }

		iterator begin() { return neighbors.begin(); }
		iterator end() { return neighbors.end(); }
		const_iterator begin() const { return neighbors.begin(); }
		const_iterator end() const { return neighbors.end(); }

		virtual int size() const { return neighbors.size(); }
		virtual void maxSize( int s ) = 0;
		virtual int maxSize() const = 0;

		// is_empty() - method name required by OK_constraints.cc
		bool is_empty() const { return neighbors.empty(); } 

		void clear() { neighbors.clear(); }
		virtual void setNeighbors ( const_iterator begin, const_iterator end ) = 0;
	};

#endif
