// .NAME vtkSOTResult.h
// By: Yasmine HADDAD && Matthew Livingstone  (mai 2008)
// re wroted by: Nehme Bilal (March 2009)
// Read SOTResult file (.csv).

#ifndef __vtkSOTResult_h
#define __vtkSOTResult_h

#include <vtksys/ios/sstream>
#include "vtkPolyDataAlgorithm.h"
#include "vtkAppendPolyData.h"
#include "vtkStringList.h"
#include "vtkStdString.h"
#include "vtkPlane.h"

//class vtkStdString;
class vtkInformationVector;
class vtkCellArray;
class vtkPoints;
class vtkImplicitFunction;

struct Internal;


class VTK_EXPORT vtkSOTResult : public vtkPolyDataAlgorithm
{
public:
  static vtkSOTResult* New();
  vtkTypeRevisionMacro(vtkSOTResult,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
	
	//all set macro's should have a get macro too incase somebody needs to use the class 

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

	vtkSetStringMacro(ClipType);
	vtkGetStringMacro(ClipType);

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
	char* ClipType;

	int numberOfCircles;
	int numberOfGrids;

private:
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
