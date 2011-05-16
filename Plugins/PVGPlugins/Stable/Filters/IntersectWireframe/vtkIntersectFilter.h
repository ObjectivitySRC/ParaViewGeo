/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkIntersectFilter.h $
  Author:    Arolde VIDJINNAGNI	     
  Date:      MIRARCO february 03, 2008
  Version:   0.1

=========================================================================*/
/*find the intersect section of two input.if there is not
  intersection the filter return a null output	
   vtkIntersectFilter is vtkIntersectFilter subclass for parallel datasets.
   It gathers the Intersection data on the root node at the end.
*/


#ifndef __vtkIntersectFilter_h
#define __vtkIntersectFilter_h
#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkDataSetAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSetGet.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
class vtkUniformGrid;
class vtkMultiProcessController;
#include "vtkIntersectWireframe.h"


class VTK_EXPORT vtkIntersectFilter : public vtkIntersectWireframe
{

public:
	
	static vtkIntersectFilter *New();

    vtkTypeRevisionMacro(vtkIntersectFilter,vtkIntersectWireframe);

    void PrintSelf(ostream& os, vtkIndent indent);
   
   /* Get/Set the multiprocess controller. If no controller is set,
   single process is assumed.*/
   virtual void SetController(vtkMultiProcessController*);
   vtkGetObjectMacro(Controller, vtkMultiProcessController);

   vtkDataSet *GetSource();  
   vtkSetMacro(TypeOfPoints, int); //SetTypeOfPoints(TypeOfPoints)

	// Description:
   // Specify the point locations used to probe input. Any geometry
   // can be used. New style. Equivalent to SetInputConnection(1, algOutput).
   void SetSourceConnection(vtkAlgorithmOutput* algOutput);


protected:
   vtkIntersectFilter();
   ~vtkIntersectFilter();

   virtual int RequestData(vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector);

   
  int TypeOfPoints;

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
        

   vtkIntersectFilter(const vtkIntersectFilter&);  // Not implemented.
   void operator=(const vtkIntersectFilter&);  // Not implemented.
};


#endif
