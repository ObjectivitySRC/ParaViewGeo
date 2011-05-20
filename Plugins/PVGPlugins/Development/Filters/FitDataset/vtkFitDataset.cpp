//===================================================================================//
//				Mirarco Mining Innovation
// Author:   Nehme Bilal
// Date:     Augaust 2009
// contact: nehmebilal@gmail.com
//====================================================================================//

#include "vtkFitDataset.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"



#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkLine.h"

#include "../../../common/vtkCoordinateSystemMapper.h"
#include "vtkLeastSquareFit.h"
#include "vtkConvexHull.h"

#include<sstream>


#define tolerance 1

#define LINE 0
#define RECTANGLE 1
#define POLYGON 2
#define BOX 3
#define POLYHEDRON 4


vtkCxxRevisionMacro(vtkFitDataset, "$Revision: 1.34 $");
vtkStandardNewMacro(vtkFitDataset);

//--------------------------------------------------------------------------------------
vtkFitDataset::vtkFitDataset()
{
	this->Type = 0;
}
//--------------------------------------------------------------------------------------
vtkFitDataset:: ~vtkFitDataset(){}


//--------------------------------------------------------------------------------------
int vtkFitDataset::RequestData(vtkInformation *vtkNotUsed(request),
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	inPoints = input->GetPoints();
	numberOfPoints = inPoints->GetNumberOfPoints();

	if(numberOfPoints < 3)
	{
		return 0;
	}

	points = new double*[numberOfPoints];

	double p[3];
	for(int i=0; i<numberOfPoints; i++)
	{
		inPoints->GetPoint(i, p);
		points[i] = new double[3];
		points[i][0] = p[0];
		points[i][1] = p[1];
		points[i][2] = p[2];
	}

	if(this->Type == POLYGON)
	{
		double N[3];
		vtkLeastSquareFit::fitPlane3D(points, numberOfPoints, N, centroid);
		this->computeODRPolygon(N);
	}

	else if(this->Type == LINE)
	{
		double V[3];
		vtkLeastSquareFit::fitLine3D(points, numberOfPoints, V, centroid);
		this->computeODRLine(V);
	}

	else if(this->Type == RECTANGLE)
	{
		double N[3], V[3];
		vtkLeastSquareFit::fitPlane3D(points, numberOfPoints, N, centroid);
		vtkLeastSquareFit::fitLine3D(points, numberOfPoints, V, centroid, false);
		this->computeODRRectangle(N, V);
	}

	else if(this->Type == BOX)
	{
		double N[3], V[3];
		vtkLeastSquareFit::fitPlane3D(points, numberOfPoints, N, centroid);
		vtkLeastSquareFit::fitLine3D(points, numberOfPoints, V, centroid, false);
		this->computeODRBox(N, V);
	}

	//else if(this->Type == POLYHEDRON)
	//{
	//	//this->compute3dConvexHull();
	//}


	for(int i=0; i<numberOfPoints; i++)
	{
		delete[] points[i];
	}
	delete[] points;


	return 1;
}


//----------------------------------------------------------------------------
void vtkFitDataset::computeODRPolygon(double* N)
{
	vtkMath::Normalize(N);

	double** projectedPoints;
	projectedPoints = new double*[this->numberOfPoints];

	for(int i=0; i<numberOfPoints; i++)
	{
		projectedPoints[i] = new double[3];
		vtkPlane::ProjectPoint(points[i], centroid, N, projectedPoints[i]);
	}

	int i=0;
	while(projectedPoints[i][0] == centroid[0] &&
				projectedPoints[i][1] == centroid[1] &&
				projectedPoints[i][2] == centroid[2])
	{
		i++;
	}
	double xv[3];	// x axis of the plan reference
	xv[0] = projectedPoints[i][0] - centroid[0];
	xv[1] = projectedPoints[i][1] - centroid[1];
	xv[2] = projectedPoints[i][2] - centroid[2];

	vtkMath::Normalize(xv);

	double yv[3]; // y axis of the plan reference
	vtkMath::Cross(N, xv, yv);	// yv is already normalized because N and xv are units perpendicular vectors

	double xw[3] = {1.0, 0.0, 0.0};
	double yw[3] = {0.0, 1.0, 0.0};
	double zw[3] = {0.0, 0.0, 1.0};
	double cw[3] = {0.0, 0.0, 0.0};

	vtkCoordinateSystemMapper *cMapper = vtkCoordinateSystemMapper::New();
	cMapper->setCoordSystemA(cw, xw, yw, zw);
	cMapper->setCoordSystemB(centroid, xv, yv, N);
	cMapper->computeMappingMatrices();


	vtkPoints *outPoints = vtkPoints::New();
	vtkCellArray *outCells = vtkCellArray::New();
	outCells->InitTraversal();


	for(int i=0; i<numberOfPoints; i++)
	{
		cMapper->mapPointA_B(projectedPoints[i], projectedPoints[i]);
	}


	double** hullPoints = NULL;
	int n;
	vtkConvexHull::convexHull2D(projectedPoints, numberOfPoints, hullPoints, n);

	for(int i=0; i<numberOfPoints; i++)
	{
		delete[] projectedPoints[i];
	}

	delete[] projectedPoints;

	outCells->InsertNextCell(n);
	double p[3];

	for(int i=0; i<n; i++)
	{
		p[0] = hullPoints[i][0];
		p[1] = hullPoints[i][1];
		p[2] = 0.0;
		delete[] hullPoints[i]; // free memory

		cMapper->mapPointB_A(p, p);
		outPoints->InsertNextPoint(p[0], p[1], p[2]);
		outCells->InsertCellPoint(i);
	}

	delete[] hullPoints;

	this->output->SetPoints(outPoints);
	this->output->SetPolys(outCells);

	cMapper->Delete();
	outPoints->Delete();
	outCells->Delete();
}





