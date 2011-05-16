/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkIntersectWireframe.cxx $
  Author:   Arolde VIDJINNAGNI
	    
  Date:    JUNE 24, 2008 
  Version:   0.1


 =========================================================================*/
#include "vtkIntersectWireframe.h"
#include "vtkCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include <math.h>
#include <time.h>
#include "vtkstd/vector"
#include "vtkstd/list"
#include "vtkPoints.h"
#include "vtkTriangleFilter.h"
#include "vtkCleanPolyData.h"
//time tracker define
#define MYDEBUG
#define NOT_FOUND -1
#define XYZ  3
#define X  0
#define Y  1
#define Z  2

vtkCxxRevisionMacro ( vtkIntersectWireframe, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkIntersectWireframe );

//----------------------------------------------------------------------------
vtkIntersectWireframe::vtkIntersectWireframe()
{ 
   const int ports = 2;
   this->SetNumberOfInputPorts ( ports );
   this->pointSet = VERTEX;
   this->TypeOfPoints=0;
}
//----------------------------------------------------------------------------
vtkPolyData *vtkIntersectWireframe::GetSource()
{
   if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
   {
      return NULL;
   }

   vtkDataObject  *src = this->GetExecutive()->GetInputData ( 1, 0 );
   vtkPolyData *source = vtkPolyData::SafeDownCast ( src );
   
   return source;
}
vtkIntersectWireframe::~vtkIntersectWireframe()
{	
}
void vtkIntersectWireframe::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
{
   this->SetInputConnection ( 1, algOutput );
}
//----------------------------------------------------------------------------
int vtkIntersectWireframe::FillInputPortInformation ( int port, vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
 // info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 1 );
  return 1;
  }
  
//----------------------------------------------------------------------------
int vtkIntersectWireframe::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  return 1;
}
  

//----------------------------------------------------------------------------
// Description:
// This is called by the superclass.
// This is the method you should override.
int vtkIntersectWireframe::RequestInformation(
  vtkInformation *request, 
  vtkInformationVector **InputVector, 
  vtkInformationVector *outputVector)
{
  if(!this->Superclass::RequestInformation(request,InputVector,outputVector))
    {
    return 0;
    }
   
  vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
  inputInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

  vtkInformation *sourceInfo = InputVector[1]->GetInformationObject ( 0 );
  sourceInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

  vtkInformation *outInfo=outputVector->GetInformationObject(0);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
  return 1;
}

