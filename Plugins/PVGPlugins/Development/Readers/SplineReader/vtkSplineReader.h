#ifndef _vtkSplineReader_h
#define _vtkSplineReader_h
#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"

#include <vector>
#include <valarray>

typedef std::valarray<double> ValD;
typedef std::vector<double> VecD;
typedef std::vector<std::valarray<double>> VecValD;



class VTK_EXPORT vtkSplineReader : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkSplineReader *New();
  vtkTypeRevisionMacro(vtkSplineReader, vtkMultiBlockDataSetAlgorithm);
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  vtkGetMacro(NumSegments, int);
  vtkSetMacro(NumSegments, int);

  vtkGetMacro(TotalLength, double);
  vtkSetMacro(TotalLength, double);

  vtkGetMacro(DIFT, double);
  vtkSetMacro(DIFT, double);

  vtkGetMacro(PolynomialInterpolation, bool);
  vtkSetMacro(PolynomialInterpolation, bool);

protected:
  vtkSplineReader();
  ~vtkSplineReader();
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
	//global variables
	char *FileName;
	double TotalLength;
	int NumSegments;	
  double DIFT;
  bool PolynomialInterpolation;

private:
  vtkSplineReader(const vtkSplineReader&);
  void operator = (const vtkSplineReader&);

  //BTX
	ValD CosineInterpolation(ValD& startPt, ValD& endPt, double& t);
	ValD BSplineInterpolation(ValD& startPt, ValD& endPt, double& t);
  void Geo2Cos(double& azimuth, double& dip, ValD& cosines);
  void Cos2Geo(ValD& cosines, double& az, double& dip);
  void Cos2Geo_dekemp(ValD& xyz, ValD& cosines, double &az, double &dip);
  VecD PartialSumDistance(int position, int total, VecValD& xyz);
  void Convert(vtkPoints*, vtkCellArray*, vtkCellArray*, ValD&, ValD&);
  //ETX

};
#endif
