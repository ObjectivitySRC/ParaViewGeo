/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkDistanceCellsToDrillhole.cxx $
  Author:    Arolde VIDJINNAGNI	    
  Date:      MIRARCO May 08, 2009 
  Version:   0.2
 =========================================================================*/
#include "vtkDistanceCellsToDrillhole.h"
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
#include "vtkDataSet.h"
#include <vtkMath.h>
#include <time.h>
#include "vtkstd/vector"
#include "vtkstd/list"
#include <vtkstd/map>
#include "vtkPoints.h"
#include "vtkAppendPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkAttributeDataReductionFilter.h"
#include "vtkCollectPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkToolkits.h"
#include "vtkStdString.h"
#include <vtksys/ios/sstream>
#include <sstream>

#include <assert.h>

#include <vtkstd/string>
#include <vtksys/RegularExpression.hxx>

vtkCxxRevisionMacro ( vtkDistanceCellsToDrillhole, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkDistanceCellsToDrillhole );

//--------------------------------------------------------------------------
vtkDistanceCellsToDrillhole::vtkDistanceCellsToDrillhole()
{ 
   const int ports = 2;
   this->SetNumberOfInputPorts ( ports );
   this->NbClosestDrillholes=5;
   this->MinimumDistance= 5;
   this->ComputeDistanceTo=0; 
}
//--------------------------------------------------------------------------
vtkDistanceCellsToDrillhole::~vtkDistanceCellsToDrillhole()
{		
}
void vtkDistanceCellsToDrillhole::SetSourceConnection ( vtkAlgorithmOutput* algOutput )
{
   this->SetInputConnection ( 1, algOutput );
}

//----------------------------------------------------------------------------
int vtkDistanceCellsToDrillhole::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 1 );
  return 1;
  }


//----------------------------------------------------------------------------
int vtkDistanceCellsToDrillhole::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid");
  return 1;
}