//----------------------------------------------------------------------------
int vtkIntersectWireframe::RequestData( vtkInformation *vtkNotUsed ( request ), 
                                         vtkInformationVector **InputVector, 
                                         vtkInformationVector *outputVector )
{
   vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
   vtkInformation *sourceInfo = InputVector[1]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

   // multi group input input
   vtkPolyData *input = vtkPolyData::SafeDownCast (
                         inputInfo->Get ( vtkPolyData::DATA_OBJECT() ) );

    vtkPolyData *source = vtkPolyData::SafeDownCast (
                         sourceInfo->Get ( vtkPolyData::DATA_OBJECT() ) );

   vtkPolyData *output = vtkPolyData::SafeDownCast (
                        outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   if ( !source ){
      return 0;
   }
   vtkCellArray* cells = source->GetPolys();
   if( cells->GetMaxCellSize() != 3 )
      cout << "Other polys than triangles are found in the surface" << endl;


   vtkIdType numPointsInput;   
   numPointsInput = input->GetNumberOfPoints();
   cout << "vtkIntersectwireframe Id process +++++++++ Points set input  nbpt=" << numPointsInput << endl;

   vtkIdType numPointsSource;   
   numPointsSource = source->GetNumberOfPoints();

   cout <<"vtkIntersectwireframe Id process=  ++++++++++ Points set source  nbpt=" << numPointsSource << endl;     


   
#ifdef MYDEBUG
   double startTime, endTime;
   startTime = time ( NULL );
#endif

   // First step is only checking. Useless for the second step
   vtkPolyData* boundary = this->boundaryEdges(source);
   if( boundary->GetLines()->GetSize( ) == 0 ){
      cout << "The surface is closed" << endl;
   }
   else{
      cout << "The surface is not topologically closed !!" << endl;    
   }

   // Second step is taging inside points
   this->ConstructOutput( input, output, source, this->TypeOfPoints);
   
#ifdef MYDEBUG
   endTime = time ( NULL );
   cout << "Time taken " << ( endTime - startTime ) << endl;
#endif
   
   return 1;
}
//----------------------------------------------------------------------------
// Compute the list of edges belonging to an odd number of triangles.
std::vector< std::list<int> > edgesAlone (vtkPolyData *source)
{
   vtkCellArray* cells = source->GetPolys();
   if( cells->GetMaxCellSize() != 3 ){
      cerr << "Other polys than triangles are found in the surface" << endl;
   //transform the source in triangulate wireframe
   vtkTriangleFilter *connect = vtkTriangleFilter::New();
   connect->SetInput(source);
   connect->Update();
   source->ShallowCopy( connect->GetOutput() );
   connect->Delete();
   
   }

   int nbpt = source->GetPoints()->GetNumberOfPoints();
   int nbtr = source->GetNumberOfCells();
   // Build an edge list to check each belongs to exactly 2 triangles
   std::vector< std::list<int> >  edgeEnds( nbpt );
   std::list<int>::iterator existingEdge;
   vtkIdList *pts = vtkIdList::New();
   vtkIdType  eb, ee;
   int i, j;
   for ( i=0; i<nbtr; i++){
      source->GetCellPoints( i, pts );
      pts->InsertId( 3, pts->GetId(0) );
      for( j=0; j<3; j++ )
      {
         eb = pts->GetId(j);
         ee = pts->GetId(j+1);
         if( eb > ee ){
            eb = ee;
            ee = pts->GetId(j);
         }
         // look for an already existing edge
         int exist = 0;
         for ( existingEdge  = edgeEnds[ eb ].begin(); 
               existingEdge != edgeEnds[ eb ].end(); existingEdge++ )
         {
            if ( *existingEdge == ee ){
               exist = 1; // remove the edge found
               edgeEnds[ eb ].erase( existingEdge );
               break;
            }
         }
         if ( exist == 0 ){ // add it as a new edge         
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
vtkPolyData* vtkIntersectWireframe::boundaryEdges(vtkPolyData *source)
{
   //vtkPolyData *source = this->GetSource();
   int nbpt = source->GetPoints()->GetNumberOfPoints();
   std::vector< std::list<int> > edgeEnds = edgesAlone( source );
   std::list<int>::iterator existingEdge;
   // If all edges belong to exactly 2 triangles, the edge list is now empty
   vtkCellArray* lines = vtkCellArray::New();
   vtkIdType* index = new vtkIdType[2];
   for ( int i=0; i<nbpt; i++){
      index[0] = i;
      for ( existingEdge  = edgeEnds[ i ].begin(); 
            existingEdge != edgeEnds[ i ].end(); existingEdge++ ){
         index[1] = *existingEdge;
         lines->InsertNextCell( 2, index );
      }
   }
   vtkPolyData* boundary = vtkPolyData::New();
     boundary->SetPoints( source->GetPoints() );
     boundary->SetLines( lines );
   return boundary;
}

//----------------------------------------------------------------------------
// Volume of a tetrahedron.
// Return a positive value if AB, AC, AD follows the right hand rule.
// Return a negative value otherwise
double OrientedVolume( double* A, double* B, double* C, double* D )
{
   double AB[3], AC[3], AD[3];
   for (int i=0; i<3; ++i ){
      AB[i] = B[i] - A[i];
      AC[i] = C[i] - A[i];
      AD[i] = D[i] - A[i];
   }
   double pv0 = AC[1]*AD[2] - AD[1]*AC[2];
   double pv1 = AC[2]*AD[0] - AD[2]*AC[0];
   double pv2 = AC[0]*AD[1] - AD[0]*AC[1];
   return 0.166666666666667 * ( AB[0]*pv0 + AB[1]*pv1 + AB[2]*pv2 );
}
//----------------------------------------------------------------------------
double TriangleArea( double* A, double* B, double* C )
{
   double AB[3], AC[3];
   for (int i=0; i<3; ++i ){
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
int vtkIntersectWireframe::nbIntersections( double *ptA, double *ptB,
										   double eps, vtkPolyData *source)
{
  //vtkPolyData *source = this->GetSource();
  int nbint = 0;
  vtkPoints* points = source->GetPoints();
  int* pts = new int[8];
  double ptC[3], ptD[3], ptE[3];
   for (int i = 0; i < source->GetNumberOfCells(); i++)
   {
      vtkIdList* pt = source->GetCell(i)->GetPointIds();
      source->GetPoint( pt->GetId(0), ptC );
      source->GetPoint( pt->GetId(1), ptD );
      source->GetPoint( pt->GetId(2), ptE );
      double area = TriangleArea( ptC, ptD, ptE );
      double zero = eps * area; // homogeneus to a volume
      double vb = OrientedVolume( ptC, ptD, ptE, ptA );
      double va = OrientedVolume( ptC, ptD, ptE, ptB );
      double dvol;
      if ( va*vb > zero*zero ) // A and B on the same side of DEF
         continue;
      if ( va*vb < -zero*zero )
      {
         double vc = OrientedVolume( ptA, ptB, ptD, ptE );
         double vd = OrientedVolume( ptA, ptB, ptE, ptC );  
         double ve = OrientedVolume( ptA, ptB, ptC, ptD );
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
         nbint++; 		
      }
      else {
         return -1;
      }
   }
   return nbint;
}

//----------------------------------------------------------------------------
int vtkIntersectWireframe::isInside( double *pt, double epsilon,
									vtkPolyData *source)
{
  // vtkPolyData *source = this->GetSource();
   // Compute the largest distance in the source
   double* bounds;
   bounds = source->GetBounds();
   double Xmin=0,Xmax=0,Ymin=0,Ymax=0,Zmin=0,Zmax=0;
   Xmin= bounds[0],Xmax= bounds[1],Ymin= bounds[2],Ymax= bounds[3],Zmin= bounds[4],Zmax= bounds[5];

   Xmax-=Xmin;
   Ymax-= Ymin;
   Zmax-= Zmin;

   double diametre= Xmax*Xmax + Ymax*Ymax + Zmax*Zmax;
   diametre = sqrt( diametre );

   // First algorithm : count the number of intersections between the source
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
      nbint = this->nbIntersections( pt, endpt, epsilon, source);
   }   while( nbint < 0 && ray-candidates < 3*nbCandidates );//ici on aura alors maximum 18 intersections
  
   if ( nbint >= 0 ) // Problem solved
   {
      // The point is inside iff there is an odd number of intersections
      return ( nbint%2 == 1 );  
   }

   // The case is undetermined. The point may be closed to one or several
   // triangle plans.
   // A second algorithm is used
   double h = diametre*epsilon; 
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
      nbint = this->nbIntersections( ptph, ptmh, epsilon, source);
   }   while( nbint != 0 && ray-candidates < 3*nbCandidates );

   if ( nbint != 0 ) // Point is near the surface
      return 0;      // It is said inside

   ray = candidates;
   do {
      endpt[0] = pt[0] + diametre * ray[0];
      endpt[1] = pt[1] + diametre * ray[1];
      endpt[2] = pt[2] + diametre * ray[2];
      ray += 3;
      nbint = this->nbIntersections( ptph, endpt, epsilon, source);
   }   while( nbint < 0 && ray-candidates < 3*nbCandidates );
   if ( nbint >= 0 ) // Problem now solved
   {
      return ( nbint%2 == 1 );
   }
   cerr << pt[0] << " " << pt[1] << " " << pt[2] << " isInside undetermined case" << endl ;
   return -1; // we are in trouble...
}
//----------------------------------------------------------------------------
vtkIntersectWireframe::vtkPointSetDefinition vtkIntersectWireframe::insidePoints( vtkDataArray* infield , 
																				 vtkPolyData* input,
																				 vtkPolyData *source) 
{
   //vtkPolyData *source = this->GetSource();
   double EPS = 0.000001; // distance relative to triangle size
   int nbpt = input->GetNumberOfCells();
   double xyz[3];
   vtkIdType* nodesa = NULL;
   int c1=0,c0=0;
   double**  PointsInside; 

   if( this->pointSet == CELLCENTER || nbpt < 1 ) // points are cell centers
   {
      cout << "Cell centers nbpt=" << nbpt << endl;

	  PointsInside= new double*[nbpt];
	  for(int i=0; i<nbpt;i++)
	  PointsInside[i]= new double[3];	  

      double paramCoo[3];
      double weights[8];
      int subId;
      for( int i=0; i<nbpt; ++i ){
         vtkCell* cell = input->GetCell( i );
         subId = cell->GetParametricCenter( paramCoo );
         cell->EvaluateLocation( subId, paramCoo, xyz, weights );
         infield->InsertNextTuple1( (double) this->isInside( xyz, EPS, source) );
         
		PointsInside[i][0]=xyz[0];
		PointsInside[i][1]=xyz[1];
		PointsInside[i][2]=xyz[2]; 
	 }
	 c1=0;c0=0;
	 for ( int i = 0; i < infield->GetNumberOfTuples() ; ++i ){
		  if( (int) infield->GetTuple1( i ) == 1 )
		  ++c1;
		  else if( (int) infield->GetTuple1( i ) == 0 )
          ++c0;
     }
	 this->myPointsPtr=vtkPoints::New(); 
	 this->myCellsPtr=vtkCellArray::New();
     this->myPointsPtr->Allocate(c1);
     this->myCellsPtr->Allocate(c1);

	 this->myPointsPtr1=vtkPoints::New(); 
	 this->myCellsPtr1=vtkCellArray::New();
     this->myPointsPtr1->Allocate(c0);
     this->myCellsPtr1->Allocate(c0);

	 for(int i=0; i<nbpt;i++){
		 if( (int) infield->GetTuple1( i ) == 1 ){
			 nodesa = new vtkIdType[1];
			 nodesa[0] = this->myPointsPtr->InsertNextPoint(PointsInside[i][0],PointsInside[i][1],PointsInside[i][2]);
			 this->myCellsPtr->InsertNextCell(1, nodesa);
          }
		 else if( (int) infield->GetTuple1( i ) == 0 ){
			 nodesa = new vtkIdType[1];
			 nodesa[0] = this->myPointsPtr1->InsertNextPoint(PointsInside[i][0],PointsInside[i][1],PointsInside[i][2]);
			 this->myCellsPtr1->InsertNextCell(1, nodesa);
          }
	  }
	return CELLCENTER;
   }
   else
   {
	   
      nbpt = input->GetNumberOfPoints();
      cout << "IntersectWireframe Points set input nbpt=" << nbpt << endl;

	  //list of inside points   par Arolde
	  PointsInside= new double*[nbpt];
	  for(int i=0; i<nbpt;i++)
	  PointsInside[i]= new double[3];
	 
      for( int i=0; i<nbpt; ++i ){
         input->GetPoint( i, xyz );
         infield->InsertNextTuple1( (double) this->isInside( xyz, EPS, source) );
		 PointsInside[i][0]=xyz[0];
		 PointsInside[i][1]=xyz[1];
		 PointsInside[i][2]=xyz[2]; 
	  }
	 c1=0;c0=0;
	 for ( int i = 0; i < infield->GetNumberOfTuples() ; ++i ){
		  if( (int) infield->GetTuple1( i ) == 1 )
		  ++c1;
		  else if( (int) infield->GetTuple1( i ) == 0 )
          ++c0;
     }
	 this->myPointsPtr=vtkPoints::New(); 
	 this->myCellsPtr=vtkCellArray::New();
     this->myPointsPtr->Allocate(c1);
     this->myCellsPtr->Allocate(c1);

	 this->myPointsPtr1=vtkPoints::New(); 
	 this->myCellsPtr1=vtkCellArray::New();
     this->myPointsPtr1->Allocate(c0);
     this->myCellsPtr1->Allocate(c0);

	 for(int i=0; i<nbpt;i++){
	    if( (int) infield->GetTuple1( i ) == 1 ){
		  nodesa = new vtkIdType[1];
		  nodesa[0] = this->myPointsPtr->InsertNextPoint(PointsInside[i][0],PointsInside[i][1],PointsInside[i][2]);
		  this->myCellsPtr->InsertNextCell(1, nodesa);
        }
		else if( (int) infield->GetTuple1( i ) == 0 ){
			 nodesa = new vtkIdType[1];
			 nodesa[0] = this->myPointsPtr1->InsertNextPoint(PointsInside[i][0],PointsInside[i][1],PointsInside[i][2]);
			 this->myCellsPtr1->InsertNextCell(1, nodesa);
        }
	  }
		return VERTEX;
   }
}

//----------------------------------------------------------------------------
void vtkIntersectWireframe::ConstructOutput ( vtkPolyData *input,vtkPolyData *output, vtkPolyData *source, vtkIdType typeOfPoint)
{

	this->TypeOfPoints = typeOfPoint; 
   
#ifdef MYDEBUG
   double startTime, endTime;
   startTime = time ( NULL );
#endif

   // First step is only checking. Useless for the second step
   vtkPolyData* boundary = this->boundaryEdges(source);
   if( boundary->GetLines()->GetSize( ) == 0 ){
      cout << "vtkIntersectWireframe The surface is closed" << endl;
   }
   else{
      cout << "vtkIntersectWireframe The surface is not topologically closed !!" << endl;    
   }
   
#ifdef MYDEBUG
   endTime = time ( NULL );
   cout << "Time taken " << ( endTime - startTime ) << endl;
#endif


   vtkIdType numCells = input->GetNumberOfCells();

   vtkDataArray* infield = vtkDataArray::CreateDataArray( VTK_DOUBLE );
   infield->SetName( "isInside" );

   //set the number of components per array slot
   infield->SetNumberOfComponents ( 1 );

   
   if( this->insidePoints( infield, input, source) == CELLCENTER ){

	      if(this->TypeOfPoints==0){
		  output->SetPoints(this->myPointsPtr);
		  output->SetVerts(this->myCellsPtr);
		}
		else if(this->TypeOfPoints==1){
		   output->SetPoints(this->myPointsPtr1);
		   output->SetVerts(this->myCellsPtr1);		   
		}     
   }
   else{

	   if(this->TypeOfPoints==0){

	     output->SetPoints(this->myPointsPtr);
         output->SetVerts(this->myCellsPtr);
	   }
	   else if(this->TypeOfPoints==1){

		 output->SetPoints(this->myPointsPtr1);
	     output->SetVerts(this->myCellsPtr1);
	   }
   }
   //free up memory
   this->myPointsPtr1->Delete();
   this->myCellsPtr1->Delete();
}

//----------------------------------------------------------------------------
void vtkIntersectWireframe::PrintSelf ( ostream& os, vtkIndent indent)
{   
  vtkPolyData *source = this->GetSource();
  this->Superclass::PrintSelf ( os, indent );
  os << indent << "source: " << source << "\n";

}
