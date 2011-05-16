/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkInsideLinesSurface.cxx $
  Author:    Marie-Gabrielle Vallet
			 Arolde VIDJINNAGNI
	     
  Date:      JUNE 28, 2008
  Version:   1.1

 =========================================================================*/
#include "vtkThreshold.h"
#include "vtkConnectivityFilter.h"
#include "vtkInsideLinesSurface.h"
#include "vtkCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkSmartPointer.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkDataArray.h"
#include "vtkPolyData.h"
#include <math.h>
#include <time.h>
#include "vtkstd/vector"
#include "vtkstd/list"
#include "vtkPoints.h"
#include "vtkImageData.h"
#include "vtkTriangleFilter.h"
//time tracker define
#define MYDEBUG
#define NOT_FOUND -1
#define XYZ  3
#define X  0
#define Y  1
#define Z  2

using namespace std;

vtkCxxRevisionMacro ( vtkInsideLinesSurface, "$Revision: 0.1 $" );
vtkStandardNewMacro ( vtkInsideLinesSurface );


/***************************************************************************/
vtkInsideLinesSurface::vtkInsideLinesSurface()
{
	 this->HoleID= NULL;
   const int ports = 2;
   this->SetNumberOfInputPorts ( ports );
   
   this->pointSet = VERTEX;

}
//----------------------------------------------------------------------------
vtkInsideLinesSurface::~vtkInsideLinesSurface()
{
}
//----------------------------------------------------------------------------
void vtkInsideLinesSurface::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
{
   this->SetInputConnection ( 1, algOutput );
}

//----------------------------------------------------------------------------
vtkPolyData *vtkInsideLinesSurface::GetSource()
{
   if ( this->GetNumberOfInputConnections ( 1 ) < 1 )
   {
      return NULL;
   }

   vtkDataObject  *src = this->GetExecutive()->GetInputData ( 1, 0 );
   vtkPolyData *source = vtkPolyData::SafeDownCast ( src );
   
   return source;
}
//----------------------------------------------------------------------------
// Description:
// This is called by the superclass.
// This is the method you should override.
int vtkInsideLinesSurface::RequestInformation(
  vtkInformation *request, 
  vtkInformationVector **inputVector, 
  vtkInformationVector *outputVector)
{
  if(!this->Superclass::RequestInformation(request,inputVector,outputVector))
    {
    return 0;
    }
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
  inInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
  
  
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject ( 0 );
  sourceInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
 

  vtkInformation *outInfo=outputVector->GetInformationObject(0);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);
  return 1;
}

