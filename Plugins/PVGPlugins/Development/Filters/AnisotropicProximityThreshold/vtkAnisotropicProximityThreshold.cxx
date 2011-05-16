/*This filter is a threshold that returns objects near the input.
The distance check is ellipsoidal (as opposed to vtkProximityThreshold, which uses a sphere)
The ellipsoid can be non-axis-aligned.*/

#include "vtkAnisotropicProximityThreshold.h"
#include "vtkUnstructuredGrid.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkDoubleArray.h"
#include "vtkDataSet.h" 
#include "vtkCellData.h"
#include "vtkThreshold.h"
#include "vtkObjectFactory.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkPolyData.h"

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

vtkCxxRevisionMacro(vtkAnisotropicProximityThreshold, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkAnisotropicProximityThreshold);

//----------------------------------------------------------------------------
vtkAnisotropicProximityThreshold::vtkAnisotropicProximityThreshold()
  {
  this->Magnitude[0] = 1;
  this->Magnitude[1] = 1;
  this->Magnitude[2] = 1;
  this->AngleX = 0;
  this->AngleY = 0;
  this->AngleZ = 0;
  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(1);

  this->MetricTensor = new double[6]; 
  this->MetricTensor[0]=1.0;
  this->MetricTensor[1]=0.0;
  this->MetricTensor[2]=0.0;
  this->MetricTensor[3]=1.0;
  this->MetricTensor[4]=0.0;
  this->MetricTensor[5]=1.0;
  }

//----------------------------------------------------------------------------
vtkAnisotropicProximityThreshold::~vtkAnisotropicProximityThreshold()
  {

  }

//----------------------------------------------------------------------------
void vtkAnisotropicProximityThreshold::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
  {
  this->SetInputConnection ( 1, algOutput );
  }

//----------------------------------------------------------------------------
int vtkAnisotropicProximityThreshold::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
  {
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject ( 0 );
  vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast (
    inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  vtkDataSet *source = vtkDataSet::SafeDownCast (
    sourceInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast (
    outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  //make sure the objects are of the right type
  if ( !source )
    {
    vtkErrorMacro(<<"The Source is null");
    return 1;
    }
  else if ( !input)
    {
    vtkErrorMacro(<<"The Input is null");
    return 1;
    }

  //defines  
  const int XYZ = 3;
  const int CUBE_ARRAY_SIZE = 8;

  //point to store in the loop
  double point[XYZ];

  //point found in the source
  double foundPoint[XYZ];

  //findCell vars
  int subId = 0;

  //vars for the counting on the single property
  //for now will just be occurance counting
  int numberOfPoints = source->GetNumberOfPoints();
  double *distance = new double[numberOfPoints];

  //set all distances to -1
  for (int i=0; i < numberOfPoints; i++)
    {
    distance[i] = -1;
    }

  this->SetMetric();
  for ( int i = 0; i < source->GetNumberOfPoints(); i++ )
    {
    source->GetPoint( i, point );

    //find the nearest point in the source, to the point in the input
    vtkIdType pointId = input->FindPoint( point );
    if (pointId != -1)
      {
      //calculate out the distance between the two points
      input->GetPoint( pointId, foundPoint );

      //store the distance

      distance[i] = sqrt( this->EvaluateDist2(point, foundPoint) );
      //distance[i] = sqrt(vtkMath::Distance2BetweenPoints(point, foundPoint));
      //vtkErrorMacro(<< distance[i]);
      }
    }

  //convert distance double array to vtkDoubleArray
  vtkDoubleArray *distanceProp = vtkDoubleArray::New();
  distanceProp->SetArray(distance, numberOfPoints, 0); //we use 0 so the arrary will be deleted by paraview
  distanceProp->SetName("distance");

  //make a temp object to threshold with the distance value
  vtkPolyData *distanceObject = vtkPolyData::New();
  distanceObject->ShallowCopy(source);
  distanceObject->GetPointData()->AddArray( distanceProp );

  //now we create a threshold of the distance object
  vtkThreshold *thresh = vtkThreshold::New();
  thresh->SetInput(distanceObject);

  if (this->AllScalars == 1)
    {
    thresh->AllScalarsOn();
    }
  else
    {
    thresh->AllScalarsOff();
    }

  //tell it to use the distance array
  thresh->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, distanceProp->GetName());

  //objects between 0 and 1 are inside the ellipsoid
  if (this->Invert)
    {
    thresh->ThresholdByUpper( 1 ); // greater than or equal to 1.0
    }  
  else
    {
    thresh->ThresholdByLower( 1 ); // less than or equal to 1.0
    }

  thresh->Update();
  output->ShallowCopy ( thresh->GetOutput() );
  thresh->Delete();    

  distanceProp->Delete();
  return 1;
  }

int vtkAnisotropicProximityThreshold::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 1 );
  return 1;
  }

