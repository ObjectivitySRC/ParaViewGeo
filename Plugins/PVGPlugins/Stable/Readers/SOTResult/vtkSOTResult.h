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
// .NAME vtkSOTResult.h
// By: Yasmine HADDAD && Matthew Livingstone  (mai 2008)
// re wroted by: Nehme Bilal (March 2009)
// Read SOTResult file (.csv).

// TODO: File needs to be cleaned up to vtk style. Variables and indentation.
#ifndef __vtkSOTResult_h
#define __vtkSOTResult_h

#include <vtksys/ios/sstream>
#include "vtkPolyDataAlgorithm.h"
#include "vtkAppendPolyData.h"
#include "vtkStringList.h"
#include "vtkStdString.h"
#include "vtkPlane.h"
#include "vtkDataArraySelection.h"

//class vtkStdString;
class vtkInformationVector;
class vtkCellArray;
class vtkPoints;
class vtkImplicitFunction;

struct Internal;

struct Internal_PropertiesStatus;

class VTK_EXPORT vtkSOTResult : public vtkPolyDataAlgorithm
{
public:
  static vtkSOTResult* New();
  vtkTypeRevisionMacro(vtkSOTResult,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
	
	vtkSetMacro(ApplyPressed, int);
	vtkGetMacro(ApplyPressed, int);

	vtkSetStringMacro(RadiusVariable);
	vtkGetStringMacro(RadiusVariable);

	vtkSetStringMacro(DistanceVariable);
	vtkGetStringMacro(DistanceVariable);

	vtkSetStringMacro(AngleVariable);
	vtkGetStringMacro(AngleVariable);

	// Used to determine with property will be used to threshold the data in comparison view
	vtkSetStringMacro(ThVariable);
	vtkGetStringMacro(ThVariable);

	vtkSetStringMacro(Headers);
	vtkGetStringMacro(Headers);

	//all set macro's should have a get macro too incase somebody needs to use the class
	void SetArraySelection(const char*name, int status);

	vtkSetStringMacro(StringHeaders);
	vtkGetStringMacro(StringHeaders);

	vtkSetMacro(ReverseRadius, bool);
	vtkGetMacro(ReverseRadius, bool);

	vtkSetMacro(ReverseDistance, bool);
	vtkGetMacro(ReverseDistance, bool);

	vtkSetMacro(ReverseAngle, bool);
	vtkGetMacro(ReverseAngle, bool);

	vtkSetMacro(AlignCircles, bool);	
	vtkGetMacro(AlignCircles, bool);	

	vtkSetMacro(DistanceBetween, double);
	vtkGetMacro(DistanceBetween, double);
	
	vtkSetMacro(ReferenceCirclesSize, double);
	vtkGetMacro(ReferenceCirclesSize, double);
		
	vtkSetMacro(CircleThickness, double);
	vtkGetMacro(CircleThickness, double);

	vtkSetMacro(AxisThickness, double);
	vtkGetMacro(AxisThickness, double);

	vtkSetMacro(numberOfCircles, int);
	vtkGetMacro(numberOfCircles, int);

	vtkSetMacro(numberOfGrids, int);
	vtkGetMacro(numberOfGrids, int);

	vtkSetMacro(PlaneDistanceVariable, double);
	vtkGetMacro(PlaneDistanceVariable, double);

	vtkSetMacro(EnableClip, bool);
	vtkGetMacro(EnableClip, bool);

	vtkSetMacro(DistMin, double);
	vtkGetMacro(DistMin, double);

	vtkSetMacro(DistMax, double);
	vtkGetMacro(DistMax, double);

	vtkSetMacro(BTransXVariable, double);
	vtkGetMacro(BTransXVariable, double);
	vtkSetMacro(BTransYVariable, double);
	vtkGetMacro(BTransYVariable, double);
	vtkSetMacro(BTransZVariable, double);
	vtkGetMacro(BTransZVariable, double);

	vtkSetMacro(BScaleXVariable, double);
	vtkGetMacro(BScaleXVariable, double);
	vtkSetMacro(BScaleYVariable, double);
	vtkGetMacro(BScaleYVariable, double);
	vtkSetMacro(BScaleZVariable, double);
	vtkGetMacro(BScaleZVariable, double);

  vtkGetMacro(ArtificialMinMineLife, double);
	vtkSetMacro(ArtificialMinMineLife, double);

	vtkGetMacro(ArtificialMaxMineLife, double);
	vtkSetMacro(ArtificialMaxMineLife, double);

	vtkGetMacro(ArtificialMinNPV, double);
	vtkSetMacro(ArtificialMinNPV, double);

	vtkGetMacro(ArtificialMaxNPV, double);
	vtkSetMacro(ArtificialMaxNPV, double);
	
  // Description
  // Specify the implicit function to perform the cutting.
  virtual void SetCutFunction(vtkImplicitFunction*);
  vtkGetObjectMacro(CutFunction,vtkImplicitFunction);

	int CanReadFile( const char* fname );
protected:
  vtkSOTResult(vtkImplicitFunction *cf=NULL);
  ~vtkSOTResult();

	vtkImplicitFunction *CutFunction;

  virtual int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);
                  
  virtual int RequestInformation(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

	bool parseFile(int lineSize, ifstream &file);
	int transformCoordinates();
	int transformComparisonCoordinates();
	void freeMemory();
	int addToVerts(double radius, double distance, double angle, vtkIdType counter,
		double dx, double dy);
	int AddGrid(double x, double y, double z, bool showDistLabel, double Label);
	int CreateAxes(double x, double y, double dz);
	int addNormalViewGrids(double x, double y, double z, vtkStdString compareLabel, bool showCompLabel);
	int addInitialGrid(double x, double y, double z, double distLabel, bool showDistLabel,
										vtkStdString compareLabel, bool showCompLabel);
	int addComparisonGrids();

	int ApplyPressed;
	char* Headers;
	char* StringHeaders;
	char* RadiusVariable;
	char* DistanceVariable;
	char* AngleVariable;
	char* ThVariable;
	bool ReverseRadius;
	bool ReverseDistance;
	bool ReverseAngle;
	bool AlignCircles;
	double DistanceBetween;
	double ReferenceCirclesSize;
	double AxisThickness;
	double CircleThickness;
  double ArtificialMinMineLife;
	double ArtificialMaxMineLife;
	double ArtificialMinNPV;
	double ArtificialMaxNPV;

	double PlaneDistanceVariable;
	bool EnableClip;
	double DistMin;
	double DistMax;
	double BTransXVariable;
	double BTransYVariable;
	double BTransZVariable;
	double BScaleXVariable;
	double BScaleYVariable;
	double BScaleZVariable;
	
	int numberOfCircles;
	int numberOfGrids;

private:
	Internal_PropertiesStatus* SotProperties;
	void ParseLine(vtkStdString line);

	const char* FileName; 

	vtkDoubleArray* CapacityValues;
	vtkStringList* PropertyNames;

	unsigned int radiusIndex;
	unsigned int distanceIndex;
	unsigned int angleIndex;
	unsigned int thIndex;

	double minRadius;
	double maxRadius;
	double deltaRadius;

	double minDistance;
	double maxDistance;
	double deltaDistance;

	double minAngle;
	double maxAngle;
	double deltaAngle;

	double dimension;

	bool guiLoaded;

	vtkCellArray *verts;
	vtkPoints *points;
	vtkPolyData *axesOutput;
	vtkPolyData *circlesOutput;
	vtkAppendPolyData *circles;
	vtkAppendPolyData *axes;

	// internal class used to enable using of std containers
	Internal *internals;
};

#endif
