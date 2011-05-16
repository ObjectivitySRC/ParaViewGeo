/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkPVIntersectFilter.h $
  Author:    Arolde VIDJINNAGNI	     
  Date:      MIRARCO february 03, 2008
  Version:   0.1

=========================================================================*/
/*find the intersect section of two input.if there is not
  intersection the filter return a null output	
   vtkPVIntersectFilter is vtkIntersectFilter subclass for parallel datasets.
   It gathers the Intersection data on the root node at the end.
*/


#ifndef __vtkPVIntersectFilter_h
#define __vtkPVIntersectFilter_h
#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkDataSetAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
class vtkUniformGrid;
class vtkMultiProcessController;
#include "vtkIntersectWireframe.h"


class VTK_EXPORT vtkPVIntersectFilter : public vtkIntersectWireframe
{

public:
	
	static vtkPVIntersectFilter *New();

    vtkTypeRevisionMacro(vtkPVIntersectFilter,vtkIntersectWireframe);

    void PrintSelf(ostream& os, vtkIndent indent);
   
   /* Get/Set the multiprocess controller. If no controller is set,
   single process is assumed.*/
   virtual void SetController(vtkMultiProcessController*);
   vtkGetObjectMacro(Controller, vtkMultiProcessController);

   vtkPolyData *GetSource();  
   vtkSetMacro(TypeOfPoints, int); //SetTypeOfPoints(TypeOfPoints)

   vtkSetMacro(Reconstruction, int); //SetTypeReconstruction(Reconstruction)


	// Description:
   // Specify the point locations used to probe input. Any geometry
   // can be used. New style. Equivalent to SetInputConnection(1, algOutput).
   void SetSourceConnection(vtkAlgorithmOutput* algOutput);


protected:
   vtkPVIntersectFilter();
   ~vtkPVIntersectFilter();

   int RequestInformation( vtkInformation *request, 
						vtkInformationVector **inputVector, 
						 vtkInformationVector *outputVector);

   virtual int RequestData(vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector);

   
  int TypeOfPoints;
  int Reconstruction;


   vtkMultiProcessController* Controller;
  
private:
	vtkCellArray* Cells;
	vtkCellArray* Surfaces;
	vtkCellArray* StripSurfaces;
	vtkCellArray* Lines;
	vtkCellArray* Points;
	vtkPointData* PointData;
	vtkCellData* CellData;
	vtkDataArray* PointCoords;
        

   vtkPVIntersectFilter(const vtkPVIntersectFilter&);  // Not implemented.
   void operator=(const vtkPVIntersectFilter&);  // Not implemented.
};


#endif
