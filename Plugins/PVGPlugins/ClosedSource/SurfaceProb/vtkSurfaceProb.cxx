#include "vtkSurfaceProb.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkImageAlgorithm.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPolyData.h"
#include "vtkPointLocator.h"
#include "vtkIdList.h"
#include "vtkGenericCell.h"
#include "vtkCellData.h"

//----------------------------------------------------------------------------
#include <valarray>
#include <cmath>

#define FROM_TOP 0
#define AT_DEPTH 1

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkSurfaceProb, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkSurfaceProb);

//----------------------------------------------------------------------------
vtkSurfaceProb::vtkSurfaceProb()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
  this->TWO_PI = 2 * vtkMath::DoublePi();
  this->SQRT_TWO_PI = sqrt(this->TWO_PI);
}

//----------------------------------------------------------------------------
vtkSurfaceProb::~vtkSurfaceProb()
  {
  }

//----------------------------------------------------------------------------
void vtkSurfaceProb::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os,indent);
  }

//----------------------------------------------------------------------------
int vtkSurfaceProb::FillInputPortInformation( int vtkNotUsed(port), vtkInformation* info)
  {
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
  }

//----------------------------------------------------------------------------
int vtkSurfaceProb::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
  {
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
  return 1;
  }

//----------------------------------------------------------------------------
int vtkSurfaceProb::RequestInformation(vtkInformation *vtkNotUsed(request),vtkInformationVector **inputVector,vtkInformationVector *outputVector)   
  {
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  //set the extents - you have to do this here or PV will not remember the extents on state save/load
  int whole[6];
  whole[0] = whole[2] = whole[4]=0;
  whole[1] = this->GridCells[0];
  whole[3] = this->GridCells[1];
  whole[5] = this->GridCells[2];
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),whole,6);
  return 1;
  }

