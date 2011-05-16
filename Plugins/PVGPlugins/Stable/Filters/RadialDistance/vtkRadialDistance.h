/*=========================================================================

   Program:   Visualization Toolkit
  Module:    $ vtkRadialDistance.h $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      August 29, 2008
  Version:   1




=========================================================================*/


#ifndef __vtkRadialDistance_h
#define __vtkRadialDistance_h

#include "vtkDataSetAlgorithm.h"

class VTK_EXPORT vtkRadialDistance : public vtkDataSetAlgorithm
{
public:
  static vtkRadialDistance *New();
  vtkTypeRevisionMacro(vtkRadialDistance,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
        
  //setup macros 
  vtkSetVector3Macro(MeasurePoint,double);
  vtkGetVector3Macro(MeasurePoint,double);
  
  vtkSetClampMacro(StepNumber, int,1,1000000000);
  vtkGetMacro(StepNumber, int);
      
protected:   
  vtkRadialDistance();  
  ~vtkRadialDistance(); 
  
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);      
  
  double MeasurePoint[3];
  int StepNumber;
  
private:
    
  vtkRadialDistance(const vtkRadialDistance&);  // Not implemented.
  void operator=(const vtkRadialDistance&);  // Not implemented.
};


#endif
