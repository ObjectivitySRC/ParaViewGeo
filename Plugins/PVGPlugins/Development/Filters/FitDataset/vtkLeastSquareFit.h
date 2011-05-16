//===================================================================================//
//				Mirarco Mining Innovation
// Author:   Nehme Bilal
// Date:     october 2009
// contact: nehmebilal@gmail.com 
//====================================================================================//

#ifndef VTK_LEAST_SQUARE_FIT_H
#define VTK_LEAST_SQUARE_FIT_H

class vtkLeastSquareFit
{
public:

	vtkLeastSquareFit(){}
	~vtkLeastSquareFit(){}

	// Fit n points with a line which have a direction v and contains a point p.
	// p is also the centroid of the points. If P you already computed the centroid
	// outside this function, set computeCentroid to False and then the function 
	// will use the passed centroid instead of recomputing it
	// the orthogonal least square distance is used
	static void fitLine3D(double** points, int n, double V[3], double p[3],
		bool computeCentroid = true);

	// Fit n points with a plane which have a normal N and contains a point p.
	// p is also the centroid of the points. If P you already computed the centroid
	// outside this function, set computeCentroid to False and then the function 
	// will use the passed centroid instead of recomputing it
	// the orthogonal least square distance is used
	static void fitPlane3D(double** points, int n, double N[3], double p[3],
		bool computeCentroid = true);
};






#endif