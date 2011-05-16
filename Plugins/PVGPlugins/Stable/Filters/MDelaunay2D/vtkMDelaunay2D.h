/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkDelaunay2D.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMDelaunay2D - create 2D Delaunay triangulation of input points
// .SECTION Description
 
// .SECTION See Also
// vtkDelaunay3D vtkTransformFilter vtkGaussianSplatter

#ifndef __vtkMDelaunay2D_h
#define __vtkMDelaunay2D_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkInformationVector.h"
#include "vtkDelaunay2D.h"

class vtkAbstractTransform;

/*
#define VTK_DELAUNAY_XY_PLANE 0
#define VTK_SET_TRANSFORM_PLANE 3
#define VTK_BEST_FITTING_PLANE 4
//#define VTK_DELAUNAY_YX_PLANE 3
#define VTK_DELAUNAY_YZ_PLANE 1
#define VTK_DELAUNAY_ZX_PLANE 2
//#define VTK_DELAUNAY_ZY_PLANE 6
*/
#define VTK_DELAUNAY_XY_PLANE 0
#define VTK_SET_TRANSFORM_PLANE 1
#define VTK_BEST_FITTING_PLANE 2
//#define VTK_DELAUNAY_YX_PLANE 3
#define VTK_DELAUNAY_YZ_PLANE 3
#define VTK_DELAUNAY_ZX_PLANE 4
//#define VTK_DELAUNAY_ZY_PLANE 6


class VTK_EXPORT vtkMDelaunay2D : public vtkPolyDataAlgorithm
{
public:
	// Description:
  // Construct object with Alpha = 0.0; Tolerance = 0.001; Offset = 1.25;
  // BoundingTriangulation turned off.
  static vtkMDelaunay2D *New();
  vtkTypeRevisionMacro(vtkMDelaunay2D,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
  // Specify alpha (or distance) value to control output of this filter.
  // For a non-zero alpha value, only edges or triangles contained within
  // a sphere centered at mesh vertices will be output. Otherwise, only
  // triangles will be output.
  vtkSetClampMacro(Alpha,double,0.0,VTK_DOUBLE_MAX);
  vtkGetMacro(Alpha,double);

  // Description:
  // Specify a tolerance to control discarding of closely spaced points.
  // This tolerance is specified as a fraction of the diagonal length of
  // the bounding box of the points.
  vtkSetClampMacro(Tolerance,double,0.0,1.0);
  vtkGetMacro(Tolerance,double);

  // Description:
  // Specify a multiplier to control the size of the initial, bounding
  // Delaunay triangulation.
  vtkSetClampMacro(Offset,double,0.75,VTK_DOUBLE_MAX);
  vtkGetMacro(Offset,double);

  // Description:
  // Boolean controls whether bounding triangulation points (and associated
  // triangles) are included in the output. (These are introduced as an
  // initial triangulation to begin the triangulation process. This feature
  // is nice for debugging output.)
  vtkSetMacro(BoundingTriangulation,int);
  vtkGetMacro(BoundingTriangulation,int);
  vtkBooleanMacro(BoundingTriangulation,int);

  // Description:
  // Set / get the transform which is applied to points to generate a
  // 2D problem.  This maps a 3D dataset into a 2D dataset where
  // triangulation can be done on the XY plane.  The points are
  // transformed and triangulated.  The topology of triangulated
  // points is used as the output topology.  The output points are the
  // original (untransformed) points.  The transform can be any
  // subclass of vtkAbstractTransform (thus it does not need to be a
  // linear or invertible transform).
  virtual void SetTransform(vtkAbstractTransform*);
  vtkGetObjectMacro(Transform, vtkAbstractTransform);

  // Description:
  // Define
  vtkSetClampMacro(ProjectionPlaneMode,int,
                   VTK_DELAUNAY_XY_PLANE,VTK_DELAUNAY_ZX_PLANE);
  vtkGetMacro(ProjectionPlaneMode,int);

	void SetSource(vtkPolyData *);

  void SetSourceConnection(vtkAlgorithmOutput *algOutput);

  vtkPolyData *GetSource();

protected:
  vtkMDelaunay2D();
  ~vtkMDelaunay2D();

	double FindAxisAvg(vtkPolyData *input, int axis);

	void TranslatePoints(vtkPolyData *input, vtkDelaunay2D *del2D, int swapX, int swapY, int swapZ);

	virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	vtkAbstractTransform * ComputeBestFittingPlane(vtkPointSet *input);

  double Alpha;
  double Tolerance;
  int BoundingTriangulation;
  double Offset;

  vtkAbstractTransform *Transform;

  int ProjectionPlaneMode; //selects the plane in 3D where the Delaunay triangulation will be computed.

	virtual int FillInputPortInformation(int, vtkInformation*);

private:
  vtkMDelaunay2D(const vtkMDelaunay2D&);  // Not implemented.
  void operator=(const vtkMDelaunay2D&);  // Not implemented.
};

#endif