//----------------------------------------------------------------------------
int vtkSurfaceProb::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
  {
  // get the output ready
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkImageData *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  //create s0 and s1
  ValD O(this->ObsXYZ, 3);
  double across = this->MaxAcrossSurface / 2.0;
  double cos_strike = cos(vtkMath::DoubleDegreesToRadians() * this->Strike);
  double sin_strike = sin(vtkMath::DoubleDegreesToRadians() * this->Strike);
  ValD cs0(3); cs0[0] = cos_strike; cs0[1] = sin_strike;
  ValD s0 = O - across * cs0;
  ValD s1 = O + across * cs0;

  //create s2
  double dip = vtkMath::DoubleDegreesToRadians() * this->Dip;
  double cos_dip = cos(dip);
  double sin_dip = sin(dip);
  double cos_dip_dir = cos(vtkMath::DoubleDegreesToRadians() * this->DipDirection);
  double sin_dip_dir = sin(vtkMath::DoubleDegreesToRadians() * this->DipDirection);
  ValD dip_direction(3); dip_direction[0] = cos_dip_dir; dip_direction[1] = sin_dip_dir;
  dip_direction *= cos_dip;
  ValD unit_down_surface(3); unit_down_surface[2] = 1.0;
  unit_down_surface *= sin_dip;
  unit_down_surface += dip_direction;
  unit_down_surface /= vtkMath::Norm(&unit_down_surface[0]);
  ValD s2 = s0 + this->MaxDownSurface * unit_down_surface;

  //setup the output grid
  output->SetDimensions(this->GridCells[0], this->GridCells[1], this->GridCells[2]);
  output->SetOrigin(this->GridOrigin);
  output->SetSpacing(this->GridSpacing);

  //get the extents from the pipeline - you should have set this in RequestInformation
  int *ext = outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
  output->SetWholeExtent(ext);
  output->SetExtent(ext);
  output->SetUpdateExtent(ext);
  output->AllocateScalars();

  //allocate prob arrays
  int num_points = output->GetNumberOfPoints();
  double* prob = new double[num_points];
  double* stddev = new double[num_points];
  for (int i = 0; i < num_points; ++i)
    {
    prob[i] = 0;
    stddev[i] = 999999999;
    }

  //convert the dip and strike errors to radians
  double dip_angle_delta = vtkMath::DoubleDegreesToRadians() * this->DipError;
  double strike_angle_delta = vtkMath::DoubleDegreesToRadians() * this->StrikeError;

  //only GeneratedPlane needs these lines
  ValD s0s1(3);
  ValD s0s2(3);
  if (this->GeneratedPlane)
    {
    s0s1 = s1 - s0;
    s0s1 /= vtkMath::Norm(&s0s1[0]);
    s0s2 = s2 - s0;
    s0s2 /= vtkMath::Norm(&s0s2[0]);
    }
  
  //If we are using de Kemp's input, construct a point locator for it
  vtkPointLocator* locator;
  if (!this->GeneratedPlane)
    {
    locator = vtkPointLocator::New();
    locator->SetDataSet(input);
    }

  //BEGIN MAIN LOOP
  for (int i = 0; i < num_points; ++i)
    {
    ValD g0(3);
    output->GetPoint(i, &g0[0]);

    ValD p1(3);
    ValD g1(3);
    if (this->GeneratedPlane)
      {
      //project point g0 onto s0s1
      p1 = s0 + this->proj_unit(g0 - s0, s0s1);

      //project point g0 onto s0s2
      ValD p2 = s0 + this->proj_unit(g0 - s0, s0s2);

      //create point g1
      g1 = s0 + (p1 - s0) + (p2 - s0);
      }
    else
      {
      //First, find g1 as the closest point to g0 in the dataset.
      vtkIdType g1_id = locator->FindClosestPoint(&g0[0]);
      input->GetPoint(g1_id, &g1[0]);

      //Next, find p1 such that it is the closest "highest" point that 
      //is connected to a triangle with the same normal as the triangle 
      //that g1 is connected to.
      vtkIdList* cell_ids = vtkIdList::New();
      input->GetPointCells(g1_id, cell_ids);
      //just use the first cell you find, points should only be connected to one line
      vtkIdType cell_id = cell_ids->GetId(0);
      //now figure out which points are in this cell
      vtkIdList* pt_ids = vtkIdList::New();
      input->GetCellPoints(cell_id, pt_ids);

      //now find the highest point in the cell
      vtkIdType pt_id = pt_ids->GetId(0);
      input->GetPoint(pt_id, &p1[0]);
      p1 = this->FindTop(input, pt_ids);
      cell_ids->Delete();
      pt_ids->Delete();
      }
    
    //calculate distance d = |g0-g1|
    double d = this->veclen(g0 - g1);

    //calculate distance L (down-surface or dip distance)
    //note: I add 0.00001 so that in the prob functions I don't have to
    //      check for sigma == 0, which speeds things up
    double L = veclen(p1 - g1) + 0.00001;

    //calculate distance S (across-surface or strike distance)
    double S = 0;
    if (this->GeneratedPlane)
      {
      //note: I add 0.00001 so that in the prob functions I don't have to
      //      check for sigma == 0, which speeds things up
      S = veclen(O - p1) + 0.00001;
      }
    else
      {
      //get a reference to the property that tells us whether a line is an observation line or not
      vtkIntArray* props = vtkIntArray::SafeDownCast(input->GetCellData()->GetArray("IsObs"));

      //find the line id of the current projection line
      vtkIdType p1_id = input->FindPoint(&p1[0]);
      vtkIdList* cell_ids = vtkIdList::New();
      input->GetPointCells(p1_id, cell_ids);
      vtkIdType line_id = cell_ids->GetId(0);
      
      //if this line is already an observation line, then the distance is 0
      if (props->GetValue(line_id))
        {
        S = 0;
        }
      //else, find closest obs point. S = distance to it
      else
        {
        vtkIdList* obs_lines = vtkIdList::New();
        for (vtkIdType j = 0; j < props->GetNumberOfTuples(); ++j)
          {
          if (props->GetValue(j))
            {
            obs_lines->InsertNextId(j);
            }
          }
        //arbitrarily high number
        S = 999999999;
        //for each line, determine if it is the closest, and set S to the closest distance
        for (int j = 0; j < obs_lines->GetNumberOfIds(); ++j)
          {
          vtkIdList* pt_ids = vtkIdList::New();
          input->GetCellPoints(obs_lines->GetId(j), pt_ids); 
          ValD top = this->FindTop(input, pt_ids);
          //if distance mode is FROM_TOP then we want the distance between the top of both lines
          if (this->DistanceMode == FROM_TOP)
            {
            S = std::min(S, this->veclen(p1-top));
            }
          //if the distance mode is AT_DEPTH then we want the distance between the projection points
          else if(this->DistanceMode == AT_DEPTH)
            {
            ValD bottom = this->FindBottom(input, pt_ids);
            ValD on = bottom + this->proj(g1-bottom, top-bottom);
            S = std::min(S, this->veclen(g1-on));
            }
          pt_ids->Delete();
          }
        obs_lines->Delete();
        }    
      cell_ids->Delete();
      }

    //if the two distances are within the bounds we set for them
    if ( (S <= across) && (L <= this->MaxDownSurface) )
      {
      //calculate sigmas and probabilities
      double sigma = this->SigmaDip(L, dip_angle_delta) + this->SigmaStrike(S, strike_angle_delta, dip);
      prob[i] = this->ProbChris(d, sigma);
      stddev[i] = d / sigma;
      }
    } // END MAIN LOOP

  //cleanup
  if (!this->GeneratedPlane)
    {
    locator->Delete();
    }

  //10. Prepare output arrays (the 0 on SetArray means don't save the original array)
  vtkDoubleArray* vtk_prob = vtkDoubleArray::New();
  vtk_prob->SetName("Probability_Density");
  vtk_prob->SetNumberOfValues(num_points);
  vtk_prob->SetArray(&prob[0], num_points, 0);
  vtkDoubleArray* vtk_stddev = vtkDoubleArray::New();
  vtk_stddev->SetName("Standard_Normal_Deviate");
  vtk_stddev->SetNumberOfValues(num_points);
  vtk_stddev->SetArray(&stddev[0], num_points, 0);

  //add arrays to output
  output->GetPointData()->AddArray(vtk_prob);
  output->GetPointData()->AddArray(vtk_stddev);

  return 1;
  }


