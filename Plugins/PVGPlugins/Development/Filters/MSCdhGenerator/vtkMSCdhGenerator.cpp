#include "vtkMSCdhGenerator.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"

#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkLine.h"

#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkPointData.h"
#include "vtkPointLocator.h"
#include "vtkCylinder.h"
#include "vtkExtractGeometry.h"
#include "vtkImplicitBoolean.h"
#include "vtkPlanes.h"
//#include "vtkCoordinateSystemMapper.h"
#include "vtkDoubleArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkGeneralTransform.h"
#include "vtkTransform.h"

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>

using namespace std;

vtkCxxRevisionMacro(vtkMSCdhGenerator, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkMSCdhGenerator);

//--------------------------------------------------------------------------------------
vtkMSCdhGenerator::vtkMSCdhGenerator()
{
	this->SetNumberOfInputPorts(2);
	this->CollarFile = NULL;
	this->OutputFile = NULL;
	this->BlocksMineralValue = NULL;
	this->UseEllipsoid = 0;

  //metric tensor stuff (the ability to measure ellipsoidal distances)
  this->MetricTensor = new double[6]; 
  this->MetricTensor[0]=1.0;
  this->MetricTensor[1]=0.0;
  this->MetricTensor[2]=0.0;
  this->MetricTensor[3]=1.0;
  this->MetricTensor[4]=0.0;
  this->MetricTensor[5]=1.0;

	this->constraints.push_back(&vtkMSCdhGenerator::depthConstraint);
}
//--------------------------------------------------------------------------------------
vtkMSCdhGenerator:: ~vtkMSCdhGenerator(){}


// python programmable source grid:
/*
pdo =  self.GetPolyDataOutput()
points = vtk.vtkPoints()
verts = vtk.vtkCellArray()

counter = 0
for x in range(0,1000,100):
  for y in range(0,1000,100):
    for z in range(0,1000,100):
      points.InsertNextPoint(x,y,z)
      verts.InsertNextCell(1)
      verts.InsertCellPoint(counter)
      counter += 1

pdo.SetPoints(points)
pdo.SetVerts(verts)
*/

//----------------------------------------------------------------------------
// SetSourceConnection
//----------------------------------------------------------------------------
void vtkMSCdhGenerator::SetSourceConnection( vtkAlgorithmOutput *algOutput )
	{
	this->SetInputConnection( 1, algOutput );
	}

//----------------------------------------------------------------------------
// Fill Input Port Information (Source and Input here)
//----------------------------------------------------------------------------
int vtkMSCdhGenerator::FillInputPortInformation ( int , vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 1 );
  info->Set ( vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  return 1;
  }