//----------------------------------------------------------------------------
void vtkFitDataset::computeODRLine(double* N)
{
	vtkMath::Normalize(N);
	
	// compute a vector U perpendicular to N
	double U[3];
	if( fabs(N[2]) != 0.0 )
	{
		U[0] = 0.0;
		U[1] = 1.0;
		U[2] = - N[3]/N[2];
	}
	else if( fabs(N[1]) != 0.0 )
	{
		U[0] = 1.0;
		U[1] = -N[0]/N[1];
		U[2] = 0.0;
	}
	else
	{
		U[0] = 0.0;
		U[1] = 1.0;
		U[2] = 0.0;
	}

	vtkMath::Normalize(U);

	double V[3];
	vtkMath::Cross(N, U, V);


	double xw[3] = {1.0, 0.0, 0.0};
	double yw[3] = {0.0, 1.0, 0.0};
	double zw[3] = {0.0, 0.0, 1.0};
	double cw[3] = {0.0, 0.0, 0.0};

	double** projectedPoints;
	projectedPoints = new double*[this->numberOfPoints];

	vtkCoordinateSystemMapper *cMapper = vtkCoordinateSystemMapper::New();
	cMapper->setCoordSystemA(cw, xw, yw, zw);
	cMapper->setCoordSystemB(centroid,  N, U, V);
	cMapper->computeMappingMatrices();


	projectedPoints[0] = new double[3];

	// projection of a point onto the line
	double d[3];
	double alpha;

	d[0] = points[0][0] - this->centroid[0];
	d[1] = points[0][1] - this->centroid[1];
	d[2] = points[0][2] - this->centroid[2];
	
	alpha = vtkMath::Dot(d, N);
	projectedPoints[0][0] = alpha*N[0] + this->centroid[0];
	projectedPoints[0][1] = alpha*N[1] + this->centroid[1];
	projectedPoints[0][2] = alpha*N[2] + this->centroid[2];
	// end of projection
	
	cMapper->mapPointA_B(projectedPoints[0], projectedPoints[0]);

	double xmin = projectedPoints[0][0];
	double xmax = projectedPoints[0][0];

	for(int i=1; i<numberOfPoints; i++)
	{
		projectedPoints[i] = new double[3];
		
		// projection
		d[0] = points[i][0] - this->centroid[0];
		d[1] = points[i][1] - this->centroid[1];
		d[2] = points[i][2] - this->centroid[2];
		
		alpha = vtkMath::Dot(d, N);
		projectedPoints[i][0] = alpha*N[0] + this->centroid[0];
		projectedPoints[i][1] = alpha*N[1] + this->centroid[1];
		projectedPoints[i][2] = alpha*N[2] + this->centroid[2];
		// end of projection

		cMapper->mapPointA_B(projectedPoints[i], projectedPoints[i]);

		if(xmin > projectedPoints[i][0])
		{
			xmin = projectedPoints[i][0];
		}
		else if(xmax < projectedPoints[i][0])
		{
			xmax = projectedPoints[i][0];
		}
	}

	for(int i=0; i<numberOfPoints; i++)
	{
		delete[] projectedPoints[i];
	}

	delete[] projectedPoints;

	double p0[3] = {xmin, 0.0, 0.0};
	double p1[3] = {xmax, 0.0, 0.0};

	cMapper->mapPointB_A(p0, p0);
	cMapper->mapPointB_A(p1, p1);

	vtkPoints *outPoints = vtkPoints::New();
	vtkCellArray *outCells = vtkCellArray::New();
	outCells->InitTraversal();

	outPoints->InsertNextPoint(p0);
	outPoints->InsertNextPoint(p1);

	outCells->InsertNextCell(2);
	outCells->InsertCellPoint(0);
	outCells->InsertCellPoint(1);

	this->output->SetPoints(outPoints);
	this->output->SetLines(outCells);

	cMapper->Delete();
	outPoints->Delete();
	outCells->Delete();
}


