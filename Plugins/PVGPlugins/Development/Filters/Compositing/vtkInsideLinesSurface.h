/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkInsideLinesSurface.h $
  Author:    Marie-Gabrielle Vallet
			 Arolde VIDJINNAGNI
	     
  Date:      JUNE 28, 2008
  Version:   1.1

=========================================================================*/


#ifndef __vtkInsideLinesSurface_h
#define __vtkInsideLinesSurface_h
#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"
#include "vtkTriangleFilter.h"


class VTK_EXPORT vtkInsideLinesSurface : public vtkPolyDataAlgorithm
{
public:
   //BTX
   typedef enum vtkPointSetDefinition { VERTEX, CELLCENTER };
   //ETX
   static vtkInsideLinesSurface *New();
   vtkTypeRevisionMacro(vtkInsideLinesSurface,vtkPolyDataAlgorithm);
   void PrintSelf( ostream& os, vtkIndent indent);
   

	 vtkSetStringMacro(HoleID);
   vtkGetStringMacro(HoleID);
  
   // Description:
   // Get a pointer to the source object.
   vtkPolyData *GetSource();
   
   // Description:
   // Specify the point locations used to probe input. Any geometry
   // can be used. New style. Equivalent to SetInputConnection(1, algOutput).
   void SetSourceConnection(vtkAlgorithmOutput* algOutput);
  
  
  vtkSetMacro(TypeLines, int);
  protected:
   vtkInsideLinesSurface();
   ~vtkInsideLinesSurface();

   virtual int FillInputPortInformation(int port, vtkInformation* info);


  // Description:
  // This is called by the superclass.
  // This is the method you should override.
  virtual int RequestInformation(vtkInformation *request, 
                                 vtkInformationVector **inputVector, 
                                 vtkInformationVector *outputVector);

   virtual int RequestData(vtkInformation *, 
                           vtkInformationVector **, 
                           vtkInformationVector *);

   vtkPointSetDefinition pointSet;
 
  int TypeLines;
private:

	char* HoleID; 
	vtkPolyData* boundaryEdges();  // to visualise the gaps (geometric closeness)

   // Count source intersections with a line
   // return the number of intersections between the line AB and the source.
   // return -1 in undetermined cases
   int nbIntersections( double *ptA, double *ptB, double epsilon );

   // Use brut force to check if a point is inside or not
   int isInside( double *pt, double epsilon );

   // Loop on points from input and add a scalar field 
   void ConstructOutput(  vtkPolyData *input, vtkPolyData *output );
   vtkPointSetDefinition insidePoints(  vtkPolyData *input, vtkDataArray *infield );
   
   vtkInsideLinesSurface(const vtkInsideLinesSurface&);  // Not implemented.
   void operator=(const vtkInsideLinesSurface&);  // Not implemented.
};


#endif
