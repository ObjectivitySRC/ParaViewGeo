/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSequenceArrows.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSequenceArrows.h"

#include "vtkAppendPolyData.h"
#include "vtkConeSource.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"
#include "vtkCollection.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkCylinderSource.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkMath.h"

#include <math.h>

vtkCxxRevisionMacro(vtkSequenceArrows, "$Revision: 1.7 $");
vtkStandardNewMacro(vtkSequenceArrows);

vtkSequenceArrows::vtkSequenceArrows()
{
  this->SelectedComponent = 0;
  this->Size = 1;  
  pointOrder = vtkIntArray::New();
}

vtkSequenceArrows::~vtkSequenceArrows()
{
	pointOrder->Delete();
}

int vtkSequenceArrows::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );

  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  if ( ! this->GetInputArrayToProcess(0, inputVector) )
    {
    vtkErrorMacro(<<"No scalar data to Map");
    return 1;
    }

  int piece = output->GetUpdatePiece();
  int numPieces = output->GetUpdateNumberOfPieces();
  int ghostLevel = output->GetUpdateGhostLevel();

  //variables needed to figure out the angle of the line
  double* point0 = new double[3];
  double* point1 = new double[3];  

  //need a storage spot for all the cones
  vtkAppendPolyData* append = vtkAppendPolyData::New();
  
  vtkFloatArray *pointProps = vtkFloatArray::New();
  pointProps->SetName(this->GetInputArrayToProcess(0, inputVector)->GetName());
  
  //add sorting algorithm here
	this->SortOrder(input->GetVerts(), inputVector);
	
	double currentId, nextId;
	
	double currentProp;

  for (vtkIdType i = 0; i < this->pointOrder->GetNumberOfTuples()-1; i++)
    {    
    currentId = this->pointOrder->GetTuple1(i);
    nextId = this->pointOrder->GetTuple1(i+1);
    input->GetPoint(currentId, point0);
    input->GetPoint(nextId,point1);

    
    int size = this->AddArrow( append, point0, point1 );
    
    //we have to add the property for the Arrow
    currentProp = this->GetInputArrayToProcess(0, inputVector)->GetTuple1(currentId);
    for (int i=0; i < size; i++)
      {        
      pointProps->InsertNextTuple1(currentProp);
      }
    }

  if (output->GetUpdatePiece() == 0 && numPieces > 0)
    {
    append->Update();
    output->ShallowCopy(append->GetOutput());
    }
    
	output->GetPointData()->AddArray(pointProps);
  output->SetUpdatePiece(piece);
  output->SetUpdateNumberOfPieces(numPieces);
  output->SetUpdateGhostLevel(ghostLevel);
  append->Delete();

  delete [] point0;
  delete [] point1;  
  return 1;
}

//----------------------------------------------------------------------------
int vtkSequenceArrows::AddArrow( vtkAppendPolyData* &append, double* point1, double* point2)
{
  double numberOfCells = 0;
  double height = 0;
  double* direction = new double[3];
  double *center = new double[3];
  double radius = this->Size / 2;
  
  //calculate out some stuff
  height = sqrt(vtkMath::Distance2BetweenPoints(point1,point2));
  for (int i=0; i < 3; i++)
    {
    direction[i] = point2[i]-point1[i];  
    center[i] = (point2[i]+point1[i])/2.0;
    }    
   
  //set up the transform for the cylinder
  double rotation[16];
  this->CreateMatrix( rotation, direction, center );
  vtkTransform *transform = vtkTransform::New();  
  transform->SetMatrix( rotation );  
  
  //cylinder that is the base of the 
  vtkCylinderSource *cylinder = vtkCylinderSource::New();    
  cylinder->SetRadius( radius/3 ); //our cylinder is always have a thick as the cone 
  cylinder->SetHeight( height  );
  cylinder->SetCenter( 0 , 0, 0 );
  cylinder->Update();
  
  numberOfCells += ( cylinder->GetOutput()->GetNumberOfPoints() );
  
  //move the cylinder
  vtkTransformFilter *tf = vtkTransformFilter::New();
  tf->SetTransform( transform );
  tf->SetInput( cylinder->GetOutput() );  
  tf->Update();  
        
  
  
  //here is the cone at the base of the line
  vtkConeSource *startCone = vtkConeSource::New();
  startCone->SetHeight( this->Size );
  startCone->SetRadius( radius );  
  startCone->SetCenter( point1 );
  startCone->SetDirection( direction );
  startCone->Update(); 
  
  numberOfCells += ( startCone->GetOutput()->GetNumberOfPoints() );
  
  //here is the cone that composes the middle section of the line
  vtkConeSource *midCone = vtkConeSource::New();
  midCone->SetHeight(this->Size);
  midCone->SetRadius( radius );  
  midCone->SetCenter( center );
  midCone->SetDirection( direction );
  midCone->Update();  
  
  numberOfCells += ( midCone->GetOutput()->GetNumberOfPoints() );
    
  append->AddInput( (vtkPolyData*) tf->GetOutput() );
  append->AddInput( startCone->GetOutput() );
  append->AddInput( midCone->GetOutput() );
        
    
  //clean up the memory we have used
  tf->Delete();
  transform->Delete();
  cylinder->Delete();
  midCone->Delete();
  startCone->Delete();
  
  delete[] direction;
  delete[] center;
  return numberOfCells;
}


