/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkDistanceCellsToDrillhole.cxx $
  Author:    Arolde VIDJINNAGNI	    
  Date:      MIRARCO May 08, 2009 
  Version:   0.2
 =========================================================================*/


#ifndef __vtkPVDistanceCellsToDrillhole_h
#define __vtkPVDistanceCellsToDrillhole_h
#include "vtkSetGet.h"
#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkDistanceCellsToDrillhole.h"
class vtkMultiProcessController;

class VTK_EXPORT vtkPVDistanceCellsToDrillhole : public vtkDistanceCellsToDrillhole
{

public:
	
	static vtkPVDistanceCellsToDrillhole *New();

    vtkTypeRevisionMacro(vtkPVDistanceCellsToDrillhole,vtkDistanceCellsToDrillhole);
   
   // Description:
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
 

   vtkSetMacro(ComputeDistanceTo, int);
   vtkSetMacro(NbClosestDrillholes, int); 
   vtkSetMacro(MinimumDistance, double); 

   vtkSetStringMacro(FileName);
   vtkGetStringMacro(FileName);



	// Description:
   // Specify the point locations used to probe input. Any geometry
   // can be used. New style. Equivalent to SetInputConnection(1, algOutput).
   void SetSourceConnection(vtkAlgorithmOutput* algOutput);



protected:
   vtkPVDistanceCellsToDrillhole();
   ~vtkPVDistanceCellsToDrillhole();

   int RequestInformation( vtkInformation *request, 
						vtkInformationVector **inputVector, 
						 vtkInformationVector *outputVector);

   virtual int RequestData(vtkInformation *request,
						   vtkInformationVector **inputVector,
						   vtkInformationVector *outputVector);

   
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info); 
   
  void ComputeCenters(vtkUnstructuredGrid* dataset);

  
  vtkUnstructuredGrid* ComputeDistanceToHoles(vtkUnstructuredGrid* dataset,
											  vtkPolyData* in);
  

  int ComputeDistanceTo;
  int NbClosestDrillholes;
  double MinimumDistance;
  char* FileName;

  
 vtkMultiProcessController* Controller;

private:
	
	vtkPoints* centers;

   vtkPVDistanceCellsToDrillhole(const vtkDistanceCellsToDrillhole&);  // Not implemented.
   void operator=(const vtkDistanceCellsToDrillhole&);  // Not implemented.
};


#endif
