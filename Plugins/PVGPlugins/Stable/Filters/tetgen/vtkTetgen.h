/*
   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


//===================================================================================//
//				Mirarco Mining Innovation
//
// Filter:   Volumique Tetrahedralisation
// Class:    vtkTetgen
// Author:   Nehmeh Bilal
// Director: Marie-Grabrielle Valet 
// Date:     July 2008
// contact: nehmebilal@gmail.com 
//====================================================================================//

#ifndef __vtkTetgen_h
#define __vtkTetgen_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkUnstructuredGridAlgorithm.h"


struct tetgen_Internal;


class VTK_EXPORT vtkTetgen : public vtkUnstructuredGridAlgorithm 
{
public:
  static vtkTetgen *New();    
  vtkTypeRevisionMacro(vtkTetgen,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

	vtkSetMacro(tetgenCommand, int);  // vtkSetMacro is used to get a input value of a parameter from the filter Menu

	vtkSetMacro(RadiusEdgeRatio, double);

	vtkSetMacro(MaxTetrahedronVolume, double);

	vtkSetMacro(MaxTetrahedronVolumeInBox, double);
	vtkSetMacro(MaxTetrahedronVolumeInBox2, double);

	vtkSetMacro(WithRegions,int);
	vtkGetMacro(WithRegions,int);

	vtkSetMacro(WithPredefinedRegions,int);
	vtkGetMacro(WithPredefinedRegions,int);

	vtkSetMacro(WithBoundingBox,int);
	vtkGetMacro(WithBoundingBox,int);

  vtkSetMacro(deltaX, double);
  vtkSetMacro(deltaY, double);
  vtkSetMacro(deltaZ, double);

	vtkSetMacro(WithBBX1,int);
	vtkGetMacro(WithBBX1,int);

	vtkSetMacro(WithBBX2,int);
	vtkGetMacro(WithBBX2,int);

  vtkSetMacro(deltaX2, double);
  vtkSetMacro(deltaY2, double);
  vtkSetMacro(deltaZ2, double);

	vtkSetStringMacro(RegionArray);
	vtkGetStringMacro(RegionArray);

	vtkSetMacro(PieceInvariant, int);
	vtkGetMacro(PieceInvariant, int);

	vtkSetMacro(Tolerance,double);
	vtkSetMacro(AbsoluteTolerance,double);

	vtkSetMacro(ToleranceIsAbsolute,int);
	vtkGetMacro(ToleranceIsAbsolute,int);

	vtkSetMacro(ConvertLinesToPoints,int);
	vtkGetMacro(ConvertLinesToPoints,int);

	vtkSetMacro(ConvertPolysToLines,int);
	vtkGetMacro(ConvertPolysToLines,int);

	vtkSetMacro(ConvertStripsToPolys,int);
	vtkGetMacro(ConvertStripsToPolys,int);

	vtkSetMacro(PointMerging,int);
	vtkGetMacro(PointMerging,int);

	//vtkSetMacro(numberOfTetrahedron, int);
	vtkGetMacro(numberOfTetrahedron, int);
	

	void addBBX(int i); // add a bounding box to the input before tetrahedralization
    
  
protected:
  vtkTetgen();
  ~vtkTetgen();
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  int tetgenCommand;				// to specifiy wich tetgen command will be excecuted

  double RadiusEdgeRatio;  /*   A tetrahedron t has a unique circumsphere. Let R = R(t) 
																be that radius and L = L(t) the length
																of the shortest edge. The radius-edge ratio 
																Q = Q(t) of the tetrahedron is: Q = R/L.
																This ratio is minimized by the regular tetrahedron (in which case the lengths
																of the six edges are equal, and the circumcenter is the barycenter), that is:
																Q >= 0.612 */

  double MaxTetrahedronVolume; // no tetrahedra is generated whose volume is larger than that number

	int numberOfTetrahedron;		 // used to show the number of generated tetrahedron 		

	int WithRegions;						 // this variable is used as a boolean, 1 -> find and create regions, 0-> forget about regions 

	int WithPredefinedRegions;   // 1 -> read MAP3D regions attribut from the input surfaces and give all tetrahedra a region
															 // attribut corresponding to the surface region attribut where they are located

	int WithBoundingBox;				// 1 -> add bounding box to the input 

	double MaxTetrahedronVolumeInBox; // no tetrahedra in the bounding box and outside the original data
																		// is generated whose volume is larger than that number.
	double MaxTetrahedronVolumeInBox2;
							//////
	double deltaX;	//	100% -> exact bounding box
  double deltaY;	//	200% -> 2* exact bounding box
  double deltaZ;	//	xx % -> x * exact bounding box
							//////

	int WithBBX1;				// 1 -> add first bounding box to the input
	int WithBBX2;				// 1 -> add second bounding box to the input 

							////////
	double deltaX2;	//	100% -> exact bounding box
  double deltaY2;	//	200% -> 2* exact bounding box
  double deltaZ2;	//	xx % -> x * exact bounding box
							////////


	// clean filter property
	int PieceInvariant;
	double Tolerance;
	double AbsoluteTolerance;
	int ToleranceIsAbsolute;
	int ConvertLinesToPoints;
	int ConvertPolysToLines;
	int ConvertStripsToPolys;
	int PointMerging;







private:
	vtkCellArray *inCells;	// real input cells
	vtkCellArray *Cells;    // cells after cleaning and triangulation
	vtkPoints *inPoints;		// input cleaned points
	vtkIdType numberOfCells;			// input number of cells
	vtkIdType numberOfPoints;			// input number of points

	char* RegionArray;

	double bounds[6];
	vtkIdType coordCounter;
	double bounds1[6];
	double bounds2[6];

	tetgen_Internal *Internal;


	vtkTetgen(const vtkTetgen&);  // Not implemented.
  void operator=(const vtkTetgen&);  // Not implemented.
};

#endif