//----------------------------------------------------------------------------
void vtkSequenceArrows::SortOrder(vtkCellArray *verts, vtkInformationVector **inputVector)
{
  
	this->pointOrder->Reset();
	verts->InitTraversal();
	vtkIdType npts, *pts;
	for(int i = 0; i < verts->GetNumberOfCells(); i++)
	{
		verts->GetNextCell(npts, pts);
		for(int j = 0; j < npts; j++)
			this->pointOrder->InsertNextTuple1(pts[j]);
	}
	vtkDataArray *inScalars = this->GetInputArrayToProcess(0, inputVector);
	double minVal;
	int minIndex;
	int ipointId, jpointId;
	int temp;
	for(int i = 0; i < this->pointOrder->GetNumberOfTuples()-1; i++)
	{
		//verts->GetCell(pointOrder->GetTuple1(i), nIpts, ipts);
		ipointId = pointOrder->GetTuple1(i);
		minVal = inScalars->GetComponent(ipointId, this->SelectedComponent);
		minIndex = i;
		for(int j = i+1; j < this->pointOrder->GetNumberOfTuples(); j++)
		{
			//verts->GetCell(pointOrder->GetTuple1(j), nMinIdPts, minIdPts);
			jpointId = this->pointOrder->GetTuple1(j);
			if(inScalars->GetComponent(jpointId, this->SelectedComponent) < minVal)
			{
				minIndex = j;
				minVal = inScalars->GetComponent(jpointId, this->SelectedComponent);
			}
		}
		if(minIndex > i)
		{
			//swap in order list
			temp = this->pointOrder->GetTuple1(i);
			this->pointOrder->SetTuple1(i, this->pointOrder->GetTuple1(minIndex));
			this->pointOrder->SetTuple1(minIndex, temp);
		}
	}
}

//----------------------------------------------------------------------------
void vtkSequenceArrows::CreateMatrix( double rotation[16], double *direction, double *center )
{
  
  //storage for cross products  
  double norm[3]={0,1,0};
  double firstVector[3];
  double secondVector[3];
  double thirdVector[3];

  //copy the point, so that we do not destroy it
  firstVector[0] = direction[0];
  firstVector[1] = direction[1];
  firstVector[2] = direction[2];
  vtkMath::Normalize( firstVector );
      
  //have to find the other 2 vectors, to create a proper transform matrix
  vtkMath::Cross( firstVector, norm, secondVector );
  vtkMath::Cross( firstVector, secondVector, thirdVector );
   
  //rotate and centre according to normalized axes and centre point
  //column 1
  rotation[0] = secondVector[0];
  rotation[1] = firstVector[0];
  rotation[2] = thirdVector[0];
  rotation[3] = center[0];

  //column 2
  rotation[4] = secondVector[1];
  rotation[5] = firstVector[1];
  rotation[6] = thirdVector[1];
  rotation[7] = center[1];

  //column 3
  rotation[8] = secondVector[2];
  rotation[9] = firstVector[2];
  rotation[10] = thirdVector[2];
  rotation[11] = center[2];

  //column 4
  rotation[12] = 0.0;
  rotation[13] = 0.0;
  rotation[14] = 0.0;
  rotation[15] = 1.0;    

}

//----------------------------------------------------------------------------
int vtkSequenceArrows::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

//----------------------------------------------------------------------------
void vtkSequenceArrows::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Selected Component: " << this->SelectedComponent << endl;
  os << indent << "Size: " << this->Size << endl;
  this->Superclass::PrintSelf(os,indent);
}
