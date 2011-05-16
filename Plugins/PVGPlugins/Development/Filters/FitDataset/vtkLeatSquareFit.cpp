#include "vtkLeastSquareFit.h"
#include "vtkMath.h"

//--------------------------------------------------------------------------------------
void vtkLeastSquareFit::fitLine3D(double** points, int n, double V[3], double p[3],
																	bool computeCentroid /*=true*/)
{
	double Sxx = 0.0, 
				 Sxy = 0.0,
				 Sxz = 0.0,
				 Syy = 0.0,
				 Syz = 0.0,
				 Szz = 0.0;
	double dx, dy, dz;
	
	if(computeCentroid)
		vtkMath::Centroid(points, n, p);

	for(int i=0; i<n; i++)
	{
		dx = points[i][0] - p[0];
		dy = points[i][1] - p[1];
		dz = points[i][2] - p[2];

		Sxx += dx*dx;
		Sxy += dx*dy;
		Sxz += dx*dz;
		Syy += dy*dy;
		Syz += dy*dz;
		Szz += dz*dz;
	}

	double gamma = Sxx + Syy + Szz;

	double M[3][3];

	M[0][0] = gamma - Sxx;
	M[0][1] = -Sxy;
	M[0][2] = -Sxz;
	M[1][0] = -Sxy;
	M[1][1] = gamma - Syy;
	M[1][2] = -Syz;
	M[2][0] = -Sxz;
	M[2][1] = -Syz;
	M[2][2] = gamma-Szz;

	double eigValues[3];
	double eigVectors[3][3];

	vtkMath::Diagonalize3x3(M, eigValues, eigVectors);

	int posMin = 0;
	double eigMin = eigValues[0];

	if(eigValues[1] < eigMin)
	{
		eigMin = eigValues[1];
		posMin = 1;
	}
	if(eigValues[2] < eigMin)
	{
		posMin = 2;
	}


	V[0] = eigVectors[0][posMin];
	V[1] = eigVectors[1][posMin];
	V[2] = eigVectors[2][posMin];
	

}

//--------------------------------------------------------------------------------------
void vtkLeastSquareFit::fitPlane3D(double** points, int n, double N[3], double p[3],
																	 bool computeCentroid/*=true*/)
{
	double Sxx = 0.0, 
				 Sxy = 0.0,
				 Sxz = 0.0,
				 Syy = 0.0,
				 Syz = 0.0,
				 Szz = 0.0;
	double dx, dy, dz;
	
	if(computeCentroid)
		vtkMath::Centroid(points, n, p);

	for(int i=0; i<n; i++)
	{
		dx = points[i][0] - p[0];
		dy = points[i][1] - p[1];
		dz = points[i][2] - p[2];

		Sxx += dx*dx;
		Sxy += dx*dy;
		Sxz += dx*dz;
		Syy += dy*dy;
		Syz += dy*dz;
		Szz += dz*dz;
	}

	double gamma = Sxx + Syy + Szz;

	double M[3][3];

	M[0][0] = Sxx;
	M[0][1] = Sxy;
	M[0][2] = Sxz;
	M[1][0] = Sxy;
	M[1][1] = Syy;
	M[1][2] = Syz;
	M[2][0] = Sxz;
	M[2][1] = Syz;
	M[2][2] = Szz;

	double eigValues[3];
	double eigVectors[3][3];

	vtkMath::Diagonalize3x3(M, eigValues, eigVectors);

	int posMin = 0;
	double eigMin = eigValues[0];

	if(eigValues[1] < eigMin)
	{
		eigMin = eigValues[1];
		posMin = 1;
	}
	if(eigValues[2] < eigMin)
	{
		posMin = 2;
	}


	N[0] = eigVectors[0][posMin];
	N[1] = eigVectors[1][posMin];
	N[2] = eigVectors[2][posMin];
}
