#ifndef _VTK_MSC_DH_GENERATOR_H
#define _VTK_MSC_DH_GENERATOR_H

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"

#include <vector>
#include <map>
#include <set>
#include <fstream>

class vtkUnstructuredGrid;
class vtkPointLocator;



class VTK_EXPORT vtkMSCdhGenerator : public vtkPolyDataAlgorithm
{
public:
	static vtkMSCdhGenerator *New();
	vtkTypeRevisionMacro(vtkMSCdhGenerator,vtkPolyDataAlgorithm);

	/**
	This function is needed because we accept an Input and a Source in the filter.

	We set the source connection to be port 1. (Input is port 0)
	*/
	void SetSourceConnection(vtkAlgorithmOutput* algOutput);


	vtkSetStringMacro(CollarFile);
	vtkSetStringMacro(OutputFile);

	vtkSetMacro(CollarLocation, int);
	vtkGetMacro(CollarLocation, int);

	vtkSetMacro(DAzimuthMin, double);
	vtkGetMacro(DAzimuthMin, double);

	vtkSetMacro(DAzimuthMax, double);
	vtkGetMacro(DAzimuthMax, double);

	vtkSetMacro(DAzimuthStep, double);
	vtkGetMacro(DAzimuthStep, double);

	vtkSetMacro(DDipMin, double);
	vtkGetMacro(DDipMin, double);

	vtkSetMacro(DDipMax, double);
	vtkGetMacro(DDipMax, double);

	vtkSetMacro(DDipStep, double);
	vtkGetMacro(DDipStep, double);

	vtkSetMacro(DLengthMin, double);
	vtkGetMacro(DLengthMin, double);

	vtkSetMacro(DLengthMax, double);
	vtkGetMacro(DLengthMax, double);

	vtkSetMacro(DLengthStep, double);
	vtkGetMacro(DLengthStep, double);

	vtkSetMacro(DDepthMin, double);
	vtkGetMacro(DDepthMin, double);

	vtkSetMacro(DDepthMax, double);
	vtkGetMacro(DDepthMax, double);

	vtkSetMacro(SegmentLength, double);
	vtkGetMacro(SegmentLength, double);

	vtkSetMacro(DRadius, double);
	vtkGetMacro(DRadius, double);


	vtkSetMacro(EMajor, double);
	vtkSetMacro(EMedium, double);
	vtkSetMacro(EMinor, double);
	vtkSetMacro(EAzimuth, double);
	vtkSetMacro(EDip, double);
	vtkSetMacro(ERake, double);

	vtkGetMacro(EMajor, double);
	vtkGetMacro(EMedium, double);
	vtkGetMacro(EMinor, double);
	vtkGetMacro(EAzimuth, double);
	vtkGetMacro(EDip, double);
	vtkGetMacro(ERake, double);

	vtkSetMacro(ComputeRanges, int);
	vtkGetMacro(ComputeRanges, int);

	vtkSetMacro(CostPerMeter, double);
	vtkGetMacro(CostPerMeter, double);

	vtkSetMacro(DrillMovingCost, double);
	vtkGetMacro(DrillMovingCost, double);

	vtkSetStringMacro(BlocksMineralValue);
	vtkGetStringMacro(BlocksMineralValue);	

	vtkSetMacro(UseEllipsoid, int);
	vtkGetMacro(UseEllipsoid, int);

protected:

	vtkMSCdhGenerator();
	~vtkMSCdhGenerator();

	// here I am using pointer to methods to implement constraints.
	// to add a new constraint, all you need to do is to create a new 
	// method with the same signature (see depthConstraint method ) 
	// and add it to the list of constraints inside the constructor.
	// The benefit of using pointers to methods in comparaison with
	// using pointers to function, which must be static function, 
	// is that we have access to attributes in the methods implementing constraints.
	//BTX
	typedef  bool (vtkMSCdhGenerator::*constraintMF)(const double[3], const double[3]);
	std::vector<constraintMF> constraints;


	bool ReadCollarPoints(std::vector<double*>& collarPoints);
	//ETX

	//constraints begin ---------------
	bool depthConstraint(const double startPt[3], const double endPt[3]) ;

