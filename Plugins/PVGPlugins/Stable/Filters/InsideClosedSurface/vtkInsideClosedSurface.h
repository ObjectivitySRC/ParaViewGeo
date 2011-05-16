/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkInsideClosedSurface.h $
  Author:    Marie-Gabrielle Vallet
       MGV Research
  Date:      Jan 30, 2008
  Version:   0.1

=========================================================================*/
// .NAME vtkInsideClosedSurface - sample data values at specified point
// locations .SECTION Description vtkInsideClosedSurface is a filter that
// computes a point scalar field at specified point positions. The filter
// has two inputs: the Input and Source. The Source is a triangular mesh and
// should be a closed surface. The Input geometric structure is passed
// through the filter. The point scalar is computed at the Input point
// positions by finding whether the point is inside the volume bounded by
// the source data.
//
// The filter adds or updates two data to the Input structure:
//  - a set of lines representing the boundary edges (ideally must be empty)
//  - an attribute on the points ( 1 iff inside, 0 iff outside )
//

#ifndef __vtkInsideClosedSurface_h
#define __vtkInsideClosedSurface_h

#include "vtkDataSetAlgorithm.h"

class VTK_EXPORT vtkInsideClosedSurface : public vtkDataSetAlgorithm
{
public:

	static vtkInsideClosedSurface *New();
   vtkTypeRevisionMacro(vtkInsideClosedSurface,vtkDataSetAlgorithm);
   void PrintSelf(ostream& os, vtkIndent indent);

//BTX
	enum vtkPointSetDefinition {VERTEX, CELLCENTER};
//ETX
   //Description
   //Set/Get the point set location
   //void SetPointSetDefinition( vtkPointSetDefinition where );
   //   vtkGetMacro(distInfluence, double);

   // Description:
   // Get a pointer to the source object.
   vtkPolyData *GetSource();

   // Description:
   // Specify the point locations used to probe input. Any geometry
   // can be used. New style. Equivalent to SetInputConnection(1, algOutput).
   void SetSourceConnection(vtkAlgorithmOutput* algOutput);

protected:
   vtkInsideClosedSurface();
   ~vtkInsideClosedSurface();
   virtual int FillInputPortInformation(int port, vtkInformation* info);
   virtual int RequestData(vtkInformation *,
                           vtkInformationVector **,
                           vtkInformationVector *);

   // data membre
   vtkPointSetDefinition pointSet;

private:

   // Checking data
   int IsClosed();                // topological closeness
   void boundaryEdges( vtkPolyData* edges );  // to visualise the gaps (geometric closeness)

   // Count source intersections with a line
   // return the number of intersections between the line AB and the source.
   // return -1 in undetermined cases
   int nbIntersections( double *ptA, double *ptB, double epsilon );

   // Use brut force to check if a point is inside or not
   int isInside( double *pt, double epsilon );

   // Loop on points from input and add a scalar field
   void ConstructOutput( vtkDataSet *input, vtkDataSet *output );
   vtkPointSetDefinition insidePoints( vtkDataSet *input, vtkDataArray *infield );

   vtkInsideClosedSurface(const vtkInsideClosedSurface&);  // Not implemented.
   void operator=(const vtkInsideClosedSurface&);  // Not implemented.
	 vtkPolyData *source;
};


#endif
