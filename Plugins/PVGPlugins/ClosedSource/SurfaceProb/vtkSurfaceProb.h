#ifndef _vtkSurfaceProb_h
#define _vtkSurfaceProb_h
#include "vtkImageAlgorithm.h"
#include "vtkMath.h"
#include <valarray>

typedef std::valarray<double> ValD;

class vtkPolyData;
class vtkIdList;

class VTK_EXPORT vtkSurfaceProb : public vtkImageAlgorithm
{
public:
  //PV standard stuff
  static vtkSurfaceProb *New();
  vtkTypeRevisionMacro(vtkSurfaceProb, vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  //Parameters
  vtkGetVector3Macro(ObsXYZ, double);
  vtkGetMacro(Dip, double);
  vtkGetMacro(Strike, double);
  vtkGetMacro(DipDirection, double);
  vtkGetMacro(MaxDownSurface, double);
  vtkGetMacro(MaxAcrossSurface, double);
  vtkGetMacro(DipError, double);
  vtkGetMacro(StrikeError, double);
  vtkGetVector3Macro(GridSpacing, double);
  vtkGetVector3Macro(GridCells, int);
  vtkGetVector3Macro(GridOrigin, double);
  vtkGetMacro(GeneratedPlane, bool);
  vtkGetMacro(DistanceMode, int);

  vtkSetVector3Macro(ObsXYZ, double);
  vtkSetMacro(Dip, double);
  vtkSetMacro(Strike, double);
  vtkSetMacro(DipDirection, double);
  vtkSetMacro(MaxDownSurface, double);
  vtkSetMacro(MaxAcrossSurface, double);
  vtkSetMacro(DipError, double);
  vtkSetMacro(StrikeError, double);
  vtkSetVector3Macro(GridSpacing, double);
  vtkSetVector3Macro(GridCells, int);
  vtkSetVector3Macro(GridOrigin, double);
  vtkSetMacro(GeneratedPlane, bool);
  vtkSetMacro(DistanceMode, int);

protected:
  //PV standard stuff
  vtkSurfaceProb();
  ~vtkSurfaceProb();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info);

  //Parameters
  double ObsXYZ[3];
  double Dip;
  double Strike;
  double DipDirection;
  double MaxDownSurface;
  double MaxAcrossSurface;
  double DipError;
  double StrikeError;
  double GridSpacing[3];
  int GridCells[3];
  double GridOrigin[3];
  bool GeneratedPlane;
  int DistanceMode;


private:
  //PV standard stuff
  vtkSurfaceProb(const vtkSurfaceProb&);
  void operator = (const vtkSurfaceProb&);

  //Statistics Functions
  double TWO_PI;
  double SQRT_TWO_PI;
  double SigmaDip(double down_line, double alpha_error)
    {
    return down_line * alpha_error;
    }
  double SigmaStrike(double S, double strike_error, double dip)
    {
    return S * strike_error * sin(fabs(dip));
    }
  double ProbChris(double x, double sigma, double c=1.0);
  double ProbMG(double x, double sigma, double c=1.0);

  //Vector Functions
  double veclen(ValD xyz)
    {
    return sqrt((xyz*xyz).sum());
    }
  ValD proj(ValD v, ValD u) // v onto u
    {
    return ( vtkMath::Dot(&u[0], &v[0]) / vtkMath::Dot(&u[0], &u[0]) ) * u;
    }
  ValD proj_unit(ValD v, ValD u)
    {
    return vtkMath::Dot(&u[0], &v[0]) * u;
    }

  ValD FindTop(vtkPolyData* input, vtkIdList* pt_ids);
  ValD FindBottom(vtkPolyData* input, vtkIdList* pt_ids);

};
#endif
