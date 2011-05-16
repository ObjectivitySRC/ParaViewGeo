/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkCompositing.h $
  Author:    Arolde VIDJINNAGNI	     
  Date:      Febrary 04, 2009
  Version:   0.1

=========================================================================*/
// .NAME vtkThresholdProcessId - Extract cell or point scalars to the processor rank.
//
// .SECTION Description
// vtkThresholdProcessId is meant to display which processor owns which cells
// and points.  It is useful for visualizing the partitioning for
// streaming or distributed pipelines.
//
// .SECTION See Also
// vtkPolyDataStreamer

#ifndef __vtkThresholdProcessId_h
#define __vtkThresholdProcessId_h

#include "vtkDataSetAlgorithm.h"

class vtkFloatArray;
class vtkIntArray;
class vtkMultiProcessController;

class VTK_EXPORT vtkThresholdProcessId : public vtkDataSetAlgorithm
{
public:

  static vtkThresholdProcessId *New();

  vtkTypeRevisionMacro(vtkThresholdProcessId,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

 
  

  // Description:
  // By defualt this filter uses the global controller,
  // but this method can be used to set another instead.
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  
  vtkSetMacro(ProccesId, int);
 // vtkGetMacro(ProccesId, int);

  vtkSetMacro(OutData, int);
 // vtkGetMacro(OutData, int);

   vtkSetMacro(WholeOutput, int);
 // vtkGetMacro(WholeOutput, int);
  
protected:
  vtkThresholdProcessId();
  ~vtkThresholdProcessId();
  
   int RequestData(
    vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
  vtkIntArray *MakeProcessIdScalars(int processid, vtkIdType numCells);

  int OutData;
  int ProccesId;	
  int WholeOutput;
  vtkMultiProcessController* Controller;

private:
  vtkThresholdProcessId(const vtkThresholdProcessId&);  // Not implemented.
  void operator=(const vtkThresholdProcessId&);  // Not implemented.
};

#endif