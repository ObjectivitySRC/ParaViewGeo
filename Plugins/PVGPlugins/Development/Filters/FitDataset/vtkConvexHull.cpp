#include "vtkConvexHull.h"
#include "Inc3dCHull_ConvexHull.h"

#include <vtkstd/deque>
#include <vtkstd/vector>
#include <vtkstd/list>
#include <vtkstd/algorithm>
#include "vtkMath.h"
#include "vtkLine.h"

#include "vtkPoints.h"
#include "vtkCellArray.h"


// all function using vtkstd are implemented into an anonymous namespace.
// this will make the access to this global function internal only

namespace
{
	//---------------------Graham's scan 2d convex hull functions:



	//
	// Building the hull consists of two procedures: building the lower and
	// then the upper hull. The two procedures are nearly identical - the main
	// difference between the two is the test for convexity. When building the upper
	// hull, our rule is that the middle point must always be *above* the line formed
	// by its two closest neighbors. When building the lower hull, the rule is that point
	// must be *below* its two closest neighbors. We pass this information to the
	// building routine as the last parameter, which is either -1 or 1.
	//



	// translate the points so that p1 is at the origin,
	// then takes the cross product of p0 and p2. The result will be positive,
	// negative, or 0, meaning respectively that p2 has turned right, left, or
	// is on a straight line.
	int direction( vtkstd::pair<double,double> p0,
												vtkstd::pair<double,double> p1,
												vtkstd::pair<double,double> p2 )
	{
			double res =  ( (p0.first - p1.first ) * (p2.second - p1.second ) )
						 - ( (p2.first - p1.first ) * (p0.second - p1.second ) );

			if(res > 0.0)
				return 1;
			else if(res < 0.0)
				return -1;
			else
			return 0;
	}


	// This is the method that builds either the upper or the lower half convex
	// hull. It takes as its input a copy of the input array, which will be the
	// sorted list of points in one of the two halfs. It produces as output a list
	// of the points in the corresponding convex hull.
	//
	// The factor should be 1 for the lower hull, and -1 for the upper hull.

	void build_half_hull( vtkstd::deque< vtkstd::pair<double,double> > &input,
												vtkstd::deque< vtkstd::pair<double,double> > &output,
												vtkstd::pair<double,double> left,
												vtkstd::pair<double,double> right,
												int factor )
	{
		//
		// The hull will always start with the left point, and end with the right
		// point. According, we start by adding the left point as the first point
		// in the output sequence, and make sure the right point is the last point
		// in the input sequence.
		//
		input.push_back( right );
		output.push_back( left );
		//
		// The construction loop runs until the input is exhausted
		//
		while ( input.size() != 0 ) {
				//
				// Repeatedly add the leftmost point to the null, then test to see
				// if a convexity violation has occured. If it has, fix things up
				// by removing the next-to-last point in the output suqeence until
				// convexity is restored.
				//
				output.push_back( input.front() );
				input.pop_front();

				while ( output.size() >= 3 ) {
						size_t end = output.size() - 1;
						if ( factor * direction( output[ end - 2 ],
																		 output[ end ],
																		 output[ end - 1 ] ) <= 0 ) {
								output.erase( output.begin() + end - 1 );
						}
						else
								break;
				}
		}
	}



	void build_hull(vtkstd::deque<vtkstd::pair<double,double> > &raw_points,
									vtkstd::deque<vtkstd::pair<double,double> > &output)
	{
		if(raw_points.size()<=4)
		{
			output = raw_points;
			return;
		}
		output.clear();
	//
	// The initial array of points is stored in deque raw_points. I first
	// sort it, which gives me the far left and far right points of the hull.
	// These are special values, and they are stored off separately in the left
	// and right members.
	//
	// I then go through the list of raw_points, and one by one determine whether
	// each point is above or below the line formed by the right and left points.
	// If it is above, the point is moved into the upper_partition_points sequence. If it
	// is below, the point is moved into the lower_partition_points sequence. So the output
	// of this routine is the left and right points, and the sorted points that are in the
	// upper and lower partitions.
	//

		vtkstd::pair<double,double> left;
		vtkstd::pair<double,double> right;
		vtkstd::deque< vtkstd::pair<double,double> > upper_partition_points;
		vtkstd::deque< vtkstd::pair<double,double> > lower_partition_points;


		//
		// Step one in partitioning the points is to sort the raw data
		//
		vtkstd::sort( raw_points.begin(), raw_points.end() );
		//
		// The the far left and far right points, remove them from the
		// sorted sequence and store them in special members
		//
		left = raw_points.front();
		raw_points.pop_front();

		right = raw_points.back();
		raw_points.pop_back();
		//
		// Now put the remaining points in one of the two output sequences
		//
		for ( size_t i = 0 ; i < raw_points.size() ; i++ )
		{
				int dir = direction( left, right, raw_points[ i ] );
				if ( dir < 0 )
						upper_partition_points.push_back( raw_points[ i ] );
				else
						lower_partition_points.push_back( raw_points[ i ] );
		}

		vtkstd::deque< vtkstd::pair<double,double> > upper_hull;

		build_half_hull(lower_partition_points, output, left, right, 1 );
		build_half_hull(upper_partition_points, upper_hull, left, right, -1 );

		for(vtkstd::deque< vtkstd::pair<double,double> >::reverse_iterator it = upper_hull.rbegin()+1;
			it != upper_hull.rend()-1; it++)
		{
			output.push_back(*it);
		}
	}

	
	//------------- Incremental 2d convex hull ------------------------------

