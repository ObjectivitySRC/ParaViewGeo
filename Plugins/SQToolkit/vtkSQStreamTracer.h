/*
   ____    _ __           ____               __    ____
  / __/___(_) /  ___ ____/ __ \__ _____ ___ / /_  /  _/__  ____
 _\ \/ __/ / _ \/ -_) __/ /_/ / // / -_|_-</ __/ _/ // _ \/ __/
/___/\__/_/_.__/\__/_/  \___\_\_,_/\__/___/\__/ /___/_//_/\__(_) 

Copyright 2008 SciberQuest Inc.

*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSQStreamTracer.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSQStreamTracer - Streamline generator
// .SECTION Description
// vtkSQStreamTracer is a filter that integrates a vector field to generate
// streamlines. The integration is performed using a specified integrator,
// by default Runge-Kutta2. 
// 
// vtkSQStreamTracer produces polylines as the output, with each cell (i.e.,
// polyline) representing a streamline. The attribute values associated
// with each streamline are stored in the cell data, whereas those
// associated with streamline-points are stored in the point data.
//
// vtkSQStreamTracer supports forward (the default), backward, and combined
// (i.e., BOTH) integration. The length of a streamline is governed by 
// specifying a maximum value either in physical arc length or in (local)
// cell length. Otherwise, the integration terminates upon exiting the
// flow field domain, or if the particle speed is reduced to a value less
// than a specified terminal speed, or when a maximum number of steps is 
// completed. The specific reason for the termination is stored in a cell 
// array named ReasonForTermination.
//
// Note that normalized vectors are adopted in streamline integration,
// which achieves high numerical accuracy/smoothness of flow lines that is
// particularly guranteed for Runge-Kutta45 with adaptive step size and
// error control). In support of this feature, the underlying step size is
// ALWAYS in arc length unit (LENGTH_UNIT) while the 'real' time interval 
// (virtual for steady flows) that a particle actually takes to trave in a 
// single step is obtained by dividing the arc length by the LOCAL speed. 
// The overall elapsed time (i.e., the life span) of the particle is the 
// sum of those individual step-wise time intervals.
//
// The quality of streamline integration can be controlled by setting the
// initial integration step (InitialIntegrationStep), particularly for 
// Runge-Kutta2 and Runge-Kutta4 (with a fixed step size), and in the case
// of Runge-Kutta45 (with an adaptive step size and error control) the
// minimum integration step, the maximum integration step, and the maximum
// error. These steps are in either LENGTH_UNIT or CELL_LENGTH_UNIT while
// the error is in physical arc length. For the former two integrators,
// there is a trade-off between integration speed and streamline quality.
//
// The integration time, vorticity, rotation and angular velocity are stored
// in point data arrays named "IntegrationTime", "Vorticity", "Rotation" and
// "AngularVelocity", respectively (vorticity, rotation and angular velocity
// are computed only when ComputeVorticity is on). All point data attributes
// in the source dataset are interpolated on the new streamline points.
//
// vtkSQStreamTracer supports integration through any type of dataset. Thus if
// the dataset contains 2D cells like polygons or triangles, the integration
// is constrained to lie on the surface defined by 2D cells.
//
// The starting point, or the so-called 'seed', of a streamline may be set
// in two different ways. Starting from global x-y-z "position" allows you
// to start a single trace at a specified x-y-z coordinate. If you specify
// a source object, traces will be generated from each point in the source
// that is inside the dataset.
//
// .SECTION See Also
// vtkRibbonFilter vtkRuledSurfaceFilter vtkInitialValueProblemSolver 
// vtkRungeKutta2 vtkRungeKutta4 vtkRungeKutta45 vtkTemporalStreamTracer
// vtkInterpolatedVelocityField
//  

#ifndef __vtkSQStreamTracer_h
#define __vtkSQStreamTracer_h

#include "vtkPolyDataAlgorithm.h"

#include "vtkInitialValueProblemSolver.h" // Needed for constants

class vtkCompositeDataSet;
class vtkDataArray;
class vtkDoubleArray;
class vtkExecutive;
class vtkGenericCell;
class vtkIdList;
class vtkIntArray;
class vtkInterpolatedVelocityField;

class VTK_EXPORT vtkSQStreamTracer : public vtkPolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkSQStreamTracer,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object to start from position (0,0,0), with forward
  // integration, terminal speed 1.0E-12, vorticity computation on,
  // integration step size 0.5 (in cell length unit), maximum number
  // of steps 2000, using Runge-Kutta2, and maximum propagation 1.0 
  // (in arc length unit).
  static vtkSQStreamTracer *New();

  // Description:
  // Specify the starting point (seed) of a streamline in the global
  // coordinate system. Search must be performed to find the initial cell
  // from which to start integration.
  vtkSetVector3Macro(StartPosition, double);
  vtkGetVector3Macro(StartPosition, double);

  // Description:
  // Specify the source object used to generate starting points (seeds).
  // Old style. Do not use.
  void SetSource(vtkDataSet *source);
  vtkDataSet *GetSource();

  // Description:
  // Specify the source object used to generate starting points (seeds).
  // New style.
  void SetSourceConnection(vtkAlgorithmOutput* algOutput);

//BTX
  // The previously-supported TIME_UNIT is excluded in this current
  // enumeration definition because the underlying step size is ALWAYS in
  // arc length unit (LENGTH_UNIT) while the 'real' time interval (virtual 
  // for steady flows) that a particle actually takes to trave in a single
  // step is obtained by dividing the arc length by the LOCAL speed. The
  // overall elapsed time (i.e., the life span) of the particle is the sum
  // of those individual step-wise time intervals. The arc-length-to-time 
  // convertion only occurs for vorticity computation and for generating a
  // point data array named 'IntegrationTime'.
  enum Units
  {
    LENGTH_UNIT = 1,
    CELL_LENGTH_UNIT = 2
  };

  enum Solvers
  {
    RUNGE_KUTTA2,
    RUNGE_KUTTA4,
    RUNGE_KUTTA45,
    NONE,
    UNKNOWN
  };

  enum ReasonForTermination
  {
    OUT_OF_DOMAIN = vtkInitialValueProblemSolver::OUT_OF_DOMAIN,
    NOT_INITIALIZED = vtkInitialValueProblemSolver::NOT_INITIALIZED ,
    UNEXPECTED_VALUE = vtkInitialValueProblemSolver::UNEXPECTED_VALUE,
    OUT_OF_LENGTH = 4,
    OUT_OF_STEPS = 5,
    STAGNATION = 6
  };
//ETX

  // Description:
  // Set/get the integrator type to be used for streamline generation. 
  // The object passed is not actually used but is cloned with 
  // NewInstance in the process of integration  (prototype pattern). 
  // The default is Runge-Kutta2. The integrator can also be changed
  // using SetIntegratorType. The recognized solvers are:
  // RUNGE_KUTTA2  = 0
  // RUNGE_KUTTA4  = 1
  // RUNGE_KUTTA45 = 2
  void SetIntegrator(vtkInitialValueProblemSolver *);
  vtkGetObjectMacro ( Integrator, vtkInitialValueProblemSolver );
  void SetIntegratorType(int type);
  int GetIntegratorType();
  void SetIntegratorTypeToRungeKutta2()
    {this->SetIntegratorType(RUNGE_KUTTA2);};
  void SetIntegratorTypeToRungeKutta4()
    {this->SetIntegratorType(RUNGE_KUTTA4);};
  void SetIntegratorTypeToRungeKutta45()
    {this->SetIntegratorType(RUNGE_KUTTA45);};

  // Description:
  // Specify the maximum length of a streamline expressed in LENGTH_UNIT.
  void SetMaximumPropagation(double max);
  double GetMaximumPropagation() { return this->MaximumPropagation; }
  // Description:
  // Specify a uniform integration step unit for MinimumIntegrationStep, 
  // InitialIntegrationStep, and MaximumIntegrationStep. NOTE: The valid
  // unit is now limited to only LENGTH_UNIT (1) and CELL_LENGTH_UNIT (2),
  // EXCLUDING the previously-supported TIME_UNIT.  
  void SetIntegrationStepUnit( int unit );
  int  GetIntegrationStepUnit() { return this->IntegrationStepUnit; } 

  // Description:
  // Specify the Initial step size used for line integration, expressed in:
  // LENGTH_UNIT      = 1
  // CELL_LENGTH_UNIT = 2
  // (either the starting size for an adaptive integrator, e.g., RK45,
  // or the constant / fixed size for non-adaptive ones, i.e., RK2 and RK4)
  void SetInitialIntegrationStep(double step);
  double GetInitialIntegrationStep() { return this->InitialIntegrationStep; }

  // Description:
  // Specify the Minimum step size used for line integration, expressed in:
  // LENGTH_UNIT      = 1
  // CELL_LENGTH_UNIT = 2
  // (Only valid for an adaptive integrator, e.g., RK45)
  void SetMinimumIntegrationStep( double step );
  double GetMinimumIntegrationStep() { return this->MinimumIntegrationStep; }

  // Description:
  // Specify the Maximum step size used for line integration, expressed in:
  // LENGTH_UNIT      = 1
  // CELL_LENGTH_UNIT = 2
  // (Only valid for an adaptive integrator, e.g., RK45)
  void SetMaximumIntegrationStep( double step );
  double GetMaximumIntegrationStep() { return this->MaximumIntegrationStep; }

  // Description
  // Specify the maximum error tolerated throughout streamline integration.
  vtkSetMacro(MaximumError, double);
  vtkGetMacro(MaximumError, double);

  // Description
  // Specify the maximum number of steps for integrating a streamline.
  vtkSetMacro(MaximumNumberOfSteps, vtkIdType);
  vtkGetMacro(MaximumNumberOfSteps, vtkIdType);

  // Description
  // Specify the terminal speed value, below which integration is terminated.
  vtkSetMacro(TerminalSpeed, double);
  vtkGetMacro(TerminalSpeed, double);

//BTX
  enum
  {
    FORWARD,
    BACKWARD,
    BOTH
  };
//ETX

  // Description:
  // Specify whether the streamline is integrated in the upstream or
  // downstream direction.
  vtkSetClampMacro(IntegrationDirection, int, FORWARD, BOTH);
  vtkGetMacro(IntegrationDirection, int);
  void SetIntegrationDirectionToForward()
    {this->SetIntegrationDirection(FORWARD);};
  void SetIntegrationDirectionToBackward()
    {this->SetIntegrationDirection(BACKWARD);};
  void SetIntegrationDirectionToBoth()
    {this->SetIntegrationDirection(BOTH);};  

  // Description
  // Turn on/off vorticity computation at streamline points
  // (necessary for generating proper stream-ribbons using the
  // vtkRibbonFilter.
  vtkSetMacro(ComputeVorticity, bool);
  vtkGetMacro(ComputeVorticity, bool);

  // Description
  // This can be used to scale the rate with which the streamribbons
  // twist. The default is 1.
  vtkSetMacro(RotationScale, double);
  vtkGetMacro(RotationScale, double);

  // Description:
  // The object used to interpolate the velocity field during
  // integration is of the same class as this prototype.
  void SetInterpolatorPrototype(vtkInterpolatedVelocityField* ivf);

protected:

  vtkSQStreamTracer();
  ~vtkSQStreamTracer();

  // Create a default executive.
  virtual vtkExecutive* CreateDefaultExecutive();

  // hide the superclass' AddInput() from the user and the compiler
  void AddInput(vtkDataObject *) 
    { vtkErrorMacro( << "AddInput() must be called with a vtkDataSet not a vtkDataObject."); };
  
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int, vtkInformation *);

  void CalculateVorticity( vtkGenericCell* cell, double pcoords[3],
                           vtkDoubleArray* cellVectors, double vorticity[3] );
  void Integrate(vtkDataSet *input,
                 vtkPolyData* output,
                 vtkDataArray* seedSource, 
                 vtkIdList* seedIds,
                 vtkIntArray* integrationDirections,
                 double lastPoint[3],
                 vtkInterpolatedVelocityField* func,
                 int maxCellSize,
                 const char *vecFieldName,
                 double& propagation,
                 vtkIdType& numSteps);
  void SimpleIntegrate(double seed[3], 
                       double lastPoint[3], 
                       double stepSize,
                       vtkInterpolatedVelocityField* func);
  int CheckInputs(vtkInterpolatedVelocityField*& func,
                  int* maxCellSize);
  void GenerateNormals(vtkPolyData* output, double* firstNormal, const char *vecName);

  bool GenerateNormalsInIntegrate;

  // starting from global x-y-z position
  double StartPosition[3];

  static const double EPSILON;
  double TerminalSpeed;

  double LastUsedStepSize;

//BTX
  struct IntervalInformation
  {
    double Interval;
    int Unit;
  };

  double MaximumPropagation;
  double MinimumIntegrationStep;
  double MaximumIntegrationStep;
  double InitialIntegrationStep;

  void ConvertIntervals( double& step, double& minStep, double& maxStep,
                        int direction, double cellLength );
  static double ConvertToLength( double interval, int unit, double cellLength );
  static double ConvertToLength( IntervalInformation& interval, double cellLength );
  
//ETX

  int SetupOutput(vtkInformation* inInfo, 
                  vtkInformation* outInfo);
  void InitializeSeeds(vtkDataArray*& seeds,
                       vtkIdList*& seedIds,
                       vtkIntArray*& integrationDirections,
                       vtkDataSet *source);
  
  int IntegrationStepUnit;
  int IntegrationDirection;

  // Prototype showing the integrator type to be set by the user.
  vtkInitialValueProblemSolver* Integrator;

  double MaximumError;
  vtkIdType MaximumNumberOfSteps;

  bool ComputeVorticity;
  double RotationScale;

  vtkInterpolatedVelocityField* InterpolatorPrototype;

  vtkCompositeDataSet* InputData;


private:
  vtkSQStreamTracer(const vtkSQStreamTracer&);  // Not implemented.
  void operator=(const vtkSQStreamTracer&);  // Not implemented.
};


#endif


