//===================================================================================//
//				Mirarco Mining Innovation
//
// Filter:   Mine24DtoMap3D
// Class:    vtkMine24DtoMap3D
// Author:   Nehmeh Bilal
// Director: Marie-Grabrielle Vallet 
// Date:     July 2008
// contact: nehmebilal@gmail.com 

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
//====================================================================================//

#ifndef __vtkMine24DtoMap3D_h
#define __vtkMine24DtoMap3D_h



#include "vtkPolyDataAlgorithm.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include <math.h>
#include <algorithm>
#include <ostream>
#include<iostream>
#include "vtkCleanPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkDataArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"

using namespace std;

class VTK_EXPORT vtkMine24DtoMap3D : public vtkPolyDataAlgorithm
{
public:
  static vtkMine24DtoMap3D *New();
  vtkTypeRevisionMacro(vtkMine24DtoMap3D,vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);



	vtkSetMacro(Simplification, int);
	vtkSetMacro(SimplifyMesh, int);
	vtkSetMacro(Approximation, int);
	vtkSetMacro(Conversion, int);
	vtkSetStringMacro(ActivityFile);
	vtkGetStringMacro(ActivityFile);
	vtkSetMacro(WithActivity, int);
	vtkSetMacro(ActivityStep, int);
	vtkSetMacro(dateFormat, int);
	vtkSetMacro(AngleTolerance, double);
	vtkSetMacro(AngleStep, double);

	vtkSetStringMacro(SegmentID);
	vtkGetStringMacro(SegmentID);
	
	vtkSetStringMacro(SegmentColor);
	vtkGetStringMacro(SegmentColor);


	//---------------- CleanPolyData -------------------
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
	// ---------------------------------------------------


protected:
  vtkMine24DtoMap3D();
  ~vtkMine24DtoMap3D();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

  char *SegmentID;
  char *SegmentColor;

	// CleanPolyData
	int PieceInvariant;
	double Tolerance;
	double AbsoluteTolerance;
	int ToleranceIsAbsolute;
	int ConvertLinesToPoints;
	int ConvertPolysToLines;
	int ConvertStripsToPolys;
	int PointMerging;

private:
  vtkMine24DtoMap3D(const vtkMine24DtoMap3D&);  // Not implemented.
  void operator=(const vtkMine24DtoMap3D&);  // Not implemented.

	int getColor(int color);
	void readActivity(char* ActivityFile);
	void approximateStopes();
	void simplifyStopes();
	void convertStopes();	

	void simplifyMesh();

	vtkPolyData *realInput;
	vtkPolyData *output;
	vtkPolyData *input;
	vtkCellArray *outCells;
	vtkCellArray *inCells;
	vtkPoints *inPoints;
	vtkPoints *outPoints;

	vtkDataArray* M4DNUM;
	vtkDataArray* COLOUR;
	vtkIntArray* BlkNum;
	vtkIntArray* MiningBlock;

	int numberOfCells;

	int Simplification;
	int SimplifyMesh;
	int Approximation;
	int Conversion;
	bool activityFound;
	int ActivityStep;
	int dateFormat;
	double AngleTolerance;
	double AngleStep;
	char* ActivityFile;
	int WithActivity;  
	int blue;
	int green;
	int yellow;
	int red;
	int grey;
	bool orientedRectangle;


};

#endif
