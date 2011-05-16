/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkInsideClosedSurface.cxx $
  Author:    Marie-Gabrielle Vallet
       MGV Research
  Date:      Jan 30, 2008
  Version:   0.3


 =========================================================================*/
#include "vtkInsideClosedSurface.h"
#include "vtkCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkSmartPointer.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkDataArray.h"
#include "vtkPolyData.h"
#include <math.h>
#include <time.h>
#include "vtkstd/vector"
#include "vtkstd/list"

//time tracker define
//#define MYDEBUG
#define NOT_FOUND -1
#define XYZ  3
#define X  0
#define Y  1
#define Z  2

vtkCxxRevisionMacro ( vtkInsideClosedSurface, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkInsideClosedSurface );

//----------------------------------------------------------------------------
vtkInsideClosedSurface::vtkInsideClosedSurface()
{
   const int ports = 2;
   this->SetNumberOfInputPorts ( ports );
   pointSet = VERTEX;
	 this->source = 0;

}

vtkInsideClosedSurface::~vtkInsideClosedSurface()
{
}

//----------------------------------------------------------------------------
void vtkInsideClosedSurface::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
{
   this->SetInputConnection ( 1, algOutput );
}

//----------------------------------------------------------------------------
vtkPolyData *vtkInsideClosedSurface::GetSource()
{
   if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
   {
      return NULL;
   }

   //vtkDataObject  *src = this->GetExecutive()->GetInputData ( 1, 0 );
   //vtkPolyData *source = vtkPolyData::SafeDownCast ( src );

   //return source;
	 return vtkPolyData::SafeDownCast( this->GetExecutive()->GetInputData( 1, 0 ) );
}


