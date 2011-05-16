#include "vtkKriging.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include <GsTL/kriging/kriging_weights.h>

#define SPHERICAL 0
#define GAUSSIAN 1
#define EXPONENTIAL 2

#define FAIL 0
#define WIN 1

vtkCxxRevisionMacro(vtkKriging, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkKriging);

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
vtkKriging::vtkKriging()
	{
	const int ports = 2;
  this->SetNumberOfInputPorts ( ports );

	this->KConstraints = 0;
	this->KCombiner = 0;
	this->SimulGrid = 0;
	this->Neighbourhood = 0;

	this->MinNeigh = 2;
  this->MaxNeigh = 12;
  for (int i = 0; i < 3; i++)
    {
    this->Ranges[i] = 0.0;
    this->Angles[i] = 0.0;
    }

  this->Nugget = 0.0;
  this->StructCount = 0;
  for (int i = 0; i < DEFAULT_STRUCT_COUNT; i++)
    {
    this->Contribution[i] = 0.0;
    this->R1[i] = 0.0;
    this->R2[i] = 0.0;
    this->R3[i] = 0.0;
    this->A1[i] = 0.0;
    this->A2[i] = 0.0;
    this->A3[i] = 0.0;
    this->StructType[i] = 0;
    }
	}

//----------------------------------------------------------------------------
// Deconstructor
//----------------------------------------------------------------------------
vtkKriging::~vtkKriging()
	{
  if (this->KConstraints)
		delete this->KConstraints;
	if (this->KCombiner)
		delete this->KCombiner;
  if (this->Neighbourhood)
    delete this->Neighbourhood;
  if (this->RhsCovar)
    delete this->RhsCovar;
	}

//----------------------------------------------------------------------------
// SetSourceConnection
//----------------------------------------------------------------------------
void vtkKriging::SetSourceConnection( vtkAlgorithmOutput *algOutput )
	{
	this->SetInputConnection( 1, algOutput );
	}

//----------------------------------------------------------------------------
// Fill Input Port Information (Source and Input here)
//----------------------------------------------------------------------------
int vtkKriging::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 1 );
  return 1;
  }

//----------------------------------------------------------------------------
// PrintSelf
//----------------------------------------------------------------------------
void vtkKriging::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os, indent);
  }

