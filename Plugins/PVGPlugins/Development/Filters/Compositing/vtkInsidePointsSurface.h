/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkInsidePointsSurface.h $
  Author:    Marie-Gabrielle Vallet
			 Arolde VIDJINNAGNI
	     
  Date:      JUNE 28, 2008
  Version:   1.2

=========================================================================*/


#ifndef __vtkInsidePointsSurface_h
#define __vtkInsidePointsSurface_h
#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
//#include "vtkSmartPointer.h"
//#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
class vtkUniformGrid;



class VTK_EXPORT vtkInsidePointsSurface : public vtkPolyDataAlgorithm
{
public:
   //BTX
   typedef enum vtkPointSetDefinition { VERTEX, CELLCENTER };
   //ETX
   static vtkInsidePointsSurface *New();
   vtkTypeRevisionMacro(vtkInsidePointsSurface,vtkPolyDataAlgorithm);
   void PrintSelf(ostream& os, vtkIndent indent,vtkPolyData *input);
   
   
   // Description:
   // Specify the point locations used to probe secondInput. Any geometry
   // can be used. New style. Equivalent to SetsecondInputConnection(1, algOutput).
   //void SetSourceConnection(vtkAlgorithmOutput* algOutput);

  //dimension x of the grid
  vtkSetMacro(Nx, double);
  vtkGetMacro(Nx, double);

  //dimension y of the grid
  vtkSetMacro(Ny, double);
  vtkGetMacro(Ny, double);

  //dimension z of the grid
  vtkSetMacro(Nz, double);
  vtkGetMacro(Nz, double);
  
  vtkSetMacro(TypeOfPoints, int); //SetTypeOfPoints(TypeOfPoints)
protected:
   vtkInsidePointsSurface();
   ~vtkInsidePointsSurface();

  // Description:
  // This is called by the superclass.
  // This is the method you should override.
  virtual int RequestInformation(vtkInformation *request, 
                                 vtkInformationVector **secondInputVector, 
                                 vtkInformationVector *outputVector);

   virtual int RequestData(vtkInformation *, 
                           vtkInformationVector **, 
                           vtkInformationVector *);

   // data membre
   vtkPointSetDefinition pointSet; 

  double Nx;
  double Ny;
  double Nz;

  int TypeOfPoints;
private:
	vtkPoints* myPointsPtr;//inside points
    vtkCellArray* myCellsPtr;
	vtkPoints* myPointsPtr1;//outside points
    vtkCellArray* myCellsPtr1;
	vtkPolyData* boundaryEdges(vtkPolyData *input);  // to visualise the gaps (geometric closeness)

   // Count source intersections with a line
   // return the number of intersections between the line AB and the source.
   // return -1 in undetermined cases
   int nbIntersections( double *ptA, double *ptB, double epsilon ,vtkPolyData *input);

   // Use brut force to check if a point is inside or not
   int isInside( double *pt, double epsilon,vtkPolyData *input);

   // Loop on points from secondInput and add a scalar field 
   void ConstructOutput( vtkImageData *secondInput, vtkPolyData *output,vtkPolyData *input );
   vtkPointSetDefinition insidePoints( vtkImageData *secondInput, vtkDataArray *infield,vtkPolyData *input );
   
   vtkInsidePointsSurface(const vtkInsidePointsSurface&);  // Not implemented.
   void operator=(const vtkInsidePointsSurface&);  // Not implemented.
};


#endif
