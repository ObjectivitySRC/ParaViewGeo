#include "vtkSplineReader.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkMultiBlockDataSet.h"

#include "vtkPolyData.h"
#include "vtkCardinalSpline.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkType.h"
#include "vtkMath.h"
#include "vtkCellData.h"

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <valarray>
#include <cmath>

#define COS_ALPHA 0
#define COS_BETA 1
#define COS_GAMMA 2

#define NO_ANGLE 999

#define X 0
#define Y 1
#define Z 2

#define FILE_X 0
#define FILE_Y 1
#define FILE_Z 2
#define FILE_AZ 3
#define FILE_DIP 4

vtkCxxRevisionMacro(vtkSplineReader, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkSplineReader);

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
vtkSplineReader::vtkSplineReader()
  {
  this->SetNumberOfInputPorts(0);
  this->FileName = 0;
  this->TotalLength = 100.0;
  this->NumSegments = 4;
  }

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
vtkSplineReader::~vtkSplineReader()
  {
  this->SetFileName(0);
  }

//----------------------------------------------------------------------------
// RequestData
//----------------------------------------------------------------------------
int vtkSplineReader::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
  {
  vtkInformation *out_info = outputVector->GetInformationObject(0);
  vtkMultiBlockDataSet *output = vtkMultiBlockDataSet::SafeDownCast(
    out_info->Get(vtkDataObject::DATA_OBJECT()));

  //-------------------------------------------------------------------
  //try to open the file first
  std::ifstream file;
  file.open(this->FileName);
  if(!file || file.fail())
    {
    vtkErrorMacro("File not found.");
    return 0;
    }

  //-------------------------------------------------------------------
  // each line in file is x y z azi dip otherstuff
  // if azi or dip == 999 then it's not an observation point
  // these contain the control and observation points
  VecValD surface_xyz;
  std::map<int, double> surface_az, surface_dip;
  std::vector<int> is_obs_point;
  int num_points_across = 0;
  std::string line;
  while (getline(file, line))
    {
    std::vector<std::string> v;
    std::string buf;
    std::stringstream ss(line);
    while (ss >> buf)
      {
      v.push_back(buf);
      }
    if (!v.empty())
      {
      //xyz
      ValD xyz(3);
      xyz[X] = atof(v[FILE_X].c_str());
      xyz[Y] = atof(v[FILE_Y].c_str());
      xyz[Z] = atof(v[FILE_Z].c_str());
      surface_xyz.push_back(xyz);
      //azimuth, dip
      double azimuth = atof(v[FILE_AZ].c_str());
      double dip = atof(v[FILE_DIP].c_str());
      if (azimuth != NO_ANGLE || dip != NO_ANGLE)
        {
        is_obs_point.push_back(1);
        surface_az[num_points_across] = azimuth;
        surface_dip[num_points_across] = dip;
        }
      else
        {
        is_obs_point.push_back(0);
        }
      }
    ++num_points_across;
    }
  file.close();
  int num_lines = num_points_across - 1;

  //-------------------------------------------------------------------
  // create top spline
  VecD psd = this->PartialSumDistance(0, num_points_across, surface_xyz);
  vtkPoints *top_points = vtkPoints::New();
  vtkCellArray *top_lines = vtkCellArray::New();
  double max_dist_inv = 1.0 / psd[num_lines];
  for (int i = 0; i < num_lines; ++i)
    {
    double t = psd[i] * max_dist_inv;
    ValD xyz = this->BSplineInterpolation(surface_xyz[i], surface_xyz[i + 1], t);
    top_points->InsertPoint(i, &xyz[0]);
    }
  top_lines->InsertNextCell(num_lines);
  for (int i = 0; i < num_lines; ++i)
    {
    top_lines->InsertCellPoint(i);
    }
  vtkPolyData *top_spline = vtkPolyData::New();
  top_spline->SetPoints(top_points);
  top_spline->SetLines(top_lines);
  output->SetNextBlock(top_spline);
  top_spline->Delete();
  top_lines->Delete();
  top_points->Delete();

  //-------------------------------------------------------------------
  // create projection lines
  vtkPoints *push_points = vtkPoints::New();
  vtkCellArray *push_verts = vtkCellArray::New();
  vtkCellArray *push_lines = vtkCellArray::New();

  if (!this->PolynomialInterpolation)
    {
    ValD start_angles(0.0, 3);
    ValD end_angles(0.0, 3);

    std::map<int, double>::iterator init = surface_az.begin();
    std::map<int, double>::iterator next = init;
    ++next;
    int k0 = init->first;
    for (next; next != surface_az.end(); ++next)
      {
      int k1 = next->first;
      int last_index = k1 - k0;
      psd = this->PartialSumDistance(k0, last_index + 1, surface_xyz);
      double max_dist_inv = 1.0 / psd[last_index];

      this->Geo2Cos(surface_az[k0], surface_dip[k0], start_angles);
      this->Geo2Cos(surface_az[k1], surface_dip[k1], end_angles);

      int cursor = k0;
      // for each parametric distance / for each line
      for (VecD::iterator dist = psd.begin(); dist != psd.end(); ++dist) 
        {
        //why do we want the case when cursor == 0, but not when cursor == k?
        //because the line for cursor == k has already been done by the last
        //iteration of the outer for loop
        if (cursor == 0 || cursor != k0)
          {
          double t = *dist * max_dist_inv;
          ValD cosines = this->BSplineInterpolation(start_angles, end_angles, t);
          this->Convert(push_points, push_verts, push_lines, cosines, surface_xyz[cursor]);
          }
        ++cursor;
        }
      k0 = k1;
      }
    psd.clear();
    }
  else
    {
    //-------------------------------------------------------------------
    // Polynomial Interpolation
    psd = this->PartialSumDistance(0, surface_xyz.size(), surface_xyz);
    VecD GeoD, GeoAz, GeoDip;
    for(std::map<int,double>::iterator it = surface_az.begin(); it != surface_az.end(); ++it)
      {
      GeoD.push_back(psd[it->first]);
      GeoAz.push_back(surface_az[it->first]);
      GeoDip.push_back(surface_dip[it->first]);
      }
    for (unsigned int Record=0; Record < surface_xyz.size(); ++Record)
      {
      //part 1
      unsigned int Point = 0;
      VecD Ca, Da, Cb, Db, Cg, Dg;
      for (unsigned int I = 0; I < GeoD.size(); ++I)
        {
        double DIF = abs(psd[Record] - GeoD[I]);
        if (DIF > this->DIFT)
          {
          Point = I;
          //DIF = DIFT;
          }
        ValD temp_cosines(0.0,3);
        this->Geo2Cos(GeoAz[I], GeoDip[I], temp_cosines);
        Ca.push_back(temp_cosines[COS_ALPHA]);
        Da.push_back(temp_cosines[COS_ALPHA]);
        Cb.push_back(temp_cosines[COS_BETA]);
        Db.push_back(temp_cosines[COS_BETA]);
        Cg.push_back(temp_cosines[COS_GAMMA]);
        Dg.push_back(temp_cosines[COS_GAMMA]);
        }
      ValD temp_cosines(0.0,3);
      this->Geo2Cos(GeoAz[Point], GeoDip[Point], temp_cosines);
      double new_cos_alpha = temp_cosines[COS_ALPHA];
      double new_cos_beta = temp_cosines[COS_BETA];
      double new_cos_gamma = temp_cosines[COS_GAMMA];
      double error_cos_alpha = 0;
      double error_cos_beta = 0;
      double error_cos_gamma = 0;
      if (Point > 0)
        --Point;
      //part 2
      for (unsigned int M = 1; M < GeoD.size(); ++M)
        {
        for (unsigned int I = 0; I < GeoD.size() - M; ++I)
          {
          double HO = GeoD[I] - psd[Record];
          double HP = GeoD[I + M] - psd[Record];
          double Wa = Ca[I + 1] - Da[I];
          double Wb = Cb[I + 1] - Db[I];
          double Wg = Cg[I + 1] - Dg[I];
          double DEN = HO - HP;
          if (DEN == 0)
            DEN = 0.0000001;
          double DENa = Wa / DEN;
          double DENb = Wb / DEN;
          double DENg = Wg / DEN;
          Da[I] = HP * DENa;
          Ca[I] = HO * DENa;
          Db[I] = HP * DENb;
          Cb[I] = HO * DENb;
          Dg[I] = HP * DENg;
          Cg[I] = HO * DENg;
          }
        if (2 * Point < GeoD.size() - M) //should there be that extra minus one?
          {
          error_cos_alpha = Ca[Point + 1];
          error_cos_beta = Cb[Point + 1];
          error_cos_gamma = Cg[Point + 1];
          }
        else
          {
          error_cos_alpha = Da[Point];
          error_cos_beta = Db[Point];
          error_cos_gamma = Dg[Point];
          --Point;
          }
        new_cos_alpha += error_cos_alpha;
        new_cos_beta += error_cos_beta;
        new_cos_gamma += error_cos_gamma;
        }
      //which one? cos2geo or convert?
      //this->Cos2Geo({New_CosAlpha,New_CosBeta,New_CosGamma}, az, dip)
      ValD final_cosines(0.0,3);
      final_cosines[COS_ALPHA] = new_cos_alpha;
      final_cosines[COS_BETA] = new_cos_beta;
      final_cosines[COS_GAMMA] = new_cos_gamma;
      double final_az = 0;
      double final_dip = 0;
      this->Cos2Geo_dekemp(surface_xyz[Record], final_cosines, final_az, final_dip);
      ValD other_cosines(0.0,3);
      this->Geo2Cos(final_az, final_dip, other_cosines);
      //TODO: which one is better, other_cosines or final_cosines?
      //      need to figure out why this function puts out different results than de kemp's
      this->Convert(push_points, push_verts, push_lines, final_cosines, surface_xyz[Record]);
      }
    }

  //-------------------------------------------------------------------
  //Create projected faces
  vtkCellArray *push_faces = vtkCellArray::New();
  int num_points_down = this->NumSegments + 1;
  int num_points = num_points_across * num_points_down;
  //points_per_face: The surface is made up of the points from 2 lines
  int points_per_face = 2 * num_points_down;
  for (int d = 0; d <= num_points - points_per_face; d+=num_points_down)
    {
    push_faces->InsertNextCell(points_per_face); 
    for (int i = 0; i < num_points_down; ++i)
      push_faces->InsertCellPoint(d+i);
    for (int i = points_per_face - 1; i >= num_points_down; --i)
      push_faces->InsertCellPoint(d+i);
    }

  //-------------------------------------------------------------------
  //Lines
  //add property that indicates whether it was an observation point
  vtkIntArray* vtk_is_obs_point = vtkIntArray::New();
  int iop_size = is_obs_point.size();
  int* iop = new int[iop_size];
  for (int i = 0; i < iop_size; ++i)
    {
    iop[i] = is_obs_point[i];
    }
  vtk_is_obs_point->SetName("IsObs");
  vtk_is_obs_point->SetNumberOfValues(iop_size);
  vtk_is_obs_point->SetArray(iop, iop_size, 0);
  vtkPolyData *proj_lines = vtkPolyData::New();
  proj_lines->SetPoints(push_points);
  proj_lines->SetLines(push_lines);
  proj_lines->GetCellData()->AddArray(vtk_is_obs_point);
  output->SetNextBlock(proj_lines);
  proj_lines->Delete();
  push_lines->Delete();
  //-------------------------------------------------------------------
  //Vertices
  vtkPolyData *proj_verts = vtkPolyData::New();
  proj_verts->SetPoints(push_points);
  proj_verts->SetVerts(push_verts);
  output->SetNextBlock(proj_verts);
  proj_verts->Delete();
  push_verts->Delete();
  //-------------------------------------------------------------------
  //Faces
  vtkPolyData *proj_faces = vtkPolyData::New();
  proj_faces->SetPoints(push_points);
  proj_faces->SetPolys(push_faces);
  output->SetNextBlock(proj_faces);
  proj_faces->Delete();
  push_faces->Delete();
  push_points->Delete();

  //Final Notes:
  //The de Kemp test data produces 205 points, 41 lines, and 40 faces
  return 1;
  }