//----------------------------------------------------------------------------
// RequestData
//----------------------------------------------------------------------------
int vtkKriging::RequestData(vtkInformation *request, 
														vtkInformationVector **inputVector, 
														vtkInformationVector *outputVector)
  {
	// get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Get the input (assays) and source (pointset)
	this->HardDataGrid = vtkDataSet::SafeDownCast (inInfo->Get(vtkDataObject::DATA_OBJECT()));
	this->SimulGrid = vtkDataSet::SafeDownCast (sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
	if (!this->HardDataGrid)
		{
		vtkErrorMacro(<<"The HardDataGrid (input) is null.");
		return FAIL;
		}
	if (!this->SimulGrid)
    {
    vtkErrorMacro(<<"The SimulGrid (source) is null.");
    return FAIL;
    }

	// Get the output (kriging)
  this->OutputGrid = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
	this->OutputGrid->DeepCopy(SimulGrid);

	// Get the assay property and its name
	vtkDataArray *inScalars = this->GetInputArrayToProcess(0,inputVector);
  if (!inScalars)
    {
    vtkErrorMacro(<<"The assay property is invalid (no input scalars)");
    return FAIL;
    }
	this->HardDataPropertyName = inScalars->GetName();
	if (!this->HardDataPropertyName)
		{
		vtkErrorMacro(<<"The assay property is invalid (no name)");
		return FAIL;
		}
  this->HardDataProperty = vtkDoubleArray::SafeDownCast(inScalars);
	if (!this->HardDataProperty)
		{
		vtkErrorMacro(<<"The assay property is invalid (no array)");
		return FAIL;
		}

  //range and bounds checks
  if (this->MinNeigh >= this->MaxNeigh)
		{
		vtkErrorMacro(<<"Minimum neighbours must be less than or equal to Maximum neighbours.\n");
		return FAIL;
		}
  if (this->MinNeigh < 0 || this->MaxNeigh < 0)
    {
    vtkErrorMacro(<<"Minimum neighbours and Maximum neighbours must be greater than 0.\n");
    return FAIL;
    }

	return this->CallKriging();
  }

//----------------------------------------------------------------------------
// CallKriging
//----------------------------------------------------------------------------
int vtkKriging::CallKriging()
  {
  if (this->InitKriging() == FAIL)
    {
    return FAIL;
    }
	return this->ExecuteKriging();
  }

//----------------------------------------------------------------------------
// InitKriging
//----------------------------------------------------------------------------
int vtkKriging::InitKriging()
	{
	// set up the left and right hand side covariances
	if (!this->InitializeCovariance( &this->Covar ))
		{
		vtkErrorMacro(<<"Covariance could not be initialized.\n");
		return FAIL;
		}
	this->RhsCovar = new Covariance<Location>(this->Covar);

	// Modify the Ellipsoid for GsTL
	if (!this->GsTLEllipsoid( this->Ranges, this->Angles))
		{
		vtkErrorMacro(<<"Error in ellipsoid definition.\n");
		return FAIL;
		}

  // reserve space in the kriging weights vector
	this->KrigingWeights.reserve( 2 * this->MaxNeigh );
	
  // kriging constraints
	return this->InitializeOrdinaryKrigingConstraints( this->KCombiner, this->KConstraints, "OK");
	}

//----------------------------------------------------------------------------
// ExecuteKriging
//----------------------------------------------------------------------------
int vtkKriging::ExecuteKriging()
	{

  //moved neighborhood construction here because it has to be done every time
  //become a PointSetNeighborhood
	//(pointSet and property are passed during PointSetNeighborhood constructing)
	this->Neighbourhood = this->neighborhood( this->Ranges, this->Angles, &this->Covar ); 
	this->Neighbourhood->selectProperty( this->HardDataPropertyName );
	this->Neighbourhood->maxSize( this->MaxNeigh );

	bool WarnSingularSystem = false;

  //create the properties
  const int numPts = this->SimulGrid->GetNumberOfPoints();

	// grade estimate
	vtkDoubleArray* est_prop = this->InitProperty("Grade_Estimate", numPts);
  this->OutputGrid->GetPointData()->AddArray(est_prop);
  double *estimate_prop = new double[numPts];
	// kriging variance
	vtkDoubleArray* var_prop = this->InitProperty("Kriging_Variance", numPts);
  this->OutputGrid->GetPointData()->AddArray(var_prop);
  double *variance_prop = new double[numPts];
  for(int i = 0; i < numPts; i++)
    {
    estimate_prop[i] = GeoValue::nullData;
    variance_prop[i] = GeoValue::nullData;
    }

  //find points in the assays that are within the neighbourhood of the
  //points in the pointset, then calculate variance and grade estimate
	GeoValue begin;
  this->PointsMissed = numPts;
	for (vtkIdType i = 0; i < numPts; i++)
		{
    // Make a GeoValue object as "begin", iterating over number of points
		begin = GeoValue(this->OutputGrid, estimate_prop, i);
		if (begin.isInformed())
			continue;
    //find neighbours
		this->Neighbourhood->findNeighbors(begin);
		if (this->Neighbourhood->size() < this->MinNeigh)
			continue;
    //get the variance
		double variance;
		int status = kriging_weights_2(	this->KrigingWeights, variance, begin.location(), 
																		*this->Neighbourhood, this->Covar, *this->RhsCovar, 
																		*this->KConstraints );
    variance_prop[i] = variance;
    if (variance >= 0)
      {
      --this->PointsMissed;
      }
    /*
		if (status == 0)
			{
      //get the grade estimate
			double estimate = (*this->KCombiner)(this->KrigingWeights.begin(), 
																						this->KrigingWeights.end(), 
																						*this->Neighbourhood);
			begin.setPropertyValue( estimate );
			variance_prop[i] = variance;
			}
		else
			{
			WarnSingularSystem = true;
			}
    */
		}

  est_prop->SetArray(estimate_prop, numPts, false);
  var_prop->SetArray(variance_prop, numPts, false);

  this->CleanupKriging();

	if (WarnSingularSystem)
		{
		vtkWarningMacro(<<"Kriging system is singular.\n");
		return FAIL;
		}
	return WIN;
	}

//----------------------------------------------------------------------------
// CleanupKriging
// You have to do this after ExecuteKriging or else repeated calls to Kriging 
// will add more and more structures to the covariances.
//----------------------------------------------------------------------------
void vtkKriging::CleanupKriging()
  {
  for (int i = this->StructCount - 1; i >= 0; --i)
    {
    this->Covar.remove_structure(i);
    this->RhsCovar->remove_structure(i);
    }
  }

//----------------------------------------------------------------------------
// InitProperty
// Creates a GsTL grid property where all values are initialized to nullData
//----------------------------------------------------------------------------
vtkDoubleArray* vtkKriging::InitProperty ( const std::string& name, const int &size)
	{ 
	vtkDoubleArray* gridProperty = vtkDoubleArray::New();
	gridProperty->SetName(name.c_str());
	gridProperty->SetNumberOfValues(size);
	for (int i = 0; i < size; ++i )
    {
		gridProperty->SetValue(i, GeoValue::nullData);
    }
  return gridProperty;
	}

//----------------------------------------------------------------------------
// GsTLEllipsoid
// Verifies the ellipsoid ranges and converts the angles to GsTL Format
//----------------------------------------------------------------------------
bool vtkKriging::GsTLEllipsoid( double* Ranges, double* Angles)
	{ // adapted from utilities.h
	if ( !this->IsValidRange( this->Ranges[0], this->Ranges[1], this->Ranges[2] ))
		{
		vtkErrorMacro( <<"Range must verify: major >= medium >= minor > 0\n" );
		return FAIL;
		}
	Angles[0] *= vtkMath::DegreesToRadians();
	Angles[1] *= vtkMath::DegreesToRadians();
	Angles[2] *= vtkMath::DegreesToRadians();
	this->GsTLAnglesFromRadians( Angles[0], Angles[1], Angles[2] );
	return WIN;
	}

//----------------------------------------------------------------------------
// GsTLAnglesFromRadians
// Converts radian angles into GsTL format radian angles.
//----------------------------------------------------------------------------
void vtkKriging::GsTLAnglesFromRadians( double& azimuth, double& dip, double& rake )
	{ 
	azimuth = vtkMath::DoublePi() / 2.0 - azimuth;
	dip = -dip;
	rake = rake;
	}

//----------------------------------------------------------------------------
// IsValidRange
// Verifies that Primary, Secondary, and Tertiary angles are in order and > 0
//----------------------------------------------------------------------------
bool vtkKriging::IsValidRange( double& Primary, double& Secondary, double& Tertiary )
	{ 
  //the ranges have to be correctly ordered and greater than 0
	if ( Primary < Secondary || Secondary < Tertiary ) 
    {
    return FAIL;
    }
  return Tertiary > 0;
	}

//----------------------------------------------------------------------------
// IsValidRange
// Verifies that Primary, Secondary, and Tertiary angles are in order and > 0
//----------------------------------------------------------------------------
int vtkKriging::InitializeCovariance( Covariance<Location>* cov )
	{
	if (StructCount == 0 && Nugget == 0)
		{
		vtkErrorMacro(<<"If there are no structures then Nugget must be > 0.\n");
		return FAIL;
		}
  cov->nugget(Nugget);
	for (int i = 0; i <StructCount; i++)
		{
		if (Contribution[i] == 0)
			{
			vtkErrorMacro( << "Variogram structure contribution must be > 0.\n" );
			return FAIL;
			}
    //check the variogram structure ranges
    if ( !this->IsValidRange( R1[i], R2[i], R3[i] ))
		  {
		  vtkErrorMacro(<<"Variogram ranges must be: major >= medium >= minor > 0.\n" );
		  return FAIL;
		  }
    //correct the angles for GsTL
		double a1 = vtkMath::DegreesToRadians() * (A1[i]);
		double a2 = vtkMath::DegreesToRadians() * (A2[i]);
		double a3 = vtkMath::DegreesToRadians() * (A3[i]);
		this->GsTLAnglesFromRadians( a1, a2, a3 );
    //add the structure to the variogram
    std::string s;
    if (this->StructType[i] == SPHERICAL)
      s = "Spherical";
    else if(this->StructType[i] == GAUSSIAN)
      s = "Gaussian";
    else if(this->StructType[i] == EXPONENTIAL)
      s = "Exponential";
    else
      s = "Spherical";
    cov->add_structure( s );
		cov->sill( i, Contribution[i] );
		cov->set_geometry(i, R1[i], R2[i], R3[i], a1, a2, a3 );
		}
	return WIN;
	}

//----------------------------------------------------------------------------
// InitializeOrdinaryKrigingConstraints
// Initializes the Constraints and Combiner. Only works for "Ordinary Kriging"
//----------------------------------------------------------------------------
int vtkKriging::InitializeOrdinaryKrigingConstraints( KrigingCombiner*& Kcombiner, KrigingConstraints*& Kconstraints, Kriging_type ktype )
	{
  // currently we only support ordinary kriging
	if ( ktype != "OK" )
		{
		vtkErrorMacro( <<"No valid kriging type provided.\n" );
		return FAIL;
		}

	// kriging constraints
	typedef Kriging_constraints_impl<Neighborhood, Location> KConstraintsImpl;
	KConstraintsImpl* constraints = new OKConstraints_impl<Neighborhood, Location>;
	this->KConstraints = new KrigingConstraints ( constraints );
  delete constraints;

  // kriging combiner
  typedef Kriging_combiner_impl<WeightIterator, Neighborhood> KCombinerImpl;
	KCombinerImpl* combiner = new KCombinerImpl;
	this->KCombiner = new KrigingCombiner( combiner );
	delete combiner;

	return WIN;
	}

//----------------------------------------------------------------------------
// neighborhood
// Creates a PointSetNeighborhood from the assay points and property
//----------------------------------------------------------------------------
Neighborhood* vtkKriging::neighborhood( double* dim, double* angles, const Covariance<Location>* cov )
	{
	//pointSet = from input data: this->HardDataGrid
	//property = from input data: this->HardDataProperty
  double *assayProperty;
  assayProperty = new double[this->HardDataGrid->GetNumberOfPoints()];
  for (int i = 0; i < this->HardDataGrid->GetNumberOfPoints(); i++)
    {
    assayProperty[i] = this->HardDataProperty->GetValue(i);
    }
	return new PointSetNeighborhood(dim[0], dim[1], dim[2], 
																	angles[0], angles[1], angles[2], 
																	this->MaxNeigh, this->MinNeigh, 
                                  this->HardDataGrid, assayProperty, cov); 
	}