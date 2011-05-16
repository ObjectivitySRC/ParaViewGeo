/*This filter implements a resource classifier using a spherical distance check.

*/

#include "vtkIsotropicResourceClassifier.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkExecutive.h"
#include "vtkProximityThreshold.h"
#include "vtkCellData.h"
#include "vtkIntArray.h"
#include "vtkThreshold.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"

// Included instead of iostream to remove OS dependency
#include <vtksys/ios/sstream>

vtkCxxRevisionMacro(vtkIsotropicResourceClassifier, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkIsotropicResourceClassifier);

vtkIsotropicResourceClassifier::vtkIsotropicResourceClassifier()
  {
  this->DistanceMeasured = 50.0;
  this->DistanceIndicated = 100.0;
  this->DistanceInferred = 150.0;
  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(1);
  }


//----------------------------------------------------------------------------
void vtkIsotropicResourceClassifier::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
  {
  this->SetInputConnection ( 1, algOutput );
  }

//----------------------------------------------------------------------------
vtkDataObject* vtkIsotropicResourceClassifier::GetSource()
  {
  if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
    {
    return NULL;
    }

  return this->GetExecutive()->GetInputData ( 1, 0 );
  }


int vtkIsotropicResourceClassifier::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
  {
  //typical useless six lines just to start this damn method
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0); 
  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT())); 
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0); 
  vtkPolyData *source_ = vtkPolyData::SafeDownCast ( sourceInfo->Get ( vtkDataObject::DATA_OBJECT() ) ); 
  vtkInformation *outInfo = outputVector->GetInformationObject(0); 
  vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));

  //indices
  int SEMI_MEASURED = -2;
  int MEASURED = 2;
  int INDICATED = 1;
  int INFERRED = 0;
  int UNTESTED = -1;

  //init some counts and arrays
  int line_count = input->GetNumberOfLines();
  int point_count = source_->GetNumberOfPoints();
  int *classification = new int[point_count];
  int *point_ids_arr = new int[point_count];
  for (int i = 0; i < point_count; i++)
    {
    classification[i] = UNTESTED;
    point_ids_arr[i] = i;
    }

  //add an identifier to the points/grid
  vtkIntArray *point_ids = vtkIntArray::New();
  point_ids->SetArray(point_ids_arr,point_count,0); //we use 0 so the arrary will be deleted by paraview
  point_ids->SetName("PointIds");
  vtkPolyData *source = vtkPolyData::New();
  source->DeepCopy(source_);
  source->GetPointData()->AddArray( point_ids );


  // get the hole ids
  vtkIntArray *ids = vtkIntArray::SafeDownCast(input->GetCellData()->GetArray("Id"));

  for (int i = 0; i < line_count; i++)
    {
    // get a drillhole
    int id = ids->GetValue(i);
    vtkThreshold *thresh = vtkThreshold::New();
    thresh->SetInput(input);
    thresh->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS, "Id");
    thresh->ThresholdBetween(id,id);
    thresh->Update();
    vtkUnstructuredGrid *one_hole = thresh->GetOutput();

    for (int j = 1; j < 4; j++)
      {
      //Find all grid points near the drillhole
      vtkProximityThreshold *prox = vtkProximityThreshold::New();
      prox->SetAllScalars(1);
      if (j == 1)
        {
        prox->SetDistance(this->DistanceMeasured);
        }
      else if(j == 2)
        {
        prox->SetDistance(this->DistanceIndicated);
        }
      else if (j == 3)
        {
        prox->SetDistance(this->DistanceInferred);
        }
      prox->SetInput(0, one_hole);
      prox->SetInput(1, source);
      prox->Update();

      // mark the classification array
      vtkUnstructuredGrid *points_near_hole = prox->GetOutput();
      vtkPoints *near_points = points_near_hole->GetPoints();
      vtkIntArray *near_point_ids = vtkIntArray::SafeDownCast(points_near_hole->GetPointData()->GetArray("PointIds"));
      int num_near_points = near_points->GetNumberOfPoints();
      for (int k = 0; k < num_near_points; k++)
        {
        // find out the id of the cell
        int near_id = near_point_ids->GetValue(k);
        // change the classification
        int old_class = classification[near_id];
        if (j == 1)
          {
          //try marking it measured or semi-measured
          if (old_class == SEMI_MEASURED)
            {
            classification[near_id] = MEASURED;
            }
          else if (old_class == INDICATED || old_class == INFERRED || old_class == UNTESTED)
            {
            classification[near_id] = SEMI_MEASURED;
            }
          }
        else if(j == 2)
          {
          //try marking it indicated
          //ignore semi-measured, because at the end we'll turn semi-measured into indicated
          if (old_class == INFERRED || old_class == UNTESTED)
            {
            classification[near_id] = INDICATED;
            }

          }
        else if (j == 3)
          {
          //try marking it inferred
          if (old_class == UNTESTED)
            {
            classification[near_id] = INFERRED;
            }
          
          }


        }
      }
    }

  //determine how many blocks are measured, indicated, inferred, and untested
  int num_measured = 0;
  int num_indicated = 0;
  int num_inferred = 0;
  int num_untested = 0;
  for (int i = 0; i < point_count; i++)
    {
    if (classification[i] == MEASURED)
      {
      num_measured++;
      }
    else if (classification[i] == SEMI_MEASURED)
      {
      classification[i] = INDICATED;
      num_indicated++;
      }
    else if (classification[i] == INDICATED)
      {
      num_indicated++;
      }
    else if (classification[i] == INFERRED)
      {
      num_inferred++;
      }
    else if (classification[i] == UNTESTED)
      {
      num_untested++;
      }

    }

  //report the measured, indicated, inferred and untested counts
  //TODO: vtkWarningMacro is not a desirable way to display this information to the user
  vtkstd::stringstream ss;
  ss << "Measured: " << num_measured << endl
    << "Indicated: " << num_indicated << endl
    << "Inferred: " << num_inferred << endl
    << "Untested: " << num_untested << endl;
  vtkWarningMacro(<<ss.str().c_str());

  //Add the classification array to the output
  vtkIntArray *cls = vtkIntArray::New();
  cls->SetArray(classification,point_count, 0); //we use 0 so the arrary will be deleted by paraview
  cls->SetName("Classification");
  source->GetPointData()->AddArray( cls );

  //create the output object and delete source (copy of actual source)
  output->ShallowCopy(source);
  source->Delete();

  return 1;
  }


void vtkIsotropicResourceClassifier::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os, indent);
  os << indent << "DistanceMeasured" << this->GetDistanceMeasured() << endl;
  os << indent << "DistanceIndicated" << this->GetDistanceIndicated() << endl;
  os << indent << "DistanceInferred" << this->GetDistanceInferred() << endl;
  }

