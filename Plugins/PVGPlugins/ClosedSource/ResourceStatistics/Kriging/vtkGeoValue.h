#ifndef _vtkGeoValue_h
#define _vtkGeoValue_h

#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include <vector>

/* Neccessary template class for Location type
   Triplet of coordinates (float type) */
template <class R> class GsTLVector
	{
	public:
    //GsTL requirements
    typedef R coordinate_type;
		typedef GsTLVector<R> difference_type;
		static const int dimension = 3;

	protected:
		R coords[dimension];


  public:
    //KD tree requirements?
    typedef R ScalarType;
    static int get_dim() {return GsTLVector<R>::dimension;}
    R length() 
      { 
      return std::sqrt(coords[0]*coords[0] + coords[1]*coords[1] + coords[2]*coords[2]);
      }
    void normalize() 
      {
      R l=length();
      coords[0] /= l;
      coords[1] /= l;
      coords[2] /= l;
      }

    GsTLVector()
			{
			coords[0] = 0;
			coords[1] = 0;
			coords[2] = 0;
			}

    GsTLVector( R x, R y, R z )
			{
			coords[0] = x;
			coords[1] = y;
			coords[2] = z;
			}

    GsTLVector( const GsTLVector<R>& gp )  
      { 
      coords[0] = gp.x(); 
      coords[1] = gp.y(); 
      coords[2] = gp.z(); 
      } 

		~GsTLVector() {}

		R& x() { return coords[0]; }
		R& y() { return coords[1]; }
		R& z() { return coords[2]; }

    const R& x() const { return coords[0]; } 
    const R& y() const { return coords[1]; } 
    const R& z() const { return coords[2]; } 

    GsTLVector<R>& operator = ( const GsTLVector<R>& t ) 
      { 
      if( this != &t ) 
        { 
        coords[0] = t.x(); 
        coords[1] = t.y();
        coords[2] = t.z();
        } 
        return *this; 
      } 

    GsTLVector<R> operator + (const difference_type& v ) const 
      { 
      return (GsTLVector<R>(coords[0]+v[0], coords[1]+v[1], coords[2]+v[2])); 
      } 

    //required for gstl
		R& operator [] (int i) { return coords[i]; }
		const R& operator[] (int i) const { return coords[i]; }

		/* need to implement "-" for CovarianceDistance in PointSetNeighborhood
		in order to satisfy operator () {... u2-u1 ... } in covariance.h */
		GsTLVector<R> operator - ( const GsTLVector<R>& t ) const 
			{
			return ( GsTLVector<R>(
        coords[0] - t.coords[0],
				coords[1] - t.coords[1],
				coords[2] - t.coords[2] ) );
			}
     /*GsTLVector<R> operator + (const GsTLVector<R>& t1 ) const 
       { 
          return (GsTLVector<R>( 
                  coords[0] + t1.coords[0], 
                  coords[1] + t1.coords[1], 
                  coords[2] + t1.coords[2] 
              ) 
          ); 
        }*/
    void operator -= ( const GsTLVector<R>& t ) 
      {
      coords[0] -= t.coords[0];
      coords[1] -= t.coords[1];
      coords[2] -= t.coords[2];
      }

    bool operator == (const GsTLVector<R>& t ) const 
      { 
      if( (coords[0] == t.coords[0]) &&  (coords[1] == t.coords[1]) &&  (coords[2] == t.coords[2]) ) 
        { 
        return true; 
        } 
      return false; 
      } 

    const R* raw_access() const { return coords; }
    R* raw_access() { return coords; }
	};

template <class R> 
std::ostream& operator << ( std::ostream& o,  const GsTLVector<R>& t ) 
  { 
  o << t[0] << " " << t[1] << " " << t[2] ; 
  return o; 
  }
template<class R>
inline R dot( const GsTLVector<R>& t1, const GsTLVector<R>& t2 ) 
  {
  return ( t1.x() * t2.x() + 
           t1.y() * t2.y() + 
           t1.z() * t2.z() );
  }

// CGLA requirements
namespace CGLA 
  {
  template<class R>
  inline GsTLVector<R> v_min( const GsTLVector<R>& t1, const GsTLVector<R>& t2 ) 
    {
    GsTLVector<R> v;
    v.x() = std::min( t1.x(), t2.x() );
    v.y() = std::min( t1.y(), t2.y() );
    v.z() = std::min( t1.z(), t2.z() );
    return v;
    }
  template<class R>
  inline GsTLVector<R> v_max( const GsTLVector<R>& t1, const GsTLVector<R>& t2 ) 
    {
    GsTLVector<R> v;
    v.x() = std::max( t1.x(), t2.x() );
    v.y() = std::max( t1.y(), t2.y() );
    v.z() = std::max( t1.z(), t2.z() );
    return v;
    }
}

typedef GsTLVector<double> CoordVector;



class GeoValue
	{
	public:
		typedef double property_type;
		typedef CoordVector location_type; // location of xyz position

		/**
		coordinate_type = typename R
		-> typename R = GsTLCoord
		-> GsTLCoord = float
		*/
		static const property_type invalidCoord;
		static const property_type nullData; //no_data_value

	private: 
		vtkDataSet* dataGrid;
		property_type* propArray;
		vtkIdType nodeID;
		mutable location_type locations;

	public:
		GeoValue();
		GeoValue( const GeoValue& rhs );
		GeoValue( vtkDataSet* grid, property_type* prop, vtkIdType nodeid );

		GeoValue& operator = ( const GeoValue& rhs );

		void init( vtkDataSet* grid, property_type* prop, vtkIdType nodeid );
		const vtkIdType& node_id() const { return nodeID; }
		vtkIdType& node_id() { return nodeID; }

		void setNodeID ( vtkIdType id );
		void setPropertyArray ( property_type* prop );
		const property_type* property_array() const { return propArray; }

		const vtkDataSet* grid() const { return dataGrid; }
		void setCachedLocation( const location_type& loc ) { locations = loc; }

		bool operator == ( const GeoValue &rhs ) const;
		bool operator != ( const GeoValue &rhs ) const;

		// isHardData


	public:
		void setNotInformed()
			{
			propArray[nodeID] = nullData;
			}

		bool isInformed() const
			{
      if ((nodeID >= 0) && (nodeID < dataGrid->GetNumberOfPoints()))
			  return ( propArray[nodeID] != nullData );
      return false;
			}

		property_type property_value() const //linear_comination.h requires this method name
			{
			if ((nodeID >= 0) && (nodeID < dataGrid->GetNumberOfPoints()))
				return propArray[nodeID];
			return nullData;
			}

		void setPropertyValue( const property_type& val )
			{
			if ((nodeID >= 0) && (nodeID < dataGrid->GetNumberOfPoints()))
				propArray[nodeID] = val;
			}

		const location_type& location() const
			{
			if (locations.x() == invalidCoord)
				{
        double pt[3];
	      this->dataGrid->GetPoint(nodeID, pt);
	      this->locations = location_type(pt[0],pt[1],pt[2]);
				}
			return locations;
			}
	};

#endif