	//constraints end -----------------

	void SetMetric();
	void MakeRotMatrix(double ax, double ay, double az, double *m);
	double EvaluateDistSquare( double* P, double* Q);
	void getAzimuthDipLength(  const double *start_point, 
		const double *end_point, 
		double *az, 
		double *dip, 
		double *length);

	void generateCollarPointDrillholes(const double collarPoint[3]);
	void getDipRange(const double collarPoint[3], const double azimuth,
		double *dipMin, double *dipMax );
	void getAzimuthRange(const double collarPoint[3], 
		double *azMin, double *azMax );
	void computeBlockModelCenter();

	//BTX
	void generateDrillHoles(std::vector<double*>& collarPoints);

	void generateDrillHole(std::set<int>& currentElements, 
		const double collarPoint[3], 
		const double endPoint[3],
		double azr,
		double dipr, 
		double length);

	void addDrillholeNeighbors(std::set<int>& currentElements,
		const double startPoint[3], 
		const double endPoint[3],
		vtkPoints* points,
		double azr,
		double dipr);

	void writeDrillholeToFile(const std::set<int>& currentElements, 
		const double collarPoint[3],
		const double endPoint[3]);
	//ETX

	//vtkUnstructuredGrid* cylindricalClip(const double startPoint[3], 
	//																		 const double endPoint[3],
	//																		 const double X[3],
	//																		 const double Y[3],
	//																		 const double Z[3],
	//																		 double radius, 
	//																		 double azR,
	//																		 double dipR);

	unsigned int getNumberOfCollarPoints();


	// Description:
	// given a vector N, this method will find two other 
	// unit vector perpendicular to N and to each other in order
	// to find an orthonormal repair:
	// x axis = N
	// y axis = U
	// z axis = V
	// memory for N, U and V must be allocated before calling this 
	// method
	//	void getReference(double N[3], double U[3], double V[3]);


	virtual int RequestData(vtkInformation *, vtkInformationVector **, 
		vtkInformationVector *);
	virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
	vtkMSCdhGenerator(const vtkMSCdhGenerator&);  // Not implemented.
	void operator=(const vtkMSCdhGenerator&);  // Not implemented.

	/**
	When the input collar points come from the Filter's Source, this is where they are stored.
	*/
	vtkPolyData *InputCollarPoints;

	/**
	The magnitudes (major, medium, minor) of the search ellipsoid.
	*/
	//double Ranges[3];

	/**
	The Metric Tensor
	*/
	double* MetricTensor;

	/**
	Indicates where the input collar points should come from.

	- 0 = CSV File
	- 1 = From Filter Source
	*/
	int CollarLocation;

	/**
	Path to the CSV file that has collar points. Only matters if <code>CollarLocation</code> is set to 0.
	\see vtkDrillholeGA::CollarLocation
	*/
	char *CollarFile;

	char* OutputFile;

	double SegmentLength;

	double DAzimuthMin;
	double DAzimuthMax;
	double DAzimuthStep;

	double DDipMin;
	double DDipMax;
	double DDipStep;

	double DLengthMin;
	double DLengthMax;
	double DLengthStep;

	double DDepthMin;
	double DDepthMax;

	double DRadius;

	char* BlocksMineralValue;

	vtkPolyData* InputGrid;
	vtkPolyData* output;

	vtkPoints* outPoints;
	vtkCellArray* outLines;

	//BTX
	//	std::map<int, std::vector<int>> neighbors;
	std::ofstream outputFile;
	//ETX

	vtkPointLocator *Locator;

	double EMajor;
	double EMedium;
	double EMinor;
	double EAzimuth;
	double EDip;
	double ERake;

	double center[3];

	// dip, azimuth and length for the reference drillhole.
	// each collar point will have a reference drillhole and 
	// these values will change during runtime
	double rAz;
	double rDip;
	double rLength;

	int ComputeRanges;

	double CostPerMeter;
	double DrillMovingCost;

	int currentCollarPointID;

	int numberOfCollarPoints;

	vtkDataArray* blocksMineralValueArray;

	int UseEllipsoid;

	int nbTry;
	int nbHole;
};

#endif