/***************************************************************************/
int vtkInsideLinesSurface::RequestData( vtkInformation *vtkNotUsed ( request ), 
                                         vtkInformationVector **inputVector, 
                                         vtkInformationVector *outputVector )
{
   // get the info objects
   vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
   vtkInformation *sourceInfo = inputVector[1]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );
   
   // get the input and ouptut
   vtkPolyData *input = vtkPolyData::SafeDownCast (
                       inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   // multi group source input
  
   vtkPolyData *source = vtkPolyData::SafeDownCast (
                         sourceInfo->Get ( vtkPolyData::DATA_OBJECT() ) );

   // get the info objects
     
   vtkPolyData *output = vtkPolyData::SafeDownCast (
                        outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   if ( !source ){
      return 0;
   }
   vtkCellArray* cells = source->GetPolys();
   
  
#ifdef MYDEBUG
   double startTime, endTime;
   startTime = time ( NULL );
#endif

   // First step is only checking. Useless for the second step
   vtkPolyData* boundary = this->boundaryEdges();
 
   // Second step is taging inside points
   this->ConstructOutput( input, output );

#ifdef MYDEBUG
   endTime = time ( NULL );
 
#endif
   
   return 1;
}
/**************************MIRARCO***************************************************
* Function: 			edgesAlone1        											*
* Description:	Compute the list of edges belonging to an odd number of triangles.	*														  *
* Input parameters :																*		                                                                 
* Output Parametres:																*
* Date:																				*
*************************************************************************************/
// 
std::vector< std::list<int> > edgesAlone1 ( vtkPolyData *source )
{
   vtkCellArray* cells = source->GetPolys();
   if( cells->GetMaxCellSize() != 3 ){
     // cerr << "Other polys than triangles are found in the surface" << endl;
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
/**************************MIRARCO*****************************************
* Function: 		boundaryEdges          	                              *
* Description: Output the edges belonging to an odd number of triangles	  *
*			   When the surface is manifold, they are the surface         *
*			   boundary. If the surface is manifold and closed,           *
*			   the output should be empty.                                *
*																		  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
vtkPolyData* vtkInsideLinesSurface::boundaryEdges()
{
   vtkPolyData *source = this->GetSource();

   int nbpt = source->GetPoints()->GetNumberOfPoints();
   std::vector< std::list<int> > edgeEnds = edgesAlone1( source );
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
/**************************MIRARCO*****************************************
* Function:		 OrientedVolume1	       	                              *
* Description:  Volume of a tetrahedron.								  *
*				Return a positive value if AB, AC, AD follows the         *
*				right hand rule. Return a negative value otherwise	      *													  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
double OrientedVolume1( double* A, double* B, double* C, double* D )
{
   double AB[3], AC[3], AD[3];
   for (int i=0; i<3; ++i ){
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
/**************************MIRARCO*****************************************
* Function: 	TriangleArea1	          	                              *
* Description:	calculate the value of the surface						  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
double TriangleArea1( double* A, double* B, double* C )
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
/**************************MIRARCO*****************************************
* Function: 				nbIntersections	                              *
* Description:	find the number of intersection the line defined by       *
*                the two points A and B have with the wireframe		      *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
int vtkInsideLinesSurface::nbIntersections( double *ptA, double *ptB, double eps )
{
   vtkPolyData *source = this->GetSource();

   int nbint = 0;
   vtkPoints* points = source->GetPoints();
   double ptC[3], ptD[3], ptE[3];
   for (int i = 0; i < source->GetNumberOfCells(); i++)
   {
      vtkIdList* pt = source->GetCell(i)->GetPointIds();
      source->GetPoint( pt->GetId(0), ptC );
      source->GetPoint( pt->GetId(1), ptD );
      source->GetPoint( pt->GetId(2), ptE );
      double area = TriangleArea1( ptC, ptD, ptE );
      double zero = eps * area; // homogeneus to a volume
      double vb = OrientedVolume1( ptC, ptD, ptE, ptA );
      double va = OrientedVolume1( ptC, ptD, ptE, ptB );
      double dvol;
      if ( va*vb > zero*zero ) // A and B on the same side of DEF
         continue;
      if ( va*vb < -zero*zero ){
         double vc = OrientedVolume1( ptA, ptB, ptD, ptE );
         double vd = OrientedVolume1( ptA, ptB, ptE, ptC );  
         double ve = OrientedVolume1( ptA, ptB, ptC, ptD );
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
      else {
         return -1;
      }
   }
   return nbint;
}
/**************************MIRARCO*****************************************
* Function: 			isInside         	                              *
* Description:	return a result if the point is inside the wireframe      *
*								or not 									  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
int vtkInsideLinesSurface::isInside( double *pt, double epsilon )
{
   vtkPolyData *source = this->GetSource();

   // Compute the largest distance in the source
   double* bounds = source->GetBounds();
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
/**************************MIRARCO*****************************************
* Function: 			insidePoints       	                              *
* Description:															  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
vtkInsideLinesSurface::vtkPointSetDefinition vtkInsideLinesSurface::insidePoints( vtkPolyData* input,
                                          vtkDataArray* infield ) 
{
   vtkPolyData *source = this->GetSource();
   double EPS = 0.000001; // distance relative to triangle size 

   int nbpt = input->GetNumberOfCells();
   double xyz[3];
   
   if( this->pointSet == CELLCENTER || nbpt < 1 ) // points are cell centers
   {
      //cerr << "Cell centers nbpt=" << nbpt << endl;
      double paramCoo[3];
      double weights[8];
      int subId;
      for( int i=0; i<nbpt; ++i )
      {
         vtkCell* cell = input->GetCell( i );
         subId = cell->GetParametricCenter( paramCoo );
         cell->EvaluateLocation( subId, paramCoo, xyz, weights );
         infield->InsertNextTuple1( (double) this->isInside( xyz, EPS ) );        
      }
      return CELLCENTER;
   }
   else
   {
      nbpt = input->GetNumberOfPoints();
      //cerr << "Points set nbpt=" << nbpt << endl;
      for( int i=0; i<nbpt; ++i )
      {
         input->GetPoint( i, xyz );
         infield->InsertNextTuple1( (double) this->isInside( xyz, EPS ) );        
      }
      return VERTEX;
   }
}
/**************************MIRARCO*****************************************
* Function: 		ConstructOutput        	                              *
* Description:	the function find the inside points of the drill holes    *
*                lines or outside and compute that in the output		  *
* Input parameters :													  *		                                                                 
* Output Parametres:                                                      *
* Date:					                                                  *
***************************************************************************/
void vtkInsideLinesSurface::ConstructOutput ( vtkPolyData *input,
                                               vtkPolyData *output )
{
   vtkPolyData *source = this->GetSource();

   //number of cells in the list
   vtkIdType numCells = input->GetNumberOfCells();

   vtkDataArray* infield = vtkDataArray::CreateDataArray( VTK_DOUBLE );
   infield->SetName( "isInside" );

   //set the number of components per array slot
   infield->SetNumberOfComponents ( 1 );

 
   vtkCellArray* Lines = input->GetLines();
   vtkCellArray* Cells;
   vtkDataArray* PointCoords;
   double* currentTuple;
   int numberOfCells;
   double EPS = 0.000001;
   double pt1[3];
   double pt2[3];
   int c1=0,c0=0;
   
  
  if(Lines->GetNumberOfCells() > 0)
  {
	Cells = input->GetLines();
  }
		
	numberOfCells= Cells->GetNumberOfCells();
	Cells->InitTraversal();	

	PointCoords = input->GetPoints()->GetData();

	//find the total numbers of points and cells to make in the output by finding the number of intersections
	int nbCellToMake=0;int nbCellOut=0;
	int nbPtsTotalIntersect=0;
	for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
	{		 
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType(); 
		// Create array of points in current cell.
		vtkIdType *pts; 
		
		Cells->GetNextCell(*npts, *&pts);
		
		c1=0;c0=0;
		int nbIntersect=0;//give the number of intersections of the drillhole
		//find the number of intersections
		int pointBefore=2; //0 if out and 1 if point in
		
		for(int currentPoint = 0; currentPoint < *npts-1; currentPoint++)
		{
			currentTuple = NULL;
			currentTuple = PointCoords->GetTuple(pts[currentPoint]);
			if((double) this->isInside( currentTuple, EPS )==1 ){
				c1++;
				if(pointBefore==0)
					nbIntersect++;
				pointBefore=1;
			}
			else if((double) this->isInside( currentTuple, EPS )==0 ){
				c0++;
				if(pointBefore==1)
					nbIntersect++;
				pointBefore=0;
			}				
		}
	
		if( (nbIntersect%2)==1 ){
			nbPtsTotalIntersect+= nbIntersect;
			nbCellToMake+= nbIntersect+1;
		}
		else {		
				if(nbIntersect==0)
					nbCellToMake+=1;
				else{
					nbPtsTotalIntersect+= nbIntersect;
					nbCellToMake+= nbIntersect;
				}				
		}
		nbCellOut+= (nbIntersect/2 +1);
	}

	c1=0;c0=0;
	for ( int i = 0; i < infield->GetNumberOfTuples() ; ++i ){
		  if( (int) infield->GetTuple1( i ) == 1 ){
		  ++c1;		  
		  }
		  else if( (int) infield->GetTuple1( i ) == 0 ){
          ++c0;		  
		  }
     }
	
	vtkPoints* listPoints;//list of the inside points
	listPoints=vtkPoints::New();
	int nb= c1+ nbPtsTotalIntersect;
	listPoints->Allocate(nb);

	vtkFloatArray* pointsDepth; //list of the point depth
	pointsDepth = vtkFloatArray::New();
	pointsDepth->Allocate(nb);
	((vtkFloatArray*)pointsDepth)->SetName("PointDepth");

	vtkCellArray* newLinesInside;
	newLinesInside = vtkCellArray::New();
	newLinesInside->Allocate(nbCellToMake);

	//save the id of drillHoles and add it for each cells of the drillHole
	vtkDataArray* oldLinesInsideId;
	vtkCellData* p= input->GetCellData();
	oldLinesInsideId= p->GetArray(this->HoleID);

	vtkFloatArray* newLinesInsideId; //each new cell will have the old id of the drillHole
	newLinesInsideId = vtkFloatArray::New();
	newLinesInsideId->Allocate(nbCellToMake);
	((vtkFloatArray*)newLinesInsideId)->SetName(this->HoleID);

	nb= c0+ nbPtsTotalIntersect;
  vtkPoints* listPtr1;//outside segments
  vtkCellArray* CellsPtr1;
	listPtr1=vtkPoints::New();
	CellsPtr1=vtkCellArray::New();
	listPtr1->Allocate(nb);
	CellsPtr1->Allocate(nbCellOut);


	//construct the segments lines inside 
	Cells->InitTraversal();	
	int compteur=0;
	int compteur2=0;
	//double HoleId;
	
	for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
	{			
		double myPoint[3];
		double middle[3];
		bool point_Trouve=false;			
		
		myPoint[0]=0;myPoint[1]=0;myPoint[2]=0;
		middle[0]=0;middle[1]=0;middle[2]=0;
	
		// Stores number of points in current cell
		vtkIdType* npts = new vtkIdType(); 
		// Create array of points in current cell.
		vtkIdType *pts; 

		Cells->GetNextCell(*npts, *&pts);
		double* inside = new double[*npts];

		c1=0;c0=0;
		for(int currentPoint = 0; currentPoint < *npts; currentPoint++){
			currentTuple = NULL;
			currentTuple = PointCoords->GetTuple(pts[currentPoint]);
			if((double) this->isInside( currentTuple, EPS )==1 ){
				inside[currentPoint]= 1;
				c1++;
			}
			else if((double) this->isInside( currentTuple, EPS )==0 ){
				inside[currentPoint]= 0;
				c0++;					
			}
			else inside[currentPoint]= -1;
		}
		 
		//list of inside points for each drillHole 
		double**  PointsInside;
		PointsInside= new double*[*npts];
		for(int i=0; i<*npts;i++)
		PointsInside[i]= new double[3];
		float* myDepths;
		myDepths= new float[*npts];
		float distance=0;
		float a=0;
		//calculate the depth of all points of drillHoles because we don't have it
		//the first point of each drillHole will have the depth 0
		for(int currentPoint = 0; currentPoint < *npts; currentPoint++){
			currentTuple = NULL;
			currentTuple = PointCoords->GetTuple(pts[currentPoint]);
			PointsInside[currentPoint][0]=currentTuple[0];
			PointsInside[currentPoint][1]=currentTuple[1];
			PointsInside[currentPoint][2]=currentTuple[2];

			//calcul the depth of the point and add it the table
			if(currentPoint == 0){
				myDepths[0]=0;
			}
			else{
				distance= sqrt( pow((PointsInside[currentPoint][0]-PointsInside[currentPoint-1][0]),2) 
				  + pow((PointsInside[currentPoint][1]-PointsInside[currentPoint-1][1]),2) 
				  + pow((PointsInside[currentPoint][2]-PointsInside[currentPoint-1][2]),2) );
				myDepths[currentPoint]= myDepths[currentPoint-1]+distance;
			}

		}
		
		int newCell=1;	
		int nbPointsCell=0;
		int nbOut=0,nbTotalOut=0;
		for(int currentPoint = 0; currentPoint < (*npts -1); currentPoint++)
		{
			currentTuple = NULL;
			currentTuple = PointCoords->GetTuple(pts[currentPoint]);

			if(inside[currentPoint]==0 )
			{
				listPtr1->InsertPoint(compteur2,currentTuple[0],currentTuple[1],currentTuple[2]);
				compteur2++;
				nbOut++;
				nbTotalOut++;
				
				if(currentPoint== (*npts -2) ){
					if(inside[currentPoint+1]==0){
						currentTuple = NULL;
						currentTuple = PointCoords->GetTuple(pts[currentPoint+1]);
						listPtr1->InsertPoint(compteur2,currentTuple[0],currentTuple[1],currentTuple[2]);
						compteur2++;
						nbOut++;
						nbTotalOut++;
					}
					//make the fisrt out segment 
					CellsPtr1->InsertNextCell(nbOut);
					for (int i= compteur2- nbOut; i < compteur2; i++)
						CellsPtr1->InsertCellPoint (i);
					nbOut=0;

				}
								
				//i will test if the next point is inside
				if (inside[currentPoint+1]==1)
				{
					
					//the point is inside so i will find the intersection point
					int nIter=0;
					//then find an intersection
					//make an algorithm to find an midle
					pt1[0]= currentTuple[0];pt1[1]= currentTuple[1];pt1[2]= currentTuple[2];
					pt2[0]= PointsInside[currentPoint+1][0];pt2[1]= PointsInside[currentPoint+1][1];
					pt2[2]= PointsInside[currentPoint+1][2];

					point_Trouve=false;
					while(point_Trouve==false)
					{
						middle[0]= (pt1[0]+pt2[0])/2;
						middle[1]= (pt1[1]+pt2[1])/2;
						middle[2]= (pt1[2]+pt2[2])/2;
						
						if( ((double) this->isInside(pt1, EPS)==1 && (double) this->isInside(middle, EPS)==0)||
							((double) this->isInside(pt1, EPS)==0 && (double) this->isInside(middle, EPS)==1) ){
							pt2[0]= middle[0];pt2[1]= middle[1];pt2[2]= middle[2];
						}
						else if( ((double) this->isInside(pt2, EPS)==1 && (double) this->isInside(middle, EPS)==0)||
							((double) this->isInside(pt2, EPS)==0 && (double) this->isInside(middle, EPS)==1) ){
							pt1[0]= middle[0];pt1[1]= middle[1];pt1[2]= middle[2];
						}
						if( nIter==10 )
							if(point_Trouve==false){
								myPoint[0]=  middle[0];
								myPoint[1]=  middle[1];
								myPoint[2]=  middle[2];

								middle[0]=0;
								middle[1]=0;
								middle[2]=0;							

								//insertion of the intersection point in the list of inside points
								listPoints->InsertPoint(compteur,myPoint[0],myPoint[1],myPoint[2]);

								//insertion of the intersection point in the list of outside points 
								//too to a good generation of outside segments
								listPtr1->InsertPoint(compteur2,myPoint[0],myPoint[1],myPoint[2]);
								compteur2++;
								nbOut++;
								nbTotalOut++;
								a= sqrt( pow((myPoint[0]-PointsInside[currentPoint][0]) ,2) + pow((myPoint[1]-PointsInside[currentPoint][1]) ,2) +
										pow((myPoint[2]-PointsInside[currentPoint][2]) ,2) );
								//add the depht of the intersection point
								pointsDepth->InsertNextValue(myDepths[currentPoint]+a); 
								compteur++;
								nbPointsCell=1;
								newCell=1;
							 
							point_Trouve=true;
								
							}	
							nIter++;
						 }
						//make the fisrt out segment 
						if(nbOut>0)
						{
							CellsPtr1->InsertNextCell(nbOut);
							for (int i= compteur2- nbOut; i < compteur2; i++)
								CellsPtr1->InsertCellPoint (i);							
							nbOut=0;
						}
				    }
				}
				else if(inside[currentPoint]==1 )
				{
					//the point is inside so i will insert it in the inside listpoints
					listPoints->InsertPoint(compteur,currentTuple[0],currentTuple[1],currentTuple[2]);	
					//add the depht of that inside point
					pointsDepth->InsertNextValue(myDepths[currentPoint]); 
					compteur++;
					nbPointsCell++;

					//i will test if the next point is outside
					if (inside[currentPoint+1]==0){
						//the point is outside so i will find the intersection point
						int nIter=0;
						//then find an intersection
						//make an algorithm to find an midle
						pt1[0]= currentTuple[0];pt1[1]= currentTuple[1];pt1[2]= currentTuple[2];
						pt2[0]= PointsInside[currentPoint+1][0];pt2[1]= PointsInside[currentPoint+1][1];
						pt2[2]= PointsInside[currentPoint+1][2];
		
						
						point_Trouve=false;
						while(point_Trouve==false)
						{
							middle[0]= (pt1[0]+pt2[0])/2;
							middle[1]= (pt1[1]+pt2[1])/2;
							middle[2]= (pt1[2]+pt2[2])/2;
							
							if( ((double) this->isInside(pt1, EPS)==1 && (double) this->isInside(middle, EPS)==0)||
								((double) this->isInside(pt1, EPS)==0 && (double) this->isInside(middle, EPS)==1) ){
								pt2[0]= middle[0];pt2[1]= middle[1];pt2[2]= middle[2];
							}
							else if( ((double) this->isInside(pt2, EPS)==1 && (double) this->isInside(middle, EPS)==0)||
								((double) this->isInside(pt2, EPS)==0 && (double) this->isInside(middle, EPS)==1) ){
								pt1[0]= middle[0];pt1[1]= middle[1];pt1[2]= middle[2];
							}
							if( nIter==10 )
								if(point_Trouve==false){
									myPoint[0]=  middle[0];
									myPoint[1]=  middle[1];
									myPoint[2]=  middle[2];

									middle[0]=0;
									middle[1]=0;
									middle[2]=0;	

									//insertion of the point
									listPoints->InsertPoint(compteur,myPoint[0],myPoint[1],myPoint[2]);	

									//insertion of the intersection point in the list of outside points 
									//too to a good generation of outside segments
									listPtr1->InsertPoint(compteur2,myPoint[0],myPoint[1],myPoint[2]);
									compteur2++;
									nbOut++;
									nbTotalOut++;

									//add the depht of the intersection point
									a= sqrt( pow((myPoint[0]-currentTuple[0]) ,2) + pow((myPoint[1]-currentTuple[1]) ,2) +
										pow((myPoint[2]-currentTuple[2]) ,2) );
									pointsDepth->InsertNextValue(myDepths[currentPoint]+a); 
									compteur++;
									nbPointsCell++;
									point_Trouve=true;

									//insert the cell
									newLinesInside->InsertNextCell(nbPointsCell);
									double* current;										
									current=oldLinesInsideId->GetTuple(currentCell);
									newLinesInsideId->InsertNextValue(current[0]); //add the HoleId
									for (int i= compteur- nbPointsCell; i < compteur; i++){
										newLinesInside->InsertCellPoint (i);
									}								
									nbPointsCell=0;
									newCell=0;
									
								  }
							    nIter++;
							}
					     }
					    else if( (inside[currentPoint+1]==1) && ( (currentPoint+1)==(*npts -1) ) ){
							 //insertion of the point
							 listPoints->InsertPoint(compteur,PointsInside[currentPoint+1][0],PointsInside[currentPoint+1][1],
														PointsInside[currentPoint+1][2]);	
							 //add the depht of the last inside point
							 pointsDepth->InsertNextValue(myDepths[currentPoint+1]); 
							 compteur++;
							 nbPointsCell++;

							//insert the cell
							newLinesInside->InsertNextCell(nbPointsCell);
							double* current;										
							current=oldLinesInsideId->GetTuple(currentCell);
							newLinesInsideId->InsertNextValue(current[0]); //add the HoleId
							for (int i= compteur- nbPointsCell; i < compteur; i++){
								newLinesInside->InsertCellPoint (i);
							 }
							
							 nbPointsCell=0;
							 newCell=0;
						 }
					}				 
			}
			delete myDepths;
			for(int i=0; i<*npts;i++)
			delete PointsInside[i];
			delete PointsInside;
			delete inside;					
	}						
	
   if(this->TypeLines==0)
	 {
	  output->SetPoints(listPoints);
	  output->GetPointData()->AddArray(pointsDepth);
	  output->SetLines(newLinesInside);
	  output->GetCellData()->AddArray(newLinesInsideId);
	 
	  
	}
	else if(this->TypeLines==1){
	   output->SetPoints(listPtr1);
	   output->SetLines(CellsPtr1);	  
	} 

	//free up memory
	listPoints->Delete();	
	newLinesInside->Delete();
	listPtr1->Delete();
	CellsPtr1->Delete();
		   
}
/***************************************************************************/
int vtkInsideLinesSurface::FillInputPortInformation ( int port, vtkInformation* info ){
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
   return 1;
}
/***************************************************************************/
void vtkInsideLinesSurface::PrintSelf ( ostream& os, vtkIndent indent )
{      
   vtkPolyData *source = this->GetSource();
  this->Superclass::PrintSelf ( os, indent );
  os << indent << "Source: " << source << "\n";
 
}