	//----------------------------------------------------------------------
	class Cpoint2D
	{
	public:

		Cpoint2D()
		{
			//_tuple[0] = 0.0;
			//_tuple[1] = 0.0;
		}

		//-------------------------------------
		Cpoint2D(double x, double y)
		{
			_tuple[0] = x;
			_tuple[1] = y;
		}

		//-------------------------------------
		Cpoint2D(const Cpoint2D& otherPoint)
		{
			_tuple[0] = otherPoint._tuple[0];
			_tuple[1] = otherPoint._tuple[1];
		}

		//-------------------------------------
		inline virtual double* tuple()
		{
			return _tuple;
		}

		//-------------------------------------
    inline Cpoint2D& operator = (const Cpoint2D& otherPoint) 
		{
			_tuple[0] = otherPoint._tuple[0];
			_tuple[1] = otherPoint._tuple[1];
			return *this;
		}

		//-------------------------------------
		bool operator< (const Cpoint2D& p) const
		{
			if(_tuple[0] < p.X() ||
			((_tuple[0] - p.X() < VTK_DBL_EPSILON) && _tuple[1] < p.Y()) )
				return true;

			return false;
		}

		//-------------------------------------
    inline double operator[] (int i) const
		{
			return _tuple[i];
		}

		//-------------------------------------
    inline double& operator[] (int i)
		{
			return _tuple[i];
		}

		//-------------------------------------
    inline double X () const
		{
			return _tuple[0];
		}

		//-------------------------------------
    inline double Y () const
		{
			return _tuple[1];
		}

		//-------------------------------------
    inline void setX (double x) 
		{
			_tuple[0] = x;
		}

		//-------------------------------------
    inline void setY (double y) 
		{
			_tuple[1] = y;
		}

		//-------------------------------------
    inline void setCoord (double x, double y) 
		{
			_tuple[0] = x;
			_tuple[1] = y;
		}

	private:
		double _tuple[2];
	};


	
	//----------------------------------------------------------------------
	class IncrementalHull2D
	{
	public:

		enum
    {
    LEFT=0,
    RIGHT,
		COLLINEAR_FRONT,
		COLLINEAR_BACK,
		COLLINEAR_BETWEEN,
		POINT,
		LINEAR,
		PLANAR
    };

		
		//----------------------------------------------------------------------
		IncrementalHull2D()
		{
		}

		//----------------------------------------------------------------------
		void getHull(vtkstd::deque<Cpoint2D>& inPoints, vtkstd::deque<Cpoint2D>& hull)
		{
			vtkstd::sort(inPoints.begin(), inPoints.end());
			type = POINT;
			hull.push_back(inPoints.front());

			for(vtkstd::deque<Cpoint2D>::iterator it = inPoints.begin()+1;
				it != inPoints.end(); ++it)
			{
				switch(type)
				{
				case POINT:
					type = LINEAR;
					hull.push_back(*it);
					break;
				case LINEAR:
					this->mergeLinear(*it, hull);
					break;
				case PLANAR:
					this->mergePlanar(*it, hull);
				}
			}
		}

		//----------------------------------------------------------------------
		// this function is used to find the position of a point p relatively
		// to the segment formed by p1 and p2.
		// return:
		// LEFT : The point is not on the line but on the left of p1p2
		// RIGHT : The point is not on the line but on the right of p1p2
		// COLLINEAR_FRONT : The point is on the line left to p1
		// COLLINEAR_BACK : The point is on the line right to p2
		// COLLINEAR_CONTAIN : The point is on the line between p1 and p2
		int collinearTest(const Cpoint2D& p, const Cpoint2D& p1, const Cpoint2D& p2)
		{
			double v[2] = {p2[0]-p1[0], p2[1]-p1[1]}; // direction of p1p2
			double a[2] = {p[0]-p1[0], p[1]-p1[1]}; // direction of p1p
			double n[2] = {-v[1], v[0]}; // perpendicular to v
			double nda = vtkMath::Dot2D(n,a);
			double ndn = vtkMath::Dot2D(n,n);
			double ada = vtkMath::Dot2D(a,a);

			if(nda * nda > VTK_DBL_EPSILON * ndn * ada)
			{
				if(nda > 0)
					return LEFT;
				if(nda < 0)
					return RIGHT;
			}

			double vda = vtkMath::Dot2D(v,a);
			if(vda < -VTK_DBL_EPSILON * ndn)
				return COLLINEAR_BACK;
			if(vda > (1 + VTK_DBL_EPSILON) * ndn)
				return COLLINEAR_FRONT;

			return COLLINEAR_BETWEEN;
		}