//----------------------------------------------------------------------------
void vtkFitDataset::computeODRRectangle(double* N, double* V)
{
	vtkMath::Normalize(N);
	vtkMath::Normalize(V);
	double U[3];
	vtkMath::Cross(N, V, U);


	double xw[3] = {1.0, 0.0, 0.0};
	double yw[3] = {0.0, 1.0, 0.0};
	double zw[3] = {0.0, 0.0, 1.0};
	double cw[3] = {0.0, 0.0, 0.0};

	double** projectedPoints;
	projectedPoints = new double*[this->numberOfPoints];

	vtkCoordinateSystemMapper *cMapper = vtkCoordinateSystemMapper::New();
	cMapper->setCoordSystemA(cw, xw, yw, zw);
	cMapper->setCoordSystemB(centroid,  U, V, N);
	cMapper->computeMappingMatrices();


	projectedPoints[0] = new double[3];
	vtkPlane::ProjectPoint(points[0], centroid, N, projectedPoints[0]);
	cMapper->mapPointA_B(projectedPoints[0], projectedPoints[0]);

	double xmin = projectedPoints[0][0];
	double xmax = projectedPoints[0][0];
	double ymin = projectedPoints[0][1];
	double ymax = projectedPoints[0][1];



	for(int i=1; i<numberOfPoints; i++)
	{
		projectedPoints[i] = new double[3];
		vtkPlane::ProjectPoint(points[i], centroid, N, projectedPoints[i]);
		cMapper->mapPointA_B(projectedPoints[i], projectedPoints[i]);

		if(xmin > projectedPoints[i][0])
		{
			xmin = projectedPoints[i][0];
		}
		else if(xmax < projectedPoints[i][0])
		{
			xmax = projectedPoints[i][0];
		}
		if(ymin > projectedPoints[i][1])
		{
			ymin = projectedPoints[i][1];
		}
		else if(ymax < projectedPoints[i][1])
		{
			ymax = projectedPoints[i][1];
		}
	}

	for(int i=0; i<numberOfPoints; i++)
	{
		delete[] projectedPoints[i];
	}

	delete[] projectedPoints;

	double p0[3] = {xmin, ymin, 0.0};
	double p1[3] = {xmax, ymin, 0.0};
	double p2[3] = {xmax, ymax, 0.0};
	double p3[3] = {xmin, ymax, 0.0};

	cMapper->mapPointB_A(p0, p0);
	cMapper->mapPointB_A(p1, p1);
	cMapper->mapPointB_A(p2, p2);
	cMapper->mapPointB_A(p3, p3);

	vtkPoints *outPoints = vtkPoints::New();
	vtkCellArray *outCells = vtkCellArray::New();
	outCells->InitTraversal();

	outPoints->InsertNextPoint(p0);
	outPoints->InsertNextPoint(p1);
	outPoints->InsertNextPoint(p2);
	outPoints->InsertNextPoint(p3);

	outCells->InsertNextCell(4);
	outCells->InsertCellPoint(0);
	outCells->InsertCellPoint(1);
	outCells->InsertCellPoint(2);
	outCells->InsertCellPoint(3);

	this->output->SetPoints(outPoints);
	this->output->SetPolys(outCells);

	cMapper->Delete();
	outPoints->Delete();
	outCells->Delete();
}