//----------------------------------------------------------------------------
// CosineInterpolation
// Performs a Cosine interpolation between two values
//
// start - the first point or angles (in)
// end - the second point or angles (in)
// t - parameter between 0 and 1 (in)
//
// returns an interpolated valarray of the same size as start and end
//----------------------------------------------------------------------------
ValD vtkSplineReader::CosineInterpolation(ValD& start, ValD& end, double &t)
  {
  double m = 0.5 - 0.5 * cos(t * vtkMath::DoublePi());
  return (1.0 - m) * start + m * end;
  }

//----------------------------------------------------------------------------
// BSplineInterpolation
// Performs a B-Spline interpolation between two values
//
// start - the first point or angles (in)
// end - the second point or angles (in)
// t - parameter between 0 and 1 (in)
//
// returns an interpolated valarray of the same size as start and end
//----------------------------------------------------------------------------
ValD vtkSplineReader::BSplineInterpolation(ValD& start, ValD& end, double &t)
  {
  return (1.0 - t) * start + t * end;
  }

//----------------------------------------------------------------------------
// Geo2Cos
// Converts Azimuth and Dip to three cosines
//
// azimuth - the azimuth angle (in)
// dip - the dip angle (in)
// cosines - the 3 direction cosines (out)
//----------------------------------------------------------------------------
void vtkSplineReader::Geo2Cos(double& azimuth, double& dip, ValD& cosines)
  {
  double ac = azimuth * vtkMath::DegreesToRadians();
  double dc = dip * vtkMath::DegreesToRadians();
  cosines[COS_ALPHA] = cos(dc) * sin(ac);
  cosines[COS_BETA] = cos(dc) * cos(ac);
  cosines[COS_GAMMA] = sin(dc);
  }

