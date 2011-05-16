/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkRelativeTransform.cxx $
  Author:    Chris Cameron
             MIRARCO Mining Innovation
  Date:      August 29, 2008
  Version:   0.1

  =========================================================================*/

// .NAME vtkRelativeTransform - Performs a relative transform filter.
// .SECTION Description
// vtkRelativeTransform transforms a dataset about its centroid instead of
// about (0,0,0) origin. This allows for rotations about the centroid, as well
// as relative translations. In this way, multiple relative transforms are
// capable of translating data in increments by applying the same transform.

#ifndef __vtkRelativeTransform_h
#define __vtkRelativeTransform_h

#include "vtkDataSetAlgorithm.h"

class VTK_EXPORT vtkRelativeTransform : public vtkDataSetAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkRelativeTransform, vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Constructor
  static vtkRelativeTransform* New();
 
  // Description:
  vtkGetVector3Macro(Trans, double);
  vtkSetVector3Macro(Trans, double);
  
  vtkGetVector3Macro(Rot, double);
  vtkSetVector3Macro(Rot, double);

  vtkGetVector3Macro(Scale, double);
  vtkSetVector3Macro(Scale, double);

	
  vtkGetVector3Macro(Origin, double);
  vtkSetVector3Macro(Origin, double);

protected:
  vtkRelativeTransform();
  ~vtkRelativeTransform() {};

	virtual int RequestInformation(vtkInformation *, 
                           vtkInformationVector **, 
                           vtkInformationVector *);

  virtual int RequestData (vtkInformation *, 
                           vtkInformationVector **, 
                           vtkInformationVector *);

  double Trans[3];
  double Scale[3];
  double Rot[3];
	double Origin[3];

private:

  //BTX
  vtkRelativeTransform(const vtkRelativeTransform&);  // Not implemented.
  void operator=(const vtkRelativeTransform&);  // Not implemented.
  //ETX
};

#endif

