/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkCompositing.h $
  Author:    Arolde VIDJINNAGNI	     
  Date:      Febrary 04, 2009
  Version:   0.1

=========================================================================*/
// .NAME vtkPVCollectDataSet - Extract cell or point scalars to the processor rank.
//
// .SECTION Description
// vtkPVCollectDataSet is
//
// .SECTION See Also

#ifndef __vtkPVCollectDataSet_h
#define __vtkPVCollectDataSet_h

#include "vtkDataSetAlgorithm.h"

class vtkFloatArray;
class vtkIntArray;
class vtkMultiProcessController;

class VTK_EXPORT vtkPVCollectDataSet : public vtkDataSetAlgorithm
{
public:

  static vtkPVCollectDataSet *New();

  vtkTypeRevisionMacro(vtkPVCollectDataSet,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

 
   // Description:
  vtkSetMacro(PassThrough, int);
  vtkGetMacro(PassThrough, int);

  // Description:
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  
protected:
  vtkPVCollectDataSet();
  ~vtkPVCollectDataSet();
  
  // Append the pieces.
  int RequestData(
    vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
  vtkMultiProcessController* Controller;

  int PassThrough;

private:
  vtkPVCollectDataSet(const vtkPVCollectDataSet&);  // Not implemented.
  void operator=(const vtkPVCollectDataSet&);  // Not implemented.
};

#endif