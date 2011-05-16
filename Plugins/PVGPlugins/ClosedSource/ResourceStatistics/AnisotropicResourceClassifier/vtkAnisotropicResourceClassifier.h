#ifndef _vtkAnisotropicResourceClassifier_h
#define _vtkAnisotropicResourceClassifier_h
#include "vtkPolyDataAlgorithm.h"
class VTK_EXPORT vtkAnisotropicResourceClassifier : public vtkPolyDataAlgorithm
{
public:
  static vtkAnisotropicResourceClassifier *New();
  vtkTypeRevisionMacro(vtkAnisotropicResourceClassifier,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetVector3Macro(Magnitude, double);
  vtkGetVector3Macro(Magnitude, double);

  vtkSetMacro(AngleX,double);
  vtkGetMacro(AngleX,double);
  
  vtkSetMacro(AngleY,double);
  vtkGetMacro(AngleY,double);

  vtkSetMacro(AngleZ,double);
  vtkGetMacro(AngleZ,double);

  vtkSetMacro(FactorIndicated,double);
  vtkGetMacro(FactorIndicated,double);

  vtkSetMacro(FactorInferred,double);
  vtkGetMacro(FactorInferred,double);
  
  // Description:
  // Get a pointer to the source object.
  vtkDataObject *GetSource();

  // Description:
  // Specify the point locations
  void SetSourceConnection(vtkAlgorithmOutput* algOutput);

protected:
  vtkAnisotropicResourceClassifier();
  ~vtkAnisotropicResourceClassifier();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  double Magnitude[3];
  double AngleX;
  double AngleY;
  double AngleZ;
  double FactorIndicated;
  double FactorInferred;
  

private:
  vtkAnisotropicResourceClassifier(const vtkAnisotropicResourceClassifier&);
  void operator = (const vtkAnisotropicResourceClassifier&);
};
#endif