//----------------------------------------------------------------------------
void vtkFitDataset::computeODRBox(double* N, double* V)
{
	vtkMath::Normalize(N);
	vtkMath::Normalize(V);
	double U[3];
	vtkMath::Cross(N, V, U);


	double xw[3] = {1.0, 0.0, 0.0};
	double yw[3] = {0.0, 1.0, 0.0};
	double zw[3] = {0.0, 0.0, 1.0};
	double cw[3] = {0.0, 0.0, 0.0};

	double** mappedPoints;
	mappedPoints = new double*[this->numberOfPoints];

	vtkCoordinateSystemMapper *cMapper = vtkCoordinateSystemMapper::New();
	cMapper->setCoordSystemA(cw, xw, yw, zw);
	cMapper->setCoordSystemB(centroid, U, V, N);
	cMapper->computeMappingMatrices();

	mappedPoints[0] = new double[3];
	cMapper->mapPointA_B(points[0], mappedPoints[0]);

	double xmin = mappedPoints[0][0];
	double xmax = mappedPoints[0][0];
	double ymin = mappedPoints[0][1];
	double ymax = mappedPoints[0][1];
	double zmin = mappedPoints[0][2];
	double zmax = mappedPoints[0][2];



	for(int i=1; i<numberOfPoints; i++)
	{
		mappedPoints[i] = new double[3];
		cMapper->mapPointA_B(points[i], mappedPoints[i]);

		if(xmin > mappedPoints[i][0])
		{
			xmin = mappedPoints[i][0];
		}
		else if(xmax < mappedPoints[i][0])
		{
			xmax = mappedPoints[i][0];
		}
		if(ymin > mappedPoints[i][1])
		{
			ymin = mappedPoints[i][1];
		}
		else if(ymax < mappedPoints[i][1])
		{
			ymax = mappedPoints[i][1];
		}
		if(zmin > mappedPoints[i][2])
		{
			zmin = mappedPoints[i][2];
		}
		else if(zmax < mappedPoints[i][2])
		{
			zmax = mappedPoints[i][2];
		}
	}

	for(int i=0; i<numberOfPoints; i++)
	{
		delete[] mappedPoints[i];
	}

	delete[] mappedPoints;

	double p0[3] = {xmin, ymin, zmin};
	double p1[3] = {xmax, ymin, zmin};
	double p2[3] = {xmax, ymax, zmin};
	double p3[3] = {xmin, ymax, zmin};

	double p4[3] = {xmin, ymin, zmax};
	double p5[3] = {xmax, ymin, zmax};
	double p6[3] = {xmax, ymax, zmax};
	double p7[3] = {xmin, ymax, zmax};

	cMapper->mapPointB_A(p0, p0);
	cMapper->mapPointB_A(p1, p1);
	cMapper->mapPointB_A(p2, p2);
	cMapper->mapPointB_A(p3, p3);
	cMapper->mapPointB_A(p4, p4);
	cMapper->mapPointB_A(p5, p5);
	cMapper->mapPointB_A(p6, p6);
	cMapper->mapPointB_A(p7, p7);

	vtkPoints *outPoints = vtkPoints::New();
	vtkCellArray *outCells = vtkCellArray::New();
	outCells->InitTraversal();

	outPoints->InsertNextPoint(p0);
	outPoints->InsertNextPoint(p1);
	outPoints->InsertNextPoint(p2);
	outPoints->InsertNextPoint(p3);

	outPoints->InsertNextPoint(p4);
	outPoints->InsertNextPoint(p5);
	outPoints->InsertNextPoint(p6);
	outPoints->InsertNextPoint(p7);

	outCells->InsertNextCell(4);
	outCells->InsertCellPoint(0);
	outCells->InsertCellPoint(1);
	outCells->InsertCellPoint(2);
	outCells->InsertCellPoint(3);

	outCells->InsertNextCell(4);
	outCells->InsertCellPoint(1);
	outCells->InsertCellPoint(5);
	outCells->InsertCellPoint(6);
	outCells->InsertCellPoint(2);

	outCells->InsertNextCell(4);
	outCells->InsertCellPoint(3);
	outCells->InsertCellPoint(2);
	outCells->InsertCellPoint(6);
	outCells->InsertCellPoint(7);

	outCells->InsertNextCell(4);
	outCells->InsertCellPoint(0);
	outCells->InsertCellPoint(4);
	outCells->InsertCellPoint(5);
	outCells->InsertCellPoint(1);

	outCells->InsertNextCell(4);
	outCells->InsertCellPoint(5);
	outCells->InsertCellPoint(4);
	outCells->InsertCellPoint(7);
	outCells->InsertCellPoint(6);

	outCells->InsertNextCell(4);
	outCells->InsertCellPoint(0);
	outCells->InsertCellPoint(3);
	outCells->InsertCellPoint(7);
	outCells->InsertCellPoint(4);

	this->output->SetPoints(outPoints);
	this->output->SetPolys(outCells);

	cMapper->Delete();
	outPoints->Delete();
	outCells->Delete();
}



//----------------------------------------------------------------------------
void vtkFitDataset::compute3dConvexHull()
{
	vtkPoints *outPoints = vtkPoints::New();
	vtkCellArray *outCells = vtkCellArray::New();
	vtkConvexHull::incrementalConvexHull3D(this->points, this->numberOfPoints,
		outPoints, outCells);
	
	this->output->SetPoints(outPoints);
	this->output->SetPolys(outCells);

	outPoints->Delete();
	outCells->Delete();
}

//----------------------------------------------------------------------------
int vtkFitDataset::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}





