#ifndef __vtkPointSetNeighborhood_h
#define __vtkPointSetNeighborhood_h

#include "vtkDataSet.h"
#include "vtkPointSet.h"
#include "vtkNeighborhood.h"

#include <GsTL/geometry/geometry_algorithms.h>
#include <GsTL/geometry/covariance.h>
#include <GsTL/utils/KDTree.h>

#include <vector>

class vtkPointLocator;

class PointSetNeighborhood: public Neighborhood
	{
	public:
		typedef GeoValue value_type;
		typedef Neighborhood::iterator iterator;
		typedef Neighborhood::const_iterator const_iterator;
		typedef GeoValue::location_type location_type;
		
  public:
		PointSetNeighborhood( double a, double b, double c,
													double ang1, double ang2, double ang3,
													int max_neighbors,
                          int min_neighbors,
													vtkDataSet* pset,
													double* prop,
													const Covariance<location_type>* cov = 0 );
		virtual ~PointSetNeighborhood();
		virtual bool selectProperty( const std::string& prop_name );
		virtual const double* selectedProperty() const { return gridProperty; }
		virtual void findNeighbors( const GeoValue& center );
		virtual void maxSize( int s );
		virtual int maxSize() const { return MaxNeighbors; }
		virtual GeoValue center() const { return geoCenter; }
		virtual void setNeighbors( const_iterator begin, const_iterator end );

	protected:
		vtkDataSet* pointSet;
		double* gridProperty; //assay

		double x, y, z, angle1, angle2, angle3;
		int MaxNeighbors;
    int MinNeighbors;
		std::vector<int> candidates;
		GeoValue geoCenter;

		Anisotropic_norm_3d<CoordVector> norm;
		typedef Anisotropic_transform_3d<location_type> CoordTransform;
		CoordTransform* coordTransformation;
		Covariance<location_type>* covariance;

    //BEGIN USE_KDTREE
    KDTree<CoordVector, int> kdtree_;
    //END USE_KDTREE
    vtkPointLocator *Locator;
	};

#endif
