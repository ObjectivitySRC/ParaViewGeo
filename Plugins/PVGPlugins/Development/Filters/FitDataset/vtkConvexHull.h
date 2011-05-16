#ifndef VTK_CONVEX_HULL_H
#define VTK_CONVEX_HULL_H


class vtkPoints;
class vtkCellArray;

class vtkConvexHull
{

public:

	// build the convex hull of a 2D point-set passed in input parameter,
	// and add the results into the output parameter.
	// n1 = number of input points (in)
	// n2 = number of output points (out)
	// the output parameter shouldn't be pre-allocated and must be intialized
	// to NULL, the memory allocation will be done internally.
	// the output points will be sorted in counter-clockwise direction from
	// left to right
	static void convexHull2D(double** input, const int ni, double** &output, int &no);

	static void incrementalConvexHull2D(double** input, const int ni, double** &output, int &no);

	static void incrementalConvexHull3D(double** input, const int ni,
		 vtkPoints* outPoints, vtkCellArray* outTriangles);

	int CollinearTest();


	enum CollinearFlag{LEFT=0, RIGHT, COLINEAR_LEFT, COLINEAR_RIGHT, COLLINEAR_CONTAIN};

};




#endif