//----------------------------------------------------------------------------
// Cos2Geo
// Converts three cosines to Azimuth and Dip
//
// cosines - the 3 direction cosines (in)
// az - the azimuth angle (out)
// dip - the dip angle (out)
//----------------------------------------------------------------------------
void vtkSplineReader::Cos2Geo(ValD& cosines, double &az, double &dip)
  {
  if (cosines[COS_BETA] != 0.0)
    {
    az = atan(cosines[COS_ALPHA] / cosines[COS_BETA]) * vtkMath::RadiansToDegrees();
    }
  if (az < 0.0)
    {
    az += 360.0;
    }
  dip = asin(cosines[COS_GAMMA]) * vtkMath::RadiansToDegrees();
  }

void vtkSplineReader::Cos2Geo_dekemp(ValD& xyz, ValD& cosines, double &az, double &dip)
  {
  double PUSH = 100.0;

	// Calculate Azimuth and Diplunge from Direction Cosines
  ValD xyz1 = xyz + PUSH * cosines;
	ValD xyz_dist = xyz1 - xyz;
	
	double Hd = sqrt(xyz_dist[0]*xyz_dist[0] + xyz_dist[1]*xyz_dist[1])	;
  double Gamma =  (atan2(xyz_dist[2], Hd) * 57.2958);
  dip = Gamma ;

	//#	****** A Condition  ******
  double Alpha;
  double Beta;
  double Xd = xyz_dist[0];
  double Yd = xyz_dist[1];
	if ((Xd > 0) && (Yd > 0)) 
    {
		Alpha = atan2(Xd,Yd) * 57.2958;
		Beta = atan2(Yd,Xd) * 57.2958;			
		}
	//#	****** B Condition  ******
	if ((Xd > 0) && (Yd == 0)) 
    { 
		Alpha = 90;
		Beta = 0; 	
    }
	//#	****** C Condition  ******	
	if ((Xd > 0) && (Yd < 0)) 
    {
		Alpha = 90 + (-1 * atan2(Yd,Xd) * 57.2958);
		Beta = atan2(Yd,Xd) * 57.2958; 
    }
	//#	****** D Condition  ******	
	if ((Xd == 0) && (Yd < 0)) 
    {
		Alpha = 180;
		Beta = 90;	
    }
	//#	****** E Condition  ******	
	if ((Xd < 0) && (Yd < 0)) 
    {	
		Xd = sqrt(Xd * Xd);
		Yd = sqrt(Yd * Yd);					
		Alpha =  (-1) * (90 + (atan2(Yd,Xd)* 57.2958));
		Beta = 180 - (atan2(Yd,Xd) * 57.2958);
    }
	//#	****** F Condition  ******		
	if ((Xd < 0) && (Yd == 0)) 
    {
		Alpha = 90;
		Beta = 180;
    }	
	//#	****** G Condition  ******	
	if ((Xd < 0) && (Yd > 0)) 
    {
		Xd = sqrt(Xd * Xd);
		Yd = sqrt(Yd * Yd);
		Alpha = (-1) * (90 - (atan2(Yd,Xd) * 57.2958));
		Beta =  (-1) * (180 - (atan2(Yd,Xd) * 57.2958));
    }
	//#	****** H Condition  ******	
	if ((Xd == 0) && (Yd > 0)) 
    {
		Alpha = 0;
		Beta = 90;
    }
  if (Alpha >= 0) 
    {
    az =  Alpha;
    }
  else 
    {
	  if (Beta >= 0) 
      {
      az = 90 + Beta;
      }
	  else 
      {
		  az = 360 + Alpha;
	    }
    }
  }

