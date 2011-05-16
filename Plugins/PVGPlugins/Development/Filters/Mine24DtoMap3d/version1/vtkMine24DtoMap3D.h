//===================================================================================//
//				Mirarco Mining Innovation
//
// Filter:   Mine24DtoMap3D
// Class:    vtkMine24DtoMap3D
// Author:   Nehmeh Bilal
// Director: Marie-Grabrielle Valet 
// Date:     July 2008
// contact: nehmebilal@gmail.com 
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
#include <map>
#include <algorithm>
#include <vector>
#include <ostream>
#include<iostream>
#include "vtkCleanPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkDataArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"






class VTK_EXPORT vtkMine24DtoMap3D : public vtkPolyDataAlgorithm
{
public:
  static vtkMine24DtoMap3D *New();
  vtkTypeRevisionMacro(vtkMine24DtoMap3D,vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);
	int* getVertices(void);
	vtkSetStringMacro(ActivityFile);
	vtkGetStringMacro(ActivityFile);
	vtkSetMacro(WithActivity, int);
	vtkSetMacro(ActivityStep, int);
	vtkSetMacro(dateFormat, int);
	vtkSetMacro(AngleTolerance, double);

	bool activityFound;
	int ActivityStep;
	int dateFormat;
	double AngleTolerance;
  

protected:
  vtkMine24DtoMap3D();
  ~vtkMine24DtoMap3D();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

	char* ActivityFile;
	int WithActivity;
	

	
	

private:
  vtkMine24DtoMap3D(const vtkMine24DtoMap3D&);  // Not implemented.
  void operator=(const vtkMine24DtoMap3D&);  // Not implemented.
	bool* goodConfig(double* pt1, double* pt2, double*pt3, double* pt4); 
	int getColor(int color);
	void readActivity(char* ActivityFile);

	int blue;
	int green;
	int yellow;
	int red;
	int grey;
	
	
	
};

#endif
