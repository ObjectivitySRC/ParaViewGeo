#ifndef _VTK_MSC_DH_GENERATOR_H
#define _VTK_MSC_DH_GENERATOR_H

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"



class VTK_EXPORT vtkProximityToLines : public vtkPolyDataAlgorithm
{
public:
  static vtkProximityToLines *New();
  vtkTypeRevisionMacro(vtkProximityToLines,vtkPolyDataAlgorithm);

  /**
    This function is needed because we accept an Input and a Source in the filter.

    We set the source connection to be port 1. (Input is port 0)
  */
  void SetSourceConnection(vtkAlgorithmOutput* algOutput);

	vtkSetMacro(NClosestLines, int);
	vtkGetMacro(NClosestLines, int);


protected:

  vtkProximityToLines();
  ~vtkProximityToLines();



  virtual int RequestData(vtkInformation *, vtkInformationVector **, 
		vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);
  
  vtkPolyData* input;
  vtkPolyData* source;
	vtkPolyData* output;

	int NClosestLines;

private:
  vtkProximityToLines(const vtkProximityToLines&);  // Not implemented.
  void operator=(const vtkProximityToLines&);  // Not implemented.

};

#endif