//----------------------------------------------------------------------------
// Description:
// This is called by the superclass.
// This is the method you should override.
int vtkDistanceCellsToDrillhole::RequestInformation(
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

void vtkDistanceCellsToDrillhole::ComputeCenters(vtkUnstructuredGrid* dataset)
{
	int nbCell= dataset->GetNumberOfCells();

	this->centers= vtkPoints::New();
	this->centers->Allocate(nbCell);

	double center[3];
	double bounds[6];
	for(int i=0; i<nbCell; i++)
	{
		dataset->GetCellBounds(i,bounds);	
		center[0]= bounds[0]+(bounds[1]-bounds[0])/2;
		center[1]= bounds[2]+(bounds[3]-bounds[2])/2;
		center[2]= bounds[4]+(bounds[5]-bounds[4])/2;
		this->centers->InsertPoint(i, center);		
	}
}

//-----------------------------------------------------------------------------
float DistancePointToHole_Wrong(double point[3], vtkIdType HoleId,vtkPolyData* holes)
{
	vtkIdList* listId= vtkIdList::New();
	listId= holes->GetCell(HoleId)->GetPointIds();

	int nbPoint= listId->GetNumberOfIds();
	float MinimumDistance= 0;
	float temp=0;
	double current[3];
	holes->GetPoint(listId->GetId(0),current);
	MinimumDistance= sqrt( vtkMath::Distance2BetweenPoints(point, current) );

	for(int i=1; i<nbPoint; i++)
	{
	  holes->GetPoint(listId->GetId(i),current);
	  temp= sqrt( vtkMath::Distance2BetweenPoints(point, current) );
	  if(temp<MinimumDistance)
	  MinimumDistance= temp;
	}
	return MinimumDistance;
}


//-----------------------------------------------------------------------------
float DistancePointToLine(double P[3], double A[3], double B[3])
{
	float distance=-1;
	//calculate the scalar product AP scalar AB
   double AB[3],BA[3],BP[3], AP[3],P1[3];
   float AB_distance= sqrt( vtkMath::Distance2BetweenPoints( A, B ) ); 
   for (int i=0; i<3; ++i ){
      AB[i] = B[i] - A[i];
	  BA[i] = A[i] - B[i];
      AP[i] = P[i] - A[i];
	  BP[i] = P[i] - B[i];
   }
   double pscalar = AB[0]*AP[0] + AB[1]*AP[1] + AB[2]*AP[2] ;
   float alpha= pscalar/(AB_distance*AB_distance);
   double pscalar1 = BA[0]*BP[0] + BA[1]*BP[1] + BA[2]*BP[2] ;


   if(pscalar < 0)
   {
	    distance =sqrt( vtkMath::Distance2BetweenPoints( P, A ) );
   }
   else if(pscalar1 < 0 )
   {
	    distance =sqrt( vtkMath::Distance2BetweenPoints( P, B ) );
   }
   else
   {
	    //find the coordonnate of the orthogonal projected point
	    P1[0]= (1-alpha)* A[0] +  alpha* B[0];
		P1[1]= (1-alpha)* A[1] +  alpha* B[1];
		P1[2]= (1-alpha)* A[2] +  alpha* B[2];

	    distance =sqrt( vtkMath::Distance2BetweenPoints( P, P1 ) );
   }

   return distance;
}
//---------------------------------------------------------------------------------------
float DistancePointToHole(double point[3], vtkIdType HoleId,vtkPolyData* holes)
{
	double xyz[3];
	float distance=-100;
	double segment_PointA[3];
	double segment_PointB[3];
	int cpt=0;	
	float tempdist;

	vtkIdList* pts = holes->GetCell(HoleId)->GetPointIds();

	int numberOfPoints =  holes->GetCell(HoleId)->GetNumberOfPoints();// holes->GetNumberOfPoints();

	holes->GetPoint( pts->GetId(0), xyz );//first point    
	segment_PointA[0]= xyz[0];
	segment_PointA[1]= xyz[1];
	segment_PointA[2]= xyz[2];	

	for(int currentPoint = 1; currentPoint < numberOfPoints; currentPoint++) 																			
	{
		holes->GetPoint( pts->GetId(currentPoint), xyz );//next point		
		segment_PointB[0]= xyz[0];
		segment_PointB[1]= xyz[1];
		segment_PointB[2]= xyz[2];
		
		tempdist=DistancePointToLine(point, segment_PointA, segment_PointB);		
		if( (cpt==0)|| ((cpt>0) && (tempdist< distance)) )
		distance= tempdist;
		
		segment_PointA[0]= xyz[0];
		segment_PointA[1]= xyz[1];
		segment_PointA[2]= xyz[2];	
		cpt++;
	}
	
	return distance;
}


vtkUnstructuredGrid* vtkDistanceCellsToDrillhole::ComputeDistanceToHoles(vtkUnstructuredGrid* dataset,
																		 vtkPolyData* in)
{
	 this->ComputeCenters(dataset);

	vtkUnstructuredGrid* outDataset= vtkUnstructuredGrid::New();
	outDataset->ShallowCopy(dataset);
	int nbCellIn= in->GetNumberOfCells();
	int nbCellDataset= dataset->GetNumberOfCells();


	double distance=-1;
	double temp=0;
	double center[3];
	int hole=-1;

	if(this->ComputeDistanceTo==0)//we need to compute the distance for each center to the closest drillhole
	{
		cout<<"tototpttptop"<<endl;
		vtkIdTypeArray* holeIdArray= vtkIdTypeArray::New();
		holeIdArray->Allocate(nbCellDataset);
		vtkDoubleArray* distanceArray= vtkDoubleArray::New();
		distanceArray->Allocate(nbCellDataset);
		holeIdArray->SetName("Id");
		distanceArray->SetName("Distance");
		
		for(int idc=0; idc<nbCellDataset; idc++)
		{
			hole=-1;
			this->centers->GetPoint(idc, center);
			distance= DistancePointToHole(center, 0, in);
			hole= 0;

			for(int holeId=1; holeId<nbCellIn; holeId++)
			{
			  temp= DistancePointToHole(center, holeId, in);
			  if(temp<distance)
			  {
			 	hole= holeId;
				distance= temp;
			  }
			}
		
			holeIdArray->InsertTuple1(idc, hole);
			distanceArray->InsertTuple1(idc, distance);
		}
		outDataset->GetCellData()->AddArray(holeIdArray);
		outDataset->GetCellData()->AddArray(distanceArray);

	}
	else if(this->ComputeDistanceTo==1)
	{
		vtkIdTypeArray** holeIdArray1= new vtkIdTypeArray*[nbCellDataset];
	    vtkDoubleArray** distanceArray1= new vtkDoubleArray*[nbCellDataset];	
		for(int i=0; i<nbCellDataset; i++)
		{
		  holeIdArray1[i]= vtkIdTypeArray::New();
		  holeIdArray1[i]->Allocate(this->NbClosestDrillholes);
		  distanceArray1[i]= vtkDoubleArray::New();
		  distanceArray1[i]->Allocate(this->NbClosestDrillholes);
		}

		for(int idc=0; idc<nbCellDataset; idc++)
		{
			this->centers->GetPoint(idc, center);
			vtkstd::map < float, int > distanceArrayMap;

			for(int holeId=0; holeId<nbCellIn; holeId++)
			{
			  distance= DistancePointToHole(center, holeId, in);
			  distanceArrayMap[distance]= holeId;
			}
				
			vtkstd::map<float, int>::iterator it = distanceArrayMap.begin();
			for(int i=0; i<this->NbClosestDrillholes; i++)
			{
			   holeIdArray1[idc]->InsertTuple1(i,it->second);
			   distanceArray1[idc]->InsertTuple1(i,it->first);
			   it++;
			}			
		}

		for(int j=0; j< this->NbClosestDrillholes; j++)
		{
			vtkstd::stringstream textStream;
			textStream << j;
			vtkStdString text;
			textStream >>text;

			vtkDoubleArray* distanceArray= vtkDoubleArray::New();
			distanceArray->Allocate(nbCellDataset);
			distanceArray->SetName( ((vtkStdString("Distance "))+text).c_str());

			for(int idc=0; idc<nbCellDataset; idc++)
				distanceArray->InsertTuple1(idc,distanceArray1[idc]->GetTuple1(j));
			outDataset->GetCellData()->AddArray(distanceArray);
			distanceArray->Delete();
		}		
	}	
	else if(this->ComputeDistanceTo==2)
	{
	  this->distanceArray_Proc= vtkDoubleArray::New();
	  this->distanceArray_Proc->Allocate(nbCellDataset*(nbCellIn+1));
	  for(int idc=0; idc<nbCellDataset; idc++)
	  {
		this->distanceArray_Proc->InsertNextValue(idc);	
		this->centers->GetPoint(idc, center);
		for(int holeId=0; holeId<nbCellIn; holeId++)
		{
		  distance= DistancePointToHole(center, holeId, in);
		  this->distanceArray_Proc->InsertNextValue(distance);			  
		}		
	  }		
	}	

	return outDataset;
}

vtkDoubleArray*  vtkDistanceCellsToDrillhole::ProcessCalcul()
{
	return this->distanceArray_Proc;
}


//--------------------------------------------------------------------------
int vtkDistanceCellsToDrillhole::RequestData( vtkInformation *request, 
                                         vtkInformationVector **InputVector, 
                                         vtkInformationVector *outputVector )
{
   vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
   vtkInformation *sourceInfo = InputVector[1]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

    vtkUnstructuredGrid* input = vtkUnstructuredGrid::SafeDownCast (
                         inputInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

   vtkPolyData* source = vtkPolyData::SafeDownCast (
                         sourceInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

   vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast (
                        outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
   if ( !source ){
      return 0;
   }
  
   output->ShallowCopy(this->ComputeDistanceToHoles(input, source));
  	
   return 1;

}