		//----------------------------------------------------------------------
		void mergeLinear(const Cpoint2D& p, vtkstd::deque<Cpoint2D>& hull)
		{
			Cpoint2D *pt;
			switch(collinearTest(p, hull[0], hull[1]))
			{
			case LEFT:
				type = PLANAR;
				hull.push_front(p);
				break;
			case RIGHT:
				type = PLANAR;
				pt = new Cpoint2D(hull[0]);
				hull[0] = hull[1];
				hull[1] = *pt;
				hull.push_front(p);
				delete pt;
				break;
			case COLLINEAR_BACK:
				hull[0] = p;
				break;
			case COLLINEAR_FRONT:
				hull[1] = p;
			}
		}


		//----------------------------------------------------------------------
		void mergePlanar(const Cpoint2D& p, vtkstd::deque<Cpoint2D>& hull)
		{
			int U=0, i=1, test;
			int n = hull.size();

			// find the upper tangent point
			for(int counter=0; counter<n; ++counter)
			{
				test = collinearTest(p, hull[U], hull[i]);
				
				if(test == RIGHT)
				{
					U = i;
					i = (i+1)%n;
					continue;
				}

				if(test == LEFT || test == COLLINEAR_BACK)
				{
					break; // upper tangent point found
				}

				return;
			}

			int L = 0;

			// find the lower tangent point
			for(i = n-1; i>=0; --i)
			{
				test = collinearTest(p, hull[i], hull[L]);
				
				
				if(test == RIGHT)
				{
					L = i;
					continue;
				}

				if(test == LEFT || test == COLLINEAR_FRONT)
				{
					break; // upper tangent point found
				}

				return;
			}

			
			vtkstd::deque<Cpoint2D> tempHull;
			tempHull.push_back(p);
			while(1)
			{
				tempHull.push_back(hull[U]);
				if(U == L)
					break;
				
				U = (U+1)%n;
			}

			hull = tempHull;
		}


	private:

		int type;

	};








}





//----------------------------------------------------------------------------
void vtkConvexHull::convexHull2D(double** input, const int ni,
																 double** &output, int &no)
{
	vtkstd::deque < vtkstd::pair< double,double > > inPoints;
	vtkstd::deque < vtkstd::pair< double,double > > outPoints;


	for(int i=0; i<ni; i++)
	{
		inPoints.push_back(vtkstd::pair<double,double>(input[i][0], input[i][1]));
	}

	build_hull(inPoints, outPoints);
	no = outPoints.size();
	output = new double*[no];

	int i=0;
	for(vtkstd::deque<vtkstd::pair<double,double> >::iterator it = outPoints.begin();
		it != outPoints.end(); it++)
	{
		output[i] = new double[2];
		output[i][0] = it->first;
		output[i][1] = it->second;
		i++;
	}
}




//----------------------------------------------------------------------------
void vtkConvexHull::incrementalConvexHull2D(double** input, const int ni,
																						double** &output, int& no)
{
	vtkstd::deque < Cpoint2D > inPoints;
	vtkstd::deque < Cpoint2D > outPoints;

	Cpoint2D p;

	for(int i=0; i<ni; i++)
	{
		p.setCoord(input[i][0], input[i][1]);
		inPoints.push_back(p);
	}

	IncrementalHull2D hull;
	hull.getHull(inPoints, outPoints);
	no = outPoints.size();
	output = new double*[no];

	int i=0;
	for(vtkstd::deque<Cpoint2D >::iterator it = outPoints.begin();
		it != outPoints.end(); it++)
	{
		output[i] = new double[2];
		output[i][0] = it->X();
		output[i][1] = it->Y();
		i++;
	}
}


//----------------------------------------------------------------------------
void vtkConvexHull::incrementalConvexHull3D(double** input, const int ni,
							vtkPoints* outPoints, vtkCellArray* outTriangles)
{
	Inc3dCHull::ConvexHull hull(input, ni);

	vtkstd::list< vtkstd::pair<int,double*> > points;
	vtkstd::list<int*> triangles;

	hull.getHull(points, triangles);

	outPoints->SetNumberOfPoints(points.size());
	for(vtkstd::list< vtkstd::pair<int,double*> >::iterator it = points.begin();
		it != points.end(); ++it)
	{
		outPoints->SetPoint(it->first, it->second);
		delete[] it->second;
	}

	for(vtkstd::list<int*>::iterator it = triangles.begin();
		it != triangles.end(); ++it)
	{
		outTriangles->InsertNextCell(3, *it);
		delete[] *it;
	}

}