//----------------------------------------------------------------------------
void vtkAnisotropicProximityThreshold::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os, indent);
  }

//----------------------------------------------------------------------------
void vtkAnisotropicProximityThreshold::SetMetric(  )
  {
  //convert angles to radioans
  double *angleRads = new double[3];
  double factor = vtkMath::DoublePi() / 180.0;
  angleRads[0] = this->AngleX * factor;
  angleRads[1] = this->AngleY * factor;
  angleRads[2] = this->AngleZ * factor;

  //make rotation matrix
  double *rot = new double[16];
  this->MakeRotMatrix( angleRads[0], angleRads[1], angleRads[2], rot);
  vtkMatrix4x4 *rotMatrix = vtkMatrix4x4::New();
  rotMatrix->DeepCopy(rot);

  //make transpose of rotation matrix
  vtkMatrix4x4 *inv = vtkMatrix4x4::New();
  vtkMatrix4x4::Transpose(rotMatrix, inv);

  //make scale matrix
  vtkMatrix4x4 *sca = vtkMatrix4x4::New();
  sca->Identity();
  vtkTransform *transform = vtkTransform::New();
  transform->SetMatrix(sca);
  transform->Scale(1.0 / (this->Magnitude[0] * this->Magnitude[0]), 
                   1.0 / (this->Magnitude[1] * this->Magnitude[1]), 
                   1.0 / (this->Magnitude[2] * this->Magnitude[2]));
  vtkMatrix4x4 *scaleMatrix = transform->GetMatrix();

  //make the matrix for tensor
  vtkMatrix4x4 *temp = vtkMatrix4x4::New();
  vtkMatrix4x4 *for_tensor = vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(rotMatrix, scaleMatrix, temp);
  vtkMatrix4x4::Multiply4x4(temp, inv, for_tensor);


  this->MetricTensor[0] = for_tensor->GetElement(0,0);
  this->MetricTensor[1] = for_tensor->GetElement(0,1);
  this->MetricTensor[2] = for_tensor->GetElement(0,2);
  this->MetricTensor[3] = for_tensor->GetElement(1,1);
  this->MetricTensor[4] = for_tensor->GetElement(1,2);
  this->MetricTensor[5] = for_tensor->GetElement(2,2);
  }

void vtkAnisotropicProximityThreshold::MakeRotMatrix(double ax, double ay, double az, double *m)
  {
  double t1,t2,t4,t5,t7,t8,t9,t17 ;
  t1 = cos(az);
  t2 = cos(ay);
  t4 = sin(az);
  t5 = cos(ax);
  t7 = sin(ay);
  t8 = t1*t7;
  t9 = sin(ax);
  t17 = t4*t7;
#ifdef COLUMN_ORDER
  m[0] = t1*t2;
  m[4] = -t4*t5+t8*t9;
  m[8] = t4*t9+t8*t5;
  m[12] = 0.0 ;
  m[1] = t4*t2;
  m[5] = t1*t5+t17*t9;
  m[9] = -t1*t9+t17*t5;
  m[13] = 0.0 ;
  m[2] = -t7;
  m[6] = t2*t9;
  m[10] = t2*t5;
  m[14] = m[3] = m[7] = m[11] = 0 ;
  m[15] = 1.0 ;
#else
  m[0] = t1*t2;
  m[1] = -t4*t5+t8*t9;
  m[2] = t4*t9+t8*t5;
  m[3] = 0.0 ;
  m[4] = t4*t2;
  m[5] = t1*t5+t17*t9;
  m[6] = -t1*t9+t17*t5;
  m[7] = 0.0 ;
  m[8] = -t7;
  m[9] = t2*t9;
  m[10] = t2*t5;
  m[11] = m[12] = m[13] = m[14] = 0 ;
  m[15] = 1.0 ;
#endif
  }

//----------------------------------------------------------------------------
double  vtkAnisotropicProximityThreshold::EvaluateDist2( double* P, double* Q)
  {
  double dx = Q[0] - P[0];
  double dy = Q[1] - P[1];
  double dz = Q[2] - P[2];
  double* met = this->MetricTensor;

  double ret = met[0]*dx*dx + met[3]*dy*dy + met[5]*dz*dz 
    + 2.* ( met[1]*dx*dy + met[2]*dx*dz + met[4]*dy*dz );

  return ret;
  }