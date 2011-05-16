/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkDelaunay2D.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMDelaunay2D.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkDelaunay2D.h"

#include "vtkAbstractTransform.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTriangle.h"
#include "vtkTransform.h"
#include "vtkDataSetSurfaceFilter.h"

#define VTK_DELAUNAY_XY_PLANE 0
#define VTK_SET_TRANSFORM_PLANE 1
#define VTK_BEST_FITTING_PLANE 2
//#define VTK_DELAUNAY_YX_PLANE 3
#define VTK_DELAUNAY_YZ_PLANE 3
#define VTK_DELAUNAY_ZX_PLANE 4
//#define VTK_DELAUNAY_ZY_PLANE 6

vtkCxxRevisionMacro(vtkMDelaunay2D, "$Revision: 1.00 $");
vtkStandardNewMacro(vtkMDelaunay2D);
vtkCxxSetObjectMacro(vtkMDelaunay2D,Transform,vtkAbstractTransform);

// Construct object with Alpha = 0.0; Tolerance = 0.00001; Offset = 1.25;
// BoundingTriangulation turned off.
vtkMDelaunay2D::vtkMDelaunay2D()
{
}

vtkMDelaunay2D::~vtkMDelaunay2D()
{
}

//----------------------------------------------------------------------------
// Specify the input data or filter. Old style.
void vtkMDelaunay2D::SetSource(vtkPolyData *input)
{
  this->Superclass::SetInput(1, input);
}

//----------------------------------------------------------------------------
// Specify the input data or filter. New style.
void vtkMDelaunay2D::SetSourceConnection(vtkAlgorithmOutput *algOutput)
{
  this->Superclass::SetInputConnection(1, algOutput);
}

vtkPolyData *vtkMDelaunay2D::GetSource()
{
  if (this->GetNumberOfInputConnections(1) < 1)
    {
    return NULL;
    }
  return vtkPolyData::SafeDownCast(
    this->GetExecutive()->GetInputData(1, 0));
}

/*
FindAxisAvg will return the average distance between the points
on the given axis
*/
double vtkMDelaunay2D::FindAxisAvg(vtkPolyData *input, int axis)
{
	// The XYZ data from the points
	vtkDataArray *ptData = input->GetPoints()->GetData();
	// Store first value, so that it can be used later
	double lastVal = ( ptData->GetTuple3(0) )[axis];

	double totalVal = 0.0;
	double currentVal;
	int numOfTuples = ptData->GetNumberOfTuples();
	for(int currentPoint = 1; currentPoint < numOfTuples; currentPoint++)
		{
		// Get the value for the next point on given axis
		currentVal = ( ptData->GetTuple3(currentPoint) )[axis];
		// Calculate the distance between them and add it to the total
		totalVal += abs(currentVal - lastVal);
		// Save value for next iteration
		lastVal = currentVal;
		// Just incase abs() is not cross-platform
		//x = x < 0? -x : x;
		}

	// Return the average
	return (totalVal/numOfTuples);
}

/*
This method is used to translate the points from XZ or YZ into XY, which
is what Delaunay2D needs to draw a surface. After Delaunay2D has drawn
the surface, the points are translated again, back into their original
positions.

The method takes the input data, a Delaunay2D object, and three integers.

The integers are used to determine which values should be swapped.
Example: If you want to swap the XYZ to YZX, you would call
TranslatePoints(input, del2D, 1, 2, 0)
*/
void vtkMDelaunay2D::TranslatePoints(vtkPolyData *input, vtkDelaunay2D *del2D,
																		 int swapX, int swapY, int swapZ)
{
	// The XYZ data from the points
	vtkDataArray *ptData = input->GetPoints()->GetData();
	double x, y, z;

	double* currentTuple;
	int numOfTuples = ptData->GetNumberOfTuples();
	for(int currentPoint = 0; currentPoint < numOfTuples; currentPoint++) 
		{
		// Store the XYZ values in local variables, so they can be swapped
		currentTuple = ptData->GetTuple3(currentPoint);
		// Read the coordinates, and preform the swap at the same time
		x = currentTuple[swapX];
		y = currentTuple[swapY];
		z = currentTuple[swapZ];

		// Swap the Y and Z coordinates
		ptData->SetTuple3(currentPoint, x, y, z);
		}
	// Let Delaunay do the dirty work again
	del2D->SetInput(input);
	del2D->Update();

	// Grab the point data again, but this time from the Delaunay object
	vtkDataArray *d2DOut = del2D->GetOutput()->GetPoints()->GetData();
	for(int currentPoint = 0; currentPoint < numOfTuples; currentPoint++) 
		{
		currentTuple = d2DOut->GetTuple3(currentPoint);
		// Now the points have to be put back in their original positions
		// This group of if else will determine where the X Y and Z are
		// and place them back in the correct positions
		// Find X
		if(swapX == 0)
		{
			x = currentTuple[0];
		}
		else if (swapY == 0)
		{
			x = currentTuple[1];
		}
		else if(swapZ == 0)
		{
			x = currentTuple[2];
		}

		// Find Y
		if(swapX == 1)
		{
			y = currentTuple[0];
		}
		else if (swapY == 1)
		{
			y = currentTuple[1];
		}
		else if(swapZ == 1)
		{
			y = currentTuple[2];
		}

		// Find Z
		if(swapX == 2)
		{
			z = currentTuple[0];
		}
		else if (swapY == 2)
		{
			z = currentTuple[1];
		}
		else if(swapZ == 2)
		{
			z = currentTuple[2];
		}

		// Swap the X, Y and Z coordinates back into their original positions
		d2DOut->SetTuple3(currentPoint, x, y, z);
		}
}