//----------------------------------------------------------------------------
// ProbChris - Chris's Probability Function (thanks Wikipedia)
double vtkSurfaceProb::ProbChris(double x, double sigma, double c)
  {
  return (c / (sigma * this->SQRT_TWO_PI)) * exp(-0.5 * pow(x, 2) * pow(sigma, -2));
  }

//----------------------------------------------------------------------------
// ProbMG - Marie-Gabrielle's Probability function
double vtkSurfaceProb::ProbMG(double x, double sigma, double c)
  {
  return (c / (sigma * this->TWO_PI)) * exp(-0.5 * (pow(x, 2.0)) / sigma);
  }

//----------------------------------------------------------------------------
// FindTop - Looks for the highest point (z-axis) among the points listed
// in pt_ids. 
// Returns the x,y,z point in valarray goodness
ValD vtkSurfaceProb::FindTop(vtkPolyData* input, vtkIdList* pt_ids)
  {   
  vtkIdType pt_id = pt_ids->GetId(0);
  ValD top(3);
  input->GetPoint(pt_id, &top[0]);
  for (int k = 1; k < pt_ids->GetNumberOfIds(); ++k)
    {
    pt_id = pt_ids->GetId(k);
    ValD candidate(3);
    input->GetPoint(pt_id, &candidate[0]);
    if (candidate[2] > top[2])
      top = candidate;
    }
  return top;
  }

//----------------------------------------------------------------------------
// FindBottom - Looks for the lowest point (z-axis) among the points listed
// in pt_ids. 
// Returns the x,y,z point in valarray goodness
ValD vtkSurfaceProb::FindBottom(vtkPolyData* input, vtkIdList* pt_ids)
  {   
  vtkIdType pt_id = pt_ids->GetId(0);
  ValD bottom(3);
  input->GetPoint(pt_id, &bottom[0]);
  for (int k = 1; k < pt_ids->GetNumberOfIds(); ++k)
    {
    pt_id = pt_ids->GetId(k);
    ValD candidate(3);
    input->GetPoint(pt_id, &candidate[0]);
    if (candidate[2] < bottom[2])
      bottom = candidate;
    }
  return bottom;
  }