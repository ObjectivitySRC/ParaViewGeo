#ifndef _vtkAnisotropicProximityThreshold_h
#define _vtkAnisotropicProximityThreshold_h

#include "vtkUnstructuredGridAlgorithm.h"

class VTK_EXPORT vtkAnisotropicProximityThreshold : public vtkUnstructuredGridAlgorithm
  {
  public:
    static vtkAnisotropicProximityThreshold *New();
    vtkTypeRevisionMacro(vtkAnisotropicProximityThreshold,vtkUnstructuredGridAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Specify the point locations
    void SetSourceConnection(vtkAlgorithmOutput* algOutput);  

    vtkSetVector3Macro(Magnitude, double);
    vtkGetVector3Macro(Magnitude, double);

    vtkSetMacro(AngleX, double);
    vtkGetMacro(AngleX, double);
    vtkSetMacro(AngleY, double);
    vtkGetMacro(AngleY, double);
    vtkSetMacro(AngleZ, double);
    vtkGetMacro(AngleZ, double);

    vtkSetMacro(Invert,int);
    vtkGetMacro(Invert, int);

    vtkSetMacro(AllScalars,int);
    vtkGetMacro(AllScalars, int);

    

  protected:
    vtkAnisotropicProximityThreshold();
    ~vtkAnisotropicProximityThreshold();

    virtual int RequestData(vtkInformation *, 
      vtkInformationVector **, 
      vtkInformationVector *);

    virtual int FillInputPortInformation(int port, vtkInformation* info);

    double Magnitude[3];
    double AngleX;
    double AngleY;
    double AngleZ;
    int Invert;
    int AllScalars;

  private:
    vtkAnisotropicProximityThreshold(const vtkAnisotropicProximityThreshold&);
    void operator = (const vtkAnisotropicProximityThreshold&);

    //Set/Get the metric tensor
    void SetMetric(  );

    // Compute the anisotropic squared distance between 2 points
    double EvaluateDist2( double* P, double* Q );
    void MakeRotMatrix(double ax, double ay, double az, double *m);

    double* MetricTensor;
  };
#endif
