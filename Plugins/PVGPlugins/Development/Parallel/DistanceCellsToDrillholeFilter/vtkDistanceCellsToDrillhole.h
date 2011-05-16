/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkDistanceCellsToDrillhole.h $
  Author:    Arolde VIDJINNAGNI	     
  Date:      MIRARCO february 03, 2008
  Version:   0.1

=========================================================================*/


#ifndef __vtkDistanceCellsToDrillhole_h
#define __vtkDistanceCellsToDrillhole_h
#include "vtkSetGet.h"
#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"


class VTK_EXPORT vtkDistanceCellsToDrillhole : public vtkUnstructuredGridAlgorithm
{

public:
   	static vtkDistanceCellsToDrillhole *New();

    vtkTypeRevisionMacro(vtkDistanceCellsToDrillhole,vtkUnstructuredGridAlgorithm);
 
   vtkSetMacro(ComputeDistanceTo, int);
   vtkSetMacro(NbClosestDrillholes, int); 
   vtkSetMacro(MinimumDistance, double);    
  

	// Description:
   // Specify the point locations used to probe input. Any geometry
   // can be used. New style. Equivalent to SetInputConnection(1, algOutput).
   void SetSourceConnection(vtkAlgorithmOutput* algOutput);




protected:
   vtkDistanceCellsToDrillhole();
   ~vtkDistanceCellsToDrillhole();

   int RequestInformation( vtkInformation *request, 
						vtkInformationVector **inputVector, 
						 vtkInformationVector *outputVector);

   virtual int RequestData(vtkInformation *request,
						   vtkInformationVector **inputVector,
						   vtkInformationVector *outputVector);

   
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info); 
   
  void ComputeCenters(vtkUnstructuredGrid* dataset);

  
  vtkDoubleArray* ProcessCalcul();
  vtkUnstructuredGrid* ComputeDistanceToHoles(vtkUnstructuredGrid* dataset,
											  vtkPolyData* in);
  

  int ComputeDistanceTo;
  int NbClosestDrillholes;
  double MinimumDistance;
  
private:
	
	vtkPoints* centers;
	vtkDoubleArray* distanceArray_Proc;
	
   vtkDistanceCellsToDrillhole(const vtkDistanceCellsToDrillhole&);  // Not implemented.
   void operator=(const vtkDistanceCellsToDrillhole&);  // Not implemented.
};


#endif