//----------------------------------------------------------------------------
int vtkInsideClosedSurface::RequestData( vtkInformation *vtkNotUsed ( request ),
                                         vtkInformationVector **inputVector,
                                         vtkInformationVector *outputVector )
{
   // get the info objects
   vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
   vtkInformation *sourceInfo = inputVector[1]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

   // get the input and ouptut
   vtkDataSet *input = vtkDataSet::SafeDownCast (
                       inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   // multi group source input
   vtkPolyData *source = vtkPolyData::SafeDownCast (
                         sourceInfo->Get ( vtkPolyData::DATA_OBJECT() ) );

	 //this->source = vtkPolyData::SafeDownCast (
   //                      sourceInfo->Get ( vtkPolyData::DATA_OBJECT() ) );

   vtkDataSet *output = vtkDataSet::SafeDownCast (
                        outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   if ( !source)
   {
      return 0;
   }
   //vtkCellArray* cells = source->GetPolys();
	 if( source->GetPolys()->GetMaxCellSize() != 3 )
      cout << "Other polys than triangles are found in the surface" << endl;

#ifdef MYDEBUG
   double startTime, endTime;
   startTime = time ( NULL );
#endif

	 this->source = this->GetSource();

   // First step is only checking. Useless for the second step
	 vtkPolyData* edges = vtkPolyData::New();
	 this->boundaryEdges( edges );
   if( edges->GetLines()->GetSize( ) == 0 )
   {
      cout << "The surface is closed" << endl;
   }
   else
   {
      cout << "The surface is not topologically closed !!" << endl;
      // seg fault!!!!
      //      output->CopyStructure( boundary );
   }
	 edges->Delete();

   // Second step is taging inside points
   this->ConstructOutput( input, output );

#ifdef MYDEBUG
   endTime = time ( NULL );
   cout << "Time taken " << ( endTime - startTime ) << endl;
#endif

   return 1;
}

std::vector< std::list<int> > edgesAloneCS ( vtkPolyData *source );
//----------------------------------------------------------------------------
// Test the topological closeness, watertight.
// Usually, a surface can be considered as close, even with some (small
// enought) gaps. This is not tested.
//
int vtkInsideClosedSurface::IsClosed()
{
   //vtkPolyData *source = this->GetSource();

   // By Euler-Poincare formula,
   //    np - ne + nf = 2
   // for a single connected close surface.
   // In the case of all triangular faces, the number of edges is
   //    ne = 3 ( np - 2 )
   // The number of faces is then
   //    nf = 2 - np + ne = 2 - np + 3 np - 6 = 2( np - 2 )
   // It is needded for a surface to be close, but it is not enought.
   int nbpt = source->GetPoints()->GetNumberOfPoints();
   int nbtr = source->GetNumberOfCells();
   if ( nbtr != 2*( nbpt - 2 ) )
   {
      cout << "Euler-Poincare number = " << 2*( nbpt - 2 ) << " " << nbtr << endl;
      return 0;
   }

   // Test grid connectivity
   std::vector< std::list<int> > edgeEnds = edgesAloneCS( source );
   std::list<int>::iterator existingEdge;
   int counter = 0;
   for ( int i=0; i<nbpt; i++)
   {
      for ( existingEdge  = edgeEnds[ i ].begin();
            existingEdge != edgeEnds[ i ].end(); existingEdge++ )
         counter++;
   }
   return( counter == 0 );
}

//----------------------------------------------------------------------------
// Compute the list of edges belonging to an odd number of triangles.
std::vector< std::list<int> > edgesAloneCS ( vtkPolyData *source )
{
   vtkCellArray* cells = source->GetPolys();
   if( cells->GetMaxCellSize() != 3 )
      cerr << "Other polys than triangles are found in the surface" << endl;
   int nbpt = source->GetPoints()->GetNumberOfPoints();
   int nbtr = source->GetNumberOfCells();
   // Build an edge list to check each belongs to exactly 2 triangles
   std::vector< std::list<int> >  edgeEnds( nbpt );
   std::list<int>::iterator existingEdge;
	 // Grab points from first cell. This avoids using vtkIdList::New(),
	 //   which will leave a pointer around when GetCellPoints is used
	 vtkIdList *pts = source->GetCell(0)->GetPointIds();
   vtkIdType  eb, ee;
   int i, j;
   for ( i=0; i<nbtr; i++)
   {
      source->GetCellPoints( i, pts );
      pts->InsertId( 3, pts->GetId(0) );
      for( j=0; j<3; j++ )
      {
         eb = pts->GetId(j);
         ee = pts->GetId(j+1);
         if( eb > ee )
         {
            eb = ee;
            ee = pts->GetId(j);
         }
         // look for an already existing edge
         int exist = 0;
         for ( existingEdge  = edgeEnds[ eb ].begin();
               existingEdge != edgeEnds[ eb ].end(); existingEdge++ )
         {
            if ( *existingEdge == ee )
            {
               exist = 1; // remove the edge found
               edgeEnds[ eb ].erase( existingEdge );
               break;
            }
         }
         if ( exist == 0 ) // add it as a new edge
         {
            edgeEnds[ eb ].push_back( ee );
         }

      }
   }
   return edgeEnds;
}

//----------------------------------------------------------------------------
// Output the edges belonging to an odd number of triangles
// When the surface is manifold, they are the surface boundary.
// If the surface is manifold and closed, the output should be empty.
//
void vtkInsideClosedSurface::boundaryEdges(vtkPolyData* edges)
{
   //vtkPolyData *source = this->GetSource();

   int nbpt = this->source->GetPoints()->GetNumberOfPoints();
   std::vector< std::list<int> > edgeEnds = edgesAloneCS( this->source );
   std::list<int>::iterator existingEdge;
   // If all edges belong to exactly 2 triangles, the edge list is now empty
   vtkCellArray* lines = vtkCellArray::New();
   vtkIdType* index = new vtkIdType[2];
   for ( int i=0; i<nbpt; i++)
   {
      index[0] = i;
      for ( existingEdge  = edgeEnds[ i ].begin();
            existingEdge != edgeEnds[ i ].end(); existingEdge++ )
      {
         index[1] = *existingEdge;
         lines->InsertNextCell( 2, index );
      }
   }
	 delete[] index;
   //vtkPolyData* boundary = vtkPolyData::New();
   edges->SetPoints( this->source->GetPoints() );
   edges->SetLines( lines );
	 lines->Delete();
   //return boundary;
}

//----------------------------------------------------------------------------
// Volume of a tetrahedron.
// Return a positive value if AB, AC, AD follows the right hand rule.
// Return a negative value otherwise
double OrientedVolumeCS( double* A, double* B, double* C, double* D )
{
   double AB[3], AC[3], AD[3];
   for (int i=0; i<3; ++i )
   {
      AB[i] = B[i] - A[i];
      AC[i] = C[i] - A[i];
      AD[i] = D[i] - A[i];
   }
   // volume = AB . (AC x AD ) / 6
   double pv0 = AC[1]*AD[2] - AD[1]*AC[2];
   double pv1 = AC[2]*AD[0] - AD[2]*AC[0];
   double pv2 = AC[0]*AD[1] - AD[0]*AC[1];
   return 0.166666666666667 * ( AB[0]*pv0 + AB[1]*pv1 + AB[2]*pv2 );
}
//----------------------------------------------------------------------------
double TriangleAreaCS( double* A, double* B, double* C )
{
   double AB[3], AC[3];
   for (int i=0; i<3; ++i )
   {
      AB[i] = B[i] - A[i];
      AC[i] = C[i] - A[i];
   }
   // Area = norm ( AB x AC ) / 2
   double pv0 = AB[1]*AC[2] - AC[1]*AB[2];
   double pv1 = AB[2]*AC[0] - AC[2]*AB[0];
   double pv2 = AB[0]*AC[1] - AC[0]*AB[1];
   return 0.5 * sqrt( pv0*pv0 + pv1*pv1 + pv2*pv2 );
}
//----------------------------------------------------------------------------
int vtkInsideClosedSurface::nbIntersections( double *ptA, double *ptB, double eps )
{
   //vtkPolyData *source = this->GetSource();

   int nbint = 0;
   //vtkPoints* points = this->source->GetPoints();
   //int nbpt;
   int* pts = new int[8];
   double ptC[3], ptD[3], ptE[3];
   for (int i = 0; i < this->source->GetNumberOfCells(); i++)
   {
      vtkIdList* pt = this->source->GetCell(i)->GetPointIds();
      this->source->GetPoint( pt->GetId(0), ptC );
      this->source->GetPoint( pt->GetId(1), ptD );
      this->source->GetPoint( pt->GetId(2), ptE );
      double area = TriangleAreaCS( ptC, ptD, ptE );
      double zero = eps * area; // homogeneus to a volume
      double vb = OrientedVolumeCS( ptC, ptD, ptE, ptA );
      double va = OrientedVolumeCS( ptC, ptD, ptE, ptB );
      double dvol;
      if ( va*vb > zero*zero ) // A and B on the same side of DEF
         continue;
      if ( va*vb < -zero*zero )
      {
         double vc = OrientedVolumeCS( ptA, ptB, ptD, ptE );
         double vd = OrientedVolumeCS( ptA, ptB, ptE, ptC );
         double ve = OrientedVolumeCS( ptA, ptB, ptC, ptD );
         if( (vc < zero && vc > -zero) ||
             (vd < zero && vd > -zero) ||
             (ve < zero && ve > -zero) )
            return -1; // intersection too close from a triangle side
         if ( va < 0.0 ) va = -va;
         if ( vb < 0.0 ) vb = -vb;
         if ( vc < 0.0 ) vc = -vc;
         if ( vd < 0.0 ) vd = -vd;
         if ( ve < 0.0 ) ve = -ve;
         dvol = (vc + vd + ve) - (va + vb);
         if ( dvol > zero ) // AB intersects the plan outside the triangle
            continue;
         nbint++; // intersection found
      }
      else
      {
				//points->Delete();
        return -1;
      }
   }
	 //points->Delete();
   return nbint;
}

//----------------------------------------------------------------------------
int vtkInsideClosedSurface::isInside( double *pt, double epsilon )
{
   //vtkPolyData *source = this->GetSource();

   // Compute the largest distance in the this->source
   double* bounds = this->source->GetBounds();
   bounds[1] -= bounds[0];
   bounds[3] -= bounds[2];
   bounds[5] -= bounds[4];
   double diametre = bounds[1]*bounds[1] + bounds[3]*bounds[3] + bounds[5]*bounds[5];
   diametre = sqrt( diametre );

   // First algorithm : count the number of intersections between the this->source
   // and some ray starting from the point. Several candidates for the ray
   // direction.
   int nbint;
   int nbCandidates = 6;
   double candidates[] = { 1.0, 0.0, 0.0,
                           0.0, 1.0, 0.0,
                           0.0, 0.0, 1.0,
                           -1.0, 0.0, 0.0,
                           0.0,-1.0, 0.0,
                           0.0, 0.0,-1.0 };
   double endpt[3];
   double *ray = candidates;
   do {
      endpt[0] = pt[0] + diametre * ray[0];
      endpt[1] = pt[1] + diametre * ray[1];
      endpt[2] = pt[2] + diametre * ray[2];
      ray += 3;
      nbint = this->nbIntersections( pt, endpt, epsilon );
   }   while( nbint < 0 && ray-candidates < 3*nbCandidates );

   if ( nbint >= 0 ) // Problem solved
   {
      // The point is inside iff there is an odd number of intersections
      return ( nbint%2 == 1 );
   }

   // The case is undetermined. The point may be closed to one or several
   // triangle plans.
   // A second algorithm is used
   double h = diametre*epsilon; // how to choose this distance ?
   double ptph[3];
   double ptmh[3];
   ray = candidates;
   // If there is no intersection between P+h and P-h, then P is on the same
   // side than P+h
   do {
      ptph[0] = pt[0] + h * ray[0];
      ptph[1] = pt[1] + h * ray[1];
      ptph[2] = pt[2] + h * ray[2];
      ptmh[0] = pt[0] - h * ray[0];
      ptmh[1] = pt[1] - h * ray[1];
      ptmh[2] = pt[2] - h * ray[2];
      ray += 3;
      nbint = this->nbIntersections( ptph, ptmh, epsilon );
   }   while( nbint != 0 && ray-candidates < 3*nbCandidates );

   if ( nbint != 0 ) // Point is near the surface
      return 0;      // It is said inside

   ray = candidates;
   do {
      endpt[0] = pt[0] + diametre * ray[0];
      endpt[1] = pt[1] + diametre * ray[1];
      endpt[2] = pt[2] + diametre * ray[2];
      ray += 3;
      nbint = this->nbIntersections( ptph, endpt, epsilon );
   }   while( nbint < 0 && ray-candidates < 3*nbCandidates );
   if ( nbint >= 0 ) // Problem now solved
   {
      return ( nbint%2 == 1 );
   }
   cerr << pt[0] << " " << pt[1] << " " << pt[2] << " isInside undetermined case" << endl ;
   return -1; // we are in trouble...
}

//----------------------------------------------------------------------------
void vtkInsideClosedSurface::ConstructOutput ( vtkDataSet *input,
                                               vtkDataSet *output )
{
   //vtkPolyData *source = this->GetSource();

   //number of cells in the list
   vtkIdType numCells = input->GetNumberOfCells();

   //smart pointers to the attribute arrays
   //   vtkSmartPointer<vtkDoubleArray> insideSurface = vtkSmartPointer<vtkDoubleArray>::New();
   vtkDataArray* infield = vtkDataArray::CreateDataArray( VTK_DOUBLE );
   infield->SetName( "isInside" );

   //set the number of components per array slot
   infield->SetNumberOfComponents ( 1 );

   //convert passed in vars
   output->CopyStructure ( input );

   //construct the output data from all the storageDrillCells
   //copy the input data to the output data, so we have the proper shape
   output->GetPointData()->PassData ( input->GetPointData() );
   output->GetCellData()->PassData ( input->GetCellData() );

   // Add the new array to the output.
   //so that the cells will have these values
   if( this->insidePoints( input, infield ) == CELLCENTER )
   {
      output->GetCellData()->AddArray ( infield );
      output->GetCellData()->SetActiveScalars ( "isInside" );
   }
   else
   {
      output->GetPointData()->AddArray ( infield );
      output->GetPointData()->SetActiveScalars ( "isInside" );
   }

   // just for verification
   int c1 = 0;
   int c0 = 0;
   for ( int i = 0; i < infield->GetNumberOfTuples() ; ++i )
   {
      if( (int) infield->GetTuple1( i ) == 1 )
         ++c1;
      else if( (int) infield->GetTuple1( i ) == 0 )
         ++c0;
   }
   cout << "Nb of inside  points = " << c1 << " (" << 100.0*c1/infield->GetNumberOfTuples()
        << "%)" << endl;
   cout << "Nb of outside points = " << c0 << " (" << 100.0*c0/infield->GetNumberOfTuples()
        << "%)" << endl;

	 infield->Delete();
}

//----------------------------------------------------------------------------
vtkInsideClosedSurface::vtkPointSetDefinition vtkInsideClosedSurface::insidePoints( vtkDataSet* input,
                                          vtkDataArray* infield )
{
   //vtkPolyData *source = this->GetSource();
   double EPS = 0.000001; // distance relative to triangle size

   int nbpt = input->GetNumberOfCells();
   double xyz[3];
   if( this->pointSet == CELLCENTER || nbpt < 1 ) // points are cell centers
   {
      cout << "Cell centers nbpt=" << nbpt << endl;
      double paramCoo[3];
      double weights[8];
      int subId;
			
      for( int i=0; i<nbpt; ++i )
      {
         vtkCell* cell = input->GetCell( i );
         subId = cell->GetParametricCenter( paramCoo );
         cell->EvaluateLocation( subId, paramCoo, xyz, weights );
         infield->InsertNextTuple1( (double) this->isInside( xyz, EPS ) );
         //         cout << xyz[0] << " " << xyz[1] << " " << xyz[2] << " isInside = " << infield->GetTuple1(i) << endl ;
      }
			//cell->Delete();
      return CELLCENTER;
   }
   else
   {
      nbpt = input->GetNumberOfPoints();
      cout << "Points set nbpt=" << nbpt << endl;
      for( int i=0; i<nbpt; ++i )
      {
         input->GetPoint( i, xyz );
         infield->InsertNextTuple1( (double) this->isInside( xyz, EPS ) );
         //         cout << xyz[0] << " " << xyz[1] << " " << xyz[2] << " isInside = " << infield->GetTuple1(i) << endl ;
      }
      return VERTEX;
   }
}

//----------------------------------------------------------------------------
int vtkInsideClosedSurface::FillInputPortInformation ( int port, vtkInformation* info )
{
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet" );
   return 1;
}

//----------------------------------------------------------------------------
void vtkInsideClosedSurface::PrintSelf ( ostream& os, vtkIndent indent )
{
   //vtkDataObject *source = this->GetSource();

   this->Superclass::PrintSelf ( os, indent );
   os << indent << "Source: " << this->source << "\n";
}
