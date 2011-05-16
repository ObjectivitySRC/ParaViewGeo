/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkIntersectWireframe.h $
  Author:    Arolde VIDJINNAGNI
	     
  Date:      JULY 22, 2008
  Version:   0.1

=========================================================================*/


#ifndef __vtkIntersectWireframe_h
#define __vtkIntersectWireframe_h
#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
//#include "vtkSmartPointer.h"
//#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
class vtkUniformGrid;
#include <math.h>



class VTK_EXPORT vtkIntersectWireframe : public vtkPolyDataAlgorithm
{
public:
   //BTX
   typedef enum vtkPointSetDefinition { VERTEX, CELLCENTER };
   //ETX
   static vtkIntersectWireframe *New();
   vtkTypeRevisionMacro(vtkIntersectWireframe,vtkPolyDataAlgorithm);
   void PrintSelf(ostream& os, vtkIndent indent);
   
 vtkPolyData *GetSource();
  
 vtkSetMacro(TypeOfPoints, int); //SetTypeOfPoints(TypeOfPoints)
 vtkSetMacro(Reconstruction, int); //SetTypeOfPoints(TypeOfPoints)
    // Description:
   // Specify the point locations used to probe input. Any geometry
   // can be used. New style. Equivalent to SetInputConnection(1, algOutput).
   void SetSourceConnection(vtkAlgorithmOutput* algOutput);
protected:
   vtkIntersectWireframe();
   ~vtkIntersectWireframe();



   virtual int FillInputPortInformation(int port, vtkInformation* info);
   virtual int FillOutputPortInformation(int port, vtkInformation* info); 
  

  /*virtual int RequestDataObject(vtkInformation *req,
    vtkInformationVector **inV,
    vtkInformationVector *outV);*/

  // Description:
  // This is called by the superclass.
  // This is the method you should override.
  virtual int RequestInformation(vtkInformation *request, 
                                 vtkInformationVector **secondInputVector, 
                                 vtkInformationVector *outputVector);

   virtual int RequestData(vtkInformation *, 
                           vtkInformationVector **, 
                           vtkInformationVector *);
    // Loop on points from secondInput and add a scalar field 
  virtual void ConstructOutput(vtkPolyData *input,vtkPolyData *output, vtkPolyData *source, vtkIdType typeOfPoint, vtkIdType reconstruction);

   // data membre
   vtkPointSetDefinition pointSet; 

  int TypeOfPoints;

  int Reconstruction;

private:
	vtkPoints* myPointsPtr;//inside points
    vtkCellArray* myCellsPtr;
	vtkPoints* myPointsPtr1;//outside points
    vtkCellArray* myCellsPtr1;


	vtkPoints* reconstructPoints;
	vtkCellArray* reconstructCells;
	vtkPoints* reconstructPoints1;
	vtkCellArray* reconstructCells1;



	vtkPolyData* boundaryEdges(vtkPolyData *source);  // to visualise the gaps (geometric closeness)

   // Count source intersections with a line
   // return the number of intersections between the line AB and the source.
   // return -1 in undetermined cases
   int nbIntersections( double *ptA, double *ptB, double epsilon, vtkPolyData *source );

   // Use brut force to check if a point is inside or not
   int isInside( double *pt, double epsilon, vtkPolyData *source);
   double* findSegmentPointInside(double A[3], double B[3], vtkPolyData *source);
   // Loop on points from secondInput and add a scalar field 
  // void ConstructOutput(vtkPolyData *output,vtkPolyData *input, vtkPolyData *source);
   vtkPointSetDefinition insidePoints(vtkDataArray *infield,vtkPolyData *input, vtkPolyData *source );
   
   vtkIntersectWireframe(const vtkIntersectWireframe&);  // Not implemented.
   void operator=(const vtkIntersectWireframe&);  // Not implemented.
};


#endif
