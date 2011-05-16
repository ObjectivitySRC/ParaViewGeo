/*=========================================================================

   Program:   Visualization Toolkit
  Module:    $ vtkProximityThreshold.h $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      June 16 2008
  Version:   0.1




=========================================================================*/


#ifndef __vtkProximityThreshold_h
#define __vtkProximityThreshold_h

#include "vtkUnstructuredGridAlgorithm.h"

class VTK_EXPORT vtkProximityThreshold : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkProximityThreshold *New();
  vtkTypeRevisionMacro(vtkProximityThreshold,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
      
  // Description:
  // Specify the point locations
  void SetSourceConnection(vtkAlgorithmOutput* algOutput);  
  
  //setup macros 
  vtkSetMacro(Invert,int);
  vtkGetMacro(Invert, int);
  
  vtkSetMacro(AllScalars,int);
  vtkGetMacro(AllScalars, int);
    
  vtkSetMacro(Distance,double);
  vtkGetMacro(Distance,double);   
  
protected:
  int AllScalars;
  int    Invert;
  double Distance;
 
  vtkProximityThreshold();  
  ~vtkProximityThreshold(); 
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);    
  virtual int FillInputPortInformation(int port, vtkInformation* info);
private:
  virtual char* CreateName(vtkDataArray* propertyArray, char* name);
  vtkProximityThreshold(const vtkProximityThreshold&);  // Not implemented.
  void operator=(const vtkProximityThreshold&);  // Not implemented.
};


#endif
