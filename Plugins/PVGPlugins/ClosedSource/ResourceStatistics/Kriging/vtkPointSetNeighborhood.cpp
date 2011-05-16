#include "vtkPointSetNeighborhood.h"
#include "vtkPointLocator.h"
#include "vtkIdList.h"
#include "vtkPolyData.h"

//#define USE_KD_TREE
#define USE_LOCATOR

class CovarianceDistance
  {
  private:
    CoordVector center;
    Covariance<CoordVector> covariance;

  public:
    CovarianceDistance( const CoordVector& cen, const Covariance<CoordVector>& cov )
      : center( cen ), covariance( cov ) {}
    bool operator () ( const GeoValue& p1, const GeoValue& p2 )
      {
      return covariance(p1.location(), center) > covariance(p2.location(), center);
      }
  };

// Constructors
PointSetNeighborhood::PointSetNeighborhood(double a, double b, double c, double ang1, double ang2, double ang3, int max_neighbors, int min_neighbors, vtkDataSet* pset, double* prop, const Covariance<location_type>* cov ): norm( a, b, c, ang1, ang2, ang3 )
  {
  this->pointSet = pset;
  this->gridProperty = prop;
  this->MaxNeighbors = max_neighbors;
  this->MinNeighbors = min_neighbors;
  this->x = a;
  this->y = b;
  this->z = c;
  this->angle1 = ang1;
  this->angle2 = ang2;
  this->angle3 = ang3;

  if (cov)
    this->covariance = new Covariance<location_type>( *cov );
  else
    this->covariance = 0;

  this->coordTransformation = new CoordTransform( this->x, this->y, this->z, 
    this->angle1, this->angle2, this->angle3 );

#ifdef USE_LOCATOR
  this->Locator = vtkPointLocator::New();
  vtkPoints *blah = vtkPoints::New();
  //yess this is a stupid frakkin way to do it
  for (int i = 0; i < this->pointSet->GetNumberOfPoints(); i++)
    {
    double point[3];
    this->pointSet->GetPoint(i,point);
    location_type coord = location_type(point[0], point[1], point[2]);
    location_type transformed = (*this->coordTransformation)(coord);
    double tpoint[3];
    tpoint[0] = transformed[0];
    tpoint[1] = transformed[1];
    tpoint[2] = transformed[2];
    blah->InsertPoint(i, tpoint);
    }
  vtkPolyData *pointset = vtkPolyData::New();
  pointset->SetPoints(blah);
  this->Locator->SetDataSet(pointset);
  /*
  this->Locator->InitPointInsertion(vtkPoints::New(), blah->GetBounds());
  for (int i = 0; i < this->pointSet->GetNumberOfPoints(); i++)
    {
    double point[3];
    blah->GetPoint(i, point);
    this->Locator->InsertPoint(i, point);
    }
  */
#endif
  
#ifdef USE_KD_TREE
  for (int i = 0; i < this->pointSet->GetNumberOfPoints(); i++)
    {
    double point[3];
    this->pointSet->GetPoint(i, point);
    location_type coord = location_type(point[0], point[1], point[2]);
    location_type transformed = (*this->coordTransformation)(coord);
    kdtree_.insert( transformed, i );
    }
  kdtree_.build();
#endif
  }


PointSetNeighborhood::~PointSetNeighborhood()
  {
  delete this->covariance;
  }


void PointSetNeighborhood::maxSize( int s )
  {
  this->MaxNeighbors = std::min( s, this->pointSet->GetNumberOfPoints() );
  }


bool PointSetNeighborhood::selectProperty( const std::string& prop_name )
  {
  return (this->gridProperty != 0);
  }


void PointSetNeighborhood::findNeighbors(const GeoValue& center)
  {
  this->neighbors.clear();
  this->candidates.clear();
  if (!this->gridProperty)
    return;
  this->geoCenter = center;
  this->geoCenter.setPropertyArray( this->gridProperty );

#ifdef USE_LOCATOR
  location_type loc = (*this->coordTransformation)(center.location());
  vtkIdList *result = vtkIdList::New();
  vtkIdList *result2 = vtkIdList::New();
  this->Locator->FindClosestNPoints(this->MaxNeighbors, loc.x(), loc.y(), loc.z(), result);
  this->Locator->FindPointsWithinRadius(this->x, loc.x(), loc.y(), loc.z(), result2);

  if (result->GetNumberOfIds() < this->MinNeighbors)
    {
    return;
    }
  for (int i = 0; i < result->GetNumberOfIds(); i++)
    {
    int j = result->GetId(i);
    //check if was in radius, too
    if (this->gridProperty[j] != GeoValue::nullData && result2->IsId(j) > -1)
      {
      this->neighbors.push_back(GeoValue(this->pointSet,this->gridProperty,j));
		  }
    }
  result->Delete();
  result2->Delete();
#endif

#ifdef USE_KD_TREE
  location_type loc = (*this->coordTransformation)(center.location());
  int size = this->kdtree_.in_sphere(loc, this->x, this->candidates);
  for( int j=0; j < this->candidates.size() ; j++ ) 
    {
    //First check if that node is informed
    if (this->gridProperty[this->candidates[j]] != GeoValue::nullData)
      {
      this->neighbors.push_back(GeoValue(this->pointSet,this->gridProperty,this->candidates[j]));
		  }
	  }
  return;
  /*
  // If we found more than MaxNeighbors neighbors, only keep the 
  // MaxNeighbors closest (using the covariance distance).
  if(this->neighbors.size() > this->MaxNeighbors && this->covariance != 0) 
    {
    std::partial_sort(this->neighbors.begin(), this->neighbors.begin() + this->MaxNeighbors, 
      this->neighbors.end(), CovarianceDistance(loc, *this->covariance));
    this->neighbors.erase(this->neighbors.begin() + this->MaxNeighbors, this->neighbors.end());
    }
  */
#endif
  }

void PointSetNeighborhood::setNeighbors( const_iterator begin, const_iterator end )
  {
  this->neighbors.clear();
  const_iterator it = begin;

  for (int i = 0; it != end; ++it, ++i )
    {
    const vtkDataSet* datagrid = it->grid();
    if ( datagrid != this->pointSet )
      continue;
    this->neighbors.push_back( *it );
    this->neighbors[i].setPropertyArray( this->gridProperty );
    }
  }