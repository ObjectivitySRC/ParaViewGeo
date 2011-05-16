#ifndef __vtkKriging_h
#define __vktKriging_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkGeoValue.h"
#include "vtkPointSetNeighborhood.h"

#include <vtk_gstl.h>
#include <GsTL/geometry/covariance.h>
#include <GsTL/kriging/kriging_combiner.h>
#include <GsTL/kriging/kriging_constraints.h>

#include <string>
#include <vector>

//BTX
class Neighborhood;
//ETX

const int DEFAULT_STRUCT_COUNT = 3;

class VTK_EXPORT vtkKriging : public vtkPolyDataAlgorithm
	{
	public:
    static vtkKriging *New();
    vtkTypeRevisionMacro(vtkKriging,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    //required because it's a multi-input thing
		void SetSourceConnection(vtkAlgorithmOutput* algOutput);

    //kriging
    vtkGetMacro(MinNeigh, int);
    vtkSetMacro(MinNeigh, int);
    vtkGetMacro(MaxNeigh, int);
    vtkSetMacro(MaxNeigh, int);
    vtkGetVector3Macro(Ranges, double);
    vtkSetVector3Macro(Ranges, double);
    vtkGetVector3Macro(Angles, double);
    vtkSetVector3Macro(Angles, double);
    
    //variogram
    vtkGetMacro(Nugget, double);
    vtkSetMacro(Nugget, double);
    vtkGetMacro(StructCount, int);
    vtkSetMacro(StructCount, int);

		vtkSetVector3Macro(Contribution, double);
		vtkGetVector3Macro(Contribution, double);
		vtkSetVector3Macro(R1, double);
		vtkGetVector3Macro(R1, double);
		vtkSetVector3Macro(R2, double);
		vtkGetVector3Macro(R2, double);
		vtkSetVector3Macro(R3, double);
		vtkGetVector3Macro(R3, double);
		vtkSetVector3Macro(A1, double);
		vtkGetVector3Macro(A1, double);
		vtkSetVector3Macro(A2, double);
		vtkGetVector3Macro(A2, double);
		vtkSetVector3Macro(A3, double);
		vtkGetVector3Macro(A3, double);
		vtkSetVector3Macro(StructType, int);
		vtkGetVector3Macro(StructType, int);

    //This is for the DrillholeGA
    int PointsMissed;

  protected:
    vtkKriging();
		~vtkKriging();
    virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
		int FillInputPortInformation ( int port, vtkInformation* info );
    //Input: the pointset of the block model or target
		vtkDataSet* SimulGrid;

    //Input: Assays and particular property name
		vtkDataSet* HardDataGrid;
		vtkDoubleArray* HardDataProperty;
		char* HardDataPropertyName;

    //Output: the name of the property and the grid
    vtkDataSet* OutputGrid;
		
    //kriging ellipsoid and conditioning data
		int MinNeigh;
    int MaxNeigh;
		double Ranges[3];
		double Angles[3];

    //variogram data
    double Nugget;
    int StructCount;
    double Contribution[DEFAULT_STRUCT_COUNT];
    double R1[DEFAULT_STRUCT_COUNT];
    double R2[DEFAULT_STRUCT_COUNT];
    double R3[DEFAULT_STRUCT_COUNT];
    double A1[DEFAULT_STRUCT_COUNT];
    double A2[DEFAULT_STRUCT_COUNT];
    double A3[DEFAULT_STRUCT_COUNT];
    int StructType[DEFAULT_STRUCT_COUNT];

	private: 
    vtkKriging(const vtkKriging&);
    void operator = (const vtkKriging&);

    //kriging
    int CallKriging();
    int InitKriging();
		int ExecuteKriging();
    void CleanupKriging();

    //required typedefs (must be private and before rest of header)
    //BTX
    typedef std::vector<double>::const_iterator WeightIterator;
		typedef Kriging_combiner<WeightIterator, Neighborhood> KrigingCombiner;
    typedef GeoValue::location_type Location;
		typedef Kriging_constraints<Neighborhood, Location> KrigingConstraints;
    typedef std::string Kriging_type; //type OK
  

    //utilities
		int InitializeCovariance( Covariance<Location>* cov );
		int InitializeOrdinaryKrigingConstraints (KrigingCombiner*& Kcombiner, KrigingConstraints*& Kconstraints, Kriging_type ktype);
   

		//search ellipsoid stuff
    bool GsTLEllipsoid( double* ranges, double* angles);
		void GsTLAnglesFromRadians( double& az, double& dip, double& rake );
		bool IsValidRange(double& pri, double& sec, double& ter);

		//Neighborhood
		Neighborhood* neighborhood(double* dim, double* angles, const Covariance<Location>* cov = 0 );
    

    //nice helper function
		vtkDoubleArray* InitProperty( const std::string& name, const int& size );

    //GsTL classes
    Neighborhood* Neighbourhood;
		Covariance<Location> Covar;
		Covariance<Location>* RhsCovar;
		KrigingCombiner* KCombiner;
		KrigingConstraints* KConstraints;
		std::vector<double> KrigingWeights;
    //ETX
	};

#endif