int vtkMDelaunay2D::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  //vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  //vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkDataSetSurfaceFilter *surface = vtkDataSetSurfaceFilter::New();
	surface->SetInput(this->GetInput());
	surface->Update();

	// Delaunay 2D object that will do all the "real" work for us
	vtkDelaunay2D *del2D = vtkDelaunay2D::New();
	del2D->SetProjectionPlaneMode(0);
	del2D->SetAlpha(this->Alpha);
	del2D->SetTolerance(this->Tolerance);
	del2D->SetOffset(this->Offset);
	del2D->SetBoundingTriangulation(this->BoundingTriangulation);

	if(this->GetProjectionPlaneMode() == VTK_DELAUNAY_XY_PLANE)
		{ // If it's on the XY plane, let Delaunay do the work
		del2D->SetInput(surface->GetOutput());
		del2D->Update();
		}
	if(this->GetProjectionPlaneMode() == VTK_DELAUNAY_ZX_PLANE)
		{ // Points are on the ZX plane

		// TranslatePoints(input, Delaunay2D, XYZ index (0-2), XYZ index (0-2), XYZ index (0-2))
		this->TranslatePoints(surface->GetOutput(), del2D, 0, 2, 1);
		}
	if(this->GetProjectionPlaneMode() == VTK_DELAUNAY_YZ_PLANE)
		{ // Points are on the YZ plane
		
		// TranslatePoints(input, Delaunay2D, XYZ index (0-2), XYZ index (0-2), XYZ index (0-2))
		this->TranslatePoints(surface->GetOutput(), del2D, 1, 2, 0);
		}
	if(this->GetProjectionPlaneMode() == VTK_BEST_FITTING_PLANE)
		{
		// Find the average distance between the points for each axis
		// The axis with the smallest average will be ignored
		double xAvg = this->FindAxisAvg(surface->GetOutput(), 0);
		double yAvg = this->FindAxisAvg(surface->GetOutput(), 1);
		double zAvg = this->FindAxisAvg(surface->GetOutput(), 2);
		// Check XY
		if(xAvg > zAvg && yAvg > zAvg)
			{
			del2D->SetInput(surface->GetOutput());
			del2D->Update();
			}
		// Check XZ
		if(xAvg > yAvg && zAvg > yAvg)
			{
			this->TranslatePoints(surface->GetOutput(), del2D, 0, 2, 1);
			}
		// Check YZ
		if(yAvg > xAvg && zAvg > xAvg)
			{
			this->TranslatePoints(surface->GetOutput(), del2D, 1, 2, 0);
			}
		}

	// Show our new surface in Paraview
  output->ShallowCopy(del2D->GetOutput());
	return 1;
}

int vtkMDelaunay2D::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
    }
  else if (port == 1)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    }
  return 1;
}

void vtkMDelaunay2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  /*os << indent << "Alpha: " << this->Alpha << "\n";
  os << indent << "ProjectionPlaneMode: " 
     << ((this->ProjectionPlaneMode == VTK_BEST_FITTING_PLANE)? "Best Fitting Plane" : "XY Plane") << "\n";
  os << indent << "Transform: " << (this->Transform ? "specified" : "none") << "\n";
  os << indent << "Tolerance: " << this->Tolerance << "\n";
  os << indent << "Offset: " << this->Offset << "\n";
  os << indent << "Bounding Triangulation: "
     << (this->BoundingTriangulation ? "On\n" : "Off\n");*/
}
