#ifndef _vtkIsotropicResourceClassifier_h
#define _vtkIsotropicResourceClassifier_h
#include "vtkPolyDataAlgorithm.h"
class VTK_EXPORT vtkIsotropicResourceClassifier : public vtkPolyDataAlgorithm
{
public:
  static vtkIsotropicResourceClassifier *New();
  vtkTypeRevisionMacro(vtkIsotropicResourceClassifier,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkSetMacro(DistanceMeasured,double);
  vtkGetMacro(DistanceMeasured,double);
  vtkSetMacro(DistanceIndicated,double);
  vtkGetMacro(DistanceIndicated,double);
  vtkSetMacro(DistanceInferred,double);
  vtkGetMacro(DistanceInferred,double);
  
  // Description:
  // Get a pointer to the source object.
  vtkDataObject *GetSource();

  // Description:
  // Specify the point locations
  void SetSourceConnection(vtkAlgorithmOutput* algOutput);

protected:
  vtkIsotropicResourceClassifier();
  //~vtkIsotropicResourceClassifier();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  double DistanceMeasured;
  double DistanceIndicated;
  double DistanceInferred;

  

private:
  vtkIsotropicResourceClassifier(const vtkIsotropicResourceClassifier&);
  void operator = (const vtkIsotropicResourceClassifier&);
};
#endif