//--------------------------------------------------------------------------------------
int vtkMSCdhGenerator::RequestData(vtkInformation *vtkNotUsed(request),
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	if(this->DLengthStep < this->SegmentLength)
	{
		vtkErrorMacro("the length step must be greater than segment length");
		return 0;
	}

	if(this->DLengthMin > this->DLengthMax)
	{
		vtkErrorMacro("length min must be smaller or equal to length max");
		return 0;
	}

	if(this->DAzimuthMin > this->DAzimuthMax)
	{
		vtkErrorMacro("azimuth min must be smaller or equal to azimuth max");
		return 0;
	}

	if(this->DDipMin > this->DDipMax )
	{
		vtkErrorMacro("dip min must be smaller or equal to dip max");
		return 0;
	}
	//TODO: parameters consistency verification

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  output = vtkPolyData::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	// Get the input (pointset) and optionally the source (collar points)
  this->InputGrid = vtkPolyData::SafeDownCast (
		inInfo->Get(vtkDataObject::DATA_OBJECT()));
  if (!this->InputGrid)
    {
    vtkErrorMacro(<<"The Input Grid (Input) is null.");
    return 0;
    }
  if (sourceInfo)
  {
		this->InputCollarPoints = vtkPolyData::SafeDownCast (
			sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
  }
  else
  {
		this->InputCollarPoints = NULL;
	}

	this->blocksMineralValueArray = 
		this->InputGrid->GetPointData()->GetArray(this->BlocksMineralValue);

	if(this->blocksMineralValueArray == NULL)
	{
		vtkErrorMacro("this filter require a block mineral value point property to work");
		return 0;
	}

	if(this->ComputeRanges == 1)
		this->computeBlockModelCenter();

  //initialize the metric tensor for ellipsoid distances
  this->SetMetric();

  //initialize the point locator based on the input grid
  //this->Locator = vtkPointLocator::New();
  //this->Locator->SetDataSet(this->InputGrid);


	// if the neighbors has already been computed, there is no need to 
	// compute them again. I think this is safe unless the input of the 
	// filter is modified !!
	//if(!this->neighbors.size())
		//this->InitialNeighbors();

	vector<double*> collarPoints;
	if(!this->ReadCollarPoints(collarPoints))
	{
		vtkErrorMacro("Coudn't read collar points file");
		return 0;
	}

	this->numberOfCollarPoints = collarPoints.size();
	if(this->numberOfCollarPoints == 0)
	{
		vtkErrorMacro("The number of collar points cannot be 0");
		return 0;
	}


	this->outputFile.open(this->OutputFile);
	if(!this->outputFile)
	{
		vtkErrorMacro("coudn't open the output file");
		return 0;
	}
	this->outputFile << "Drill Moving Cost = " << this->DrillMovingCost << endl;
	this->outputFile << "Number Of Blocks = " << this->InputGrid->GetNumberOfPoints() << endl;
	this->outputFile << "Number Of Groups = " << this->numberOfCollarPoints << endl;

	this->outPoints = vtkPoints::New();
	this->outLines = vtkCellArray::New();


	this->generateDrillHoles(collarPoints);
	this->outputFile.close();

	this->output->SetPoints(this->outPoints);
	this->output->SetLines(this->outLines);

	this->outPoints->Delete();
	this->outLines->Delete();


	//this->Locator->Delete();
	return 1;
}


//--------------------------------------------------------------------------------------
void vtkMSCdhGenerator::generateDrillHoles(vector<double*>& collarPoints)
{
	time_t start,end;
	time (&start);



	this->currentCollarPointID = 0;
	for(vector<double*>::iterator it = collarPoints.begin();
		it != collarPoints.end(); ++it)
	{
		if(this->ComputeRanges == 1)
			this->generateCollarPointDrillholes2(*it);
		else
			this->generateCollarPointDrillholes(*it);
		delete[] (*it);
		++this->currentCollarPointID;
	}

	//Final time
	time (&end);
	//Calculate difference
	double dif = difftime (end,start);
	int time = (int)dif;
	int min = dif/60;
	int sec = time%60;

	vtkWarningMacro("The run time is "<<min<<":"<<sec<<" (min:sec)");
}

//--------------------------------------------------------------------------------------
void vtkMSCdhGenerator::generateCollarPointDrillholes(const double collarPoint[3])
{
	double rads = vtkMath::RadiansFromDegrees( 1. );

	double azimuth = this->DAzimuthMin;
	while(azimuth <= this->DAzimuthMax)
	{
		double azr = azimuth * rads;

		double dip = this->DDipMin;
		while(dip <= this->DDipMax)
		{	
			double dipr = dip * rads;
			double temp = fabs(this->DLengthStep * cos(dipr));
			double dX = temp * cos(azr);
			double dY = temp * sin(azr);
			double dZ = this->DLengthStep * sin(dipr);
			
			temp = fabs(this->DLengthMin * cos(dipr));
			double sX = (temp * cos(azr)) + collarPoint[0];
			double sY = (temp * sin(azr)) + collarPoint[1];
			double sZ = (this->DLengthMin * sin(dipr)) + collarPoint[2];

			set<int> currentElements;
			double length = this->DLengthMin;
			int lCounter = 0;
			while(length <= this->DLengthMax)
			{
				double endPoint[3];
				endPoint[0] = sX + lCounter*dX;
				endPoint[1] = sY + lCounter*dY;
				endPoint[2] = sZ + lCounter*dZ;

				// if the drillhole doesn't meet the depth constraints,
				// discard it.
				//if(endPoint[2] <=this->DDepthMax && endPoint[2]>=this->DDepthMin )
				//{
				//	this->generateDrillHole(currentElements, collarPoint, endPoint, 
				//		azr, dipr, length);
				//}

				// the following will iterate over all constraints and check if they are
				// all satisfied. If one of the constraints is not satisfied, the 
				// drillhole will be discarded.
				bool constraintsSatisfied = true;
				for(vector<constraintMF>::iterator it = this->constraints.begin();
					it != this->constraints.end(); ++it)
				{
					constraintMF constraint = (*it);
					if(! (this->*constraint)(collarPoint, endPoint) )
					{
						constraintsSatisfied = false;
						break;
					}
				}

				if(constraintsSatisfied)
					this->generateDrillHole(currentElements, collarPoint, endPoint, 
						azr, dipr, length);		
				
				length += this->DLengthStep;
				++lCounter;
			}
			dip += this->DDipStep;
		}
		azimuth += this->DAzimuthStep;
	}

}

//--------------------------------------------------------------------------------------
void vtkMSCdhGenerator::generateCollarPointDrillholes2(const double collarPoint[3])
{
	vtkPoints* inPoints = this->InputGrid->GetPoints();
	double endPoint[3];

	// get azimuth, dip and length of the drillholes joining 
	// the collar point and the center of the block model
	// we call this drillholes here reference drillhole
	this->getAzimuthDipLength(collarPoint, this->center, 
														&this->rAz, &this->rDip, &this->rLength);

	double rads = vtkMath::RadiansFromDegrees( 1. );

	double azimuth = this->DAzimuthMin;
	while(azimuth <= this->DAzimuthMax)
	{
		double azr = (azimuth + this->rAz) * rads;

		// consider that the reference drillholes is of length 1
		// and compute it's dx,dy,dz
		double dipr = this->rDip * rads;
		double temp = fabs(cos(dipr)); // consider length = 1
		double dX = temp * cos(azr);
		double dY = temp * sin(azr);
		double dZ = sin(dipr); // consider length = 1

		endPoint[0] = collarPoint[0] + dX;
		endPoint[1] = collarPoint[1] + dY;
		endPoint[2] = collarPoint[2] + dZ;

		// here we want to find the equation of a plan containing the 
		// reference vector and parallel to z
		// we first find to vectors in this plan, we cross this vectors
		// to compute the normal of the plan. Since the collar point is 
		// also part of the plan we can compute the equation of this plan.
		// another thing we want to do is to find all the point in the block
		// model that are neighbors to this plan(distance < r).
		// to do so we compute the equation of 2 planes bounding this plane
		// and all the points in between these 2 planes will be the neighbors.
		double vect1[] = {dX, dY, dZ-1.0};
		double vect2[] = {dX, dY, dZ};
		double n1[3];
		vtkMath::Cross(vect1, vect2, n1);
		vtkMath::Normalize(n1);
		double n2[] = {-n1[0], -n1[1], -n1[2]};

		// first plane
		double d1 = n1[0]*collarPoint[0] +
							  n1[1]*collarPoint[1] + 
							  n1[2]*collarPoint[2] - this->EMajor;

		// second plane
		double d2 = n2[0]*collarPoint[0] +
							  n2[1]*collarPoint[1] + 
							  n2[2]*collarPoint[2] - this->EMajor;

		double pts[3];

		// every time we find a neighbor of the plane, we project
		// this point on the plane and we compute it's dip angle.
		// we keep track of the minimum and maximum dip found !
		double dipMinr = VTK_DOUBLE_MAX;
		double dipMaxr = VTK_DOUBLE_MIN;
		for(int i=0; i < inPoints->GetNumberOfPoints(); ++i)
		{
			inPoints->GetPoint(i, pts);
			double a1 = n1[0]*pts[0] +
							    n1[1]*pts[1] + 
							    n1[2]*pts[2];

			double a2 = n2[0]*pts[0] +
							    n2[1]*pts[1] + 
							    n2[2]*pts[2];

			if(a1 >= d1 && a2 >= d2)
			{
				double p[3];
				vtkPlane::ProjectPoint(pts, endPoint, n1, p);
				double dx = p[0] - collarPoint[0];
				double dy = p[1] - collarPoint[1];
				double dz = p[2] - collarPoint[2];

				double l = sqrt(dx*dx + dy*dy + dz*dz);
				if(l < 0.001)
					continue;

				//to find dipr, we divide dZ by length, then take arcsin
				double dipr = asin(dz / l); //let's hope length != 0

				if(dipr < dipMinr)
					dipMinr = dipr;
				if(dipr > dipMaxr)
					dipMaxr = dipr;
			}
		}

		double dip = dipMinr*vtkMath::DegreesFromRadians(1.0);
		double dipMax = dipMaxr*vtkMath::DegreesFromRadians(1.);
		while(dip <= dipMax)
		{	
			double dipr = dip * rads;
			double temp = fabs(this->DLengthStep * cos(dipr));
			double dX = temp * cos(azr);
			double dY = temp * sin(azr);
			double dZ = this->DLengthStep * sin(dipr);
			
			temp = fabs(this->DLengthMin * cos(dipr));
			double sX = (temp * cos(azr)) + collarPoint[0];
			double sY = (temp * sin(azr)) + collarPoint[1];
			double sZ = (this->DLengthMin * sin(dipr)) + collarPoint[2];

			set<int> currentElements;
			double length = this->DLengthMin;
			int lCounter = 0;
			while(length <= this->DLengthMax)
			{
				double endPoint[3];
				endPoint[0] = sX + lCounter*dX;
				endPoint[1] = sY + lCounter*dY;
				endPoint[2] = sZ + lCounter*dZ;

				// if the drillhole doesn't meet the depth constraints,
				// discard it.
				//if(endPoint[2] <=this->DDepthMax && endPoint[2]>=this->DDepthMin )
				//{
				//	this->generateDrillHole(currentElements, collarPoint, endPoint, 
				//		azr, dipr, length);
				//}

				bool constraintsSatisfied = true;
				for(vector<constraintMF>::iterator it = this->constraints.begin();
					it != this->constraints.end(); ++it)
				{
					constraintMF constraint = (*it);
					if(! (this->*constraint)(collarPoint, endPoint) )
					{
						constraintsSatisfied = false;
						break;
					}
				}

				if(constraintsSatisfied)
					this->generateDrillHole(currentElements, collarPoint, endPoint, 
						azr, dipr, length);				

				length += this->DLengthStep;
				++lCounter;
			}
			dip += this->DDipStep;
		}
		azimuth += this->DAzimuthStep;
	}
}

//--------------------------------------------------------------------------------------
bool vtkMSCdhGenerator::depthConstraint(const double startPt[3], const double endPt[3]) 
{
	 return (endPt[2] <= this->DDepthMax && endPt[2] >= this->DDepthMin);
}

//--------------------------------------------------------------------------------------
void vtkMSCdhGenerator::computeBlockModelCenter()
{
	vtkPoints* inPoints = this->InputGrid->GetPoints();
	int n = inPoints->GetNumberOfPoints();

	this->center[0] = 0.0;
	this->center[1] = 0.0;
	this->center[2] = 0.0;

	double pts[3];
	for(int i=0; i<n; ++i)
	{
		inPoints->GetPoint(i, pts);

		// we divide over n right now to avoid getting
		// very big number or maybe infinite number
		this->center[0] += pts[0]/n;
		this->center[1] += pts[1]/n;
		this->center[2] += pts[2]/n;
	}
}



//--------------------------------------------------------------------------------------
void vtkMSCdhGenerator::generateDrillHole(std::set<int>& currentElements,
										const double collarPoint[3],
										const double endPoint[3],
										double azr, 
										double dipr, 
										double length)
{
	double startPoint[3];
	if(length == this->DLengthMin)
	{
		startPoint[0] = collarPoint[0];
		startPoint[1] = collarPoint[1];
		startPoint[2] = collarPoint[2];
	}
	else
	{
		double len = length - this->DLengthStep;
		double temp = fabs(len * cos(dipr));
		double dX = temp * cos(azr);
		double dY = temp * sin(azr);
		double dZ = len * sin(dipr);

		startPoint[0] = collarPoint[0] + dX;
		startPoint[1] = collarPoint[1] + dY;
		startPoint[2] = collarPoint[2] + dZ;
	}

	//double N[3], U[3], V[3];
	//N[0] = endPoint[0] - collarPoint[0];
	//N[1] = endPoint[1] - collarPoint[1];
	//N[2] = endPoint[2] - collarPoint[2];
	//this->getReference(N, U, V);

	//vtkUnstructuredGrid* uGrid = this->cylindricalClip(startPoint, endPoint, N, U, V, 
	//	this->DRadius, azr, dipr);

	//vtkPoints* clippedPoints = uGrid->GetPoints();

	int n = currentElements.size();

	this->addDrillholeNeighbors(currentElements, startPoint, endPoint, 
		this->InputGrid->GetPoints(), azr, dipr);

	if(currentElements.size() > n )
	{
		this->writeDrillholeToFile(currentElements, collarPoint, endPoint);

		int id1 = this->outPoints->InsertNextPoint(collarPoint);
		int id2 = this->outPoints->InsertNextPoint(endPoint);
		this->outLines->InsertNextCell(2);
		this->outLines->InsertCellPoint(id1);
		this->outLines->InsertCellPoint(id2);
	}
}


//--------------------------------------------------------------------------------------
void vtkMSCdhGenerator::addDrillholeNeighbors(std::set<int>& currentElements,
												const double startPoint[3], 
												const double endPoint[3],
												vtkPoints* points,
												double azr,
												double dipr)
{
	double temp = fabs(this->SegmentLength * cos(dipr));
	double dX = temp * cos(azr);
	double dY = temp * sin(azr);
	double dZ = this->SegmentLength * sin(dipr);

	double len = 0.0;
	double lenMax = (endPoint[0]-startPoint[0])*(endPoint[0]-startPoint[0]) + 
									(endPoint[1]-startPoint[1])*(endPoint[1]-startPoint[1]) + 
									(endPoint[2]-startPoint[2])*(endPoint[2]-startPoint[2]);
	lenMax = sqrt(lenMax);

	int counter = 0;
	double pt[3];
	double dPoint[3];
	dPoint[0] = startPoint[0];
	dPoint[1] = startPoint[1];
	dPoint[2] = startPoint[2];

	if(this->UseEllipsoid == 0)
	{
		for(int i=0; i<points->GetNumberOfPoints(); ++i)
		{
			points->GetPoint(i, pt);
			double closestPoint[3];
			double t;
			double p1[] = {startPoint[0], startPoint[1], startPoint[2]}; 
			double p2[] = {endPoint[0], endPoint[1], endPoint[2]};
			double d = sqrt(vtkLine::DistanceToLine(pt, p1, p2, t, closestPoint));
			if(d <= this->DRadius)
			{
				currentElements.insert(i);
			}
		}
	}
	else
	{
		while(len < lenMax)
		{
			for(int i=0; i < points->GetNumberOfPoints(); ++i)
			{
				points->GetPoint(i, pt);
				if(
					( pt[0] <= (dPoint[0]+this->EMajor) && pt[0] >= (dPoint[0]-this->EMajor) ) &&
					( pt[1] <= (dPoint[1]+this->EMajor) && pt[1] >= (dPoint[1]-this->EMajor) ) &&
					( pt[2] <= (dPoint[2]+this->EMajor) && pt[2] >= (dPoint[2]-this->EMajor) ) 
					)
				{
					if(this->EvaluateDist2(dPoint, pt) <= 1.0)
					{
						currentElements.insert(i);
					}
				}
			}
			dPoint[0] += dX;
			dPoint[1] += dY;
			dPoint[2] += dZ;
			len += this->SegmentLength;
		}
	}
}

//--------------------------------------------------------------------------------------
void vtkMSCdhGenerator::writeDrillholeToFile(const std::set<int>& currentElements,
																						 const double collarPoint[3],
																						 const double endPoint[3])
{
	double length = sqrt( ((endPoint[0]-collarPoint[0])*(endPoint[0]-collarPoint[0]))
						+ ((endPoint[1]-collarPoint[1])*(endPoint[1]-collarPoint[1]))
						+ ((endPoint[2]-collarPoint[2])*(endPoint[2]-collarPoint[2])) );

	this->outputFile << "s " << collarPoint[0] << " " 
													 << collarPoint[1] << " "
													 << collarPoint[2] << " "
													 << endPoint[0] << " "
													 << endPoint[1] << " "
													 << endPoint[2] << " "
													 << length * this->CostPerMeter << " "
													 << this->currentCollarPointID;

	for(set<int>::const_iterator it = currentElements.begin(); 
		it != currentElements.end(); ++it)
	{
		this->outputFile << " " << *it << "|" 
			<< this->blocksMineralValueArray->GetComponent(*it,0);
	}
	this->outputFile << endl;
}

//--------------------------------------------------------------------------------------
void vtkMSCdhGenerator::getReference(double N[3], double U[3], double V[3])
{
	vtkMath::Normalize(N);
	if( fabs(N[2]) > 0.1 )
	{
		U[0] = 0.0;
		U[1] = 1.0;
		U[2] = - N[1]/N[2];
	}
	else if( fabs(N[1]) > 0.1 )
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

	vtkMath::Cross(N, U, V);
}

//--------------------------------------------------------------------------------------
vtkUnstructuredGrid* vtkMSCdhGenerator::cylindricalClip(const double startPoint[3], 
																											  const double endPoint[3],
																												const double X[3],
																												const double Y[3],
																												const double Z[3],
																											  double radius, 
																												double azR,
																											  double dipR)
{
	//vtkCylinder *cyl = vtkCylinder::New(); 
	//cyl->SetCenter(startPoint[0],startPoint[1],startPoint[2]); 
	//cyl->SetRadius(radius); 

	////TODO:
	//// the cylindre is currently in y direction.
	//// add the necessary transformation to the cylinder.

	//vtkTransform* tr = vtkTransform::New();
	//tr->Translate(-startPoint[0], -startPoint[1], -startPoint[2]);
	//tr->RotateX(dipR);
	//tr->RotateZ(azR -90);
	//tr->Translate(startPoint[0], startPoint[1], startPoint[2]);
	//cyl->SetTransform(tr);

	//double xw[3] = {1.0, 0.0, 0.0};
	//double yw[3] = {0.0, 1.0, 0.0};
	//double zw[3] = {0.0, 0.0, 1.0};
	//double cw[3] = {0.0, 0.0, 0.0};

	//vtkCoordinateSystemMapper *cMapper = vtkCoordinateSystemMapper::New();
	//cMapper->setCoordSystemB(cw, xw, yw, zw);
	//cMapper->setCoordSystemA(startPoint,  X, Y, Z);
	//cMapper->computeMappingMatrices();

	//vtkPoints* clippingPlanesPoints = vtkPoints::New();
	//vtkDoubleArray* clippingPlanesNormals = vtkDoubleArray::New();
	//clippingPlanesNormals->SetNumberOfComponents(3);

	//clippingPlanesPoints->InsertNextPoint(endPoint);
	//clippingPlanesNormals->InsertNextTuple(X);

	//double v[] = {-X[0], -X[1], -X[2]};
	//clippingPlanesPoints->InsertNextPoint(startPoint);
	//clippingPlanesNormals->InsertNextTuple(v);	

	//double pt[3];
	//pt[0] = 0.0;
	//pt[1] = radius;
	//pt[2] = 0.0;
	//double transPt[3];
	//cMapper->mapPointA_B(pt, transPt);
	//clippingPlanesPoints->InsertNextPoint(transPt);
	//clippingPlanesNormals->InsertNextTuple(Y);

	//pt[1] = -radius;	
	//cMapper->mapPointA_B(pt, transPt);
	//
	//v[0] = -Y[0];
	//v[1] = -Y[1];
	//v[2] = -Y[2];
	//clippingPlanesPoints->InsertNextPoint(transPt);
	//clippingPlanesNormals->InsertNextTuple(v);

	////pt[0] = 0.0;
	//pt[1] = 0.0;
	//pt[2] = radius;	
	//cMapper->mapPointA_B(pt, transPt);
	//clippingPlanesPoints->InsertNextPoint(transPt);
	//clippingPlanesNormals->InsertNextTuple(Z);

	//v[0] = -Z[0];
	//v[1] = -Z[1];
	//v[2] = -Z[2];
	//pt[2] = -radius;	
	//cMapper->mapPointA_B(pt, transPt);
	//clippingPlanesPoints->InsertNextPoint(transPt);
	//clippingPlanesNormals->InsertNextTuple(v);

	//vtkPlanes* clippingPlanes = vtkPlanes::New();
	//clippingPlanes->SetPoints(clippingPlanesPoints);
	//clippingPlanes->SetNormals(clippingPlanesNormals);

	//vtkImplicitBoolean* implicitBool = vtkImplicitBoolean::New();
	//implicitBool->SetOperationTypeToIntersection();
	//implicitBool->AddFunction(cyl);
	//implicitBool->AddFunction(clippingPlanes);

	//vtkExtractGeometry *clipped = vtkExtractGeometry::New(); 
	//clipped->SetInput(this->InputGrid); 
	//clipped->SetImplicitFunction(implicitBool); 
	//clipped->ExtractInsideOn(); 
	//clipped->ExtractBoundaryCellsOn();   
	//clipped->Update();

	//vtkUnstructuredGrid* res = vtkUnstructuredGrid::New();
	//res->DeepCopy(clipped->GetOutput());

	//cyl->Delete();
	//cMapper->Delete();
	//clippingPlanesPoints->Delete();
	//clippingPlanesNormals->Delete();
	//clippingPlanes->Delete();
	//implicitBool->Delete();
	//clipped->Delete();

	//return res;

	return 0;
}


//----------------------------------------------------------------------------
// GetAzimuthDipLength - gets the drillhole's azimuth, dip, length, from
//                       the start and end-point of the drillhole
//----------------------------------------------------------------------------
void vtkMSCdhGenerator::getAzimuthDipLength(const double *start_point, 
																						const double *end_point, 
																						double *az, 
																						double *dip, 
																						double *length)
  {
  //get length
  *length = 0;
  for (int i = 0; i < 3; ++i)
    {
    *length += pow((end_point[i] - start_point[i]),2);
    }
  *length = sqrt(*length);

  //get dx, dy, dz
  double dX = end_point[0] - start_point[0];
  double dZ = end_point[2] - start_point[2];

  //to find dipr, we divide dZ by length, then take arcsin
  double dipr = asin(dZ / *length); //let's hope length != 0

  //to find azr, we divide dX by temp, then take the arccos
  double temp = abs(*length * cos(dipr));
  double azr = acos(dX / temp);

  //convert to degrees, check sign
  double degs = vtkMath::DegreesFromRadians( 1. );
  *az = azr * degs;
  if (end_point[1] < start_point[1])
    {
    *az = -(*az);
    }
  *dip = dipr * degs;
  }


//----------------------------------------------------------------------------
// ReadCollarPoints - Retrieves collar points from either a csv file or the
// filter's input.
//----------------------------------------------------------------------------
bool vtkMSCdhGenerator::ReadCollarPoints(
			std::vector<double*>& collarPoints)
{
  collarPoints.clear();
  if (this->CollarLocation == 0) //CollarLocation == 0 is csv file
    {
    std::ifstream file;
    file.open(this->CollarFile);
    if (!file || file.fail())
      {
      vtkErrorMacro("Could not open collar point file.\n");
      return false;
      }
    std::string line;
    while(getline(file, line))
      {
      std::stringstream ss(line);
      std::string item;
      std::vector<std::string> v;
      while (std::getline(ss, item, ','))
        {
        v.push_back(item);
        }
      if (!v.empty())
        {
        double* dv = new double[3];
        dv[0] = (atof(v[0].c_str()));
        dv[1] = (atof(v[1].c_str()));
        dv[2] = (atof(v[2].c_str()));
        collarPoints.push_back(dv);
        }
      }
    }
  else if(this->CollarLocation == 1) // CollarLocation == 1 is the Filter's Source
    {
    if (!this->InputCollarPoints)
		  {
		  vtkErrorMacro(<<"No Collar Points specified in the Filter Source");
		  return false;
		  }
    int npts = this->InputCollarPoints->GetNumberOfPoints();
    if (npts == 0)
      {
      vtkErrorMacro(<<"No Collar Points found in the Filter Source");
      return false;
      }
    for (int i = 0; i < npts; ++i)
      {
      double* xyz = new double[3];
      this->InputCollarPoints->GetPoint(i, xyz);
      //vector<double> dv(&xyz[0], &xyz[3]);
      collarPoints.push_back(xyz);
      }
    }
  
  return true;
}

//----------------------------------------------------------------------------
// SetMetric
//----------------------------------------------------------------------------
void vtkMSCdhGenerator::SetMetric()
{
  /*
    Marie-Gabrielle is the one you should talk to if you have trouble with this
  */
  //convert angles to radians
  double *angleRads = new double[3];
  double factor = vtkMath::DoublePi() / 180.0;
	angleRads[0] = this->EAzimuth * factor;
  angleRads[1] = this->EDip * factor;
	angleRads[2] = this->ERake * factor;

  //make rotation matrix
  double *rot = new double[16];
  this->MakeRotMatrix( angleRads[1], angleRads[2], angleRads[0], rot);
  vtkMatrix4x4 *rotMatrix = vtkMatrix4x4::New();
  rotMatrix->DeepCopy(rot);

  //make transpose of rotation matrix
  vtkMatrix4x4 *inv = vtkMatrix4x4::New();
  vtkMatrix4x4::Transpose(rotMatrix, inv);

  //make scale matrix
  vtkMatrix4x4 *sca = vtkMatrix4x4::New();
  sca->Identity();
  vtkTransform *transform = vtkTransform::New();
  transform->SetMatrix(sca);
  transform->Scale(1.0 / (this->EMajor * this->EMajor), 
                   1.0 / (this->EMedium * this->EMedium), 
                   1.0 / (this->EMinor * this->EMinor));
  vtkMatrix4x4 *scaleMatrix = transform->GetMatrix();

  //make the matrix for tensor
  vtkMatrix4x4 *temp = vtkMatrix4x4::New();
  vtkMatrix4x4 *for_tensor = vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(rotMatrix, scaleMatrix, temp);
  vtkMatrix4x4::Multiply4x4(temp, inv, for_tensor);


  this->MetricTensor[0] = for_tensor->GetElement(0,0);
  this->MetricTensor[1] = for_tensor->GetElement(0,1);
  this->MetricTensor[2] = for_tensor->GetElement(0,2);
  this->MetricTensor[3] = for_tensor->GetElement(1,1);
  this->MetricTensor[4] = for_tensor->GetElement(1,2);
  this->MetricTensor[5] = for_tensor->GetElement(2,2);

  //cleanup
  delete [] angleRads;
  delete [] rot;
  rotMatrix->Delete();
  inv->Delete();
  sca->Delete();
  transform->Delete();
  //scaleMatrix->Delete();//this was a reference from 'transform', so dont del
  temp->Delete();
  for_tensor->Delete();

}

//----------------------------------------------------------------------------
// MakeRotMatrix
//  the rotation moves the xyz axis onto the ellipsoid axis 
//----------------------------------------------------------------------------
void vtkMSCdhGenerator::MakeRotMatrix(double ax, double ay, double az, double *m)
{
  /*
    Marie-Gabrielle is the one you should talk to if you have trouble with this
  */
  // Assume az = azimuth, ay = rake, ax = dip, as defined in SGeMS
  //  - rotation around Z, angle pi/2, to initialize as SGeMS do,
  //  - First rotation in SGeMS  around Z', angle alpha = -azimuth
  //  - Second rotation in SGeMS around X', angle beta  = -dip
  //  - Third rotation in SGeMS  around Y', angle theta = plunge
  //
  // Maxima commands
  // Rx(a) := matrix([1,0,0],[0,cos(a),-sin(a)],[0,sin(a),cos(a)]);
  // Ry(b) := matrix([cos(b), 0, -sin(b)],[0,1,0],[sin(b),0,cos(b)]);
  // Rz(a) := matrix([cos(a), -sin(a),0],[sin(a),cos(a),0],[0,0,1]);
  // Ry(ay).Rx(-ax).Rz(-az).Rz(%pi/2);
  double cx,sx,cy,sy,cz,sz,cycz,cysz;
  cx = cos(ax);
  cy = cos(ay);
  cz = cos(az);
  sx = sin(ax);
  sy = sin(ay);
  sz = sin(az);
  cycz = cy*cz;
  cysz = cy*sz;
#ifdef COLUMN_ORDER
  m[0]  = sx*sy*cz + cysz;  
  m[4]  = cx*cz;
  m[8]  = sy*sz - sx*cycz;
  m[12] = 0.0;
  m[1]  = sx*sy*sz - cycz;
  m[5]  = cx*sz;
  m[9]  = - sy*cz - sx*cysz;
  m[13] = 0.0;
  m[2]  = - cx*sy;
  m[6]  = sx;
  m[10] = cx*cy;
  m[14] = 0.0;
  m[3]  = 0.0;
  m[7]  = 0.0;
  m[11] = 0.0;
  m[15] = 1.0;
#else
  m[0]  = sx*sy*cz + cysz; 
  m[1]  = cx*cz;	   
  m[2]  = sy*sz - sx*cycz; 
  m[3]  = 0.0;		   
  m[4]  = sx*sy*sz - cycz; 
  m[5]  = cx*sz;	   
  m[6]  = - sy*cz - sx*cysz;
  m[7]  = 0.0;		   
  m[8]  = - cx*sy;	   
  m[9]  = sx;		   
  m[10] = cx*cy;	   
  m[11] = 0.0;		   
  m[12] = 0.0;		   
  m[13] = 0.0;		   
  m[14] = 0.0;		   
  m[15] = 1.0;		   
#endif
}

//----------------------------------------------------------------------------
// EvaluateDist2 - gives you a squared ellipsoidal distance between points
//                 less than 1 means inside the ellipsoid
//----------------------------------------------------------------------------
double vtkMSCdhGenerator::EvaluateDist2( double* P, double* Q)
  {
  /*
    Marie-Gabrielle is the one you should talk to if you have trouble with this
  */
  double dx = Q[0] - P[0];
  double dy = Q[1] - P[1];
  double dz = Q[2] - P[2];
  double* met = this->MetricTensor;

  double ret = met[0]*dx*dx + met[3]*dy*dy + met[5]*dz*dz 
    + 2.* ( met[1]*dx*dy + met[2]*dx*dz + met[4]*dy*dz );

  return ret;
  }


//----------------------------------------------------------------------------
// InitialNeighbors - calculates the neighbors of the pointset to itself
//----------------------------------------------------------------------------
void vtkMSCdhGenerator::InitialNeighbors()
{
  //for each point in the input grid, find its neighbours
  int npts = this->InputGrid->GetNumberOfPoints();
  for (int i = 0; i < npts; ++i)
    {
    int row = i;
    for (int j = 0; j < npts; ++j)
      {
      int col = j;
      double p[3];
      double q[3];
      this->InputGrid->GetPoint(i, p);
      this->InputGrid->GetPoint(j, q);
      if (this->EvaluateDist2(p,q) <= 1)
        {
        //the point is inside the ellipsoid, so mark it as a neighbour
        this->SetNeighbor(row, col, 1);
        }
      else
        {
        //the point is not inside the ellipsoid, so mark it as not neighbor
		//nbilal: calling this method in this case doesn't do anything at all
			// so why call it ?
        this->SetNeighbor(row, col, 0);
        }
      }
    }
}

//----------------------------------------------------------------------------
// SetNeighbor - sets whether a point is a neighbor of another point
//----------------------------------------------------------------------------
void vtkMSCdhGenerator::SetNeighbor(int row, int col, int is_neighbor)
  {
  //You're probably wondering why if it's a vector of neighbours that
  //I bother with "is_neighbor" at all. It's because in the past the
  //data structure for neighbors was not a map of vectors, and this way
  //you can define a new way of storing neighbours that may be more
  //efficient which knows what isn't a neighbour
  if (is_neighbor)
    {
    this->neighbors[row].push_back(col);
    }
  }

//----------------------------------------------------------------------------
// IsNeighbor - tells you whether a point is a neighbor of another point
//----------------------------------------------------------------------------
bool vtkMSCdhGenerator::IsNeighbor(int row, int col)
  {
  std::vector<int> row_ = this->neighbors[row];
  return std::find(row_.begin(), row_.end(), col) != row_.end();
  }