//----------------------------------------------------------------------------
// PartialSumDistance
// Returns the partial sum distance from the point at position to the 
// point at total-1
//
// position - the first point's position in xyz (in)
// total - the total number of points (not the same as length of xyz) (in)
// xyz - a vector of x,y,z co-ordinates. (in)
//
// returns a vector of the partial sums
//----------------------------------------------------------------------------
VecD vtkSplineReader::PartialSumDistance(int position, int total, VecValD& xyz)
  {
  VecD psd(total,0);
  psd[0] = 0;
  ValD xyz0 = xyz[position];
  for (int p = 1; p < total; ++p)
    {
    ValD xyz1 = xyz[p + position];
    double distance = sqrt(pow(xyz1 - xyz0, 2.0).sum());
    psd[p] = psd[p - 1] + distance;
    xyz0 = xyz1;
    }
  return psd;
  }

//----------------------------------------------------------------------------
// Convert
// This method creates the points and cells required to generate
// the surface geometry later. 
//
// push_points - points required to generate geometry (in/out)
// push_verts - cells required to generate vertices (in/out)
// push_lines - cells required to generate lines (in/out)
// cosines - the direction cosines for this particular line (in)
// surf_point - the point on surface that will be projected (in)
//----------------------------------------------------------------------------
void vtkSplineReader::Convert(vtkPoints* push_points, vtkCellArray* push_verts, vtkCellArray* push_lines, ValD& cosines, ValD& surf_point)
  {
  //this method 'projects' the lines
  ValD xyz0 = surf_point;

  double length = this->TotalLength / this->NumSegments;

  vtkIdType id = push_points->InsertNextPoint(&xyz0[0]);
  push_verts->InsertNextCell(1);
  push_verts->InsertNextCell(id);

  push_lines->InsertNextCell(this->NumSegments + 1);
  push_lines->InsertCellPoint(id);

  ValD segment_vector = cosines * length;
  segment_vector[Z] *= -1;

  for (int i = 1; i <= this->NumSegments; ++i)
    {
    ValD xyz1 = xyz0 + segment_vector;
    vtkIdType id2 = push_points->InsertNextPoint(&xyz1[0]);
    push_verts->InsertNextCell(1);
    push_verts->InsertCellPoint(id2);
    push_lines->InsertCellPoint(id2);
    xyz0 = xyz1;
    }
  }